---
type: case-study
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System Design Interview - An Insider's Guide, Alex Xu"
  - "The Architecture of Twitter, Raffi Krikorian (QCon)"
  - "Facebook Engineering: Serving Cellular News Feed (TAO & Multifeed)"
---

# Design a Social Media Feed (Twitter / Instagram)

## 1. TL;DR

A social media feed system ingests user-generated posts and delivers personalized, chronological, or algorithmic timelines to hundreds of millions of users in real time.
At Twitter or Instagram scale, the platform supports over 300 million Daily Active Users (DAU) publishing 500 million posts daily while serving over 1.5 billion feed retrievals daily, peaking above 50,000 queries per second (QPS).
The fundamental system design challenge is the tension between write amplification and read latency, formalized as **Fan-Out on Write (Push)** versus **Fan-Out on Read (Pull)**.
A pure push model delivers sub-10ms read times by pre-computing timelines into in-memory caches, but collapses under write amplification when a celebrity with 50+ million followers posts.
A pure pull model eliminates write amplification but incurs unacceptably high multi-second database read and merge latency.
The production-grade solution is a **Hybrid Fan-Out Architecture**: standard users execute fan-out on write to followers' Redis timeline caches, while high-follower "celebrity" accounts bypass write fan-out and are dynamically merged on the fly during the user's feed retrieval request.

---

## 2. Mental Model

The hybrid architecture isolates standard fan-out workers from celebrity bypass paths, merging candidate streams in a high-performance feed service.

```mermaid
flowchart TD
    subgraph PostIngestion["Post Ingestion & Routing"]
        Author["Author (User A)"] -->|POST /v1/posts| IngestGW["API Gateway / LB"]
        IngestGW --> PostService["Post Creation Service"]
        PostService --> PostStore[(Post Metadata Store: DynamoDB / Cassandra)]
        PostService --> MediaStore[(Media Storage: Amazon S3 + CDN)]
        PostService --> FollowerDB[(Social Graph: Neo4j / MySQL Shards)]
        PostService --> KafkaIngest[Kafka: post-created-topic]
    end

    subgraph FanOutSubsystem["Fan-Out Processing Engine"]
        KafkaIngest --> FanOutWorker["Fan-Out Worker Fleet"]
        FanOutWorker --> CheckCelebrity{"Author Followers > 20,000?"}
        CheckCelebrity -->|Yes (Celebrity)| Bypass["Skip Fan-Out Write (Write only to Author Outbox)"]
        CheckCelebrity -->|No (Standard)| PushTimeline["Push Post ID to All Followers' Timelines"]
        PushTimeline --> RedisTimelines[(Redis Timeline Cache: ZSET per user)]
    end

    subgraph FeedRetrieval["Feed Retrieval & Merging"]
        Consumer["Reader (User B)"] -->|GET /v1/feed| ReadGW["Edge Gateway"]
        ReadGW --> FeedService["Feed Aggregator Service"]
        FeedService -->|1. Fetch Pre-computed IDs| RedisTimelines
        FeedService -->|2. Fetch Followed Celebrities| FollowerDB
        FeedService -->|3. Fetch Recent Celebrity Posts| PostStore
        FeedService --> MergeRank["K-Way Merge & Ranking Model"]
        MergeRank --> Hydrate["Hydrate Post Content & Author Profiles"]
        Hydrate --> PostStore
        Hydrate --> Consumer
    end
```

---

## 3. Architectural Internals and Deep Dive

### 3.1 Fan-Out Models: Push vs. Pull vs. Hybrid

#### 1. Fan-Out on Write (Push Model)
- **Workflow**: When User A publishes a post, a background worker queries the social graph for all followers of User A.
The worker appends the new `post_id` directly into the in-memory timeline queue (Redis `ZSET`) of every single follower.
- **Read Path**: Extremely fast ($O(1)$ lookup).
When a user opens their feed, the system simply reads the pre-computed post IDs from their Redis timeline cache.
- **Failure Mode (The Celebrity Problem)**: If an account with 80 million followers posts, the worker must execute 80 million cache insertions.
This causes massive queue backlog, high CPU spikes, and severe delivery delays for all other platform users.

#### 2. Fan-Out on Read (Pull Model)
- **Workflow**: When User A posts, the record is saved only to User A's outbox.
No background fan-out occurs.
- **Read Path**: When User B opens their feed, the feed service queries the social graph for all users User B follows (e.g., 500 accounts), fetches the recent posts for all 500 accounts from storage, performs a multi-way merge sort in memory, and returns the top 20 items.
- **Failure Mode**: Read operations are exceptionally slow ($O(K \times \log N)$ where $K$ is followed accounts), requiring massive database I/O that makes sub-100ms response SLAs impossible.

#### 3. The Hybrid Fan-Out Architecture (Recommended)
The platform establishes a follower threshold (e.g., 20,000 followers):
- **Standard Accounts ($\le 20,000$ followers)**: Use **Fan-Out on Write**.
New posts are pushed immediately into followers' Redis timeline caches.
- **Celebrity Accounts ($> 20,000$ followers)**: Use **Fan-Out on Read**.
New posts are appended only to the celebrity's outbox.
- **Feed Generation Phase**: When a user requests their timeline:
  1. Fetch the user's pre-computed timeline from Redis (containing posts from standard accounts).
  2. Inspect the user's following list to identify which followed accounts are celebrities.
  3. Fetch the recent posts published by those specific celebrities from cache/database.
  4. Perform an in-memory K-way merge sort combining the pre-computed timeline with the celebrity posts.
  5. Paginate and return the top results.

```
+---------------------------------------------------------------+
|             Hybrid Feed Aggregation Architecture              |
+---------------------------------------------------------------+
| User Timeline Cache (Redis ZSET)                              |
| [Post_1 (ts: 100), Post_2 (ts: 85), Post_3 (ts: 60)]          |
|                             +                                 |
| Followed Celebrity Outbox (e.g., @elonmusk)                   |
| [Post_Celeb (ts: 92), Post_Celeb2 (ts: 40)]                   |
|                             |                                 |
|                             v (In-Memory K-Way Merge Sort)    |
| Combined Feed Output:                                         |
| 1. Post_1     (ts: 100)                                       |
| 2. Post_Celeb (ts: 92)                                        |
| 3. Post_2     (ts: 85)                                        |
| 4. Post_3     (ts: 60)                                        |
+---------------------------------------------------------------+
```

### 3.2 Timeline Storage and Redis Caching

The pre-computed timeline does not store full post objects (text, images, author names).
Storing full objects would duplicate millions of copies of identical text and metadata across memory.
Instead, the timeline cache stores only **64-bit Snowflake Post IDs**:
- **Data Structure**: Redis Sorted Set (`ZSET`).
  - **Key**: `timeline:<user_id>`
  - **Score**: Millisecond timestamp (or the Snowflake `post_id` itself, which is inherently time-sortable).
  - **Member**: `post_id`
- **Bounded Retention**: Users rarely scroll beyond 800 historical posts.
The system limits each user's `ZSET` to 800 items using `ZREMRANGEBYRANK timeline:<user_id> 0 -801`.
Older posts are evicted from RAM and retrieved from persistent storage only if the user scrolls deeply.

### 3.3 Pagination: Why Cursor-Based Pagination is Mandatory

Standard SQL `OFFSET` pagination fails in dynamic feeds:
`SELECT post_id FROM posts ORDER BY created_at DESC LIMIT 20 OFFSET 40;`
- **Offset Shift Flaw**: If 5 new posts are published while the user is reading page 1, querying page 2 with `OFFSET 20` shifts results, returning the bottom 5 posts of page 1 again (duplicate posts displayed to user).
- **Performance Flaw**: `OFFSET 10000` forces the database engine to traverse 10,000 records before returning the requested 20.

**Cursor-Based Pagination (Selected Standard)**:
Clients pass a cursor token representing the oldest `post_id` received on the previous page:
`GET /v1/feed?limit=20&cursor=1775529000123`
In Redis, this maps directly to an efficient range query:
`ZREVRANGEBYSCORE timeline:<user_id> (cursor_id -inf LIMIT 0 20`
This completely eliminates duplicate items, skips zero records, and runs in $O(\log N + M)$ time.

### 3.4 Feed Ranking and Algorithmic Scoring

Modern social platforms do not present purely chronological feeds; they rank candidate items based on engagement probability:
1. **Candidate Retrieval**: The feed service pulls ~800 candidate post IDs from the user's hybrid timeline.
2. **Feature Extraction**: Retrieve author engagement history, user interaction history (likes, profile visits, shares), media type (video vs image), and post freshness.
3. **Scoring Model**: A lightweight Machine Learning model (e.g., Gradient Boosted Decision Trees or deep ranking networks) predicts click-through rate ($P(\text{click})$), like probability ($P(\text{like})$), and dwell time:
   $$\text{Score} = w_1 P(\text{like}) + w_2 P(\text{share}) + w_3 P(\text{comment}) - w_4 P(\text{skip}) - \text{Decay}(\Delta t)$$
4. **Diversity and Deduplication Filter**: Prevents showing 5 consecutive posts from the same author or multiple identical video memes.
5. **Hydration Phase**: Hydrate the top 20 ranked post IDs with text, URLs, and author metadata via a batched `MGET` against Redis or DynamoDB.

---

## 4. Trade-offs and Comparisons

| Dimension | Push Model (Fan-Out on Write) | Pull Model (Fan-Out on Read) | Hybrid Model (Push + Pull) |
|---|---|---|---|
| Read Latency | Ultra-fast (< 10ms from Redis) | Slow (200ms - 2s multi-query merge) | Fast (< 30ms) |
| Write Latency | Slow / Spiky on celebrity posts | Instant ($O(1)$ single write) | Instant for celebrities, fast for standard |
| Storage Overhead | High (stores post ID per follower) | Minimal (stores 1 copy of post) | Moderate (bounded 800 items per user) |
| Celebrity Handling | Severe bottleneck (crashes workers) | Natural (no extra work on post) | Optimal (bypasses write fan-out) |
| Architectural Complexity | Low | Low | Moderate to High |

---

## 5. Failure Modes and Mitigations

### 5.1 Celebrity Tweet Queue Jamming
- **Failure Mode**: An account with 80M followers posts.
If misclassified as a standard user, the fan-out queue becomes jammed with 80 million write tasks, delaying standard post delivery for hours.
- **Mitigation**: Maintain a real-time set of high-follower account IDs in Redis.
Before enqueuing a fan-out task, the post service inspects the author's follower count; if $> 20,000$, it routes the task to a no-op bypass handler.

### 5.2 Cache Cold-Start for Inactive Users
- **Failure Mode**: Inactive users who have not opened the app in 60 days consume gigabytes of expensive RAM if their timelines are continuously updated via fan-out.
- **Mitigation**: Inactive user eviction.
If a user has not logged in for 14 days, remove their timeline from Redis and flag them as inactive in the session store.
Fan-out workers skip writing to inactive users.
When an inactive user re-authenticates, a background job rebuilds their timeline on demand from persistent storage.

### 5.3 Feed Aggregator Timeout Degradation
- **Failure Mode**: Downstream ML scoring service experiences latency spikes (> 500ms), threatening feed loading timeouts.
- **Mitigation**: Deploy strict fallback degradations.
If the ML ranking service fails to respond within 50ms, the feed service drops the algorithmic ranking tier and falls back immediately to returning reverse-chronological order from the Redis timeline cache.

---

## 6. Hands-On Verification

The following standalone Python implementation demonstrates the Hybrid Fan-out architecture, Redis-style ZSET timeline caching, celebrity dynamic merging, and cursor-based pagination.

```python
#!/usr/bin/env python3
"""
Production-grade demonstration of Hybrid Social Media Feed Architecture:
- Standard user fan-out on write
- Celebrity bypass and fan-out on read
- In-memory K-way merge sort
- Cursor-based pagination
"""

import time
import heapq
from typing import Dict, List, Set, Tuple, Optional

CELEBRITY_FOLLOWER_THRESHOLD = 5


class Post:
    def __init__(self, post_id: int, author_id: str, content: str, created_at: float):
        self.post_id = post_id
        self.author_id = author_id
        self.content = content
        self.created_at = created_at

    def __lt__(self, other: "Post") -> bool:
        # Reverse ordering for max-heap / descending sort
        return self.created_at > other.created_at


class SocialGraphService:
    def __init__(self):
        # author_id -> set of follower_ids
        self.followers: Dict[str, Set[str]] = {}
        # user_id -> set of followed author_ids
        self.following: Dict[str, Set[str]] = {}

    def follow(self, follower: str, followee: str):
        self.followers.setdefault(followee, set()).add(follower)
        self.following.setdefault(follower, set()).add(followee)

    def get_followers(self, user_id: str) -> Set[str]:
        return self.followers.get(user_id, set())

    def get_following(self, user_id: str) -> Set[str]:
        return self.following.get(user_id, set())

    def is_celebrity(self, user_id: str) -> bool:
        return len(self.get_followers(user_id)) >= CELEBRITY_FOLLOWER_THRESHOLD


class HybridFeedService:
    def __init__(self, graph: SocialGraphService):
        self.graph = graph
        self.posts_db: Dict[int, Post] = {}
        # Simulated Redis Timelines: user_id -> list of (created_at, post_id) sorted DESC
        self.user_timelines: Dict[str, List[Tuple[float, int]]] = {}
        # Celebrity outboxes: author_id -> list of Post sorted DESC
        self.celebrity_outboxes: Dict[str, List[Post]] = {}
        self._post_id_counter = 1000

    def create_post(self, author_id: str, content: str) -> Post:
        self._post_id_counter += 1
        post = Post(self._post_id_counter, author_id, content, time.time())
        self.posts_db[post.post_id] = post

        # Check if author is a celebrity
        if self.graph.is_celebrity(author_id):
            print(f"[Write Path] Author '{author_id}' is a CELEBRITY. Appending to outbox only.")
            self.celebrity_outboxes.setdefault(author_id, []).insert(0, post)
        else:
            print(f"[Write Path] Author '{author_id}' is STANDARD. Fan-out on write to followers.")
            followers = self.graph.get_followers(author_id)
            for f in followers:
                timeline = self.user_timelines.setdefault(f, [])
                timeline.insert(0, (post.created_at, post.post_id))
                # Bound cache size to 800 items
                if len(timeline) > 800:
                    timeline.pop()
        return post

    def get_feed(self, user_id: str, limit: int = 10, cursor_post_id: Optional[int] = None) -> Tuple[List[Post], Optional[int]]:
        candidate_posts: List[Post] = []

        # 1. Fetch pre-computed timeline from Redis
        cached_timeline = self.user_timelines.get(user_id, [])
        for _, pid in cached_timeline:
            candidate_posts.append(self.posts_db[pid])

        # 2. Fetch recent posts from followed celebrities (Fan-out on read)
        followed_users = self.graph.get_following(user_id)
        for followed in followed_users:
            if self.graph.is_celebrity(followed):
                celeb_posts = self.celebrity_outboxes.get(followed, [])
                candidate_posts.extend(celeb_posts[:20])  # Fetch recent 20

        # 3. In-memory deduplication and sort by created_at DESC
        unique_posts = {p.post_id: p for p in candidate_posts}.values()
        sorted_posts = sorted(unique_posts, key=lambda x: x.created_at, reverse=True)

        # 4. Apply cursor-based pagination
        if cursor_post_id:
            cursor_index = -1
            for i, p in enumerate(sorted_posts):
                if p.post_id == cursor_post_id:
                    cursor_index = i
                    break
            if cursor_index != -1:
                paginated = sorted_posts[cursor_index + 1 : cursor_index + 1 + limit]
            else:
                paginated = sorted_posts[:limit]
        else:
            paginated = sorted_posts[:limit]

        next_cursor = paginated[-1].post_id if paginated else None
        return paginated, next_cursor


if __name__ == "__main__":
    graph = SocialGraphService()
    feed_service = HybridFeedService(graph)

    # Setup Social Graph: 'celebrity_vip' followed by 5 users (reaches threshold)
    for i in range(5):
        graph.follow(f"user_{i}", "celebrity_vip")

    # 'alice' followed only by 'bob'
    graph.follow("bob", "alice")
    graph.follow("bob", "celebrity_vip")

    print("--- 1. Publishing Standard Post (Alice) ---")
    feed_service.create_post("alice", "Hello from Alice! Standard post.")

    print("\n--- 2. Publishing Celebrity Post (Celebrity VIP) ---")
    feed_service.create_post("celebrity_vip", "Breaking Announcement from VIP!")

    print("\n--- 3. Bob Retrieves Feed (Should merge Alice and Celebrity VIP) ---")
    bob_feed, next_cursor = feed_service.get_feed("bob", limit=5)
    for idx, p in enumerate(bob_feed):
        print(f"  Item {idx + 1}: [{p.author_id}] {p.content} (ID: {p.post_id})")

    assert len(bob_feed) == 2, "Bob's feed should contain exactly 2 posts!"
    print("\nVerification Passed: Hybrid feed successfully merged standard and celebrity posts!")
```

### CLI Verification

Execute feed retrieval tests across platforms:

```bash
# Linux / macOS: Retrieve feed with cursor pagination via curl
curl -X GET "https://api.example.com/v1/feed?limit=20&cursor=1775529000456" \
  -H "Authorization: Bearer test_auth_token"

# Windows PowerShell: Test Feed API response
$feed = Invoke-RestMethod -Uri "https://api.example.com/v1/feed?limit=10" -Headers @{"Authorization"="Bearer token"}
$feed.posts | Format-Table -Property id, author_id, created_at
```

---

## 7. Performance Characteristics and Capacity Planning

### 7.1 Throughput Calculations
- **Active User Base**: 300 million Daily Active Users (DAU).
- **Post Ingestion Rate**:
  - 500 million posts/day.
  - Average Write QPS = $\frac{500,000,000}{86,400} \approx 5,787 \text{ writes/sec}$.
  - Peak Write QPS ($3\times$ factor) $\approx 17,360 \text{ writes/sec}$.
- **Feed Read Throughput**:
  - Assume each user opens their feed 5 times per day.
  - Total feed queries = $300,000,000 \times 5 = 1.5 \text{ billion feed requests/day}$.
  - Average Read QPS = $\frac{1,500,000,000}{86,400} \approx 17,360 \text{ reads/sec}$.
  - Peak Read QPS ($2.5\times$ factor) $\approx 43,400 \text{ reads/sec}$.

### 7.2 Cache Memory Sizing (Redis Timeline Tier)
- Each active user's timeline caches the top 800 post IDs.
- Data stored per post ID in Redis `ZSET`:
  - Member (`post_id` 64-bit integer): 8 bytes.
  - Score (Timestamp 64-bit float): 8 bytes.
  - Redis skip-list and dict node overhead: ~32 bytes.
  - Total per item $\approx 48 \text{ bytes}$.
- Memory per active user = $800 \text{ items} \times 48 \text{ bytes} \approx 38.4 \text{ KB}$.
- For 300 million DAU:
  - Total RAM required = $300,000,000 \times 38.4 \text{ KB} \approx 11.52 \text{ TB}$.
  - With a 25-shard Redis cluster running 512 GB RAM nodes with primary-replica replication, 11.5 TB fits comfortably in memory.

---

## 8. In Production: Real-World Architecture (Twitter & Facebook)

1. **Twitter Timeline Architecture**:
   - Twitter initially used a pure pull model (relational MySQL joins), which collapsed during major global events.
   - Migrated to **Timeline Service (Redis cluster codenamed "Trogdor")**, running a push model for normal users.
   - Designed a hybrid search-indexer engine ("Earlybird") to index tweets in real time and merge celebrity tweets into feeds during retrieval.
2. **Facebook News Feed (Multifeed & TAO)**:
   - Facebook built **TAO** (a distributed graph store caching objects and associations in MySQL and memcached) to represent the social graph.
   - The **Multifeed** engine maintains leaf aggregator trees that pull candidate stories from friends' outboxes and rank them dynamically using multi-tier Machine Learning pipelines before assembling the final feed.

---

## 9. Interview Questions and Deep Dives

> [!question] Question 1: What is the "Celebrity Problem" in social media feed architectures?
> [!success]- Answer
> The Celebrity Problem occurs under a pure Fan-Out on Write (Push) model when an account with millions of followers (e.g., 80M followers) publishes a post.
> The fan-out worker must duplicate that single post into 80 million individual follower timeline caches in Redis.
> This creates a massive write spike, saturating Redis network bandwidth, causing worker queue lag, and delaying feed updates for millions of other users.

> [!question] Question 2: Why is cursor-based pagination strictly required over offset-based pagination in live feeds?
> [!success]- Answer
> Live feeds are constantly receiving new incoming posts.
> With offset pagination (`OFFSET 20`), if 5 new posts are published while a user views page 1, requesting page 2 shifts all rows downwards by 5 positions.
> The user sees the last 5 posts of page 1 repeated at the top of page 2.
> Cursor pagination uses the `post_id` of the last seen item (`cursor=pid`).
> The query executes `WHERE post_id < cursor ORDER BY post_id DESC LIMIT 20`, guaranteeing that new posts inserted at the top do not alter the sequence of older posts below the cursor.

> [!question] Question 3: How does the Hybrid Fan-Out model solve both read latency and celebrity write amplification?
> [!success]- Answer
> The hybrid model partitions accounts based on follower thresholds (e.g., 20,000 followers).
> Normal accounts use fan-out on write: their posts are pushed directly to followers' Redis caches, ensuring $99\%$ of posts are pre-computed for ultra-fast reads.
> Celebrity accounts bypass write fan-out entirely: their posts are written only to their own outbox.
> When a user requests their feed, the feed service reads their pre-computed timeline and merges in the recent posts of only the specific celebrities that user follows.

> [!question] Question 4: Why should Redis timelines store only 64-bit `post_id` values rather than full post JSON documents?
> [!success]- Answer
> If a post is sent to 10,000 followers, storing full JSON documents (e.g., 1 KB per post) would store 10 MB of duplicate data across Redis.
> Storing only the 8-byte `post_id` consumes only 80 KB across the 10,000 followers.
> Full post content is stored once in a persistent store (DynamoDB or Cassandra) and cached in a shared Redis key (`post:<post_id>`), hydrated in a single batched `MGET` call for the final 20 displayable posts.

> [!question] Question 5: How do you handle inactive users who have not logged into the platform for several months?
> [!success]- Answer
> To avoid wasting expensive RAM on users who may never view their feeds, evict timelines of users who have been inactive for more than 14 days from Redis.
> Fan-out workers inspect an in-memory bitmap or cache of active user IDs; if a follower is inactive, the worker skips updating their timeline.
> When an inactive user logs back in, a background job reconstructs their timeline on demand from persistent storage.

> [!question] Question 6: How does the two-stage ranking architecture work in algorithmic feeds?
> [!success]- Answer
> Evaluating complex deep learning models on millions of candidate posts is too slow for sub-50ms latency.
> The system divides ranking into two stages:
> 1. **Candidate Retrieval (Coarse Filter)**: Rapidly pulls ~800 candidate posts from the user's hybrid timeline cache using lightweight heuristic filtering.
> 2. **Scoring & Ranking (Fine Filter)**: Passes the 800 candidates through a rich Machine Learning model that evaluates user interaction history, dwell time, and content embeddings to score and sort the top 20 items.

> [!question] Question 7: What happens if the machine learning feed ranking service fails or times out?
> [!success]- Answer
> The feed service wraps the ranking service in a circuit breaker with a tight 50ms timeout.
> If the ranking service times out or fails, the feed service gracefully degrades by returning the candidates sorted in strict reverse-chronological order directly from the Redis timeline cache.
> Users experience a functional feed rather than an error screen or infinite loading spinner.

> [!question] Question 8: How do you ensure users do not see the same posts repeatedly when refreshing their feed?
> [!success]- Answer
> Maintain a compact seen-state cache for each user in Redis using a **Bloom filter** or a rolling fixed-size ring buffer of seen post IDs (`seen:<user_id>`).
> When candidate posts are retrieved, the feed service checks candidate IDs against the user's seen filter and drops already-viewed items before presenting the final page.

> [!question] Question 9: How does the system handle media assets (images, videos) attached to posts?
> [!success]- Answer
> Clients upload media directly to Amazon S3 via pre-signed URLs, bypassing application web servers to conserve server bandwidth.
> An asynchronous processing pipeline (AWS Lambda or Kafka worker fleet) compresses, resizes, and transcodes the media into multiple resolutions (e.g., HLS streams for video, WebP for images).
> Assets are distributed globally through a Content Delivery Network (CDN) edge cache.

> [!question] Question 10: How do you prevent split-brain issues in the social graph following relationship store?
> [!success]- Answer
> The social graph requires strict consistency for follow/unfollow operations to prevent orphaned follower pointers.
> Graph associations are sharded by `user_id` using consistent hashing and stored in a database supporting ACID transactions (e.g., PostgreSQL with row-level locks) or distributed consensus stores (e.g., CockroachDB).
> Cache invalidation for followers uses transactional outbox patterns to guarantee cache-database consistency.

---

## 10. Related Concepts and Wikilinks

- [[Redis-Architecture]]: Sorted sets (ZSET), TTL eviction, and memory optimization.
- [[Pub-Sub-Architecture]]: Asynchronous event-driven fan-out pipelines using Apache Kafka.
- [[Pagination-Strategies]]: Cursor-based vs offset-based pagination across high-velocity datasets.
- [[Consistent-Hashing]]: Sharding timeline caches across distributed Redis clusters.
- [[04-Distributed-ID-Generator/design|Distributed-ID-Generator]]: Generating monotonically increasing 64-bit IDs for chronological sorting.

---

## 11. Further Reading

- Xu, Alex. *System Design Interview – An Insider’s Guide (Volume 1)*. Chapter 11: Design a News Feed System.
- Krikorian, Raffi. *Timelines at Scale*. Twitter Engineering Presentation (QCon).
- Bronson, Nathan, et al. *TAO: Facebook’s Distributed Data Store for the Social Graph*. USENIX ATC 2013.
- Twitter Engineering Blog: *The Architecture Behind Twitter's Open Source Recommendation Algorithm*.

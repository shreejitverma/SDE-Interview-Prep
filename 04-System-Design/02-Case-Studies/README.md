---
type: moc
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
---

# System Design Case Studies

This module contains end-to-end, production-grade system design case studies demonstrating real-world distributed architectures at scale.
Each case study follows a standardized 12-section blueprint: TL;DR, Mental Model (Mermaid diagrams), Architectural Internals & Deep Dives, Trade-offs & Comparisons, Failure Modes & Mitigations, Hands-On Verification (runnable code without placeholders and multi-OS CLI commands), Performance Characteristics & Capacity Planning (exact traffic/storage/bandwidth math), In Production case studies, 10 folded active recall interview questions with detailed answers, Related Concepts, and Further Reading.

---

## Complete Case Study Catalog

| # | System Design Case Study | Primary Architecture & Key Challenges | Key Technologies & Patterns |
|---|---|---|---|
| 01 | [[01-URL-Shortener/design\|URL Shortener (TinyURL / Bitly)]] | Base62 encoding, Key Generation Service (KGS), HTTP 301 vs 302, cache stampede defense, 100:1 read-to-write ratio | KGS, Base62, Redis, DynamoDB, Kafka |
| 02 | [[02-Rate-Limiter/design\|Distributed Rate Limiter]] | Token bucket vs sliding window counter, Redis Lua scripts, multi-tier enforcement, fail-open vs fail-closed | Redis Lua, Sliding Window, Envoy, Cloudflare |
| 03 | [[03-Real-Time-Chat/design\|Real-Time Chat (WhatsApp / Discord / Slack)]] | Stateful WebSocket gateways, Redis session registry, pub/sub inter-gateway routing, Cassandra/ScyllaDB message clustering | WebSockets, ScyllaDB, Redis Pub/Sub, Snowflake |
| 04 | [[04-Distributed-ID-Generator/design\|Distributed Unique ID Generator (Twitter Snowflake)]] | 64-bit time-sortable IDs, clock skew & NTP backward jumps, sequence exhaustion, ZooKeeper worker leases | Snowflake, ZooKeeper, etcd, Bitwise Shifting |
| 05 | [[05-Social-Media-Feed/design\|Social Media Feed (Twitter / Instagram)]] | Hybrid Fan-Out (Push for normal users, Pull for celebrities), Redis ZSET timelines, cursor-based pagination, ML ranking | Redis ZSET, Hybrid Fan-Out, Kafka, GBDT Ranking |
| 06 | [[06-Ride-Sharing-Service/design\|Ride-Sharing Service (Uber / Lyft)]] | Real-time GPS ingestion (1.25M updates/sec), Uber H3 hexagonal indexing, batch bipartite dispatch matching, surge pricing | Uber H3, Google S2, Redis Geo, Hungarian Algorithm |
| 07 | [[07-Distributed-Web-Crawler/design\|Distributed Web Crawler (Googlebot)]] | Mercator URL Frontier (Priority + Politeness queues), asynchronous DNS cache, Bloom filter seen check, SimHash deduplication | Mercator Frontier, Bloom Filter, SimHash, robots.txt |
| 08 | [[08-Video-Streaming-Platform/design\|Video Streaming Platform (YouTube / Netflix)]] | Resumable multipart upload, parallel DAG transcoding (GOP splitting), Adaptive Bitrate Streaming (HLS/DASH), multi-tier CDN | HLS, MPEG-DASH, FFmpeg, CDN Edge, Kafka/Flink |
| 09 | [[09-Ticket-Booking-System/design\|Ticket Booking System (Ticketmaster / BookMyShow)]] | Zero double-booking, 10-minute temporary seat hold leases via Redis Lua, Virtual Waiting Room traffic leveling, Saga transactions | Redis Lua, Saga Pattern, PostgreSQL, Cloudflare Queue |
| 10 | [[10-E-Commerce-Flash-Sale/design\|E-Commerce Flash Sale (Amazon Prime Day / Alibaba 11.11)]] | Zero overselling under 200k QPS, stock pre-warming, stock sharding across Redis nodes, Kafka rate leveling to database | Stock Sharding, Redis Lua, Kafka, AliSQL / InnoDB |
| 11 | [[11-Distributed-Key-Value-Store/design\|Distributed Key-Value Store (Dynamo)]] | Consistent-hash placement, N/R/W quorums, sloppy quorum, vector-clock siblings, Merkle repair | Consistent hashing, quorum, hinted handoff |
| 12 | [[12-Typeahead-Search/design\|Typeahead and Search Suggestions]] | Precomputed top-K per prefix, separate from the BM25 inverted index, snapshot publish | In-memory trie, heavy hitters, edge cache |
| 13 | [[13-Notification-System/design\|Notification System]] | Transactional path isolated from marketing, inbox as source of truth, per-device idempotency | Outbox, provider senders, preferences |
| 14 | [[14-Collaborative-Editor/design\|Collaborative Document]] | OT against a server sequence, or a CRDT for offline editors, tombstones and presence | Op log, CRDT, session fan-out |
| 15 | [[15-Payment-Ledger/design\|Payment Ledger]] | Append-only double-entry, integer minor units, one writer per account, reconciler | Idempotency key, saga hold, outbox |
| 16 | [[16-Metrics-Platform/design\|Metrics Platform]] | Series cardinality, histogram buckets, Gorilla-style chunks, burn-rate queries | Pull and push, Kafka buffer, recording rules |
| 17 | [[17-File-Sync/design\|File Sync]] | Content-defined chunks, metadata compare-and-swap, block store, conflict copies | Rabin or FastCDC, content addressing |
| 18 | [[18-Distributed-Lock/design\|Distributed Lock and Leader Election]] | Lease plus fencing token, consensus lock service, row versions for data | ZooKeeper or etcd, reject stale tokens |

---

## Architectural Patterns Across Case Studies

### 1. Ingress & Traffic Shaping
- **Virtual Waiting Room**: Metering thousands of concurrent on-sale requests into a flat, predictable ingestion stream ([[09-Ticket-Booking-System/design\|Ticket Booking]]).
- **Multi-Tier Rate Limiting**: Enforcing IP-based rate limiting at the edge and authenticated API key quotas at the API gateway ([[02-Rate-Limiter/design\|Rate Limiter]]).

### 2. High-Concurrency Concurrency & Locking
- **In-Memory Atomic State Machine**: Executing atomic reservations with TTL leases in Redis Lua scripts to eliminate database row-lock contention ([[09-Ticket-Booking-System/design\|Ticket Booking]], [[10-E-Commerce-Flash-Sale/design\|Flash Sale]]).
- **Stock Sharding**: Partitioning single-key hot spots across multiple Redis cluster nodes to avoid single-core CPU saturation ([[10-E-Commerce-Flash-Sale/design\|Flash Sale]]).

### 3. Spatial & Time-Series Data Modeling
- **Hexagonal Spatial Discretization**: Indexing moving driver coordinates in Uber H3 hexagonal cells with invariant equidistant neighbors ([[06-Ride-Sharing-Service/design\|Ride Sharing]]).
- **LSM Clustered Time-Series**: Storing conversation logs partitioned by channel and clustered on disk by 64-bit Snowflake IDs in Cassandra ([[03-Real-Time-Chat/design\|Real-Time Chat]]).

### 4. Asynchronous Decoupling & Leveling
- **Kafka Buffer & Worker Leveling**: Absorbing high-velocity write bursts in message queues and draining to relational databases at a controlled rate ([[08-Video-Streaming-Platform/design\|Video Streaming]], [[10-E-Commerce-Flash-Sale/design\|Flash Sale]]).
- **The Saga Pattern**: Coordinating distributed multi-service checkout workflows with compensating transactions instead of blocking Two-Phase Commit ([[09-Ticket-Booking-System/design\|Ticket Booking]]).

---

## Related Curriculum Modules

- [[00-Concepts/README\|00-Concepts]]: Core distributed systems theory (CAP, ACID vs BASE, Consistent Hashing, Concurrency).
- [[04-APIs-and-Protocols/README\|04-APIs-and-Protocols]]: Network transport protocols, WebSockets, gRPC, REST, and pagination.
- [[05-Data-Stores/README\|05-Data-Stores]]: Relational, NoSQL, in-memory caches, search engines, and coordination stores.
- [[06-Messaging-and-Streaming/README\|06-Messaging-and-Streaming]]: Distributed event streaming and pub/sub message brokers.
- [[07-Infrastructure-and-Orchestration/README\|07-Infrastructure-and-Orchestration]]: Proxies, container runtimes, Kubernetes, and load balancers.
- [[08-Big-Data/README\|08-Big-Data]]: Distributed batch and DAG computing engines (Hadoop, Spark).

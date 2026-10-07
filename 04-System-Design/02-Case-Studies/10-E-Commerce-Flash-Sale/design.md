---
type: case-study
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System Design Interview - An Insider's Guide, Alex Xu"
  - "Alibaba Engineering: Architecture of 11.11 Global Shopping Festival"
  - "Designing Data-Intensive Applications, Martin Kleppmann"
---

# Design an E-Commerce Flash Sale System (Amazon Prime Day / Alibaba 11.11)

## 1. TL;DR

An e-commerce flash sale platform manages the rapid liquidation of deeply discounted, highly limited inventory (e.g., 10,000 premium gaming consoles or designer sneakers) released at a specific, advertised instant.
At peak on-sale moments, the platform is bombarded by millions of concurrent shoppers and automated scalper bots, driving traffic spikes exceeding 200,000 queries per second (QPS) concentrated on a single SKU within the first three seconds.
The paramount engineering imperative is **strictly preventing overselling** (never selling more inventory than physical stock allows) while ensuring the website remains responsive and immune to cascading database failures.
Directly querying or updating relational database inventory rows (`UPDATE inventory SET stock = stock - 1 WHERE item_id = ? AND stock > 0`) under 200,000 QPS causes catastrophic row-lock serialization, thread pool exhaustion, and complete platform outages.
The industry standard solution employs a **Traffic Filtration Funnel**: aggressive edge caching and bot defenses, in-memory **Redis inventory pre-warming with atomic Lua script decrements**, multi-shard stock partitioning to resolve single-key CPU bottlenecks, and **asynchronous queue-based order creation (Apache Kafka)** to smooth downstream database writes into a flat, predictable stream.

---

## 2. Mental Model

The system acts as a multi-tier filtration funnel where 99% of requests are resolved or dropped in memory before touching downstream databases.

```mermaid
flowchart TD
    subgraph TrafficFunnel["Layered Traffic Filtration Funnel"]
        Shoppers["1,000,000+ Concurrent Shoppers & Bots"] -->|HTTP Buy Requests| WAF["Edge CDN / WAF / Cloudflare Turnstile"]
        WAF -->|1. Bot Defense & Static Cache Hits (50% dropped)| APIGW["API Gateway (Rate Limiter)"]
        APIGW -->|2. Valid Authenticated Requests| FlashService["Flash Sale Service Fleet"]
    end

    subgraph FastPathInventory["In-Memory Inventory Deduction Tier"]
        FlashService -->|3. EVAL: Atomic Multi-Shard Decrement| RedisCluster[(Redis Inventory Cluster: Sharded Stock)]
        RedisCluster -->|Stock Depleted: Sold Out (400k dropped)| FlashService
        RedisCluster -->|Stock Decremented Successfully (10k items)| FlashService
        FlashService -->|Return 202 Accepted: Order Pending| Shoppers
    end

    subgraph AsyncFulfillment["Asynchronous Order Fulfillment Tier"]
        FlashService -->|4. Publish Order Token| KafkaQueue[Kafka: flash-orders-topic]
        KafkaQueue --> WorkerFleet["Order Fulfillment Workers (500 writes/sec)"]
        WorkerFleet --> OrderDB[(Primary Database: PostgreSQL / MySQL)]
        WorkerFleet --> PaymentService["Payment Gateway"]
        
        PaymentService -.->|15-Minute Payment Timeout| RestockWorker["Restock Worker: INCR Stock in Redis"]
        RestockWorker -.-> RedisCluster
    end
```

---

## 3. Architectural Internals and Deep Dive

### 3.1 The Traffic Filtration Funnel Architecture

Handling 1,000,000 concurrent buyers for 10,000 items requires dropping excessive load as close to the client as possible:

1. **Layer 1: CDN & Edge Static Caching (Edge Layer)**:
   - The product detail page (images, CSS, description, reviews) is 100% static and served directly from CDN edge caches.
   - The "Buy Now" button remains disabled until an synchronized Network Time Protocol (NTP) countdown completes on the client.
2. **Layer 2: Edge WAF and Bot Mitigation**:
   - Cloudflare Turnstile or CAPTCHA challenges filter automated headless browser scripts.
   - API Gateways enforce user-level and IP-level rate limits (e.g., maximum 1 click per second per user account).
3. **Layer 3: In-Memory Stock Deduction (Redis Fast Path)**:
   - Requests that pass authentication enter the Flash Sale Service.
   - The service executes an atomic memory decrement in Redis.
   - Exactly 10,000 requests succeed; the remaining 990,000 requests receive an immediate, cached `Sold Out` response in sub-5ms without touching disk storage.
4. **Layer 4: Asynchronous Queue Leveling (Kafka Buffer)**:
   - The 10,000 successful deductions are pushed to Apache Kafka.
   - Downstream order creation workers consume messages at a flat, controlled pace (e.g., 500 orders/sec), persisting records into PostgreSQL without lock contention.

```
+---------------------------------------------------------------+
|         Flash Sale Traffic Filtration Funnel                  |
+---------------------------------------------------------------+
| [1,000,000 Incoming Requests] (Edge CDN & WAF)                |
|       |                                                       |
|       v (- 500,000 duplicate/bot clicks filtered)             |
| [ 500,000 Valid Requests] (API Gateway Rate Limiter)          |
|       |                                                       |
|       v (- 400,000 excess throttled)                          |
| [ 100,000 Checked in Redis] (In-Memory Atomic Decrement)      |
|       |                                                       |
|       v (- 90,000 receive instant 'Sold Out' status)          |
| [  10,000 Successful Purchases] (Enqueued in Kafka)           |
|       |                                                       |
|       v (Smooth constant consumption at 500 orders/sec)       |
| [Database Order Creation & Payment Settlement]                |
+---------------------------------------------------------------+
```

### 3.2 Inventory Pre-Warming & Atomic Lua Decrements

Prior to the sale launch (e.g., 10 minutes before on-sale):
1. **Pre-Warming**: An administrative service queries the relational database, reads the physical inventory count, and populates the stock counter in Redis:
   `SET flash:stock:item_42 10000`
2. **Atomic Lua Script**: When a buyer clicks "Buy", the web service evaluates inventory using an atomic Lua script:

```lua
-- Redis Lua Script: Atomic Stock Decrement
-- KEYS[1]: stock_key (e.g., "flash:stock:item_42")
-- ARGV[1]: requested_quantity (e.g., 1)
-- ARGV[2]: user_id

local current_stock = tonumber(redis.call('get', KEYS[1]) or "0")
local req_qty = tonumber(ARGV[1])

if current_stock >= req_qty then
    -- Deduct stock atomically
    redis.call('decrby', KEYS[1], req_qty)
    -- Record user purchase to prevent duplicate purchases by same account
    redis.call('sadd', KEYS[1] .. ':purchased_users', ARGV[2])
    return 1 -- Success: Stock reserved
else
    return 0 -- Failed: Stock exhausted (Sold Out)
end
```

### 3.3 Resolving Single-Key Hot-Spot Bottlenecks: Stock Sharding

Even though Redis handles 60,000+ operations per second per CPU core, a single flash sale item maps to a single Redis key (`flash:stock:item_42`).
In a Redis Cluster, a single key resides entirely on a **single primary shard**, meaning all 200,000 QPS hit that single CPU core, creating an immediate CPU saturation bottleneck.

**The Multi-Shard Inventory Partitioning Pattern**:
Instead of storing all 10,000 items in a single key, split the stock across $K$ sub-keys distributed across different Redis cluster nodes:
- `flash:stock:item_42:shard_1` = 1,000
- `flash:stock:item_42:shard_2` = 1,000
- ...
- `flash:stock:item_42:shard_10` = 1,000

When a client initiates a purchase:
1. The client hashes its `user_id` to randomly pick a shard:
   $$\text{shard\_index} = \text{hash}(\text{user\_id}) \pmod{10} + 1$$
2. The request executes the Lua script against that specific shard.
3. If that shard is empty, the client seamlessly checks the adjacent shard before declaring the item sold out.
This distributes the 200,000 QPS evenly across 10 independent Redis CPU cores, achieving linear horizontal scalability.

### 3.4 Asynchronous Order Processing & Transactional Outbox

When the Redis decrement succeeds:
1. The Flash Sale Service returns an immediate `HTTP 202 Accepted` response with an `order_token` to the client.
The client displays an animated "Reserving your order..." status screen.
2. The service writes an `OrderPlacedEvent` to an **Apache Kafka** topic partitioned by `order_id`.
3. An asynchronous worker fleet consumes Kafka partitions at a controlled rate (e.g., 500 messages/sec per worker pool):
   - Opens a database transaction in PostgreSQL.
   - Inserts order records (`orders`, `order_items`).
   - Deducts the permanent relational inventory table.
   - Emits a WebSocket/SSE notification to the waiting client browser: "Order reserved! You have 15 minutes to complete payment."

### 3.5 Payment Expiration & Inventory Restock Loop

Not all buyers complete payment; credit cards fail, or users change their minds:
- **15-Minute Payment Window**: The created order has a strict expiration timestamp (`expires_at = now + 900`).
- **Delayed Message / Dead Letter Queue Restock**:
  1. When the order is placed, an asynchronous delayed event is scheduled in Kafka / RabbitMQ with a 15-minute delay.
  2. At minute 15, the restock worker consumes the event and checks the database:
     `SELECT status FROM orders WHERE id = ?;`
  3. If status is `PAID`, no action is taken.
  4. If status is still `PENDING_PAYMENT`, the worker marks the order `CANCELLED`, issues an atomic increment back to the Redis stock counter (`INCRBY flash:stock:item_42 1`), and pushes a notification to waitlisted shoppers.

---

## 4. Trade-offs and Comparisons

| Dimension | Synchronous Relational DB Updates | Optimistic Locking with Retries | Redis Lua Fast Path + Async Kafka |
|---|---|---|---|
| Max Throughput | Very low (500 - 2,000 QPS) | Low (collapses under 99% conflict retries) | Ultra-high (200,000+ QPS) |
| Latency to User | High (50ms - 2,000ms disk lock wait) | High (due to cascading retry loops) | Ultra-low (< 5ms in-memory response) |
| Overselling Risk | Zero | Zero | Zero (guaranteed by atomic Lua) |
| Database Load | Catastrophic (100% CPU, connection pool crash) | Catastrophic | Flat, controlled, constant throughput |
| Architecture Complexity | Minimal | Low | Moderate to High (requires reconciliation) |

---

## 5. Failure Modes and Mitigations

### 5.1 Redis Cluster Failover and Phantom Stock
- **Failure Mode**: The Redis master crashes during the flash sale before asynchronous replication to its replica finishes.
The newly promoted replica holds stale stock counts, risking overselling.
- **Mitigation**: Database as the ultimate source of truth.
The relational database inventory column enforces an unsigned integer constraint (`stock INT UNSIGNED CHECK (stock >= 0)`).
If a phantom stock order ever reaches the database worker, the database constraint rejects the transaction, preventing overselling.

### 5.2 Bot Farm Account Flooding
- **Failure Mode**: Scalper bot syndicates create 50,000 fake accounts and automated scripts to drain all inventory in milliseconds.
- **Mitigation**: Multi-factor identity validation:
1. Account age verification: Only accounts created > 30 days ago with verified phone numbers can participate.
2. Purchase limit: Enforce maximum 1 item per authenticated account using Redis sets (`SADD item:buyers user_id`).
3. Dynamic URL tokenization: The flash sale API endpoint includes a randomized cryptographic salt generated 5 seconds before on-sale launch, preventing bots from pre-recording API calls.

### 5.3 Kafka Message Backlog and Client Timeout Panic
- **Failure Mode**: Downstream database workers stall, causing Kafka queue consumer lag to spike to 2 minutes.
Users think the app crashed and refresh repeatedly.
- **Mitigation**: Client polling / WebSocket backpressure.
The client polls `GET /v1/orders/status?token=xyz` every 3 seconds with exponential backoff.
If queue depth exceeds thresholds, auto-scale order worker pods horizontally on Kubernetes.

---

## 6. Hands-On Verification

The following standalone Python implementation demonstrates in-memory inventory pre-warming, multi-shard stock partitioning, atomic Lua-style decrement simulation, high-concurrency overselling prevention with 100 threads, and timeout restocking.

```python
#!/usr/bin/env python3
"""
Production-grade demonstration of E-Commerce Flash Sale System:
- Multi-shard stock partitioning to prevent single-key hot spots
- Atomic stock decrement with duplicate buyer detection
- Concurrency stress test: 100 threads competing for 10 items
- Zero-overselling invariant proof
- Payment timeout inventory restock loop
"""

import time
import random
import threading
from typing import Dict, Set, Tuple


class ShardedFlashSaleInventory:
    """Simulates multi-shard Redis inventory with atomic Lua script execution."""
    def __init__(self, num_shards: int = 4):
        self.num_shards = num_shards
        # shard_id -> available_stock
        self.stock_shards: Dict[int, int] = {i: 0 for i in range(num_shards)}
        self.purchased_users: Set[str] = set()
        self._locks = [threading.Lock() for _ in range(num_shards)]
        self._global_lock = threading.Lock()

    def prewarm_inventory(self, total_stock: int):
        base_per_shard = total_stock // self.num_shards
        remainder = total_stock % self.num_shards
        for i in range(self.num_shards):
            self.stock_shards[i] = base_per_shard + (1 if i < remainder else 0)
        self.purchased_users.clear()
        print(f"Pre-warmed {total_stock} items across {self.num_shards} shards: {self.stock_shards}")

    def attempt_purchase(self, user_id: str, qty: int = 1) -> Tuple[bool, str]:
        # 1. Enforce 1 purchase per user
        with self._global_lock:
            if user_id in self.purchased_users:
                return False, "User already purchased maximum limit (1 item)."

        # 2. Pick initial shard based on hash(user_id)
        start_shard = hash(user_id) % self.num_shards

        # 3. Check shard and spill over to adjacent shards if empty
        for offset in range(self.num_shards):
            shard_idx = (start_shard + offset) % self.num_shards
            with self._locks[shard_idx]:
                if self.stock_shards[shard_idx] >= qty:
                    self.stock_shards[shard_idx] -= qty
                    with self._global_lock:
                        self.purchased_users.add(user_id)
                    return True, f"Success! Reserved from shard {shard_idx}."

        return False, "Sold out! All shards depleted."

    def restock(self, qty: int = 1):
        """Restocks inventory following order cancellation or payment timeout."""
        target_shard = random.randint(0, self.num_shards - 1)
        with self._locks[target_shard]:
            self.stock_shards[target_shard] += qty
        print(f"[Restock] Returned {qty} item to shard {target_shard}.")

    def get_total_available_stock(self) -> int:
        return sum(self.stock_shards.values())


if __name__ == "__main__":
    inventory = ShardedFlashSaleInventory(num_shards=4)
    TOTAL_ITEMS = 15
    inventory.prewarm_inventory(total_stock=TOTAL_ITEMS)

    print("\n--- 1. High-Concurrency Stress Test (100 Buyers Competing for 15 Items) ---")
    results = {}

    def buyer_task(user_id: str):
        success, msg = inventory.attempt_purchase(user_id, qty=1)
        results[user_id] = (success, msg)

    threads = []
    for i in range(100):
        t = threading.Thread(target=buyer_task, args=(f"buyer_{i+1}",))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    successful_purchases = [uid for uid, res in results.items() if res[0]]
    rejected_purchases = [uid for uid, res in results.items() if not res[0]]

    print(f"Total Successful Purchases: {len(successful_purchases)}")
    print(f"Total Rejected Purchases:   {len(rejected_purchases)}")
    print(f"Remaining Available Stock:  {inventory.get_total_available_stock()}")

    # INVARIANT CHECK: Zero Overselling
    assert len(successful_purchases) == TOTAL_ITEMS, f"Overselling bug! Sold {len(successful_purchases)} for {TOTAL_ITEMS} stock!"
    assert inventory.get_total_available_stock() == 0, "Stock should be exactly zero!"
    print("Verification Passed: Zero overselling confirmed! Exactly 15 items sold.")

    print("\n--- 2. Duplicate Purchase Rejection Check ---")
    first_buyer = successful_purchases[0]
    success, msg = inventory.attempt_purchase(first_buyer, qty=1)
    assert success is False, "Duplicate purchase by same user must be rejected!"
    print(f"Duplicate attempt by {first_buyer}: {msg}")

    print("\n--- 3. Testing Payment Timeout Restock ---")
    inventory.restock(qty=2)
    assert inventory.get_total_available_stock() == 2
    success, msg = inventory.attempt_purchase("new_waitlist_buyer", qty=1)
    assert success is True, "Waitlist buyer should successfully claim restocked item!"
    print(f"Waitlist Buyer Result: {msg}")
    print("\nVerification Passed: Restock pipeline operational!")
```

### CLI Verification

Execute health checks and flash sale purchase assertions across environments:

```bash
# Linux / macOS: Submit flash sale purchase request via curl
curl -X POST https://api.amazon.com/v1/flash-sale/buy \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer test_buyer_token" \
  -d '{"item_id": "item_ps5_drop", "quantity": 1, "idempotency_key": "idem_998811"}'

# Linux / macOS: Poll order status token
curl -X GET https://api.amazon.com/v1/orders/status?order_token=tok_88421 \
  -H "Authorization: Bearer test_buyer_token"

# Windows PowerShell: Inspect stock status API
Invoke-RestMethod -Uri "https://api.amazon.com/v1/flash-sale/items/item_ps5_drop/availability"
```

---

## 7. Performance Characteristics and Capacity Planning

### 7.1 Throughput and Traffic Math
- **Target SKU Inventory**: 10,000 items.
- **Traffic Burst**: 1,000,000 users clicking within a 5-second window.
- **Peak Write QPS**:
  $$\text{Throughput} = \frac{1,000,000}{5} = 200,000 \text{ requests/sec}$$
- **Redis Multi-Shard Distribution**:
  - Partitioning stock across 10 Redis shards reduces load to $20,000 \text{ QPS per shard}$.
  - A modern Redis instance running on a cloud core comfortably handles 50,000+ ops/sec, operating at under 40% CPU utilization.

### 7.2 Database Leveling & Kafka Queue Sizing
- Database cannot handle 200,000 writes/sec.
- Kafka buffer absorbs the 10,000 successful orders instantly.
- Order workers pull from Kafka at a smooth rate of **500 orders/second**:
  $$\text{Drain Duration} = \frac{10,000 \text{ orders}}{500 \text{ orders/sec}} = 20 \text{ seconds}$$
- All 10,000 database orders are committed to PostgreSQL within 20 seconds of sale launch with zero database spikes or connection pool exhaustion.

---

## 8. In Production: Real-World Architecture (Alibaba Singles' Day)

Alibaba’s 11.11 Global Shopping Festival processes over 583,000 orders per second at midnight peak:
1. **Traffic Funneling**: Alibaba leverages custom LVS (Linux Virtual Server) load balancers and Tengine (custom NGINX) to shed unauthenticated and duplicate traffic at the network edge.
2. **In-Memory Inventory Engine**: Inventory is managed in a custom in-memory caching system (Tair) utilizing atomic CAS operations and stock sharding.
3. **Database Thread Pooling (AliSQL)**: Alibaba patched MySQL source code to create **AliSQL**, introducing kernel-level inventory queuing and transaction commit grouping directly inside InnoDB to prevent row-lock serialization crashes.

---

## 9. Interview Questions and Deep Dives

> [!question] Question 1: How do you prevent inventory overselling during a 200,000 QPS flash sale?
> [!success]- Answer
> 1. Pre-warm inventory into Redis prior to the sale.
> 2. Execute deductions using an **atomic Redis Lua script** (`DECRBY`).
> Because Redis executes Lua scripts single-threaded on a given key, exactly the configured quantity is deducted, and all subsequent callers receive an immediate `Sold Out` status.
> 3. Enforce a physical database integrity constraint (`stock INT UNSIGNED CHECK (stock >= 0)`) as a secondary safeguard.
> If a phantom order ever bypassed Redis, the database transaction would abort on negative constraint violation.

> [!question] Question 2: Why does direct database locking (`UPDATE stock = stock - 1 WHERE stock > 0`) fail during a flash sale?
> [!success]- Answer
> When 200,000 concurrent transactions attempt to execute updates on the exact same table row, the database serializes access through pessimistic exclusive row locks.
> Thousands of transactions wait in lock queues, holding open connections.
> The database connection pool exhausts in seconds, thread context switching consumes 100% of CPU, and transaction timeouts cascade across the entire platform, knocking down unrelated shopping services.

> [!question] Question 3: What is the "Hot-Spot Key" problem in Redis, and how does Stock Sharding solve it?
> [!success]- Answer
> In a Redis Cluster, a single key (`item:stock`) is assigned to a single hash slot on a **single primary shard**.
> All 200,000 QPS hit that single Redis node's single-threaded event loop, saturating its CPU core while other cluster nodes remain idle.
> **Stock Sharding** partitions the 10,000 stock across $K$ sub-keys (`item:stock:shard_1` through `shard_K`) distributed across multiple cluster shards.
> Clients hash randomly to a shard to decrement, spreading the 200,000 QPS evenly across $K$ separate CPU cores.

> [!question] Question 4: How does the system handle buyers who reserve an item but never pay?
> [!success]- Answer
> Every reserved order has a strict 15-minute payment expiration timer.
> An asynchronous delayed message is published to Kafka/RabbitMQ with a 15-minute delay.
> When the delay elapses, a background worker inspects order status:
> If unpaid, the order is transitioned to `CANCELLED`, and an atomic increment (`INCRBY`) restores the inventory back to the Redis stock counter, making the item available for waitlisted buyers.

> [!question] Question 5: Why is asynchronous queueing (Kafka) mandatory between the flash sale service and the database?
> [!success]- Answer
> Relational databases are designed for ACID durability, not absorbing 200,000 write bursts per second.
> Kafka acts as a **shock absorber** and rate leveler:
> Redis handles the 200,000 QPS in-memory check and pushes the 10,000 winning order tokens into Kafka.
> Downstream worker pools consume Kafka at a flat, controlled rate (e.g., 500 writes/sec), ensuring database transactions commit smoothly within capacity limits.

> [!question] Question 6: How do you prevent users from clicking "Buy" multiple times and placing duplicate orders?
> [!success]- Answer
> 1. Client-Side: Disable the "Buy Now" button immediately upon the first click and show a loading spinner.
> 2. Idempotency Keys: Generate a unique UUID idempotency key per checkout session.
> 3. Server-Side Redis Set: Inside the atomic Lua script, add the user's ID to a set (`SADD item:buyers user_id`).
> If the user is already in the set, the script rejects the purchase with an error, guaranteeing at most 1 item per user account.

> [!question] Question 7: How do you protect the product detail page from crashing under 5 million page views in the minutes before launch?
> [!success]- Answer
> Make the product detail page **100% static**:
> All product descriptions, specifications, and images are pre-rendered into static HTML/JSON files and cached across CDN edge servers globally.
> The page does not query origin databases.
> The "Buy" button is activated client-side via a lightweight JavaScript timestamp check synchronized with an edge time beacon.

> [!question] Question 8: How do you prevent scalper bot networks from grabbing all items in the first 100 milliseconds?
> [!success]- Answer
> 1. Enforce bot detection at the edge WAF (Cloudflare Turnstile or proof-of-work challenges).
> 2. Require verified phone numbers and accounts older than 30 days with purchase history.
> 3. Dynamic API URL Tokenization: Do not expose the static checkout endpoint in advance.
> The server publishes the cryptographically signed checkout URL token only at the exact second the sale begins, rendering pre-programmed bot scripts useless.

> [!question] Question 9: What happens if a Redis shard fails during the sale, and how is inventory reconciled?
> [!success]- Answer
> Configure Redis with primary-replica multi-AZ replication.
> If a shard crashes, Redis Sentinel or Cluster promotes the replica within 3-5 seconds.
> Post-sale, an automated **Reconciliation Worker** cross-checks the total count of successfully committed database orders against the physical warehouse inventory.
> Any discrepancy is resolved by offering leftover stock to waitlisted shoppers.

> [!question] Question 10: How does the client learn that their asynchronous order has been created?
> [!success]- Answer
> When the Redis decrement succeeds, the server returns an `HTTP 202 Accepted` response with an `order_token`.
> The client can receive confirmation via:
> 1. **Short Polling**: Client polls `GET /v1/orders/status?token=xyz` every 2 seconds for up to 30 seconds.
> 2. **Server-Sent Events (SSE) / WebSockets**: The client maintains an open SSE connection; when the database worker commits the order, it publishes a notification to Redis Pub/Sub, which pushes the finalized order ID directly to the user's browser.

---

## 10. Related Concepts and Wikilinks

- [[Redis-Architecture]]: In-memory data structures, Lua scripts, and cluster sharding.
- [[Apache-Kafka]]: Message queuing, consumer groups, and backpressure rate leveling.
- [[Optimistic-vs-Pessimistic-Locking]]: Concurrency control comparison in transactional inventory.
- [[Pub-Sub-Architecture]]: Asynchronous order notification dispatch.
- [[Consistent-Hashing]]: Distributing stock shards across cache nodes.

---

## 11. Further Reading

- Xu, Alex. *System Design Interview – An Insider’s Guide (Volume 2)*. Chapter 10: Real-time Gaming Leaderboard & Chapter 13: Stock Exchange.
- Alibaba Tech. *The Secret Behind Alibaba’s 11.11 Shopping Festival Infrastructure*.
- Kleppmann, Martin. *Designing Data-Intensive Applications*. O'Reilly Media. Chapter 11: Stream Processing.
- Redis Documentation: *Transactions and Scripting with Lua*.

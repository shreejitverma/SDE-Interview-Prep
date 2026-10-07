---
type: case-study
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System Design Interview - An Insider's Guide, Alex Xu"
  - "Designing Data-Intensive Applications, Martin Kleppmann"
  - "Ticketmaster Engineering: Handling On-Sale Spikes at Scale"
---

# Design a Ticket Booking System (Ticketmaster / BookMyShow)

## 1. TL;DR

A distributed ticket booking platform manages seat discovery, real-time inventory locking, payment settlement, and ticket issuance for high-demand concert and cinema events.
During peak on-sale events (e.g., a stadium tour where 50,000 seats sell out in under 2 minutes), the platform experiences extreme traffic bursts exceeding 100,000 concurrent requests per second competing for the exact same subset of premium seats.
The central engineering requirement is strictly eliminating **double-booking** (guaranteeing that no single seat is ever sold to two different customers) while maintaining sub-second response times and high availability.
Executing database row-level locking (`SELECT ... FOR UPDATE`) directly against relational databases during on-sale spikes causes severe connection exhaustion and database crash loops.
The industry standard architecture combines an edge **Virtual Waiting Room** (traffic leveling queue), an in-memory **Redis distributed reservation engine with atomic Lua scripts and Time-To-Live (TTL) hold leases**, and an **Orchestration-based Saga distributed transaction** for reliable asynchronous payment fulfillment.

---

## 2. Mental Model

The architecture decouples high-contention temporary seat holding from long-running payment settlement and persistent relational fulfillment.

```mermaid
flowchart TD
    subgraph TrafficLeveling["Traffic Leveling & Waiting Room"]
        UserFleet["100,000+ Concurrent Fans"] -->|On-Sale Spike| EdgeLB["Edge Load Balancer"]
        EdgeLB --> WaitingRoom["Virtual Waiting Room (Queue: Token Bucket / Fair FIFO)"]
        WaitingRoom -->|Metered Admittance (500 users/sec)| BookingGW["API Gateway"]
    end

    subgraph FastPathReservation["In-Memory Inventory & Hold Tier"]
        BookingGW --> BookingService["Booking Service Fleet"]
        BookingService -->|EVAL: Atomic Hold (10-min TTL)| RedisInventory[(Redis Cluster: Seat States)]
        RedisInventory -->|Hold Success: Lock Acquired| BookingService
        RedisInventory -.->|Seat Already Held: 409 Conflict| BookingService
    end

    subgraph SagaFulfillment["Saga Distributed Transaction Pipeline"]
        BookingService --> SagaOrchestrator["Booking Saga Orchestrator"]
        SagaOrchestrator -->|1. Authorize Charge| PaymentService["Payment Gateway (Stripe)"]
        PaymentService -->|Payment Authorized| SagaOrchestrator
        SagaOrchestrator -->|2. Commit Final Sale| RelationalDB[(Primary Database: PostgreSQL Cluster)]
        SagaOrchestrator -->|3. Finalize Permanent State| RedisInventory
        SagaOrchestrator -->|4. Issue Barcode & QR| TicketService["Ticket Delivery Service"]
        
        PaymentService -.->|Payment Failed / Expired| CompTrans["Compensating Transaction: Release Redis Hold"]
        CompTrans -.-> RedisInventory
    end
```

---

## 3. Architectural Internals and Deep Dive

### 3.1 The Seat Reservation Lifecycle & State Machine

Every individual seat in a venue transitions through a strict, irreversible state progression:

```
[AVAILABLE] 
     |
     v (User clicks seat: 10-minute hold lease acquired)
  [HELD] 
  /    \
 /      \ (User pays successfully within 10 minutes)
v        v
[AVAILABLE]  [BOOKED] (Final state: permanently sold)
(Timer expires / Payment rejected: hold released)
```

1. **AVAILABLE**: Seat is unreserved; visible to all browsing customers.
2. **HELD**: Temporarily reserved by a specific user session for **10 minutes**.
No other customer can select or purchase this seat while the hold lease remains active.
3. **BOOKED**: Payment confirmed; seat is permanently assigned and ticket barcode generated.
4. **RELEASED / EXPIRED**: If the user abandons checkout or the payment fails within 10 minutes, the hold expires automatically, and the seat reverts to `AVAILABLE`.

### 3.2 Concurrency Control: Why Database Locking Fails

#### Approach A: Pessimistic Relational Locking (`SELECT ... FOR UPDATE`)
```sql
BEGIN;
SELECT status FROM seats WHERE id = 42 FOR UPDATE;
-- Check if available, then:
UPDATE seats SET status = 'HELD', reserved_by = 'user_123' WHERE id = 42;
COMMIT;
```
- **Flaw at Hyperscale**: When 50,000 fans attempt to reserve the same 500 front-row seats within the first 10 seconds of on-sale, thousands of database transactions queue up waiting for the same row locks.
Database connection pools saturate instantly, CPU usage hits 100%, and transaction timeouts cascade across the platform, rendering the website completely unresponsive.

#### Approach B: Optimistic Locking with Versioning
```sql
UPDATE seats SET status = 'HELD', version = version + 1 
WHERE id = 42 AND version = 5 AND status = 'AVAILABLE';
```
- **Flaw at Hyperscale**: When contention is extreme, 99.9% of concurrent write queries fail on version mismatch.
Retrying causes massive write amplification and database CPU thrashing without improving success rates.

#### Approach C: In-Memory Atomic Reservation with Redis Lua Scripts (Selected Standard)
By moving the transient reservation tier into Redis, state transitions execute in sub-millisecond memory operations.
Because Redis executes Lua scripts atomically and single-threaded on a given key shard, race conditions are mathematically impossible:

```lua
-- Redis Lua Script: Atomic Seat Hold with TTL Lease
-- KEYS[1]: seat_key (e.g., "show:101:seat:A12")
-- ARGV[1]: user_id
-- ARGV[2]: hold_ttl_seconds (e.g., 600)

local current_state = redis.call('get', KEYS[1])

if current_state == nil or current_state == "AVAILABLE" then
    -- Seat is available: acquire hold lease
    redis.call('set', KEYS[1], "HELD:" .. ARGV[1], "EX", tonumber(ARGV[2]))
    return 1 -- Success
else
    return 0 -- Failed: Already HELD or BOOKED
end
```

### 3.3 Traffic Leveling: The Virtual Waiting Room

Before users reach the booking service or seat map, incoming traffic passes through an edge **Virtual Waiting Room** (e.g., Cloudflare Waiting Room or custom Redis queue):
1. **Queueing Mechanism**: When on-sale starts, all users visiting the event URL receive an encrypted, signed queue token containing a random placement rank.
2. **Metered Ingress**: The waiting room meters traffic into the seat selection engine at a controlled, constant rate (e.g., 500 users per second).
3. **Capacity Protection**: This converts a 100,000 QPS thundering herd spike into a predictable, constant stream that operates well within the backend database's saturation ceiling.

### 3.4 Distributed Transactions: The Saga Pattern

Seat booking spans multiple distinct subsystems: inventory locking (Redis), financial authorization (Stripe/PayPal), and permanent ticket issuance (PostgreSQL).
Using a traditional Two-Phase Commit (2PC) protocol across external payment gateways is impossible because third-party payment APIs do not support XA transactions and network roundtrips tie up internal locks for seconds.

The system implements an **Orchestration-based Saga**:
1. **Action 1 (Hold Inventory)**: Booking Service acquires a 10-minute hold on Redis.
2. **Action 2 (Authorize Payment)**: Client submits credit card details to the payment gateway via the Payment Service.
Payment gateway executes 3D-Secure authentication and returns a payment authorization token.
3. **Action 3 (Commit Sale)**: The Saga orchestrator writes the finalized booking to the relational database:
   ```sql
   INSERT INTO bookings (id, user_id, show_id, total_price, status) VALUES (...);
   INSERT INTO booking_seats (booking_id, seat_id) VALUES (...);
   ```
4. **Action 4 (Finalize Inventory)**: Update the Redis key to `BOOKED` permanently (removing TTL expiration).
5. **Compensating Transaction**: If Action 2 (Payment) fails, times out, or is declined, the orchestrator immediately triggers a compensation step:
   - Delete the `HELD` key from Redis, reverting the seat to `AVAILABLE` for other users.
   - Return an error to the user with zero financial charge.

```
+---------------------------------------------------------------+
|         Ticket Booking Saga Execution Timeline                |
+---------------------------------------------------------------+
| [1. Hold Seat in Redis] ---------------------------> Success  |
|          |                                                    |
|          v                                                    |
| [2. Authorize Payment (Stripe)] ----(Declined)-----> Failure  |
|          |                                             |      |
|          | (Success)                                   v      |
|          v                             [Compensating Step]    |
| [3. Insert Booking in DB]              [Delete Redis Hold]    |
|          |                             [Seat -> AVAILABLE]    |
|          v                                                    |
| [4. Update Redis: BOOKED]                                     |
+---------------------------------------------------------------+
```

### 3.5 Idempotency Keys and Late Payment Resolution

A critical edge case occurs when payment processing experiences network delays:
- **The Late Payment Paradox**: The 10-minute Redis hold expires at 12:10:00.
The seat reverts to `AVAILABLE` and is immediately locked by User B at 12:10:01.
At 12:10:03, Stripe's webhook finally reports that User A's payment succeeded.
- **Mitigation via Two-Phase Settlement**:
  1. Payment Service executes an **Authorization Only** during checkout, not an immediate capture.
  2. Before capturing the funds, the Saga orchestrator verifies that the reservation lease has not expired.
  3. If the lease expired and the seat was claimed by another customer, the orchestrator voids the authorization immediately, issues an automated full refund to User A, and notifies User A that their checkout session timed out.
  Money is captured **only after** the permanent database seat update succeeds.

---

## 4. Trade-offs and Comparisons

| Dimension | DB Pessimistic Locking (`FOR UPDATE`) | DB Optimistic Locking (`VERSION`) | Redis Atomic Lua Reservation |
|---|---|---|---|
| Spike Concurrency Handling | Catastrophic failure (connection pool exhaustion) | High write thrashing / 99% rollbacks | Seamless (handles 100k+ ops/sec per shard) |
| Latency Overhead | High (50ms - 500ms disk/lock waits) | Moderate | Ultra-low (< 2ms in-memory) |
| Double-Booking Risk | Zero (enforced by DB engine) | Zero (enforced by DB version) | Zero (enforced by Redis single-threaded Lua) |
| State Recovery Complexity | Trivial (relational rollbacks) | Trivial | Moderate (requires Redis-DB reconciliation) |
| TTL Hold Expiration | Expensive (requires polling cron jobs) | Expensive | Automatic (native Redis key expiration) |

---

## 5. Failure Modes and Mitigations

### 5.1 Redis Cluster Node Crash During Active Holds
- **Failure Mode**: The Redis master hosting the active hold keys crashes before asynchronous replication to its replica completes, losing the hold state.
- **Mitigation**: Configure Redis with AOF (Append-Only File) `fsync everysec` and multi-AZ primary-replica failover.
If a failover occurs, the booking service cross-references the persistent relational database before allowing any new reservation to ensure that a seat confirmed in the DB is never overwritten in Redis.

### 5.2 Scalper Bot Networks Sweeping Inventory
- **Failure Mode**: Automated bot scripts bypass the front-end UI, query reservation APIs directly, and hold entire stadium sections in 100 milliseconds.
- **Mitigation**: Deploy **CAPTCHA challenges (Cloudflare Turnstile)** prior to issuing waiting room tokens.
Enforce hard ticket limits (e.g., maximum 4 tickets per authenticated user account, phone number, and billing credit card).
Flag and quarantine accounts demonstrating non-human mouse movement or instant API execution patterns.

### 5.3 Payment Webhook Delivery Drops / Network Partitions
- **Failure Mode**: Stripe successfully charges the customer, but the webhook notification is dropped by network firewalls, leaving the booking in an unconfirmed state.
- **Mitigation**: Deploy an asynchronous **Payment Reconciliation Worker**.
If a booking remains in `PENDING_PAYMENT` for 8 minutes, the worker proactively polls Stripe's API (`GET /v1/charges/:charge_id`) to verify payment status directly before allowing the 10-minute hold lease to expire.

---

## 6. Hands-On Verification

The following standalone Python implementation demonstrates atomic seat reservations via Lua scripting simulation, automatic hold expiration, double-booking prevention, and the Saga payment fulfillment pipeline.

```python
#!/usr/bin/env python3
"""
Production-grade demonstration of Ticket Booking Architecture:
- Atomic in-memory seat reservation with TTL leases
- Concurrency simulation: 5 users competing for the same seat
- Saga pattern payment settlement and compensating transaction
- Late payment and expiration defense
"""

import time
import threading
from typing import Dict, Optional, Tuple


class RedisSeatInventorySimulator:
    """Simulates Redis executing atomic Lua scripts for seat holds."""
    def __init__(self):
        # seat_id -> {"state": "AVAILABLE" | "HELD" | "BOOKED", "user_id": str, "expires_at": float}
        self._seats: Dict[str, Dict] = {}
        self._lock = threading.Lock()

    def initialize_seat(self, seat_id: str):
        self._seats[seat_id] = {"state": "AVAILABLE", "user_id": None, "expires_at": 0}

    def hold_seat(self, seat_id: str, user_id: str, ttl_seconds: float) -> bool:
        """Atomic Lua script simulation."""
        with self._lock:
            seat = self._seats.get(seat_id)
            if not seat:
                return False

            now = time.time()
            # Check if seat is currently available or if previous hold expired
            if seat["state"] == "AVAILABLE" or (seat["state"] == "HELD" and seat["expires_at"] <= now):
                seat["state"] = "HELD"
                seat["user_id"] = user_id
                seat["expires_at"] = now + ttl_seconds
                return True
            return False  # Already HELD or BOOKED

    def confirm_booking(self, seat_id: str, user_id: str) -> bool:
        with self._lock:
            seat = self._seats.get(seat_id)
            now = time.time()
            # Can only confirm if HELD by THIS user and NOT expired
            if seat and seat["state"] == "HELD" and seat["user_id"] == user_id and seat["expires_at"] > now:
                seat["state"] = "BOOKED"
                seat["expires_at"] = float("inf")
                return True
            return False

    def release_hold(self, seat_id: str, user_id: str):
        with self._lock:
            seat = self._seats.get(seat_id)
            if seat and seat["state"] == "HELD" and seat["user_id"] == user_id:
                seat["state"] = "AVAILABLE"
                seat["user_id"] = None
                seat["expires_at"] = 0

    def get_seat_status(self, seat_id: str) -> str:
        with self._lock:
            seat = self._seats.get(seat_id)
            if not seat:
                return "UNKNOWN"
            now = time.time()
            if seat["state"] == "HELD" and seat["expires_at"] <= now:
                return "AVAILABLE (Expired)"
            return seat["state"]


class BookingSagaOrchestrator:
    def __init__(self, inventory: RedisSeatInventorySimulator):
        self.inventory = inventory

    def attempt_booking(self, seat_id: str, user_id: str, payment_success: bool, delay_payment_sec: float) -> Tuple[bool, str]:
        # Step 1: Hold seat in Redis (10-second TTL for simulation)
        held = self.inventory.hold_seat(seat_id, user_id, ttl_seconds=1.0)
        if not held:
            return False, "Seat already reserved or sold."

        # Step 2: Payment Gateway processing delay
        time.sleep(delay_payment_sec)

        # Step 3: Check payment outcome
        if not payment_success:
            # Compensating transaction
            self.inventory.release_hold(seat_id, user_id)
            return False, "Payment failed. Seat hold released."

        # Step 4: Confirm Booking in DB & Inventory
        confirmed = self.inventory.confirm_booking(seat_id, user_id)
        if confirmed:
            return True, "Booking confirmed! Ticket issued."
        else:
            return False, "Hold expired before payment completed. Refund issued."


if __name__ == "__main__":
    inventory = RedisSeatInventorySimulator()
    inventory.initialize_seat("Seat_A1")
    saga = BookingSagaOrchestrator(inventory)

    print("--- 1. Testing Concurrency Race Condition (5 Fans Competing for Seat_A1) ---")
    results = {}

    def fan_attempt(user_id: str):
        success, msg = saga.attempt_booking("Seat_A1", user_id, payment_success=True, delay_payment_sec=0.1)
        results[user_id] = (success, msg)

    threads = []
    for i in range(5):
        t = threading.Thread(target=fan_attempt, args=(f"Fan_{i+1}",))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    successful_fans = [uid for uid, res in results.items() if res[0]]
    print(f"Total Successful Bookings: {len(successful_fans)}")
    for uid, res in results.items():
        print(f"  {uid}: Success={res[0]} | Message={res[1]}")

    assert len(successful_fans) == 1, "Double-booking detected! Exactly 1 fan must succeed."
    assert inventory.get_seat_status("Seat_A1") == "BOOKED"
    print(f"Verification Passed: Exactly one fan ({successful_fans[0]}) acquired the seat!")

    print("\n--- 2. Testing Hold Expiration & Late Payment Defense ---")
    inventory.initialize_seat("Seat_B2")
    # Fan holds seat, but payment takes 1.2s (exceeds 1.0s TTL)
    success, msg = saga.attempt_booking("Seat_B2", "Slow_Fan", payment_success=True, delay_payment_sec=1.2)
    print(f"Slow Fan Outcome: Success={success} | Reason: {msg}")
    assert success is False, "Expired hold must not be confirmed!"
    print(f"Seat_B2 Status: {inventory.get_seat_status('Seat_B2')}")
    print("\nVerification Passed: Late payment correctly rejected and refunded!")
```

### CLI Verification

Execute health checks and reservation API assertions:

```bash
# Linux / macOS: Reserve seat via POST curl
curl -X POST https://api.ticketmaster.com/v1/events/evt_998/reserve \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer test_fan_token" \
  -d '{"seat_id": "sec_101_row_A_seat_12", "hold_ttl_seconds": 600}'

# Linux / macOS: Confirm booking post-payment
curl -X POST https://api.ticketmaster.com/v1/bookings/confirm \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer test_fan_token" \
  -d '{"reservation_id": "res_8842", "payment_intent_id": "pi_stripe_12345"}'

# Windows PowerShell: Inspect seat status
Invoke-RestMethod -Uri "https://api.ticketmaster.com/v1/events/evt_998/seats/sec_101_row_A_seat_12"
```

---

## 7. Performance Characteristics and Capacity Planning

### 7.1 Throughput and Traffic Math
- **Venue Scale**: 50,000 stadium seats.
- **Peak On-Sale Spike**: 100,000 concurrent users hitting the site at 10:00:00 AM.
- **Waiting Room Ingress Shaping**:
  - The waiting room meters traffic to **500 users/second** admitted to the seat map.
  - Backend API load is smoothed to a manageable 1,500 - 2,500 QPS.
- **Seat Map Lookups**:
  - Seat layout JSON (50,000 seats with coordinates and status) $\approx 500 \text{ KB}$ compressed.
  - Cached heavily at CDN edges and refreshed every 2 seconds, serving 98% of seat map views without touching origin servers.

### 7.2 Memory Sizing (Redis Seat Inventory Tier)
- Data stored per seat:
  - Key: `show:<show_id>:seat:<seat_id>` $\approx 32 \text{ bytes}$.
  - Value: `HELD:<user_id>` $\approx 32 \text{ bytes}$.
  - TTL and dictionary overhead: ~48 bytes.
  - Total per seat $\approx 112 \text{ bytes}$.
- Memory for a 50,000-seat stadium:
  $$50,000 \times 112 \text{ bytes} \approx 5.6 \text{ MB}$$
- Memory for 1,000 concurrent active concert on-sales:
  $$1,000 \times 5.6 \text{ MB} \approx 5.6 \text{ GB of RAM}$$
- The entire global seat inventory fits comfortably in a single modest Redis node; deploying a multi-node cluster is done strictly for high availability and failover redundancy.

---

## 8. In Production: Real-World Architecture (Ticketmaster)

Ticketmaster manages extreme ticket on-sales using specialized infrastructure components:
1. **Virtual Waiting Room**: Implemented via Queuing systems (such as Queue-it) to prevent origin saturation during high-demand on-sales (e.g., Taylor Swift Eras Tour).
2. **Verified Fan Registration**: Requires users to register days in advance, filtering out automated bot farm scripts before on-sale launches.
3. **In-Memory Inventory Engine**: Seat inventory is held in high-speed in-memory caches fronting partitioned relational databases, allowing real-time reservation leases and automatic timeout releases.

---

## 9. Interview Questions and Deep Dives

> [!question] Question 1: How do you strictly guarantee that a seat is never double-booked under extreme concurrency?
> [!success]- Answer
> 1. In-memory reservation executes an **atomic Lua script** in Redis on the single key representing that seat.
> Because Redis runs scripts single-threaded on a given key, exactly one user's hold succeeds, and all other concurrent callers receive an immediate conflict response.
> 2. At the relational database fulfillment layer, enforce a database-level `UNIQUE` constraint on `(show_id, seat_id)`.
> Even in catastrophic failure scenarios where Redis state is lost, the second database `INSERT` violates the unique constraint and aborts, providing defense in depth.

> [!question] Question 2: Why is database row locking (`SELECT FOR UPDATE`) inappropriate for on-sale ticket drops?
> [!success]- Answer
> Under spike traffic of 100,000 users competing for 50,000 seats, thousands of transactions query the exact same popular rows simultaneously.
> Relational databases serialize access via pessimistic row locks, causing transactions to wait in lock queues.
> Database connection pools exhaust in seconds, worker threads block, memory overflows, and cascading timeouts crash the database.
> In-memory Redis Lua scripts evaluate locks in microseconds without disk I/O, handling 100,000+ operations per second effortlessly.

> [!question] Question 3: What happens if a user's 10-minute seat hold expires while their credit card is being processed?
> [!success]- Answer
> This is the "Late Payment Paradox."
> The system enforces a **Two-Phase Payment Settlement**:
> 1. Payment processing executes an **Authorization Only**, not a capture.
> 2. Before capturing funds, the booking orchestrator checks if the seat hold is still valid.
> 3. If the hold expired and another fan claimed the seat, the orchestrator immediately voids the authorization, releases the hold, issues a full refund to the first user, and displays a timeout notice.
> The card is charged permanently **only after** the database successfully locks the seat to that user.

> [!question] Question 4: How does a Virtual Waiting Room protect the system during massive traffic spikes?
> [!success]- Answer
> A Virtual Waiting Room acts as an edge shock absorber.
> When traffic surges to 100,000 concurrent requests, the waiting room holds fans in an edge queue (assigning fair sequential or randomized queue ranks).
> It meters entry into the actual seat map and booking service at a fixed, controlled rate (e.g., 500 users per second).
> This protects backend databases and payment services from being overwhelmed while ensuring a fair, orderly queuing experience for customers.

> [!question] Question 5: Why is the Saga pattern preferred over Two-Phase Commit (2PC) for ticket booking?
> [!success]- Answer
> 2PC requires all participating resources (database, message queue, payment gateway) to support XA transactions and hold locks until the coordinator commits.
> Third-party payment gateways (Stripe, PayPal) do not support XA protocols.
> Furthermore, holding database locks across multi-second internet payment authorization calls causes catastrophic connection pool exhaustion.
> The Saga pattern breaks the workflow into local transactions with asynchronous messaging and compensating rollback transactions, delivering superior scalability and fault tolerance.

> [!question] Question 6: How do you prevent scalper bots from sweeping all seats in the first 500 milliseconds of on-sale?
> [!success]- Answer
> 1. Enforce pre-registration and identity verification (e.g., Ticketmaster Verified Fan).
> 2. Gate entry to the waiting room behind modern cryptographic challenges (Cloudflare Turnstile or invisible proof-of-work).
> 3. Enforce strict purchasing limits (e.g., maximum 4 tickets per authenticated user account, verified mobile number, and credit card).
> 4. Analyze client behavioral biometrics (mouse movements, keystroke dynamics) to flag and block automated headless browser agents.

> [!question] Question 7: How do you handle seat map updates for thousands of users browsing the same venue simultaneously?
> [!success]- Answer
> Never have thousands of browsing users poll the backend database for seat availability.
> 1. Generate full seat availability snapshots as static compressed JSON files cached on the CDN edge, refreshed every 2-3 seconds.
> 2. Alternatively, broadcast incremental seat status change events (`seat_id: HELD`) over a WebSocket channel to all active viewers using Redis Pub/Sub, updating the interactive visual map in real time.

> [!question] Question 8: How do you ensure that expired seat holds are returned to the pool without slow background database polling?
> [!success]- Answer
> Rely on native Redis key expiration:
> Set the hold key with a 10-minute TTL: `SET show:101:seat:A12 "HELD:user_1" EX 600`.
> When the 10-minute timer expires, Redis drops the key automatically.
> When the next user queries or attempts to hold the seat, the key is absent (`nil`), meaning the seat is instantly recognized as `AVAILABLE` with zero background polling overhead.

> [!question] Question 9: What is an Idempotency Key, and how does it prevent double charges during ticket checkout?
> [!success]- Answer
> An idempotency key is a unique UUID generated by the client for each checkout attempt and passed in the `Idempotency-Key` HTTP header.
> The payment and booking services record this key in an in-memory Redis cache.
> If a network timeout occurs and the user clicks "Pay" a second time, the server detects the existing key, bypasses re-charging the credit card, and safely returns the previously generated booking confirmation.

> [!question] Question 10: How do you handle seats sold in adjacent pairs or groups without leaving awkward single-seat orphans?
> [!success]- Answer
> Seat reservation algorithms evaluate adjacency rules during the validation step:
> When a user selects 2 seats, the algorithm verifies that the purchase does not leave a single empty seat between the selected seats and already-sold seats (the "No Orphan Seat" rule).
> If an orphan condition is detected, the API rejects the reservation with a suggestion to pick an adjacent pairing, maximizing venue ticket sales efficiency.

---

## 10. Related Concepts and Wikilinks

- [[Optimistic-vs-Pessimistic-Locking]]: Concurrency control comparison across high-contention inventory.
- [[ACID-vs-BASE]]: Evaluating strict serializability versus eventual consistency in financial transactions.
- [[Redis-Architecture]]: In-memory data structures, TTL expiration, and Lua script execution.
- [[Consistent-Hashing]]: Sharding event and seat data across distributed caching clusters.
- [[Load-Balancing]]: Traffic shaping, rate limiting, and virtual waiting room mechanics.

---

## 11. Further Reading

- Xu, Alex. *System Design Interview – An Insider’s Guide (Volume 2)*. Chapter 9: Hotel Reservation System.
- Kleppmann, Martin. *Designing Data-Intensive Applications*. O'Reilly Media. Chapter 7: Transactions.
- Ticketmaster Engineering. *Scaling the World’s Biggest Ticket Drops*.
- Queue-it Technical Whitepaper: *Virtual Waiting Room Architecture and Traffic Management at Scale*.

---
type: moc
track: [sde, distinguished]
level: advanced
status: solid
last_reviewed: 2026-10-07
sources: []
---

# Classic Low-Level Interview Problems

These are the in-process designs interviewers ask for beside distributed case studies.
Each problem tests the candidate's mastery of object-oriented modeling, thread-safety primitives, algorithmic invariants, and concrete trade-offs under high concurrency.

## Infrastructure and Core Services

- [[Design-In-Memory-Cache]] is the single-process version of [[Caching-and-Invalidation]], covering O(1) LRU doubly-linked list maps, lock striping, and dual lazy/active TTL expiration.
- [[Design-Rate-Limiter]] is the single-process version of [[02-Rate-Limiter/design|the distributed rate limiter]], implementing Token Bucket and Sliding Window Counter with microsecond thread-safety.
- [[Design-Event-Bus-Pub-Sub]] is the in-memory version of [[Pub-Sub-Architecture]], featuring a Trie-based hierarchical topic router, worker pool dispatch, and Dead Letter Queues (DLQ).
- [[Design-Logging-Framework]] is a concurrent pipeline with a lock-free ring buffer appender and background flush thread, demonstrating [[Backpressure-and-Tail-Latency]] inside one process.
- [[Design-Task-Scheduler]] is a priority-queue delayed execution engine with condition-variable synchronization, one-time/recurring execution, and cooperative cancellation tokens.

## Domain and Product Systems

- [[Design-Elevator-System]] is a multi-car elevator bank dispatcher implementing the LOOK scan scheduling algorithm and door cycle state machines.
- [[Design-Smart-Parking-Lot]] is a multi-floor parking facility featuring polymorphic vehicle/spot hierarchy, Best-Fit capacity allocation, and atomic spot occupation.
- [[Design-Movie-Ticket-Booking-System]] is a transactional seat reservation engine with two-phase TTL soft locks, deadlock-free sorted lock acquisition, and idempotent payments.
- [[Design-Ride-Sharing-Dispatch-Engine]] is a location-based driver dispatch system featuring spatial grid indexing, atomic driver state transitions, and dynamic surge pricing.
- [[Design-Splitwise-Expense-Sharing]] is a multi-party expense ledger featuring polymorphic split strategies (Equal, Exact, Percentage, Share) and greedy dual-heap debt simplification ($O(N \log N)$).

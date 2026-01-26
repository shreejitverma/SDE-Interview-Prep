# System Design Concepts Cheatsheet

## 1. Scalability
*   **Vertical Scaling (Scale Up):** Adding more power (CPU, RAM) to an existing machine.
    *   *Pros:* Simple.
    *   *Cons:* Hardware limits, Single Point of Failure (SPOF).
*   **Horizontal Scaling (Scale Out):** Adding more machines to the pool.
    *   *Pros:* Infinite scale, redundancy.
    *   *Cons:* Complex management (Load Balancing, Data Consistency).

## 2. Load Balancing
Distributes traffic across servers.
*   **Algorithms:** Round Robin, Least Connections, Consistent Hashing (for caches).
*   **Layer 4 (Transport):** TCP/UDP based.
*   **Layer 7 (Application):** HTTP based (URL, Cookies).

## 3. Database Concepts
*   **CAP Theorem:** You can only have 2 of 3: **C**onsistency, **A**vailability, **P**artition Tolerance.
    *   *RDBMS (MySQL/Postgres):* CA (usually CP/AP in clusters). ACID transactions.
    *   *NoSQL (Cassandra/DynamoDB):* AP. BASE (Basically Available, Soft state, Eventual consistency).
*   **Sharding:** Splitting data across DBs by a key (e.g., UserID).
*   **Replication:** Master-Slave (Read Scaling) vs Master-Master.

## 4. Caching
*   **Strategies:**
    *   *Cache-Aside:* App checks Cache -> Miss -> DB -> Update Cache.
    *   *Write-Through:* App updates Cache and DB simultaneously.
*   **Eviction:** LRU (Least Recently Used), LFU.
*   **Tech:** Redis, Memcached.

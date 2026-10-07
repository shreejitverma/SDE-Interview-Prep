---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# System Design Core Concepts and Foundations

## Map of Content

Distributed systems design requires navigating fundamental architectural trade-offs across consistency, availability, latency, partitioning, concurrency, and compute virtualization.
This module establishes the theoretical foundations and design patterns necessary to architect, analyze, and scale production systems.

```mermaid
graph TD
    SystemDesign["Distributed Systems Core Principles"]
    
    subgraph ScalabilityAndPartitioning["Scaling & Partitioning"]
        Scale["[[Vertical-vs-Horizontal-Scaling]]"]
        Part["[[Partitioning-and-Sharding]]"]
        Hash["[[Consistent-Hashing]]"]
        LB["[[Load-Balancing]]"]
    end
    
    subgraph ConsistencyAndConsensus["Consistency & Consensus Models"]
        CAP["[[CAP-Theorem-and-PACELC]]"]
        ACID["[[ACID-vs-BASE]]"]
        Models["[[Consistency-Models]]"]
        Locking["[[Optimistic-vs-Pessimistic-Locking]]"]
    end
    
    subgraph CommunicationAndConcurrency["Communication & Concurrency"]
        SyncAsync["[[Synchronous-vs-Asynchronous-Communication]]"]
        PubSub["[[Pub-Sub-Architecture]]"]
        CAS["[[Concurrency-Synchronization-and-CAS]]"]
        MapRed["[[MapReduce-Architecture]]"]
    end
    
    subgraph StorageAndComputeModels["Storage & Compute Paradigms"]
        RDBMSNoSQL["[[RDBMS-vs-NoSQL]]"]
        VMContainer["[[Virtual-Machines-vs-Containers]]"]
    end
    
    SystemDesign --> ScalabilityAndPartitioning
    SystemDesign --> ConsistencyAndConsensus
    SystemDesign --> CommunicationAndConcurrency
    SystemDesign --> StorageAndComputeModels
```

## Foundational Knowledge Areas

### 1. Scalability and Partitioning
- [[Vertical-vs-Horizontal-Scaling]]: Theoretical bounds (Amdahl's Law, Universal Scalability Law), stateful vs stateless scaling, and cost efficiency inflection points.
- [[Partitioning-and-Sharding]]: Horizontal partitioning strategies (Range, Hash, Directory-based), rebalancing algorithms, and cross-shard transaction coordination.
- [[Consistent-Hashing]]: Distributed hash rings, virtual nodes, Murmur3 hashing, and minimal key redistribution under cluster topology changes.
- [[Load-Balancing]]: Layer 4 transport vs Layer 7 application load balancing, health check algorithms, and session persistence.

### 2. Consistency, Transactions, and Consensus
- [[CAP-Theorem-and-PACELC]]: Brewer's conjecture, FLP impossibility, trade-offs between consistency and availability under network partitions, and PACELC latency bounds under normal operation.
- [[ACID-vs-BASE]]: Strict relational ACID guarantees compared to BASE (Basically Available, Soft state, Eventual consistency) distributed paradigms.
- [[Consistency-Models]]: The consistency hierarchy spanning Linearizability, Sequential Consistency, Causal Consistency, Read-Your-Writes, Monotonic Reads, and Eventual Consistency.
- [[Optimistic-vs-Pessimistic-Locking]]: Comparison of two-phase locking (2PL) versus optimistic concurrency control (OCC), MVCC versioning, and fencing tokens.

### 3. Communication, Concurrency, and Parallelism
- [[Synchronous-vs-Asynchronous-Communication]]: Tight temporal coupling vs loose message-driven decoupling, backpressure mechanisms, and circuit breakers.
- [[Pub-Sub-Architecture]]: Event-driven fan-out topologies, message broker architectures, subscriber filtering models, and at-least-once delivery semantics.
- [[Concurrency-Synchronization-and-CAS]]: Hardware-level atomic primitives (CMPXCHG, LL/SC), memory barriers, ABA problem mitigation, and lock-free data structures.
- [[MapReduce-Architecture]]: Split-Map-Shuffle-Sort-Reduce distributed computational model, data locality principles, and failure recovery.

### 4. Storage and Compute Paradigms
- [[RDBMS-vs-NoSQL]]: Detailed comparison of Relational, Key-Value, Document, Wide-Column, and Graph database storage structures.
- [[Virtual-Machines-vs-Containers]]: Hypervisor hardware virtualization vs Linux kernel isolation (Namespaces, cgroups v2, OverlayFS).

### 5. Architectural Navigation Hubs
- [[System-Design-Concept-Map]]: Interactive Mermaid map connecting theoretical concepts to real-world infrastructure implementations.
- [[Technology-Comparison-Matrix]]: Master reference matrix comparing 20 primary distributed databases, brokers, caches, and runtimes across consistency, partitioning, and storage engines.

## Study and Interview Roadmap

1. Begin by understanding how [[Vertical-vs-Horizontal-Scaling]] and [[Virtual-Machines-vs-Containers]] dictate system architecture boundaries.
2. Master the core theoretical trade-offs in [[CAP-Theorem-and-PACELC]], [[ACID-vs-BASE]], and [[Consistency-Models]].
3. Study data distribution mechanisms via [[Partitioning-and-Sharding]] and [[Consistent-Hashing]].
4. Learn concurrency and synchronization primitives in [[Optimistic-vs-Pessimistic-Locking]] and [[Concurrency-Synchronization-and-CAS]].
5. Review the [[Technology-Comparison-Matrix]] to prepare for high-level technology selection questions in Staff+ system design interviews.
6. Apply these theoretical concepts across real-world distributed architectures in the [[04-System-Design/02-Case-Studies/README|Case Studies Master Hub]] (e.g., [[01-URL-Shortener/design|URL Shortener]], [[03-Real-Time-Chat/design|Real-Time Chat]], [[06-Ride-Sharing-Service/design|Ride Sharing]], [[09-Ticket-Booking-System/design|Ticket Booking]], and [[10-E-Commerce-Flash-Sale/design|Flash Sale]]).

<!-- moc:start (generated by tools/build_mocs.py; edits inside are overwritten) -->
## Also in this folder

**Notes**

- [System Design Fundamentals and Architecture Cheatsheet](system_design_basics.md)

<!-- moc:end -->

---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Distributed Infrastructure and Technology Comparison Matrix

## Overview

Modern system design requires evaluating complex distributed systems across multi-dimensional trade-offs spanning consensus protocols, storage structures, replication models, and operational overhead.
This matrix provides an exhaustive, staff-level architectural comparison across all primary persistence engines, message brokers, caching tiers, and infrastructure platforms documented in this knowledge base.

## Comprehensive Technology Comparison Matrix

| Technology | Category | CAP / PACELC | Default Consistency | Consensus / Replication | Partitioning Strategy | Storage Engine on Disk | Primary Production Use Case | Canonical Trade-off / Failure Mode |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **[[MySQL-and-InnoDB]]** | Relational RDBMS | CA (Single-node) / PC/EC | Strong (Serializable / Repeatable Read) | Binlog Semi-Sync / Group Replication (Paxos) | Manual Application Sharding / Vitess | Clustered B+ Tree (.ibd) with Redo/Undo Logs | Core transactional OLTP, e-commerce, banking | Write amplification on 16KB pages; replication lag stalls |
| **[[PostgreSQL-Architecture]]** | Object-Relational | CA (Single-node) / PC/EC | Strong (Read Committed by default) | Streaming Physical WAL / Logical Replication | Declarative Range/Hash Partitioning / Citus | In-Heap Append-Only Tuples + External B-Trees | Complex relational schemas, GIS, pgvector embeddings | Table/Index bloat; 32-bit transaction ID wraparound |
| **[[CockroachDB-Distributed-SQL]]** | Distributed NewSQL | CP / PC/EC | Strict Serializable (External Consistency) | Multi-Raft per Range + Range Leases | Dynamic 64MB - 512MB Ordered Range Splits | Pebble (Log-Structured Merge Tree) | Global multi-region transactional SQL | Transaction contention restarts (40001); clock drift panics |
| **[[Apache-Cassandra]]** | Wide-Column NoSQL | AP / PA/EL | Tunable Eventual ($R+W>N$ for Strong) | Dynamo Peer-to-Peer Ring (Gossip + Merkle Trees) | Murmur3 Consistent Hashing with Virtual Nodes | LSM-Tree (CommitLog + MemTable + SSTables) | High-throughput write ingestion, time-series, IoT | Tombstone overwhelming exceptions; lack of ad-hoc joins |
| **[[MongoDB-and-Couchbase]] (MongoDB)** | Document Store | CP / PC/EC | Strong on Primary (Tunable Read/Write Concerns)| Raft-style Replica Set Elections via Oplog | Dynamic 64MB Chunks via mongos Routers | WiredTiger B-Tree Engine with In-Memory Cache | Rapid application prototyping, catalog documents | Unsharded scatter-gather queries; ticket queue stalls |
| **[[MongoDB-and-Couchbase]] (Couchbase)**| Document / Cache | CP / PA/EL | Tunable Read/Write (In-Memory default) | In-Memory Database Change Protocol (DCP) | Fixed 1,024 vBuckets via CRC32 Smart Client | Memory-First Engine + Magma/Couchstore LSM/B-tree| Low-latency profile caching, session management | Memory quota exhaustion (`TMPFAIL`); metadata ejection |
| **[[Redis-Architecture]]** | In-Memory Key-Value | AP / PA/EL | Eventual on Replicas (Single-thread linearizable)| Sentinel (Raft) / Cluster Gossip Protocol | 16,384 Hash Slots via CRC16 Hashing | In-Memory Dict / SkipLists; RDB snapshots & AOF | Ultra-low latency caching, leaderboards, rate limits | Single-thread command blocking (BigKeys); fork() COW memory spikes |
| **[[Memcached-Architecture]]** | In-Memory Cache | N/A (Volatile) | N/A (Volatile memory only) | None (Shared-nothing server nodes) | Client-Side Consistent Hashing (Ketama ring) | Slab Allocator (Pre-allocated 1MB pages) | Raw high-concurrency string/blob caching | Zero persistence; slab calcification eviction storms |
| **[[Elasticsearch-and-Apache-Solr]]** | Search & Analytics | AP / PA/EL | Near-Real-Time (NRT 1-second refresh) | Native Master-Node Raft Consensus (ES 7+) | Primary and Replica Shards via Murmur3 Hash | Apache Lucene Immutable Segments + Doc Values | Full-text search, logging analytics (ELK), observability| Heap exhaustion via unindexed FieldData; over-sharding |
| **[[Amazon-S3-and-Object-Storage]]** | Object Storage | CP / PC/EC | Strong Read-After-Write Consistency | Multi-AZ Reed-Solomon Erasure Coding | Automated Horizontal Prefix Partitioning | Flat Key-Value Namespace over Distributed Blob Fleet| Unstructured media, backups, data lakes, analytics | High first-byte latency (50ms); no in-place byte mutation |
| **[[Apache-ZooKeeper]]** | Coordination Service | CP / PC/EC | Sequential Consistency (Linearizable via sync)| ZAB (ZooKeeper Atomic Broadcast) Protocol | Monolithic In-Memory Hierarchical Tree | In-Memory DataTree + Disk WAL & Snapshots | Cluster metadata, leader elections, distributed locks | Herd effect watch storms; JVM stop-the-world session drops |
| **[[Apache-Kafka]]** | Event Streaming Log | CP / PC/EC | Strictly Ordered per Partition (At-Least-Once)| In-Sync Replicas (ISR) + KRaft Consensus | Hash Key Partitioner per Topic | Append-Only Disk Log Segments (.log / .index) | Event-driven microservices, metrics, stream ingestion | Consumer group rebalance storms; cold page cache thrashing |
| **[[RabbitMQ]]** | AMQP Message Broker | CP / PC/EC | FIFO per Queue (At-Least-Once via Confirms) | Quorum Queues via Raft Consensus Engine | Multiple Queues bound to Exchanges | Disk-backed Raft Write-Ahead Log + Segment Files | Complex task queuing, banking workflows, delayed jobs | Memory alarm publisher pauses; poison pill requeue loops |
| **[[ZeroMQ]]** | Brokerless Sockets | N/A (Transport) | Best-Effort / In-Memory Buffering | None (Peer-to-peer embedded socket library) | Application-defined socket topologies | Memory Queues (Lock-free ypipe queues) | High-frequency trading, inter-thread microsecond IPC | Zero persistence; slow joiner syndrome drops messages |
| **[[NGINX-Architecture]]** | Reverse Proxy / Web | N/A (Proxy) | N/A (Stateless HTTP/TCP router) | Master-Worker Process Model (Signal coordination)| Upstream Server Pools (Round-robin, ip_hash) | Epoll/Kqueue non-blocking socket state machines | Edge ingress, SSL termination, microcaching, static web| Ephemeral port exhaustion on upstream HTTP/1.0 connections |
| **[[HAProxy-Architecture]]** | Dedicated L4/L7 LB | N/A (Proxy) | N/A (Stateless stream router) | Multi-Threaded Engine (nbthread) + Master-Worker | Upstream Server Farms (leastconn, roundrobin) | In-Memory Stick Tables (Zero disk file interaction) | High-density Layer 4/Layer 7 traffic routing, DDoS guard | No static file serving; queue timeouts on backend maxconn |
| **[[Docker-and-Container-Runtimes]]** | Container Runtime | N/A (Compute) | N/A (Host OS process isolation) | OCI Standards (containerd + runc) | Linux Namespaces (PID, NET, MNT, USER, IPC) | OverlayFS Union Filesystem (lowerdir / upperdir) | Application packaging, environment consistency | Soft multi-tenancy; kernel sharing prevents hard boundary |
| **[[Kubernetes-Architecture]]** | Container Orchestrator| CP / PC/EC | Declarative Desired State (Optimistic locking)| Control Plane Controllers + etcd Raft Storage | Namespaces, Nodes, Pod Topology Spread | etcd Key-Value Database (/registry/...) | Enterprise container management, autoscaling, GitOps | Control plane etcd disk saturation; CrashLoopBackOff loops |
| **[[Hadoop-and-HDFS]]** | Distributed Storage | CP / PC/EC | Strongly Consistent (Single Active NameNode) | Active/Standby NameNode via QJM Paxos + ZooKeeper| 128MB Disk Block Distribution across Racks | Local Linux ext4/xfs filesystems per DataNode | Large-scale batch MapReduce pipelines, cold archives | NameNode heap exhaustion from millions of small files |
| **[[Apache-Spark]]** | In-Memory Compute | N/A (Compute) | Deterministic Lineage DAG (Lazy Evaluation) | Dynamic Resource Allocation via Cluster Managers | In-Memory Partitions across Worker Executors | Tungsten Off-Heap Memory via sun.misc.Unsafe | Large-scale SQL, MLlib training, streaming analytics | Data skew shuffle stragglers; executor OOM errors |

## Architectural Trade-off Cheat Sheet

### When to Choose Storage Engines
- **Choose [[MySQL-and-InnoDB]]** when transactions require standard ACID compliance, existing developer familiarity, predictable clustered B+ tree indexing, and stable row-level replication.
- **Choose [[PostgreSQL-Architecture]]** when workloads benefit from rich data types (JSONB, Arrays), extensions (`pgvector`, PostGIS), complex query planning, and robust full-text search.
- **Choose [[CockroachDB-Distributed-SQL]]** when an application requires relational SQL across globally distributed regions with automated survivability and strict serializable consistency.
- **Choose [[Apache-Cassandra]]** when write ingestion volume is massive (hundreds of thousands of writes/sec), queries strictly follow known partition keys, and downtime is completely unacceptable.
- **Choose [[MongoDB-and-Couchbase]]** when document schemas evolve rapidly, data is naturally hierarchical, and low-latency document retrieval is required.
- **Choose [[Redis-Architecture]]** when sub-millisecond in-memory data structures (sets, sorted sets, hashes, bitmaps) are needed for caching, counters, or session stores.
- **Choose [[Amazon-S3-and-Object-Storage]]** when storing immutable files, backups, or raw data lake assets at scale with 11 9's durability at minimal storage cost.

### When to Choose Messaging Engines
- **Choose [[Apache-Kafka]]** when event streaming requires high throughput, replayable commit logs, event sourcing, or decoupled data pipelines feeding multiple independent downstream consumers.
- **Choose [[RabbitMQ]]** when workloads demand complex message routing (topics, wildcards, headers), granular per-message acknowledgments, priority queues, and transient task distribution.
- **Choose [[ZeroMQ]]** when microsecond latency is mandatory, zero infrastructure footprint is preferred, and the application handles delivery guarantees and peer discovery.

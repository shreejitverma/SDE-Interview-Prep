---
type: concept
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
---

# Top 20 System Design Interview Questions and Architectural Mappings

This curated index maps the top 20 industry-standard system design interview problems directly to their architectural deep-dives, concepts, and case studies within this vault.
Each problem explores distinct engineering challenges: read-to-write ratios, hot-spot sharding, distributed concurrency, consensus protocols, and geospatial indexing.

---

## 1. Social Media and Feeds
- **Problem**: Design a global social media feed platform (Twitter / Instagram).
- **Core Challenges**: Fan-out on write vs fan-out on read, celebrity influencer bypass, Redis sorted set timeline caching, cursor-based pagination, ML ranking pipelines.
- **Deep Dive**: [[05-Social-Media-Feed/design|Design a Social Media Feed (Twitter / Instagram)]].
- **Related Concepts**: [[Redis-Architecture]], [[Pub-Sub-Architecture]], [[Pagination-Strategies]].

## 2. High-Concurrency Ticketing and Reservation
- **Problem**: Design a seat-booking system (Ticketmaster / BookMyShow / IRCTC).
- **Core Challenges**: Zero double-booking, in-memory atomic seat holds with 10-minute TTL leases, Virtual Waiting Room traffic leveling, Saga distributed transactions.
- **Deep Dive**: [[09-Ticket-Booking-System/design|Design a Ticket Booking System (Ticketmaster / BookMyShow)]].
- **Related Concepts**: [[Optimistic-vs-Pessimistic-Locking]], [[ACID-vs-BASE]], [[Redis-Architecture]].

## 3. Real-Time Messaging and Chat
- **Problem**: Design a real-time messaging platform (WhatsApp / Discord / Slack).
- **Core Challenges**: Stateful WebSocket gateway management, Redis session registry, pub/sub inter-gateway routing, Cassandra/ScyllaDB wide-column message storage, presence heartbeats.
- **Deep Dive**: [[03-Real-Time-Chat/design|Design a Real-Time Chat System (WhatsApp / Discord / Slack)]].
- **Related Concepts**: [[Apache-Cassandra]], [[Pub-Sub-Architecture]], [[Load-Balancing]].

## 4. E-Commerce Flash Sale and Inventory
- **Problem**: Design a high-concurrency flash sale inventory system (Amazon Prime Day / Alibaba 11.11).
- **Core Challenges**: Zero overselling under 200,000 QPS, pre-warmed Redis inventory, multi-shard stock partitioning, asynchronous Kafka order queueing, payment timeout restocking.
- **Deep Dive**: [[10-E-Commerce-Flash-Sale/design|Design an E-Commerce Flash Sale System]].
- **Related Concepts**: [[Redis-Architecture]], [[Apache-Kafka]], [[Optimistic-vs-Pessimistic-Locking]].

## 5. Location-Based Services and Ride-Hailing
- **Problem**: Design a ride-sharing dispatch service (Uber / Lyft).
- **Core Challenges**: High-frequency driver GPS streaming (1.25M updates/sec), Uber H3 hexagonal spatial indexing, batch bipartite dispatch matching, localized dynamic surge pricing.
- **Deep Dive**: [[06-Ride-Sharing-Service/design|Design a Ride-Sharing Service (Uber / Lyft)]].
- **Related Concepts**: [[Load-Balancing]], [[Consistent-Hashing]], [[Redis-Architecture]].

## 6. Video Ingestion and Streaming
- **Problem**: Design an on-demand and live video streaming platform (YouTube / Netflix).
- **Core Challenges**: Resumable multipart uploads, parallel DAG transcoding (GOP splitting), Adaptive Bitrate Streaming (HLS / MPEG-DASH), multi-tier CDN edge caching, stream view counters.
- **Deep Dive**: [[08-Video-Streaming-Platform/design|Design a Video Streaming Platform (YouTube / Netflix)]].
- **Related Concepts**: [[Amazon-S3-and-Object-Storage]], [[Apache-Kafka]], [[Load-Balancing]].

## 7. URL Shortening and Redirects
- **Problem**: Design a scalable URL shortening service (TinyURL / Bitly).
- **Core Challenges**: Base62 encoding, Key Generation Service (KGS) pre-allocation, HTTP 301 vs 302 analytics trade-offs, cache stampede prevention.
- **Deep Dive**: [[01-URL-Shortener/design|Design a URL Shortener (TinyURL / Bitly)]].
- **Related Concepts**: [[Consistent-Hashing]], [[Redis-Architecture]], [[CAP-Theorem-and-PACELC]].

## 8. Distributed Rate Limiting
- **Problem**: Design an enterprise API rate limiter (Cloudflare / Stripe).
- **Core Challenges**: Sliding window counter algorithm, atomic Redis Lua scripts, multi-tier enforcement (Edge, Gateway, Service Mesh), fail-open vs fail-closed policies.
- **Deep Dive**: [[02-Rate-Limiter/design|Design a Distributed Rate Limiter]].
- **Related Concepts**: [[Redis-Architecture]], [[Concurrency-Synchronization-and-CAS]], [[Load-Balancing]].

## 9. Distributed Unique Identifier Generation
- **Problem**: Design a distributed unique ID generator (Twitter Snowflake).
- **Core Challenges**: 64-bit integer bit layout, millisecond timestamp ordering, clock skew and NTP backward jumps, ZooKeeper worker ID allocation.
- **Deep Dive**: [[04-Distributed-ID-Generator/design|Design a Distributed Unique ID Generator (Twitter Snowflake)]].
- **Related Concepts**: [[Concurrency-Synchronization-and-CAS]], [[Apache-ZooKeeper]], [[MySQL-and-InnoDB]].

## 10. Web Crawling and Indexing
- **Problem**: Design a distributed web crawler (Googlebot).
- **Core Challenges**: Mercator URL Frontier (Priority + Politeness queues), asynchronous in-memory DNS caching, Bloom filter seen checks, SimHash near-duplicate content elimination, crawler trap defenses.
- **Deep Dive**: [[07-Distributed-Web-Crawler/design|Design a Distributed Web Crawler (Googlebot)]].
- **Related Concepts**: [[Consistent-Hashing]], [[MapReduce-Architecture]], [[Amazon-S3-and-Object-Storage]].

## 11. Distributed Object Storage
- **Problem**: Design a high-durability object storage service (Amazon S3).
- **Core Challenges**: Erasure coding (Reed-Solomon), partitioned metadata tiers, multi-part upload, read-after-write consistency.
- **Deep Dive**: [[Amazon-S3-and-Object-Storage]].
- **Related Concepts**: [[Partitioning-and-Sharding]], [[Consistency-Models]].

## 12. Distributed Search Engine
- **Problem**: Design a full-text search and log analytics engine (Elasticsearch / Solr).
- **Core Challenges**: Inverted index data structures, segment merging, TF-IDF / BM25 relevance scoring, primary-replica shard routing.
- **Deep Dive**: [[Elasticsearch-and-Apache-Solr]].
- **Related Concepts**: [[Partitioning-and-Sharding]], [[Apache-ZooKeeper]].

## 13. Distributed Stream Processing Engine
- **Problem**: Design a distributed event log and pub/sub broker (Apache Kafka).
- **Core Challenges**: Append-only commit log, zero-copy OS transfers (`sendfile`), consumer group rebalancing, KRaft consensus.
- **Deep Dive**: [[Apache-Kafka]].
- **Related Concepts**: [[Pub-Sub-Architecture]], [[RabbitMQ]], [[ZeroMQ]].

## 14. Distributed SQL Database
- **Problem**: Design a globally distributed relational database (CockroachDB / Spanner).
- **Core Challenges**: Raft consensus per range, Hybrid Logical Clocks (HLC), multi-version concurrency control (MVCC), distributed ACID transactions.
- **Deep Dive**: [[CockroachDB-Distributed-SQL]].
- **Related Concepts**: [[ACID-vs-BASE]], [[Consistency-Models]], [[CAP-Theorem-and-PACELC]].

## 15. In-Memory Caching Cluster
- **Problem**: Design a distributed in-memory cache (Redis / Memcached).
- **Core Challenges**: Memory allocation (slab allocator vs jemalloc), single-threaded event loop vs multi-threaded I/O, cache invalidation patterns (Cache-Aside, Write-Through, Write-Behind).
- **Deep Dive**: [[Redis-Architecture]] and [[Memcached-Architecture]].
- **Related Concepts**: [[Consistent-Hashing]], [[Load-Balancing]].

## 16. Distributed Batch Computing Framework
- **Problem**: Design a distributed data processing engine (Hadoop MapReduce / Apache Spark).
- **Core Challenges**: Map, shuffle, and reduce pipelines, Resilient Distributed Datasets (RDDs), DAG optimization, lineage fault tolerance.
- **Deep Dive**: [[MapReduce-Architecture]], [[Hadoop-and-HDFS]], and [[Apache-Spark]].

## 17. API Gateway and Reverse Proxy
- **Problem**: Design an edge ingress traffic controller (NGINX / HAProxy / Envoy).
- **Core Challenges**: Event-driven asynchronous I/O (`epoll`/`kqueue`), Layer 4 vs Layer 7 load balancing, SSL termination, dynamic upstream health checking.
- **Deep Dive**: [[NGINX-Architecture]], [[HAProxy-Architecture]], and [[Load-Balancing]].

## 18. Container Orchestration Control Plane
- **Problem**: Design a distributed container scheduler (Kubernetes / Apache Mesos).
- **Core Challenges**: Declarative desired-state reconciliation loop, two-level resource scheduling, etcd consensus store, kubelet node agents.
- **Deep Dive**: [[Kubernetes-Architecture]], [[Apache-Mesos]], and [[Helm-Package-Manager]].

## 19. Distributed Coordination Service
- **Problem**: Design a distributed configuration and leader election coordinator (Apache ZooKeeper).
- **Core Challenges**: Zab atomic broadcast consensus, ephemeral sequential znodes, watch notifications, split-brain quorum guarantees.
- **Deep Dive**: [[Apache-ZooKeeper]].
- **Related Concepts**: [[CAP-Theorem-and-PACELC]], [[Consistency-Models]].

## 20. High-Performance API Protocols
- **Problem**: Design a low-latency microservice RPC framework.
- **Core Challenges**: Protocol Buffers binary framing, HTTP/2 multiplexing, bidirectional streaming, client-side load balancing.
- **Deep Dive**: [[gRPC-and-Protocol-Buffers]] and [[HTTP-Evolution-HTTP1-HTTP2-HTTP3]].
- **Related Concepts**: [[REST-APIs]], [[GraphQL]], [[API-Fundamentals]].

# Chapter 04 - The Vector Database Landscape

All product observations in this chapter are stated as of early 2026 and will rot; verify current capabilities before deciding.

## What you will master

- The actual axes on which vector stores differ, beneath the marketing.
- Working knowledge of the major options: pgvector, Qdrant, Weaviate, Milvus, Pinecone, Chroma, Turbopuffer, LanceDB, and Elasticsearch/OpenSearch dense vectors.
- A selection framework driven by scale, filtering, hybrid search, and operational burden.
- The "you probably just need pgvector" argument, made properly, including exactly where it stops being true.

## 1. What a vector database actually is

Strip the category name and a vector database is four things bolted together.
A storage engine for vectors plus payload (metadata and often the text itself).
One or more ANN indexes (almost always HNSW, sometimes IVF variants, DiskANN derivatives, or proprietary structures).
A query engine that combines similarity search with metadata filtering, and increasingly with lexical scoring for hybrid search.
And an operational wrapper: replication, persistence, multi-tenancy, auth, backups, and an API.

Chapter 03 covered the index internals; this chapter is about the wrapper, because in production the wrapper is what you live with.
The core insight of the whole chapter: ANN algorithms are commoditized (everyone has a competent HNSW), so products differentiate on filtering behavior, hybrid search, cost model, consistency, and ops burden - which is exactly the list most evaluations skip.

## 2. The selection axes

Scale: total vectors, dimension, and growth rate; this decides whether single-node RAM, quantization, disk-resident, or distributed architectures are in play (Chapter 03's triangle).
Filtering: every real workload filters (tenant, ACL, date, type); the questions are whether filters are applied pre/during/post ANN search, whether recall holds under selective filters, and whether the engine plans this automatically.
Hybrid search: is BM25 or another lexical scorer native, bolted on, or absent, and can the engine fuse results server-side (Chapter 05 explains why you want this).
Freshness and mutation: insert-to-searchable latency, delete handling, and update patterns; append-mostly analytics differ from CRUD-heavy applications.
Consistency and durability: is the store a system of record or a rebuildable cache; what happens on node loss mid-upsert.
Multi-tenancy: thousands of small isolated tenants is a different problem from one big corpus, and engines have very different answers (per-collection, per-partition, payload-filter isolation).
Cost model: RAM-resident engines price by memory; serverless engines price by storage plus request; the same workload can differ by an order of magnitude in cost across models.
Ops burden: who gets paged, what upgrades look like, and whether your team already runs the dependency (this is pgvector's trump card and Postgres teams should weight it heavily).
Ecosystem gravity: client libraries, framework integrations, hiring familiarity - real but secondary.

## 3. The players

### 3.1 pgvector

An open-source Postgres extension adding a `vector` type, exact search via operators, and HNSW and IVFFlat index types, with `halfvec` (fp16) and binary/sparse types in later releases.
Strengths: your vectors live in the same database as your application data, so joins, transactions, ACL checks, and vector search compose in one SQL query with one backup story and one operational surface.
Filtering is Postgres filtering: the planner combines B-tree predicates with the vector index, and iterative index scans (added in 0.8.x, 2024 era) mitigate the classic problem of a filter discarding most of an HNSW result set.
Weaknesses: single-node vertical scaling in stock Postgres, index build time and memory for large HNSW indexes, recall under heavily selective filters still needs care, and no native BM25 fusion (Postgres full-text search exists but ranking quality and fusion are on you).
Related work to know: pgvectorscale (Timescale) adds a DiskANN-style index and label-filtered search on top of Postgres, and several managed Postgres vendors ship tuned pgvector.
Order-of-magnitude comfort zone: up to a few million to low tens of millions of vectors with adequate RAM, which covers the vast majority of RAG systems ever deployed.

### 3.2 Qdrant

Open-source engine in Rust, also offered as managed cloud.
Strengths: filter-aware HNSW traversal is a first-class design goal (payload indexes participate in graph search rather than pre/post filtering), strong quantization support (scalar, product, binary) with oversample-and-rescore built in, good multi-tenancy ergonomics via payload partitioning, and a clean API.
Native sparse vectors enable BM25-style hybrid within one engine.
Weaknesses: another stateful service to run (or a vendor bill), and data lives outside your relational database, so joins and transactional consistency with application data are your problem.
A common and reasonable default when you have outgrown pgvector but want open source you can self-host.

### 3.3 Weaviate

Open-source engine in Go with managed cloud, HNSW-based, with a long-standing hybrid search story (BM25 plus dense with server-side fusion) and a module ecosystem that can call embedding and reranking models for you.
Strengths: hybrid search ergonomics, GraphQL and REST APIs, multi-tenancy support, built-in vectorization modules that reduce pipeline code.
Weaknesses: the module system couples your data layer to model providers if you let it, schema and API surface are heavier than Qdrant's, and resource usage at scale needs tuning attention.

### 3.4 Milvus

Open-source distributed vector database (Linux Foundation project, commercially backed by Zilliz), designed from the start for billion-scale: separated storage and compute, log-based ingestion, segment-based indexes (HNSW, IVF variants, DiskANN), GPU index options, and partitioning.
Strengths: genuine horizontal scale and index variety; if you have hundreds of millions to billions of vectors, it is one of the few open options built for that regime.
Weaknesses: architectural complexity is the price - a full deployment involves multiple components (proxies, coordinators, workers, object storage, and historically etcd/message queue dependencies; Milvus versions have been consolidating this), and running it well is real platform work.
Milvus Lite and single-binary modes exist for small scale, but choosing Milvus for a small corpus buys complexity you do not need.

### 3.5 Pinecone

The category-defining managed-only vector database.
Strengths: serverless operation with zero index tuning exposed, pay-for-what-you-use storage/read/write pricing (post-2023 serverless architecture), namespaces for multi-tenancy, metadata filtering, and sparse-dense hybrid support; nobody on your team gets paged for it.
Weaknesses: closed source and single-vendor lock-in, cost at high query volume or large scale must be modeled carefully against self-hosting, data egress and residency constraints apply, and you cannot inspect or tune the index internals when recall behaves oddly.
The rational customer is one who values ops elimination above cost control and openness.

### 3.6 Chroma

Open-source, developer-experience-first store that became the default in tutorials: pip install, in-process or client-server mode, simple API.
Strengths: fastest path from zero to working prototype; fine for local development, tests, and small single-node workloads.
Weaknesses: historically thin on distributed scale, advanced filtering, and quantization compared to the engines above; the company has been building out a distributed cloud offering, but the center of gravity remains small-to-medium workloads.
Treat it as a prototyping default that must re-justify itself before production.

### 3.7 Turbopuffer

A serverless search engine (closed source, managed) built on object storage (S3-class) with NVMe caching, offering both vector and full-text search.
Strengths: the object-storage-first architecture makes cold data extremely cheap, which is transformative for workloads with many mostly-idle namespaces (per-user or per-agent memories, thousands of small tenants); adopted publicly by several high-profile AI products (Cursor and Notion are commonly cited).
Weaknesses: cold-start latency on uncached namespaces is inherent to the design, it is a young single-vendor dependency, and hot high-QPS single-corpus workloads fit the architecture less naturally than many-cold-tenant workloads.
Its existence signals the broader 2024-2026 trend: search infrastructure migrating to disaggregated object storage, the same move analytics databases made earlier.

### 3.8 LanceDB

Open-source, embedded-first vector store built on the Lance columnar format (an Arrow-compatible format designed for ML data, with versioning and fast random access), with a managed cloud as well.
Strengths: embedded operation like "SQLite for vectors" - the database is files on disk or object storage, no server; strong fit for local pipelines, evaluation harnesses, and lakehouse-adjacent ML workflows where data versioning matters; disk-based indexes (IVF-PQ lineage) keep RAM needs low.
Weaknesses: the embedded model shifts concurrency and serving questions to you, and it is younger operationally than the server engines for high-QPS multi-writer serving.

### 3.9 Elasticsearch and OpenSearch dense vectors

Both added dense vector fields with HNSW (Lucene-based in Elasticsearch; Lucene, nmslib, or FAISS engines in OpenSearch) alongside their mature BM25, aggregation, and filtering machinery.
Strengths: if you already run them, you get credible hybrid search - the best lexical engine in the industry plus competent ANN - in infrastructure you already operate, with mature security, ILM, and observability; Lucene's quantization work (int8, binary) landed through 2024-2025.
Weaknesses: JVM-and-segment architecture makes vector memory management and index tuning less direct than purpose-built engines, historical vector query ergonomics were clunky (improving steadily), and standing up a new cluster just for vectors is heavy.
The decision is almost always "we already have it" versus "we would have to adopt it"; the former is strong, the latter rarely is.

### 3.10 Also in the room

FAISS is a library, not a database: unmatched for index experimentation and batch jobs, but persistence, serving, and filtering are on you.
Redis, MongoDB Atlas, ClickHouse, DuckDB, and SQLite (sqlite-vec) all grew vector capabilities, reinforcing the structural point: vector search is becoming a feature of general databases, not only a product category.

## 4. The "you probably just need pgvector" argument

The argument, made honestly.
Most RAG deployments hold thousands to a few million chunks - company wikis, product docs, support archives - which at d=1,024 float32 is at most a few GB of vectors, comfortably one Postgres instance.
At that scale, per Chapter 03, even modest HNSW settings deliver high recall at millisecond latencies, and brute force is often acceptable, so ANN excellence is not a differentiator.
Meanwhile the things that actually hurt - tenant isolation, ACL joins, transactional consistency between documents and their chunks, backups, migrations, monitoring - are things Postgres has done for decades and your team already operates.
A dedicated vector database adds a second stateful system, a second consistency domain (index drift versus source-of-truth), a second backup and upgrade story, and a network hop, all to solve a scale problem you may not have.
Boring-technology reasoning applies: every new stateful dependency spends limited innovation tokens.

Where the argument stops being true, concretely.
Scale: past low tens of millions of vectors, index build times, RAM economics, and single-node limits make Postgres the wrong shape; distributed (Milvus), quantization-forward (Qdrant), or serverless (Pinecone, Turbopuffer) designs win.
Hybrid search quality: if BM25-plus-dense fusion with tuned lexical ranking is core to product quality, engines with native hybrid (Weaviate, Qdrant, Elasticsearch/OpenSearch, Vespa) beat hand-rolled fusion over Postgres FTS.
Extreme multi-tenancy with idle tenants: thousands of cold namespaces price better on object-storage architectures (Turbopuffer) than on always-resident RAM.
Write-heavy churn at scale: constant re-indexing stresses vacuum, index maintenance, and replication in ways purpose-built engines absorb more gracefully.
Latency SLOs under high QPS with filters: purpose-built filter-aware traversal (Qdrant) or heavily provisioned managed services can hold p99s that a shared application database under mixed load cannot.
Team shape: if nobody on the team runs Postgres either, "just use pgvector" quietly becomes "also adopt Postgres", and a managed vector service may genuinely be less total burden.

The synthesis: start with pgvector when you already run Postgres and your scale is within an order of magnitude of a few million vectors; write down the exit criteria (vector count, p99 latency, recall under filters, re-index time) and migrate deliberately when a criterion trips.
The failure mode to avoid is not choosing pgvector; it is choosing a distributed vector database for a 200,000-chunk corpus and spending a quarter operating it.

## 5. Evaluation method: how to run a credible bake-off

Vendor benchmarks are marketing; run your own, small but honest.
Use your embeddings on your corpus, not standard datasets, because index behavior depends on the geometry of your vectors.
Load realistic metadata and test at your real filter selectivities, including the nasty ones (0.1 percent selectivity tenant filters).
Measure recall against brute-force ground truth at your k, latency at p50/p99 under concurrent load, insert-to-searchable lag, and behavior during index rebuild.
Price the whole thing: instance or service cost at target scale, plus the engineering time of operating it, stated in the same units.
Then weight the axes from Section 2 for your workload before looking at any number, so the numbers cannot seduce you into optimizing an axis you do not care about.

## Exercises

1. Take a real corpus of at least 100,000 chunks and stand up pgvector and one dedicated engine (Qdrant or Weaviate).
   Implement identical top-10 retrieval with a tenant filter in both, and measure recall against brute force, p50/p99 latency under 32 concurrent clients, and insert-to-searchable lag.
2. Write the exit-criteria document for a pgvector deployment: specific thresholds on vector count, latency, recall under filters, and re-index time that would trigger migration, and the migration plan (dual-write, backfill, cutover, rollback).
3. Model costs for 50 million vectors at d=1,024 with 100 QPS across: self-hosted Qdrant with int8 quantization, Pinecone serverless, and Elasticsearch on existing infrastructure.
   Use current published pricing and state every assumption; the deliverable is the model, not the (perishable) numbers.
4. Design storage for an agent product with one million users, each owning a few thousand memory vectors, queried only when that user is active.
   Compare payload-filtered single-collection, collection-per-tenant, and object-storage-namespace designs; identify where each breaks.
5. Reproduce a filtered-recall pathology on any HNSW-based engine: measure recall at 50 percent, 5 percent, and 0.1 percent filter selectivity, and read the engine's documentation to explain its mitigation strategy.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Name the four components every vector database bolts together, and explain why filtering and hybrid search, not ANN quality, are the usual differentiators.
- For each of pgvector, Qdrant, Weaviate, Milvus, Pinecone, Chroma, Turbopuffer, LanceDB, and Elasticsearch, state its architectural bet in one sentence and the workload where that bet loses.
- Make the pgvector-first argument to a skeptical architect, including its five concrete failure thresholds.
- Explain why object-storage-based search engines price idle multi-tenant workloads differently, and what latency property they trade away.
- Design a bake-off for your own workload that a vendor's benchmark page could not have answered.

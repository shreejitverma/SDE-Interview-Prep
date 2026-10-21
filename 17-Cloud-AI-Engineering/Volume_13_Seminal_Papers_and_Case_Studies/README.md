---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 13 - Seminal Papers and Case Studies

Volume 13 - Seminal Papers and Case Studies in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [Dynamo (2007) and DynamoDB (2022 USENIX ATC)](Chapter_01_Dynamo_and_DynamoDB.md) - Evolution from consistent hashing and sloppy quorums to Paxos log replicas, partition splits, and disaggregated storage.
2. [Amazon Aurora (2017 SIGMOD)](Chapter_02_Amazon_Aurora.md) - Decoupling database compute from storage, 4-of-6 quorum slice writes, and asynchronous redo log processing.
3. [Physalia: Millions of Distributed Databases (2020 NSDI)](Chapter_03_Physalia_EBS_Control_Plane.md) - Million-node EBS control plane, Paxos-based cluster compartmentalization, and blast radius containment.
4. [Firecracker: Lightweight Virtualization (2020 NSDI)](Chapter_04_Firecracker_MicroVM.md) - Minimal device virtualization in Rust, 5-millisecond startup, Linux KVM integration, and memory overcommit.
5. [Validating Amazon S3 ShardStore with Formal Methods (2021 SOSP)](Chapter_05_Amazon_S3_ShardStore_Formal_Methods.md) - Property-based testing, crash-consistency formal verification, and defect elimination in core storage engines.
6. [Google Spanner: Globally-Distributed Database (2012 OSDI)](Chapter_06_Google_Spanner.md) - TrueTime atomic and GPS clocks, external consistency, wait-for-commit protocol, and Paxos consensus groups.
7. [Google Borg (2015 EuroSys) and Omega (2013 EuroSys)](Chapter_07_Google_Borg_and_Omega.md) - Large-scale cluster management, shared-state transaction scheduling, priority preemption, and container isolation.
8. [Google Bigtable (2006 OSDI)](Chapter_08_Google_Bigtable.md) - Sparse, distributed multidimensional sorted maps, SSTable and MemTable mechanics, Chubby locking, and compaction.
9. [Google File System (2003 SOSP) and Colossus](Chapter_09_GFS_and_Colossus.md) - Master bottleneck evolution to decentralized metadata, Reed-Solomon erasure coding, and real-time distributed file storage.
10. [Dremel: Interactive Analysis of Web-Scale Datasets (2010 VLDB)](Chapter_10_Dremel_Interactive_Analysis.md) - Nested columnar data representation, multi-level execution trees, and distributed shuffle layers powering BigQuery.
11. [Maglev: Fast and Reliable Network Load Balancer (2016 NSDI)](Chapter_11_Maglev_Network_Load_Balancer.md) - Kernel-bypass packet processing, consistent hashing lookup tables, and zero-downtime line-rate packet distribution.
12. [Andromeda: Cloud Network Virtualization (2018 NSDI)](Chapter_12_Andromeda_Network_Virtualization.md) - Software-Defined Networking architecture, OpenFlow packet virtualization, fast path flow offload, and sub-millisecond latencies.
13. [Jupiter: Multi-Terabit Datacenter Network (2015 SIGCOMM)](Chapter_13_Jupiter_Datacenter_Network.md) - Multi-stage Clos fabrics, optical circuit switching, wavelength division multiplexing, and centralized traffic routing.
14. [MillWheel (2013 VLDB) and The Dataflow Model (2015 VLDB)](Chapter_14_MillWheel_and_Dataflow_Model.md) - Stream processing primitives, low-watermark progress tracking, exactly-once delivery guarantees, and unified batch/stream execution.
15. [Google TPU Architecture Papers: TPU v1 to v5p (ISCA 2017-2023)](Chapter_15_Google_TPU_Architecture_Evolution.md) - Domain-specific architecture evolution, Matrix Multiply Units (MXUs), bfloat16 arithmetic, and optical circuit switching.

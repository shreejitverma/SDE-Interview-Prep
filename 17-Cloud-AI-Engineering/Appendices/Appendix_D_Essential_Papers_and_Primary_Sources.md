---
type: concept
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Appendix D: Essential Papers, Talks, and Primary Sources Reading List

This appendix is a working syllabus, not a bibliography.
Every entry says what to extract, which chapter the idea feeds, and where the paper has been overtaken by the product it describes.
Mechanism write-ups and interview drills follow the catalog.
Appendix C ([[17-Cloud-AI-Engineering/Appendices/Appendix_C_Limits_Quotas_and_Thresholds_Matrix|Appendix C]]) holds current limits, and Appendix E ([[17-Cloud-AI-Engineering/Appendices/Appendix_E_16_Week_Mastery_Study_Plan|Appendix E]]) schedules this list across 16 weeks.

## 0. Verification status of this appendix

Only the Maglev record below was re-fetched during authoring: title, NSDI'16, pages 523-535, published 16 March 2016 (ACM DL, as of 2026-10).
The web-search quota ran out before the other entries could be re-checked.
Every other venue, year, DOI, URL, and paper-derived number comes from the author's knowledge of the papers and is listed in the UNVERIFIED block at the end.
Paper contents do not change, but URLs and session titles do.
The numbers produced by the scripts in section 4 are my own arithmetic or simulation, and they are labeled as such.

## 1. How to read a systems paper for this curriculum

Answer these five questions in writing for every Tier 0 and Tier 1 paper.
Skip the benchmark section until you have answered them.

1. What is the invariant the system exists to protect, stated in one sentence (for example "write quorum and read quorum intersect", "commit timestamps respect real-time order")?
2. Which component is on the data path of every request, and which is only on the control path?
3. What is the unit of failure and the blast radius of that unit (a segment, a tablet, a cell, a rack, an AZ)?
4. What is the slowest-path behavior, meaning what the system does when a dependency is slow rather than dead?
5. What did the authors measure on hardware that no longer exists, and which conclusions survive a 10x change in network and flash speed?

Then write one "drift note": what the production service does today that the paper does not describe.
Section 5 gives the starting drift notes.

## 2. Reading tiers and time budget

| Tier | Purpose | Items | Budget |
| --- | --- | --- | --- |
| Tier 0 | Needed to pass an L6 loop on cloud storage, databases, and reliability | Dynamo, DynamoDB ATC 2022, Aurora 2017, Spanner 2012, Borg 2015, Maglev, Physalia, Firecracker, Tail at Scale, Static Stability, Shuffle Sharding, Metastable Failures in the Wild | About 30 hours including notes |
| Tier 1 | Needed to defend design choices at L6 and to teach them at L7 | Bigtable, GFS, Chubby, Dremel, Andromeda, Jupiter Rising, Dataflow Model, TPU v4, ZeRO, Megatron-LM scaling paper, vLLM, Orca, DistServe | About 40 hours |
| Tier 2 | Breadth, history, and depth on one specialty | Remaining entries | Choose two areas, about 20 hours each |

## 3. Annotated catalog

Rows are grouped by theme.
The last column links to the chapter that teaches the mechanism.
Dates are the publication year of the primary source.
All rows are tagged (publisher record, as of 2026-10) except where noted, and the section 0 caveat applies.

### 3.1 Replication, consensus, and storage engines (Amazon)

| Source | Venue and year | Why it is essential | Chapter |
| --- | --- | --- | --- |
| [Dynamo: Amazon's Highly Available Key-value Store](https://www.allthingsdistributed.com/files/amazon-dynamo-sosp2007.pdf) | SOSP 2007 | Origin of consistent hashing with virtual nodes, sloppy quorums, hinted handoff, vector clocks, and Merkle-tree anti-entropy. Read it as a catalog of trade-offs you must be able to say no to. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_01_Dynamo_and_DynamoDB\|Dynamo and DynamoDB]] |
| [Amazon DynamoDB: A Scalable, Predictably Performant, and Fully Managed NoSQL Database Service](https://www.usenix.org/conference/atc22/presentation/elhemali) | USENIX ATC 2022 | States explicitly that the service differs from Dynamo. Covers Multi-Paxos replication groups, log replicas, per-partition and global admission control, and the lessons behind adaptive capacity. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_10_Amazon_DynamoDB_Architecture_and_Partitioning\|DynamoDB Architecture]] |
| [Amazon Aurora: Design Considerations for High Throughput Cloud-Native Relational Databases](https://dl.acm.org/doi/10.1145/3035918.3056101) | SIGMOD 2017 | The log-is-the-database design. Introduces VCL, VDL, CPL, and the segment-level quorum model, and shows why network I/O (not disk) is the bottleneck. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_02_Amazon_Aurora\|Amazon Aurora]] |
| [Amazon Aurora: On Avoiding Distributed Consensus for I/Os, Commits, and Membership Changes](https://dl.acm.org/doi/10.1145/3183713.3196937) | SIGMOD 2018 | Formalizes why a single writer plus quorum acknowledgements avoids per-write consensus, and how membership changes use overlapping quorum sets. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_11_Amazon_Aurora_Log_Is_The_Database\|Aurora Log-Is-The-Database]] |
| [Millions of Tiny Databases (Physalia)](https://www.usenix.org/conference/nsdi20/presentation/brooker) | NSDI 2020 | A control-plane datastore for EBS built as many small Paxos-replicated cells placed near the data they describe. The reference for blast-radius engineering by placement. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_03_Physalia_EBS_Control_Plane\|Physalia]] |
| [Using Lightweight Formal Methods to Validate a Key-Value Storage Node in Amazon S3 (ShardStore)](https://dl.acm.org/doi/10.1145/3477132.3483540) | SOSP 2021 | Executable reference models checked with property-based testing and stateless model checking. Shows how a hyperscaler keeps crash-consistency bugs out of a log-structured store. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_05_Amazon_S3_ShardStore_Formal_Methods\|ShardStore]] |
| [Firecracker: Lightweight Virtualization for Serverless Applications](https://www.usenix.org/conference/nsdi20/presentation/agache) | NSDI 2020 | Minimal device model, jailer, and density targets behind Lambda and Fargate. Read the section on oversubscription and the sections on what was deliberately left out. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_04_Firecracker_MicroVM\|Firecracker]] |
| [A Cloud-Optimized Transport Protocol for Elastic and Scalable HPC (SRD)](https://doi.org/10.1109/MM.2020.3016891) | IEEE Micro 2020 | The design basis of EFA: multipath packet spraying, out-of-order delivery, and sender-side congestion control instead of in-order TCP or RoCE semantics. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI\|Interconnect Technologies]] |
| [The Security Design of the AWS Nitro System](https://docs.aws.amazon.com/whitepapers/latest/security-design-of-aws-nitro-system/security-design-of-aws-nitro-system.html) | AWS whitepaper | Primary statement of the "no operator access" model, the Nitro cards, and the attestation boundary. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_04_EC2_Nitro_Architecture_and_Graviton\|Nitro and Graviton]] |

### 3.2 Storage, compute, and data processing (Google)

| Source | Venue and year | Why it is essential | Chapter |
| --- | --- | --- | --- |
| [Spanner: Google's Globally-Distributed Database](https://www.usenix.org/conference/osdi12/technical-sessions/presentation/corbett) | OSDI 2012 | TrueTime, commit wait, and Paxos groups under two-phase commit. The cleanest example of buying consistency with bounded clock uncertainty. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_06_Google_Spanner\|Google Spanner]] |
| [Spanner, TrueTime and the CAP Theorem](https://research.google/pubs/spanner-truetime-and-the-cap-theorem/) | Google white paper, 2017 | Brewer's argument that Spanner is effectively CA by making partitions rare through private networking and operations, not by escaping CAP. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_10_Cloud_Spanner_TrueTime_and_External_Consistency\|Cloud Spanner]] |
| [Spanner: Becoming a SQL System](https://dl.acm.org/doi/10.1145/3035918.3056103) | SIGMOD 2017 | Distributed query execution, schema change, and the SQL layer that the 2012 paper does not cover. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_10_Cloud_Spanner_TrueTime_and_External_Consistency\|Cloud Spanner]] |
| [Large-scale cluster management at Google with Borg](https://dl.acm.org/doi/10.1145/2741948.2741964) | EuroSys 2015 | Cells, priority bands, quota, preemption, resource reclamation, and a Paxos-replicated Borgmaster. The conceptual parent of every GPU cluster scheduler. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_07_Google_Borg_and_Omega\|Borg and Omega]] |
| [Omega: flexible, scalable schedulers for large compute clusters](https://dl.acm.org/doi/10.1145/2465351.2465386) | EuroSys 2013 | Shared-state scheduling with optimistic concurrency. Read it to understand scheduler conflict rates and why gang scheduling is hard. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_07_Google_Borg_and_Omega\|Borg and Omega]] |
| [Borg, Omega, and Kubernetes](https://queue.acm.org/detail.cfm?id=2898444) | ACM Queue 2016 | The authors' own account of which Borg lessons were carried into Kubernetes and which were not. | [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_01_Kubernetes_Control_Loop_Internals\|Kubernetes Control Loop Internals]] |
| [Bigtable: A Distributed Storage System for Structured Data](https://dl.acm.org/doi/10.1145/1365815.1365816) | OSDI 2006, TOCS 2008 | SSTables, memtable, compaction, tablet location hierarchy, and Chubby as the coordination root. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_08_Google_Bigtable\|Google Bigtable]] |
| [The Google File System](https://dl.acm.org/doi/10.1145/945445.945450) | SOSP 2003 | Single-master metadata, lease-ordered mutations, and relaxed record-append semantics. Read it to learn what Colossus removed. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_09_GFS_and_Colossus\|GFS and Colossus]] |
| [A peek behind Colossus, Google's file system](https://cloud.google.com/blog/products/storage-data-transfer/a-peek-behind-colossus-googles-file-system) | Google Cloud blog, 2021 | Official description of Colossus metadata, curators, custodians, and D servers. The closest primary source on GFS's successor. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_09_Cloud_Storage_and_Colossus_Filesystem\|Cloud Storage and Colossus]] |
| [The Chubby Lock Service for Loosely-Coupled Distributed Systems](https://www.usenix.org/legacy/event/osdi06/tech/full_papers/burrows/burrows.pdf) | OSDI 2006 | Why Google chose a lock service over a library. Explains coarse-grained locks, sessions, and KeepAlive behavior that still shape ZooKeeper and etcd usage. Compare with [[04-System-Design/05-Data-Stores/Apache-ZooKeeper\|ZooKeeper]]. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_08_Google_Bigtable\|Google Bigtable]] |
| [Dremel: Interactive Analysis of Web-Scale Datasets](https://www.vldb.org/pvldb/vol3/R29.pdf) | VLDB 2010 | Repetition and definition levels for nested columnar storage and multi-level serving trees. The foundation of BigQuery. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_10_Dremel_Interactive_Analysis\|Dremel]] |
| [MillWheel: Fault-Tolerant Stream Processing at Internet Scale](https://www.vldb.org/pvldb/vol6/p1033-akidau.pdf) | VLDB 2013 | Low watermarks and exactly-once record processing through persisted state and deduplication. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_14_MillWheel_and_Dataflow_Model\|MillWheel and Dataflow Model]] |
| [The Dataflow Model](https://www.vldb.org/pvldb/vol8/p1792-Akidau.pdf) | VLDB 2015 | The what, where, when, how decomposition (transformations, windows, triggers, accumulation). Needed to reason about late data in Pub/Sub and Dataflow pipelines. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_14_Messaging_and_Streaming_Cloud_Pub_Sub_and_Dataflow\|Pub/Sub and Dataflow]] |
| [Zanzibar: Google's Consistent, Global Authorization System](https://www.usenix.org/conference/atc19/presentation/pang) | USENIX ATC 2019 | Authorization as a globally replicated tuple store with consistency tokens ("zookies"). A model for fine-grained access checks at AI-platform scale. | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_01_Zero_Trust_Architecture_in_Hyperscale_Clouds\|Zero Trust Architecture]] |

### 3.3 Networking and virtualization

| Source | Venue and year | Why it is essential | Chapter |
| --- | --- | --- | --- |
| [Maglev: A Fast and Reliable Software Network Load Balancer](https://dl.acm.org/doi/10.5555/2930611.2930645) | NSDI 2016, pp. 523-535 (ACM DL, as of 2026-10) | Consistent hashing lookup table plus per-machine connection tracking, running on commodity Linux servers with a kernel-bypass packet path. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_11_Maglev_Network_Load_Balancer\|Maglev]] |
| [Andromeda: Performance, Isolation, and Velocity at Scale in Cloud Network Virtualization](https://www.usenix.org/conference/nsdi18/presentation/dalton) | NSDI 2018 | The Hoverboard model: send first packets through gateways and offload hot flows to direct host-to-host paths. Also describes live migration of the virtual switch. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_12_Andromeda_Network_Virtualization\|Andromeda]] |
| [Jupiter Rising: A Decade of Clos Topologies and Centralized Control in Google's Datacenter Network](https://dl.acm.org/doi/10.1145/2829988.2787508) | SIGCOMM 2015 | Five generations of Clos fabric and the case for centralized routing control. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_13_Jupiter_Datacenter_Network\|Jupiter]] |
| [Jupiter Evolving: Transforming Google's Datacenter Network via Optical Circuit Switches and Software-Defined Networking](https://dl.acm.org/doi/10.1145/3544216.3544265) | SIGCOMM 2022 | Removes the spine layer with OCS and direct connect, and explains the operational model for topology engineering and traffic engineering. Read it after the 2015 paper. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_02_Datacenter_Networking_Fabrics\|Datacenter Networking Fabrics]] |

### 3.4 Reliability theory, failure analysis, and operations

| Source | Venue and year | Why it is essential | Chapter |
| --- | --- | --- | --- |
| [The Tail at Scale](https://dl.acm.org/doi/10.1145/2408776.2408794) | CACM 2013 | Fan-out turns a 1-in-100 slow response into the common case. Hedged and tied requests are the standard remedy, with a stated load cost. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_03_Resilient_Communication_Retries_Jitter_and_Circuit_Breakers\|Resilient Communication]] |
| [Metastable Failures in Distributed Systems](https://sigops.org/s/conferences/hotos/2021/papers/hotos21-s11-bronson.pdf) | HotOS 2021 | Defines the trigger and sustaining-effect model. The vocabulary for retry storms and cache-cold collapse. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_02_Load_Shedding_Backpressure_and_Admission_Control\|Load Shedding and Admission Control]] |
| [Metastable Failures in the Wild](https://www.usenix.org/conference/osdi22/presentation/huang-lexiang) | OSDI 2022 | Empirical study of public incidents. Shows that retry policy is the most common sustaining effect. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages\|Anatomy of Major Cloud Outages]] |
| [Gray Failure: The Achilles' Heel of Cloud-Scale Systems](https://www.microsoft.com/en-us/research/publication/gray-failure-the-achilles-heel-of-cloud-scale-systems/) | HotOS 2017 | Differential observability: the health checker and the application see different realities. Explains why GPU nodes that pass health checks still stall collectives. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale\|Fault Tolerance and Fleet Resiliency]] |
| [Cores that don't count](https://sigops.org/s/conferences/hotos/2021/papers/hotos21-s01-hochschild.pdf) | HotOS 2021 | Silent data corruption from defective CPU cores at fleet scale. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale\|Fault Tolerance and Fleet Resiliency]] |
| [Silent Data Corruptions at Scale](https://arxiv.org/abs/2102.11245) | arXiv 2021 (Meta) | Complements the Google paper with a second operator's detection methodology. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale\|Fault Tolerance and Fleet Resiliency]] |
| [Site Reliability Engineering](https://sre.google/sre-book/table-of-contents/) and [The SRE Workbook](https://sre.google/workbook/table-of-contents/) | Google, 2016 and 2018 | SLOs, burn-rate alerting, overload handling, cascading failures, and postmortem practice. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_01_SLIs_SLOs_and_Error_Budget_Engineering\|SLIs, SLOs, and Error Budgets]] |
| [Paxos Made Simple](https://lamport.azurewebsites.net/pubs/paxos-simple.pdf) and [In Search of an Understandable Consensus Algorithm (Raft)](https://raft.github.io/raft.pdf) | 2001 and USENIX ATC 2014 | The consensus baseline behind Physalia, Spanner, DynamoDB, and etcd. See also [[08-Distinguished-Engineering/02-Distributed-Systems-Internals/Raft-Consensus\|Raft Consensus]]. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_03_Physalia_EBS_Control_Plane\|Physalia]] |

### 3.5 Amazon Builders' Library (read in this order)

Index: [Amazon Builders' Library](https://aws.amazon.com/builders-library/) (AWS, as of 2026-10).
Article slugs follow the pattern `https://aws.amazon.com/builders-library/<slug>/`.

| Article slug | What to extract | Chapter |
| --- | --- | --- |
| `static-stability-using-availability-zones` | Pre-provision capacity so that AZ loss needs no control-plane action. Cost: roughly 50 percent headroom in a 2-AZ design and 33 percent in a 3-AZ design. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability\|Regions, Zones, and Static Stability]] |
| `workload-isolation-using-shuffle-sharding` | Combinatorial blast-radius reduction. See section 4.4. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_03_Cell_Based_Architecture_and_Shuffle_Sharding\|Cells and Shuffle Sharding]] |
| `reliability-and-constant-work` | Replace event-driven change propagation with fixed-rate full-state push so that load does not rise during failure. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_05_Constant_Work_and_Workload_Smoothing\|Constant Work]] |
| `timeouts-retries-and-backoff-with-jitter` | Retry amplification across layers and why jitter is mandatory. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_03_Resilient_Communication_Retries_Jitter_and_Circuit_Breakers\|Resilient Communication]] |
| `using-load-shedding-to-avoid-overload` | Shed on arrival, use goodput as the target metric, and keep the shed path cheaper than the serve path. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_02_Load_Shedding_Backpressure_and_Admission_Control\|Load Shedding]] |
| `avoiding-fallback-in-distributed-systems` | Fallback paths are untested modes that activate during the worst moment. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_04_Control_Plane_vs_Data_Plane_Separation\|Control Plane vs Data Plane Separation]] |
| `avoiding-insurmountable-queue-backlogs` | Backlog drain time is backlog divided by spare capacity, so spare capacity is what you provision for. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_02_Load_Shedding_Backpressure_and_Admission_Control\|Load Shedding]] |
| `fairness-in-multi-tenant-systems` | Per-tenant admission and quota design. | [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_01_Design_Multi_Tenant_LLM_Inference_Platform\|Multi-Tenant LLM Inference Platform]] |
| `implementing-health-checks` | Local versus dependency health, and fail-open behavior when all hosts fail the check. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_03_DNS_Anycast_and_Global_Traffic_Management\|DNS, Anycast, and Global Traffic Management]] |
| `automating-safe-hands-off-deployments` | Wave-based deployment, bake time, and automatic rollback. | [[17-Cloud-AI-Engineering/Volume_06_Infrastructure_as_Code_and_Delivery/Chapter_06_Progressive_Delivery_and_Deployment_Pipelines\|Progressive Delivery]] |

Also follow [Marc Brooker's blog](https://brooker.co.za/blog/) for short derivations on quorums, queueing, and Aurora DSQL (author blog, as of 2026-10).

### 3.6 Accelerators, training systems, and serving

| Source | Venue and year | Why it is essential | Chapter |
| --- | --- | --- | --- |
| [In-Datacenter Performance Analysis of a Tensor Processing Unit](https://arxiv.org/abs/1704.04760) | ISCA 2017 | Systolic matrix unit, roofline analysis, and why inference latency targets drove the design. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_15_Google_TPU_Architecture_Evolution\|TPU Architecture Evolution]] |
| [TPU v4: An Optically Reconfigurable Supercomputer for Machine Learning with Hardware Support for Embeddings](https://arxiv.org/abs/2304.01433) | ISCA 2023 | OCS-based 3D torus slices, fault-tolerant slice composition, and SparseCore. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI\|Interconnect Technologies]] |
| [Megatron-LM: Training Multi-Billion Parameter Language Models Using Model Parallelism](https://arxiv.org/abs/1909.08053) and [Efficient Large-Scale Language Model Training on GPU Clusters Using Megatron-LM](https://arxiv.org/abs/2104.04473) | 2019 and SC 2021 | Tensor parallel layout and the interleaved pipeline schedule. The second paper is the best source for composing TP, PP, and DP against a real topology. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_04_Distributed_Training_Parallelism_Strategies\|Distributed Training Parallelism]] |
| [ZeRO: Memory Optimizations Toward Training Trillion Parameter Models](https://arxiv.org/abs/1910.02054) | SC 2020 | Memory accounting for mixed-precision Adam and the three partitioning stages. See section 4.6. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_05_Memory_Optimization_ZeRO_FSDP_and_JAX_Sharding\|Memory Optimization]] |
| [GSPMD: General and Scalable Parallelization for ML Computation Graphs](https://arxiv.org/abs/2105.04663) | arXiv 2021 | The compiler-side sharding propagation behind JAX and XLA partitioning. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_05_Memory_Optimization_ZeRO_FSDP_and_JAX_Sharding\|Memory Optimization]] |
| [Pathways: Asynchronous Distributed Dataflow for ML](https://arxiv.org/abs/2203.12533) and [PaLM](https://arxiv.org/abs/2204.02311) | MLSys 2022 and 2022 | Single-controller dispatch across TPU pods and the multislice training run that used it. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_03_High_Performance_AI_Cluster_Architectures\|High-Performance AI Cluster Architectures]] |
| [The Llama 3 Herd of Models](https://arxiv.org/abs/2407.21783) | arXiv 2024 | Section on infrastructure reports interruption causes, effective training time, and straggler handling for a roughly 16K-GPU run. Use it as a public failure-rate data set. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale\|Fault Tolerance and Fleet Resiliency]] |
| [MegaScale: Scaling Large Language Model Training to More Than 10,000 GPUs](https://arxiv.org/abs/2402.15627) | NSDI 2024 | Diagnostics, heartbeat and stall detection, and network tuning at 10K-GPU scale. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale\|Fault Tolerance and Fleet Resiliency]] |
| [Orca: A Distributed Serving System for Transformer-Based Generative Models](https://www.usenix.org/conference/osdi22/presentation/yu) | OSDI 2022 | Iteration-level scheduling, the origin of continuous batching. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_04_Continuous_Batching_and_Scheduling_Algorithms\|Continuous Batching]] |
| [Efficient Memory Management for Large Language Model Serving with PagedAttention](https://arxiv.org/abs/2309.06180) | SOSP 2023 | Block-table virtual memory for KV cache and copy-on-write sharing. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_03_KV_Cache_Management_and_PagedAttention\|KV-Cache Management]] |
| [Taming Throughput-Latency Tradeoff in LLM Inference with Sarathi-Serve](https://arxiv.org/abs/2403.02310) | OSDI 2024 | Chunked prefill with stall-free batching, which is the colocated alternative to disaggregation. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_03_KV_Cache_Management_and_PagedAttention\|KV-Cache Management]] |
| [DistServe](https://arxiv.org/abs/2401.09670) and [Splitwise](https://arxiv.org/abs/2311.18677) | OSDI 2024 and ISCA 2024 | Prefill and decode on separate pools, with goodput under TTFT and TPOT SLOs as the objective. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_07_Prefill_and_Decode_Disaggregation\|Prefill and Decode Disaggregation]] |
| [Mooncake: A KVCache-centric Disaggregated Architecture for LLM Serving](https://arxiv.org/abs/2407.00079) | arXiv 2024, FAST 2025 | Treats the KV cache as a cluster-wide cached resource with a global scheduler, plus overload-oriented admission. | [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_01_Design_Multi_Tenant_LLM_Inference_Platform\|Multi-Tenant LLM Inference Platform]] |
| [Fast Inference from Transformers via Speculative Decoding](https://arxiv.org/abs/2211.17192) | ICML 2023 | The acceptance-rate math that decides whether a draft model pays for itself. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_05_Speculative_Decoding_and_Advanced_Decoding\|Speculative Decoding]] |
| [FlashAttention](https://arxiv.org/abs/2205.14135) | NeurIPS 2022 | IO-aware attention. A concrete example of optimizing for HBM traffic and not for FLOPs. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_01_LLM_Inference_Mechanics_Prefill_vs_Decode\|LLM Inference Mechanics]] |
| [Training Compute-Optimal Large Language Models (Chinchilla)](https://arxiv.org/abs/2203.15556) | NeurIPS 2022 | Needed for any capacity plan that starts from a token budget. | [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_05_GPU_and_TPU_Unit_Economics\|GPU and TPU Unit Economics]] |

### 3.7 Talks (titles only, search by title)

Session codes and hosting URLs change between years, so no codes or URLs are given.
All titles are recalled rather than re-fetched, so confirm each exists before assigning it (see the UNVERIFIED list).

| Talk | Venue | What to extract |
| --- | --- | --- |
| A day in the life of a billion packets | AWS re:Invent | How Nitro and the VPC data path move a packet and what state lives on the host. |
| Deep dive on the AWS Nitro System | AWS re:Invent | Offload card roles and the hypervisor's reduced scope. |
| A serverless journey: AWS Lambda under the hood | AWS re:Invent | Worker placement, microVM reuse, and cold-start composition. |
| Amazon Aurora storage demystified | AWS re:Invent | Segment repair, quorum membership, and cache warming on failover. |
| Dive deep on Amazon S3 | AWS re:Invent | Scale numbers, consistency design, and durability engineering. |
| Software Engineering Advice from Building Large-Scale Distributed Systems (Jeff Dean) | Stanford talk, 2007 | The origin of "numbers every engineer should know" and the back-of-envelope habit. |

### 3.8 Post-incident reviews to read as case studies

| Incident | Source | Reading focus | Chapter |
| --- | --- | --- | --- |
| AWS EBS, us-east-1, April 2011 | [AWS summary](https://aws.amazon.com/message/65648/) | Re-mirroring storm and control-plane saturation. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages\|Anatomy of Major Cloud Outages]] |
| AWS DynamoDB, September 2015 | [AWS summary](https://aws.amazon.com/message/5467D2/) | Metadata service load amplified by retries. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages\|Anatomy of Major Cloud Outages]] |
| AWS S3, February 2017 | [AWS summary](https://aws.amazon.com/message/41926/) | Operator command scope, and restart time of a large index subsystem. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages\|Anatomy of Major Cloud Outages]] |
| AWS us-east-1, December 2021 | [AWS summary](https://aws.amazon.com/message/12721/) | Internal network congestion and its effect on monitoring and operator tooling. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages\|Anatomy of Major Cloud Outages]] |
| AWS us-east-1, October 2025 | [AWS summary](https://aws.amazon.com/message/101925/) | DynamoDB endpoint DNS automation and the dependent services that failed after it. Build a dependency graph from the text. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability\|Regions, Zones, and Static Stability]] |
| Google Cloud, 12 June 2025 | [Google Cloud status history](https://status.cloud.google.com/) (find the incident for that date) | A globally replicated policy record, a missing feature flag, and a herd effect during recovery. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_04_Control_Plane_vs_Data_Plane_Separation\|Control Plane vs Data Plane Separation]] |

## 4. Mechanisms you will not find in a getting-started guide

### 4.1 Aurora: who tracks durability, and why recovery does not replay redo

The database instance is the single writer and allocates every LSN.
Each volume is divided into 10 GB segments, each stored as 6 copies, 2 per AZ.
Writes need 4 of 6 acknowledgements and reads would need 3 of 6, so read and write quorums overlap (3 + 4 > 6) and two writes cannot both succeed on disjoint sets (4 > 6/2).
This is the Aurora SIGMOD 2017 design, recalled from the paper (see section 0).

The instance does not run a consensus round per write.
Each segment tracks the highest LSN below which it holds every record, using a per-record backlink chain.
The instance tracks the following points.

- VCL is the highest LSN for which storage guarantees that all earlier records are available.
- CPL marks mini-transaction boundaries, for example the end of a B-tree page split.
- VDL is the highest CPL not above VCL, and it is the point the database treats as durable and consistent.

A transaction commits when VDL passes its commit LSN.
A worker is not blocked on this; commit completion is signaled asynchronously.
The instance also refuses to allocate an LSN more than the LSN allocation limit (the paper gives 10 million) ahead of VDL.
That limit is backpressure: a slow storage tier throttles the writer before it can accumulate an unbounded in-flight log.

Three consequences matter in interviews.

1. Write latency is the 4th-fastest of 6 acknowledgements. One slow node is invisible, and two slow nodes set your p50.
2. Normal reads go to a single segment known to be complete at the read point. Quorum reads occur only at recovery, where the instance asks for a read quorum per protection group, computes VDL, and truncates everything above it.
3. Recovery time does not depend on the volume of redo since the last checkpoint, because storage nodes apply redo continuously in the background.

Repair is what makes 4 of 6 safe.
The paper's argument is that a 10 GB segment can be re-replicated in about 10 seconds on a 10 Gbps link, so the window in which a second correlated loss could hurt is short.
Small segments are a reliability feature, not only a sharding convenience.

### 4.2 Spanner: commit wait converts clock uncertainty into latency

TrueTime returns an interval [earliest, latest] that contains absolute time.
A read-write transaction picks a commit timestamp s that is not less than TT.now().latest at commit start.
The coordinator then waits until TT.after(s) is true, meaning s is less than TT.now().earliest, before releasing locks and acknowledging the commit.
This is Spanner OSDI 2012, recalled from the paper.

The invariant is external consistency: if transaction T1 commits before T2 starts, then s1 < s2.
It holds because T2 cannot start until real time has passed s1, and T2's timestamp is at least its own TT.now().latest.
The paper reports that the uncertainty epsilon follows a sawtooth between roughly 1 and 7 ms, averaging about 4 ms, because the time masters are polled periodically and clock drift is bounded.
Expected commit wait is therefore at least on the order of twice the average epsilon, and it overlaps with Paxos replication, so it is often partly hidden.

The failure mode is the important part.
If the time masters degrade, epsilon grows, commit latency grows, and correctness is preserved.
Compare this with a design that trusts NTP offsets with no bound, where a clock fault silently violates ordering.
Spanner fails slow, not wrong.

### 4.3 Maglev: why two tables are needed and what the lookup table actually costs

A Maglev machine keeps a connection-tracking table keyed by the 5-tuple and a lookup table built by consistent hashing.
The router spreads packets across Maglev machines with ECMP, which is stateless and can reshuffle flows when the set of Maglev machines changes.
A reshuffled flow arrives at a machine with no tracking entry, so that machine falls back to the lookup table.
Because every Maglev machine builds the same table from the same backend set, the flow still reaches the same backend with high probability.

The table of size M (prime) is filled by letting each backend claim entries in turn along its own permutation.
Each permutation is defined by an offset and a skip derived from two hashes of the backend name.
The following script is my own implementation of that algorithm, used to measure disruption.

```python
import hashlib
def h(s, salt): return int(hashlib.sha256((salt + s).encode()).hexdigest(), 16)

def populate(backends, M):
    N = len(backends)
    off = [h(b, "o") % M for b in backends]
    skip = [h(b, "s") % (M - 1) + 1 for b in backends]
    nxt, entry, n = [0] * N, [-1] * M, 0
    while True:
        for i in range(N):
            c = (off[i] + nxt[i] * skip[i]) % M
            while entry[c] >= 0:
                nxt[i] += 1
                c = (off[i] + nxt[i] * skip[i]) % M
            entry[c] = i
            nxt[i] += 1
            n += 1
            if n == M:
                return entry
```

My simulation with 100 backends (author computation, 2026-10-21) gave these results.

| Table size M | Entries owned by each backend | Entries owned by the removed backend | Extra entries that moved among survivors after removing 1 backend |
| --- | --- | --- | --- |
| 65537 | 655 or 656 | 655 | 386 (0.59 percent of table) |
| 655373 | 6553 or 6554 | 6553 | 886 (0.14 percent of table) |

Load is balanced to within one entry, and the extra disruption is small but not zero.
Larger M lowers the extra disruption.
The cost is memory and slower table rebuilds.
Extra disruption only matters for flows that lack a tracking entry on the machine that receives them.

### 4.4 Shuffle sharding: the combinatorics and the assumption behind them

If a customer is assigned k of n workers, there are C(n, k) distinct shards.
With 8 workers and k = 2 there are 28 shards, so a poison-pill customer overlaps fully with 1 in 28 other customers.
The Builders' Library article describes Route 53 using 2048 virtual name servers with 4 per hosted zone.
C(2048, 4) is 730,862,190,080 (author computation), which matches the article's "about 730 billion" as I recall it.

The assumption that breaks is independence.
The math holds only if a failing request can hurt just the workers it reaches.
Clients that retry across all 4 assigned workers will walk the poison pill through the whole shard.
Shuffle sharding needs a client that is shard-aware and a retry policy that stops after the shard is exhausted.
Overlap is also not uniform: customers with partially overlapping shards share some fate, which is why the article pairs shuffle sharding with per-tenant rate limits.

### 4.5 DynamoDB: why predictability needed admission control

The ATC 2022 paper's central claim is predictable performance, not only scale.
Each partition is a replication group using Multi-Paxos, with a leader that serves writes and strongly consistent reads.
Log replicas hold only the write-ahead log and no B-tree.
When a storage replica fails, the group can add a log replica quickly to restore durability, and rebuild the full storage replica afterwards.
Admission control uses token buckets per partition plus a global admission control layer that tracks table-level consumption, so that adaptive capacity can move throughput between partitions without letting one table consume unbounded aggregate.
The residual limit is a single hot key: a key lives in one partition, so no amount of table-level capacity helps it.

### 4.6 Two back-of-envelope formulas worth deriving yourself

**KV transfer in disaggregated serving.**
KV bytes per token = 2 x layers x KV heads x head dimension x bytes per element.
For a Llama-3-70B-class shape (80 layers, 8 KV heads, head dimension 128, 16-bit elements, from the model's published configuration) this is 2 x 80 x 8 x 128 x 2 = 327,680 bytes, or 320 KiB per token.
An 8,192-token prompt is 2.5 GiB of KV (author computation).
Moving it at 50 GB/s effective takes about 54 ms, at 100 GB/s about 27 ms, and at 25 GB/s about 107 ms (author computation).
A 128K-token context is 40 GiB, which does not fit in any single-hop budget at those rates.

**Checkpoint interval (Young/Daly first-order).**
Optimal interval T = sqrt(2 x delta x MTBF), where delta is checkpoint stall time.
Expected overhead is about delta / T + T / (2 x MTBF), plus restart time R divided by MTBF.
Assume an MTBF of 3.1 hours (11,135 s).
That figure is the Llama 3 report's 54-day interruption count of 419 unexpected events, as I recall it, and it is not re-verified.

| Checkpoint stall delta | Optimal T | Overhead before restart cost |
| --- | --- | --- |
| 60 s (synchronous write) | About 1,156 s (19 min) | About 10.4 percent |
| 5 s (snapshot to host memory, upload in background) | About 334 s (5.6 min) | About 3.0 percent |

A 10-minute restart adds about 5.4 percent (600 / 11,135) regardless of the interval.
Because MTBF scales roughly as 1/N with GPU count, quadrupling the fleet quarters MTBF and halves the optimal interval.

## 5. Paper-to-product drift notes

| Paper says | What to check in current product docs | Risk of citing the paper blindly |
| --- | --- | --- |
| Dynamo: sloppy quorum, vector clocks, client-side reconciliation | DynamoDB uses Paxos replication groups with a leader, and its ATC 2022 paper says so | Proposing last-writer or vector-clock semantics for a service that offers leader-based consistency |
| GFS: single master, 64 MB chunks | Colossus has scaled-out metadata and different chunk and encoding behavior | Designing around a master bottleneck that no longer exists |
| Jupiter 2015: Clos with spine blocks | Jupiter 2022 removes the spine layer using OCS | Quoting stale bisection bandwidth and topology constraints |
| TPU v1: inference-only, 8-bit, no inter-chip fabric | TPU v4 and later are training-capable with ICI and OCS slices | Applying inference-era roofline numbers to training planning |
| Aurora 2017: MySQL and PostgreSQL compatible, single-writer volume | Product line now includes additional features that do not change the storage model described in the paper | Treating later product variants as architectural changes without reading release docs |
| Borg 2015: Borglet and Borgmaster | Kubernetes uses declarative controllers over an etcd-backed API server | Assuming Kubernetes implements Borg's quota and reclamation behavior |

For every row, the verification action is the same: find the current product documentation or the later paper and write down what changed.

## 6. Interview questions and model answers

### L5-1: Explain Aurora's 4-of-6 quorum and what happens during an AZ loss plus one more failure

Model answer.
There are 6 copies, 2 per AZ, with write quorum 4 and read quorum 3.
Losing an AZ leaves 4 copies, so writes and reads continue.
Losing an AZ plus one more copy leaves 3, so reads still reach quorum but writes cannot.
Because reads still work, the lost segments can be rebuilt from surviving copies and write quorum restored.
Segments are 10 GB, so the paper's repair time is on the order of 10 seconds on a 10 Gbps link, which keeps the exposure window small.
The trade-off is write availability for the affected protection group during the window, and extra storage and network cost for 6 copies.

Follow-up: Why does the writer not need a consensus protocol for each write, and what exactly does recovery do instead?

### L5-2: Why can Spanner not use NTP, and what does commit wait cost?

Model answer.
External consistency needs a bounded uncertainty interval, not a point estimate.
NTP offers no guaranteed bound that the application can use to reason about ordering.
TrueTime exposes [earliest, latest], and commit wait ensures the commit timestamp is in the past for every clock before the commit is visible.
Average epsilon is roughly 4 ms in the paper, so the wait is on the order of a few to ten milliseconds, partly overlapped with Paxos.
When uncertainty grows, latency rises and correctness holds.
The cost is specialized time infrastructure (GPS and atomic clocks) and a hard lower bound on read-write commit latency.

Follow-up: How would a system using hybrid logical clocks and uncertainty restarts differ in failure behavior and in tail latency?

### L6-1: Why did DynamoDB abandon Dynamo's design, and what did it give up?

Model answer.
Dynamo optimized always-writable availability with sloppy quorums, hinted handoff, and vector clocks that push conflict resolution to clients.
Customers wanted predictable latency and simple semantics, including strongly consistent reads and conditional writes.
DynamoDB therefore uses a Multi-Paxos group per partition across AZs, so a leader orders writes, which gives conditional writes and consistent reads from the leader.
It adds log replicas so durability can be restored without waiting for a full storage-replica rebuild.
It adds per-partition token buckets plus global admission control so one tenant cannot consume a shared node and so latency stays predictable.
What it gave up: writes to a partition are unavailable for a short window during leader election, a hot key is capped by one partition, and conflicting concurrent writes can no longer be merged by the client.
A staff-level answer also states the operational reason: a leader-based group has a clean repair and membership story, while a ring with vector clocks accumulates reconciliation work during partitions.

Follow-up: A table has 10 partitions and one key receives 30 percent of traffic.
What actually limits it, and what do you change?

### L6-2: Describe how Maglev keeps connections stable when backends or Maglev machines change

Model answer.
Packets are spread over Maglev machines by router ECMP, which is stateless.
Each machine first consults a connection-tracking table keyed by 5-tuple.
On a miss it consults a consistent-hash lookup table of prime size M, built identically on every machine from the same backend set.
So when ECMP moves a flow to a different Maglev machine, it still reaches the same backend unless the backend set differs between machines at that moment.
In my simulation, removing 1 of 100 backends moved about 0.59 percent of a 65,537-entry table beyond the entries owned by the removed backend, and about 0.14 percent with M = 655,373.
Failure modes: backend-set skew between machines during a rollout misroutes flows without tracking entries, tracking entries for a dead backend must be expired, and a very small M raises imbalance.
Larger M costs memory and table rebuild time.

Follow-up: You have a 0.1 percent connection-break budget per backend change.
What do you tune, and how do you measure it in production?

### L6-3: Design an LLM gateway so that it cannot enter a metastable state

Model answer.
Name the trigger and the sustaining effect.
Triggers are a deploy that empties prefix caches, loss of Spot GPUs, or a traffic spike.
Sustaining effects are client retries that re-run prefill, queue growth beyond what clients will wait for (so served work is wasted), and KV-cache preemption that recomputes work.
Controls:

1. Admit by estimated token cost against KV and compute capacity, not by request count, because a request with 100K prompt tokens costs orders of magnitude more than a 200-token one.
2. Bound queue age, not just depth, and drop from the head of the queue once age exceeds the client deadline so the system never serves requests nobody is waiting for.
3. Retry budget of about 10 percent of first attempts, full jitter, and deadline propagation, so aggregate retry load is capped.
4. Shed by priority tier and keep the shed path cheaper than the serve path.
5. Prefix caching so that a retry hits cached prefill.
6. Static headroom, because GPU scale-out takes minutes (image pull plus weight load) and cannot rescue an overload in progress.

Recovery math: drain time is backlog / (capacity - arrival rate).
With capacity 100 req/s, arrivals at 90 req/s, and a 6,000-request backlog, drain takes 600 s, so you must shed to below capacity during the drain.
Track goodput (completed within SLO) and queue age, not raw throughput.

Follow-up: Retries come from a customer SDK you do not control.
How do you protect the platform, and what do you tell the customer?

### L7-1: When does prefill and decode disaggregation pay off, and when does it lose?

Model answer.
It pays off when prefill interference makes decode miss its inter-token SLO, so a colocated fleet must be overprovisioned to hold TPOT.
Prompts must be long enough that prefill compute dominates KV transfer time.
For a 70B-class model at 320 KiB per token, an 8K-token prompt moves 2.5 GiB, about 54 ms at 50 GB/s effective.
Streaming KV layer by layer overlaps transfer with the remaining prefill compute, so only the final layer's transfer is exposed.
It loses for short prompts, small models, and cases where chunked prefill (Sarathi-Serve) already meets TPOT.
It also loses when the KV transfer shares a fabric with training-style collectives or other tenants.
Operational costs: the pool ratio of prefill to decode workers must track traffic mix, which drifts, two autoscaling loops with different cold-start costs, a transfer path that adds a failure domain, and decode-pool KV capacity that becomes the new limiting resource.
The decision should come from a goodput measurement at your prompt and output distribution, as DistServe frames it, not from a general preference.

Follow-up: How would you size the prefill-to-decode ratio and detect that it has drifted?

### L7-2: Plan checkpointing and fault response for a 16K-GPU training run

Model answer.
Start with the failure rate.
The Llama 3 report is a public reference: hundreds of unexpected interruptions over about 54 days, mostly hardware.
Take an MTBF near 3 hours.
Young/Daly gives T = sqrt(2 x delta x MTBF).
Synchronous checkpoints with a 60 s stall give about 19 minutes between checkpoints and about 10 percent overhead.
Asynchronous snapshot to host memory with a 5 s stall gives about 5.6 minutes and about 3 percent overhead.
Restart time then dominates: 10 minutes per restart adds about 5 percent at this MTBF, so the investments are hot spares, fast reschedule onto a pre-validated node, local NVMe or peer-memory restore, and avoiding full job teardown.
Gray failures matter more than clean crashes.
A slow link or a throttled GPU stalls every collective, so you need straggler detection from per-rank step times and NCCL-level timing, and automated cordon and replace.
Silent data corruption needs periodic deterministic replay or cross-replica checksum comparison, because the training loss will not show it quickly.
MTBF scales as 1/N, so a 4x larger run halves the checkpoint interval and raises the value of every restart optimization.

Follow-up: Your checkpoint upload saturates the same NIC used for gradient all-reduce.
What do you change?

### L7-3: How do you decide whether a paper's mechanism applies to a design today?

Model answer.
Separate three layers.
The invariant, such as quorum intersection or commit-timestamp ordering, is durable.
The mechanism, such as GFS's single master or Dynamo's vector clocks, depends on the assumptions of its era.
The numbers, such as 2012 clock uncertainty or 2015 fabric bandwidth, are historical.
For each, find the later primary source: DynamoDB ATC 2022 for Dynamo, Colossus material for GFS, Jupiter Evolving for Jupiter Rising.
Then test the claim by failure injection or a small simulation, as I did for Maglev disruption.
State the delta in your design document.
Bar-raiser signal: the candidate cites a paper's number with its hardware context and says what they would re-measure.

Follow-up: A teammate justifies a sloppy-quorum store for a payments ledger by citing the Dynamo paper.
How do you respond?

## 7. Weekly use

[[17-Cloud-AI-Engineering/Appendices/Appendix_E_16_Week_Mastery_Study_Plan|Appendix E]] assigns Tier 0 papers to weeks 1 through 8 and Tier 1 papers to weeks 9 through 14.
Related vault material: [[15-Technical-Whitepapers/09-Seminal-Computer-Science-Papers/02-Distributed-Systems-and-Consensus|Distributed Systems and Consensus papers]], [[15-Technical-Whitepapers/09-Seminal-Computer-Science-Papers/03-Cloud-Infrastructure-and-Big-Data|Cloud Infrastructure and Big Data papers]], [[15-Technical-Whitepapers/09-Seminal-Computer-Science-Papers/04-Databases-and-Transaction-Processing|Databases and Transaction Processing papers]], [[08-Distinguished-Engineering/02-Distributed-Systems-Internals/Consistent-Hash-Ring|Consistent Hash Ring]], and [[04-System-Design/05-Data-Stores/Amazon-S3-and-Object-Storage|Amazon S3 and Object Storage]].
Chapter-level numbers for the interview arena are in [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_06_Cloud_AI_Numbers_Every_Staff_Engineer_Must_Know|Cloud AI Numbers Every Staff Engineer Must Know]].

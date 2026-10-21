---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 03 - GCP Deep Dive

Volume 03 - GCP Deep Dive in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [Resource Hierarchy and Organization Policies](Chapter_01_Resource_Hierarchy_and_Organization_Policies.md) - Org, folder, project tree inheritance, declarative org policy constraints, and programmatic policy evaluation.
2. [Cloud IAM: Roles, Policies, and Conditions](Chapter_02_Cloud_IAM_Roles_Policies_and_Conditions.md) - IAM policy evaluation order, Deny policy precedence, Common Expression Language conditions, and service account key governance.
3. [VPC Service Controls: Security Perimeters](Chapter_03_VPC_Service_Controls_Security_Perimeters.md) - Ingress and egress rules, access levels, dry-run evaluation, and preventing perimeter exfiltration across managed APIs.
4. [Global VPC, Andromeda SDN, and Network Tiers](Chapter_04_Global_VPC_Andromeda_SDN_and_Network_Tiers.md) - Andromeda OpenFlow packet virtualization, global subnets, Premium vs Standard network tiers, and Cloud NAT gateway scaling.
5. [Cloud Load Balancing: GFEs and Maglev](Chapter_05_Cloud_Load_Balancing_GFEs_and_Maglev.md) - Google Front End edge proxying, Maglev consistent hash lookup tables, connection tracking, and cross-region traffic steering.
6. [Compute Engine and Titanium Architecture](Chapter_06_Compute_Engine_and_Titanium_Architecture.md) - Titanium offload processors, machine families, vCPU NUMA affinity, and local NVMe SSD raw performance.
7. [GKE Deep Dive: Standard vs Autopilot](Chapter_07_GKE_Deep_Dive_Standard_vs_Autopilot.md) - Managed control plane high availability, Autopilot automated node provisioning, Datapath V2 eBPF networking, and workload shielding.
8. [Cloud Run and Serverless Container Execution](Chapter_08_Cloud_Run_and_Serverless_Container_Execution.md) - gVisor kernel sandboxing, concurrent request multiplexing, cold start optimization, and sidecar container lifecycles.
9. [Cloud Storage and Colossus Filesystem](Chapter_09_Cloud_Storage_and_Colossus_Filesystem.md) - Colossus file system architecture, Reed-Solomon erasure coding, bucket namespaces, and dual-region replication consistency.
10. [Cloud Spanner: TrueTime and External Consistency](Chapter_10_Cloud_Spanner_TrueTime_and_External_Consistency.md) - TrueTime atomic and GPS clock synchronization, wait-for-commit protocol, Paxos split groups, and two-phase commit.
11. [Cloud Bigtable: LSM Storage and Tablet Serving](Chapter_11_Cloud_Bigtable_LSM_Storage_and_Tablet_Serving.md) - SSTables on Colossus, MemTable flushes, tablet split mechanics, compactions, and row-key schema design.
12. [AlloyDB for PostgreSQL: Disaggregated Architecture](Chapter_12_AlloyDB_for_PostgreSQL_Disaggregated_Architecture.md) - Decoupled database engine and storage, log-processing storage service, vectorized analytical accelerator, and WAL replication.
13. [BigQuery: Dremel, Capacitor, and Slot Orchestration](Chapter_13_BigQuery_Dremel_Capacitor_and_Slot_Orchestration.md) - Capacitor columnar format, Dremel dynamic query trees, in-memory shuffle architecture, and slot reservations.
14. [Messaging and Streaming: Cloud Pub/Sub and Dataflow](Chapter_14_Messaging_and_Streaming_Cloud_Pub_Sub_and_Dataflow.md) - Partitionless topic scaling, subscriber lease management, Apache Beam streaming execution, watermarks, and triggers.
15. [Security and Confidential Computing: Cloud KMS](Chapter_15_Security_and_Confidential_Computing_Cloud_KMS.md) - Cloud KMS cryptographic key rings, CMEK integration, and Confidential VM memory encryption via AMD SEV and Intel TDX.
16. [Observability: Cloud Monitoring, Logging, and Trace](Chapter_16_Observability_Cloud_Monitoring_Logging_and_Trace.md) - Time-series metric ingestion, log sinks and BigQuery routing, Cloud Trace context propagation, and OpenTelemetry exporters.

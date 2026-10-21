---
type: concept
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Appendix B: Comprehensive AWS to GCP Mechanism Translation Map

This appendix maps AWS and Google Cloud primitives by mechanism, not by product name.
Use it after the AWS and GCP deep-dive volumes and before the limits matrix in [[17-Cloud-AI-Engineering/Appendices/Appendix_C_Limits_Quotas_and_Thresholds_Matrix|Appendix C]].
Terms are defined in [[17-Cloud-AI-Engineering/Appendices/Appendix_A_Glossary|Appendix A]].

## 0. How to read this map

Evidence tags used throughout:

- `[V]` means the number or behavior was checked against an official doc or whitepaper during this revision, and the source and date are given inline.
- `[P]` means the mechanism comes from a peer-reviewed paper or an official engineering paper, and is stable across product revisions.
- `[U]` means a volatile figure, name, or GA status that was not re-verified in this revision.
- Treat every `[U]` as "recheck before quoting".
- Service-name mappings and non-numeric semantic claims in the Section 2 tables are author knowledge and were not re-verified in this revision, unless tagged `[V]` or `[P]`.
- The footer lists every unverified item.

The five axes where the same-sounding service differs in semantics:

1. **Scope of the resource.** An AWS VPC is regional and its subnets are zonal. A GCP VPC is global and its subnets are regional.
2. **Grant algebra.** AWS policies intersect through ceilings (SCP, RCP, boundary, session policy). GCP allow policies union down the hierarchy, and only deny policies subtract.
3. **Isolation unit.** The AWS account is both an IAM trust root and a quota and billing boundary. The GCP project is a resource container. Principals and grants cross project boundaries with a single-sided grant.
4. **Lease semantics.** SQS visibility timeout and Pub/Sub ack deadline are both leases, but their redelivery and ordering knobs differ.
5. **Concurrency and consistency model.** Lambda uses one request per execution environment. Cloud Run multiplexes. Aurora DSQL uses optimistic concurrency. Spanner uses locks plus commit wait.

A five-step method for translating any design:

1. State the invariant you rely on (durability, ordering, isolation level, failure scope).
2. Find the isolation unit that enforces it on each cloud.
3. Find who must grant what, on which side of the boundary.
4. Find the scope of a bad config push: zone, region, or global.
5. Find the billing meter that scales with the mechanism (request, GB-s, port, IP, provisioned unit).

## 1. Translation tables by layer

Columns: concern, AWS mechanism, GCP mechanism, the difference that bites when you port, and the chapter to read.

### 1.1 Organization, identity, governance

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Hierarchy | Organization, OU, account | Organization, folder, project | Account is an IAM trust root. Project is a container, and principals from any project can be granted on it. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_06_Multi_Account_and_Multi_Project_Landing_Zones\|Landing zones]] |
| Permission ceiling | SCP (identity side), RCP (resource side) `[U]` | IAM deny policies, principal access boundary `[U]` | SCP and RCP filter and never grant. A GCP allow policy cannot be capped from below, only denied. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_01_IAM_Policy_Evaluation_Engine\|IAM engine]] |
| Resource-config guardrail | Config rules, SCP on API calls | Organization Policy constraints | Org Policy constrains the configuration state of a resource (for example allowed regions). It is not an identity permission. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_01_Resource_Hierarchy_and_Organization_Policies\|Org policies]] |
| Workload principal | IAM role via STS AssumeRole, short-lived session credentials | Service account, attached or impersonated, short-lived tokens | A GCP service account is both a principal and a resource that others can be granted on (impersonation). | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_02_Workload_Identity_Federation\|Workload identity]] |
| Credentials on a VM | IMDSv2: PUT for a session token, then GET with a header. Hop limit often 1, so bridged containers need 2. | Metadata server requiring the `Metadata-Flavor: Google` header | Both are link-local HTTP services. Both are SSRF targets. | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_01_Zero_Trust_Architecture_in_Hyperscale_Clouds\|Zero trust]] |
| Conditions | Condition keys such as `aws:PrincipalOrgID` | IAM Conditions in CEL on a binding | Different expression languages. Attribute availability differs per service. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_02_Cloud_IAM_Roles_Policies_and_Conditions\|Cloud IAM]] |
| Audit | CloudTrail. Management events on, data events opt-in. | Cloud Audit Logs. Admin Activity always on, Data Access mostly opt-in. `[U]` | Data-plane auditing is opt-in on both, so S3 object reads and GCS object reads are not logged by default. | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_07_Compliance_Auditing_and_Non_Repudiation\|Auditing]] |
| Quota scope | Per account, per region (Service Quotas) | Per project, global or per region (Cloud Quotas) | Account-per-cell multiplies AWS quota. Project-per-cell does the same on GCP. | [[17-Cloud-AI-Engineering/Appendices/Appendix_C_Limits_Quotas_and_Thresholds_Matrix\|Appendix C]] |

### 1.2 Compute and virtualization

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Virtualization stack | Nitro: KVM-based hypervisor plus Nitro Cards for VPC, EBS, and security | KVM-based hypervisor plus Titanium offload `[U]` | Both move networking and storage to dedicated silicon so guest CPU is not shared with the I/O path. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_04_EC2_Nitro_Architecture_and_Graviton\|Nitro]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_06_Compute_Engine_and_Titanium_Architecture\|Titanium]] |
| Sizing | Fixed instance sizes per family | Predefined and custom machine types with an arbitrary vCPU-to-memory ratio `[U]` | GCP custom shapes remove the "buy 2x RAM to get 1 more vCPU" tax. | [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_01_Compute_Translation\|Compute translation]] |
| Host maintenance | Instance retirement and scheduled events. Nitro live update for some host patches. `[U]` | Live migration by default on most series. GPU and TPU VMs usually terminate or restart instead. `[U]` | A GCE workload sees a blackout of milliseconds to seconds. An EC2 workload sees a scheduled event. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_01_Hypervisors_MicroVMs_and_Sandboxing\|Hypervisors]] |
| Serverless sandbox | Firecracker microVM `[P]` (NSDI 2020) | gVisor in Cloud Run gen1. Gen2 uses a full Linux kernel. `[U]` | Syscall-compatibility surface differs. gVisor intercepts syscalls, and the microVM path gives a full guest kernel. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_04_Firecracker_MicroVM\|Firecracker]] |
| Spot or preemptible | Spot: 2-minute interruption notice via IMDS and EventBridge `[U]` | Spot VM: about 30 seconds via ACPI G2 soft-off and metadata `[U]` | The checkpoint budget differs by 4x. Your SIGTERM handler must fit the shorter window. | [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_03_Spot_Economics_and_Interruption_Handling\|Spot]] |
| Co-location | Placement groups: cluster, partition, spread | Compact placement and spread placement policies | Cluster placement trades blast radius for latency on both. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_06_Physical_Failure_Domains_and_Blast_Radius\|Failure domains]] |
| Scarce capacity | On-Demand Capacity Reservations, Capacity Blocks for ML `[U]` | Reservations, future reservations, Dynamic Workload Scheduler `[U]` | Accelerator capacity is a reservation problem first and a price problem second. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale\|Fleet resiliency]] |
| Instance-group autoscaling | Auto Scaling groups, predictive scaling | Managed instance groups (zonal or regional) | A regional MIG spreads across zones natively. An ASG spreads via the subnets you list. | [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability\|Static stability]] |

### 1.3 Containers and Kubernetes

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Managed Kubernetes | EKS (AWS-managed control plane) | GKE Standard and Autopilot | Autopilot bills per pod resource requests. EKS and GKE Standard bill per node. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_07_Amazon_EKS_Control_Plane_and_Networking\|EKS]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_07_GKE_Deep_Dive_Standard_vs_Autopilot\|GKE]] |
| Pod IP source | VPC CNI: pod IPs are secondary IPs on ENIs from the VPC subnet | VPC-native clusters: pod IPs from a secondary alias range per subnet | See the pod IP math in Section 2.8. | [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_03_CNI_and_eBPF_with_Cilium\|CNI and Cilium]] |
| Node provisioning | Karpenter calls EC2 Fleet directly, bypassing the ASG | Cluster autoscaler, node auto-provisioning, Autopilot, custom compute classes `[U]` | Karpenter picks instance types per pending pod. A node pool autoscaler grows pre-declared shapes. | [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_04_Autoscaling_at_Scale_HPA_KEDA_and_Karpenter\|Autoscaling]] |
| Pod identity | IRSA, EKS Pod Identity | GKE Workload Identity Federation | All three are OIDC-token exchanges. Only the trust-policy shape differs. | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_02_Workload_Identity_Federation\|Workload identity]] |
| Container serverless | ECS on Fargate | Cloud Run services and jobs | Cloud Run multiplexes requests per instance. Fargate runs one task per microVM. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_06_Container_Services_ECS_and_Fargate\|ECS and Fargate]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_08_Cloud_Run_and_Serverless_Container_Execution\|Cloud Run]] |

### 1.4 Network and private connectivity

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Network scope | VPC is regional. Subnet is one AZ. | VPC is global. Subnet is regional. | A GCP VPC config object is a global blast radius. See Section 2.6. | [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_02_Networking_and_Load_Balancing_Translation\|Networking translation]] |
| Packet filtering | Security group (stateful, on the ENI, allow-only) plus NACL (stateless) | VPC firewall rules and hierarchical firewall policies (stateful, priority-ordered, allow and deny, target by tag or service account) | AWS SGs can reference other SGs. GCP rules target by network tag or service account identity. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_03_VPC_Networking_Internals\|VPC internals]] |
| Hub-and-spoke | Transit Gateway route domains | Network Connectivity Center, VPC peering | Peering is non-transitive on both. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_04_Global_VPC_Andromeda_SDN_and_Network_Tiers\|Global VPC]] |
| Private service access | PrivateLink interface endpoints (ENIs) and gateway endpoints for S3 and DynamoDB (route-table prefix-list targets) | Private Service Connect: a forwarding rule with an internal IP in the consumer VPC | Producer and consumer CIDRs may overlap on both because traffic is NATed. | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_06_Constructing_an_Impenetrable_Data_Perimeter\|Data perimeter]] |
| Egress NAT | NAT gateway: zonal, billed per hour plus per GB | Cloud NAT: regional config, implemented in the host dataplane (Andromeda) with no appliance VM `[P]` | Per-destination connection budget versus per-VM port blocks. See Section 2.1. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_12_Andromeda_Network_Virtualization\|Andromeda]] |
| Dedicated links | Direct Connect | Cloud Interconnect (Dedicated, Partner, Cross-Cloud) `[U]` | Cross-cloud managed links are a fast-moving area. Verify status before designing. | [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_07_Multi_Cloud_and_Hybrid_Architecture_Reality\|Multi-cloud reality]] |
| Subnet resize | Immutable. Add secondary VPC CIDRs. | Primary range expandable in place, never shrinkable `[U]` | GCP forgives under-sizing. AWS does not. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_02_Datacenter_Networking_Fabrics\|Fabrics]] |

### 1.5 Load balancing, edge, DNS

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| L7 balancer | ALB: regional, per-AZ nodes behind DNS | Global external Application LB: one anycast VIP, GFE proxies at the edge | AWS clients resolve a set of per-AZ IPs. GCP clients hit one IP that the edge fans out. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_05_Cloud_Load_Balancing_GFEs_and_Maglev\|GFE and Maglev]] |
| L4 balancer | NLB (flow-hash, preserves client IP) | Passthrough Network LB (Maglev-based, consistent hash with connection tracking) `[P]` | Maglev's consistent-hash lookup table gives minimal disruption when backends change (NSDI 2016). | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_11_Maglev_Network_Load_Balancer\|Maglev]] |
| Anycast front door | Global Accelerator (static anycast IPs onto the AWS backbone) | Premium tier: Google backbone from the nearest edge `[U]` | In GCP anycast is the default model for the global LB. In AWS it is a separate product. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_03_DNS_Anycast_and_Global_Traffic_Management\|DNS and anycast]] |
| Cross-zone behavior | ALB cross-zone is always on. NLB cross-zone is opt-in and incurs inter-AZ data charges. `[U]` | Backend selection is zone-aware within the balancing mode | AWS cross-zone settings change both failure behavior and bill. | [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_04_Network_Egress_Economics_and_Optimization\|Egress economics]] |
| CDN and WAF | CloudFront, AWS WAF, Shield | Cloud CDN, Cloud Armor | | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_04_Edge_Infrastructure_and_TLS_Termination\|Edge and TLS]] |

### 1.6 Storage

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Object store | S3 (strong read-after-write consistency) | Cloud Storage (strong consistency) | Request scaling differs. See Section 2.2. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_08_S3_Deep_Dive_Consistency_and_ShardStore\|S3]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_09_Cloud_Storage_and_Colossus_Filesystem\|GCS]] |
| Low-latency object tier | S3 Express One Zone: single-AZ directory buckets | Zonal and Rapid bucket variants `[U]` | You trade durability scope (one zone) for latency on both. Confirm GA status. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_06_High_Throughput_Checkpointing_to_Object_Storage\|Checkpointing]] |
| Cross-region durability | Cross-region replication between two buckets, async, with optional replication-time control (15-minute target) `[U]` | Dual-region and multi-region are a single bucket namespace. Turbo replication targets a 15-minute RPO. `[U]` | In S3 the two copies are separate buckets with separate names. In GCS dual-region it is one bucket. | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_04_Disaster_Recovery_Tiers_and_RTO_RPO_Engineering\|DR tiers]] |
| Block storage | EBS: Nitro-attached NVMe over the network, replicated within one AZ | Persistent Disk and Hyperdisk, with regional variants replicated synchronously across two zones `[U]` | EBS has no synchronous cross-AZ volume. You need application-level replication. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_09_Elastic_Block_Store_and_Network_Storage\|EBS]] |
| High-IOPS tier | io2 Block Express (up to 256,000 IOPS, 4,000 MiB/s) `[U]` | Hyperdisk Extreme (up to 350,000 IOPS) `[U]` | Both let you provision performance independently of size. | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_05_Storage_Media_and_Block_Storage_Foundations\|Storage media]] |
| Parallel file system | FSx for Lustre | Managed Lustre `[U]` | | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_07_High_Throughput_Data_Loaders_and_Storage_IO\|Data loaders]] |
| Metadata control plane | Physalia for EBS `[P]` (NSDI 2020) | Colossus with Bigtable-backed metadata `[P]` | Both avoid one global metadata master by partitioning. | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_03_Physalia_EBS_Control_Plane\|Physalia]], [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_09_GFS_and_Colossus\|Colossus]] |

### 1.7 Databases

| Workload shape | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Hash-partitioned KV | DynamoDB | Firestore (closest API shape), Bigtable (different model) | DynamoDB hash-partitions on the key. Bigtable stores sorted ranges and hotspots on monotonic keys. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_10_Amazon_DynamoDB_Architecture_and_Partitioning\|DynamoDB]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_11_Cloud_Bigtable_LSM_Storage_and_Tablet_Serving\|Bigtable]] |
| Cloud-native relational | Aurora: log is the database, 6 copies across 3 AZs `[P]` | AlloyDB: disaggregated log-processing storage `[U]` | Both ship redo to the storage tier instead of pages. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_11_Amazon_Aurora_Log_Is_The_Database\|Aurora]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_12_AlloyDB_for_PostgreSQL_Disaggregated_Architecture\|AlloyDB]] |
| Multi-region ACID | Aurora DSQL (optimistic concurrency) `[U]` for GA status | Spanner (locks plus TrueTime commit wait) `[P]` | Retry contract differs. See Section 2.5. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_12_Aurora_DSQL_Multi_Region_Active_Active_SQL\|Aurora DSQL]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_10_Cloud_Spanner_TrueTime_and_External_Consistency\|Spanner]] |
| Analytics warehouse | Redshift, Athena | BigQuery (Dremel execution trees, in-memory shuffle) `[P]` | BigQuery separates storage and compute with slots. Redshift is cluster-shaped (serverless variants exist). | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_13_BigQuery_Dremel_Capacitor_and_Slot_Orchestration\|BigQuery]] |
| Cache | ElastiCache | Memorystore | | [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_04_Storage_and_Database_Translation\|Storage and DB translation]] |

### 1.8 Messaging and streaming

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Queue | SQS standard (at-least-once) and FIFO (message group ID, 5-minute deduplication window `[U]`) | Pub/Sub pull subscription with ack deadline, optional ordering keys | Both are leases. SQS visibility timeout is per receive. Pub/Sub ack deadlines can be extended per message. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_13_Messaging_and_Streaming_SQS_SNS_EventBridge_and_MSK\|AWS messaging]] |
| Fan-out | SNS to SQS | Pub/Sub topic with many subscriptions | In Pub/Sub, fan-out is native. In AWS it is two services. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_14_Messaging_and_Streaming_Cloud_Pub_Sub_and_Dataflow\|Pub/Sub]] |
| Stream | Kinesis Data Streams (shards, you manage capacity) | Pub/Sub (no user-visible partitions) | Kinesis shard count is an operational knob. Pub/Sub hides it. | [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_05_Messaging_Streaming_and_Analytics_Translation\|Messaging translation]] |
| Kafka | MSK | Managed Service for Apache Kafka `[U]` | | Kafka note |
| Event routing | EventBridge (content-based rules) | Eventarc | | [[17-Cloud-AI-Engineering/Volume_15_Capstones_Production_Systems/Chapter_01_Capstone_Resilient_Serverless_Event_Pipeline\|Capstone 1]] |

### 1.9 Security and cryptography

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Key service | KMS keys (non-exportable), `GenerateDataKey` for envelope encryption, direct `Encrypt` limited to 4 KB `[U]` | Cloud KMS key rings, CMEK, Cloud HSM, External Key Manager `[U]` | Both are envelope designs. The KMS request rate is a shared quota that can throttle S3 or GCS with CMEK. | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_04_Encryption_Key_Hierarchies_and_Envelope_Encryption\|Key hierarchies]] |
| Per-object key amortization | S3 Bucket Keys cut per-object KMS calls `[U]` | Default encryption uses per-chunk DEKs wrapped by KEKs `[P]` | Same idea: reduce key-service calls on the data path. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_15_Security_and_Cryptography_KMS_and_Nitro_Enclaves\|KMS and Enclaves]] |
| Data perimeter | Identity, resource, and network policies composed: SCP, RCP, bucket policy, VPC endpoint policy | VPC Service Controls perimeter on API requests by project membership, with ingress and egress rules and access levels | VPC-SC is one object, with dry-run mode. The AWS perimeter is an assembly of several policy types. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_03_VPC_Service_Controls_Security_Perimeters\|VPC-SC]] |
| Confidential compute | Nitro Enclaves (no network, vsock, attestation) | Confidential VMs (AMD SEV, Intel TDX) `[U]` | An enclave is a carve-out of a VM. A confidential VM encrypts the whole VM's memory. | [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_15_Security_and_Confidential_Computing_Cloud_KMS\|Confidential computing]] |

### 1.10 Observability and FinOps

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Metrics | CloudWatch, Embedded Metric Format | Cloud Monitoring, Managed Service for Prometheus | Custom metric cardinality is billed on both. | [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_14_Observability_CloudWatch_Metrics_Logs_and_X_Ray\|CloudWatch]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_16_Observability_Cloud_Monitoring_Logging_and_Trace\|Cloud Monitoring]] |
| Trace | X-Ray | Cloud Trace | Prefer OpenTelemetry on both to keep the instrumentation portable. | Tracing note |
| Cost attribution | Cost allocation tags (must be activated), Data Exports | Labels flow to billing export in BigQuery | The AWS activation step creates a gap: untagged spend before activation is not attributable retroactively. `[U]` | [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_01_Cloud_Billing_Models_and_Unit_Cost_Allocation\|Billing models]] |
| Commitments | Savings Plans (dollars per hour) | Resource-based and spend-based committed use discounts `[U]` | Spend-based commitments are portable across shapes. Resource-based ones are cheaper but rigid. | [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_02_Commitment_Discounts_Savings_Plans_vs_CUDs\|Commitments]] |

### 1.11 AI infrastructure and managed AI

| Concern | AWS | GCP | Difference that bites | See |
|---|---|---|---|---|
| Custom accelerator | Trainium (NeuronCore, NeuronLink), Inferentia `[U]` for current generations | TPU (MXU systolic arrays, ICI 3D torus with optical circuit switches) `[U]` for current generations | TPU slices are topology-shaped. GPU and Trainium instances are node-shaped. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_01_Accelerator_Architectures_GPUs_Trainium_TPUs\|Accelerators]] |
| Scale-out fabric | EFA with SRD: multipath, out-of-order delivery handled in the NIC `[P]` | GPUDirect over TCPX or RoCE on GPU series, ICI plus data-center network for TPU multislice `[U]` | See ECMP collision math in Section 2.7. | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI\|Interconnects]] |
| Resilient training clusters | SageMaker HyperPod (automated node repair) | GKE-based AI clusters, TPU multislice | | [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_03_High_Performance_AI_Cluster_Architectures\|AI clusters]] |
| Managed model APIs | Bedrock: on-demand, provisioned throughput (model units), cross-region inference profiles `[U]` | Vertex AI: pay-as-you-go, provisioned throughput, regional and global endpoints `[U]` | Quota, burst, and throttle semantics differ. Read the per-model quota page. | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_09_Managed_Inference_Amazon_Bedrock_vs_Vertex_AI\|Managed inference]], [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_10_Claude_on_Bedrock_and_Vertex_AI\|Claude on both]] |
| ML pipelines | SageMaker Pipelines | Vertex AI Pipelines (Kubeflow DAGs) | | [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_04_MLOps_Pipelines_SageMaker_vs_Vertex_AI\|MLOps]] |
| Guardrails | Bedrock Guardrails | Vertex safety filters, Model Armor `[U]` | | [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_07_AI_Safety_Guardrails_and_Governance\|Guardrails]] |

## 2. Mechanisms that getting-started guides skip

### 2.1 NAT port arithmetic: per-destination budget versus per-VM port blocks

The two clouds expose different failure surfaces for the same symptom, which is intermittent outbound connect failures under load.

**AWS NAT gateway.**
<br>A single NAT gateway supports up to 55,000 simultaneous connections per assigned IP address to each unique destination.
That number can be raised by quota request to eight assigned IPs, which gives 440,000 simultaneous connections to one destination (AWS whitepaper, as of 2026-10) `[V]`.
The limit is keyed on the destination, so fan-out across many destination IPs multiplies capacity, and fan-in to one SaaS IP exhausts it.
A NAT gateway lives in one AZ, so a per-AZ routing design divides the budget by the number of AZs.

**GCP Cloud NAT.**
<br>Each NAT IP offers ports 1024 to 65535, which is 64,512 usable ports.
By default a public NAT with static port allocation reserves a minimum of 64 ports per VM, and dynamic port allocation defaults to 32 minimum ports per VM when you do not set a value (third-party summary of Google docs, as of 2026-10; recheck against Cloud NAT docs) `[U]`.
Dynamic allocation requires endpoint-independent mapping to be disabled, as the gcloud flags in the same summary show.
Capacity is therefore a reservation of ports per VM, not a per-destination counter.
Cloud NAT is implemented in the host networking stack (Andromeda), so there is no NAT appliance to scale, only a port inventory to size `[P]`.

**Worked sizing, AWS.**

1. 600 pods each hold 150 concurrent connections to one SaaS endpoint, which is 90,000 concurrent flows to a single destination.
2. One NAT IP allows 55,000, so the minimum is ceil(90,000 / 55,000) = 2 IPs.
3. With 30 percent headroom the target is 117,000 flows, which is ceil(117,000 / 55,000) = 3 IPs.
4. If you route by AZ and one AZ carries half the pods, each AZ's NAT needs its own sizing.

**Worked sizing, GCP.**

1. 100 VMs each need 4,800 concurrent connections to one destination (32 pods times 150 connections).
2. Total ports needed are 100 x 4,800 = 480,000.
3. 480,000 / 64,512 = 7.44, so 8 NAT IPs.
4. Static allocation must reserve 4,800 ports on every VM. Dynamic allocation reserves them only on the VMs that need them, at the price of allocation delay when a VM ramps.

**Mitigations on both clouds.**
Use connection pooling and HTTP keep-alive, spread across more destination IPs where the vendor allows it, add NAT IPs, and prefer private connectivity (PrivateLink or PSC) to the vendor.

### 2.2 Object store request scaling: index partitions and ramp curves

Both stores scale by splitting a range-partitioned key index, and both punish a sudden step-load.

**S3.**
Your application can achieve at least 3,500 PUT/COPY/POST/DELETE or 5,500 GET/HEAD requests per second per partitioned prefix, and the number of prefixes is unbounded (AWS docs, as of 2026-10) `[V]`.
When a spike exceeds what the current partitions serve, you can receive a temporary HTTP 503 until the optimization completes (AWS re:Post knowledge center, as of 2026-10) `[V]`.

**GCS.**
Buckets start with an initial capacity of 1,000 object writes per second and 5,000 object reads per second (Google Cloud docs, as of 2026-10) `[V]`.
The guidance is to double the request rate every 20 minutes for ramp-up (Google Cloud docs, as of 2026-10) `[V]`.
A new key range has the same issue: writing under a new prefix should also ramp gradually or you risk temporarily higher latency and error rates (Google Cloud docs, as of 2026-10) `[V]`.

**Ramp arithmetic.**

- From 5,000 reads per second to at least 100,000 requires 5 doublings (5,000 x 2^5 = 160,000), so about 100 minutes at the documented cadence.
- From 1,000 writes per second to at least 100,000 requires 7 doublings (128,000), so about 140 minutes.
- A training fleet that starts 1,024 readers at the same instant is a step function, not a ramp.
- Mitigation: stagger worker start, shard the key space so no single range carries the load, retry with full jitter on 503 or 429, and for hot checkpoint paths consider the single-zone low-latency tiers `[U]`.

**S3 lower bound.**
For 100,000 GET/s over evenly spread keys you need at least ceil(100,000 / 5,500) = 19 partitioned prefixes at the documented floor.
The documented number is a floor, not a ceiling, and the observed ceiling depends on the split history of that index range.

### 2.3 IAM: a filter lattice versus a union with subtractive deny

**AWS (simplified).**
Within an account, access requires an allow from an identity policy or a same-account resource policy, no explicit deny, and the request must sit inside every ceiling that applies (SCP, RCP, permission boundary, session policy).
Cross-account access needs both sides: an allow in the caller's identity policy and an allow in the target's resource policy.
SCPs and RCPs never grant anything.

**GCP (simplified).**
Effective permissions are the union of role bindings on the resource and every ancestor (project, folder, organization).
IAM deny policies are evaluated first and can subtract.
A cross-project grant is single-sided: the resource owner binds a principal from any project.
Org Policy constraints are orthogonal and act on resource configuration.

**Consequences when you port.**

1. A "Deny unless in my org" ceiling in AWS (SCP and RCP) becomes a deny policy plus Org Policy domain restriction plus VPC-SC in GCP. No single object does it.
2. A broad grant at a GCP folder is inherited by every project below it, and the only fix is a deny or removing the grant at the parent. AWS's equivalent mistake is bounded by the account.
3. Cross-account trust bugs in AWS are "forgot the second side" (access denied). On GCP the same mistake is "granted too widely from one side" (silent over-permission).
4. Detective tooling differs: IAM Access Analyzer on AWS, Policy Analyzer and recommenders on GCP `[U]`.

### 2.4 Concurrency models: Lambda versus Cloud Run

Lambda assigns one in-flight request to one execution environment, so concurrency equals in-flight requests (Little's law: N = lambda x W).
Cloud Run multiplexes up to a configurable per-instance concurrency (default 80 and maximum 1,000 `[U]`), so instance count depends on whichever of CPU or concurrency binds first.

**Worked example.**
Traffic is lambda = 500 requests per second and W = 200 ms.

1. In-flight requests are 500 x 0.2 = 100.
2. Lambda needs 100 concurrent environments, regardless of how little CPU each request uses.
3. Cloud Run, I/O-bound case (5 ms of CPU per request): CPU demand is 500 x 0.005 = 2.5 vCPU. Concurrency alone needs ceil(100 / 80) = 2 instances. The binding constraint is CPU, so 3 one-vCPU instances (more with utilization headroom).
4. Cloud Run, CPU-bound case (150 ms CPU per request): CPU demand is 500 x 0.15 = 75 vCPU, so at least 75 one-vCPU instances. Multiplexing gives nothing.

**What this means.**

- For I/O-heavy handlers, Cloud Run needs about 30x fewer instances, and cold starts amortize across 80 requests.
- The cost is intra-instance contention. One slow request can steal CPU and memory from its neighbors, and global state (connection pools, caches) is shared. Lambda gives per-request isolation for free.
- Memory must fit concurrency x per-request footprint, or you get OOM kills that take down 80 requests at once.
- Autoscalers on both clouds target a utilization below the configured maximum, so provision above the arithmetic minimum `[U]`.

### 2.5 Transactions: optimistic retry versus lock plus commit wait

**Spanner `[P]`.**
Read-write transactions take locks (wound-wait deadlock avoidance) and commit through two-phase commit across Paxos groups when more than one group is involved.
TrueTime exposes an interval [earliest, latest], and the coordinator picks a commit timestamp s and waits until TT.after(s) is true before applying the commit and releasing locks.
The paper reports that epsilon varied between about 1 and 7 ms with an average of about 4 ms, so expected commit wait is about 2 x epsilon (OSDI 2012).
That wait runs with locks held.

**Aurora DSQL `[U]`.**
The design is optimistic: transactions run without locks, conflicts are detected at commit, and the application sees a serialization failure that it must retry.
Isolation is snapshot-style rather than Spanner's external consistency, so write skew is the application's problem.
Check the current GA and isolation documentation before relying on this.

**Hot-row ceiling (modeled, not measured).**

1. Suppose a hot row has a transaction hold time of L = Paxos write (about 10 ms in-region) + commit wait (about 8 ms) = 18 ms.
2. A lock-based store serializes writers on that row, so the ceiling is about 1 / 0.018 = 55 transactions per second, and the excess queues (latency grows, work is not wasted).
3. An optimistic store has the same ceiling on successful commits (about 1 / commit latency) but the excess offered load burns retries.
4. If the conflict probability per attempt is p, expected attempts are 1 / (1 - p): 2 at p = 0.5 and 10 at p = 0.9. Retry traffic amplifies load on the hot key exactly when it is already saturated.

**Port checklist.**
Wrap every DSQL transaction in a bounded retry loop with jittered backoff, shard hot counters, and avoid read-modify-write on single rows.
For Spanner, keep read-write transactions short because locks are held through commit wait, and use read-only or stale reads where you can.

### 2.6 Global VPC versus regional VPC: where a bad push lands

A GCP VPC is a single global configuration object, so a firewall rule, route, or peering change applies to every region at once.
An AWS VPC is regional, so the same change touches one region unless you replicate it through automation.
That automation is the real exposure: a pipeline that applies the same Terraform to all regions at once recreates the global blast radius.

**Design response.**

- On GCP, treat the project or VPC as the cell, and keep per-environment or per-cell VPCs instead of one organization-wide VPC.
- Roll network changes through a ring order (canary project, then a small cell set, then the rest), as in [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_03_Cell_Based_Architecture_and_Shuffle_Sharding\|Cell-Based Architecture and Shuffle Sharding]].
- On AWS, apply the same ring discipline across accounts and regions.

**IP planning.**
GCP lets you expand a subnet's primary range in place `[U]`, while AWS subnets cannot be resized.
Overlapping CIDRs cannot be peered on either cloud, so allocate from one IPAM plan from the start.

**Control-plane lesson.**
Large public outages on both clouds have involved globally replicated control data or configuration pushed faster than it was validated.
Read [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages\|Anatomy of Major Cloud Outages]] for the incident reports, and check dates and details there, since this appendix does not re-verify them `[U]`.

### 2.7 Multipath transport: ECMP flow hashing versus packet spraying

Standard ECMP hashes the 5-tuple, so a flow is pinned to one path for its lifetime.
Distributed training produces a small number of very large, long-lived flows, which is the worst case for hashing.

**Collision math.**
Hash 8 elephant flows onto 8 equal-cost uplinks:

1. The probability that every flow lands on a distinct uplink is 8! / 8^8 = 40,320 / 16,777,216 = 0.24 percent.
2. So with probability 99.76 percent at least one uplink carries two or more flows, and each of those flows gets at most half the link.
3. Expected number of distinct uplinks used is 8 x (1 - (7/8)^8) = 8 x 0.656 = 5.25, so on average about 2.75 uplinks sit idle.
4. A collective operation completes at the speed of its slowest flow, so the whole step is gated by the most loaded link.

**Mitigations by cloud.**

- AWS EFA uses the Scalable Reliable Datagram protocol, which sprays a flow across many paths and tolerates reordering in the NIC `[P]`.
- On other fabrics the options are many parallel connections (queue pairs) per peer so the hash has more entries to spread, rail-optimized topology, and adaptive routing where the switch supports it `[U]`.
- TPU ICI avoids the problem with a statically routed torus, and optical circuit switches reconfigure the torus around failed blocks `[P]`.

**What to measure.**
Per-link utilization variance across uplinks during an all-reduce, and per-step time p99 versus p50.
A wide gap with idle links is hash collision, not congestion.

### 2.8 Pod IP exhaustion

**EKS with the VPC CNI.**
Max pods per node = ENIs x (IPv4 addresses per ENI - 1) + 2.
For an instance type with 3 ENIs and 10 addresses per ENI that is 3 x 9 + 2 = 29 pods `[U]`.
Pod IPs come from the node's subnet, so 29 pods per node on a /24 subnet (251 usable addresses) leaves room for roughly 8 nodes' worth of pods before exhaustion.
Prefix delegation raises the per-node ceiling at the cost of coarser IP allocation `[U]`.

**GKE VPC-native.**
Default max pods per node is 110, and each node is allocated a /24 slice of the pod secondary range (256 addresses, about 2x pods to allow for churn) `[U]`.
A /16 pod range therefore supports 256 nodes at the default.
The pod range is separate from the node subnet, so pod growth does not consume node addresses.

**Failure mode.**
Both fail with pods stuck in `ContainerCreating` or `Pending` while compute is idle.
Size pod CIDRs from max-nodes x per-node-allocation, not from current pod counts.

## 3. Selected numbers with provenance

| Quantity | Value | Source |
|---|---|---|
| AWS NAT gateway connections per IP per unique destination | 55,000 | AWS whitepaper, as of 2026-10 `[V]` |
| AWS NAT gateway with 8 assigned IPs | 440,000 to one destination | AWS whitepaper, as of 2026-10 `[V]` |
| Usable ports per Cloud NAT IP | 64,512 (ports 1024 to 65535) | Arithmetic, confirmed by third-party summary of Google docs, as of 2026-10 `[U]` |
| Cloud NAT static minimum ports per VM, default | 64 | Third-party summary of Google docs, as of 2026-10 `[U]` |
| S3 per-prefix request floor | 3,500 write-class, 5,500 read-class per second | AWS docs, as of 2026-10 `[V]` |
| GCS initial bucket I/O | 1,000 writes per second, 5,000 reads per second | Google Cloud docs, as of 2026-10 `[V]` |
| GCS ramp guidance | Double the request rate every 20 minutes | Google Cloud docs, as of 2026-10 `[V]` |
| Aurora storage layout | 6 copies, 3 AZs, write quorum 4 of 6, read quorum 3 of 6 | Verbitski et al., SIGMOD 2017 `[P]` |
| Spanner clock uncertainty | Epsilon about 1 to 7 ms, average about 4 ms | Corbett et al., OSDI 2012 `[P]` |
| Firecracker | Boot in about 125 ms, about 5 MiB memory overhead per microVM | Agache et al., NSDI 2020 `[P]` |
| Maglev lookup table | Prime-sized table, consistent hashing over 5-tuple | Eisenbud et al., NSDI 2016 `[P]` |

## 4. Porting failure-mode crosswalk

| Symptom after a port | Likely cause on AWS | Likely cause on GCP | First check |
|---|---|---|---|
| Intermittent outbound connect failures to one vendor | NAT gateway per-destination budget exhausted | Cloud NAT port block exhausted on a hot VM | NAT metrics and drop logs, then count distinct source ports per destination |
| 503 or 429 at job start on the object store | Index partition split in progress | Bucket or key range ramp limit | Request rate over time, staggered start |
| Cross-account call denied | Missing resource-policy side, or SCP or RCP ceiling | Not applicable (single-sided), so check deny policy or VPC-SC | Policy simulator, VPC-SC audit log |
| Unexpected access granted | Overly broad resource policy | Inherited role at folder or organization | Effective-policy analyzers |
| Serialization failures spike after a DB migration | DSQL optimistic conflicts on hot rows | Not applicable. Spanner shows lock-wait latency instead. | Per-key write rate, retry counters |
| Training step time p99 far above p50 | ECMP collisions on EFA-less paths | Fabric hash collisions or a straggler NIC | Per-link utilization during all-reduce |
| Pods pending with idle nodes | ENI or secondary IP exhaustion | Pod secondary range exhausted | CNI logs, subnet free-IP count |
| Spot job loses state on reclaim | 2-minute window assumed | Roughly 30-second window | Handler time versus the notice window `[U]` |
| KMS throttling on bulk reads | Shared KMS request quota with S3 SSE-KMS | CMEK on GCS with a hot key | Key request metrics, use bucket-level keys where supported |

## 5. Interview questions and model answers

### Q1 (L5): What breaks when you lift an ALB, Fargate, RDS service onto GCP?

**Model answer.**
Do not translate by product name. Translate by invariant.

1. The balancer: ALB is regional with per-AZ nodes behind DNS. The GCP global external Application LB is one anycast VIP with edge proxies, so health checking, failover, and TLS termination happen at a different place and the client sees a different failure mode. A regional external LB exists on GCP if you want AWS-shaped behavior.
2. The runtime: Fargate gives one task per microVM. Cloud Run multiplexes requests per instance, so any process-global state, connection pool sizing, or memory assumption needs rechecking (Section 2.4).
3. The database: RDS maps to Cloud SQL, or AlloyDB if you want the log-structured storage tier. Connection limits and failover time differ, so test failover rather than reading the SLA.
4. Identity: the ECS task role becomes a Cloud Run service account, and the trust policy becomes an IAM binding on the resources the service touches.
5. Networking: egress through NAT needs the Section 2.1 sizing.

**Follow-up.**
Which of those five would you load-test before cutover, and what is the pass criterion for each?

### Q2 (L5): Pods behind Cloud NAT see intermittent connect timeouts to a payment vendor. Walk through the diagnosis.

**Model answer.**
Start by separating three causes: vendor throttling, NAT port exhaustion, and DNS or routing.

1. Vendor throttling returns HTTP 429 or resets after the TCP handshake. Port exhaustion fails before or at connect, and the NAT drop log shows allocation failures.
2. Compute the port demand: pods per node x concurrent connections per pod to that single destination, compared with the VM's allocated ports (default 64 minimum under static allocation `[U]`).
3. With 32 pods x 150 connections you need 4,800 ports on that VM, which is far over 64.
4. Fix in order of cost: pool and reuse connections, enable dynamic port allocation (requires endpoint-independent mapping off), raise minimum and maximum ports, add NAT IPs (64,512 ports each), and finally move the vendor to private connectivity if offered.
5. Remember the vendor may allowlist your NAT IPs, so adding IPs is a coordination task, not just a config change.

**Follow-up.**
How would you alarm on this before customers see it, and what is the signal on AWS NAT gateway for the same failure?

### Q3 (L6): Design cross-account (AWS) and cross-project (GCP) access from a data-science workload to a sensitive bucket, with an exfiltration-resistant perimeter.

**Model answer.**
On AWS the policy composition is a lattice of ceilings.

1. The caller's role needs an allow, and the bucket policy needs to name or admit the role. Both sides are required cross-account.
2. The perimeter is assembled: an SCP restricting what identities in your organization can do, an RCP or bucket policy conditioning on `aws:PrincipalOrgID` so foreign principals cannot read, and VPC endpoint policies plus a network condition so reads must come through your endpoints.

On GCP the grant is single-sided and inherited.

3. Bind the caller's service account on the bucket or a narrow project, never a folder, because inheritance is silent.
4. Add a VPC Service Controls perimeter containing both projects, with explicit ingress rules for any outside caller and egress rules for any outside destination, and dry-run it first against audit logs to see what would break.
5. Add IAM deny policies for the actions you never want, since allow policies cannot be capped.

The honest trade-off is that VPC-SC gives one coherent object but coarse project-level semantics, while the AWS composition is finer-grained but easy to leave with a gap between layers.

**Follow-up.**
A contractor's laptop with valid credentials downloads the data through an allowed VPC endpoint. Which controls still hold, and which do not?

### Q4 (L6): Compare Aurora DSQL and Spanner for a multi-region ledger. Which do you pick and what do you test?

**Model answer.**
They make opposite concurrency choices (Section 2.5).

1. Spanner is lock-based, with external consistency from TrueTime. A writer holds locks through commit wait (about 2 x epsilon, so single-digit milliseconds) and through the Paxos round, so a hot row has a throughput ceiling of about 1 / hold time and excess load queues.
2. DSQL is optimistic `[U]`. Reads and writes proceed without locks and conflicts surface at commit as retryable errors. Throughput on cold keys is excellent, but a hot row turns excess load into retry storms (expected attempts 1 / (1 - p)).
3. For a ledger, the hot rows are account balances and sequence counters. I would model per-key write rates first.
4. If a few accounts see more than a few dozen writes per second, I prefer Spanner or redesign into append-only entries with derived balances, which works on either engine.
5. I would test: the retry rate at 2x peak on the hottest key, commit latency from every region at p99, behavior during a region loss, and the isolation level's effect on invariants like "no overdraft" (write skew under snapshot isolation).
6. I would verify current GA status, limits, and isolation guarantees against official docs first, since this area changes quickly.

**Follow-up.**
Your product owner wants a global unique-sequence-number feature. How do you build it so it does not become the hot row?

### Q5 (L6): A 512-GPU job on one cloud has a p99 step time 40 percent above p50 while links look healthy on average. Diagnose.

**Model answer.**
Average utilization hides variance, so look at per-link distribution.

1. Large collectives create few, long-lived flows. Under 5-tuple ECMP, 8 flows over 8 uplinks collide with probability 99.76 percent, and expected distinct uplinks used is about 5.25, so the fabric is partly idle while some links are shared (Section 2.7).
2. Because a collective finishes at the slowest flow, step time follows the most loaded link, which explains the p99 gap with a healthy average.
3. Confirm with per-uplink counters during the all-reduce. If variance is high and some links are idle, it is hashing, not capacity.
4. Remedies: more queue pairs or channels per peer so the hash has more entries, rail-aligned topology, a transport with multipath (SRD on EFA), or adaptive routing where available.
5. Rule out a straggler node (thermal throttle, a degraded NIC, silent data corruption) by comparing per-rank compute times. That is a separate cause and needs node evacuation, not fabric tuning.

**Follow-up.**
How would you automate straggler detection so it does not need a human reading counters?

### Q6 (L7): The CTO proposes active-active across AWS and GCP for the inference platform "to avoid cloud lock-in and outages". Respond.

**Model answer.**
Separate the goals, because they need different mechanisms.

1. Availability: cross-cloud active-active adds a dependency on the glue (global traffic management, identity federation, config sync, artifact replication). Each is a new correlated failure path, and the control plane for the glue sits in one place. Most major cloud outages have been control-plane or config-propagation failures, which a second cloud does not fix if your own tooling is the shared fate. See [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability\|Static Stability]].
2. Stateless tier: serving is nearly stateless (weights from object storage, KV cache local), so active-active across clouds is feasible if weights are replicated and each cloud is its own cell with its own quota and capacity reservation.
3. Stateful tier: anything with transactions (billing, tenant config) pays a consensus latency tax across clouds, plus egress for replication. Use one primary with a warm standby and a rehearsed failover instead.
4. Capacity: GPU capacity is the binding constraint. A second cloud only helps if you hold reservations there, which means paying for idle accelerators. Model that number before agreeing.
5. Lock-in: it is mostly in identity, data gravity, and the IaC layer, not compute. Portable container images, OpenTelemetry, and OIDC federation get most of the benefit cheaply.
6. Recommendation: per-cloud cells with independent control planes, a thin global router, regional failover inside each cloud first, and cross-cloud failover for the stateless tier only, tested with game days.

**Follow-up.**
How do you prove, before an incident, that the second cloud has enough GPU capacity and warm weights to absorb the failover load?

### Q7 (L7): Your platform team wants a cloud-neutral abstraction (one Terraform module, one API) over both clouds. When does this fail, and what do you build instead?

**Model answer.**
An abstraction at the product-name level fails at the semantic divergences in this appendix.

1. The grant model, scope of the network object, lease semantics, and transaction model all differ. A "bucket" or "queue" module hides exactly the properties that cause incidents: prefix ramp, ack deadlines, cross-account sidedness.
2. The abstraction becomes the lowest common denominator, so teams either escape it (shadow IaC) or lose the features that justify the cloud (for example per-destination NAT budgets or VPC-SC dry-run).
3. Maintenance cost scales with the number of cloud releases you must track.
4. Build thin portability at stable boundaries instead: OCI images, Kubernetes APIs, OIDC for identity, OpenTelemetry for telemetry, S3-style object APIs where both support them, and PostgreSQL wire compatibility for relational data.
5. Keep per-cloud modules for network, identity, and data, expressing the invariants as tests: each module ships a conformance suite (for example "an egress flood cannot exhaust NAT ports", "a foreign principal cannot read the bucket") run in CI against both clouds.
6. Publish this appendix's semantic differences as module documentation so consumers know what the module does not hide.

**Follow-up.**
Which two of those conformance tests would you write first, and what would make you delete a module instead of maintaining it?

## 6. Further reading

- [Static stability using Availability Zones](https://aws.amazon.com/builders-library/static-stability-using-availability-zones/) - the design rule behind eliminating runtime cross-AZ dependencies, essential for comparing zonal AWS and regional GCP constructs.
- [Workload isolation using shuffle sharding](https://aws.amazon.com/builders-library/workload-isolation-using-shuffle-sharding/) - the combinatorics behind cell and tenant isolation used in Section 2.6.
- [Timeouts, retries, and backoff with jitter](https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/) - the retry discipline you need for S3 503, GCS ramp errors, and DSQL serialization failures.
- [Amazon S3 performance guidelines](https://docs.aws.amazon.com/AmazonS3/latest/userguide/optimizing-performance.html) - the source of the per-prefix request floor.
- [Cloud Storage request rate and access distribution guidelines](https://docs.cloud.google.com/storage/docs/request-rate) - the source of the ramp-up guidance for GCS.
- [Cloud Storage best practices for media workloads](https://docs.cloud.google.com/storage/docs/best-practices-media-workload) - states the initial I/O capacity and the 20-minute doubling cadence.
- [Building a scalable and secure multi-VPC AWS network infrastructure](https://docs.aws.amazon.com/whitepapers/latest/building-scalable-secure-multi-vpc-network-infrastructure/using-nat-gateway-for-centralized-egress.html) - NAT gateway connection limits and centralized egress trade-offs.
- [Spanner: Google's globally distributed database (OSDI 2012)](https://www.usenix.org/conference/osdi12/technical-sessions/presentation/corbett) - TrueTime, commit wait, and the locking model.
- [Amazon Aurora: design considerations for high-throughput cloud-native relational databases (SIGMOD 2017)](https://dl.acm.org/doi/10.1145/3035918.3056101) - the 6-copy, quorum, and redo-shipping design.
- [Firecracker: lightweight virtualization for serverless applications (NSDI 2020)](https://www.usenix.org/conference/nsdi20/presentation/agache) - the microVM design behind Lambda and Fargate.
- [Maglev: a fast and reliable software network load balancer (NSDI 2016)](https://www.usenix.org/conference/nsdi16/technical-sessions/presentation/eisenbud) - consistent hashing and connection tracking behind Google's L4 load balancing.
- [Andromeda: performance, isolation, and velocity at scale in cloud network virtualization (NSDI 2018)](https://www.usenix.org/conference/nsdi18/presentation/dalton) - why Cloud NAT and the global VPC are software features of the host dataplane.
- [A peek behind Colossus, Google's file system](https://cloud.google.com/blog/products/storage-data-transfer/a-peek-behind-colossus-googles-file-system) - the storage substrate under GCS and Persistent Disk.
- [Site Reliability Engineering (the Google SRE book)](https://sre.google/sre-book/table-of-contents/) - the operational framing used for the failure-mode crosswalk.

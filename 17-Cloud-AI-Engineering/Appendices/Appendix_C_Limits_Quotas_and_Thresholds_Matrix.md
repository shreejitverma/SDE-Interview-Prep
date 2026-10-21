---
type: concept
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Appendix C: Limits, Quotas, and Thresholds Reference Matrix

## 0. How to read this appendix

Every row carries a status code.
`V` means the value was checked against an AWS or Google Cloud primary source on 2026-10-21 and is tagged `(source, as of 2026-10)`.
`U` means the value was recalled from earlier reading of official documentation and was not re-checked on 2026-10-21.
Treat `U` rows as study material, not as design inputs, until you open the linked quota page or run the CLI in section 6.
The research tool was exhausted partway through this batch, so only five numeric claims are `V`.
Every `U` claim is also listed in the batch report at the end.

Quota vocabulary used throughout:

| Term | Meaning | Who can change it |
|---|---|---|
| Hard limit | Architectural ceiling baked into a data structure, wire format, or storage layout. | Nobody, short of a service redesign. |
| Soft quota | Admission-control value stored in the control plane (Service Quotas, Cloud Quotas). | You, through a request, often with human or risk-model review. |
| Scaling threshold | A rate the service sustains today and ramps toward under load, such as per-prefix request rates. | The service, reactively, on a timescale of minutes. |
| Guideline | A tested, supported operating point rather than a rejection point. | Not applicable. |

Related chapters: [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_05_Constant_Work_and_Workload_Smoothing|Constant Work]], [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_02_Load_Shedding_Backpressure_and_Admission_Control|Load Shedding]], [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_03_Resilient_Communication_Retries_Jitter_and_Circuit_Breakers|Retries and Jitter]], and [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_06_Cloud_AI_Numbers_Every_Staff_Engineer_Must_Know|Numbers Every Staff Engineer Must Know]].

## 1. Mechanisms the getting-started guides skip

### 1.1 A quota is admission control, not capacity

A quota is a number in a control-plane database that gates the API call.
Capacity is physical inventory in a specific zone: accelerators, NVLink-connected hosts, and rack power.
A granted GPU or TPU quota can coexist with an `InsufficientInstanceCapacity` error on AWS or a stockout on Google Cloud, because the two systems are separate.
The quota check passes first, then the placement service fails to find a host.
Mitigations differ by cloud: AWS offers On-Demand Capacity Reservations and Capacity Blocks for ML, and Google Cloud offers reservations and Dynamic Workload Scheduler.
All four are `U` here, so verify availability and terms before you plan around them.
The operational rule is to treat quota and capacity as two separate procurement tracks with separate lead times.
See [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale|Fault Tolerance and Fleet Resiliency]] for the reservation strategy.

### 1.2 Every rate limit is a (rate, burst) pair

Control planes and many data planes implement limits as token buckets with a refill rate `r` and a capacity `B`.
Documentation usually states only `r`.
A client arriving at rate `L > r` drains a full bucket in `B / (L - r)` seconds and then sees rejections at a steady `L - r` per second.
With hypothetical values `B = 100`, `r = 20`, and `L = 60`, the first throttle appears after 2.5 seconds and 40 requests per second are rejected thereafter.
The same arithmetic explains why a fleet that passes load tests in 30 seconds fails in production minutes later: the burst allowance hid the sustained rate.
The retry policy then decides whether the rejection is absorbed or amplified.
Full jitter is mandatory, and a retry budget (a fraction of first-attempt traffic, for example 10 percent) bounds amplification.
EC2 documents its API throttling as token buckets, with different bucket sizes and refill rates per action category (AWS docs, UNVERIFIED, last known).

### 1.3 Partition-based limits are reactive, not provisioned

Object stores and key-value stores do not hold a fixed limit.
They hold a number of partitions multiplied by a per-partition rate, and the partition count follows load with a lag.
For S3, the documentation states 

your application can achieve at least 3,500 PUT/COPY/POST/DELETE or 5,500 GET/HEAD requests per second per partitioned prefix

 (AWS docs, as of 2026-10).
The same documentation adds that 

Amazon S3 automatically scales in response to sustained request rates above these guidelines

 (AWS docs, as of 2026-10).
The word that matters is "sustained".
A job that goes from zero to 50,000 GET/s in one second hits a topology sized for yesterday's traffic.
The client sees HTTP 503 SlowDown until the index layer splits and rebalances, which takes minutes rather than milliseconds (mechanism described in the S3 and ShardStore literature, see [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_08_S3_Deep_Dive_Consistency_and_ShardStore|S3 Deep Dive]]).
DynamoDB works the same way, with a per-partition token bucket and roughly five minutes of retained unused capacity as burst (AWS docs, UNVERIFIED, last known).
The ceiling that adaptive capacity cannot move is a single item: one hot item is bounded by one partition, roughly 1,000 WCU/s and 3,000 RCU/s (AWS docs, UNVERIFIED, last known).
Cloud Storage documents an analogous ramp: new buckets start near 1,000 object writes/s and 5,000 reads/s and double about every 20 minutes under sustained load (Google Cloud docs, UNVERIFIED, last known).
Design rule: ramp workloads on a schedule, spread keys across prefixes or hash buckets, and make the first request of a cold fleet a staggered one.

### 1.4 Stateful dataplane tables fail silently

Nitro instances track connections for security groups.
Each instance type has a connection-tracking allowance, and exceeding it drops new flows without an API-visible error.
The ENA driver exposes counters for this: `bw_in_allowance_exceeded`, `bw_out_allowance_exceeded`, `pps_allowance_exceeded`, `conntrack_allowance_exceeded`, and `linklocal_allowance_exceeded` (AWS docs, UNVERIFIED, last known).
Read them with `ethtool -S eth0`.
Security group rules that allow all traffic from `0.0.0.0/0` on all ports make flows "untracked", which spares table space but changes return-path semantics (AWS docs, UNVERIFIED, last known).
Idle timeouts for tracked flows are configurable per ENI, with defaults of roughly 5 days for established TCP and 30 to 180 seconds for UDP (AWS docs, UNVERIFIED, last known).
A long TCP default combined with a client that never closes connections is how a table fills.
NAT is the second stateful table.
An AWS NAT gateway supports 55,000 simultaneous connections per unique destination (IP, port, protocol) and reports `ErrorPortAllocation` when it cannot allocate (AWS docs, UNVERIFIED, last known).
Cloud NAT gives each IP 64,512 usable ports and allocates a minimum number per VM, 64 by default for static allocation (Google Cloud docs, UNVERIFIED, last known).
That default supports about 1,008 VMs per IP at 64 ports each.
A single VM opening many connections to one destination exhausts its own allocation long before the gateway is "busy".

### 1.5 The 1,024 packets per second resolver limit

Each ENI can send at most 1,024 packets per second to the VPC resolver, instance metadata service, and time sync service combined (AWS docs, UNVERIFIED, last known).
Kubernetes pods default to `ndots:5`, so a name such as `api.example.com` with two dots is tried against every search suffix first.
With four search domains in a typical EKS `resolv.conf`, one application lookup can emit up to five attempts, each as A and AAAA queries, so about 10 packets.
At that amplification, one ENI carrying CoreDNS upstream traffic saturates near 100 uncached lookups per second.
The failure appears as intermittent `EAI_AGAIN` or 5-second DNS stalls and `linklocal_allowance_exceeded` increments, never as an AWS API error.
Mitigations are fully qualified names with a trailing dot, lowering `ndots`, NodeLocal DNSCache, and spreading CoreDNS across nodes.

### 1.6 LLM token quotas: reservation at admission, settlement afterward

Managed LLM endpoints meter tokens per minute (TPM) as well as requests per minute (RPM).
The behavior I recall from the Bedrock documentation is that admission uses the input tokens plus the requested `max_tokens`, because output length is unknown.
After completion, the quota is settled against actual usage, with output tokens weighted by a model-specific burndown multiplier (5x for recent Claude models) (AWS docs, UNVERIFIED, last known).
Whether the burndown multiplier also applies to the admission estimate is not something I could confirm, so the calculator in section 5 treats it as a parameter.
Consequence: an oversized `max_tokens` reduces admitted throughput even when responses are short, and returns 429s while a dashboard of settled usage shows headroom.
The worked example in section 5 shows a 29 percent throughput difference from `max_tokens` alone.
Vertex AI Gemini models use dynamic shared quota, where there is no fixed per-project TPM and capacity is contended among customers.
Provisioned Throughput is the contractual alternative (Google Cloud docs, UNVERIFIED, last known).
Third-party models served on Vertex, such as Claude, use explicit per-region quotas (Google Cloud docs, UNVERIFIED, last known).
See [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_09_Managed_Inference_Amazon_Bedrock_vs_Vertex_AI|Managed Inference]] and [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_10_Claude_on_Bedrock_and_Vertex_AI|Claude on Bedrock and Vertex AI]].

### 1.7 Pod IP arithmetic

On EKS with the AWS VPC CNI, max pods per node is `ENIs * (IPv4 addresses per ENI - 1) + 2`.
For an instance with 3 ENIs and 10 addresses each, that is 29 pods.
The `+2` accounts for host-network system pods, and each ENI's primary address is not available to pods.
Prefix delegation changes the unit from one address to a /28 block of 16 addresses (AWS docs, UNVERIFIED, last known).
On GKE, each node receives a pod CIDR sized to the next power of two at or above twice the max pods per node.
At 110 pods per node that is a /24, 256 addresses, so a /14 pod range supports 1,024 nodes.
Pod ranges cannot be freely resized in place, so IP exhaustion at node 1,025 is an architecture event, not a quota request (Google Cloud docs, UNVERIFIED, last known).

## 2. Object storage, databases, and caches

| Service | Limit | Value | Type | Status | Source |
|---|---|---|---|---|---|
| S3 general purpose | Request rate per partitioned prefix | At least 3,500 write-class and 5,500 read-class requests/s | Scaling | V | [AWS docs](https://docs.aws.amazon.com/AmazonS3/latest/userguide/optimizing-performance.html), as of 2026-10 |
| S3 general purpose | Prefixes per bucket | No limit | Hard (none) | V | AWS docs, as of 2026-10 |
| S3 general purpose | Scaling trigger | Sustained request rate above guideline | Scaling | V | AWS docs, as of 2026-10 |
| S3 Express One Zone | Transactions per directory bucket | Up to 2 million TPS, independent of directory count | Scaling | V | [AWS S3 FAQ](https://aws.amazon.com/s3/faqs/), as of 2026-10 |
| S3 Express One Zone | Observed checkpoint workload | Up to 2 million reads/s and 200,000 writes/s, 503 rate under 0.1 percent | Case study | V | [AWS Storage Blog](https://aws.amazon.com/blogs/storage/prime-video-improved-stream-analytics-performance-with-amazon-s3-express-one-zone/), as of 2026-10 |
| S3 | Max object size, single PUT, multipart parts | 5 TB, 5 GB, 10,000 parts of 5 MiB to 5 GiB | Hard | U | AWS docs, UNVERIFIED, last known |
| Cloud Storage | Writes to the same object name | About 1 per second | Hard-ish | U | Google Cloud docs, UNVERIFIED, last known |
| Cloud Storage | Initial bucket I/O capacity | About 1,000 object writes/s and 5,000 reads/s, doubling about every 20 min | Scaling | U | Google Cloud docs, UNVERIFIED, last known |
| Cloud Storage | Max object size, compose components | 5 TiB, 32 | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| DynamoDB | Max item size | 400 KB | Hard | U | AWS docs, UNVERIFIED, last known |
| DynamoDB | Per-partition throughput | About 3,000 RCU and 1,000 WCU | Hard-ish | U | AWS docs, UNVERIFIED, last known |
| DynamoDB | Unused-capacity burst retention | About 300 s | Scaling | U | AWS docs, UNVERIFIED, last known |
| DynamoDB | Query or Scan page; TransactWriteItems; BatchWriteItem | 1 MB; 100 items and 4 MB; 25 items | Hard | U | AWS docs, UNVERIFIED, last known |
| DynamoDB | On-demand default per-table throughput | 40,000 read and 40,000 write request units/s | Soft | U | AWS docs, UNVERIFIED, last known |
| DynamoDB | GSIs, LSIs per table; LSI item collection | 20, 5; 10 GB | Soft, Hard | U | AWS docs, UNVERIFIED, last known |
| Aurora | Storage replication and quorum | 6 copies across 3 AZs, write quorum 4 of 6, read quorum 3 of 6 | Design | U | Aurora paper (SIGMOD 2017), not re-checked |
| Spanner | Commit size; mutations per commit | 100 MB; 80,000 including index entries | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Spanner | Processing units per node | 1,000 PUs = 1 node | Design | U | Google Cloud docs, UNVERIFIED, last known |
| Bigtable | Max row size; max cell size | 256 MB; 100 MB | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Kubernetes etcd | Default backend DB size quota | 2 GiB, then NOSPACE alarm and read-only mode | Soft (flag) | U | etcd docs, UNVERIFIED, last known |

Cross-reference: [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_10_Amazon_DynamoDB_Architecture_and_Partitioning|DynamoDB]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_09_Cloud_Storage_and_Colossus_Filesystem|Cloud Storage and Colossus]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_10_Cloud_Spanner_TrueTime_and_External_Consistency|Spanner]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_11_Cloud_Bigtable_LSM_Storage_and_Tablet_Serving|Bigtable]].

## 3. Compute, serverless, and containers

| Service | Limit | Value | Type | Status | Source |
|---|---|---|---|---|---|
| Lambda | Scale-out rate per function | 1,000 new execution environments every 10 seconds | Scaling | V | [Lambda quotas (docs mirror)](https://docs.amazonaws.cn/en_us/lambda/latest/dg/gettingstarted-limits.html), as of 2026-10 |
| Lambda | Account concurrency default | 1,000, shared by all functions in a Region | Soft | U | AWS docs, UNVERIFIED, last known |
| Lambda | Timeout; memory; ephemeral storage | 900 s; 128 to 10,240 MB; 512 to 10,240 MB | Hard | U | AWS docs, UNVERIFIED, last known |
| Lambda | Synchronous payload | 6 MB request and 6 MB response (streaming raises response) | Hard | U | AWS docs, UNVERIFIED, last known |
| Lambda | Asynchronous payload | 256 KB historically; a later increase may exist, re-check | Hard | U | AWS docs, UNVERIFIED, last known |
| Cloud Run | Concurrency per instance | Default 80, max 1,000 | Soft | U | Google Cloud docs, UNVERIFIED, last known |
| Cloud Run | Request timeout | Up to 60 minutes | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Cloud Run | Max instance memory and vCPU | 32 GiB and 8 vCPU | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| EC2 | On-Demand quota unit | vCPUs per instance-family group (for example Standard, G and VT, P) per Region | Soft | U | AWS docs, UNVERIFIED, last known |
| EC2 | Spot interruption notice | 2 minutes | Hard | U | AWS docs, UNVERIFIED, last known |
| Compute Engine | GPU quota | Per Region per GPU type, plus a global all-regions GPU cap | Soft | U | Google Cloud docs, UNVERIFIED, last known |
| Compute Engine | Spot VM preemption notice | 30 seconds | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Kubernetes upstream | Tested scale | 5,000 nodes, 150,000 pods, 110 pods per node default | Guideline | U | Kubernetes docs, UNVERIFIED, last known |
| GKE Standard | Nodes per cluster | 5,000 documented; larger sizes announced and require re-check | Soft | U | Google Cloud docs, UNVERIFIED, last known |
| EKS | Max pods per node | `ENIs * (IPs per ENI - 1) + 2`, or prefix delegation | Derived | U | AWS docs, UNVERIFIED, last known |

Cross-reference: [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_05_Serverless_Compute_and_Lambda_Internals|Lambda internals]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_08_Cloud_Run_and_Serverless_Container_Execution|Cloud Run]], [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_07_Amazon_EKS_Control_Plane_and_Networking|EKS]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_07_GKE_Deep_Dive_Standard_vs_Autopilot|GKE]], [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_03_Spot_Economics_and_Interruption_Handling|Spot Economics]].

Lambda note: the 1,000 per 10 seconds rate applies per function and is separate from the account concurrency cap.
Both gates must pass, so a single function can be limited by either one.

## 4. Network, messaging, analytics, identity, and AI

| Area | Limit | Value | Type | Status | Source |
|---|---|---|---|---|---|
| AWS ENA | Single flow bandwidth | About 5 Gbps, about 10 Gbps inside a cluster placement group, 25 Gbps with ENA Express | Hard-ish | U | AWS docs, UNVERIFIED, last known |
| AWS ENI | Packets/s to resolver, IMDS, time sync | 1,024 per ENI | Hard | U | AWS docs, UNVERIFIED, last known |
| AWS NAT gateway | Connections per unique destination; bandwidth | 55,000; up to 100 Gbps | Hard, Scaling | U | AWS docs, UNVERIFIED, last known |
| AWS Transit Gateway | Bandwidth per VPC attachment | Up to 50 Gbps | Hard-ish | U | AWS docs, UNVERIFIED, last known |
| AWS VPC | Security groups per ENI; rules per group | 5; 60 inbound and 60 outbound | Soft | U | AWS docs, UNVERIFIED, last known |
| GCP VM networking | Egress bandwidth | About 2 Gbps per vCPU up to 32 Gbps; Tier_1 networking higher | Hard-ish | U | Google Cloud docs, UNVERIFIED, last known |
| Cloud NAT | Usable ports per external IP; default min ports per VM | 64,512; 64 | Hard, Soft | U | Google Cloud docs, UNVERIFIED, last known |
| SQS | In-flight messages | 120,000 standard, 20,000 FIFO | Soft | U | AWS docs, UNVERIFIED, last known |
| SQS | FIFO throughput without high-throughput mode | 300 messages/s per API action, 3,000 with batching | Soft | U | AWS docs, UNVERIFIED, last known |
| SQS | Visibility timeout max; retention max; message size | 12 h; 14 days; 256 KiB historically, re-check for 1 MiB | Hard | U | AWS docs, UNVERIFIED, last known |
| Kinesis Data Streams | Per-shard write; read; GetRecords calls | 1 MiB/s or 1,000 records/s; 2 MiB/s; 5 calls/s | Hard | U | AWS docs, UNVERIFIED, last known |
| Pub/Sub | Message size; publish request | 10 MB; 10 MB and 1,000 messages | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Pub/Sub | Ack deadline; subscription retention max | 10 to 600 s; 7 days | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| BigQuery | Query execution time limit | 6 hours | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| BigQuery | Default on-demand slots per project | 2,000 | Soft | U | Google Cloud docs, UNVERIFIED, last known |
| AWS IAM | Managed policy size; roles per account | 6,144 characters; 1,000 | Hard, Soft | U | AWS docs, UNVERIFIED, last known |
| AWS IAM | Role chaining session cap | 1 hour | Hard | U | AWS docs, UNVERIFIED, last known |
| AWS KMS | Cryptographic request rate | Shared per account and Region, tiered 5,500, 10,000, or 30,000 requests/s by Region | Soft | U | AWS docs, UNVERIFIED, last known |
| Google Cloud IAM | Service accounts per project; keys per account | 100 default; 10 | Soft, Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Google Cloud IAM | Allow policy size | 1,500 principals | Hard | U | Google Cloud docs, UNVERIFIED, last known |
| Bedrock | Inference quota dimensions | RPM and TPM per model, per Region, per account, with output burndown | Soft | U | AWS docs, UNVERIFIED, last known |
| Vertex AI | Gemini quota model | Dynamic shared quota, Provisioned Throughput for reserved capacity | Design | U | Google Cloud docs, UNVERIFIED, last known |
| Trainium | Trn2 UltraServer | 64 chips | Hardware | U | AWS docs, UNVERIFIED, last known |
| Cloud TPU | v5p max pod; v6e pod | 8,960 chips; 256 chips | Hardware | U | Google Cloud docs, UNVERIFIED, last known |

KMS note: S3 Bucket Keys exist specifically to cut SSE-KMS request volume, because a bucket full of objects each needing a data key can saturate the shared KMS request quota (AWS docs, UNVERIFIED, last known).
The S3 performance page links readers to the KMS limits for the same reason (AWS docs, as of 2026-10).

Cross-reference: [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_03_VPC_Networking_Internals|VPC internals]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_04_Global_VPC_Andromeda_SDN_and_Network_Tiers|Global VPC and Cloud NAT]], [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_15_Security_and_Cryptography_KMS_and_Nitro_Enclaves|KMS]], [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI|Interconnects]].

## 5. Capacity calculator

This script is pure Python standard library and provisions nothing, so it has no cost, no budget alarm, and no teardown.
Every constant in it that maps to a `U` row must be replaced with the value from your account's quota page before you use the output.

```python
import math

def lambda_concurrency(rps, avg_s):
    return rps * avg_s

def lambda_ramp_seconds(target, initial=1000, step=1000, every=10):
    if target <= initial:
        return 0
    return math.ceil((target - initial) / step) * every

def eks_max_pods(enis, ips_per_eni):
    return enis * (ips_per_eni - 1) + 2

def gke_nodes_per_pod_range(pod_prefix, max_pods_per_node):
    per_node = 1 << math.ceil(math.log2(max_pods_per_node * 2))
    return (1 << (32 - pod_prefix)) // per_node, per_node

def aws_nat_ips(new_conn_per_s, avg_life_s, per_dest_limit=55000):
    return math.ceil(new_conn_per_s * avg_life_s / per_dest_limit)

def cloud_nat_ips(vms, ports_per_vm, usable_ports_per_ip=64512):
    return math.ceil(vms * ports_per_vm / usable_ports_per_ip)

def dynamodb_min_partitions(wcu, rcu):
    return max(math.ceil(wcu / 1000), math.ceil(rcu / 3000))

def llm_rpm(tpm_quota, in_tok, out_tok, max_tokens, burndown):
    admit = tpm_quota / (in_tok + max_tokens)
    settle = tpm_quota / (in_tok + burndown * out_tok)
    return admit, settle
```

Output from running the full script with the inputs below on 2026-10-21:

| Scenario | Inputs | Result |
|---|---|---|
| Lambda steady state | 2,000 rps, 0.4 s average duration | Concurrency 800 (Little's law: arrival rate times service time) |
| Lambda cold ramp | Target 5,000 concurrent, first 1,000 immediate | At least 40 s under the 1,000 per 10 s model |
| EKS max pods | 3 ENIs, 10 IPs each | 29 pods |
| GKE node capacity | /14 pod range, 110 pods per node | /24 per node, 1,024 nodes |
| AWS NAT | 3,000 new conn/s, 30 s lifetime, one destination | 90,000 concurrent, so at least 2 IPs (assuming additional IPs raise the per-destination ceiling, UNVERIFIED) |
| Cloud NAT | 500 VMs at 1,024 ports each | 8 external IPs; 1,008 VMs per IP at 64 ports |
| DynamoDB | 25,000 WCU and 30,000 RCU evenly spread | At least 25 partitions |
| S3 | 50,000 GET/s at 5,500 per prefix | At least 10 prefixes, assuming the guideline is the operating point |
| LLM gateway | 1,000,000 TPM (hypothetical), 3,000 in, 500 out, burndown 5, `max_tokens` 4,096 | Admission bound 140.9 RPM versus settlement bound 181.8 RPM, so 140.9 binds |
| LLM gateway tuned | Same, `max_tokens` 800 | Admission 263.2 RPM, so the settlement bound of 181.8 RPM binds, a 29 percent gain |
| Cloud Storage ramp | 5,000 to 80,000 reads/s, doubling every 20 min | 4 doublings, so about 80 minutes |

Reading the LLM rows: the binding constraint is the minimum of the two bounds, and the lever is `max_tokens` because you control it.

## 6. Quota operations runbook

Alert thresholds that survive review:

| Signal | Threshold | Action |
|---|---|---|
| Quota usage ratio | 70 percent warn, 85 percent page | Open increase request; increases can take days |
| Throttle or 429 rate | Over 1 percent of requests for 5 minutes | Check burst versus sustained, then retry policy |
| ENA allowance counters | Any increase | Investigate flow table, PPS, or resolver traffic |
| NAT allocation failures | Any | Add IPs, raise ports per VM, or move traffic to private endpoints |
| Pod IP free ratio | Under 20 percent | Plan CIDR expansion before the autoscaler hits it |

List and request AWS quotas with the Service Quotas API (CLI subcommands recalled, UNVERIFIED):

```bash
aws service-quotas list-service-quotas --service-code lambda
aws service-quotas request-service-quota-increase \
  --service-code lambda --quota-code <code-from-list> --desired-value 3000
```

Google Cloud exposes quota consumption as Cloud Monitoring metrics under `serviceruntime.googleapis.com/quota/` (Google Cloud docs, UNVERIFIED, last known).
AWS publishes usage metrics for many quotas in the `AWS/Usage` namespace (AWS docs, UNVERIFIED, last known).
Alarm on the ratio of usage to quota, not on raw usage, so a granted increase does not silently invalidate alert thresholds.

## 7. Failure modes and public incident analyses

| Pattern | Mechanism | Public analysis |
|---|---|---|
| Hidden OS-level limit hit during capacity add | Front-end fleet growth pushed thread counts past an OS configuration ceiling, and the failure propagated across servers. | AWS Kinesis us-east-1 event, 2020-11-25 ([AWS message 11201](https://aws.amazon.com/message/11201/), URL recalled, UNVERIFIED) |
| Retry and connection storm after internal capacity scaling | Automated scaling activity congested internal network devices, and client retries amplified the congestion. | AWS us-east-1 event, 2021-12-07 ([AWS message 12721](https://aws.amazon.com/message/12721/), URL recalled, UNVERIFIED) |
| Race in DNS automation for a regional endpoint | An automation race left an empty DNS record for DynamoDB, with a long recovery tail from dependent control planes. | AWS us-east-1 event, 2025-10-19 and 2025-10-20 ([AWS message 101925](https://aws.amazon.com/message/101925/), URL recalled, UNVERIFIED) |
| Globally replicated quota policy crashes the quota check | A policy record with blank fields reached the Service Control quota-check path, which crashed, and recovery was prolonged by a herd effect on restart without randomized backoff. | Google Cloud incident report, 2025-06-12 (on the Google Cloud status page, details recalled, UNVERIFIED) |

Lessons that generalize from these four:

- A quota or policy system is on the critical path of every API call, so its configuration must roll out in cells, never globally in seconds.
- Decide fail-open versus fail-closed per quota class, in writing, before the incident.
- Restarts need jittered, budgeted backoff, or the recovery itself is the second outage.
- Capacity adds can fail on limits that no console shows, so staged adds with a health gate matter.

Further mechanics in [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages|Anatomy of Major Cloud Outages]] and [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_03_Cell_Based_Architecture_and_Shuffle_Sharding|Cell-Based Architecture]].

## 8. Interview questions and model answers

### Q1 (L5): A training job's data loaders hit S3 and fail with 503 SlowDown at job start.

Model answer: The first question is whether the rate is sustained or a step.
S3 guarantees at least 3,500 write-class and 5,500 read-class requests per second per partitioned prefix and scales in response to sustained rates (AWS docs, as of 2026-10).
A thousand workers all requesting the first shard at t=0 is a step from near zero, so the partition topology lags, and SlowDown is the designed signal.
Fixes in order of cost: stagger worker start with random delay, retry with full jitter and a retry budget, spread shards over several prefixes (50,000 GET/s needs at least 10 prefixes at the guideline rate), use ranged GETs of larger sizes to cut request count, and cache hot shards on local NVMe.
For repeated checkpoint or shard access with tight latency, S3 Express One Zone documents up to 2 million TPS per directory bucket, at the cost of single-AZ placement (AWS FAQ, as of 2026-10).
Follow-up: How do you prove the fix without waiting for a production job?
Expected: a ramped synthetic load test against a prefix set, with the 503 rate per minute as the pass criterion and a comparison of step versus ramp starts.

### Q2 (L5): An API on Lambda sees 2,000 rps with a 400 ms average duration and spikes to 5,000 rps in a minute. What happens?

Model answer: Steady concurrency is 2,000 x 0.4 = 800 by Little's law.
Per-function scale-out is 1,000 new execution environments every 10 seconds (AWS docs, as of 2026-10).
Reaching 2,000 concurrent from a cold start takes about 10 to 20 seconds, so the spike is survivable if the account concurrency cap is not the first gate.
The account-level cap (default 1,000, soft, UNVERIFIED) is shared, so one noisy function can starve the others, and reserved concurrency both guarantees and caps a function.
The usual hidden failure is downstream: 2,000 concurrent environments open 2,000 database connections, so put a connection proxy in front or bound concurrency with reserved concurrency.
Follow-up: When do you pick provisioned concurrency over reserved concurrency?
Expected: provisioned removes cold starts for predictable load and costs while idle, and reserved only partitions the account cap.

### Q3 (L6): Pods stay Pending on EKS while the subnet has free IPs, and the same cluster design on GKE fails differently. Explain both.

Model answer: On EKS, max pods per node derives from ENI count and per-ENI addresses, `ENIs * (IPs - 1) + 2`, so an instance with 3 ENIs and 10 addresses each is capped at 29 pods regardless of CPU.
Free subnet IPs do not help because the limit is per-instance attachment geometry.
Prefix delegation hands out /28 blocks and raises the ceiling, but it needs contiguous free /28s, so a fragmented subnet can still fail.
On GKE, the node receives a pod CIDR sized to a power of two at or above twice max pods, so 110 pods uses a /24.
A /14 pod range thus holds 1,024 nodes, and the failure is node-pool autoscaling stalling at the range, which cannot be resized in place without adding ranges.
Both are design-time geometry problems, not quota requests.
Follow-up: How would you size a pod range for a cluster you expect to grow 8x?
Expected: derive nodes from per-node CIDR, reserve a larger non-overlapping range up front, and consider IPv6 or secondary ranges.

### Q4 (L6): One in a thousand outbound connections from a fleet time out. Metrics show low NAT bandwidth. Diagnose.

Model answer: Low bandwidth with sporadic failures points to a state table, not throughput.
Candidates are NAT port exhaustion, connection-tracking exhaustion on the instance, and resolver packet limits.
For AWS NAT, check `ErrorPortAllocation`: the 55,000 simultaneous connections cap is per unique destination, so a fleet hammering one SaaS IP and port hits it at far lower aggregate traffic than a diverse workload (AWS docs, UNVERIFIED, last known).
For Cloud NAT, each VM gets a fixed minimum port allocation, so one chatty VM exhausts its own ports while the gateway looks idle.
On the instance, read `conntrack_allowance_exceeded` and `linklocal_allowance_exceeded` from `ethtool -S`.
Remedies: connection pooling and keep-alive to cut new-connection rate, shorter idle timeouts for tracked flows, extra NAT IPs or higher ports per VM, per-AZ NAT, and private endpoints that bypass NAT for cloud APIs.
Follow-up: Why not just add NAT IPs everywhere?
Expected: each IP is an allowlisting and cost item, and the real fix is reducing connections per destination.

### Q5 (L6): Your LLM gateway gets 429s from Bedrock while its dashboard shows 40 percent of TPM used. Why, and what do you change?

Model answer: Token quotas are enforced at admission against an estimate, input tokens plus requested `max_tokens`, and settled afterward on actual usage with output weighted by a burndown multiplier (AWS docs, UNVERIFIED, last known).
A dashboard of settled usage understates admitted pressure whenever `max_tokens` far exceeds actual output.
In the worked example, 3,000 input tokens, 500 output tokens, burndown 5, and `max_tokens` of 4,096 admit only 140.9 RPM from a 1,000,000 TPM quota, whereas tuning `max_tokens` to 800 reaches the settlement bound of 181.8 RPM.
Changes: set `max_tokens` per route from the observed p99 output length, estimate tokens in the gateway with a local token bucket that mirrors the provider's accounting, shed or queue low-priority traffic before the provider does, and use cross-region inference or Provisioned Throughput for baseline load.
Follow-up: What would make you distrust your gateway's local estimate?
Expected: tokenizer mismatch across models, cache-read accounting differences, and provider-side changes to burndown rates, so reconcile against provider-reported usage.

### Q6 (L6): A single DynamoDB item takes 3,000 writes per second at 1 KB. Quota raises do not help. Why?

Model answer: An item lives in one partition, and a partition's write capacity is on the order of 1,000 WCU/s, so 3,000 WCU/s for one key exceeds the ceiling (AWS docs, UNVERIFIED, last known).
Adaptive capacity moves capacity between partitions and can isolate a hot item onto its own partition, but cannot exceed one partition's rate.
Raising table-level quota raises an admission number that is not the binding constraint.
Fix at the data model: shard the key into N suffixed items and aggregate on read, or accumulate counters in a stream or in-memory aggregator and flush periodically.
The cost is read amplification and a merge step, and the shard count must be fixed or versioned so readers find all shards.
Follow-up: How do you choose N?
Expected: N at least target rate divided by per-partition rate with 2x headroom, so 3,000 WCU/s needs at least 6 shards, and bias toward more because splits cannot be forced.

### Q7 (L7): Design quota governance for 500 internal teams across 200 AWS accounts and 200 GCP projects, including how it fails.

Model answer: Treat quota as a product with a three-layer model.
Layer one is the provider limit, which you cannot change.
Layer two is platform allocation: a central ledger assigns each team a share of scarce pools (accelerators, KMS request rate, NAT IPs, LLM TPM) and enforces it at your own gateway, so teams hit your 429 with an actionable message before the provider's opaque one.
Layer three is per-account provider quotas, requested through automation and stored as code, with usage-ratio alarms at 70 and 85 percent.
Failure design matters more than allocation.
The Google Cloud incident of 2025-06-12 showed a quota policy change replicating globally and crashing the check path (Google Cloud status page, details recalled, UNVERIFIED), so your own quota config needs cell-by-cell rollout, validation of schema and non-empty fields before publish, last-known-good caching on the data plane, and jittered restart backoff.
Decide fail-open or fail-closed per pool: fail open for low-risk read paths to protect availability, fail closed for spend-bearing pools such as GPUs and LLM tokens.
Follow-up: What is the risk of fail-open on the token pool?
Expected: a bounded cost blowout during the outage, mitigated by a hard budget alarm and a per-team circuit breaker that does not depend on the quota service.

### Q8 (L7): You have an approved GPU quota for 2,048 accelerators in one Region but the training run cannot start. What is your plan?

Model answer: Quota is permission, and the failure is a capacity or topology problem (see section 1.1).
Training needs a contiguous, same-fabric block, so scattered spare hosts do not satisfy it.
The plan has four tracks.
First, convert intent into a reservation: AWS capacity reservations or Capacity Blocks for a dated window, or Google Cloud reservations or calendar-style scheduling, with the lead time and terms verified at purchase.
Second, relax the topology: accept 90 to 95 percent of the target size plus hot spares, and rely on checkpoint-and-resume instead of a single perfect cluster.
Third, diversify across two zones or Regions for the spare pool only, because cross-zone sync in the main ring costs step time.
Fourth, price the idle reservation: a reservation held for weeks before the data and code are ready burns real money, so tie the booking date to the readiness gate, not the calendar.
Follow-up: When do you abandon the Region?
Expected: when the expected wait times the cost of delay exceeds the penalty of moving data and rebuilding the cluster, which should be computed before the decision, not during it.

## 9. Further reading

- [S3 request rate and performance guidelines](https://docs.aws.amazon.com/AmazonS3/latest/userguide/optimizing-performance.html) - the primary statement of per-prefix rates and the sustained-rate scaling behavior.
- [Amazon S3 FAQs](https://aws.amazon.com/s3/faqs/) - the source for the 2 million TPS directory bucket figure.
- [Prime Video on S3 Express One Zone](https://aws.amazon.com/blogs/storage/prime-video-improved-stream-analytics-performance-with-amazon-s3-express-one-zone/) - a measured checkpoint-spike workload with the 503 rate reported.
- [Lambda quotas](https://docs.amazonaws.cn/en_us/lambda/latest/dg/gettingstarted-limits.html) - the scale-out rate per function, on a docs mirror.
- AWS Builders' Library article on timeouts, retries, and backoff with jitter - why a rate limit is only as good as the retry policy in front of it (search by title, URL not verified).
- Google SRE book chapters on handling overload and addressing cascading failures - the origin of load shedding and retry budgets.
- Internal sibling chapters: [[17-Cloud-AI-Engineering/Appendices/Appendix_B_AWS_to_GCP_Mechanism_Translation_Map|Appendix B]] for service mapping, [[17-Cloud-AI-Engineering/Appendices/Appendix_D_Essential_Papers_and_Primary_Sources|Appendix D]] for papers, and [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_06_Capacity_and_Quotas|Agentic AI: Capacity and Quotas]] for the application-side view.

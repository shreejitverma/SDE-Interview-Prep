---
type: concept
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Appendix E: 16-Week Mastery Study Plan for Senior Engineers

This plan converts the curriculum into 16 weekly work packages.
Each week has a reading allocation, one hands-on milestone, one mechanism that getting-started guides omit, and one timed interview drill.
Model answers for the hardest drills are in the defense drill section after the weekly plan.
Companion references are [[17-Cloud-AI-Engineering/Appendices/Appendix_C_Limits_Quotas_and_Thresholds_Matrix|Appendix C]] for limits, [[17-Cloud-AI-Engineering/Appendices/Appendix_D_Essential_Papers_and_Primary_Sources|Appendix D]] for primary sources, and [[17-Cloud-AI-Engineering/Appendices/Appendix_A_Glossary|Appendix A]] for terminology.

## 1. Operating Assumptions

The plan assumes about 12 hours per week: 4 hours reading, 5 hours lab, 2 hours drill, 1 hour spaced review.
Total effort is about 190 hours.
At 8 hours per week, drop every Tier B item and run the plan over 24 weeks.
Tier A means read in full and be able to redraw the mechanism from memory.
Tier B means skim in 30 minutes and extract one mechanism and one number.
Week 0 is a diagnostic: attempt drill D1 and drill D4 cold, and skip any Tier A chapter where you can already score "pass" against the rubric in section 5.
Spaced review rule: each week, re-answer one drill from week N-1 and one from week N-4 without notes.

This plan deliberately does not hard-code list prices for GPU or TPU instances.
Prices and quotas change monthly, and a stale number in a study plan is worse than none.
Every billable lab therefore starts with the price-query and estimate guard in section 2.

## 2. Lab Guardrails (apply to every billable lab)

Rule 1: one project or account per lab family, so teardown is a single deletion.
Rule 2: every lab gets a budget alert before the first resource is created.
Rule 3: budgets are alerts, not caps; spend data lags real usage, so an alert fires after the money is gone (exact refresh cadence UNVERIFIED).
Rule 4: run the estimate guard below, and abort if the estimate exceeds the lab budget.
Rule 5: tag or label every resource with `caie-lab`, and audit for leftovers after teardown.
Rule 6: GPU quotas often default low or zero on new accounts, so request them in week 1 and do not make weeks 1 to 12 depend on GPUs (check with `aws service-quotas list-service-quotas --service-code ec2` and `gcloud compute regions describe REGION`).

AWS budget alarm (80 percent actual, 100 percent forecast):

```bash
ACCT=$(aws sts get-caller-identity --query Account --output text)
aws budgets create-budget --account-id "$ACCT" \
  --budget '{"BudgetName":"caie-lab","BudgetLimit":{"Amount":"25","Unit":"USD"},"TimeUnit":"MONTHLY","BudgetType":"COST"}' \
  --notifications-with-subscribers '[
   {"Notification":{"NotificationType":"ACTUAL","ComparisonOperator":"GREATER_THAN","Threshold":80,"ThresholdType":"PERCENTAGE"},
    "Subscribers":[{"SubscriptionType":"EMAIL","Address":"you@example.com"}]},
   {"Notification":{"NotificationType":"FORECASTED","ComparisonOperator":"GREATER_THAN","Threshold":100,"ThresholdType":"PERCENTAGE"},
    "Subscribers":[{"SubscriptionType":"EMAIL","Address":"you@example.com"}]}]'
```

GCP budget alarm:

```bash
gcloud billing budgets create --billing-account="$BILLING_ACCOUNT" \
  --display-name="caie-lab" --budget-amount=25USD \
  --threshold-rule=percent=0.5 --threshold-rule=percent=0.9 \
  --threshold-rule=percent=1.0,basis=forecasted-spend
```

Estimate guard (refuses to proceed when the estimate exceeds the budget):

```bash
#!/usr/bin/env bash
# usage: lab_guard.sh INSTANCE_TYPE HOURS COUNT BUDGET_USD [PRICE_PER_HOUR]
set -euo pipefail
IT="$1"; HRS="$2"; N="$3"; BUDGET="$4"; PRICE="${5:-}"
if [ -z "$PRICE" ]; then
  # Conservative proxy for Spot labs: highest current Spot price across AZs.
  PRICE=$(aws ec2 describe-spot-price-history --instance-types "$IT" \
    --product-descriptions "Linux/UNIX" --start-time "$(date -u +%Y-%m-%dT%H:%M:%SZ)" \
    --query 'max_by(SpotPriceHistory,&to_number(SpotPrice)).SpotPrice' --output text)
fi
# 1.25 headroom covers EBS, data transfer, and a forgotten hour.
EST=$(awk -v p="$PRICE" -v h="$HRS" -v n="$N" 'BEGIN{printf "%.2f", p*h*n*1.25}')
echo "price=$PRICE/h hours=$HRS count=$N estimate=$EST budget=$BUDGET"
awk -v e="$EST" -v b="$BUDGET" 'BEGIN{exit !(e<=b)}' || { echo "ABORT: estimate exceeds budget"; exit 1; }
```

For On-Demand labs, pass the hourly price from the AWS pricing page or Pricing API as the fifth argument.
For GCP, take prices from the Cloud Billing pricing pages at run time and apply the same formula by hand.

Teardown patterns used below:

```bash
# AWS: audit for leftovers after any destroy
aws resourcegroupstaggingapi get-resources --tag-filters Key=caie-lab \
  --query 'ResourceTagMappingList[].ResourceARN' --output text
# GCP: delete the whole lab project (30-day recoverable pending deletion)
gcloud projects delete "$LAB_PROJECT" --quiet
```

## 3. Plan at a Glance

| Week | Theme | Hands-on milestone | Drill |
|---|---|---|---|
| 1 | Virtualization, fabrics, DNS | Firecracker snapshot clones on nested-virt GCE | D1 |
| 2 | Storage media, failure domains, static stability | Zonal shift drill on a 3-AZ service | D2 |
| 3 | Cells, shuffle sharding, planes | Shuffle-shard simulator, 3 numbers reproduced | D2 |
| 4 | Identity, accounts, VPC, Nitro | Cross-account access vs simulator vs reality | D7 |
| 5 | Serverless, containers, S3, EBS | S3 Express vs S3 Standard latency benchmark | D1 |
| 6 | Distributed databases | DynamoDB hot-key experiment | D3 |
| 7 | GCP networking and security | VPC SC dry-run perimeter | D7 |
| 8 | AWS to GCP translation | Same service on Lambda and Cloud Run | D8 |
| 9 | Kubernetes platform | Stale resourceVersion conflict demo, Capstone 2 | D4 |
| 10 | Delivery and SRE core | Retry-storm injection, Capstone 1 | D4 |
| 11 | Outages and security | OIDC trust and KMS encryption context lab | D7 |
| 12 | FinOps and accelerators | Spot interruption handler with Young/Daly interval | D5 |
| 13 | Training infrastructure | FSDP memory and collective measurements | D9 |
| 14 | LLM inference mechanics | vLLM vs roofline prediction | D6 |
| 15 | Disaggregation, autoscaling, managed inference | Scale-from-zero TTFT, Capstone 3 | D6 |
| 16 | Applications and interview integration | Written design doc plus two mock loops | D1 to D9 |

Mock loops at weeks 4, 8, 12, and 16 are full 45-minute sessions with a peer or a recorded self-review.
At week 4 use D2, at week 8 use D3, at week 12 use D8, at week 16 use a fresh system design from Volume 14.

## 4. Weekly Plan

### Week 1 - Virtualization, Fabrics, and Name Resolution

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_01_Hypervisors_MicroVMs_and_Sandboxing|Hypervisors, MicroVMs, and Sandboxing]]
- [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_02_Datacenter_Networking_Fabrics|Datacenter Networking Fabrics]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_04_Firecracker_MicroVM|Firecracker (NSDI 2020)]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_03_DNS_Anycast_and_Global_Traffic_Management|DNS, Anycast, and Global Traffic Management]]

Lab: boot Firecracker on a GCE VM with nested virtualization, snapshot a booted guest, and restore two clones.
Setup: `gcloud compute instances create fc-lab --zone=us-central1-a --machine-type=n2-standard-4 --enable-nested-virtualization --labels=caie-lab=w01`.
Milestone: record cold boot time and snapshot-restore time, then read 16 bytes of `/dev/urandom` in both clones immediately after restore and compare.
Budget: 10 USD alert, a few instance-hours; run the estimate by hand from the current n2 price.
Teardown: `gcloud projects delete "$LAB_PROJECT" --quiet`.

Mechanism not in getting-started guides: snapshot restore clones guest memory, so the kernel entropy pool and any in-memory TLS or session state are duplicated across clones.
Two clones can emit identical "random" output until the kernel reseeds, which is a real hazard for key generation and request IDs in snapshot-based serverless.
Whether your guest kernel and VMM version reseed on restore (via a VM generation identifier) is something you must test, not assume.
Second mechanism: ECMP hashes the 5-tuple, so one long-lived flow pins to one path, and a few elephant flows collide on a link while the fabric looks underloaded.
This is the root reason AI fabrics use packet spraying (AWS SRD) or optical circuit scheduling (TPU ICI) rather than plain ECMP, covered in Week 13.

Drill: D1 cold, then re-draw the Firecracker device model from memory.
Exit criterion: you can explain why a microVM has a smaller attack surface than a container sharing a host kernel, and name one cost of that choice.

### Week 2 - Storage Media, Failure Domains, and Static Stability

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_05_Storage_Media_and_Block_Storage_Foundations|Storage Media and Block Storage Foundations]]
- [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_06_Physical_Failure_Domains_and_Blast_Radius|Physical Failure Domains and Blast Radius]]
- [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability|Regions, Zones, and Static Stability]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_04_Edge_Infrastructure_and_TLS_Termination|Edge Infrastructure and TLS Termination]]
- [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_01_Well_Architected_Frameworks|Well-Architected Frameworks]]

Lab: deploy a 3-AZ service (OpenTofu) with capacity sized so that losing one AZ leaves 100 percent of demand served.
Milestone: use `aws arc-zonal-shift start-zonal-shift` to drain one AZ at the load balancer, and show from metrics that the remaining AZs absorbed load with zero control-plane calls (no scaling action) during the shift.
Budget: 20 USD alert; run the guard with your instance type, 3 hours, 6 instances.
Teardown: `tofu -chdir=labs/w02 destroy -auto-approve`, then the tag audit.

Mechanism not in getting-started guides: static stability is arithmetic, not a slogan.
With k AZs and a requirement to survive loss of one without scaling, each AZ may carry at most (k-1)/k of its own capacity, so provisioned capacity must be k/(k-1) times demand.
That is 1.5x for three AZs and 2x for two AZs, which is why "two AZs, scale on failure" is not statically stable: it depends on the EC2 control plane being healthy exactly when an AZ is impaired.
Corollary: an Auto Scaling reaction is a control-plane dependency; pre-provisioned headroom is a data-plane property.

Drill: D2.
Exit criterion: you can compute the headroom cost of 2, 3, and 4 AZ designs and argue which one a given SLO justifies.

### Week 3 - Cells, Shuffle Sharding, and Plane Separation

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_03_Cell_Based_Architecture_and_Shuffle_Sharding|Cell-Based Architecture and Shuffle Sharding]]
- [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_04_Control_Plane_vs_Data_Plane_Separation|Control Plane vs Data Plane Separation]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_03_Physalia_EBS_Control_Plane|Physalia (NSDI 2020)]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_05_Constant_Work_and_Workload_Smoothing|Constant Work and Workload Smoothing]]
- [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_06_Multi_Account_and_Multi_Project_Landing_Zones|Multi-Account and Multi-Project Landing Zones]]
- [[08-Distinguished-Engineering/02-Distributed-Systems-Internals/Consistent-Hash-Ring|Consistent Hash Ring]]

Lab: write a 30-line Python simulator with 8 workers, shard size 2, 1,000 clients, and one poison client that kills every worker it touches.
Milestone: reproduce three numbers from your simulator output: 28 distinct shards, about 3.6 percent of clients sharing both workers with the poison client, and about 46.4 percent sharing at least one.
Cost: none.

Mechanism not in getting-started guides: shuffle sharding trades total-outage probability for partial-degradation probability.
With 8 workers and shard size 2 there are C(8,2) = 28 shards, so a full-overlap victim set is 1/28 of clients.
The share of clients overlapping on at least one worker is 1 - C(6,2)/C(8,2) = 13/28, so nearly half see degraded capacity, which is acceptable only if clients retry across their shard.
With 100 workers and shard size 5 there are C(100,5) = 75,287,520 shards, which is why the technique pays off at fleet scale and barely at all for 8 nodes.
Physalia's lesson is the dual: compartmentalize the control plane by cell so that a bad configuration reaches a bounded set of volumes.

Drill: D2 again, now with a poison-pill request as the failure.
Exit criterion: you can choose shard size for a given fleet size and quote the blast radius as a fraction.

### Week 4 - Identity, Accounts, VPC, and Nitro

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_01_IAM_Policy_Evaluation_Engine|IAM Policy Evaluation Engine]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_03_VPC_Networking_Internals|VPC Networking Internals]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_04_EC2_Nitro_Architecture_and_Graviton|EC2, Nitro Architecture, and Graviton]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_02_Cloud_IAM_Roles_Policies_and_Conditions|Cloud IAM: Roles, Policies, and Conditions]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_02_AWS_Organizations_and_Account_Governance|AWS Organizations and Account Governance]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_01_Resource_Hierarchy_and_Organization_Policies|Resource Hierarchy and Organization Policies]]

Lab: build a cross-account role with an SCP, a permission boundary, and a session policy, then predict allow or deny for 10 requests before testing.
Milestone: compare `aws iam simulate-principal-policy` output with the real result of each call, and list every case where the simulator and reality differed and why.
GCP equivalent: add a deny policy and run `gcloud policy-troubleshoot iam` for the same 10 requests.
Cost: negligible; Organizations features are free of direct charge, but alerts stay on.
Teardown: `tofu -chdir=labs/w04 destroy -auto-approve`.

Mechanism not in getting-started guides: guardrails never grant.
An SCP, permission boundary, or session policy only narrows the maximum; an allow must still come from an identity or resource policy.
Within one account an allow in either the identity policy or the resource policy suffices, while cross-account access needs an allow on both sides.
The simulator evaluates the policies you hand it, so it cannot see an SCP, resource policy, or session policy you forgot to include, which is the usual source of simulator-versus-reality gaps.
GCP differs structurally: deny policies are evaluated before allow bindings and cannot be overridden by a grant lower in the hierarchy.

Drill: mock loop 1 with D2, then D7 (read the model answer only after attempting it).
Exit criterion: you can walk a request through every layer that can deny it, in order, without notes.

### Week 5 - Serverless, Containers, Object Storage, and Block Storage

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_05_Serverless_Compute_and_Lambda_Internals|Serverless Compute and Lambda Internals]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_08_S3_Deep_Dive_Consistency_and_ShardStore|S3 Deep Dive: Consistency and ShardStore]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_09_Elastic_Block_Store_and_Network_Storage|Elastic Block Store and Network Storage]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_05_Amazon_S3_ShardStore_Formal_Methods|ShardStore Formal Methods (SOSP 2021)]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_06_Container_Services_ECS_and_Fargate|Container Services: ECS and Fargate]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_07_Amazon_EKS_Control_Plane_and_Networking|Amazon EKS: Control Plane and Networking]]
- [[04-System-Design/05-Data-Stores/Amazon-S3-and-Object-Storage|Amazon S3 and Object Storage]]

Lab: benchmark small-object GET latency, p50 and p99 over 100,000 requests, from one EC2 instance in the same AZ against S3 Standard and an S3 Express One Zone directory bucket.
Milestone: report the p99 to p50 ratio for each, and the effect of connection reuse versus new connections.
Facts to anchor: S3 Express One Zone is a single-zone storage class designed for consistent single-digit millisecond access, with up to 10x faster access and request costs 80 percent lower than S3 Standard (AWS docs, as of 2026-10, [S3 directory buckets](https://docs.aws.amazon.com/AmazonS3/latest/userguide/directory-bucket-high-performance.html)).
Every connector or client using directory buckets needs `s3express:CreateSession` in its IAM policy (AWS re:Post, as of 2026-10, [connector guidance](https://repost.aws/articles/AROs3zfLYxT56d3MAKp28Tqg/choosing-an-s3-connector-for-ml-training-with-s3-express-one-zone)).
Budget: 15 USD alert; guard with a small instance type, 2 hours, 1 instance.
Teardown: `tofu -chdir=labs/w05 destroy -auto-approve`, after emptying buckets with `aws s3 rm s3://BUCKET --recursive`.

Mechanism not in getting-started guides: directory buckets use a session-based authorization model in which `CreateSession` returns short-lived credentials that the SDK reuses, so the per-request authorization cost is amortized rather than paid on every call (design rationale from AWS material, not re-verified, see UNVERIFIED).
A single-AZ class also moves durability from "three or more AZs" to "one AZ", so the failure domain of your training data becomes the AZ, and the cost of a zonal event is a re-ingest.
Lambda mechanism: SnapStart-style restore trades init time for the Week 1 uniqueness hazard, so test any code that caches randomness, tokens, or connections during init.

Drill: D1 (DynamoDB-style hot spot reasoning generalizes to S3 prefix and EBS volume throughput limits).
Exit criterion: you can state when S3 Express is the wrong choice (multi-AZ durability requirement, cold archival data, mostly large sequential streams).

### Week 6 - Distributed Databases

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_10_Amazon_DynamoDB_Architecture_and_Partitioning|Amazon DynamoDB: Architecture and Partitioning]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_11_Amazon_Aurora_Log_Is_The_Database|Amazon Aurora: Log-Is-The-Database]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_10_Cloud_Spanner_TrueTime_and_External_Consistency|Cloud Spanner: TrueTime and External Consistency]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_01_Dynamo_and_DynamoDB|Dynamo and DynamoDB]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_02_Amazon_Aurora|Amazon Aurora (SIGMOD 2017)]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_06_Google_Spanner|Google Spanner (OSDI 2012)]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_12_Aurora_DSQL_Multi_Region_Active_Active_SQL|Aurora DSQL]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_11_Cloud_Bigtable_LSM_Storage_and_Tablet_Serving|Cloud Bigtable]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_09_Cloud_Storage_and_Colossus_Filesystem|Cloud Storage and Colossus]]
- [[08-Distinguished-Engineering/02-Distributed-Systems-Internals/Raft-Consensus|Raft Consensus]]

Lab: create a provisioned-capacity DynamoDB table, drive writes to a single partition key above the per-partition write ceiling, and observe throttling while table-level capacity is mostly idle.
Milestone: fix it with write sharding (key suffix 0..N-1), prove the fix with the throttle metrics, and state the read-side cost of the fix (scatter-gather over N keys).
Budget: 10 USD alert; use a small provisioned capacity and a 30-minute window.
Teardown: `aws dynamodb delete-table --table-name caie-w06`.

Mechanism not in getting-started guides: DynamoDB enforces throughput per partition, not per table, so a table provisioned for 10,000 WCU can still reject writes to one hot key at roughly the per-partition write ceiling (commonly cited as 1,000 WCU, 3,000 RCU, and 10 GB per partition; primary-source confirmation UNVERIFIED, see below).
Aurora's quorum arithmetic: 6 copies across 3 AZs, write quorum 4, read quorum 3, so Vr + Vw = 7 > 6 and Vw = 4 > 6/2 (Aurora paper, SIGMOD 2017).
Losing an AZ leaves 4 copies, so writes continue; losing an AZ plus one more node leaves 3 copies, so reads continue but writes block until repair, which the paper bounds by segmenting storage into small protection groups for fast re-replication.
Spanner commit wait: a transaction picks a commit timestamp no earlier than TrueTime's latest bound and then waits until that timestamp is definitely in the past, so average commit latency carries roughly twice the clock-uncertainty bound (Spanner paper, OSDI 2012, which reports uncertainty sawtooth in the low single-digit milliseconds).
The consequence is that external consistency is bought with wall-clock waiting, and the price falls as clocks improve, not as you add nodes.

Drill: D3.
Exit criterion: you can choose among DynamoDB, Aurora, and Spanner for a given consistency, latency, and geography requirement and quote the mechanism that dominates cost.

### Week 7 - GCP Networking, Load Balancing, Compute, and Perimeters

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_03_VPC_Service_Controls_Security_Perimeters|VPC Service Controls: Security Perimeters]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_04_Global_VPC_Andromeda_SDN_and_Network_Tiers|Global VPC, Andromeda SDN, and Network Tiers]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_05_Cloud_Load_Balancing_GFEs_and_Maglev|Cloud Load Balancing: GFEs and Maglev]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_11_Maglev_Network_Load_Balancer|Maglev (NSDI 2016)]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_06_Compute_Engine_and_Titanium_Architecture|Compute Engine and Titanium Architecture]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_07_GKE_Deep_Dive_Standard_vs_Autopilot|GKE Deep Dive]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_08_Cloud_Run_and_Serverless_Container_Execution|Cloud Run and Serverless Container Execution]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_12_Andromeda_Network_Virtualization|Andromeda (NSDI 2018)]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_13_Jupiter_Datacenter_Network|Jupiter (SIGCOMM 2015)]]

Lab: place a Cloud Storage bucket and a BigQuery dataset in a VPC Service Controls perimeter in dry-run mode, then generate violations from outside the perimeter.
Milestone: read the dry-run violations in the audit logs, write the minimum ingress rule that fixes a legitimate caller, and only then enforce.
VPC SC requires an organization resource, so use a sandbox organization.
Budget: 15 USD alert.
Teardown: delete the perimeter first with `gcloud access-context-manager perimeters delete PERIMETER --policy=POLICY_ID`, then `gcloud projects delete "$LAB_PROJECT" --quiet`.

Mechanism not in getting-started guides: Maglev's lookup table is a prime-sized array filled by having each backend walk its own permutation (offset and skip derived from two hashes) and claim the next free slot in round-robin order.
That produces near-equal share per backend and bounded disruption when a backend leaves, without per-connection state in the common case; connection tracking is only a fallback for table changes (Maglev paper, NSDI 2016).
VPC SC acts at the Google API layer, not the packet layer, so a perimeter does not stop data leaving through a VM's public internet egress, only through in-scope Google APIs.

Drill: D7.
Exit criterion: you can state exactly which exfiltration paths a VPC SC perimeter closes and which it leaves open.

### Week 8 - AWS to GCP Translation and Synthesis

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_01_Compute_Translation|Compute Translation]]
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_02_Networking_and_Load_Balancing_Translation|Networking and Load Balancing Translation]]
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_03_Identity_Security_and_Governance_Translation|Identity, Security, and Governance Translation]]
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_04_Storage_and_Database_Translation|Storage and Database Translation]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_05_Messaging_Streaming_and_Analytics_Translation|Messaging, Streaming, and Analytics Translation]]
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_06_AI_and_ML_Platform_Translation|AI and ML Platform Translation]]
- [[17-Cloud-AI-Engineering/Volume_04_AWS_vs_GCP_Translation/Chapter_07_Multi_Cloud_and_Hybrid_Architecture_Reality|Multi-Cloud and Hybrid Architecture Reality]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_13_Messaging_and_Streaming_SQS_SNS_EventBridge_and_MSK|Messaging and Streaming on AWS]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_14_Messaging_and_Streaming_Cloud_Pub_Sub_and_Dataflow|Pub/Sub and Dataflow]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_14_Observability_CloudWatch_Metrics_Logs_and_X_Ray|AWS Observability]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_16_Observability_Cloud_Monitoring_Logging_and_Trace|GCP Observability]]

Lab: deploy one stateless HTTP container to Lambda (container image) and to Cloud Run, then run the same open-loop load profile against both.
Milestone: produce a table of cold-start p50 and p99, and cost per million requests computed from current price pages, and explain the difference in terms of the concurrency model (one in-flight request per Lambda execution environment versus multiple per Cloud Run instance).
Budget: 10 USD alert on each cloud.
Teardown: `tofu -chdir=labs/w08 destroy -auto-approve` and `gcloud run services delete caie-w08 --region=us-central1 --quiet`.

Mechanism not in getting-started guides: AWS Availability Zone names are mapped per account, so `us-east-1a` in your account may be a different physical zone than in another account.
Use AZ IDs (for example `use1-az1`) whenever two accounts must co-locate, such as a producer and consumer sharing a PrivateLink service or a cross-account data path where inter-AZ transfer cost matters.
GCP zone names are consistent across projects, which removes this class of bug but means the translation table must record the asymmetry.
The structural difference to internalize is regional VPC plus Transit Gateway on AWS versus a global VPC on GCP, because it changes the blast radius of one bad route or firewall change.

Drill: mock loop 2 with D3, then D8.
Exit criterion: you can translate any AWS design into GCP and name two places where the translation is not one-to-one.

### Week 9 - Kubernetes Platform

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_01_Kubernetes_Control_Loop_Internals|Kubernetes Control Loop Internals]]
- [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_02_Advanced_Scheduling_and_Topology_Aware_Placement|Advanced Scheduling and Topology-Aware Placement]]
- [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_04_Autoscaling_at_Scale_HPA_KEDA_and_Karpenter|Autoscaling at Scale]]
- [[04-System-Design/07-Infrastructure-and-Orchestration/Kubernetes-Architecture|Kubernetes Architecture]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_03_CNI_and_eBPF_with_Cilium|CNI and eBPF with Cilium]]
- [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_08_Service_Mesh_Architecture_and_Trade_Offs|Service Mesh Architecture and Trade-Offs]]
- [[17-Cloud-AI-Engineering/Volume_06_Infrastructure_as_Code_and_Delivery/Chapter_01_Terraform_and_OpenTofu_Internals|Terraform and OpenTofu Internals]]
- [[17-Cloud-AI-Engineering/Volume_06_Infrastructure_as_Code_and_Delivery/Chapter_05_State_Management_Drift_Detection_and_Compliance|State Management and Drift]]

Lab: build [[17-Cloud-AI-Engineering/Volume_15_Capstones_Production_Systems/Chapter_02_Capstone_Production_Kubernetes_Platform_with_GitOps|Capstone 2]] on kind first, and use cloud clusters only for the final 3 hours.
Milestone: with the Python Kubernetes client, read an object, update it elsewhere, then submit your stale copy and capture the 409 Conflict, and write the retry loop that re-reads and re-applies.
Budget: 30 USD alert across EKS and GKE; the guard applies to node instance hours, and managed control planes bill hourly in addition, so price them from the current pages.
Teardown: `tofu -chdir=labs/w09 destroy -auto-approve` for both clouds, then the tag audit and `gcloud container clusters list`.

Mechanism not in getting-started guides: controllers are level-triggered over a deduplicating work queue.
Many events for one object collapse into one reconcile, so a reconciler must compute desired minus observed from current state and never rely on seeing each event.
Writes use optimistic concurrency on `resourceVersion`, and informers serve reads from a local cache that can be stale, so "read then write" without a conflict-aware retry is a latent bug.
Karpenter-style provisioning bin-packs pending pods into instance types it chooses at runtime, which is why node pools of fixed instance types are the cost anti-pattern for spiky workloads.

Drill: D4 (apply the retry-amplification analysis to controller retries against the API server).
Exit criterion: you can explain why an operator that "missed an event" is a design bug rather than an infrastructure bug.

### Week 10 - Delivery, SLOs, and Resilient Communication

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_01_SLIs_SLOs_and_Error_Budget_Engineering|SLIs, SLOs, and Error Budget Engineering]]
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_02_Load_Shedding_Backpressure_and_Admission_Control|Load Shedding, Backpressure, and Admission Control]]
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_03_Resilient_Communication_Retries_Jitter_and_Circuit_Breakers|Resilient Communication]]
- [[17-Cloud-AI-Engineering/Volume_06_Infrastructure_as_Code_and_Delivery/Chapter_06_Progressive_Delivery_and_Deployment_Pipelines|Progressive Delivery and Deployment Pipelines]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_06_Infrastructure_as_Code_and_Delivery/Chapter_04_Policy_as_Code_OPA_Sentinel_and_Guardrails|Policy as Code]]
- [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_07_GitOps_Delivery_Argo_CD_and_Flux|GitOps Delivery]]

Lab: build [[17-Cloud-AI-Engineering/Volume_15_Capstones_Production_Systems/Chapter_01_Capstone_Resilient_Serverless_Event_Pipeline|Capstone 1]], then inject a downstream brownout (50 percent of calls slowed to 5 seconds).
Milestone: show queue depth, DLQ rate, and retry count before and after adding a retry budget and jittered backoff, and attach the multi-window burn-rate alert definitions.
Budget: 10 USD alert on each cloud.
Teardown: `tofu -chdir=labs/w10 destroy -auto-approve`.

Mechanism not in getting-started guides: retries multiply across layers.
With three stacked layers that each make up to 3 attempts, one failing leaf call can see 3 x 3 x 3 = 27 attempts, and the load arrives exactly when the leaf is slowest.
Burn-rate alerting is an identity, not a rule of thumb: for a 30-day (720 hour) SLO, burning at 14.4x for 1 hour consumes 14.4/720 = 2 percent of budget, 6x for 6 hours consumes 5 percent, and 1x for 3 days consumes 10 percent (Google SRE Workbook alert windows).
Pair a long window with a short one (for example 1 hour and 5 minutes) so the alert both detects sustained burn and resets quickly after recovery.

Drill: D4.
Exit criterion: you can derive the burn-rate multiplier for any (budget fraction, window) pair.

### Week 11 - Outages, Identity Federation, and the Data Perimeter

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages|Anatomy of Major Cloud Outages]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_02_Workload_Identity_Federation|Workload Identity Federation]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_04_Encryption_Key_Hierarchies_and_Envelope_Encryption|Encryption Key Hierarchies and Envelope Encryption]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_06_Constructing_an_Impenetrable_Data_Perimeter|Constructing an Impenetrable Data Perimeter]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_04_Disaster_Recovery_Tiers_and_RTO_RPO_Engineering|Disaster Recovery Tiers]]
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_05_Chaos_Engineering_and_Game_Days|Chaos Engineering and Game Days]]
- [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_06_Incident_Response_Blameless_Postmortems_and_RCA|Incident Response and Postmortems]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_01_Zero_Trust_Architecture_in_Hyperscale_Clouds|Zero Trust Architecture]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_03_Secrets_Management_and_Dynamic_Credentials|Secrets Management]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore|SLSA and Sigstore]]
- [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_07_Compliance_Auditing_and_Non_Repudiation|Compliance and Auditing]]
- [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_15_Security_and_Cryptography_KMS_and_Nitro_Enclaves|AWS KMS and Nitro Enclaves]]
- [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_15_Security_and_Confidential_Computing_Cloud_KMS|Cloud KMS and Confidential Computing]]

Outage reading list (primary post-incident documents; read the timeline, then the contributing factors, then the corrective actions):
- AWS post-event summaries index: [AWS Post-Event Summaries](https://aws.amazon.com/premiumsupport/technology/pes/).
- Google Cloud incident reports: [Google Cloud Service Health incidents](https://status.cloud.google.com/summary).
- For each incident write three lines: the trigger, the amplifier, and the missing safeguard.
- Specific incident details you cite in an interview must be re-checked against the primary document, because secondary summaries drift.

Lab: federate a CI system to AWS and GCP through OIDC, with no static keys, and then break your own trust policy on purpose.
Milestone 1: write the trust condition on the OIDC subject claim, then demonstrate that removing the subject condition lets an unrelated repository in the same identity provider assume the role.
Milestone 2: encrypt an object with a KMS data key, bind an encryption context, and show that decryption fails with a different context.
Milestone 3: add an organization-ID condition to a bucket policy plus a VPC endpoint policy, and show a principal from another organization denied.
Cost: near zero; KMS keys cannot be deleted instantly.
Teardown: `tofu -chdir=labs/w11 destroy -auto-approve`, then `aws kms schedule-key-deletion --key-id KEY_ID --pending-window-in-days 7`.

Mechanism not in getting-started guides: the encryption context is authenticated additional data bound into the ciphertext and recorded in audit logs, so it is both an integrity control and a forensic field.
Envelope encryption means KMS sees only small data keys, so KMS request quotas bound your key-generation rate, not your byte throughput, and caching data keys with a bounded use count is the lever when a hot path hits the quota.
The common federation flaw is a trust policy that checks the issuer and audience but not the subject, which turns "any workflow on that platform" into "my role".

Drill: mock loop 3 uses D8; drill D7 as the security component.
Exit criterion: you can write the exact condition keys that make a federated role usable by one repository and one branch.

### Week 12 - FinOps and Accelerator Architecture

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_02_Commitment_Discounts_Savings_Plans_vs_CUDs|Commitment Discounts]]
- [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_03_Spot_Economics_and_Interruption_Handling|Spot Economics and Interruption Handling]]
- [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_05_GPU_and_TPU_Unit_Economics|GPU and TPU Unit Economics]]
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_01_Accelerator_Architectures_GPUs_Trainium_TPUs|Accelerator Architectures]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_01_Cloud_Billing_Models_and_Unit_Cost_Allocation|Billing Models and Unit Cost Allocation]]
- [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_04_Network_Egress_Economics_and_Optimization|Network Egress Economics]]
- [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_06_FinOps_Organizational_Practices_and_Automation|FinOps Organizational Practices]]
- [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_15_Google_TPU_Architecture_Evolution|TPU Architecture Evolution]]

Lab: write a Spot interruption handler and test it with a real injected interruption.
Milestone: use AWS Fault Injection Service (`aws:ec2:send-spot-instance-interruptions`) against a small Spot instance, measure how long your handler takes to flush state, and compare against the notice window.
Notice windows to design against: AWS Spot gives a two-minute interruption notice (AWS docs, as of 2026-10, [Spot interruption notices](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/spot-instance-termination-notices.html)).
Compute Engine gives a best-effort shutdown period of up to 30 seconds after an ACPI G2 soft-off signal (Google Cloud docs, as of 2026-10, [preemptible and Spot VMs](https://docs.cloud.google.com/compute/docs/instances/preemptible)).
Accelerator anchor: TPU v6e (Trillium) per chip is 918 TFLOPs bf16, 32 GB HBM, 1638 GBps HBM bandwidth, 800 GBps bidirectional ICI, 8 chips per host, 256-chip pod in a 2D torus (Google Cloud docs, as of 2026-10, [TPU v6e](https://docs.cloud.google.com/tpu/docs/v6e)).
Budget: 10 USD alert; guard with a small Spot instance type, 2 hours, 1 instance.
Teardown: `aws ec2 terminate-instances --instance-ids $(aws ec2 describe-instances --filters Name=tag-key,Values=caie-lab Name=instance-state-name,Values=running --query 'Reservations[].Instances[].InstanceId' --output text)`.

Mechanism not in getting-started guides: the Young/Daly optimal checkpoint interval is T = sqrt(2 x C x M), where C is checkpoint cost and M is mean time between interruptions of the whole job.
Expected waste is approximately C/T + T/(2M), minimized at that T, and both terms are equal at the optimum.
With C = 120 s and M = 8 hours, T is about 2,629 s (44 minutes) and total waste is about 9.1 percent.
A 120 s checkpoint cannot complete inside a 30 s preemption window and barely fits a 2-minute notice, so the notice is for flushing the last incremental delta, not for taking a full checkpoint.
Commitment break-even: with discount d, a commitment costs (1 - d) x P x H regardless of use, so it beats on-demand only when utilization u exceeds 1 - d, which is 70 percent for a 30 percent discount.
Roofline anchor: the ridge point for v6e is 918e12 / 1.638e12 = about 560 FLOPs per byte, so any kernel below that arithmetic intensity is bandwidth-bound (derived from the cited figures).

Drill: D5, then D8 mock loop 3.
Exit criterion: you can compute the checkpoint interval and the commitment break-even from raw inputs in under 3 minutes.

### Week 13 - Training Infrastructure

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI|Interconnect Technologies: NVLink, EFA, and TPU ICI]]
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_04_Distributed_Training_Parallelism_Strategies|Distributed Training Parallelism Strategies]]
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_05_Memory_Optimization_ZeRO_FSDP_and_JAX_Sharding|Memory Optimization: ZeRO, FSDP, and JAX Sharding]]
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_06_High_Throughput_Checkpointing_to_Object_Storage|High-Throughput Checkpointing]]
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale|Fault Tolerance and Fleet Resiliency]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_03_High_Performance_AI_Cluster_Architectures|High-Performance AI Cluster Architectures]]
- [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_07_High_Throughput_Data_Loaders_and_Storage_IO|Data Loaders and Storage I/O]]

Lab, tiered by budget:
- Tier 0 (free): run `torch.distributed` all-reduce on CPU with the gloo backend across 4 local processes and fit time versus message size to a latency-plus-bandwidth model.
- Tier 1 (paid): one multi-GPU instance for at most 2 hours; compare DDP and FSDP peak memory per rank and tokens per second on a small model.
- Gate Tier 1 with `lab_guard.sh INSTANCE_TYPE 2 1 60`, and confirm the GPU vCPU quota first.
Milestone: report peak memory per rank for DDP versus FSDP, and compare to the model-state formula below.
Teardown: `aws ec2 terminate-instances --instance-ids INSTANCE_ID` (or the tag-based command from Week 12), then the tag audit.

Mechanism not in getting-started guides: mixed-precision Adam holds about 16 bytes per parameter of model state (2 bf16 weights, 2 bf16 gradients, 12 for fp32 master weights, momentum, and variance), per the ZeRO paper.
For 70B parameters that is 1.12 TB, and ZeRO-3 across 64 ranks reduces model state to about 17.5 GB per rank before activations.
ZeRO-3 pays for this with about 1.5x the communication volume of plain data parallelism (ZeRO paper, SC 2020).
A ring all-reduce moves 2(N-1)/N times the buffer size per rank, so step time for gradient reduction is dominated by bandwidth once N is large, which is why topology-aware placement and non-ECMP transports matter.
Checkpoint cost scales with sharded state size over aggregate write bandwidth, so the right question is how many writers and how many object-store prefixes, not how big the bucket is.

Drill: D9.
Exit criterion: you can predict per-rank memory for a model and sharding config to within 10 percent before running it.

### Week 14 - LLM Inference Mechanics

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_01_LLM_Inference_Mechanics_Prefill_vs_Decode|LLM Inference Mechanics: Prefill vs Decode]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_03_KV_Cache_Management_and_PagedAttention|KV-Cache Management and PagedAttention]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_04_Continuous_Batching_and_Scheduling_Algorithms|Continuous Batching and Scheduling]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_06_Model_Quantization_and_Compression|Model Quantization and Compression]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_02_Inference_Engines_vLLM_SGLang_TensorRT_LLM|Inference Engines]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_05_Speculative_Decoding_and_Advanced_Decoding|Speculative Decoding]]

Lab: serve a small open model with vLLM on one GPU instance, and predict before measuring.
Milestone: write down predicted decode tokens per second from HBM bandwidth and weight bytes, then measure, and explain the gap; then vary maximum concurrent sequences, GPU memory utilization fraction, chunked prefill, and prefix caching, and record TTFT and inter-token latency for each (flag names vary by vLLM version, so check `vllm serve --help`).
Gate with `lab_guard.sh INSTANCE_TYPE 3 1 40`.
Teardown: `aws ec2 terminate-instances --instance-ids INSTANCE_ID`.

Mechanism not in getting-started guides: each decode step reads all weights once plus every active sequence's KV cache, so step time is about (weight bytes + sum of KV bytes) / HBM bandwidth.
Worked example on one v6e host (8 chips, 256 GB HBM, 13.1 TB/s aggregate; figures from the TPU v6e docs above) serving a 70B-parameter bf16 model with grouped-query attention (80 layers, 8 KV heads, head dimension 128; model-card values, see UNVERIFIED):
- KV per token = 2 x 80 x 8 x 128 x 2 bytes = 327,680 bytes (320 KiB).
- KV per 8,192-token sequence = 2.68 GB.
- Weights = 140 GB, leaving 90.4 GB after reserving 10 percent of HBM, which holds 33 full-length sequences.
- Weight-read floor per step = 140 GB / 13.1 TB/s = 10.7 ms.
- At batch 32 with full 8k contexts, step time = (140 + 85.9) GB / 13.1 TB/s = 17.2 ms, or about 1,856 tokens per second per host before inter-chip communication costs.
Decode arithmetic intensity is about 1 FLOP per byte per sequence in bf16, against a ridge near 560, so decode needs a batch in the hundreds to become compute-bound, which KV capacity forbids at long context.
This is why KV capacity, not FLOPs, sets decode throughput, and why PagedAttention (virtual-memory-style block tables that remove fragmentation, SOSP 2023) raises throughput by raising feasible batch size.

Drill: D6.
Exit criterion: you can size a model on a given accelerator count, state max concurrent sequences, and estimate tokens per second from first principles.

### Week 15 - Disaggregation, Autoscaling, and Managed Inference

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_07_Prefill_and_Decode_Disaggregation|Prefill and Decode Disaggregation]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_08_Cloud_GPU_Autoscaling_and_Cold_Start_Mitigation|Cloud GPU Autoscaling and Cold-Start Mitigation]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_11_End_to_End_Latency_Throughput_and_Cost_Modeling|End-to-End Latency, Throughput, and Cost Modeling]]
- [[17-Cloud-AI-Engineering/Volume_15_Capstones_Production_Systems/Chapter_03_Capstone_High_Throughput_LLM_Serving_Stack|Capstone 3]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_09_Managed_Inference_Amazon_Bedrock_vs_Vertex_AI|Managed Inference: Bedrock vs Vertex AI]]
- [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_10_Claude_on_Bedrock_and_Vertex_AI|Claude on Bedrock and Vertex AI]]
- [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_06_Capacity_and_Quotas|Capacity and Quotas (Agentic AI)]]

Lab: complete Capstone 3 and measure scale-from-zero time to first token with weights pulled from object storage versus pre-baked on local NVMe.
Milestone: break the cold start into phases (node provision, image pull, weight load, engine warmup, first token), and identify the dominant phase with timestamps.
Budget: 80 USD alert; gate with `lab_guard.sh INSTANCE_TYPE 4 1 80`.
Teardown: `tofu -chdir=labs/w15 destroy -auto-approve`, then the tag audit and `gcloud compute instances list`.

Mechanism not in getting-started guides: disaggregation only wins if KV transfer is cheap relative to prefill.
Example for the same 70B model at 8,192 tokens: prefill FLOPs = 2 x 70e9 x 8,192 = 1.15e15, which on 8 v6e chips at an assumed 50 percent utilization takes about 0.31 s.
Transferring the 2.68 GB KV cache over an assumed 50 GB/s (400 Gbps) path takes about 54 ms, or about 17 percent of prefill time, so layer-by-layer streaming that overlaps transfer with compute is required to hide it.
Utilization and link rate here are stated assumptions, not measurements; replace them with yours.
Cold-start arithmetic is the same shape: loading 140 GB of weights at a sustained 5 GB/s takes 28 s, so weight-load bandwidth, not container start, is usually the floor.

Drill: D6 again with a bursty arrival process, then D9 as a follow-up.
Exit criterion: you can produce a latency budget from request arrival to first token for a cold replica.

### Week 16 - Applications, Integration, and Final Defense

Deep read (Tier A):
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_02_Enterprise_Retrieval_Augmented_Generation_Architecture|Enterprise RAG Architecture]]
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_08_Cloud_Security_for_AI_Systems_Attacks_and_Defenses|Cloud Security for AI Systems]]
- [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_01_Design_Multi_Tenant_LLM_Inference_Platform|Design: Multi-Tenant LLM Inference Platform]]
- [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_08_Staff_Plus_Evaluation_Bar_L5_L6_L7|Staff+ Evaluation Bar]]

Skim (Tier B):
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_01_Vector_Databases_and_Similarity_Search_on_Cloud|Vector Databases]]
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_03_Agent_Runtimes_and_Cloud_Execution_Environments|Agent Runtimes]]
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_04_MLOps_Pipelines_SageMaker_vs_Vertex_AI|MLOps Pipelines]]
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_05_Feature_Stores_and_Real_Time_Data_Serving|Feature Stores]]
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_06_Model_Evaluation_Monitoring_and_Drift_Detection|Evaluation, Monitoring, and Drift]]
- [[17-Cloud-AI-Engineering/Volume_12_AI_Applications_MLOps_and_Data_Platforms/Chapter_07_AI_Safety_Guardrails_and_Governance|AI Safety, Guardrails, and Governance]]
- [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_07_Production_Evaluation|Production Evaluation (Agentic AI)]]
- [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_06_Cloud_AI_Numbers_Every_Staff_Engineer_Must_Know|Cloud AI Numbers Every Staff Engineer Must Know]]
- [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_07_Cloud_and_AI_Certification_Strategic_Navigation|Certification Navigation Guide]]

Work package:
- Write a 4-page design document for one Volume 14 system design prompt, including capacity math, failure modes, and a cost model.
- Run two timed 45-minute mock loops, one on [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_02_Design_Distributed_GPU_Cluster_Scheduler|the GPU scheduler design]] and one on [[17-Cloud-AI-Engineering/Volume_14_Staff_Plus_Interview_Arena/Chapter_05_Design_Multi_Region_Active_Active_API|the multi-region active-active API design]].
- Re-answer all nine drills below, cold, and score them against the rubric.
- Run the full teardown audit on both clouds and confirm zero tagged resources and no running projects.

Exit criterion: seven of nine drills score "pass" on all rubric dimensions on the first attempt, and the remaining two score "pass" after one hint.

## 5. L6 Bar-Raiser Rubric

| Dimension | Below bar | Meets L6 bar |
|---|---|---|
| Scoping | Starts drawing boxes | Asks about SLO, scale, consistency, tenancy, and budget before designing |
| Quantification | "It scales" | Derives capacity, latency, and cost from stated inputs, with units, in the room |
| Mechanism | Names a service | Explains the data path, the consensus or quorum rule, and the failure behavior |
| Failure thinking | Lists generic failures | Names the amplifier, the blast radius, the detection signal, and the mitigation |
| Trade-offs | Presents one option | States what is given up, what it costs to operate, and what would change the decision |

Calibration notes: L5 answers a question correctly; L6 volunteers the failure mode and the number; L7 reframes the question when its premise is wrong and addresses organizational and economic consequences.

## 6. Defense Drills With Model Answers

The numbers in the answers are inputs to your reasoning, so state your assumptions out loud in the interview.
Reproduce the arithmetic with the calculator in section 7.

### D1 (L5): "Writes to our DynamoDB table are throttled, but the table is at 10 percent of provisioned capacity. Why, and what do you do?"

Model answer:
- Throughput is enforced per partition, so provisioned capacity is the sum of per-partition ceilings, and traffic skewed onto one key saturates one partition while the table-level metric looks idle (per-partition ceilings commonly cited as 1,000 WCU and 3,000 RCU; confirm in current docs).
- I would first confirm with Contributor Insights and the per-key throttle signal rather than guess, because a hot key and a hot partition from a bad key schema look different.
- Adaptive capacity and partition splitting help for sustained skew across keys, but a single hot item cannot be split, so the fix must be in the data model.
- For a hot key I would shard the key with a suffix of 0..N-1 and write to a random or hashed suffix, accepting that reads become an N-way scatter-gather.
- For read-hot items I would put a cache (DAX or an application cache) in front, because replicas of a single item do not help writes.
- The operational cost of write sharding is that aggregation queries and conditional writes across shards lose atomicity, so uniqueness constraints need a different mechanism.
- Detection: alarm on throttled-request count per table and on the ratio of consumed capacity to the busiest key, not on average utilization.

Follow-up: "Traffic is a viral item that is both read-hot and write-hot, and the item must stay strongly consistent. Now what?"
Expected direction: decompose the item into a low-write core record and a high-write counter or event stream, aggregate asynchronously, and explain what consistency the user-visible counter actually needs.

### D2 (L6): "Design the capacity and failure model for an inference gateway that must survive loss of an AZ without human action."

Model answer:
- The requirement says without human action and presumably without a control-plane call, so I design for static stability: no scaling event or API call may be on the recovery path.
- With three AZs the fleet must be provisioned at k/(k-1) = 1.5x of peak demand, so each AZ runs at 66.7 percent of its capacity at peak, and I state that 50 percent of the extra spend buys the SLO.
- With two AZs the factor is 2x, which is why I would push for three AZs on cost grounds even though it is more moving parts.
- Load balancer health checks must drain the failed AZ within the detection window, so I set the health check interval and threshold from the SLO, and I note that aggressive checks create their own flapping risk.
- The gateway must not call a regional control plane at request time (no on-the-fly config fetch, no per-request token mint against a single-AZ dependency), so config is pushed ahead and cached with bounded staleness.
- Accelerator capacity is the hard part: GPU or TPU capacity in the surviving AZs may not exist at failover time, so headroom must already be reserved or running, and an on-demand scale-out cannot be assumed to succeed.
- Cell structure: I would split tenants into cells and use shuffle sharding across cells, so a poison request takes out a small fraction of tenants, and I quote the fraction.
- Failure modes I would test with a game day: gray failure (an AZ that is slow, not down), partial network partition between gateway and model servers, and a retry storm from clients when 33 percent of capacity disappears.
- To prevent the retry storm I would enforce a client retry budget and server-side admission control with priority shedding, so that the 1.5x headroom is not consumed by retries.

Follow-up: "Finance says 1.5x headroom is too expensive. What are your options and what do you give up?"
Expected direction: tier tenants so only paid-SLO traffic gets zonal headroom, shed free-tier traffic during failover, use reserved capacity only for the headroom share, and be explicit that this converts an availability guarantee into a priority policy.

### D3 (L6): "Choose a database for a global payments ledger that needs strong consistency across two continents."

Model answer:
- I start by separating the requirements: serializable multi-row transactions, a recovery point of zero for committed writes, and what read staleness is tolerable.
- Light speed in fiber is about 5 microseconds per km, so a 6,000 km path is at least 30 ms one-way and 60 ms round trip, and any synchronous cross-continent commit pays at least one such round trip to reach a majority; this floor does not depend on vendor.
- Spanner provides external consistency by Paxos-replicated splits, two-phase commit across splits, and commit wait against TrueTime uncertainty, so the write latency is the quorum round trip plus commit wait, and reads at a stale timestamp can be served from the nearest replica.
- Aurora is a single-writer design with 4-of-6 quorum across AZs in one region; cross-region replication is asynchronous, so a regional failover has a nonzero recovery point, which fails the stated zero-RPO requirement for a cross-continent active design.
- Aurora DSQL is positioned for multi-region active-active with strong consistency using optimistic concurrency (detail UNVERIFIED), so contention shows up as commit-time aborts that the client must retry, and I would check hot-row behavior before choosing it for a ledger with hot accounts.
- DynamoDB global tables are multi-writer with last-writer-wins reconciliation across regions, which can lose a concurrent update, so it fits idempotent or CRDT-like data and does not fit double-entry bookkeeping without an additional protocol.
- My recommendation is a Spanner-class system if the ledger must be active in both continents, with a single home region per account to keep most commits in one region and cross-region commits rare.
- I would call out operating costs: commit latency in the tens of milliseconds for cross-region accounts, hot-account contention, and lock-in to a vendor-specific consistency mechanism.

Follow-up: "The business accepts a 5-second RPO instead of zero. Does your answer change?"
Expected direction: yes, a single-region primary with asynchronous cross-region replication becomes viable, and the trade is simpler and faster writes against a defined data-loss window and a more complex failover and reconciliation runbook.

### D4 (L6): "A dependency brownout turned into a full outage. Walk me through how retries made it worse and how you prevent it."

Model answer:
- A brownout means the dependency is slow, so requests hold connections and threads longer, queues grow, and clients time out and retry, which raises offered load precisely while capacity is lowest.
- Retries multiply across layers: three layers with three attempts each produce 27 attempts at the leaf, so I would count attempts per layer in the design review.
- The prevention stack is layered: a per-client retry budget (for example retries limited to a fraction of recent first attempts), exponential backoff with full jitter to decorrelate clients, and deadline propagation so work already past its caller's deadline is dropped.
- On the server, admission control limits in-flight requests to what the service can complete within the deadline, and sheds by priority, because queueing beyond capacity only converts latency into timeouts.
- Circuit breakers help, but they add their own failure mode: a breaker that opens for all callers at once synchronizes their return, so half-open probing needs jitter and a small probe fraction.
- Detection: alert on burn rate with a long and short window pair, and add an alert on retry ratio (retries over first attempts), because it leads the error rate.
- Postmortem hygiene: record the trigger, the amplifier (retry policy), and the missing safeguard (no budget, no shedding), and track the corrective actions to closure.

Follow-up: "Your client library is owned by 40 teams. How do you roll out a retry budget?"
Expected direction: ship it as a default in the shared library with a kill switch, canary by team, measure retry ratio before and after, and treat unmigrated clients as the risk register.

### D5 (L6): "We train on Spot GPUs across AWS and GCP. Design the interruption handling and justify the checkpoint interval."

Model answer:
- Notice windows differ: AWS gives a two-minute interruption notice and GCP gives a best-effort shutdown of up to 30 seconds, so a design that depends on a full checkpoint inside the notice fails on both clouds for a multi-hundred-GB state.
- I use periodic asynchronous checkpoints at the Young/Daly interval T = sqrt(2 x C x M), with C measured as the stall visible to training (not the background upload time), and M estimated from observed fleet interruptions.
- With C = 120 s and M = 8 hours the interval is about 44 minutes and the expected waste is about 9 percent, and I would show how waste moves if C drops to 20 s through local-NVMe staging followed by background upload.
- The notice is spent on flushing the incremental delta and draining the collective, and on marking the node as draining so the job manager reshapes the job instead of hanging in a collective.
- Elasticity matters more than checkpointing speed at scale: if the job needs exactly N ranks, any single interruption stalls the whole job, so I would prefer a design that resizes data-parallel replicas or keeps a small pool of hot spares.
- Correctness risk: a checkpoint written while ranks are inconsistent is corrupt, so I would write to a temporary prefix and commit with an atomic manifest, and I would keep the last K good checkpoints.
- Economics: Spot only wins if (discount x useful-work fraction) beats on-demand, so I compute effective cost per useful token including the waste and the restart time, not the headline discount.

Follow-up: "Spot capacity for the instance type dries up for a day. What changes?"
Expected direction: diversify across instance types and zones with a capacity-optimized allocation, fall back to a capacity reservation or on-demand for a protected slice, and decide up front which jobs can wait.

### D6 (L6/L7): "Size a serving fleet for a 70B model at 10,000 concurrent chat sessions, 8k context, and justify every number."

Model answer:
- Inputs I would confirm first: TTFT and inter-token SLO, output length distribution, prompt-sharing rate (prefix cache hit potential), and whether quality tolerates 8-bit or 4-bit weights.
- Memory: bf16 weights are 140 GB; KV for a GQA model with 80 layers, 8 KV heads, and head dimension 128 is 320 KiB per token, or 2.68 GB per full 8k session, so 10,000 full sessions need 26.8 TB of KV alone.
- On 256 GB hosts (8 chips of 32 GB), about 33 full sessions fit after weights and a 10 percent reserve, so I need about 10,000 / 33 = 303 hosts for KV capacity at worst-case full contexts.
- That number is the bound set by memory, so the levers are the right ones to discuss: 8-bit KV cache halves it, prefix sharing reduces it by the shared fraction, average context is usually far below the maximum, and paging means I allocate for actual length, not the maximum.
- Throughput check: at batch 32 and 8k contexts, step time is about 17 ms for about 1,856 tokens per second per host (before communication), so I verify the per-session token rate implied by the SLO against 1,856 / 32 = 58 tokens per second per session, which is more than a human reads, so capacity is KV-bound, not compute-bound.
- Prefill is the other axis: each 8k prefill is about 1.15e15 FLOPs, so arrival rate of new sessions times prefill time determines how much prefill capacity I need, and I would consider disaggregation if prefill spikes hurt inter-token latency.
- Cost: cost per million tokens is (host-hours price x hosts) / (tokens per second x 3,600 x 1e-6 per hour), computed from the current price page and measured, not assumed throughput.
- I would not trust any of these without a load test using the real length distribution, and I would set autoscaling on KV utilization and queue depth, not CPU.

Follow-up: "Average context is 2k and 60 percent of prompts share a system prompt. Recompute."
Expected direction: KV per session drops about 4x before sharing, the shared prefix is stored once per host, host count falls by a large factor, and the bottleneck may shift to compute or to inter-chip communication.

### D7 (L7): "Design the data perimeter so that no principal outside our organization can read our data, even with leaked credentials, on both clouds."

Model answer:
- I think in three independent layers, because each fails differently: identity (who), resource (what can be read), and network or API boundary (from where).
- AWS identity layer: SCPs and resource control policies as organization-wide ceilings, and an organization-ID condition on resource policies so that a valid credential from another organization is denied even if a bucket policy is mis-edited.
- AWS network layer: VPC endpoints with endpoint policies that restrict principals to the organization and resources to ours, so a stolen credential used inside our VPC cannot copy to an outside bucket.
- GCP: organization policy constraints plus VPC Service Controls perimeters with explicit ingress and egress rules, tested in dry-run first, which blocks API-level exfiltration even for valid credentials.
- Known gaps I would state: VPC SC does not constrain non-Google-API egress from a VM, and an endpoint policy does not cover traffic that leaves via a NAT gateway to the public service endpoint, so I add egress controls and detection.
- Federation: no static keys, and trust policies conditioned on issuer, audience, and subject, because a missing subject condition lets any workflow on that platform assume the role.
- Keys: customer-managed keys with encryption context and a key policy that denies decrypt outside the organization, so data stolen as ciphertext is not readable and key use is auditable.
- Operating cost: a perimeter breaks legitimate integrations, so I would ship it as audit-only first, measure violations for two weeks, create exceptions with owners and expiry, then enforce.
- Break-glass: a perimeter without a tested emergency path becomes an outage, so I would design and rehearse the exception path, with its own alerting.

Follow-up: "A partner needs to read one dataset. How do you grant it without punching a hole in the perimeter?"
Expected direction: a narrowly scoped ingress rule or an access point with a partner-specific policy, a time bound, a dedicated audit trail, and ideally pushing a copy to the partner's side rather than granting them access to the source.

### D8 (L7): "Leadership wants an active-active multi-cloud design for resilience. Do you agree?"

Model answer:
- I would reframe the goal: the question is what failure we are insuring against, how often it occurs, and what the insurance costs, not whether two clouds are better than one.
- Region-level failures in a single cloud are addressable with multi-region within that cloud, which is far cheaper; cross-cloud only insures against a provider-wide or control-plane-wide event, and most large outages were regional or shared-dependency events.
- The mechanism-level costs: any strongly consistent write across clouds pays at least one inter-cloud round trip (milliseconds to tens of milliseconds, bounded below by distance), so consensus groups should not span clouds on the hot path.
- Egress economics: replicating state across clouds pays per-GB egress on every byte, which turns a chatty design into a standing cost, and I would compute it from the current egress price list.
- Lowest-common-denominator effect: using only features both clouds provide removes most managed-service advantages, and the team then operates two sets of IAM, networking, and observability, doubling the operational surface and the on-call burden.
- Correlated failure: a shared dependency (DNS provider, identity provider, CI system, a global config pipeline) can take both clouds down together, so multi-cloud does not buy independence unless those are also separated.
- What I would propose instead: multi-region active-active in the primary cloud, a tested cold or warm standby on the second cloud for the narrow set of critical functions, portable data formats and infrastructure code, and a quarterly failover exercise with measured RTO and RPO.
- I would present the decision as a table of failure scenarios, annual likelihood, impact, and cost of each mitigation, and let leadership pick with the numbers visible.

Follow-up: "The board insists. What is the minimum honest version?"
Expected direction: an independently operated stateless tier plus replicated data in open formats, accepting a stated RPO and RTO, with a test cadence that proves it works.

### D9 (L7): "Training throughput per GPU dropped 30 percent when we scaled from 64 to 512 GPUs. Find it."

Model answer:
- I start by decomposing step time into compute, exposed communication, input stall, and checkpoint or synchronization stall, using a profiler timeline for one slow step, because scaling loss is almost always one of these dominating.
- Expected scaling loss is quantifiable: a ring all-reduce moves about 2x the buffer per rank, and at larger N more of that traffic crosses slower tiers (inter-node versus intra-node), so effective bandwidth per rank falls, and I compare measured collective time to a bandwidth model.
- Network suspects: hash collisions among a small number of large flows on an ECMP fabric, oversubscription between racks, a mis-set transport or NCCL configuration, and placement that splits a tensor-parallel group across racks.
- Stragglers: one slow GPU or a thermally throttled node delays every collective, so I look at per-rank step-time variance and per-rank collective-wait time, and I would run a pairwise bandwidth and GEMM health test across nodes to find the outlier.
- Input pipeline: aggregate data-loader demand grows with N, so a shared object-store prefix or a metadata server can saturate, which shows up as GPU idle gaps before the forward pass.
- Parallelism choice: a configuration tuned for 64 ranks may be wrong at 512, for example data parallel replicas now dominate communication, and I would test increasing tensor or pipeline degree within a node or rack boundary.
- Silent data corruption and straggler policy: at this scale a node-level fault rate matters, so I would verify the job has automatic detection and evacuation rather than hanging.
- Method: change one variable at a time, hold the global batch size fixed or account for its effect on convergence, and report MFU rather than tokens per second so the result is comparable across scales.

Follow-up: "Profiles show 18 percent of step time is exposed all-reduce. What are your options?"
Expected direction: overlap gradient communication with backward compute by bucketing, reduce volume with sharding or lower-precision communication, improve topology-aware placement, and quantify what each option gains against its convergence or memory cost.

## 7. Calculator for the Numbers Used Above

```python
import math

# KV cache and decode step model (inputs: model card and accelerator docs)
layers, kv_heads, head_dim, bytes_per = 80, 8, 128, 2
kv_per_token = 2 * layers * kv_heads * head_dim * bytes_per
ctx = 8192
kv_per_seq = kv_per_token * ctx
hbm_per_chip, chips, bw_per_chip = 32e9, 8, 1.638e12
weights = 70e9 * 2
usable = hbm_per_chip * chips * 0.9 - weights
batch = 32
step_s = (weights + batch * kv_per_seq) / (bw_per_chip * chips)
print("kv_per_token_B", kv_per_token, "kv_per_seq_GB", round(kv_per_seq / 1e9, 2))
print("max_full_seqs", int(usable // kv_per_seq))
print("step_ms", round(step_s * 1e3, 1), "tok_per_s", round(batch / step_s))
print("ridge_flops_per_byte", round(918e12 / bw_per_chip))

# Prefill and KV transfer (utilization and link rate are assumptions)
prefill_s = 2 * 70e9 * ctx / (918e12 * chips * 0.5)
xfer_s = kv_per_seq / 50e9
print("prefill_s", round(prefill_s, 2), "xfer_ms", round(xfer_s * 1e3), "ratio", round(xfer_s / prefill_s, 2))

# Young/Daly checkpoint interval and waste
C, M = 120.0, 8 * 3600.0
T = math.sqrt(2 * C * M)
print("interval_min", round(T / 60, 1), "waste", round(C / T + T / (2 * M), 3))

# Shuffle sharding and burn rates
print("shards", math.comb(8, 2), math.comb(100, 5))
print("overlap_at_least_one", round(1 - math.comb(6, 2) / math.comb(8, 2), 3))
print("budget_burn", 14.4 / 720, 6 * 6 / 720, 1 * 72 / 720)

# ZeRO model state: 16 bytes per parameter
print("model_state_TB", 16 * 70e9 / 1e12, "per_rank_GB_at_64", 16 * 70e9 / 64 / 1e9)
```

Expected output matches the figures quoted in weeks 3, 10, 12, 13, 14, and 15.

## 8. Failure Modes of the Study Plan Itself

- Tutorial drift: reading without the lab milestone produces recognition, not recall; the exit criterion for each week is a thing you do from memory.
- Quota delay: GPU quota requests can take days, so file them in week 1, and keep an on-demand fallback (managed inference or a smaller instance) so weeks 13 to 15 do not stall.
- Cost leakage: the largest realistic overrun is a forgotten GPU instance or managed Kubernetes control plane, so run the tag audit at the end of every lab session, not at the end of the week.
- Number staleness: any price, quota, or accelerator figure you memorize decays, so re-verify against the pages cited in [[17-Cloud-AI-Engineering/Appendices/Appendix_C_Limits_Quotas_and_Thresholds_Matrix|Appendix C]] before an interview loop.
- Over-indexing on one cloud: alternate the lab cloud weekly where the plan allows, because the translation skill is the scarce one.

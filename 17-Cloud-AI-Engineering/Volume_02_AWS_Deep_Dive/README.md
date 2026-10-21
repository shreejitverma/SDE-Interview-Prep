---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 02 - AWS Deep Dive

Volume 02 - AWS Deep Dive in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [IAM Policy Evaluation Engine](Chapter_01_IAM_Policy_Evaluation_Engine.md) - Evaluation logic across SCPs, RCPs, identity policies, resource policies, permission boundaries, and session policies.
2. [AWS Organizations and Account Governance](Chapter_02_AWS_Organizations_and_Account_Governance.md) - Multi-account OU hierarchy, Service Control Policies, Resource Access Manager, and AWS Config conformance packs.
3. [VPC Networking Internals](Chapter_03_VPC_Networking_Internals.md) - VPC route tables, Elastic Network Interfaces, Transit Gateway route domains, PrivateLink architecture, and VPC Lattice.
4. [EC2, Nitro Architecture, and Graviton](Chapter_04_EC2_Nitro_Architecture_and_Graviton.md) - Nitro Card for VPC, Nitro Security Chip, hypervisor offloading, and ARM Neoverse Graviton compiler optimizations.
5. [Serverless Compute and Lambda Internals](Chapter_05_Serverless_Compute_and_Lambda_Internals.md) - Firecracker microVM provisioning, snapshot restore via SnapStart, cold start mechanics, and runtime worker placement.
6. [Container Services: ECS and Fargate](Chapter_06_Container_Services_ECS_and_Fargate.md) - ECS agent state reconciliation, task placement strategies, Fargate microVM sandboxing, and awsvpc networking.
7. [Amazon EKS: Control Plane and Networking](Chapter_07_Amazon_EKS_Control_Plane_and_Networking.md) - Managed control plane autoscaling, AWS VPC CNI secondary IP exhaustion, IRSA, and EKS Pod Identity mechanics.
8. [S3 Deep Dive: Consistency and ShardStore](Chapter_08_S3_Deep_Dive_Consistency_and_ShardStore.md) - Strong read-after-write consistency validation, ShardStore log-structured storage, auto-partitioning prefixes, and S3 Express One Zone.
9. [Elastic Block Store and Network Storage](Chapter_09_Elastic_Block_Store_and_Network_Storage.md) - Physalia consensus engine, gp3 and io2 Block Express network fabric, distributed snapshots, and FSx for Lustre file tiering.
10. [Amazon DynamoDB: Architecture and Partitioning](Chapter_10_Amazon_DynamoDB_Architecture_and_Partitioning.md) - Request routers, partition split mechanics, Paxos replica groups, adaptive capacity evolution, and global tables.
11. [Amazon Aurora: Log-Is-The-Database](Chapter_11_Amazon_Aurora_Log_Is_The_Database.md) - Decoupled compute and storage, 4-of-6 quorum slice writes, distributed asynchronous redo log processing, and read replica scaling.
12. [Aurora DSQL: Multi-Region Active-Active SQL](Chapter_12_Aurora_DSQL_Multi_Region_Active_Active_SQL.md) - Distributed transaction commit protocols, active-active multi-region ACID consistency, and synchronous distributed consensus.
13. [Messaging and Streaming: SQS, SNS, EventBridge, and MSK](Chapter_13_Messaging_and_Streaming_SQS_SNS_EventBridge_and_MSK.md) - Distributed queues, visibility timeout race conditions, content-based routing in EventBridge, and MSK tiered storage.
14. [Observability: CloudWatch, Metrics, Logs, and X-Ray](Chapter_14_Observability_CloudWatch_Metrics_Logs_and_X_Ray.md) - Embedded Metric Format, metric alarm hysteresis, log query execution, and X-Ray trace context propagation.
15. [Security and Cryptography: KMS and Nitro Enclaves](Chapter_15_Security_and_Cryptography_KMS_and_Nitro_Enclaves.md) - Envelope encryption, CloudHSM key hierarchy, Nitro Enclaves cryptographic attestation, and GuardDuty threat detection.

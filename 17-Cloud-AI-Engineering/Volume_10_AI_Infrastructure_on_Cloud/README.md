---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 10 - AI Infrastructure on Cloud: Accelerators, Fabrics, Clusters

Volume 10 - AI Infrastructure on Cloud: Accelerators, Fabrics, Clusters in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [Accelerator Architectures: GPUs, Trainium, TPUs](Chapter_01_Accelerator_Architectures_GPUs_Trainium_TPUs.md) - NVIDIA Hopper and Blackwell SM/Tensor Core architecture, AWS Trainium NeuronCore, Google TPU v4/v5p/v6e Trillium, and HBM bandwidth.
2. [Interconnect Technologies: NVLink, EFA, and TPU ICI](Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI.md) - Scale-up NVLink NVSwitch fabrics, AWS Elastic Fabric Adapter with SRD packet spraying, and Google TPU 3D Torus ICI with OCS.
3. [High-Performance AI Cluster Architectures](Chapter_03_High_Performance_AI_Cluster_Architectures.md) - EC2 UltraClusters, SageMaker HyperPod automated cluster repair, GKE AI clusters, and TPU slices and multislice topologies.
4. [Distributed Training Parallelism Strategies](Chapter_04_Distributed_Training_Parallelism_Strategies.md) - 3D parallelism: Data Parallelism (DP), Tensor Parallelism (TP), Pipeline Parallelism (PP), Sequence Parallelism (SP), and MoE Expert Parallelism (EP).
5. [Memory Optimization: ZeRO, FSDP, and JAX Sharding](Chapter_05_Memory_Optimization_ZeRO_FSDP_and_JAX_Sharding.md) - ZeRO stages 1, 2, and 3, PyTorch Fully Sharded Data Parallel (FSDP), JAX shard_map and pjit, and activation checkpointing trade-offs.
6. [High-Throughput Checkpointing to Object Storage](Chapter_06_High_Throughput_Checkpointing_to_Object_Storage.md) - Multi-terabyte checkpoint persistence, asynchronous distributed checkpointing, S3 Express One Zone vs GCS, and local NVMe write buffering.
7. [High-Throughput Data Loaders and Storage I/O](Chapter_07_High_Throughput_Data_Loaders_and_Storage_IO.md) - Preventing GPU starvation, TorchData, AWS S3 Connector for PyTorch, Cloud Storage FUSE, and WebDataset sharded streaming.
8. [Fault Tolerance and Fleet Resiliency at Scale](Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale.md) - Silent data corruption detection, network straggler mitigation, automated node evacuation, and capacity reservation strategies.

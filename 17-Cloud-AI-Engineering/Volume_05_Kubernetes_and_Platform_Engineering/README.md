---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 05 - Kubernetes and Platform Engineering

Volume 05 - Kubernetes and Platform Engineering in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [Kubernetes Control Loop Internals](Chapter_01_Kubernetes_Control_Loop_Internals.md) - Controller reconciliation loops, Informers, Indexers, WorkQueues, and optimistic concurrency via resourceVersion.
2. [Advanced Scheduling and Topology-Aware Placement](Chapter_02_Advanced_Scheduling_and_Topology_Aware_Placement.md) - Kube-scheduler framework extension points, pod affinity/anti-affinity, taints and tolerations, and topology spread constraints.
3. [Container Networking Interface and eBPF with Cilium](Chapter_03_CNI_and_eBPF_with_Cilium.md) - CNI network plumbing, IPAM drivers, eBPF packet manipulation at the socket layer, and Cilium network policy enforcement.
4. [Autoscaling at Scale: HPA, KEDA, and Karpenter](Chapter_04_Autoscaling_at_Scale_HPA_KEDA_and_Karpenter.md) - Metric pipelines, event-driven scaling with KEDA, and fast node provisioning with Karpenter versus GKE NAP.
5. [Multi-Tenancy, Isolation, and Resource Quotas](Chapter_05_Multi_Tenancy_Isolation_and_Resource_Quotas.md) - Soft vs hard multi-tenancy, Namespace boundaries, ResourceQuotas, LimitRanges, and Hierarchical Namespaces.
6. [Internal Developer Platforms and Backstage](Chapter_06_Internal_Developer_Platforms_and_Backstage.md) - Service catalogs, declarative software templates, developer self-service abstractions, and platform engineering APIs.
7. [GitOps Delivery: Argo CD and Flux](Chapter_07_GitOps_Delivery_Argo_CD_and_Flux.md) - Git-driven reconciliation, automated drift detection, atomic rollbacks, ApplicationSets, and multi-cluster delivery.
8. [Service Mesh Architecture and Trade-Offs](Chapter_08_Service_Mesh_Architecture_and_Trade_Offs.md) - Sidecar Envoy architecture vs ambient/proxyless mesh, mTLS cryptographic overhead, and when to avoid a service mesh.

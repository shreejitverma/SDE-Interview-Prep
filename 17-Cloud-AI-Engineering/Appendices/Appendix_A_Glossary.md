---
type: concept
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Appendix A: Glossary of Cloud and AI Infrastructure Terms

This appendix defines the virtualization, networking, storage, reliability, and accelerator vocabulary used across the curriculum.
Definitions are mechanism-first: each one says what physically happens, not only what the term is called.
Section 8 works six mechanisms through to numbers, and Section 11 has eight interview questions with L6 and L7 model answers.

Verification posture for this edition, dated 2026-10-21:
- Vendor numbers in this appendix are limited to the Google Cloud TPU figures I checked against official documentation (Section 9).
- Architectural figures that come from peer-reviewed papers are tagged with the paper and year.
- I could not re-verify GPU and Trainium specifications, instance bandwidths, storage IOPS limits, or list prices during this edition. I omitted them rather than guess. Volume 14 carries the full numbers sheet.
- Anything cited from memory of a paper, or that I could not re-check, is listed in the UNVERIFIED block at the end.

Primary chapter pointers by domain:
- Virtualization: [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_01_Hypervisors_MicroVMs_and_Sandboxing|Hypervisors, MicroVMs, and Sandboxing]], [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_04_Firecracker_MicroVM|Firecracker]].
- Networking: [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_02_Datacenter_Networking_Fabrics|Datacenter Networking Fabrics]], [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI|NVLink, EFA, and TPU ICI]].
- Storage: [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_05_Storage_Media_and_Block_Storage_Foundations|Storage Media and Block Storage Foundations]], [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_09_Elastic_Block_Store_and_Network_Storage|EBS and Network Storage]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_09_Cloud_Storage_and_Colossus_Filesystem|Cloud Storage and Colossus]].
- Reliability: [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability|Regions, Zones, and Static Stability]], [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_03_Cell_Based_Architecture_and_Shuffle_Sharding|Cell-Based Architecture and Shuffle Sharding]].
- Accelerators and training: [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_01_Accelerator_Architectures_GPUs_Trainium_TPUs|Accelerator Architectures]], [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_04_Distributed_Training_Parallelism_Strategies|Distributed Training Parallelism]].
- Serving: [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_01_LLM_Inference_Mechanics_Prefill_vs_Decode|Prefill vs Decode]], [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_03_KV_Cache_Management_and_PagedAttention|KV-Cache Management and PagedAttention]].

---

## 1. Virtualization and isolation

- **Hypervisor (VMM)**: Software that multiplexes physical CPU, memory, and I/O among guests using hardware virtualization extensions.
  On x86 the CPU runs the guest in VMX non-root mode and traps to the hypervisor on privileged events.
  KVM turns the Linux kernel into the hypervisor, and a user-space process (QEMU, Firecracker, Cloud Hypervisor) supplies the device models.
- **VM exit**: The transition from guest to hypervisor on a trapped event such as an MMIO access, an I/O port access, an external interrupt, or an EPT violation.
  Each exit costs on the order of a microsecond once you add the state save and restore plus cache and TLB disturbance.
  Device-model design is mostly about avoiding exits on the hot path.
- **EPT / NPT (nested paging)**: Hardware second-level address translation, Intel EPT and AMD NPT, that maps guest-physical to host-physical addresses.
  A TLB miss triggers a two-dimensional page walk, covered in Section 8.1.
  This is why host huge pages matter for large-memory guests and for accelerator hosts.
- **virtio**: A paravirtual device standard in which guest and host share descriptor rings (virtqueues) in memory.
  The guest writes descriptors and "kicks" the host with a notification write, which is an MMIO write that causes a VM exit.
  The host posts completions to the used ring and injects an interrupt.
  vhost moves the host side into the kernel or a separate process so the VMM thread leaves the data path.
- **SR-IOV**: A PCIe feature where one physical function exposes many virtual functions, each assignable to a guest.
  The IOMMU confines each function's DMA to that guest's memory.
  The guest drives the NIC or NVMe controller directly, so no host software touches the data path.
- **IOMMU (VT-d / AMD-Vi)**: A DMA address-translation and isolation unit.
  Without it, a passthrough device could DMA into any host memory, so every safe passthrough design depends on it.
- **VFIO**: The Linux framework for exposing IOMMU-protected devices to user space or guests.
  GPU passthrough to a VM uses VFIO.
- **Nitro System (AWS)**: Dedicated cards that offload VPC networking, EBS and local-storage I/O, and system control, plus a security chip and a minimal hypervisor.
  The guest sees ENA and NVMe devices as PCIe functions, so the host CPU is not in the storage or network data path.
  The trade-off is that the offload cards are a fixed-function fleet component, and a card firmware bug has a per-host blast radius.
  See [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_04_EC2_Nitro_Architecture_and_Graviton|EC2, Nitro Architecture, and Graviton]].
- **Titanium (Google Cloud)**: Google's tiered offload architecture, with offload processors and adapters that take over host networking, storage, and security functions on current machine families.
  It plays the same role as Nitro with a different packaging.
  See [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_06_Compute_Engine_and_Titanium_Architecture|Compute Engine and Titanium Architecture]].
- **MicroVM**: A VM with a deliberately tiny device model and no BIOS or legacy hardware.
  It trades guest compatibility for boot time and attack surface.
- **Firecracker**: A Rust VMM on KVM with a minimal device set (virtio-net, virtio-block, vsock, serial, and a few others).
  The paper reports under 125 ms boot, under 5 MiB memory overhead per microVM, and up to 150 microVMs created per second per host (Firecracker, NSDI 2020).
  It underpins Lambda and Fargate.
  The jailer sets up namespaces, cgroups, and seccomp before the VMM starts handling guest input.
  See [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_04_Firecracker_MicroVM|Firecracker]].
- **gVisor**: A user-space application kernel (the Sentry, written in Go) that implements the Linux syscall surface so a sandboxed app never talks to the host kernel directly.
  File I/O goes through a separate Gofer process.
  Its interception platforms include ptrace, systrap, and KVM, and each has a different syscall-overhead profile.
  The trade-off is a compatibility gap and syscall-heavy workloads running slower.
- **Namespaces, cgroups, seccomp-bpf**: The three Linux primitives behind containers.
  Namespaces limit what a process can see, cgroups limit what it can consume, and seccomp limits which syscalls it can make.
  All three share one host kernel, so the kernel is the isolation boundary.
  This is the core reason multi-tenant serverless uses microVMs instead.
- **Snapshot restore and uniqueness**: Resuming a microVM from a memory snapshot skips boot and runtime init.
  Every clone resumes with identical RNG state, UUIDs, and cached connections.
  Hypervisors signal a restore with mechanisms such as VMGenID so the guest can reseed.
  Code that cached a secret or nonce before the snapshot can produce duplicate "random" values after it.
  See [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_05_Serverless_Compute_and_Lambda_Internals|Serverless Compute and Lambda Internals]].
- **Memory ballooning and overcommit**: A balloon driver in the guest returns pages to the host on request.
  Overcommit bets that tenants do not touch all their memory at once, and it fails as a latency cliff when they do.
- **NUMA**: Memory access time depends on which socket a core and a DIMM belong to.
  GPU hosts also have PCIe locality: a NIC and a GPU under the same PCIe switch or root complex can do peer-to-peer DMA, and ones across sockets cannot do so efficiently.
- **Steal time**: The share of time a runnable vCPU was descheduled by the host.
  It is the guest's only direct signal of CPU contention from a noisy neighbor.
- **Confidential computing**: Hardware memory encryption and integrity (AMD SEV-SNP, Intel TDX) that hides guest memory from the host.
  Attestation proves to a remote party which measured software is running.
  See [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_15_Security_and_Confidential_Computing_Cloud_KMS|Cloud KMS and Confidential Computing]] and [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_15_Security_and_Cryptography_KMS_and_Nitro_Enclaves|KMS and Nitro Enclaves]].
- **Borg cell**: One Borg cluster-management domain, typically one datacenter's worth of machines under a logical scheduler (Borg, EuroSys 2015).
  Kubernetes is the open-source descendant of its job and task model.
  See [[04-System-Design/07-Infrastructure-and-Orchestration/Kubernetes-Architecture|Kubernetes Architecture]].

---

## 2. Networking and fabrics

- **Clos / fat-tree**: A multi-stage non-blocking fabric built from identical small switches (leaf, spine, super-spine).
  Capacity grows by adding stages, and path diversity between two hosts equals the number of equal-cost spines.
  See [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_02_Datacenter_Networking_Fabrics|Datacenter Networking Fabrics]].
- **Oversubscription ratio**: Aggregate downlink bandwidth divided by aggregate uplink bandwidth at a tier.
  General-purpose fabrics tolerate a ratio above 1:1 because traffic is bursty and mostly local.
  Training fabrics for collectives are built close to 1:1 because every rank sends at line rate simultaneously.
- **Bisection bandwidth**: The minimum bandwidth across any cut that splits the hosts into two equal halves.
  It bounds all-to-all traffic such as MoE expert dispatch.
- **ECMP**: Equal-cost multipath routing, where a switch hashes the packet's 5-tuple to choose an uplink so each flow stays on one path and avoids reordering.
  It balances well across many small flows and poorly across few large ones (Section 8.2).
- **Hash polarization**: Correlated hashing across tiers, where identical hash functions and seeds leave some links idle while others overload.
  Operators mitigate it with per-tier seeds or by adding entropy fields.
- **Elephant flow**: A long, high-rate flow, such as one RDMA queue pair or one TCP stream carrying a collective.
  It cannot be split by ECMP, so one hash collision halves its throughput.
- **Flowlet switching**: Re-hashing a flow at idle gaps larger than the path-delay skew, so reordering is still avoided but balance improves.
- **Packet spraying**: Sending packets of one flow over many paths, which needs a receiver that tolerates out-of-order arrival.
  It is the opposite of ECMP's per-flow stickiness.
- **BGP**: The routing protocol both across the internet and inside many hyperscale fabrics, where each switch runs BGP with its neighbors.
  Services advertise VIPs over it, and withdrawing a route drains traffic.
- **Anycast**: Several locations advertise the same IP prefix, and routing delivers each client to the topologically nearest one.
  It is stable for connectionless or short-lived traffic, and a route change mid-connection breaks long TCP flows unless the backends share state.
  See [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_03_DNS_Anycast_and_Global_Traffic_Management|DNS, Anycast, and Global Traffic Management]].
- **Maglev**: Google's software L4 load balancer that uses consistent hashing into a prime-sized lookup table.
  This keeps most connections mapped to the same backend when the backend set changes (Maglev, NSDI 2016).
  See [[08-Distinguished-Engineering/02-Distributed-Systems-Internals/Consistent-Hash-Ring|Consistent Hash Ring]].
- **GFE**: Google Front End, the globally distributed edge proxy tier that terminates TLS and routes requests to backends.
- **Overlay / encapsulation (VXLAN, GENEVE)**: Wrapping tenant packets in an outer header so the physical fabric routes on host addresses and the virtual network is software-defined.
  It costs MTU headroom and requires an encap/decap engine, which is a Nitro card or Titanium adapter on hyperscalers.
- **Andromeda "Hoverboards"**: In Google's network virtualization stack, flows begin on shared gateway hosts.
  Only flows that exceed a traffic threshold are programmed as direct host-to-host routes (Andromeda, NSDI 2018).
  The mechanism saves control-plane state and programming time for the long tail of idle flows.
- **ENI**: An Elastic Network Interface on AWS, a virtual NIC with private IPs and security groups.
  Per-instance ENI and IP limits cap how many pod IPs the VPC CNI can assign on EKS.
- **Premium vs Standard Network Tier (GCP)**: Premium carries traffic on Google's backbone to the nearest edge.
  Standard hands traffic to the public internet at the region.
- **PrivateLink / Private Service Connect**: Producer services exposed as endpoints in the consumer's VPC address space, so traffic does not traverse the public internet.
- **RDMA**: One NIC writes directly into another host's registered memory with no remote CPU or kernel involvement.
  Completion is signaled through completion queues.
- **GPUDirect RDMA**: The NIC DMAs straight to and from GPU memory over PCIe peer-to-peer, which avoids a host-memory bounce buffer and its extra copy.
  It needs the NIC and GPU to share PCIe locality.
- **InfiniBand**: A lossless, credit-based link-level flow-control fabric with native RDMA and adaptive routing.
- **RoCEv2**: RDMA over UDP/IP on Ethernet.
  It needs a near-lossless fabric, built from PFC (priority flow control, link-level pause), ECN marking, and a rate controller such as DCQCN.
  Pause frames propagate backward hop by hop.
  A misbehaving NIC can start a "pause storm", and cyclic buffer dependencies can deadlock a fabric.
- **EFA and SRD (AWS)**: The Elastic Fabric Adapter exposes OS-bypass communication through libfabric.
  Its transport, Scalable Reliable Datagram, is implemented on the Nitro card.
  SRD sends one logical message over many network paths and tolerates out-of-order arrival, which sidesteps ECMP elephant collisions.
  It is not an InfiniBand-verbs transport.
  See [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_02_Interconnect_Technologies_NVLink_EFA_and_TPU_ICI|NVLink, EFA, and TPU ICI]].
- **NVLink / NVSwitch**: NVIDIA's scale-up interconnect, with GPU-to-GPU load/store semantics at far higher bandwidth than the host's PCIe.
  NVSwitch makes the domain all-to-all.
  Rack-scale systems such as GB200 NVL72 extend one NVLink domain to 72 GPUs.
- **Scale-up vs scale-out domain**: Scale-up is the high-bandwidth, low-latency island where accelerators share memory semantics (NVLink domain, one TPU slice).
  Scale-out is the Ethernet or InfiniBand network between islands.
  Parallelism strategies are assigned to domains by communication intensity, with tensor parallelism inside the scale-up domain and data parallelism across the scale-out network.
- **ICI (TPU Inter-Chip Interconnect)**: The direct chip-to-chip links of TPU slices, arranged as a torus.
  Wraparound links shorten diameters.
- **OCS (optical circuit switch)**: A MEMS-mirror switch that physically reconfigures light paths between racks.
  It lets a TPU pod be carved into differently shaped slices and routes around failed hardware without re-cabling (TPU v4, ISCA 2023).
- **DCN (data center network)**: In TPU documentation, the Ethernet-class network between slices and hosts, distinct from ICI.
  Multislice training sends gradient traffic over it.
- **NCCL**: NVIDIA's collective library.
  It builds ring and tree schedules over NVLink and the NIC, and its algorithm choice depends on message size and topology.
- **Collectives**: All-reduce, reduce-scatter, all-gather, all-to-all, and broadcast.
  Ring all-reduce moves `2(N-1)/N * S` bytes per rank for payload S, which is nearly independent of N.
  Latency grows with N, which is why trees are used for small messages.
- **TLS 0-RTT**: TLS 1.3 early data sent with the first flight under a resumption key.
  It is replayable by design, so only idempotent requests are safe.
  See [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_04_Edge_Infrastructure_and_TLS_Termination|Edge Infrastructure and TLS Termination]].

---

## 3. Storage, databases, and consensus

- **Block / object / file**: Block storage presents a fixed-size, random-access device.
  Object storage presents immutable whole objects addressed by key over HTTP.
  File storage presents a POSIX namespace with locks and renames.
  The layer that gives you atomic rename or append decides what a workload can run on.
- **NVMe queue pairs**: Submission and completion queues in host memory, with doorbell registers on the device.
  Parallelism comes from many queues, each lock-free and ideally one per core.
- **Flash translation layer (FTL) and write amplification**: NAND is written in pages but erased in blocks, so the controller remaps logical to physical pages and garbage-collects.
  Write amplification is the ratio of physical to host writes.
  Random small writes at high utilization drive it up, which shortens endurance and creates latency spikes during GC.
- **IOPS, throughput, latency, queue depth**: These are tied by Little's law, `concurrency = rate x latency`.
  `IOPS = queue depth / mean latency`.
  Hitting a provisioned IOPS number needs enough outstanding I/O, and a single-threaded synchronous client cannot do it.
- **Network-attached block volume (EBS, Persistent Disk, Hyperdisk)**: A virtual disk replicated by a storage service and reached through the offload card.
  The cost is a network round trip per I/O, and the benefit is detaching volume lifetime from the host.
  See [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_09_Elastic_Block_Store_and_Network_Storage|EBS and Network Storage]].
- **Physalia**: The EBS configuration store, built from a very large number of small Paxos-based cells so that a failure affects only the volumes in a nearby cell.
  Cell placement is chosen close to the clients that use the data (Physalia, NSDI 2020).
  See [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_03_Physalia_EBS_Control_Plane|Physalia]].
- **Quorum (R, W, N)**: A read of R replicas overlaps any write to W replicas of N when `R + W > N`.
  Requiring `W > N/2` also stops two disjoint writes from both succeeding.
  Aurora uses N=6 across three AZs with W=4 and R=3, which gives write availability through a lost AZ and read availability through a lost AZ plus one more node (Aurora, SIGMOD 2017).
- **Log is the database**: In Aurora the compute tier ships only redo records.
  Storage nodes materialize pages on demand and in the background, so the network carries log records, not full pages.
  See [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_11_Amazon_Aurora_Log_Is_The_Database|Aurora: Log-Is-The-Database]].
- **Paxos / Raft**: Leader-based consensus protocols that replicate a log by majority.
  A leader commits an entry once a majority has persisted it, and a new leader must contain every committed entry.
  See [[08-Distinguished-Engineering/02-Distributed-Systems-Internals/Raft-Consensus|Raft Consensus]] and [[04-System-Design/05-Data-Stores/Apache-ZooKeeper|Apache ZooKeeper]].
- **Strong consistency vs external consistency**: Strong consistency (linearizability) means operations appear to execute atomically at some point between invocation and response.
  External consistency also orders transactions by real time: if T1 commits before T2 starts, then T1's timestamp is lower.
  Spanner provides it via TrueTime (Section 8.4).
- **TrueTime**: An API that returns a time interval `[earliest, latest]` guaranteed to contain absolute time.
  Its width is the uncertainty epsilon, driven by GPS and atomic-clock time masters plus polling error (Spanner, OSDI 2012).
- **S3 strong consistency**: Since December 2020, S3 provides strong read-after-write and list consistency.
  Mechanism details are in [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_08_S3_Deep_Dive_Consistency_and_ShardStore|S3 Deep Dive]] and [[04-System-Design/05-Data-Stores/Amazon-S3-and-Object-Storage|Amazon S3 and Object Storage]].
- **ShardStore**: S3's per-disk storage node implementation, a log-structured design with an LSM-tree index.
  It was checked against an executable reference model using property-based and lightweight formal methods, specifically for crash consistency (ShardStore, SOSP 2021).
- **Colossus**: Google's successor to GFS, with a distributed, scalable metadata layer instead of a single master (GFS, SOSP 2003, for the lineage).
  Cloud Storage and Bigtable sit on it.
- **Erasure coding RS(k, m)**: Data is cut into k chunks plus m parity chunks, and any k of the k+m reconstruct the data.
  Storage overhead is `(k+m)/k`.
  The hidden cost is repair: rebuilding one lost chunk reads k chunks, so repair traffic is amplified k-fold relative to 3x replication's single-copy read (Section 8.6 gives the numbers).
- **LSM tree**: Writes go to a WAL and a memtable, which flushes as immutable sorted files (SSTables) that are merged by compaction.
  It has high write throughput.
  Read amplification rises with the number of files, and compaction competes for disk and CPU.
  See [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_11_Cloud_Bigtable_LSM_Storage_and_Tablet_Serving|Cloud Bigtable]].
- **Partition split / hot partition**: A key range is divided when it exceeds a size or throughput threshold.
  A single hot key cannot be split, so a hot-key workload is bounded by one partition's capacity no matter what the table is provisioned for.
- **Hedged request**: Send a second copy of a slow request to another replica after a delay near the p95, and take the first reply.
  It cuts tail latency at a small cost in extra load (The Tail at Scale, CACM 2013).
- **Parallel file system (Lustre, FSx for Lustre)**: Metadata servers plus many object storage servers with files striped across them.
  It is used for training data and checkpoints that need POSIX semantics over a cheaper object tier.
- **Checkpoint**: A persisted snapshot of model, optimizer state, and data-loader position.
  Asynchronous checkpointing copies to host memory or NVMe quickly and flushes to object storage in the background, so the training step pays only the first copy.
  See [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_06_High_Throughput_Checkpointing_to_Object_Storage|High-Throughput Checkpointing]].

---

## 4. Reliability and failure domains

- **Availability Zone (AWS) / Zone (GCP)**: One or more datacenters with independent power, cooling, and networking inside a region.
  AWS account-specific AZ names such as `us-east-1a` can map to different physical AZs per account, so use AZ IDs when coordinating across accounts.
  See [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_06_Physical_Failure_Domains_and_Blast_Radius|Physical Failure Domains and Blast Radius]].
- **Region**: A geographic location containing multiple zones and its own regional control plane instances.
- **Static stability**: A system keeps operating correctly in the face of a dependency impairment without having to take new actions.
  The standard example is over-provisioning capacity across AZs in advance, so that surviving AZs absorb load without calling a control plane that may itself be impaired.
  See [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_02_Regions_Zones_and_Static_Stability|Regions, Zones, and Static Stability]].
- **Cell-based architecture**: The fleet is split into independent full-stack copies (cells), and a thin router maps each customer to one cell.
  Blast radius is bounded by cell size.
- **Shuffle sharding**: Each customer is assigned a random subset of workers, so two customers rarely share their whole subset (Section 8.5 has the combinatorics).
- **Control plane vs data plane**: The control plane creates, configures, and places resources, and it is complex and lower availability.
  The data plane serves requests using already-configured state, and it must keep working when the control plane is down.
  See [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_04_Control_Plane_vs_Data_Plane_Separation|Control Plane vs Data Plane Separation]].
- **Constant work**: Do the same amount of work regardless of load or failure state, for example by pushing full configuration snapshots on a timer.
  This removes a load-proportional failure mode at the cost of steady overhead.
- **Load shedding / backpressure / admission control**: Shedding rejects work above capacity so accepted work meets its deadline.
  Backpressure propagates saturation upstream.
  Admission control makes the accept decision at the edge.
  See [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_02_Load_Shedding_Backpressure_and_Admission_Control|Load Shedding, Backpressure, and Admission Control]].
- **Retry storm**: Retries multiply offered load during a partial failure.
  With R retries at each of L layers, worst-case amplification is `(R+1)^L`.
- **Metastable failure**: A state in which a sustaining feedback loop, such as retries, cold caches, or queue backlog, keeps the system degraded after the original trigger is gone.
  Recovery requires breaking the loop by shedding load or resetting state (Bronson et al., HotOS 2021).
- **Gray failure**: A component that is degraded but passes health checks, so the failure detector and the application observe it differently (Huang et al., HotOS 2017).
  A GPU with a thermal throttle or a flapping optical link is typical.
- **Silent data corruption (SDC)**: A defective core or accelerator returns wrong results without any error signal.
  The only defenses are redundancy, checksums on data in motion, and cross-replica comparison, such as comparing gradient norms across data-parallel ranks.
  See [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_08_Fault_Tolerance_and_Fleet_Resiliency_at_Scale|Fault Tolerance and Fleet Resiliency at Scale]].
- **SLI / SLO / error budget / burn rate**: The SLI is a measured ratio of good events to valid events.
  The SLO is the target for it, and the error budget is `1 - SLO`.
  Burn rate is the observed error rate divided by the budget rate, so a burn rate of 1 exhausts the budget exactly at the window's end.
  See [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_01_SLIs_SLOs_and_Error_Budget_Engineering|SLIs, SLOs, and Error Budget Engineering]].
- **RTO / RPO**: Recovery time objective is how long you may be down.
  Recovery point objective is how much acknowledged data you may lose.
  Asynchronous cross-region replication gives RPO equal to replication lag at failure time, not zero.
- **PIR**: Post-incident review.
  The AWS and Google public incident reports are primary sources for failure-mode patterns (see [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_07_Anatomy_of_Major_Cloud_Outages|Anatomy of Major Cloud Outages]]).

---

## 5. Accelerators and AI cluster hardware

- **SM / Tensor Core (NVIDIA)**: A streaming multiprocessor runs warps of threads.
  Tensor Cores execute small matrix-multiply-accumulate tiles at reduced precisions such as bf16, FP8, and FP4.
- **MXU and systolic array (TPU)**: A grid of multiply-accumulate cells through which operands flow rhythmically, so each loaded value is reused many times without returning to memory.
  Each v5p TensorCore has four MXUs (Google Cloud docs, as of 2026-10).
  MXUs were 128x128 through the v5 generation per the TPU papers.
  Trillium is reported to use a larger array (UNVERIFIED against primary docs).
- **SparseCore (TPU)**: A dataflow unit for embedding lookups and other irregular access, separate from the MXUs.
- **NeuronCore (Trainium)**: The AWS Trainium compute unit, programmed through the Neuron SDK.
  See [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_01_Accelerator_Architectures_GPUs_Trainium_TPUs|Accelerator Architectures]].
- **HBM**: High Bandwidth Memory, DRAM dies stacked vertically and connected to the accelerator die through a silicon interposer with a very wide interface.
  It gets bandwidth from width, not clock speed.
  Capacity is limited by stack count and heat.
- **Roofline model**: Attainable FLOP/s is `min(peak FLOP/s, arithmetic intensity x memory bandwidth)`.
  The ridge point `peak / bandwidth` is the intensity at which the kernel moves from memory-bound to compute-bound (Section 8.3).
- **Arithmetic intensity**: FLOPs performed per byte moved from memory.
- **MFU / HFU**: Model FLOPs utilization is achieved model FLOPs over peak.
  Hardware FLOPs utilization also counts recomputed FLOPs, such as those from activation checkpointing.
- **Goodput**: Useful training progress per wall-clock time after subtracting failures, restarts, checkpoint stalls, and wasted recomputation.
  It is the metric procurement should use, not peak FLOP/s.
- **Slice (TPU)**: A set of chips connected by ICI with a defined topology, allocated as a unit.
  Slices must be contiguous in the torus.
  On v5p the largest schedulable job is a 96-cube, 6144-chip job out of a pod of 8960 chips (Google Cloud docs, as of 2026-10).
- **Multislice**: Training that spans several slices, using ICI within a slice and DCN between slices.
- **Superpod / UltraCluster / UltraServer**: Vendor names for the largest tightly coupled unit.
  They are not interchangeable.
  Check what interconnect joins the members and at what bandwidth before comparing them.
- **Gang scheduling**: All-or-nothing placement of every rank of a job.
  A partial allocation wastes capacity because the ranks that started wait forever for the ones that did not.
  Combined with topology constraints, this fragments clusters.
- **Straggler**: One slow rank that gates every synchronous collective.
  Step time is the maximum over ranks, not the mean.
- **Capacity reservation (ODCR, Capacity Blocks, calendar-mode reservations)**: Time-bounded guaranteed accelerator capacity.
  Provider names and terms change often, so verify them against current docs before designing around one.
- **Spot / preemptible**: Discounted capacity that can be reclaimed with a short notice.
  The notice length differs between AWS and GCP, and workloads must checkpoint faster than the notice window.
  See [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_03_Spot_Economics_and_Interruption_Handling|Spot Economics and Interruption Handling]].

---

## 6. Training and LLM serving terms

- **Data / tensor / pipeline / sequence / expert parallelism (DP, TP, PP, SP, EP)**: DP replicates the model and splits the batch, and it communicates gradients once per step.
  TP splits individual matmuls and communicates activations several times per layer, so it must sit inside the scale-up domain.
  PP splits layers into stages and communicates activations point to point, and it pays a pipeline bubble.
  SP splits the sequence dimension.
  EP places MoE experts on different devices and needs all-to-all.
  See [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_04_Distributed_Training_Parallelism_Strategies|Distributed Training Parallelism Strategies]].
- **ZeRO / FSDP**: Shard optimizer state (stage 1), gradients (stage 2), and parameters (stage 3) across data-parallel ranks, and gather parameters just in time.
  Mixed-precision Adam holds about 16 bytes per parameter in model state (ZeRO, SC 2020), so a 70B-parameter model holds about 1.12 TB of state before activations.
  Sharded across 64 ranks, that is about 17.5 GB per rank (Section 8.6).
- **Activation checkpointing**: Discard activations in the forward pass and recompute them in the backward pass.
  It trades roughly one extra forward pass of compute for a large cut in activation memory.
- **Prefill**: Processing the whole prompt in one pass.
  It is dominated by matrix-matrix multiplies and is typically compute-bound.
- **Decode**: Generating one token per step per sequence.
  Each step reads all weights and the sequence's KV cache for little compute, so it is typically memory-bandwidth-bound.
- **TTFT / ITL (TPOT)**: Time to first token is dominated by queueing plus prefill.
  Inter-token latency (time per output token) is dominated by decode step time.
- **KV cache**: Stored attention keys and values for every past token, per layer.
  Per-token size is `2 x layers x kv_heads x head_dim x bytes` (Section 8.3).
- **GQA / MQA / MLA**: Attention variants that share or compress key-value heads to shrink the KV cache.
- **PagedAttention**: KV storage in fixed-size blocks addressed through a per-sequence block table, like virtual memory.
  It removes the external fragmentation and over-reservation of contiguous per-sequence buffers.
  See [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_03_KV_Cache_Management_and_PagedAttention|KV-Cache Management and PagedAttention]].
- **Continuous (iteration-level) batching**: Sequences join and leave the running batch at every decode step, so short requests do not wait for the longest in a static batch.
  See [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_04_Continuous_Batching_and_Scheduling_Algorithms|Continuous Batching and Scheduling Algorithms]].
- **Chunked prefill**: Split a long prompt into chunks interleaved with decode steps, so one long prompt does not stall every other stream's inter-token latency.
- **Prefix caching / RadixAttention**: Reuse KV blocks for a shared prompt prefix, such as a system prompt, through a hash or radix-tree index.
  It is only a win if requests with the same prefix route to the same replica.
- **Speculative decoding**: A cheap draft proposes k tokens and the target model verifies them in one forward pass.
  The output distribution is preserved, and the gain depends on the acceptance rate.
- **Weight-only quantization (AWQ, GPTQ) vs FP8 compute**: Weight-only schemes cut bytes read per decode step and are dequantized to bf16 on the fly.
  FP8 or INT8 compute also raises peak math throughput.
- **Prefill/decode disaggregation**: Run prefill and decode on separate pools so each is sized and batched for its own bottleneck.
  The KV cache must be transferred between pools, so the transfer must be cheaper than the interference it removes.
  See [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_07_Prefill_and_Decode_Disaggregation|Prefill and Decode Disaggregation]].
- **Cold start (GPU serving)**: Time from scale-out decision to first served token.
  It is the sum of node provisioning, image pull, weight load, and engine warmup, and weight load from object storage usually dominates.
- **Provisioned throughput vs on-demand (managed inference)**: Reserved model capacity versus shared capacity with quotas and throttling.
  See [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_09_Managed_Inference_Amazon_Bedrock_vs_Vertex_AI|Managed Inference: Bedrock vs Vertex AI]] and [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_06_Capacity_and_Quotas|Capacity and Quotas]].

---

## 7. Cloud naming traps (AWS vs GCP)

| Concept | AWS | GCP | Trap |
| --- | --- | --- | --- |
| Isolation unit for billing and IAM | Account | Project | Both sit under an org hierarchy, but GCP policy inherits down a folder tree, while AWS guardrails attach as SCPs on OUs. |
| Network scope | VPC is regional | VPC is global with regional subnets | Cross-region reachability inside one GCP VPC needs no peering, and the equivalent on AWS needs peering or Transit Gateway. |
| Guardrail policy | SCP | Organization Policy | An SCP only limits what identity policies can grant and never grants access itself. An Org Policy constrains resource configuration. |
| Private API access | VPC endpoint (PrivateLink) | Private Service Connect, VPC Service Controls | PrivateLink is a network path. VPC Service Controls is an API-level perimeter with ingress and egress rules, and AWS data-perimeter work uses endpoint policies and condition keys instead. |
| Workload identity | IAM role via instance profile, IRSA, Pod Identity | Attached service account, Workload Identity | Prefer short-lived federated credentials over exported keys on both clouds. |
| Offload hardware | Nitro | Titanium | Both remove the hypervisor from the I/O path, but they are different products with different guest-visible devices. |
| HPC and AI fabric | EFA over SRD | RDMA/RoCE-based stacks on GPU VMs, ICI on TPU | Do not assume one transport model. Collectives libraries are tuned per transport. |
| Zone naming | AZ name varies per account, AZ ID is stable | Zone names are stable | Cross-account AZ affinity on AWS requires AZ IDs. |

---

## 8. Mechanisms the getting-started guides skip

### 8.1 Nested paging and why huge pages matter on accelerator hosts

With EPT, a guest virtual address needs a guest page-table walk, and every guest page-table pointer is itself a guest-physical address that needs a host walk.
With 4-level guest and 4-level host tables, the worst-case TLB miss touches `(4+1) x (4+1) - 1 = 24` memory references, versus 4 for a bare-metal walk.
Paging-structure caches absorb most of these, but a large working set that misses the TLB pays the full cost.
Backing guest memory with 2 MiB or 1 GiB host pages removes whole levels of the host walk and shrinks the number of TLB entries needed.
This is one reason large-memory instances and accelerator hosts pin and huge-page guest memory.
It is also why the Nitro and Titanium designs keep hot I/O out of the host software path: it removes a source of jitter, not only CPU cycles.

### 8.2 ECMP collisions and why collectives see tail latency

Take 8 elephant flows hashed onto 8 equal uplinks.
The probability that every uplink gets exactly one flow is `8!/8^8 = 40320/16,777,216 = 0.24%`.
The expected number of idle uplinks is `8 x (7/8)^8 = 2.75`.
So some link carries two or more flows in 99.76% of placements, and those flows get half the bandwidth.
A synchronous ring or tree all-reduce finishes when its slowest flow finishes, so one collision stretches that step's communication time toward 2x for the whole job.
Splitting each transfer across k queue pairs improves the imbalance only slowly: with per-link load modeled as binomial, the relative standard deviation is `sqrt((1-p)/(n p))` with p = 1/8.
That is 0.94 at n = 8 flows and 0.47 at n = 32 flows, so it falls as the square root of n.
Per-packet spraying with out-of-order tolerance (the SRD approach) or adaptive routing in the switch removes the per-flow hash from the picture.
The costs are receiver reordering logic and a transport that cannot be offloaded to a standard verbs NIC.
Both approaches move the problem from placement luck to congestion-control quality.

### 8.3 Roofline arithmetic for decode and the KV cache

For a dense model with P parameters in bf16, one decode step for a batch of B sequences does about `2 x B x P` FLOPs and reads `2P` bytes of weights, so arithmetic intensity is about B FLOP/byte.
On TPU v6e the ridge point is `918 TFLOP/s / 1.638 TB/s = 560 FLOP/byte` (Google Cloud docs, as of 2026-10).
Decode therefore needs a batch near 560 sequences to be compute-bound, before counting KV-cache reads, which add bytes and push the ridge farther out.
Weight-only 8-bit quantization halves the bytes and doubles the intensity (2B), so the ridge is reached at about B = 280 for the weights alone.
This is the quantitative reason decode throughput scales with batch size and with HBM bandwidth, not FLOPs.

KV cache size per token is `2 x layers x kv_heads x head_dim x bytes_per_element`.
For an illustrative model with 80 layers, 8 KV heads, head dimension 128, and bf16 (2 bytes), that is `2 x 80 x 8 x 128 x 2 = 327,680` bytes, or 320 KiB per token.
A 128k-token context is `327,680 x 131,072 = 40 GiB` for one sequence.
One hundred concurrent 8k-token sequences need `100 x 8192 x 327,680 = 250 GiB`.
With 16-token blocks, a block is 5 MiB, so a paged allocator wastes at most one partial block per sequence instead of reserving the maximum length up front.

### 8.4 TrueTime commit wait

Spanner assigns a commit timestamp `s` no earlier than `TT.now().latest` at the commit request.
It then holds the result until `TT.after(s)` is true, meaning every clock in the system agrees that `s` is in the past.
Any transaction that starts after this one finishes therefore receives a later timestamp, which is external consistency.
The expected wait is at least `2 x epsilon` where epsilon is the average clock uncertainty.
The 2012 paper reports epsilon between about 1 and 7 ms in production, averaging about 4 ms (Spanner, OSDI 2012).
That implies roughly 8 ms of commit wait.
The wait overlaps with Paxos replication, so it adds little when replication is already slow.
Locks are held during the wait, so conflicting writes to one hot row serialize at roughly `1 / (2 x epsilon)`, which is about 125 transactions per second at 4 ms.
If time masters degrade and epsilon grows, throughput and latency degrade smoothly and correctness holds.
The design chooses to slow down rather than risk violating ordering.

### 8.5 Shuffle sharding combinatorics

With 8 workers and each customer assigned 2 of them, there are `C(8,2) = 28` distinct shards.
A random second customer shares the same full shard with probability `1/28 = 3.6%`.
With 100 workers and 5 per customer, there are `C(100,5) = 75,287,520` shards, so a poison-pill customer who takes down all 5 of its workers fully overlaps almost nobody.
Partial overlap still happens, so clients must retry across the shard's workers.
The assignment function and the router that implements it are single points of failure, and they need their own cell-style containment.

### 8.6 Young/Daly checkpoint interval, erasure repair, and ZeRO memory arithmetic

For a job with mean time between failures M (seconds) and blocking checkpoint cost C (seconds), the Young/Daly first-order optimum checkpoint interval is `tau = sqrt(2 C M)`.
At the optimum, the overhead fraction from checkpointing and lost work is about `sqrt(2 C / M)`, and restart time R adds about `R / M` per failure.
If device failures are independent and exponential, the job's MTBF is the device MTBF divided by the device count, so MTBF shrinks linearly as the job grows.
Assume a job-level MTBF of 6 h (21,600 s).
With C = 120 s, `tau = sqrt(2 x 120 x 21,600) = 2,277 s` (about 38 min) and the overhead is `sqrt(240/21,600) = 10.5%`.
With asynchronous checkpointing that blocks the step for only C = 10 s, `tau = 657 s` and the overhead is 3.0%.
With R = 600 s restart, add `600/21,600 = 2.8%`.
These inputs are illustrative assumptions, not measurements.
Correlated failures, such as a rack or a switch taking out many ranks at once, break the independence assumption and shorten M beyond what the formula predicts.

For RS(6,3), storage overhead is `9/6 = 1.5x` against 3x for triple replication, and it tolerates any 3 chunk losses.
Rebuilding one lost chunk reads 6 chunks, a 6x read amplification of the lost data.
Repair bandwidth and the time spent with reduced redundancy are the operational price of the capacity saving.

For ZeRO accounting, mixed-precision Adam needs per parameter: 2 bytes bf16 weights, 2 bytes bf16 gradients, and 12 bytes optimizer state (fp32 master weights, momentum, and variance), which totals 16 bytes (ZeRO, SC 2020).
For P = 70e9 that is `16 x 70e9 = 1.12e12` bytes.
ZeRO-3 across 64 ranks divides it to `1.12e12 / 64 = 17.5e9` bytes per rank before activations and gather buffers.

---

## 9. Verified accelerator numbers (Google Cloud TPU)

| Spec | v5p | v6e (Trillium) | TPU7x (Ironwood) |
| --- | --- | --- | --- |
| Chips per pod | 8960 | 256 | 9216 |
| HBM per chip (GiB) | 95 | 32 | 192 |
| HBM bandwidth per chip (GBps) | 2765 | 1638 | 7380 |
| Bidirectional ICI per chip (GBps) | 1200 | 800 | 1200 |
| DCN per chip (Gbps) | 50 | 100 | 100 |
| Derived: pod HBM (TiB) | 831 | 8 | 1728 |

Source for the table rows other than the derived row: [TPU7x docs](https://docs.cloud.google.com/tpu/docs/tpu7x) (Google Cloud docs, as of 2026-10).
The derived row is my arithmetic from chips per pod times HBM per chip.

Additional verified facts:
- v6e peak bf16 compute is 918 TFLOPs per chip and peak Int8 is 1836 TOPs (Google Cloud docs, as of 2026-10; [TPU v6e](https://docs.cloud.google.com/tpu/docs/v6e)).
- v5p has 8960 chips in a pod, and the largest schedulable job is a 96-cube (6144-chip) job (Google Cloud docs, as of 2026-10; [TPU v5p](https://docs.cloud.google.com/tpu/docs/v5p)).
- TPU7x (Ironwood) is listed as generally available in the Cloud TPU release notes, dated March 31, 2026 (Google Cloud docs, as of 2026-10; [release notes](https://docs.cloud.google.com/tpu/docs/release-notes)).
- Google announced eighth-generation TPUs on 2026-04-22: TPU 8t for training and TPU 8i for inference. They were described as "available later this year" at announcement time, so treat them as announced, not confirmed GA, until you check the release notes ([Google blog](https://blog.google/innovation-and-ai/infrastructure-and-cloud/google-cloud/eighth-generation-tpu-agentic-era/), as of 2026-10).
- Announced TPU 8t scale is 9,600 chips and two petabytes of shared HBM per superpod ([Google blog](https://blog.google/innovation-and-ai/infrastructure-and-cloud/google-cloud/eighth-generation-tpu-agentic-era/), as of 2026-10).
- Reported TPU 8i scale is 1,152 chips per pod, with 19.2 Tbps bidirectional scale-up bandwidth per chip (trade press, [DatacenterDynamics](https://www.datacenterdynamics.com/en/news/google-unveils-eighth-generation-tpus-two-dedicated-training-and-inference-chips/), as of 2026-10).

---

## 10. Commonly confused pairs

| Pair | Distinction an L6 candidate states |
| --- | --- |
| Latency vs throughput | Little's law links them through concurrency, and a system can hold either constant while the other collapses under queueing. |
| Availability vs durability | Durability is the probability data survives, and availability is the probability you can read it now. A region outage can reduce availability without any loss of durability. |
| Replication vs backup | Replication copies mistakes and deletions immediately. A backup is a separate, versioned, independently access-controlled copy. |
| Strong vs eventual consistency | Define the read-after-write guarantee for the exact operation and API. "Strong for objects, eventual for something else" is common. |
| Scale-up vs scale-out | The difference is bandwidth and semantics (shared memory domain vs network), not just size. |
| Peak FLOP/s vs MFU | Peak is marketing, and MFU measures how much of it the software and fabric deliver. |
| HFU vs MFU | HFU counts recomputation, so aggressive activation checkpointing raises HFU without raising useful progress. |
| Prefill vs decode | Different bottlenecks (compute vs memory bandwidth), so the same batching and parallelism choices do not serve both. |
| Cell vs zone | A zone is a physical failure domain owned by the provider. A cell is a logical software partition you design. |
| Container isolation vs VM isolation | Containers share the host kernel. VMs share only the hypervisor and hardware. |
| Control plane outage vs data plane outage | A control plane outage blocks changes, and a data plane outage blocks served traffic. Static stability exists to keep the first from becoming the second. |

---

## 11. Interview questions and model answers

### Q1 (L5): Why does Lambda run each tenant in a microVM instead of a container, and what do you give up?

A container shares one host kernel, so a kernel vulnerability or a mis-scoped capability becomes a cross-tenant breach.
The kernel attack surface is the whole syscall interface, and seccomp filters narrow it but cannot make it small.
A microVM puts the hardware virtualization boundary between tenants, so the shared attack surface shrinks to the VMM and a handful of devices.
Firecracker keeps that device model tiny (virtio net and block, vsock, serial) and reports under 125 ms boot and under 5 MiB overhead per microVM (Firecracker, NSDI 2020).
What you give up is density and warm-state sharing, because each microVM has its own guest kernel and page cache.
Boot time is also higher than a container start, which is why snapshot restore exists.
Snapshot restore brings a correctness risk: clones share identical RNG state and cached secrets unless the guest is told it was restored.

Follow-up: A team's function generates idempotency keys with a UUID library initialized during init and gets duplicates after scale-out.
Explain the root cause and the fix.
Expected answer: the RNG seed was captured in the snapshot, so every clone continues the same sequence.
The fix is to reseed or generate after restore (restore hooks, or generate per invocation), never during init.
Monitor for duplicates at the consumer too.

### Q2 (L5): A decode-heavy service on one accelerator is at 20% of peak FLOP/s with p99 inter-token latency fine.
Should you buy faster chips?

Probably not.
Decode reads all weights per step for about 2 FLOPs per weight per sequence, so at batch B the intensity is about B FLOP/byte for bf16.
On v6e the ridge is 560 FLOP/byte (918 TFLOP/s over 1.638 TB/s, Google Cloud docs, as of 2026-10), so batches below that are bandwidth-bound and low FLOP utilization is expected.
The levers are, in order of cost: raise the batch via continuous batching and more concurrency per replica, shrink bytes via weight-only quantization (intensity doubles at 8 bits), shrink KV bytes via GQA or a quantized cache, and only then buy more HBM bandwidth.
Raising batch is limited by KV capacity and by the latency SLO.
Each added sequence costs 320 KiB per token in the Section 8.3 example, so memory runs out before the ridge is reached.
Check whether you are really bandwidth-bound by comparing observed step time to `bytes_read / HBM_bandwidth`.

Follow-up: Batch is already at the KV-capacity limit.
What next?
Expected: paged and prefix-shared KV, disaggregating decode onto HBM-heavy hardware, or tensor-parallel across more chips to pool HBM, accepting the all-reduce cost per layer.

### Q3 (L6): Your 1,024-GPU job shows periodic step-time spikes with no hardware errors.
Network utilization looks fine on average.
How do you reason about it?

Averages hide per-link collisions.
In a fabric using ECMP, an RDMA queue pair is an elephant flow pinned to one path.
With 8 flows on 8 uplinks, only 0.24% of placements are collision-free, so some link is almost always shared.
Because collectives are synchronous, step time follows the slowest flow, which can approach 2x for the communication portion.
The spikes are periodic because placements change when QPs are re-created, for example after a restart or an elastic resize.
Diagnosis: pull per-link counters and ECN or pause counters at leaf and spine, correlate with job phase, and run a controlled all-to-all or all-reduce benchmark across the same hosts to see run-to-run bandwidth variance.
Mitigations: more QPs per peer to smooth load (variance falls as 1/sqrt(n)), per-tier hash seeds, adaptive routing or packet spraying where the transport supports it, and topology-aware rank placement so the heaviest collectives stay under one leaf.
The trade-off: spraying needs reordering-tolerant transports, and more QPs raise NIC state and congestion-control complexity.
Also rule out PFC pause propagation, because a single slow receiver can look identical from the application.

Follow-up: How would you prove it was polarization, not a failing optic?
Expected: a failing optic shows CRC or FEC errors on one port and persists across job restarts, while collisions follow placement and move when you change QP count or hash seed.

### Q4 (L6): Compare what Spanner and Aurora each guarantee under a regional write path, and explain what commit wait buys you and what it costs.

Aurora's write path is a 4-of-6 quorum across three AZs, so a commit is durable once four storage nodes persist the redo record.
That gives high write availability through an AZ loss and strong consistency within one writer (Aurora, SIGMOD 2017).
Cross-region it is asynchronous, so RPO equals replication lag.
Spanner commits through Paxos groups, with two-phase commit across groups, and assigns timestamps with TrueTime.
Commit wait delays acknowledging until the timestamp is in the past everywhere, which gives external consistency: if T1 completes before T2 begins, T1's timestamp is smaller.
This lets snapshot reads at a timestamp be lock-free and globally consistent.
The cost is about `2 x epsilon` of added latency per read-write commit (about 8 ms at the 4 ms average uncertainty the 2012 paper reports), and locks held through the wait, bounding single-row write rate near `1/(2 epsilon)`.
Choose it where cross-region ordering matters and the writes are not concentrated on a few keys.

Follow-up: Time masters in one region fail and epsilon grows tenfold.
What does the system do and what do you tell product teams?
Expected: correctness is preserved, latency and contention rise, hot-row throughput falls by the same factor.
Run game days that inflate epsilon, and size timeouts for it.

### Q5 (L6): Pick a checkpoint interval for a 4,096-accelerator pretraining job and justify it.

Start from the failure model.
If device failures are independent, job MTBF is device MTBF divided by 4,096, so a fleet-level MTBF of hours is normal at this scale.
Young/Daly gives `tau = sqrt(2 C M)` and an overhead of `sqrt(2 C / M)`.
With an assumed M of 6 h and a blocking cost C of 120 s, that is a 38-minute interval and 10.5% overhead.
Cutting the blocking cost to 10 s with asynchronous checkpointing (device to host to NVMe, then flush to object storage) gives an 11-minute interval and 3.0% overhead.
Add restart cost R, which is detection plus rescheduling plus weight load plus data-loader fast-forward, about `R/M` per failure.
Then address what the formula omits: failures are correlated in racks and switches, so use topology-aware spare capacity and hot spares for fast node swap.
Checkpoints must be validated, because a checkpoint written during silent corruption is worthless.
Keep at least the last two good checkpoints, and cap object-store write bandwidth so the flush does not starve data loading.

Follow-up: Async flush takes longer than the interval.
Expected: the flush queue grows, memory pressure on host RAM rises, and you must either lengthen the interval, add flush bandwidth, or skip a checkpoint with explicit accounting of the exposure window.

### Q6 (L7): Define blast radius quantitatively for a multi-tenant inference platform and show how you would reduce it for a poison-pill prompt that crashes the engine.

Blast radius is the expected fraction of customers or requests affected by one fault, bounded by the smallest independent unit that fault can traverse.
Without partitioning, a poison prompt crashing one replica is retried by clients onto the next replica, and it takes down the pool, which is a metastable retry loop.
Controls, in layers: cell the fleet so a router maps tenants to a cell and a crash cannot cross cells.
Within a cell, shuffle shard tenants onto small replica subsets.
With 100 replicas and 5 per tenant there are 75,287,520 distinct shards, so the chance two tenants fully overlap is negligible.
Add a per-request crash fingerprint at the gateway: after a replica dies serving request hash h, quarantine h and fail it fast instead of retrying.
Retry budgets (a fraction of recent successes) stop amplification.
The costs: shuffle sharding lowers statistical multiplexing and so raises the replicas needed for the same utilization, KV prefix-cache hit rate falls when a tenant's traffic is spread over fewer or different replicas, and the router becomes a critical component that needs its own static stability and cell containment.
Measure the result with fault-injection drills that report the fraction of tenants impacted per injected poison prompt, which is the number the design is meant to bound.

Follow-up: Your cell router is now the single point of failure.
Expected: make routing a pure function of tenant ID with the mapping pushed as constant-work config snapshots, so routers keep working with the last good map when the control plane is down.

### Q7 (L7): You are choosing between a rack-scale NVLink domain and a TPU torus slice for a large MoE model.
What decides it?

Start from the communication pattern.
MoE expert parallelism needs all-to-all of token activations every layer, and bisection bandwidth and per-hop latency bound it.
A rack-scale NVLink domain gives all-to-all through NVSwitch inside the domain with load/store semantics and uniform latency.
The domain size caps how many experts you can place without crossing the scale-out network.
A TPU torus gives high-bandwidth nearest-neighbor links, and all-to-all traverses multiple hops, so mapping experts onto torus dimensions matters.
Its strengths are very large coherent slices (a v5p slice up to 6144 chips, Google Cloud docs, as of 2026-10) and OCS reconfiguration to carve slices and route around faults.
Then operations decide as much as peak numbers: gang scheduling and fragmentation (a torus slice must be contiguous, a failed node can strand a slice shape), spare-node strategy, software maturity of your framework on each platform, quota and capacity availability, and unit economics per goodput-hour, not per peak-FLOP.
I would run the same step on both and compare MFU and goodput including failures before committing a multi-year capacity reservation.

Follow-up: One chip in the domain gives intermittently wrong results with no error.
Expected: SDC detection through cross-replica gradient-norm or loss divergence checks, periodic known-answer tests during idle windows, and an automated drain-and-replace policy.
Losing a node from a rack-scale domain has a different blast radius than losing one from a torus.

### Q8 (L6): A VPC-attached service on EKS cannot scale pods past a limit even though subnets have free IPs.
Walk through the mechanism and the options.

With the AWS VPC CNI in its default mode, each pod gets a real VPC IP from secondary addresses on the node's ENIs.
Per-instance limits on ENIs and IPs per ENI cap pods per node.
Free addresses in the subnet do not help because the node's attach limits bind first.
Options: use larger instance types with higher limits, enable prefix delegation so each ENI slot carries a /28 block instead of one address (fewer slots consumed, but coarser subnet fragmentation), or use an overlay or alternative CNI such as Cilium in overlay mode.
Overlay removes the IP-per-ENI bound at the cost of encapsulation overhead, losing VPC-native pod addressing, and security groups per pod.
Check subnet fragmentation if you adopt prefix delegation, because contiguous /28 availability can run out before free IP counts do.

Follow-up: What breaks first when you move to an overlay?
Expected: anything that relies on VPC-native pod IPs (security group references, flow logs at pod granularity, direct ALB IP-target mode), plus MTU reduction from encapsulation.

---

## 12. Further reading

- [Firecracker: Lightweight Virtualization for Serverless Applications](https://www.usenix.org/conference/nsdi20/presentation/agache) - the source for microVM design choices and the numbers above.
- [Spanner: Google's Globally-Distributed Database](https://research.google/pubs/spanner-googles-globally-distributed-database-2/) - TrueTime and commit wait in the original text.
- [Amazon Aurora: Design Considerations for High Throughput Cloud-Native Relational Databases](https://www.amazon.science/publications/amazon-aurora-design-considerations-for-high-throughput-cloud-native-relational-databases) - quorum and log-is-the-database mechanics.
- [Millions of Tiny Databases (Physalia)](https://www.usenix.org/conference/nsdi20/presentation/brooker) - cell design and blast radius containment in the EBS control plane.
- [Maglev: A Fast and Reliable Software Network Load Balancer](https://research.google/pubs/maglev-a-fast-and-reliable-software-network-load-balancer/) - consistent-hash lookup tables and connection handling.
- [Andromeda: Performance, Isolation, and Velocity at Scale in Cloud Network Virtualization](https://research.google/pubs/andromeda-performance-isolation-and-velocity-at-scale-in-cloud-network-virtualization/) - the Hoverboard mechanism.
- [The Tail at Scale](https://research.google/pubs/the-tail-at-scale/) - hedging and tail-latency arithmetic.
- [Metastable Failures in Distributed Systems](https://sigops.org/s/conferences/hotos/2021/papers/hotos21-s11-bronson.pdf) - the vocabulary for retry-storm and cold-cache outages.
- [Gray Failure: The Achilles' Heel of Cloud-Scale Systems](https://www.microsoft.com/en-us/research/publication/gray-failure-achilles-heel-cloud-scale-systems/) - why health checks and user experience disagree.
- [ZeRO: Memory Optimizations Toward Training Trillion Parameter Models](https://arxiv.org/abs/1910.02054) - the 16-bytes-per-parameter accounting and sharding stages.
- [TPU v4: An Optically Reconfigurable Supercomputer](https://arxiv.org/abs/2304.01433) - OCS and torus slicing.
- [Cloud TPU documentation](https://docs.cloud.google.com/tpu/docs) - the live source for TPU specs and release status.
- [Amazon Builders' Library](https://aws.amazon.com/builders-library/) - static stability, shuffle sharding, and constant work essays.
- [Efficient Memory Management for Large Language Model Serving with PagedAttention](https://arxiv.org/abs/2309.06180) - the KV block-table design.

---

## 13. Where each domain is taught

| Domain | Where it is taught |
| --- | --- |
| Virtualization and isolation | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_01_Hypervisors_MicroVMs_and_Sandboxing\|Hypervisors, MicroVMs, and Sandboxing]], [[17-Cloud-AI-Engineering/Volume_02_AWS_Deep_Dive/Chapter_04_EC2_Nitro_Architecture_and_Graviton\|Nitro]], [[17-Cloud-AI-Engineering/Volume_03_GCP_Deep_Dive/Chapter_06_Compute_Engine_and_Titanium_Architecture\|Titanium]] |
| Networking and fabrics | [[17-Cloud-AI-Engineering/Volume_00_Foundations_Under_the_Cloud/Chapter_02_Datacenter_Networking_Fabrics\|Fabrics]], [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_13_Jupiter_Datacenter_Network\|Jupiter]], [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_12_Andromeda_Network_Virtualization\|Andromeda]] |
| Storage and consensus | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_06_Google_Spanner\|Spanner]], [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_02_Amazon_Aurora\|Aurora]], [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_09_GFS_and_Colossus\|GFS and Colossus]] |
| Reliability | [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/Chapter_03_Resilient_Communication_Retries_Jitter_and_Circuit_Breakers\|Retries, Jitter, and Circuit Breakers]], [[17-Cloud-AI-Engineering/Volume_01_Cloud_Architecture_Principles/Chapter_05_Constant_Work_and_Workload_Smoothing\|Constant Work]] |
| Accelerators and training | [[17-Cloud-AI-Engineering/Volume_13_Seminal_Papers_and_Case_Studies/Chapter_15_Google_TPU_Architecture_Evolution\|TPU Architecture Evolution]], [[17-Cloud-AI-Engineering/Volume_10_AI_Infrastructure_on_Cloud/Chapter_05_Memory_Optimization_ZeRO_FSDP_and_JAX_Sharding\|ZeRO, FSDP, and JAX Sharding]] |
| Serving and cost | [[17-Cloud-AI-Engineering/Volume_11_LLM_Serving_and_Inference_Systems_on_Cloud/Chapter_11_End_to_End_Latency_Throughput_and_Cost_Modeling\|End-to-End Latency, Throughput, and Cost Modeling]], [[17-Cloud-AI-Engineering/Volume_09_FinOps_and_Cloud_Economics/Chapter_05_GPU_and_TPU_Unit_Economics\|GPU and TPU Unit Economics]] |
| Next appendix | [[17-Cloud-AI-Engineering/Appendices/Appendix_B_AWS_to_GCP_Mechanism_Translation_Map\|Appendix B: AWS to GCP Mechanism Translation Map]] |

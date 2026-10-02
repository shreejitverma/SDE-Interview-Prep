---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1016/0167-8191(96)00021-X"]
course: cs6210
lesson: optional
reading: optional
venue: "Parallel Computing 1996"
authors: ["Umakishore Ramachandran", "Gautam Shah", "S. Ravikumar", "Jeyakumar Muthukumarasamy"]
tags: [cs6210, cs6210/paper]
aliases: ["Scalability Study of the KSR-1"]
---

# Scalability Study of the KSR-1

Parallel Computing 1996. Reading status: optional. [Link](https://doi.org/10.1016/0167-8191(96)00021-X).

> [!abstract] One-line summary
> A scalability study of the KSR-1 COMA shared-memory multiprocessor evaluates its pipelined ring interconnect and cache coherence using microbenchmarks and NAS parallel benchmarks.

## Problem

The scalability of shared-memory parallel architectures is a major concern in the architectural community due to the potential for large latencies during remote memory accesses as the system size grows.
Understanding how specific architectural features interact with the data access patterns of algorithms is necessary to determine if shared-memory multiprocessors can scale effectively, especially when compared to message-passing systems.

## Key idea

The performance and scalability of the KSR-1, a cache-only memory architecture (COMA) based on a hierarchy of pipelined unidirectional rings, is comprehensively evaluated through low-level latency measurements, synchronization primitive implementations, and parallel algorithm kernels.
The study demonstrates that the pipelined ring provides multiple communication paths, which can be leveraged to overlap communication and significantly improve the performance of synchronization mechanisms and parallel applications with regular access patterns.

## Design

The KSR-1 uses a 64-bit COMA model with an invalidation-based cache coherence protocol, where data migrates dynamically to the requesting node's local cache.
The interconnect relies on a unidirectional slotted pipelined ring.
The architecture provides hardware mechanisms like `get sub page` for exclusive locks, along with prefetching and poststore instructions.
The study measures latencies for sub-cache, local-cache, and remote network accesses.
Various barrier synchronization algorithms, including counter, tree, dissemination, tournament, and MCS, are implemented to test the capabilities of the interconnect.
Furthermore, NAS benchmark kernels such as Embarrassingly Parallel, Conjugate Gradient, and Integer Sort are ported and analyzed to assess algorithmic scalability.

## Evaluation

Experiments were conducted on a 32-node KSR-1 and a 64-node KSR-2 system.
Sub-cache latency was confirmed at 2 cycles, while local-cache latency was approximately 18 cycles, and remote network latency was about 175 cycles.
The tournament barrier algorithm with a global wakeup flag performed best, demonstrating excellent scalability up to 64 nodes by exploiting the parallel communication paths of the pipelined ring.
For the Conjugate Gradient kernel, the system achieved a speedup of 22.7 on 32 processors with an efficiency of 71%.
While the Embarrassingly Parallel kernel exhibited linear speedup, the Integer Sort kernel failed to scale and saturated the ring interconnect on 32 processors.

## Limitations and critiques

The KSR-1's hardware exclusive lock serializes all lock requests, making it highly inefficient for workloads that exhibit significant read-sharing.
The network interconnect can saturate when all processors simultaneously attempt to access remote data, as demonstrated by the Integer Sort kernel.
Additionally, the size of the caches at each node may be too small for the efficient implementation of large data structures, leading to excessive cache misses and network traffic.
Finally, false sharing severely degraded the performance of the MCS barrier algorithm on this specific architecture.

## What it led to

The study provided empirical evidence that shared-memory architectures with pipelined interconnects can scale well for applications exhibiting regular data access patterns.
It also highlighted the critical importance of tailoring software-level synchronization algorithms to the specific characteristics of the underlying hardware interconnect, demonstrating that algorithms designed for bus-based systems do not necessarily perform best on ring-based systems.

## Exam angles

<details>
<summary>Why did the tournament barrier algorithm outperform the MCS barrier algorithm on the KSR-1 architecture?</summary>
The tournament barrier leverages the multiple communication paths of the KSR-1's pipelined ring by allowing communication at each level of the binary tree to proceed in parallel.
The MCS algorithm uses a 4-ary tree where communication from children to parent is strictly sequential, failing to exploit the available network parallelism.
Furthermore, the MCS algorithm suffered significantly more from false sharing on the KSR-1.
</details>

<details>
<summary>How does the KSR-1's hardware exclusive lock behave under workloads with high read-sharing, and what alternative is proposed?</summary>
The hardware `get sub page` instruction provides an exclusive lock and serializes all requests, regardless of whether they are for reading or writing.
This is inefficient for read-heavy workloads.
The authors propose using a software-based read-write lock that combines consecutive read requests, allowing concurrent readers to share the lock and significantly improving performance.
</details>

<details>
<summary>According to the KSR-1 scalability study, what architectural features most limited the scalability of the Integer Sort (IS) kernel?</summary>
The scalability of the IS kernel was primarily limited by the saturation of the ring interconnect.
This occurred because the algorithm requires all processors to perform simultaneous remote memory accesses, overwhelming the communication network's capacity on a fully populated 32-node ring.
</details>

## Related

- Lessons: not covered in lectures (optional reading)

---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/103727.103729"]
course: cs6210
lesson: L04
reading: required
venue: "TOCS 1991"
authors: ["John M. Mellor-Crummey", "Michael L. Scott"]
tags: [cs6210, cs6210/paper]
aliases: ["Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors"]
---

# Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors

TOCS 1991. Reading status: required. [Link](https://doi.org/10.1145/103727.103729).

> [!abstract] One-line summary
> Introduces scalable spin locks and barriers that generate O(1) remote references per lock acquisition by spinning on locally-accessible variables.

## Problem

Typical busy-wait synchronization algorithms, such as test-and-set and ticket locks, suffer from significant memory and interconnect contention when many processors attempt to acquire the lock.
This creates hot spots that degrade overall system performance, especially as applications and multiprocessors scale.

## Key idea

The core insight is to eliminate contention by ensuring that every processor spins only on a separate, locally-accessible flag variable.
A processor holding the lock explicitly hands it off by performing a single remote write to the next waiting processor's local variable, achieving O(1) network traffic per acquisition regardless of the number of contending processors.
This completely removes the central hot spot that cripples simpler lock implementations on large-scale machines.

## Design

The MCS lock uses a linked list of queue nodes where each node corresponds to a waiting processor.
Each processor allocates a queue node locally and uses an atomic fetch-and-store operation to append itself to the global queue.
A processor spins on a locked flag inside its own local queue node.
When releasing the lock, a processor checks its successor pointer and, if non-null, flips the successor's locked flag to false.
The authors also propose a scalable software combining tree barrier where nodes are distributed across memory modules to prevent hot-spot contention during phase coordination.

## Evaluation

Evaluated on a Sequent Symmetry and a BBN Butterfly multiprocessor.
On the Sequent, which features a coherent cache, the MCS lock's overhead remains flat as the number of processors increases, significantly outperforming test-and-set with exponential backoff and ticket locks.
On the Butterfly, which uses distributed shared memory without coherent caches, the MCS lock vastly reduces network transactions compared to alternative algorithms.

## Limitations and critiques

The MCS lock requires more complex queue management, often utilizing a compare-and-swap instruction for safe removal, and has a slightly higher base latency in the uncontended case compared to a simple test-and-set lock.
The strict FIFO ordering can sometimes conflict with OS scheduling if a lock holder is preempted while in the queue.

## What it led to

MCS locks became a foundational synchronization primitive, widely used in modern operating system kernels and parallel libraries.
The principles of local spinning heavily influenced subsequent synchronization research, including queue-based locking algorithms and hierarchical locks for non-uniform memory access systems.

## Exam angles

> [!question]- Why does a test-and-set lock scale poorly on a cache-coherent multiprocessor?
> The test-and-set lock creates a hot spot on a single shared variable.
> Every lock release invalidates the cached copies for all spinning processors, causing a storm of cache misses and interconnect traffic when they simultaneously attempt to re-acquire the lock.

> [!question]- Explain how the MCS lock guarantees O(1) network transactions per lock acquisition.
> Each processor spins on a distinct, locally-accessible variable rather than a global flag.
> The only network transactions occur when a processor joins the queue via an atomic swap and when the previous lock holder writes to the local flag of its successor to pass the lock.

> [!question]- What is the specific role of the fetch-and-store instruction in the MCS lock implementation?
> It is used by a processor to atomically place itself at the tail of the global queue.
> The operation returns the previous tail, which the processor then uses to update the predecessor's next pointer.

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)

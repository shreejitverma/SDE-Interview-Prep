---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L04f]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-09-scalable-structures: Scalable kernel-style structures: per-CPU counters, read-mostly data, and false sharing

> [!info] Goal
> Demonstrate the performance implications of shared state in multiprocessor systems, focusing on false sharing, locking overhead, and read-mostly data structures.

This lab provides concrete examples for the concepts discussed in L04f (Shared Memory Multiprocessor OS), particularly the architectural lessons from Tornado and Corey regarding localized state and existence guarantees.

## Prerequisites

- Lima VM `aos` running (see [setup](../setup/README.md)).
- A basic understanding of POSIX threads, atomics, and cache lines.

## Run

Build and run the lab:

```bash
make
./scalable_counters
./false_sharing
./read_mostly
```

Alternatively, run the test suite inside the VM:

```bash
../setup/run-in-vm.sh lab-09-scalable-structures test
```

## What you should see

When running the benchmarks, you will see output similar to this (captured on an 8-vCPU arm64 VM):

```text
Running Scalable Counters Test (4 threads, 10000000 iterations)...
Locked Counter      : 1.1728 seconds
Atomic Counter      : 0.1952 seconds
Per-Thread Counter  : 0.0040 seconds

Final counts (should be 40000000):
Locked : 40000000
Atomic : 40000000
Per-Thr: 40000000
Running False Sharing Test (4 threads, 100000000 iterations)...
Unpadded (False Sh.): 2.2079 seconds
Padded (No False S.): 1.6082 seconds
Running Read-Mostly Test (4 readers, 1 writers)...
RWLock              : 0.3359 seconds
RCU-like            : 0.1458 seconds
```

The results highlight three distinct lessons:
1. **Scalable counters**: A per-thread (per-CPU) counter is orders of magnitude faster than a locked or even an atomic counter because it requires zero inter-processor communication.
2. **False sharing**: Padding elements to cache-line boundaries (64 bytes) prevents independent threads from fighting over cache-line ownership, yielding a measurable speedup.
3. **Read-mostly data**: An epoch-based RCU-like reader avoids modifying shared state (like taking an rwlock), allowing readers to execute much faster.

## How it works

- `scalable_counters.c`: Compares three synchronization strategies for a simple counter.
  - **Locked**: Uses a `pthread_mutex_t` to protect a single global counter, introducing massive contention.
  - **Atomic**: Uses `atomic_fetch_add_explicit`, which avoids blocking but still forces the hardware to ping-pong the cache line containing the counter between cores.
  - **Per-Thread**: Allocates an array of padded counters, one for each thread. Threads increment only their local counter, completely eliminating contention. The total is summed at the end.
- `false_sharing.c`: Allocates an array of counters. The "unpadded" version packs counters adjacently in memory, meaning counters for thread 0 and thread 1 likely share the same 64-byte hardware cache line. The "padded" version forces each counter onto its own cache line, preventing invalidations.
- `read_mostly.c`: Implements a simplified epoch-based existence guarantee (conceptually similar to Linux RCU or Tornado's object translation tables). Readers register their presence locally, read a pointer atomically, and dereference it. Writers allocate a new structure, perform an atomic pointer swap, and then wait for all active readers to clear the previous epoch before freeing the old structure.

## Experiments

- **Experiment 1**: Increase the number of threads in `scalable_counters.c` to match or exceed the number of vCPUs in your VM.
  - *Prediction*: The locked and atomic implementations will degrade significantly as contention scales, while the per-thread counter will remain virtually flat in time per iteration.
- **Experiment 2**: In `read_mostly.c`, change the ratio of readers to writers (e.g., 1 reader and 4 writers).
  - *Prediction*: The RCU-like implementation thrives on read-heavy workloads. With many writers, the overhead of allocating new structures and waiting for grace periods will make it slower than the rwlock.
- **Experiment 3**: Run the programs under `perf stat -e cache-misses,cache-references ./false_sharing`.
  - *Prediction*: The unpadded test will report a significantly higher number of cache misses due to coherence protocol invalidations compared to the padded test.

## Questions

<details>
<summary>Why is the atomic counter slower than the per-thread counter if both avoid OS-level blocking?</summary>
The atomic counter forces all threads to update the same memory address.
This requires the underlying hardware cache coherence protocol (e.g., MESI) to constantly invalidate the cache line on other processors and transfer ownership to the modifying processor.
The per-thread counter keeps modifications strictly localized to each processor's L1 cache.
</details>

<details>
<summary>How does the padding in `false_sharing.c` map to real OS design?</summary>
In modern kernels like Linux, per-CPU data structures (such as network interface packet counters or scheduling queues) are explicitly aligned to `L1_CACHE_BYTES`.
This ensures that a core modifying its local statistics never invalidates the cache line containing another core's statistics.
</details>

<details>
<summary>What is the downside of the RCU-like approach used in `read_mostly.c`?</summary>
The primary downsides are memory overhead and writer latency.
Writers must allocate new copies of the data structure and wait for a grace period before freeing the old memory.
If updates are frequent, this causes a high rate of memory allocation and can delay writers substantially.
</details>

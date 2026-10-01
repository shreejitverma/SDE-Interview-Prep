---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L07b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-15-dsm: User-level DSM with mprotect and SIGSEGV, twins and diffs

> [!info] Goal
> Demonstrate the mechanisms of a page-based Software Distributed Shared Memory (DSM) system, specifically focusing on Lazy Release Consistency (LRC) and the multiple-writer protocol using twins and diffs to mitigate false sharing.

## Prerequisites and Concepts

This lab exercises the following concepts from [L07b-Distributed-Shared-Memory](../../Part-4-Distributed-Subsystems-and-Recovery/L07b-Distributed-Shared-Memory.md):
- **L07b-06:** Eager versus lazy release consistency.
- **L07b-07:** Software DSM: page granularity and address space partitioning.
- **L07b-08:** Multiple-writer coherence protocol.
- **L07b-09:** Twins and diffs.

No honor-code guard is necessary; this lab implements a simplified DSM simulator and does not map to any graded project.

## Run Commands

Run this lab inside the `aos` Lima VM.
See [setup](../setup/README.md) for VM instructions.

```bash
# From the lab directory:
make clean all run
```

To run the automated tests:
```bash
make test
```

## What You Should See

The output shows two nodes (simulated as processes using `fork()`) interacting with a shared memory page.

```text
[Node B] Initial state: offset 10 = 0, offset 500 = 0
[Node B] Writing 42 to offset 500...
[Node B] SIGSEGV trapped at offset 500. Creating twin and unprotecting page.
[Node B] Computed diff in 2125 ns. 1 modifications found.
[Node A] Initial state: offset 10 = 0, offset 500 = 0
[Node A] Writing 99 to offset 10...
[Node A] SIGSEGV trapped at offset 10. Creating twin and unprotecting page.
[Node A] Computed diff in 2417 ns. 1 modifications found.
[Node A] LRC pull: Received diff with 1 modifications. Applying to local page.
[Node B] LRC pull: Received diff with 1 modifications. Applying to local page.
[Node B] Final state: offset 10 = 99, offset 500 = 42
[Node A] Final state: offset 10 = 99, offset 500 = 42
```

Both nodes successfully write to the same logical page concurrently without overwriting each other's changes.
The differences are merged correctly, yielding the final coherent state (`offset 10 = 99, offset 500 = 42`).
Performance timings, such as `2417 ns`, use `clock_gettime(CLOCK_MONOTONIC)` and illustrate the computational overhead of scanning a 4KB page for modifications.
Note that the `aos` VM has only one NUMA node. In a true multi-node cluster or multi-socket NUMA system, page fault timings and diff transmission over the network (or cross-socket interconnects) would be substantially higher due to the hardware latency. These local measurements capture solely the software overhead of the multiple-writer protocol.

## How It Works

The program uses POSIX system calls to simulate the runtime of a page-based Software DSM like TreadMarks:

1. **Initialization:** The system allocates an anonymous private memory page using `mmap()` protected with `PROT_READ`. It also registers a `SIGSEGV` handler for memory faults.
2. **Twins and Write Faults:** When a node attempts to modify the page, the OS traps the write and invokes the `SIGSEGV` handler. The handler copies the unmodified page to a "twin" buffer and calls `mprotect(..., PROT_READ | PROT_WRITE)` to allow the write to proceed.
3. **Lazy Release Consistency (LRC):** Modifications are not broadcast immediately. Instead, updates are deferred until a synchronization point (`dsm_release()`).
4. **Diff Generation:** At release, the DSM compares the modified page against the pristine twin byte-by-byte. It builds a packet of `DiffEntry` structures (offset and value) and measures the execution time using `clock_gettime()`.
5. **Multiple-Writer Protocol:** Because updates are exchanged only as diffs rather than full pages, Node A and Node B can write to different offsets on the same page concurrently. False sharing is eliminated because the diffs merge cleanly during the `dsm_acquire()` phase.

## Experiments

- **Experiment 1:** Modify the code so that Node A and Node B write to the exact same offset (e.g., both write to offset 10). *Prediction: What will the final value be, and does it depend on the order of `dsm_acquire()`?*
- **Experiment 2:** Increase the size of the shared memory region to a large multi-page buffer (e.g., 1MB) and write to a single byte. *Prediction: How will the `SIGSEGV` trap behavior change, and how long will the diff computation take compared to the 4KB baseline?*
- **Experiment 3:** Comment out the `mprotect` call inside the `SIGSEGV` handler. *Prediction: What will happen when the program runs?*

## Questions

> [!question]- Why do we use `mprotect()` and a `SIGSEGV` handler instead of just checking for writes manually?
> Intercepting writes manually would require instrumenting every single memory access in the application, which is a massive performance penalty. Using virtual memory hardware and the OS fault handler incurs zero overhead for reads and groups the write overhead strictly to the first fault per page.

> [!question]- How does the diff creation prevent false sharing ping-pongs?
> In a single-writer protocol, if two nodes write to different variables on the same page, the page ownership bounces constantly over the network (ping-ponging). By using diffs, both nodes can retain write access to their own copies. The DSM only exchanges the precise modifications, which are merged without invalidating the entire page.

> [!question]- Why does the `dsm_acquire()` function call `mprotect()` to briefly make the page writable before applying diffs?
> Because the page is normally kept read-only to catch the first write fault from the user application. When the DSM runtime itself needs to apply incoming diffs from the network, it must unprotect the page, apply the changes, and then re-protect the page to ensure future user writes are trapped correctly.

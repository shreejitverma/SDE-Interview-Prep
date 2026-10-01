---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: []
course: cs6210
lessons: [L04d, L05c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-07-rpc-costs: Where RPC time goes: copies, crossings, and zero-copy on one machine

> [!info] Goal
> Make L04d, L05c concrete with real commands and measurements.

> [!warning] Honor code guard
> This lab deliberately does not implement a course project: no gRPC store or vendor service (Project 3).

See [setup](../setup/README.md) for the VM.

## Prerequisites
- The `aos` VM configured and running.
- Basic understanding of Linux system calls (`read`, `write`, `sendfile`, `futex`).
- Familiarity with the concepts of context switches, copies across the user-kernel boundary, and zero-copy mechanisms.

## Run Commands
Run this lab natively in the VM or use the wrapper script from the host:
```bash
# From the repository root on the host:
AOS/labs/setup/run-in-vm.sh AOS/labs/lab-07-rpc-costs test
```

Or from inside the VM:
```bash
make clean all
make test
```

## What you should see
When you run the tests, you will see a comparison of median round-trip times (RTT) across different Inter-Process Communication (IPC) mechanisms. A typical run (from `expected-output.txt`) yields:

```text
=== IPC Ping-Pong Benchmarks ===
pipe 64 bytes: median RTT = 14500 ns
unix 64 bytes: median RTT = 12125 ns
shm 64 bytes: median RTT = 19500 ns
pipe 4096 bytes: median RTT = 14792 ns
unix 4096 bytes: median RTT = 15917 ns
shm 4096 bytes: median RTT = 11542 ns
```

You will also see system call counts profiled by `strace -c`. For a pipe ping-pong, you will see precisely 20,001 `read` and `write` calls. For shared memory, you will see no `read` or `write` calls for the payload, but thousands of `futex` calls used for synchronization.

Interestingly, for the 100MB file transfer benchmark, `sendfile` may perform slower than standard `read`/`write` in this specific virtualized environment (virtiofs over Lima/KVM). For example:
```text
=== Sendfile vs Read/Write (100MB) ===
rw 100MB: 483 ms
sendfile 100MB: 872 ms
```

## How it works
This lab features two C programs that explore local RPC overheads.

1. **`ipc_bench.c`**: Implements a simple ping-pong protocol between a parent and a child process. It supports three modes:
   - **`pipe`**: Two unidirectional pipes (one for each direction).
   - **`unix`**: A bidirectional UNIX domain socket created with `socketpair`.
   - **`shm`**: An anonymous shared memory mapping (`mmap` with `MAP_SHARED`). Synchronization is achieved using C11 atomic integers and the Linux `futex` system call.

2. **`sendfile_bench.c`**: Copies a 100MB file into a UNIX domain socket. It compares:
   - **`rw`**: A loop of `pread` into a userspace buffer followed by `write` to the socket.
   - **`sendfile`**: The Linux `sendfile` system call, which orchestrates the transfer entirely within the kernel, avoiding the copy to userspace.

The benchmarks use `clock_gettime(CLOCK_MONOTONIC)` to measure durations and report the median RTT. The single NUMA node environment of the VM ensures that cross-node memory access latencies do not skew the shared memory results.

## Experiments to try

1. **Payload scaling**
   * **Prompt**: What happens to the RTT of pipes versus shared memory as the message size grows to 64KB?
   * **Action**: Modify `test.sh` to run `./ipc_bench pipe 65536` and `./ipc_bench shm 65536`.
   * **Expectation**: Pipe and UNIX socket RTTs will grow significantly because the kernel must copy the data in and out of kernel space on every transfer. Shared memory RTT will remain relatively flat since the only overhead is the futex wake/wait, while the memory copy happens directly in userspace.

2. **Buffer size tuning**
   * **Prompt**: How does the userspace buffer size affect the `rw` mode in `sendfile_bench`?
   * **Action**: Change `CHUNK_SIZE` in `sendfile_bench.c` from 8192 to 1048576 (1MB). Recompile and run.
   * **Expectation**: The `rw` mode will require far fewer system calls, dropping the syscall overhead and significantly reducing the execution time.

3. **Strace overhead**
   * **Prompt**: How much overhead does `strace` add to `sendfile`?
   * **Action**: Observe the raw execution time of `sendfile_bench` without `strace` (e.g., 872 ms), then check the wall-clock time reported by `strace -c` (e.g., 2763 ms).
   * **Expectation**: `strace` intercepts every system call using `ptrace`, which forces a context switch and additional kernel processing. This drastically inflates execution times, particularly for `sendfile` and `read`/`write` loops.

## Questions

<details>
<summary>Why does the shared memory approach use <code>futex</code> instead of polling?</summary>
Polling (spin-waiting) on a shared memory flag consumes 100% of a CPU core, which is highly inefficient and can delay the peer process if they share a core. The <code>futex</code> system call allows a thread to sleep and yield the CPU until the specific memory address is modified, combining the low-latency of userspace atomic operations with the efficiency of kernel-managed sleeping.
</details>

<details>
<summary>Why does <code>sendfile</code> perform worse than <code>read`/`write</code> in our VM environment?</summary>
In a native Linux environment with a standard filesystem (e.g., ext4), <code>sendfile</code> is typically faster because it avoids copying data to userspace. However, our VM uses a shared virtiofs mount. The interaction between the guest page cache, the host filesystem, and virtiofs can cause <code>sendfile</code> to fall back to less optimal paths or trigger excessive fine-grained operations under the hood, making standard buffered reads and writes faster.
</details>

<details>
<summary>Where do the copies occur in the pipe/UNIX socket benchmarks?</summary>
There are two copies per message transfer. When the sender calls <code>write</code>, the payload is copied from the sender's userspace buffer into a kernel buffer. When the receiver calls <code>read</code>, the data is copied from the kernel buffer into the receiver's userspace buffer. For a full round-trip, this means four copies in total.
</details>

<details>
<summary>How does a single NUMA node architecture simplify our shared memory benchmark?</summary>
In a multi-node Non-Uniform Memory Access (NUMA) system, memory is physically closer to some CPU cores than others. If the parent and child were pinned to different NUMA nodes, accessing the shared memory region would incur different latencies depending on which node the physical memory was allocated on. A single NUMA node ensures memory access latencies are uniform for both processes.
</details>

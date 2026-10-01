---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L01, L02a, L02d]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-01-syscall-and-context-switch: Border crossings: syscall, context switch, and address space switch costs

> [!info] Goal
> Make L01, L02a, L02d concrete with real commands and measurements.

> [!warning] Honor Code
> This lab explores general operating system concepts and measurement techniques.
> It does not involve any protected project code or assignments from CS 6210.

## Prerequisites

See [setup](../setup/README.md) for the VM.
You will need a working C compiler, `make`, and the `perf` tool installed in the VM.

## Run Commands

To build and run the benchmarks inside the VM:
```sh
labs/setup/run-in-vm.sh labs/lab-01-syscall-and-context-switch test
```

## What You Should See

The output will display the cost of a simple system call and the cost of context switching between threads and processes.
Syscall cost for `SYS_getpid` and the `libc` wrapper should be around 110-130 nanoseconds per call.
Both thread and process context switches should measure around 30,000 to 50,000 nanoseconds per switch in this nested virtual machine.

The `perf bench sched pipe` test will report around 70 to 135 microseconds per operation, which aligns with two context switches per ping-pong operation.
Note that nested virtualization overhead dominates these numbers compared to bare-metal hardware.

## How It Works

A system call involves an architectural mode switch from unprivileged user space to privileged kernel space.
We measure the cost by timing millions of `getpid()` calls and averaging the result using `clock_gettime(CLOCK_MONOTONIC)`.
The `libc` wrapper for `getpid()` in modern glibc delegates to the actual syscall rather than caching the result, so both methods yield similar times.
A context switch requires saving and restoring register state and potentially flushing or switching memory translation structures.
We measure this by bouncing a single byte back and forth over a pipe.
The thread benchmark uses `pthread_create` to share the same address space.
The process benchmark uses `fork` to create two separate address spaces.
On ARM64, different address spaces use different Address Space IDs (ASIDs), which avoids a full TLB flush but still requires changing the base translation table register.

## Experiments to Try

1. **Vary the number of iterations.**
   Change `ITERS` in `syscall_cost.c` from 10,000,000 to 1,000,000.
   *Prediction:* The average cost per call should remain roughly constant, provided the warmup iterations mask any initial cache misses.
2. **Pin processes to the same core.**
   Modify the Makefile or use `taskset -c 0 ./context_switch` to run the context switch benchmark on a single virtual CPU.
   *Prediction:* Context switches may actually be faster due to better cache locality, or slower if they constantly preempt each other without yielding.
3. **Compare with bare-metal.**
   Run the exact same `make run` outside the VM on a bare-metal Linux host.
   *Prediction:* Bare-metal context switches will be an order of magnitude faster (e.g., 2-5 microseconds) because nested KVM exits add significant latency.

## Questions and Answers

<details>
<summary>Why does the process context switch cost more than the thread context switch?</summary>

Processes have separate address spaces.
Switching between processes requires the kernel to change the active memory translation structures.
Even with ASIDs on ARM64 or PCIDs on x86 to avoid full TLB flushes, changing the translation table base register adds overhead.
Threads share the same translation structures, skipping this step entirely.
</details>

<details>
<summary>How do these numbers compare to the Mach and L3 microkernel paper figures?</summary>

The L3 microkernel paper demonstrated that IPC and context switches could be extremely fast (on the order of a few microseconds) when carefully optimized.
Mach had much heavier IPC costs because of complex message structures and scheduling overhead.
Our numbers here are artificially high due to running inside a nested virtual machine.
On a modern bare-metal system, Linux context switches take single-digit microseconds, bringing monolithic kernels close to L3's optimized microkernel speeds.
</details>

<details>
<summary>Why do we use a pipe ping-pong to measure context switch time?</summary>

A pipe ping-pong forces a strict synchronous handoff between two execution contexts.
When process A reads from an empty pipe, it blocks, forcing the scheduler to switch to process B.
Process B writes a byte, unblocking A, and then immediately blocks trying to read from a second pipe.
This guarantees exactly two context switches per iteration, making the average calculation reliable.
</details>

<details>
<summary>Why does libc `getpid()` cost the same as raw `syscall(SYS_getpid)`?</summary>

Older versions of glibc cached the PID in user-space to avoid the syscall overhead entirely.
This cache caused subtle bugs when processes were cloned or forked in unexpected ways.
Modern glibc removed this cache, so calling `getpid()` unconditionally executes the underlying system call.
The minor difference in timing is just the function call overhead in the C library.
</details>

---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L04a]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-04-cache-coherence: Coherence traffic, false sharing, and memory-ordering litmus tests

> [!info] Goal
> Make L04a concrete with real commands and measurements.

> [!warning] Honor Code Guard
> This lab implements isolated memory and coherence test cases.
> It does not contain solutions to the barrier synchronization project or any other Georgia Tech CS 6210 assignments.

## Concepts
This lab explores cache coherence, false sharing, and memory ordering.
Specifically, it measures the overhead of snooping cache lines between processor cores and demonstrates how weak memory models reorder reads and writes.

## Prerequisites
- Start the `aos` Lima VM.
- Ensure you have C11-compatible GCC and Make installed.
- See [setup](../setup/README.md) for the VM configuration.

## Run Commands
To run the full suite of tests in the VM:
```bash
../setup/run-in-vm.sh labs/lab-04-cache-coherence run
```

To run the automated tests:
```bash
../setup/run-in-vm.sh labs/lab-04-cache-coherence test
```

## What You Should See
The tests output timing metrics and weak behavior counts.
On an Apple M3 Pro host via a Lima VM with 8 vCPUs, you should see results similar to this:

```
--- Running False Sharing ---
Starting false sharing test (4 threads, 20000000 iterations)...
Unpadded counters time: 63 ms
Padded counters time:   36 ms
Padded counters avoided false sharing successfully.
PASS: False sharing test complete.
--- Running Ping Pong ---
Starting ping-pong test (1000000 iterations)...
Total time: 175 ms
One-way coherence transfer latency: 87.67 ns
PASS: Ping-pong complete.
--- Running Litmus Tests ---
Starting memory ordering litmus tests (500000 iterations each)...
Store Buffering (relaxed): 96986 weak behaviors out of 500000
Store Buffering (seq_cst): 0 weak behaviors out of 500000
Message Passing (relaxed): 8 weak behaviors out of 500000
Message Passing (release/acquire): 0 weak behaviors out of 500000
PASS: Litmus tests complete.
```
Notice that unpadded counters are significantly slower due to cache line thrashing.
The coherence transfer latency demonstrates the time it takes for a cache line to move between cores.
The litmus tests on ARM64 reveal a substantial amount of weak behaviors (reordering) when using relaxed atomics, which are completely eliminated when strict memory ordering (`seq_cst` or `release`/`acquire`) is used.

## How it Works
The lab is divided into three distinct C programs.

`false_sharing.c` spawns multiple threads that increment independent counters.
In the unpadded version, these counters are adjacent in memory and map to the same 64-byte cache line.
When one thread modifies its counter, the hardware cache coherence protocol invalidates the entire cache line in other cores, causing "false sharing."
The padded version spaces the counters 64 bytes apart, allowing each core to keep its line in the Modified state without interference.

`ping_pong.c` uses a volatile flag to bounce execution between two threads.
One thread waits for the flag to become 0, then sets it to 1, while the other does the reverse.
This forces the cache line containing the flag to transition rapidly between the L1 caches of the two executing cores.
By dividing the total time by the number of iterations, we calculate the one-way coherence transfer latency.

`litmus.c` implements two classic memory ordering scenarios using C11 atomics.
The Store Buffering test checks if two threads writing to independent variables and reading the other's variable can both observe the old values.
The Message Passing test checks if a thread can observe a flag indicating data is ready but still read stale data.
ARM architectures permit these reorderings at the hardware level, so relaxed atomics will exhibit weak behavior.

## Experiments

1. **Vary thread count in false sharing**
   - *Prompt*: If you change `NUM_THREADS` in `false_sharing.c` from 4 to 8, what happens to the execution time of the unpadded version?
   - *Prediction*: The unpadded execution time will increase non-linearly because more cores will contend for the single cache line, exponentially increasing invalidation traffic. The padded version will scale much better.

2. **CPU pinning**
   - *Prompt*: What happens to the ping-pong latency if both threads are pinned to the same vCPU using `taskset`?
   - *Prediction*: The latency will increase dramatically because the threads must context switch to make progress, rather than passing a cache line over the interconnect between parallel executing cores.

3. **Compiler optimizations on litmus tests**
   - *Prompt*: If you remove `volatile` or the atomic load/store intrinsics in `litmus.c` and use plain assignments with `-O2`, will you see more or fewer weak behaviors?
   - *Prediction*: The compiler might optimize the loops into registers or reorder the instructions entirely, leading to unpredictable results or infinite loops where one thread never sees the other's writes.

4. **Padding size variation**
   - *Prompt*: What happens if you change `CACHE_LINE_SIZE` in `false_sharing.c` to 32 bytes on an ARM64 system?
   - *Prediction*: ARM64 typically uses 64-byte cache lines. If you pad to 32 bytes, two adjacent counters will still share the same 64-byte line, and false sharing overhead will return.

## Questions

<details>
<summary>Why does the Store Buffering test show so many more weak behaviors than Message Passing?</summary>
Store buffering occurs because CPU cores use write buffers to defer stores while executing subsequent reads.
Since the writes and reads in the store buffering test involve different memory locations, the hardware aggressively allows the reads to bypass the pending writes.
Message passing requires a specific sequence of load/store timings to hit the race condition where the data write is delayed relative to the flag write, which is less frequently exposed by the microarchitecture.
</details>

<details>
<summary>How does the hardware coherence protocol handle the ping-pong test?</summary>
The ping-pong test forces the cache line containing the flag into a Modified state in one core.
When the other core attempts to read it, it issues a read request on the interconnect, causing the first core to flush the line and transition its state (e.g., to Shared or Invalid).
When the second core writes to it, it sends an invalidate signal.
This continuous state transition (M -> S/I -> M) generates heavy interconnect traffic.
</details>

<details>
<summary>Why must we use medians or multiple iterations rather than a single timing run?</summary>
In a virtualized environment, execution can be interrupted by the hypervisor, OS scheduler, or interrupts.
A single run might include context switch overhead, making it seem like a cache transfer took milliseconds instead of nanoseconds.
Averaging over millions of iterations amortizes this noise.
</details>

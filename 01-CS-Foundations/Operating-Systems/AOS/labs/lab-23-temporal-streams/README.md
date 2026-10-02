---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L10b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-23-temporal-streams: A time-indexed stream store and clock synchronization

> [!info] Goal
> Make L10b concrete with real commands and measurements.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Ubuntu 24.04 arm64 VM.
- Python 3.12 (via `/opt/aos-venv/bin/python`).
- GCC and Make.

## Running the Lab

Use the provided scripts to build and run the demos.

```bash
cd 01-CS-Foundations/Operating-Systems/AOS/labs/lab-23-temporal-streams
make all
```

You should see output similar to this, matching the contents of `expected-output.txt`:

```text
--- clock_gettime(CLOCK_MONOTONIC) overhead ---
Iterations: 10000000
Total time: 140239328 ns
Average overhead: 14 ns per call
```

## How It Works

### Time-Indexed Stream Store
The `stream_store.py` script implements the Persistent Temporal Streams (PTS) model. Rather than reading from a socket byte-by-byte, producers `put(item, timestamp)` and consumers `get(timestamp)`. The store saves items in JSON format, demonstrating simple persistence. A built-in garbage collector (`gc()`) runs to drop items whose timestamps are older than the persistence window, keeping memory and disk utilization bound.

### Clock Synchronization Simulation
The `clock_sync.py` script contrasts feed-forward and feedback synchronization for a virtual clock. 
- A simulated **Hardware Clock** runs 10% slower than the true physical time. 
- The **Feedback Clock** uses a proportional controller to adjust the local tick frequency based on errors.
- The **Feedforward Clock** avoids changing the underlying hardware tick rate entirely; instead, it updates an offset and multiplier during synchronization events, yielding accurate time conversions directly. 

### Clock Overhead Measurement
The `measure_clock.c` program tests the latency of `clock_gettime(CLOCK_MONOTONIC)` to highlight how commodity operating systems keep clock access extremely fast. The ~14 ns latency demonstrates the impact of the vDSO (Virtual Dynamically Linked Shared Object), which lets user-space read the clock directly without paying the overhead of a system call context switch.

## Experiments to Try

> [!question] Experiment 1: Alter the Feedback Clock Controller
> Change the proportional gain in `clock_sync.py` from `0.05` to `0.5`. What happens to the feedback clock after a synchronization event? Does it overshoot or correct faster?

> [!question] Experiment 2: Modify the Hardware Clock Drift
> Change the `frequency` parameter of `HardwareClock` in `clock_sync.py` to `1.20` (20% faster than true time). Observe how the Feedforward Clock adapts its multiplier compared to the Feedback Clock.

> [!question] Experiment 3: Measure System Call Overhead
> In `measure_clock.c`, replace `clock_gettime()` with a true system call like `getpid()`. Recompile and measure. What is the difference in average latency between a vDSO call and a true context switch?

> [!question] Experiment 4: Adjust the Garbage Collection Window
> In `stream_store.py`'s `run_demo()`, reduce the `gc_window` parameter to `10`. Run the script again. What happens to the items retrieved by `get(25)` after the GC pass?

## Questions

> [!faq]- Why is the feed-forward clock synchronization preferred in time-sensitive virtual machines?
> Feedback synchronization alters the virtual clock's frequency to steer it back to physical time. This causes time to speed up or slow down from the perspective of the application, disrupting scheduling and event timings. Feed-forward synchronization maps a stable, unaltered local clock reading to global time using a multiplier and offset, providing applications with an accurate, monotonically increasing time reference without frequency distortion.

> [!faq]- How does the PTS `get(time)` operation simplify application code compared to traditional sockets?
> Traditional sockets require the application to manually buffer incoming data, align timestamps across multiple streams, and discard old data. PTS moves these responsibilities into the middleware. By allowing consumers to ask for the exact frame or data point that was active at a precise timestamp (even retroactively), PTS removes the burden of temporal correlation and buffer management from the developer.

> [!faq]- Why is the vDSO so critical for clock synchronization performance?
> Retrieving the time frequently via standard system calls involves context switches, which can add hundreds or thousands of nanoseconds of overhead. This makes highly precise clock synchronization and profiling impossible. The vDSO maps a page of memory containing the kernel's current time state directly into user space, allowing applications to read the time in ~10-20 nanoseconds with just a few instructions.

> [!faq]- Why must the garbage collection window in a temporal stream store be strictly enforced?
> Continuous media applications ingest massive volumes of data (like multi-camera 4K video feeds). If items are not purged once they fall outside the useful situation awareness window, the system will exhaust memory and disk space rapidly. Explicit bounds keep resource utilization predictable and ensure system stability.

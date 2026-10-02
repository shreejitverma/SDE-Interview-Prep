---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: []
course: cs6210
lessons: [L10a]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-22-realtime: Timeliness on Linux: cyclictest, SCHED_FIFO and SCHED_DEADLINE, timers

> [!info] Goal
> Make L10a concrete with real commands and measurements. Explore timer accuracy and latency under idle and loaded conditions.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Read lesson L10a.
- The `rt-tests` package provides `cyclictest`.
- The `stress-ng` package provides artificial system load.

## Run

```sh
# Inside the VM
make run
```

## Expected Output

You should see output similar to this:
```text
=== System Baseline (Idle) ===
[Idle, SCHED_OTHER (cyclictest)]
T: 0 (12806) P: 0 I:1000 C:   1000 Min:     29 Act:  244 Avg:  458 Max:    1117

[Idle, SCHED_FIFO (cyclictest)]
# /dev/cpu_dma_latency set to 0us
T: 0 (12809) P:90 I:1000 C:   1000 Min:     61 Act:  499 Avg:  437 Max:   15305

=== System Under Load (stress-ng) ===
Starting stress-ng...
[Load, SCHED_OTHER (cyclictest)]
T: 0 (12822) P: 0 I:1000 C:   1000 Min:     54 Act:  202 Avg:  244 Max:     603

[Load, SCHED_FIFO (cyclictest)]
# /dev/cpu_dma_latency set to 0us
T: 0 (12825) P:90 I:1000 C:   1000 Min:     21 Act:  150 Avg:   84 Max:     277

=== Custom timers (overshoot in ns) ===
[Load, timers SCHED_OTHER]
One-shot  : min= 23630 ns, avg=203452 ns, median=168571 ns, max=2843737 ns
Periodic  : min= 26836 ns, avg=113454 ns, median=105379 ns, max=374691 ns

[Load, timers SCHED_FIFO]
One-shot  : min= 25643 ns, avg=311096 ns, median=360889 ns, max=4039609 ns
Periodic  : min= 23125 ns, avg=240587 ns, median=247848 ns, max=4656793 ns

[Load, timers SCHED_DEADLINE]
One-shot  : min= 27387 ns, avg=142130 ns, median= 75169 ns, max=5933985 ns
Periodic  : min= 25250 ns, avg=312460 ns, median=150295 ns, max=18973726 ns
```

## How it Works

The Linux kernel provides standard timekeeping and scheduling capabilities.
**`SCHED_OTHER`** is the default completely fair scheduler (CFS or EEVDF), which attempts to maximize overall throughput and fairness but provides no latency guarantees.
**`SCHED_FIFO`** is a real-time scheduling policy where tasks run until they yield, block, or are preempted by a higher-priority task.
**`SCHED_DEADLINE`** is a real-time scheduling policy based on the Earliest Deadline First (EDF) algorithm, allowing tasks to specify a runtime, deadline, and period so the kernel guarantees execution within these bounds.
For precise timing, `clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME)` provides one-shot delays, while `timerfd_create` and `timerfd_settime` provide periodic timer wakeups without drift.

In this lab, we measure timer overshoot (latency), which is the difference between the requested wakeup time and the actual wakeup time.
We use `cyclictest` for baseline measurements and our custom `timers` C program to measure custom scenarios, particularly `SCHED_DEADLINE`.
We run measurements in two environments: idle and under load using `stress-ng`.

Under load, `SCHED_OTHER` typically exhibits higher average and maximum latency compared to real-time policies.
`SCHED_FIFO` prevents user-space preemption by the stressor, reducing average latency significantly (e.g., from 244us to 84us in cyclictest).
`SCHED_DEADLINE` allows explicit reservations (e.g., 500us runtime every 1ms), offering strong median latencies compared to `SCHED_OTHER`.

Note that since the Lima VM does not expose hardware performance monitoring units (PMU) to guests, we rely on `clock_gettime(CLOCK_MONOTONIC)` which reads the ARM virtual counter efficiently in vDSO.
Therefore, absolute maximum latencies might occasionally spike due to host hypervisor scheduling.

## Experiments to Try

1. **Change the stressor**: Try `stress-ng --matrix 0` to heavily load the CPU without memory/IO mixing. Predict if `SCHED_FIFO` maintains the same latency.
2. **Over-commit SCHED_DEADLINE**: Modify `timers.c` to request a runtime equal to the period. The kernel should reject this with an error.
3. **Trace latency spikes**: Run `sudo cyclictest -b 1000 --tracemark` to trigger ftrace when latency exceeds 1000us. Can you find the offending kernel function?
4. **Compare NUMA nodes**: Although our VM has only one NUMA node, on a multi-node system, cross-node wakeups add latency. Use `numactl --cpunodebind=0` to pin memory and execution.

## Questions

<details>
<summary>Why does SCHED_FIFO still show some latency even when idle?</summary>
Even with real-time priority, the task must compete with kernel threads, interrupt handlers (top halves and softirqs), and hardware latencies. The VM environment also introduces hypervisor scheduling latency, where the host OS might preempt the entire vCPU to run host processes.
</details>

<details>
<summary>How does timerfd prevent drift in periodic tasks?</summary>
A simple periodic loop with relative nanosleep accumulates error every iteration because the time taken to execute the loop body and the scheduling latency shift the next start time. The `timerfd` interface sets wakeups relative to an absolute monotonic timeline, so if one wakeup is delayed, the next one is still scheduled at the correct absolute time, eliminating cumulative drift.
</details>

<details>
<summary>Why do we use CLOCK_MONOTONIC instead of CLOCK_REALTIME?</summary>
The `CLOCK_REALTIME` clock tracks wall-clock time and can jump forwards or backwards if the system time is synchronized via NTP or changed manually. The `CLOCK_MONOTONIC` clock strictly monotonically increases from an arbitrary point (usually boot time), ensuring accurate and reliable interval measurements unaffected by time adjustments.
</details>

<details>
<summary>What happens if a SCHED_DEADLINE task exceeds its specified runtime?</summary>
The kernel strictly enforces the runtime limit. If a `SCHED_DEADLINE` task uses its allocated runtime before the period ends, the scheduler throttles it and preempts it until the next period begins, ensuring isolation for other deadline tasks.
</details>

---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L04e]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-08-scheduling: Affinity scheduling: taskset, perf sched, chrt, cgroups, and a policy simulator

> [!info] Goal
> Make L04e concrete with real commands and measurements.

## Prerequisites
- Lima VM `aos` (Ubuntu 24.04 arm64). See [setup](../setup/README.md) for the VM.
- `make`, `gcc`, `perf`, `taskset`, `chrt`, `cgroups v2` (available in the VM).

## Run Commands
1. Enter the VM and navigate to the lab directory.
2. Compile: `make`
3. Run the tests: `make test`

## What you should see
When you run `make test`, you will see:
1. **Cache Affinity (taskset)**: Pinned execution vs unpinned. Pinned threads are locked to specific CPUs, while unpinned can migrate.
2. **Perf Sched**: Output of `perf sched latency` showing how long tasks spent waiting for CPU time. You should see average delay and max delay for the `affinity_workload` tasks.
3. **Chrt Policies**: Execution times using `SCHED_OTHER` (default Linux scheduler), `SCHED_FIFO`, and `SCHED_RR`. The real-time policies (`FIFO` and `RR`) typically finish their compute bursts very slightly faster due to avoiding time-slice preemption or sharing.
4. **Cgroups v2**: Two processes pinned to the same CPU but placed in different cgroups with weights 100 and 400. The process in the 400-weight cgroup should complete noticeably faster (e.g., in 0.08s compared to 0.12s) because it receives 80% of the CPU time while they compete.
5. **Policy Simulator**: A Python script simulating scheduling policies (FCFS, fixed, last processor, minimum intervening) showing migration counts based on cache-affinity scheduling principles.

## How it works
- `taskset`: Sets the CPU affinity mask of a process, restricting it to specific processors.
- `perf sched record/latency`: Traces scheduler tracepoints (`sched:sched_switch`, `sched:sched_wakeup`) to measure wait times in the runqueue.
- `chrt`: Changes the scheduling policy and priority of a process. `SCHED_FIFO` runs until completion or yielding, while `SCHED_RR` runs in time slices among equal-priority threads.
- `cgroups v2 cpu.weight`: Proportional share scheduling. A cgroup's share of CPU time is its weight divided by the total active weight of siblings.
- **Cache-affinity Scheduling (L04e)**: Thread migration between CPUs leads to cache misses. The simulator implements the Last Processor and Minimum Intervening policies which try to preserve cache footprint.

## Experiments to try
1. **Change cgroup weights**: Edit `test.sh` to change weights to 100 and 900.
   - *Prediction*: Does the high-weight process finish exactly 9x faster, or does overhead dampen the theoretical share?
2. **Increase workload data size**: Change `ARRAY_SIZE` in `affinity_workload.c` to exceed the L3 cache size.
   - *Prediction*: Does the difference between pinned and unpinned execution grow larger due to L3 cache misses upon migration?
3. **Compare FIFO and RR under contention**: Spawn 4 threads pinned to a single CPU using `chrt -f` vs `chrt -r`.
   - *Prediction*: Which policy completes all 4 tasks with the lowest average completion time?

## Questions
<details>
<summary>Why does the Minimum Intervening policy perform better than Last Processor under high load?</summary>
Last Processor only prefers the task if the CPU was precisely the one it last ran on, which might be busy. Minimum Intervening relaxes this by looking at how many *other* threads have run on any CPU since the target thread ran there, providing a proxy for how much of its cache footprint remains intact.
</details>

<details>
<summary>Why do we see migrations even when `taskset` is not used in an idle system?</summary>
The Linux CFS scheduler performs load balancing. Even on a mostly idle system, background kernel threads or IRQ handling might cause a momentary imbalance, prompting the scheduler to migrate a running thread to a completely idle core.
</details>

<details>
<summary>How does `cpu.weight` differ from `cpu.max` in cgroups v2?</summary>
`cpu.weight` defines a proportional share (work-conserving) which only restricts usage when there is contention. `cpu.max` imposes a hard cap on CPU time (e.g., 100ms every 500ms), enforcing limits even if CPUs are idle.
</details>

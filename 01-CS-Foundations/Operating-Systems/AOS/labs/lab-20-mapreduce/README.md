---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L09b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-20-mapreduce: MapReduce as a model: Unix pipelines and a master scheduling simulator

> [!info] Goal
> Make L09b concrete with real commands and measurements.

> [!warning] Honor code guard
> This lab deliberately does not implement a course project: no MapReduce framework with RPC workers, master, or file sharding (Project 4).

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Read [L09b MapReduce](../../Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md).
- Ensure the Lima VM is running.

## Run commands

Inside the VM, run the lab with:
```sh
make run
make test
```

## What you should see

The output demonstrates MapReduce applied to word counts using Unix pipelines and Python, along with a scheduling simulation. You should see word frequencies sorted by count.

```
--- Unix Pipeline ---
      3 mapreduce
      3 lab
      3 cs
      3 a
      2 with

--- Python MapReduce ---
      3 a
      3 cs
      3 lab
      3 mapreduce
      2 and
```

The master scheduling simulator will show two runs. The first run without backup tasks is delayed by stragglers, finishing at time 22.

```
Simulation 1: NO backup tasks
--- Starting Map Phase (M=10) ---
[ 0] map-0 scheduled on worker 0
[ 0] map-1 scheduled on worker 1 (STRAGGLER)
...
[22] reduce-3 completed on worker 3
Job finished at time 22
```

The second run enables backup tasks. The master detects stragglers when other tasks complete and schedules backup instances on free workers. The backup instances finish quickly, and the straggler instances are killed, reducing the job finish time to 8.

```
Simulation 2: WITH backup tasks
...
[ 3] map-1 BACKUP scheduled on worker 2
[ 4] map-1 BACKUP completed on worker 2
[ 4] map-1 killing other instance on worker 1
...
Job finished at time 8
```

## How it works

MapReduce breaks a job into independent map and reduce phases.

1. **Unix Pipeline (`pipeline.sh`)**: This maps directly to MapReduce concepts. `tr` serves as the map function (tokenizing words and emitting them), `sort` acts as the shuffle phase (grouping identical keys together), and `uniq -c` functions as the reducer (aggregating the counts per word).
2. **Python MapReduce (`mr_python.py`)**: This implements the logic explicitly. The `map_func` yields `(word, 1)` pairs. The framework (main loop) groups pairs by key. The `reduce_func` sums the list of values for each key.
3. **Master Simulator (`scheduler.py`)**: This models the Google MapReduce master. It manages $M$ map tasks and $R$ reduce tasks across a pool of workers. Some tasks simulate "stragglers" (bad disks, CPU contention) that take 10x longer. When backup tasks are enabled, the master schedules redundant copies of remaining tasks once idle workers are available, dramatically improving tail latency.

## Experiments to try

1. **Straggler Probability**: In `scheduler.py`, increase the `straggler_prob` to 0.5.
   > **Prediction**: The job without backups will take even longer, as multiple stragglers will dominate the timeline. The job with backups will still finish relatively quickly, but you will see more backup tasks being scheduled and more original instances being killed.
2. **Worker Pool Size**: Increase `num_workers` from 4 to 8 in `scheduler.py`.
   > **Prediction**: The map and reduce phases will overlap more tasks. The job will finish faster in both cases, but the backup-enabled run will start scheduling backups much sooner because free workers will be available earlier.
3. **Combiner Logic**: Modify `mr_python.py` to use a combiner after the map phase but before the global shuffle.
   > **Prediction**: The data transferred to the shuffle phase will be significantly smaller, as words will be partially aggregated locally before the global group-by operation.
4. **Different Reducer**: Change the pipeline to find the longest word instead of counting frequencies using `awk '{ print length($0), $0 }' | sort -n | tail -1`.
   > **Prediction**: The mapping function changes (emitting word length), the shuffle sorts by length, and the reducer selects the maximum length word.

## Questions

<details>
<summary>Why must the shuffle phase complete before any reduce tasks begin?</summary>
Reduce tasks aggregate all values for a specific key. If the shuffle phase is incomplete, a reducer might start processing a key before all its values have been mapped and grouped, leading to partial and incorrect results. MapReduce guarantees that a reducer sees all values for its assigned keys.
</details>

<details>
<summary>How does the backup task mechanism deal with a worker that is consistently slow?</summary>
The backup task is scheduled on a different worker. Because the slow worker's slowness is typically due to localized issues (bad disk, local CPU contention), the backup task on a healthy worker is highly likely to finish quickly, allowing the master to ignore the slow worker's delayed result.
</details>

<details>
<summary>In the Unix pipeline analogy, where is the intermediate data stored?</summary>
In a Unix pipeline, intermediate data is passed through memory buffers (pipes) between processes. In a real MapReduce cluster, intermediate map outputs are buffered in memory and then flushed to the local disks of the map workers. Reducers then pull this data across the network.
</details>

<details>
<summary>Why are reduce tasks not re-executed if a reduce worker fails after completing its task?</summary>
Map outputs are written to local disks, so if a map worker fails, its data is lost and must be re-computed. Reduce outputs, however, are appended directly to a highly available, distributed global file system. Once a reduce task completes, its output is safe and does not need to be re-executed even if the worker subsequently dies.
</details>

---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L08c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-18-transactions: Transactions and recovery: two-phase commit, write-ahead logging, and shadow paging

> [!info] Goal
> Make L08c concrete with real commands and measurements.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- `make` and `gcc` for compiling the C simulation.
- Python 3.12 (`/opt/aos-venv/bin/python`) for the two-phase commit simulator.

## Run commands

```sh
labs/setup/run-in-vm.sh labs/lab-18-transactions run
labs/setup/run-in-vm.sh labs/lab-18-transactions test
```

## What you should see

Write-ahead logging is noticeably faster than shadow paging. In the expected output, WAL measures `0.000376` seconds median latency, while shadow paging takes `0.000533` seconds, as shadow paging requires two synchronous disk writes instead of one.

The two-phase commit simulation traces out normal commit sequences, simulated timeouts leading to aborts, and coordinator crash recoveries.

## How it works

### Write-Ahead Logging vs Shadow Paging

The `wal_shadow.c` program measures the commit overhead of two recovery techniques:
1.  **Write-Ahead Logging (WAL)**: The system writes the intention to a log and calls `fdatasync()`. The data can be flushed in-place later. This requires only one synchronous disk write to commit the transaction.
2.  **Shadow Paging**: The system creates a new page, copies modifications, and calls `fdatasync()`. It then updates the root pointer to point to the new page and calls `fdatasync()` again. This inherently requires two synchronous operations.

### Two-Phase Commit and Recovery

The `quicksilver_2pc.py` script builds a transaction tree to simulate Quicksilver's hierarchical commit.
The protocol handles the following scenarios:
1.  **Normal Commit**: The coordinator sends a vote request, subordinates propagate it to servers, all vote commit, and the coordinator logs a global commit before sending an end commit.
2.  **Voting Failure**: A server crashes or times out before voting. The subordinate votes abort, forcing the coordinator to abort the transaction.
3.  **Coordinator Crash**: The coordinator crashes immediately after writing the `GLOBAL COMMIT` record but before distributing the end commit message. Upon recovery, the coordinator reads its log, discovers the commit decision, and resends the end commit.

## Experiments

> [!abstract]- Experiment 1: Change sync behavior
> **Prompt:** In `wal_shadow.c`, if you remove the `fdatasync()` calls, how does the performance compare?
> **Prediction:** Without `fdatasync()`, writes stay in the page cache. Both methods will run extremely fast and at similar speeds, but neither will guarantee durability.

> [!abstract]- Experiment 2: Transaction Tree Depth
> **Prompt:** In `quicksilver_2pc.py`, add a deeper level of subordinates. How does this affect the voting output?
> **Prediction:** The recursive vote request will traverse down to the leaves before propagating back up, demonstrating the true hierarchical nature of Quicksilver over a flat protocol.

> [!abstract]- Experiment 3: Subordinate Crash Recovery
> **Prompt:** Simulate a subordinate crashing immediately after voting commit but before receiving the end commit.
> **Prediction:** The subordinate must ask the coordinator for the transaction outcome upon recovery, rather than deciding independently.

## Review

<details>
<summary>Why does shadow paging require two synchronous writes?</summary>

Because the new data blocks must be safely on persistent storage before the root directory or master record is updated to point to them. If the pointer was updated before the data, a crash could leave the system pointing to garbage blocks.

</details>

<details>
<summary>How does Quicksilver's hierarchical commit differ from a flat two-phase commit?</summary>

In a flat two-phase commit, the central coordinator communicates directly with all participants. In Quicksilver, the coordinator only talks to its immediate subordinates, which recursively pass the votes and commit messages down the tree. This distributes the coordination overhead.

</details>

<details>
<summary>What occurs if a node in the transaction tree crashes before voting?</summary>

Its superior node will eventually time out waiting for the vote. The superior will treat this timeout as an abort vote and pass the abort decision up the tree, causing the entire transaction to abort.

</details>

<details>
<summary>What is the purpose of the 'FORGET' log record in two-phase commit?</summary>

The 'FORGET' record signifies that a node has completed all necessary steps (including receiving acknowledgments from all subordinates) for a transaction. Once written, the transaction state can be purged from memory and the log can be truncated.

</details>

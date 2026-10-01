---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/268998.266665"]
course: cs6210
lesson: L08
reading: required
venue: "SOSP 1997"
authors: [David E. Lowell, Peter M. Chen]
tags: [cs6210, cs6210/paper]
aliases: ["Free Transactions with Rio Vista"]
---

# Free Transactions with Rio Vista

SOSP 1997. Reading status: required. [Link](https://doi.org/10.1145/268998.266665).

> [!abstract] One-line summary
> A recoverable memory library that eliminates the redo log and synchronous disk I/O by leveraging the persistent main memory of the Rio file cache, reducing transaction overhead to microseconds.

## Problem

Even minimalist recoverable memory libraries like RVM suffer from an overhead of several milliseconds per transaction due to the necessity of synchronous disk I/O for the redo log. This latency prohibits the ubiquitous use of transactions for all persistent data manipulations in high-performance applications.

## Key idea

Vista dramatically accelerates transactions by eliminating the redo log entirely, relying instead on the reliable, persistent memory provided by the Rio file cache. Because memory writes in Rio survive crashes, Vista can commit transactions instantly in memory without invoking system calls, disk I/O, or secondary memory copies.

## Design

Vista is a tiny (720 lines of C), user-level library tailored specifically for the Rio file cache. The database is mapped directly into the application's address space. At the start of a transaction, the before-image is logged to an in-memory undo log situated in Rio's persistent memory. The application then stores modifications directly to the persistent in-memory database. Committing simply discards the undo log, while aborting copies the undo log back to the database. Rio ensures this persistent memory is written to disk upon warm reboot after a crash.

## Evaluation

Vista achieves an overhead of just 5 microseconds for small transactions. This represents a 100-fold speedup over RVM running on Rio (which still manages a redo log) and a 2000-fold speedup over standard RVM. On standard transaction processing benchmarks (debit-credit and order-entry), Vista delivered a 150-556x improvement in overall throughput compared to standard RVM.

## Limitations and critiques

Vista's reliance on Rio means the system requires a battery-backed UPS to guarantee survival across power failures, and specific OS modifications to protect the file cache from kernel crashes. Like RVM, performance degrades severely (thrashing) if the working database size exceeds available main memory.

## What it led to

Vista highlighted the massive performance potential of persistent main memory systems. It provided early evidence for how non-volatile memory (NVM) architectures could completely reshape transaction processing and operating system storage hierarchies by eliminating the disk bottleneck.

## Exam angles

<details>
<summary>How does Vista eliminate the need for a redo log compared to traditional recoverable memory systems?</summary>
Because the Rio file cache makes main memory persistent across crashes, modifications to the mapped database are durable immediately. There is no need to write a sequential redo log to disk to guarantee permanence upon commit.
</details>

<details>
<summary>What operations occur during a transaction commit in Vista?</summary>
In Vista, a transaction commit simply consists of discarding the in-memory undo log. No data is copied, no system calls are executed, and no disk I/O is performed.
</details>

<details>
<summary>How does Vista handle transaction aborts and system recovery?</summary>
To abort, Vista copies the original data from the persistent undo log back to the persistent database. Upon system recovery, Vista checks for any uncommitted transactions in the persistent memory and rolls them back using the undo logs before resuming operation.
</details>

## Related

- Lessons: [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md), [L08c](../Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md)

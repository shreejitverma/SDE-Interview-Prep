---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/356842.356847"]
course: cs6210
lesson: L08
reading: self-study
venue: "ACM Computing Surveys 1981"
authors: ["Jim Gray", "Paul McJones", "Mike Blasgen", "Bruce Lindsay", "Raymond Lorie", "Tom Price", "Franco Putzolu", "Irving Traiger"]
tags: [cs6210, cs6210/paper]
aliases: ["The Recovery Manager of the System R Database Manager"]
---

# The Recovery Manager of the System R Database Manager

ACM Computing Surveys 1981.
Reading status: self-study.
[Link](https://doi.org/10.1145/356842.356847).

> [!abstract] One-line summary
> Introduces a transaction recovery manager for System R that uses a DO-UNDO-REDO protocol, disk-based logging, and shadow pages to ensure ACID properties and support save points.

## Problem

Application programmers writing electronic bookkeeping programs need to ensure their transactions are atomic and durable, even in the presence of transaction aborts, system crashes, or media failures.
At the time, providing a fault-tolerant system that supported multi-user concurrency and allowed partial undoing of effects without losing committed updates was a significant challenge for database management systems.

## Key idea

The recovery manager implements a DO-UNDO-REDO protocol allowing new recoverable types and operations to be added.
It combines a shadow-version/current-version mechanism (differential files) for system checkpoint/restart with an incremental transaction log recorded on disk (rather than tape) for transaction UNDO and REDO.
It also introduces the concept of transaction save points, which act as firewalls allowing application programs to partially roll back their transactions in case of errors instead of aborting entirely.

## Design

- Uses shadow pages and current pages for file storage.
- Changes are made to current pages in the buffer pool.
- Saving a file writes altered pages to disk while updating the page table.
- Logs updates to logged files using a DO, UNDO, REDO operations protocol.
- Log records contain the old and new values of the updated object.
- Provides transaction primitives including BEGIN, SAVE, READ_SAVE, UNDO, ABORT, and COMMIT.
- Restores the system state after a crash by resetting to the shadow version.
- Uses the transaction log to undo aborted transactions and redo committed transactions.

## Evaluation

- Approximately 97 percent of transactions execute successfully.
- Transaction undo occurs about twice a minute on a typical system running one transaction per second.
- System restarts occur every few days and take seconds to minutes.
- Media failures occur one to two times a year and take about an hour to recover.

## Limitations and critiques

- The system did not support a LOG and NO SHADOW option.
- The authors regretted this because the log makes shadows redundant and the shadow mechanism is quite expensive for large files.
- The transaction model lacks parallelism within a transaction.
- Only a limited form of transaction nesting is supported via the save point mechanism.

## What it led to

Pioneered the Write-Ahead Logging and DO-UNDO-REDO protocol used in almost all modern relational databases.
Influenced the development of the ARIES recovery algorithm.
Established the foundation for robust transactional processing in commercial database systems.

## Exam angles

- How does System R combine shadows and logging to achieve both system restart and transaction abort capabilities?
- Why did the System R team regret not supporting the LOG and NO SHADOW option, and how did it affect performance?
- What are transaction save points, and how do they simplify error handling for multi-step application transactions?

## Related

- Lessons: [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md), [L08c](../Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md)

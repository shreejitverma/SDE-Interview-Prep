---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/1629575.1629591"]
course: cs6210
lesson: L08
reading: partial
venue: "SOSP 2009"
authors: ["Donald E. Porter", "Owen S. Hofmann", "Christopher J. Rossbach", "Alexander Benn", "Emmett Witchel"]
tags: [cs6210, cs6210/paper]
aliases: ["Operating System Transactions"]
---

# Operating System Transactions

SOSP 2009.
Reading status: partial (see syllabus for required sections).
[Link](https://doi.org/10.1145/1629575.1629591).

> [!abstract] One-line summary
> Introduces TxOS, a variant of Linux that provides system transactions to allow applications to perform atomic, consistent, isolated, and durable updates to heterogeneous OS resources.

## Problem

Applications often struggle to make consistent updates to operating system resources because individual system calls are atomic but it is difficult to condense complex, multi-step operations into a single atomic unit.
This lack of concurrency control in the OS API leads to pervasive issues like time-of-check-to-time-of-use (TOCTTOU) security vulnerabilities in the file system and inconsistent states during partial software installations.

## Key idea

TxOS provides system transactions that allow programmers to group accesses to heterogeneous OS resources (like files, pipes, and signals) into logical units with ACID properties.
It utilizes optimistic, lazy version management (copy-on-write) for isolation, which avoids deadlocks and the need for complex eager versioning lock mechanisms.
Furthermore, it cleanly integrates with user-level transactional memory to provide a complete, composable transactional programming model for multi-threaded applications while ensuring strong isolation.

## Design

- Provides a simple API consisting of sys_xbegin, sys_xend, and sys_xabort to wrap code regions with consistency constraints.
- Employs lazy version management where transactions operate on private copies of data structures to prevent deadlocks and allow instant abortion of transactions.
- Enforces strong isolation by serializing system transactions and non-transactional system calls through a global ordering for kernel locks.
- Integrates with user-level software and hardware transactional memory systems to provide transactional semantics for both application and system state.

## Evaluation

- A transactional installation of OpenSSH incurs only 10 percent performance overhead.
- A non-transactional compilation of Linux incurs negligible overhead on TxOS.
- Replacing Berkeley DB with flat files and system transactions for the OpenLDAP directory service improves performance on write-mostly workloads by 2x to 4x.

## Limitations and critiques

- Does not support transactional semantics for all system calls (supports 150 out of 303 Linux system calls).
- This limits applicability for certain edge cases.
- Cannot encapsulate code that communicates outside a transaction and requires a response, as buffering the response until commit violates isolation and causes deadlocks.
- The current implementation requires that all transactional state must fit into main memory.

## What it led to

Demonstrated that mature, commodity operating systems could support system transactions efficiently.
Inspired further research into integrating OS-level abstractions with transactional memory.
Influenced discussions on providing richer, composition-friendly concurrency APIs in modern operating systems.

## Exam angles

- How does TxOS handle the interaction between transactional and non-transactional kernel threads to provide strong isolation?
- Why does TxOS use lazy version management instead of eager version management, and what trade-offs are involved?
- How do system transactions effectively eliminate TOCTTOU vulnerabilities in file systems?

## Related

- Lessons: [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md), [L08c](../Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md)

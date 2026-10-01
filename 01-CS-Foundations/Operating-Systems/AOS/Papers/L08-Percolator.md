---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi10/large-scale-incremental-processing-using-distributed-transactions-and"]
course: cs6210
lesson: L08
reading: partial
venue: "OSDI 2010"
authors: ["Daniel Peng", "Frank Dabek"]
tags: [cs6210, cs6210/paper]
aliases: ["Large-scale Incremental Processing Using Distributed Transactions and Notifications"]
---

# Large-scale Incremental Processing Using Distributed Transactions and Notifications

OSDI 2010.
Reading status: partial (see syllabus for required sections).
[Link](https://www.usenix.org/conference/osdi10/large-scale-incremental-processing-using-distributed-transactions-and).

> [!abstract] One-line summary
> Presents Percolator, a system for incrementally processing updates to massive datasets using distributed snapshot isolation transactions and a notification framework built on Bigtable.

## Problem

Updating a web search index as new documents are crawled requires continuously transforming a massive repository of existing documents via small, independent mutations.
Existing systems fail to bridge the gap: traditional databases cannot meet the storage or throughput requirements of tens of petabytes of data.
Batch-processing systems like MapReduce cannot process small updates individually and incur latency proportional to the size of the repository rather than the update.

## Key idea

Percolator is a system for incrementally processing updates to a massive dataset, providing ACID-compliant snapshot isolation transactions over a random-access repository built on Bigtable.
It structures computations using lightweight observers that are triggered when user-specified columns change.
This allows developers to maintain strict data invariants and efficiently track the state of an incremental computation without the need for periodic full repository scans that MapReduce batch processing would require.

## Design

- Uses Bigtable for reliable, scalable storage.
- Implements multi-row, distributed transactions on top of it through client-coordinated two-phase commit.
- Relies on a high-throughput timestamp oracle to provide strictly increasing timestamps, enabling snapshot isolation semantics for read-write operations.
- Explicitly maintains transaction locks in special in-memory Bigtable columns, allowing any node to issue requests to directly modify state.
- Adopts a lazy approach to lock cleanup for failed transactions, synchronizing on a designated primary lock to avoid race conditions during recovery.
- Employs an observer framework ("notifications") where worker processes scan Bigtable for changed columns and trigger application code to process updates incrementally.

## Evaluation

- Migrating the indexing system from MapReduce to Percolator reduced the average age of documents in Google search results by nearly 50 percent.
- Average document processing latency was reduced by a factor of 100.
- The centralized timestamp oracle is capable of serving around 2 million timestamps per second from a single machine.

## Limitations and critiques

- Not intended to supplant MapReduce for computations where the result cannot be broken down into small updates, such as global sorting.
- The lazy lock cleanup mechanism can potentially delay conflicting transaction commits by tens of seconds.
- This makes it unsuitable for OLTP workloads requiring extremely low latency.
- Snapshot isolation does not provide full serializability, leaving transactions susceptible to write skew anomalies.

## What it led to

Enabled Google's Caffeine search index architecture, dramatically improving index freshness and reducing indexing latency.
Influenced the design of large-scale incremental processing systems.
Prompted the addition of distributed transaction support to other NoSQL databases like Apache HBase.

## Exam angles

- How does Percolator implement distributed two-phase commit over Bigtable without relying on a centralized transaction manager?
- Why did Percolator choose a lazy approach to lock cleanup, and what impact does this have on transaction latency?
- What is the role of the timestamp oracle in Percolator's snapshot isolation protocol, and how does it guarantee consistent reads?

## Related

- Lessons: [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md), [L08c](../Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md)

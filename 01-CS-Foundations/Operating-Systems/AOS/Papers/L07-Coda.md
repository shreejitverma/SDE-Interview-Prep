---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: ["https://doi.org/10.1109/12.54838"]
course: cs6210
lesson: L07
reading: partial
venue: "IEEE Trans. Computers 1990"
authors: ["Mahadev Satyanarayanan", "James J. Kistler", "Puneet Kumar", "Maria E. Okasaki", "Ellen H. Siegel", "David C. Steere"]
tags: [cs6210, cs6210/paper]
aliases: ["Coda: A Highly Available File System for a Distributed Workstation Environment"]
---

# Coda: A Highly Available File System for a Distributed Workstation Environment

IEEE Trans. Computers 1990. Reading status: partial. [Link](https://doi.org/10.1109/12.54838).

> [!abstract] One-line summary
> Coda is a distributed file system that provides high availability through server replication and disconnected client operation.

## Problem

Large-scale distributed file systems like AFS offer excellent scalability but remain highly vulnerable to network partitions and server failures.
When a central server becomes unreachable, users are completely blocked from accessing their files, severely impacting productivity.
As clusters grow larger and rely more on portable computing devices, the need for continuous data access during intermittent connectivity becomes critical.
Achieving high availability while preserving the shared Unix file system semantics is fundamentally challenging.

## Key idea

Coda improves resiliency by employing an optimistic replication strategy that assumes write-sharing among users is rare.
It uses two complementary mechanisms: server replication using Volume Storage Groups, and disconnected operation where clients act autonomously.
During disconnected operation, a client relies entirely on its local disk cache to service file requests, logging any modifications locally.
When connectivity is restored, the client performs a reintegration process to synchronize its offline updates with the server replicas, utilizing version vectors to detect and handle conflicts.
The syllabus requires reading sections I through IV, which cover the rationale, architecture, and the mechanics of disconnected operation.

## Design

Coda groups servers into Volume Storage Groups that hold replicas of data volumes, while a client communicates with the subset of servers it can reach, called the Accessible Volume Storage Group.
Under normal operation, clients use parallel remote procedure calls to multicast updates to all accessible servers while fetching data from a single preferred server.
Cache coherence is maintained through callbacks, but Coda extends this with volume version vectors to detect missed updates upon reconnection.
If the accessible server group shrinks to zero, the client seamlessly transitions into disconnected operation.
Users can prioritize files for caching through a hoarding mechanism, ensuring critical data is locally available before a disconnection event occurs.

## Evaluation

The researchers built a prototype of Coda and evaluated it on a testbed of IBM RT workstations.
They demonstrated that the performance overhead of providing high availability is entirely reasonable for typical workloads.
For instance, the time to complete the Andrew Benchmark on Coda with server replication enabled was only a few percent longer than running it on a non-replicated AFS setup.

## Limitations and critiques

Because Coda uses optimistic replication, conflicting updates can occur if multiple disconnected clients modify the same file simultaneously.
While directory conflicts can often be resolved automatically, file conflicts must be flagged for manual resolution by the user, which can be tedious.
The system is poorly suited for database workloads or applications that require strict, pessimistic consistency guarantees.

## What it led to

Coda pioneered the concept of disconnected operation and profoundly influenced the field of mobile computing.
It established that optimistic replication could provide massive usability benefits for intermittent network connections without overwhelming users with conflict resolution.
The hoarding and reintegration techniques introduced by Coda became foundational elements in many subsequent mobile and weakly connected file systems.

## Exam angles

<details>
<summary>Why does Coda use an optimistic replication strategy rather than a pessimistic one?</summary>
An optimistic strategy allows clients to continue working and modifying files even when disconnected from the network or during a partition.
This approach assumes that concurrent write-sharing is rare in academic and development environments, favoring high availability over strict consistency.
</details>

<details>
<summary>How does Coda handle cache misses during disconnected operation?</summary>
If a client encounters a cache miss while disconnected, the file system operation simply fails and blocks the computation.
To mitigate this, Coda provides a hoarding mechanism that lets users specify a prioritized list of files to keep in the local disk cache at all times.
</details>

<details>
<summary>What is the difference between a Volume Storage Group and an Accessible Volume Storage Group?</summary>
A Volume Storage Group is the complete set of servers that hold replicas for a specific volume.
The Accessible Volume Storage Group is the subset of those servers that a particular client can currently communicate with over the network.
</details>

## Related

- Lessons: [L07a](../Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md), [L07b](../Part-4-Distributed-Subsystems-and-Recovery/L07b-Distributed-Shared-Memory.md), [L07c](../Part-4-Distributed-Subsystems-and-Recovery/L07c-Distributed-File-Systems.md)

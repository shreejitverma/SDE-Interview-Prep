---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/1294261.1294281"]
course: cs6210
lesson: L09
reading: required
venue: "SOSP 2007"
authors: ["Giuseppe DeCandia", "Deniz Hastorun", "Madan Jampani", "Gunavardhan Kakulapati", "Avinash Lakshman", "Alex Pilchin", "Swaminathan Sivasubramanian", "Peter Vosshall", "Werner Vogels"]
tags: [cs6210, cs6210/paper]
aliases: ["Dynamo: Amazon's Highly Available Key-value Store"]
---

# Dynamo: Amazon's Highly Available Key-value Store

SOSP 2007. Reading status: required. [Link](https://doi.org/10.1145/1294261.1294281).

> [!abstract] One-line summary
> Dynamo is a highly available, decentralized key-value store that sacrifices strong consistency for availability to provide an always-on experience for Amazon's core services.

## Problem

Amazon requires an extremely reliable and scalable storage system for its e-commerce platform.
Traditional relational databases provide ACID guarantees at the expense of availability and scalability.
Even slight outages impact customer trust and financial performance, necessitating an always-writable data store that treats failure as the normal case.

## Key idea

Dynamo provides a highly available, decentralized key-value store that sacrifices strong consistency for availability, operating under an eventual consistency model.
It achieves this by synthesizing several techniques: consistent hashing for partitioning, object versioning to handle concurrent updates, vector clocks to capture causality, a sloppy quorum with hinted handoff for handling temporary failures, and anti-entropy using Merkle trees to recover from permanent failures.

## Design

Data is partitioned and replicated using consistent hashing with virtual nodes.
Consistency is maintained through object versioning, and vector clocks identify conflicting versions that are resolved during reads, often by the application.
Updates are managed using a quorum-like technique, but Dynamo uses a sloppy quorum and hinted handoff to ensure writes succeed even during network partitions or server failures.
A gossip-based distributed failure detection and membership protocol maintains ring state, while Merkle trees synchronize divergent replicas in the background.

## Evaluation

Dynamo was evaluated in Amazon's production environment, handling core services like the Shopping Cart.
It efficiently scaled during the busy holiday shopping season, serving tens of millions of requests resulting in millions of checkouts in a single day.
The system met stringent service level agreements (SLAs) measured at the 99.9th percentile, consistently providing read and write latencies under 300ms even under peak loads.

## Limitations and critiques

Pushing conflict resolution to the read path and the application level increases the complexity of application development.
The eventual consistency model means that reads may return stale data, which is unacceptable for applications requiring strong consistency or isolation.
Additionally, the system's focus is on a trusted, single administrative domain, so it lacks security mechanisms for untrusted environments.

## What it led to

Dynamo significantly influenced the design of subsequent NoSQL databases and highly available distributed data stores, popularizing the concept of eventual consistency in enterprise systems.
Systems like Apache Cassandra and Riak heavily adopted Dynamo's architectural concepts, including consistent hashing, vector clocks, and gossip protocols.

## Exam angles

<details>
<summary>How does Dynamo use consistent hashing, and what problem do virtual nodes solve?</summary>
Dynamo uses consistent hashing to partition data across storage nodes by hashing keys onto a circular ring space, assigning each key to the first node clockwise from its position.
To address the non-uniform data and load distribution of basic consistent hashing, Dynamo introduces virtual nodes.
Each physical node is assigned multiple positions on the ring, allowing load to be distributed more evenly and proportioned according to the heterogeneous capacities of the physical servers.
</details>

<details>
<summary>Why does Dynamo resolve conflicts during reads rather than writes, and how does it track these conflicts?</summary>
Dynamo resolves conflicts during reads to ensure that the data store is always writable, prioritizing high availability so that actions like adding items to a shopping cart are never rejected due to temporary network partitions or failures.
It uses vector clocks, which associate a list of (node, counter) pairs with each object version, to capture causality between updates.
When concurrent writes lead to divergent versions, the system returns all conflicting versions to the application during a read, forcing the application to perform semantic reconciliation.
</details>

<details>
<summary>Explain how hinted handoff and sloppy quorums improve Dynamo's write availability during failures.</summary>
In a traditional strict quorum, a write might fail if a specific replica node is unreachable.
Dynamo uses a sloppy quorum, where read and write operations are performed on the first N healthy nodes in the preference list, which may not be the designated replica nodes.
If a designated node is unavailable, another node temporarily accepts the write and stores a hint indicating the intended recipient (hinted handoff).
Once the original node recovers, the temporary node forwards the data to it, ensuring high write availability despite temporary failures.
</details>

## Related

- Lessons: [L09a](../Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md), [L09b](../Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md), [L09c](../Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md)

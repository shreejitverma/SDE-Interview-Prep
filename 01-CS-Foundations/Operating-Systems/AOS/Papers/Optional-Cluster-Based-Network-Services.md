---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/268998.266662"]
course: cs6210
lesson: optional
reading: optional
venue: "SOSP 1997"
authors: ["Armando Fox", "Steven D. Gribble", "Yatin Chawathe", "Eric A. Brewer", "Paul Gauthier"]
tags: [cs6210, cs6210/paper]
aliases: ["Cluster-Based Scalable Network Services"]
---

# Cluster-Based Scalable Network Services

SOSP 1997.
Reading status: optional.
[Link](https://doi.org/10.1145/268998.266662).

> [!abstract] One-line summary
> A layered architecture and programming model (BASE and TACC) for building scalable, highly available network services on commodity clusters.

## Problem

Building scalable, highly available, and cost-effective network services was difficult.
Using SMPs was expensive and less scalable than clusters, but clusters were hard to manage, lacked shared state, and frequently faced partial hardware or software failures.
A new architectural approach was needed to hide these complexities from service developers.

## Key idea

Introduce a layered architecture to encapsulate cluster management and utilize BASE (Basically Available, Soft State, Eventual consistency) semantics instead of ACID, trading strict consistency for high availability and simplicity.
They also introduce TACC (Transformation, Aggregation, Caching, Customization) as a composable programming model for internet services.

## Design

The architecture is divided into three layers: the SNS (Scalable Network Service) layer for load balancing and fault tolerance, the TACC layer for worker composition, and the Service layer.
A centralized manager handles load balancing and monitors node health, avoiding the complexity of distributed scheduling.
Soft state is aggressively utilized and can be rebuilt after a failure via multicasts from peer nodes.
The system also supports an overflow pool of non-dedicated desktop machines to absorb unexpected spikes in traffic.

## Evaluation

Evaluated via TranSend, a web distillation proxy deployed for UC Berkeley dialup users, on a cluster of SPARCstations.
The system demonstrated linear scalability up to 15 nodes.
It successfully withstood node failures, recovering and rebuilding soft state within seconds.
By compressing and transforming images, TranSend reduced end-to-end latency by a factor of 3 to 5 for dialup users.

## Limitations and critiques

The central manager could potentially become a bottleneck or a single point of failure, although the authors mitigate this using process-pair fault tolerance.
Furthermore, BASE semantics are not suitable for all applications; features like financial billing or strict user data management still require ACID databases.

## What it led to

The paper formally introduced and popularized the concept of BASE semantics, which became a foundational principle for modern NoSQL databases and distributed systems.
It also heavily influenced modern cloud and microservices architectures, where stateless workers sit behind intelligent load balancers.

## Exam angles

<details>
<summary>What are BASE semantics and how do they differ from ACID?</summary>
BASE stands for Basically Available, Soft State, Eventual consistency.
It prioritizes continuous availability and fault tolerance over strict consistency, unlike ACID which ensures absolute data consistency and durability even if it means the system becomes unavailable.
</details>

<details>
<summary>Why did the authors choose a centralized manager for load balancing instead of a distributed one?</summary>
A centralized manager is easier to implement and reason about.
Since it is made fault-tolerant and is designed to avoid becoming a performance bottleneck, it simplifies the load balancing policy compared to a complex distributed approach.
</details>

<details>
<summary>What is the purpose of the overflow pool?</summary>
It provides a set of non-dedicated backup machines, such as desktop workstations, that can be temporarily harnessed by the manager to handle unexpected bursts of load without requiring the provisioning of expensive dedicated hardware.
</details>

## Related

- Lessons: not covered in lectures (optional reading)

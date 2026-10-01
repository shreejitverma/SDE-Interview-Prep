---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1109/4236.939450"]
course: cs6210
lesson: L09
reading: partial
venue: "IEEE Internet Computing 2001"
authors: ["Eric A. Brewer"]
tags: [cs6210, cs6210/paper]
aliases: ["Lessons from Giant-Scale Services"]
---

# Lessons from Giant-Scale Services

IEEE Internet Computing 2001. Reading status: partial. [Link](https://doi.org/10.1109/4236.939450).
Note: For this partial reading, the syllabus typically focuses on cluster architectures, yield, harvest, and the CAP theorem concepts.

> [!abstract] One-line summary
> Giant-scale services must embrace failure and trade strong consistency for high availability using cluster-based architectures and degraded operating modes.

## Problem

Traditional metrics of uptime and availability are insufficient for evaluating the performance and reliability of giant-scale internet services.
These services must handle unprecedented scale, continuous evolution, and partial failures without sacrificing the user experience.
Scaling up high-end symmetric multiprocessor mainframes becomes economically unfeasible and technically challenging at internet scale.
Software must be designed with the assumption that underlying hardware components will fail frequently.

## Key idea

Giant-scale services should be designed with the CAP theorem in mind, trading strong consistency for high availability and partition tolerance.
System availability should be measured in terms of yield, which is the fraction of queries answered, and harvest, which is the fraction of the complete database reflected in the response.
By decoupling these metrics, a service can maintain a high yield even during component failures by intentionally returning partial results or degrading its harvest.
This paradigm shift allows for the construction of highly robust systems from unreliable commodity components.

## Design

The architecture typically relies on large clusters of commodity hardware rather than specialized fault-tolerant mainframes.
The design embraces the principle that failures are the norm rather than the exception, employing software-level fault tolerance.
Data is heavily replicated and partitioned across multiple nodes to ensure both availability and scalability.
The system leverages degraded modes to return partial results when some nodes are unreachable or under heavy load.
This strategy ensures the service remains responsive and maintains its yield, even if the harvest is temporarily reduced.

## Evaluation

The paper draws upon observational data from real-world systems like the Inktomi search engine and major web portals.
It demonstrates that using clusters of commodity workstations achieves higher cost-efficiency and comparable availability to high-end servers.
A well-managed 100-node cluster can achieve over 99.99% availability by carefully balancing redundancy, load distribution, and automated failover.
The DQ principle is used to evaluate system capacity, showing that the product of queries per second and data size per query is a constant limit.

## Limitations and critiques

The paper provides high-level architectural guidelines and retrospective lessons rather than proposing a specific, reproducible algorithm or system implementation.
The focus on eventual consistency and degraded modes makes the programming model significantly more complex for application developers.
Developers must manually handle stale, partial, or conflicting data at the application layer.
The CAP theorem, while foundational to distributed systems, is presented informally here and lacks a rigorous mathematical proof in this specific paper.

## What it led to

The concepts of yield and harvest became fundamental metrics for designing and evaluating internet-scale distributed systems.
The informal introduction of the CAP theorem sparked extensive research into the theoretical limits of distributed computing and data consistency.
The architectural shift toward commodity clusters influenced the design of modern cloud computing infrastructure and large-scale web services.
It directly influenced the creation of NoSQL databases that prioritize availability and partition tolerance over strict ACID properties.

## Exam angles

> [!question]- What is the difference between yield and harvest in the context of giant-scale services?
> Yield is the probability of completing a request, effectively measuring the system's uptime.
> Harvest measures the completeness of the data returned in the response, such as the fraction of the index searched.

> [!question]- How does the DQ principle relate to the performance of a distributed service?
> The DQ principle states that the product of queries per second and the data size per query is a constant limit for a given system capacity.
> To increase queries per second, the system must either increase overall capacity or proportionally decrease the data size searched per query.

> [!question]- Why might a giant-scale service intentionally reduce its harvest during periods of high load?
> Reducing harvest by ignoring some data partitions allows the service to maintain a high yield and responsiveness despite the elevated load.
> This degraded mode ensures that users still receive a fast, albeit partial, response rather than experiencing a complete service outage.

## Related

- Lessons: [L09a](../Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md), [L09b](../Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md), [L09c](../Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md)

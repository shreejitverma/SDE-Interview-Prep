---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/582419.582443"]
course: cs6210
lesson: L06
reading: required
venue: "OOPSLA 2002"
authors: ["Emmanuel Cecchet", "Julie Marguerite", "Willy Zwaenepoel"]
tags: [cs6210, cs6210/paper]
aliases: ["Performance and Scalability of EJB Applications"]
---

# Performance and Scalability of EJB Applications

OOPSLA 2002. Reading status: required. [Link](https://doi.org/10.1145/582419.582443).

> [!abstract] One-line summary
> An empirical study of EJB application performance demonstrating that application design choices dominate performance, with stateless session beans matching servlet performance while entity beans suffer from severe communication overheads.

## Problem

Enterprise JavaBeans (EJB) was introduced to simplify the development of scalable, distributed business applications by handling middleware services like transaction management, persistence, and security.
However, developers faced a bewildering array of architectural choices, such as using session beans, entity beans with bean-managed or container-managed persistence, or various design patterns like the session facade.
The performance implications of these choices, combined with different EJB container designs and communication layer optimizations, were poorly understood.
This lack of empirical data led to systems that failed to scale under load despite using enterprise-grade application servers.

## Key idea

The performance of an EJB application is overwhelmingly determined by its architectural design and implementation method rather than the choice of application server or container design.
The authors show that using stateless session beans performs just as well as a pure servlet implementation and an order of magnitude better than entity bean implementations.
Entity beans suffer from excessive fine-grained network calls and serialization overheads.
While the session facade pattern attempts to mitigate this by wrapping entity beans, it only succeeds if the container optimizes local communication or if EJB 2.0 local interfaces are used to bypass the network stack entirely.
Container design choices, such as dynamic proxies using reflection versus pre-compiled stubs, only become significant bottlenecks when the application architecture forces a high volume of bean invocations.

## Design

The authors built RUBiS, an auction site modeled after eBay, using six different architectural approaches.
The baseline implementation used pure Java servlets with hand-coded database access.
The first EJB version used stateless session beans containing all business and data access logic.
Two entity bean versions mapped database tables to objects, using either Container-Managed Persistence (CMP) or Bean-Managed Persistence (BMP).
A session facade version wrapped the entity beans with stateless session beans to provide a coarse-grained remote interface to clients.
The final version used the session facade pattern with EJB 2.0 local interfaces to ensure intra-JVM calls bypassed the network stack.
These versions were deployed on two open-source containers with orthogonal designs: JBoss, which relies heavily on dynamic proxies and reflection, and JOnAS, which uses pre-compiled classes.

## Evaluation

The experiments used a multi-tier setup with a Web server, servlet container, EJB server, and database server on separate physical machines.
The workload was driven by a client emulator simulating concurrent users on a 1.4GB database populated with 1 million users and 330,000 bids.
The servlets-only baseline peaked at 12,000 interactions per minute under the browsing mix.
The stateless session beans implementation closely followed, peaking at 10,150 interactions per minute.
In contrast, the entity bean implementations peaked at under 2,000 interactions per minute, making them over five times slower than the session bean approach.
While the session facade pattern suffered from communication overheads with standard RMI, EJB 2.0 local interfaces allowed it to bypass network marshalling and achieve much better scalability.

## Limitations and critiques

The study heavily relies on an auction site workload that is very read-intensive.
A write-heavy workload might expose different bottlenecks, particularly in database lock contention and transaction management overhead.
The experiments were conducted on relatively old hardware and early versions of the JVM and EJB containers.
Modern JVMs with advanced just-in-time compilation and improved reflection performance might narrow the gap between the pre-compiled and dynamic proxy container designs.
Furthermore, the shift towards lightweight frameworks like Spring and object-relational mappers like Hibernate has made traditional EJB entity beans largely obsolete.

## What it led to

The findings validated the industry shift away from heavy-weight remote entity beans toward lightweight, POJO-based architectures.
It highlighted the critical importance of coarse-grained interfaces in distributed object systems to avoid network overhead.
The severe performance issues with EJB 1.1 entity beans documented in this paper directly motivated the improvements in EJB 2.0 and EJB 3.0, specifically the introduction of local interfaces.
Ultimately, the complexities and overheads of the EJB architecture highlighted here paved the way for simpler dependency injection frameworks like Spring.

## Exam angles

<details>
<summary>Why did the session facade pattern perform worse than direct entity bean access in EJB 1.1 containers?</summary>
The session facade pattern introduced an additional layer of abstraction.
In EJB 1.1, all bean-to-bean communication, even within the same JVM, was treated as a remote method invocation.
This meant that calls from the session facade to the entity beans incurred serialization and network stack overheads.
Instead of reducing overhead, the facade added another hop unless the container explicitly optimized local routing or EJB 2.0 local interfaces were used.
</details>

<details>
<summary>How did the design choice between dynamic proxies and pre-compiled classes affect performance?</summary>
JBoss used dynamic proxies which relied heavily on Java reflection at runtime to dispatch method calls.
JOnAS used pre-compiled stubs which avoided reflection overhead.
For session beans with coarse-grained calls, this difference was negligible because the network overhead dominated.
However, for entity beans with many fine-grained getter and setter calls, the reflection overhead in JBoss became a significant bottleneck.
</details>

<details>
<summary>What is the primary architectural lesson for building distributed object systems highlighted by this study?</summary>
Avoid fine-grained remote object interactions.
Remote method invocations are orders of magnitude slower than local method calls due to network and serialization overheads.
Distributed components should expose coarse-grained interfaces that transfer bulk data in a single call rather than requiring many small requests to retrieve individual attributes.
</details>

## Related

- Lessons: [L06a](../Part-3-Distributed-Systems/L06a-Spring-Operating-System.md), [L06b](../Part-3-Distributed-Systems/L06b-Java-RMI.md), [L06c](../Part-3-Distributed-Systems/L06c-Enterprise-Java-Beans.md)

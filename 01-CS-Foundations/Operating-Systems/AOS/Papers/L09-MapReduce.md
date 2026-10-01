---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi-04/mapreduce-simplified-data-processing-large-clusters"]
course: cs6210
lesson: L09
reading: required
venue: "OSDI 2004"
authors: ["Jeffrey Dean", "Sanjay Ghemawat"]
tags: [cs6210, cs6210/paper]
aliases: ["MapReduce: Simplified Data Processing on Large Clusters"]
---

# MapReduce: Simplified Data Processing on Large Clusters

OSDI 2004. Reading status: required. [Link](https://www.usenix.org/conference/osdi-04/mapreduce-simplified-data-processing-large-clusters).

> [!abstract] One-line summary
> MapReduce provides a simple programming model and a scalable runtime system for processing massive datasets across large clusters of commodity machines.

## Problem

Processing large datasets on commodity clusters is complex because programmers must handle parallelization, fault tolerance, data distribution, and load balancing manually.
This underlying complexity obscures the core computation logic of the application.
Previous abstractions were often too low-level or restricted in their applicability to general-purpose data processing tasks.
It was difficult to write distributed programs that were both efficient and resilient to the frequent hardware failures common in large clusters.

## Key idea

MapReduce provides a simple programming model consisting of a Map function that processes key-value pairs and a Reduce function that merges values associated with the same intermediate key.
The underlying runtime system automatically handles partitioning the input data, scheduling the program's execution across a set of machines, handling machine failures, and managing the required inter-machine communication.
By hiding the complex details of distributed execution, MapReduce allows programmers with no experience in parallel distributed systems to easily utilize the resources of a large distributed system.

## Design

The system consists of a single master and multiple worker nodes.
The master assigns Map and Reduce tasks to idle workers and tracks their state.
Map tasks read splits of the input data and generate intermediate key-value pairs stored on local disks.
Reduce tasks read these intermediate pairs, sort them by key, and pass them to the user's Reduce function.
The design leverages the Google File System for reading inputs and writing outputs.
The system uses data locality to schedule Map tasks on nodes that already hold the input data, minimizing network traffic.

## Evaluation

The authors evaluated MapReduce sorting a terabyte of data on a cluster of about 1,800 machines.
The system sorted 10^10 100-byte records in 891 seconds, including startup overhead.
Another benchmark running a distributed grep over 10^10 100-byte records finished in 150 seconds.
These results demonstrated that the framework could achieve performance comparable to highly tuned, custom-built distributed systems.

## Limitations and critiques

MapReduce is designed for batch processing and is unsuitable for interactive or real-time queries.
The single master architecture presents a potential bottleneck and a single point of failure, although the master's state can be checkpointed.
Stragglers can delay the entire job, which the system mitigates through redundant execution of trailing tasks.
The model forces all computations into a two-phase Map and Reduce structure, which can be unnatural or inefficient for certain algorithms like iterative graph processing.

## What it led to

MapReduce catalyzed the big data revolution and inspired open-source implementations like Apache Hadoop.
It shifted the paradigm for large-scale data processing toward commodity clusters and simplified distributed computing for non-expert programmers.
It also paved the way for subsequent frameworks like Spark that address MapReduce's performance limitations for iterative algorithms by keeping intermediate data in memory.

## Exam angles

> [!question]- What is the purpose of the backup tasks mechanism in MapReduce?
> It mitigates the straggler problem by redundantly executing the remaining tasks near the end of a MapReduce operation.
> The framework schedules backup executions of the remaining in-progress tasks.
> The task is marked as completed whenever either the primary or the backup execution finishes.

> [!question]- How does the MapReduce framework minimize network bandwidth consumption during the map phase?
> The master scheduler takes the location of the input files into account and attempts to schedule map tasks on machines that contain a replica of the corresponding input data.
> If that fails, it attempts to schedule a map task near a replica of that task's input data on the same network switch.

> [!question]- Describe the role of the combiner function in MapReduce.
> A combiner function performs partial merging of intermediate data on the map worker's local disk before it is sent over the network to a reduce worker.
> This significantly reduces the volume of data transmitted across the network, thereby improving overall performance.

## Related

- Lessons: [L09a](../Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md), [L09b](../Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md), [L09c](../Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md)

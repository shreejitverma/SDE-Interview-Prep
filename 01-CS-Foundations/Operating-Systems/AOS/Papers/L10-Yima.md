---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1109/MC.2002.1009169"]
course: cs6210
lesson: L10
reading: required
venue: "IEEE Computer 2002"
authors: ["Cyrus Shahabi", "Roger Zimmermann", "Kun Fu", "Shu-Yuen Didi Yao"]
tags: [cs6210, cs6210/paper]
aliases: ["Yima: A Second-Generation Continuous Media Server"]
---

# Yima: A Second-Generation Continuous Media Server

IEEE Computer 2002. Reading status: required. [Link](https://doi.org/10.1109/MC.2002.1009169).

> [!abstract] One-line summary
> Yima is a completely distributed, scalable, bipartite continuous media server built from commodity hardware that uses pseudorandom placement and a client-controlled delivery rate to smoothly stream media.

## Problem

First-generation continuous media servers were either expensive, proprietary carrier-class solutions or low-cost single-node systems that failed to scale.
Master-slave cluster designs suffered from centralized bottlenecks where the master node handled all control and delivery logic, causing internode traffic and creating a single point of failure.
Continuous media requires high throughput and precise synchronization to avoid presentation glitches while accommodating very large object sizes.

## Key idea

To overcome the bottlenecks of master-slave multimedia clusters, Yima proposes a completely distributed, bipartite architecture where all nodes run identical software.
Data is pseudo-randomly assigned across storage nodes to enable load balancing and online scaling without huge metadata overhead or full data redistribution.
Client feedback via a fine-grained speedup and slowdown interpacket delay protocol drives the flow control of packets directly from individual storage nodes.
This eliminates centralized scheduling and allows precise multi-stream synchronization for continuous media.

## Design

Yima uses pseudorandom block placement (specifically the Scaddar algorithm) rather than round-robin striping, allowing the system to scale disks online with minimal block movement.
Only the seed for each file object is stored instead of a full metadata index.
A bipartite architecture splits functions between a client group and a server group.
In the server cluster, every node retrieves, schedules, and sends its own local data blocks directly to the requesting client, rather than forwarding data through a master node.
The Real-Time Streaming Protocol (RTSP) module handles control signaling, and can run on any server node for load balancing.
A distributed file system provides a uniform view of media data across nodes.
At the client side, a circular buffer reassembles media streams.
Instead of using a bursty pause and resume technique, the client sends speedup and slowdown messages to fine-tune the interpacket delivery time based on buffer watermarks.
Yima supports selective retransmission of missing UDP packets by unicasting requests directly to the server node that holds the missing block, calculating the responsible node from node-specific sequence numbers.

## Evaluation

The system was tested with up to an eight-way cluster of PCs connected via Fast Ethernet.
Performance in Yima-2 exhibited almost perfectly linear scalability in the number of supported streams as the number of nodes increased, overcoming the early plateau seen in the Yima-1 master-slave architecture.
The server successfully streamed high-definition video at 45 Mbps and 10.2-channel surround audio at 11 Mbps synchronously across a wide-area network from Virginia and California to a single client in Los Angeles.

## Limitations and critiques

UDP transmission over the public Internet can suffer from high packet loss and jitter, meaning Yima's performance is sensitive to the path between client and server despite selective retransmissions.
Since it relies on randomized placement, worst-case disk load could theoretically skew during pathological short-term intervals, unlike strict cycle-based round-robin which guarantees a perfect bound.
The unicast retransmission scheme requires the client to understand the server cluster's block distribution logic to some degree.

## What it led to

Yima demonstrated that commodity PC clusters could rival proprietary multimedia servers by cleverly structuring random placement and network flow control.
It paved the way for scalable edge-streaming nodes and content delivery networks that rely on distributed, scale-out architectures rather than monolithic servers.
The Scaddar random placement and online scaling techniques became useful references for distributed storage systems facing dynamic scaling requirements.

## Exam angles

<details>
<summary>What are the distinct disadvantages of using a master-slave architecture for a continuous media server, and how does Yima's bipartite design resolve them?</summary>

Master-slave routing creates a single point of failure and a network bottleneck at the master node, as all media traffic passes through it.
Yima's bipartite design has every storage node directly send its blocks to the client, eliminating the middleman and greatly reducing internode traffic.
</details>

<details>
<summary>Why does Yima choose pseudorandom data placement over round-robin placement?</summary>

Round-robin placement distributes load perfectly but requires massive data reorganization when scaling up or down by adding or removing a disk.
Random placement handles multiple delivery rates seamlessly and allows scaling by moving only a small fraction of blocks.
Using a pseudorandom sequence avoids storing massive centralized metadata indices.
</details>

<details>
<summary>How does Yima achieve smooth flow control for variable bit-rate media without generating bursty traffic?</summary>

Instead of a strict pause and resume mechanism which creates bursty on-off traffic, the client tracks buffer watermarks.
The client sends fine-grained speedup and slowdown commands to adjust the server's interpacket delivery time dynamically.
</details>

## Related

- Lessons: [L10a](../Part-5-Internet-Scale-Real-Time-and-Security/L10a-TS-Linux.md), [L10b](../Part-5-Internet-Scale-Real-Time-and-Security/L10b-Persistent-Temporal-Streams.md)

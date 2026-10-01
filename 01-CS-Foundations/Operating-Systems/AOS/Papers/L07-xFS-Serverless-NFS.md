---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: ["https://doi.org/10.1145/225535.225537"]
course: cs6210
lesson: L07
reading: required
venue: "TOCS 1996"
authors: ["Thomas E. Anderson", "Michael D. Dahlin", "Jeanna M. Neefe", "David A. Patterson", "Drew S. Roselli", "Randolph Y. Wang"]
tags: [cs6210, cs6210/paper]
aliases: ["Serverless Network File Systems"]
---

# Serverless Network File Systems

TOCS 1996. Reading status: required. [Link](https://doi.org/10.1145/225535.225537).

> [!abstract] One-line summary
> xFS distributes storage, caching, and control across cooperating workstations to provide a highly scalable and available serverless network file system.

## Problem

Traditional network file systems rely on a centralized server to manage metadata, satisfy cache misses, and store all data.
This central server becomes a severe performance and reliability bottleneck as I/O demands increase and the cluster scales.
Upgrading the central server with specialized multiprocessor hardware is expensive and does not fundamentally solve the scaling limits.
Furthermore, a single central server represents a single point of failure that can halt the entire network if it crashes.

## Key idea

xFS completely decentralizes file system services by treating cooperating workstations as peers, a paradigm shift termed a serverless network file system.
It dynamically distributes control processing and metadata management across the network on a per-file basis using a globally replicated manager map.
To provide high-performance and scalable storage, xFS utilizes log-based network striping over subsets of storage servers called stripe groups, effectively implementing a software RAID.
Additionally, it leverages cooperative caching to aggregate client memory into a massive global file cache, significantly reducing disk accesses and bypassing central server bottlenecks.

## Design

The architecture is built on an "anything, anywhere" philosophy where data, metadata, and control can reside on any machine and migrate dynamically.
A globally replicated manager map directs clients to the specific manager responsible for a given file's cache consistency and disk location metadata.
Each manager maintains a portion of the system-wide imap, tracking the on-disk locations of the index nodes for its assigned files.
Clients write data into local logs and stripe these logs across storage servers using parity for fault tolerance.
A distributed log cleaner reclaims free disk space by coalescing live data from partially empty segments into new, contiguous segments.

## Evaluation

The authors built an xFS prototype and evaluated it on a 32-node cluster of SPARCStation workstations connected by a Myrinet network.
The system demonstrated excellent scalability, showing that in a 32-node system with 32 active clients, each client received nearly as much read and write throughput as it would alone.
Specifically, xFS sustained an aggregate write bandwidth of roughly 14 megabytes per second across 32 clients, effectively matching the underlying network capabilities.

## Limitations and critiques

The serverless design assumes a secure and trusted environment where all machines trust one another's kernels to enforce access control.
This trust model makes xFS unsuitable for deployment over untrusted wide-area networks or the public internet without an additional secure gateway protocol.
Additionally, distributed log cleaning can introduce significant performance variability and overhead under heavy write workloads.

## What it led to

xFS proved that completely decentralized metadata management and storage could deliver robust performance in local area networks.
The project heavily influenced the development of peer-to-peer storage systems and modern clustered file systems.
Its integration of cooperative caching and log-based network striping set a standard for distributed storage research.

## Exam angles

<details>
<summary>How does xFS distribute metadata management without relying on a central server?</summary>
xFS uses a globally replicated manager map that maps a file's index number to a specific manager machine.
This mapping allows clients to locate the appropriate manager directly without funneling requests through a centralized metadata server.
</details>

<details>
<summary>Why does xFS use stripe groups instead of striping across all storage servers?</summary>
Stripe groups improve write efficiency by ensuring log segments are not fractured into tiny fragments across too many servers.
They also improve availability because multiple server failures can be tolerated if they occur in different stripe groups.
</details>

<details>
<summary>What is the role of cooperative caching in xFS?</summary>
Cooperative caching allows xFS to utilize the memory of all client workstations as a massive global cache.
When a client experiences a cache miss, the manager can forward the request to another client that currently holds the data in its memory, avoiding a slow disk access.
</details>

## Related

- Lessons: [L07a](../Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md), [L07b](../Part-4-Distributed-Subsystems-and-Recovery/L07b-Distributed-Shared-Memory.md), [L07c](../Part-4-Distributed-Subsystems-and-Recovery/L07c-Distributed-File-Systems.md)

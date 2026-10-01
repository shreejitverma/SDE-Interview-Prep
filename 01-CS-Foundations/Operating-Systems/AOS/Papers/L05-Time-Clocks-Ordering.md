---
type: paper
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: ["https://doi.org/10.1145/359545.359563"]
course: cs6210
lesson: L05
reading: required
venue: "CACM 1978"
authors: ["Leslie Lamport"]
tags: [cs6210, cs6210/paper]
aliases: ["Time, Clocks, and the Ordering of Events in a Distributed System"]
---

# Time, Clocks, and the Ordering of Events in a Distributed System

CACM 1978. Reading status: required. [Link](https://doi.org/10.1145/359545.359563).

> [!abstract] One-line summary
> Defines a partial ordering of events in distributed systems and introduces logical clocks to create a consistent total ordering.

## Problem

In distributed systems, spatial separation and independent execution make it impossible to rely on physical time to determine the order of events.
Without a consistent notion of time, coordinating state or agreeing on the sequence of operations becomes extremely difficult, leading to anomalous behavior in distributed applications.

## Key idea

The paper formalizes the "happened before" relation to establish a partial ordering of events based on causality (local execution order and message passing).
It introduces logical clocks (Lamport clocks) where each process maintains a counter, incremented between events and piggybacked on messages, allowing the system to derive a consistent total ordering of all events by breaking ties with process IDs.
This total ordering enables distributed synchronization, such as mutual exclusion, without central coordination.

## Design

- Happened-before relation (->): $a \rightarrow b$ if they are in the same process and $a$ precedes $b$, or if $a$ is the sending of a message and $b$ is the receipt of that message, or transitively.
- Logical Clocks: A counter $C_i$ at each process $P_i$.
Rule 1: Increment $C_i$ between successive events.
Rule 2: Piggyback $C_i(a)$ on messages; receiver $P_j$ sets $C_j = \max(C_j, T_m) + 1$.
- Total ordering (=>): $a \Rightarrow b$ if $C_i(a) < C_j(b)$ or ($C_i(a) = C_j(b)$ and $P_i < P_j$).
- Distributed Mutual Exclusion: Uses a request queue at each process, ordered by the total ordering.
A process gets the resource when its request is the earliest in the queue and it has received messages from all other processes with later timestamps.

## Evaluation

- The paper is primarily theoretical and foundational; it does not provide empirical performance evaluation or simulation numbers.
- It proves the correctness of the distributed mutual exclusion algorithm logically.
- It also establishes bounds on physical clock synchronization, showing that physical clocks can be kept synchronized within a bound proportional to the network diameter and unpredictable message delay.

## Limitations and critiques

- Logical clocks do not capture causality perfectly: $C(a) < C(b)$ does not imply $a \rightarrow b$ (later addressed by vector clocks).
- The mutual exclusion algorithm requires active participation from all processes, meaning a single node failure halts the entire system.
- It assumes a fully connected network and reliable, in-order message delivery.

## What it led to

- Formed the foundation of distributed computing theory.
- Inspired vector clocks (Fidge, Mattern) which capture true causality.
- Influenced state machine replication and consensus protocols (Paxos, Raft).
- Led to practical logical time mechanisms in distributed databases (e.g., Spanner's use of TrueTime is a physical answer to this logical problem).

## Exam angles

<details>
<summary>Why does the paper argue that physical clocks are insufficient for ordering events in a distributed system?</summary>
Physical clocks cannot be perfectly synchronized across distributed nodes.
Because message transmission delays are variable and non-negligible compared to event execution times, relying on physical clocks can result in inconsistencies where an event that causally affects another might be assigned a later timestamp.
</details>

<details>
<summary>Explain why the Lamport clock condition $C(a) < C(b)$ does not necessarily mean that event $a$ happened before event $b$.</summary>
Lamport clocks only guarantee the forward implication: if $a \rightarrow b$, then $C(a) < C(b)$.
If $a$ and $b$ are concurrent, they might still be assigned ordered timestamps (e.g., $C(a) < C(b)$) depending on the arbitrary advancement of independent local clocks, so one cannot infer causality solely from the timestamps.
</details>

<details>
<summary>What is the primary vulnerability of the distributed mutual exclusion algorithm presented in the paper?</summary>
The algorithm requires a process to receive a message from every other process to confirm its request is the oldest.
Consequently, if a single process crashes or a network partition occurs, the entire system halts because processes can never satisfy the condition to enter the critical section.
</details>

## Related

- Lessons: [L05a](../Part-3-Distributed-Systems/L05a-Distributed-Systems-Definitions.md), [L05b](../Part-3-Distributed-Systems/L05b-Lamport-Clocks.md), [L05c](../Part-3-Distributed-Systems/L05c-Latency-Limits.md), [L05d](../Part-3-Distributed-Systems/L05d-Active-Networks.md), [L05e](../Part-3-Distributed-Systems/L05e-Systems-from-Components.md)


---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: []
course: cs6210
tags: [cs6210, cs6210/cheatsheet]
---

# Part 5 Cheat Sheet: Internet-Scale, Real-Time, and Security

## Giant-Scale Services and the DQ Principle

Capacity planning for data-intensive internet services relies on Brewer's DQ Principle.

> **$D \times Q = \text{constant}$**
> - **$D$ (Data per query)**: The amount of data examined or moved per query.
> - **$Q$ (Queries per second)**: The throughput rate.
> - The product is bounded by physical bottlenecks like total disk bandwidth or network capacity.
> - To increase $Q$ by a factor, $D$ must be reduced by the inverse factor.

### Availability Metrics

- **Yield**: The fraction of offered queries that are completed (reflects user experience).
- **Harvest**: The fraction of the total data corpus reflected in the returned answer.
- **Uptime**: $MTBF / (MTBF + MTTR)$.

### Faults in a Replica Group

For a replica group of $n$ nodes experiencing $k$ failures:
- **Lost capacity**: $k / n$
- **Redirected load on each survivor**: $k / (n - k)$
- **Overload factor**: $n / (n - k)$

### Replication versus Partitioning

| Architecture | Harvest Impact | Yield Impact | Fault Trade-off |
| :--- | :--- | :--- | :--- |
| **Replicated** | Holds at 1.00 (all data exists on survivor) | Falls (unless spare DQ absorbs redirected load) | Spends a fault on yield. |
| **Partitioned** | Falls (part of the data is lost) | Holds at 1.00 (all queries are still answered) | Spends a fault on harvest. |

*Note: In a saturated system, losing half the nodes results in $0.50$ of the original DQ left in both cases.*

### Online Evolution and Upgrades

An upgrade is a controlled fault that intentionally removes DQ.
Assuming time $u$ to upgrade one node and $n$ total nodes, all three strategies lose the same total DQ-time ($DQ \times u$), but differ in shape.

| Upgrade Type | Wall-Clock Time | Peak DQ Left | Concurrent Versions | Typical Use Case |
| :--- | :--- | :--- | :--- | :--- |
| **Fast reboot** | $u$ | 0 | One | Off-peak, fully automated. |
| **Rolling upgrade** | $n \times u$ | $(n - 1) / n$ | Two | Ordinary software pushes; smallest peak hole. |
| **Big flip** | $2 \times u$ | $1/2$ | One | Schema, physical moves, or non-interoperable changes. |

## Content Delivery Networks (CDNs)

Distributed Hash Tables (DHTs) decentralize content location by mapping both keys (content hashes) and nodes to the same cryptographic identifier space (e.g., SHA-1).

| Feature | Traditional Greedy DHT | Coral Sloppy DHT | Dynamo |
| :--- | :--- | :--- | :--- |
| **Primary Goal** | Minimize lookup hops | Avoid tree saturation and hot spots | Always writeable high availability |
| **Routing Algorithm** | Jump to numerically closest ID | Distance halving (slow progression) | Zero-hop (full routing table at each node) |
| **Storage Placement** | Exactly at the closest node | Intermediate nodes (if path is congested) | Coordinator and N-1 successors on the ring |
| **Consistency** | Strong | Soft-state (multiple cached copies) | Eventual (Vector clocks, hinted handoffs) |

## MapReduce

A functional programming model for embarrassingly parallel big data processing.
- **Map**: `map(k1, v1) -> list(k2, v2)`
- **Reduce**: `reduce(k2, list(v2)) -> list(v2)`

The runtime abstracts away distributed systems complexity.
It automatically handles input splitting, worker task assignment, network shuffling, and re-executing backup tasks for stragglers.

## Time-Sensitive Linux (TSL) and Streams

Kernel latency originates from three sources: timer resolution, preemption latency, and scheduling latency.

### Firm Timers

Firm timers combine periodic, soft, and one-shot timers using an **overshoot window**.
The one-shot hardware timer is deliberately programmed to fire slightly after the actual deadline.
If the kernel naturally encounters a soft timer check during this overshoot window, it processes the timer without incurring the overhead of an asynchronous interrupt.

### Persistent Temporal Streams (PTS) and Yima

- **PTS Model**: Abstracts continuous media into channels of time-indexed items (`put(item, t)`, `get(t)`).
It provides built-in windowed persistence and automatic garbage collection without requiring application-level buffering.
- **SCADDAR Placement**: Yima uses a pseudorandom data block placement algorithm seeded by file ID.
Adding a new disk to a cluster of $N$ disks requires moving exactly $1 / (N+1)$ of the data.
It maintains perfect probabilistic load balancing without a centralized metadata directory.

## Principles of Information Security

- **Privacy**: The socially defined policy of determining when and to whom data is released.
- **Security**: The operational mechanisms (locks, encryption, policies) used to achieve privacy.
- **Protection**: The architectural and OS mechanisms that constrain executing programs.

Security violations fall into three categories: unauthorized release, unauthorized modification, and unauthorized denial of use.

### Eight Design Principles (Saltzer and Schroeder)

1. **Economy of mechanism**: Keep the design simple and small.
2. **Fail-safe defaults**: Base access decisions on permission rather than exclusion.
3. **Complete mediation**: Every access to every object must be checked for authority.
4. **Open design**: Security should not depend on the ignorance of attackers.
5. **Separation of privilege**: Require multiple independent conditions for access.
6. **Least privilege**: Programs and users operate with the minimum necessary rights.
7. **Least common mechanism**: Minimize mechanisms shared by all users.
8. **Psychological acceptability**: Mechanisms must be intuitive and easy to use.

### Capabilities versus Access Control Lists (ACLs)

| Feature | Capabilities (Tickets) | Access Control Lists (Locks) |
| :--- | :--- | :--- |
| **Definition** | Unforgeable token held by the subject. | List of permissions attached to the object. |
| **Revocation** | Hard (must track down or invalidate all tickets). | Easy (remove the user from the object's list). |
| **Delegation** | Easy (just hand a copy of the ticket). | Hard (must update the central ACL). |
| **Auditing** | Hard ("Who has access?"). | Easy ("Who has access?"). |


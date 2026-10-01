---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L05a, L05b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-10-clocks-and-mutex: Lamport and vector clocks, total order, and Lamport mutual exclusion

> [!info] Goal
> Make L05a, L05b concrete with Lamport clock simulators, vector clocks, and Lamport mutual exclusion.

See [setup](../setup/README.md) for the VM.

## Concepts Exercised
- Logical Clocks: Lamport scalar clocks and vector clocks.
- Distributed State: Capturing causality and concurrency.
- Distributed Algorithms: Lamport's Mutual Exclusion algorithm using 3(N-1) messages.
- Clock Drift: The divergence of physical clocks in a distributed system.

## Prerequisites
- Basic understanding of POSIX threads (pthreads).
- Familiarity with the concepts from L05a and L05b.

## Run Commands
To build and run the simulation, execute the following from the lab directory within the `aos` VM:
```bash
make clean
make test
```
To run the simulations individually:
```bash
make clocks
./clocks
make mutex
./mutex
```

## What You Should See
For the clocks simulation (`./clocks`), you will see the progression of Lamport and Vector clocks alongside simulated physical clocks with drift:
```
Node 0 LOCAL      | Lamport:  1 | Vector: [1, 0, 0] | Physical (drifted):  1.00
Node 0 SEND       | Lamport:  2 | Vector: [2, 0, 0] | Physical (drifted):  2.00
Node 1 RECV       | Lamport:  3 | Vector: [2, 1, 0] | Physical (drifted):  3.15
```
Notice how `Node 1`'s physical clock drifts by 5% (`3.00` real time becomes `3.15`).

For the mutual exclusion simulation (`./mutex`), you will see nodes entering the Critical Section strictly ordered by their Lamport timestamps:
```
Node 0 ENTERING CS (Iter 0) with Lamport 6
Node 0 LEAVING CS (Iter 0)
Node 1 ENTERING CS (Iter 0) with Lamport 8
Node 1 LEAVING CS (Iter 0)
Node 2 ENTERING CS (Iter 0) with Lamport 10
...
Total Messages EXCHANGED: 36
Expected Messages (3*(N-1) per CS): 36
```

## How It Works
1. **Clock Simulator (`clocks.c`)**:
   - Each node maintains a scalar Lamport clock and an array for the Vector clock.
   - Physical clocks are simulated with a simple formula `physical_time = real_time * (1 + drift_rate)`.
   - On a local event or message send, the node increments its Lamport clock and its own index in the Vector clock.
   - On message receive, it updates its Lamport clock to `max(local, msg_lamport) + 1` and takes the element-wise maximum for the Vector clock.
2. **Mutual Exclusion Simulator (`mutex.c`)**:
   - 3 nodes use `pthread`s to simulate a distributed system communicating over message queues.
   - Nodes use Lamport's distributed mutual exclusion algorithm to safely enter a critical section.
   - Requests are broadcast with logical timestamps and queued locally at each node, sorted by timestamp (with Node ID breaking ties).
   - A node enters the CS when its own request is at the top of its queue and it has received a message with a timestamp larger than its request from every other node.
   - Releasing the CS broadcasts a release message, removing the request from all queues.

## Experiments

1. **Modify Clock Drift Rates**
   - **Action**: In `clocks.c`, change `nodes[i].drift_rate` to extreme values (e.g., `-0.5` or `2.0`).
   - **Prediction**: What happens to the causal ordering of physical clocks compared to logical clocks?

2. **Increase Node Count**
   - **Action**: In `mutex.c`, change `NUM_NODES` to `5` and `NUM_CS_ENTRIES` to `5`.
   - **Prediction**: How many total messages will be exchanged? Use the formula $3 \times (N-1) \times N \times \text{ENTRIES}$.

3. **Induce a Tie in Timestamps**
   - **Action**: In `mutex.c`, force two nodes to request the CS at the exact same Lamport clock value.
   - **Prediction**: How will the algorithm determine which node enters first? Look at the tie-breaking logic in `add_request`.

## Questions

<details>
<summary>Why do Lamport clocks provide a total ordering of events, but not necessarily a causal ordering?</summary>
Lamport clocks ensure that if event $A$ happens before $B$ ($A \rightarrow B$), then $L(A) < L(B)$. However, the converse is not true: $L(A) < L(B)$ does not imply $A \rightarrow B$. Two concurrent events can have arbitrary Lamport clock values depending on the tie-breaking mechanism (like process ID).
</details>

<details>
<summary>How do Vector clocks solve the causal ordering ambiguity of Lamport clocks?</summary>
Vector clocks maintain an array of counters, one for each process. By capturing the knowledge each process has about others, we can precisely determine concurrency. Specifically, $A \rightarrow B$ if and only if $V(A) < V(B)$ (meaning every element of $V(A)$ is $\le$ the corresponding element of $V(B)$, and at least one is strictly less). If neither $V(A) < V(B)$ nor $V(B) < V(A)$, the events are concurrent.
</details>

<details>
<summary>Why does Lamport's Mutual Exclusion algorithm require exactly $3(N-1)$ messages per critical section entry?</summary>
For each critical section entry, a node must:
1. Send a `REQUEST` to the other $N-1$ nodes.
2. Receive a `REPLY` from the other $N-1$ nodes.
3. Send a `RELEASE` to the other $N-1$ nodes upon exiting.
Summing these up yields $3(N-1)$ messages per request.
</details>

<details>
<summary>In the Lamport Mutual Exclusion algorithm, why must a node receive a message with a timestamp greater than its request from every other node before entering the CS?</summary>
This condition guarantees that no other node has an outstanding request with an earlier (smaller) timestamp. Because messages from any given node arrive in order, receiving a message with timestamp $T_{msg} > T_{req}$ from node $J$ means that any future request from node $J$ will have a timestamp even larger than $T_{msg}$, and thus larger than $T_{req}$.
</details>

---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L09c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-21-dht: DHTs: consistent hashing, key-based routing, Coral sloppy DHT, Dynamo quorums

> [!info] Goal
> Make L09c concrete with real commands and measurements.

See [setup](../setup/README.md) for the VM.
This lab uses Python simulators to demonstrate key distributed system mechanics.
We explore consistent hashing load balancing, Chord routing bounds, Coral hot-spot relief, and Dynamo quorum conflict resolution.

## Prerequisites
- A running `aos` Lima VM.
- Python 3.12 available at `/opt/aos-venv/bin/python`.
- Basic understanding of distributed hash tables and vector clocks.

## Run Commands
Run the simulation scripts via the provided Makefile.
```bash
cd 01-CS-Foundations/Operating-Systems/AOS/labs/lab-21-dht
make run
```
To verify the scripts complete without errors, run:
```bash
make test
```

## What You Should See
The consistent hashing simulation shows the coefficient of variation (CV) drop drastically as virtual nodes increase.
With 1 virtual node, the CV is around 100.1%.
With 100 virtual nodes, the CV drops to 7.9%, demonstrating much tighter load balancing.
The Chord routing simulation shows an average of 4.23 hops for a 100-node network in a 16-bit identifier space.
This matches the expected logarithmic bound.
The Coral sloppy DHT simulation outputs a tree path trace.
As the root node reaches its capacity of 2 items, subsequent inserts spill over to children (like Node 2 or Node 3) and grandchildren (like Node 7).
The Dynamo quorums simulation outputs a sequence of reads and writes.
A simulated network partition causes concurrent writes from Node 0 and Node 2.
The final read correctly detects a conflict and returns both sibling versions: `item1,item2,item3A` with clock `{0: 3}` and `item1,item2,item3B` with clock `{0: 2, 2: 1}`.

## How It Works
The consistent hashing simulator (`consistent_hashing.py`) maps keys and virtual nodes to a 128-bit MD5 ring.
It measures the standard deviation of key counts assigned to each physical node.
The Chord simulator (`chord_routing.py`) builds finger tables for each node based on the power-of-two offset rule.
It recursively resolves the closest preceding node to find the target key.
The Coral simulator (`coral_sloppy.py`) models a binary tree where each node has a fixed storage capacity.
During an insert, the algorithm traverses from a leaf toward the root and stores the value at the closest non-full node, preventing the root from becoming a hotspot.
The Dynamo simulator (`dynamo_quorums.py`) implements a partial replication scheme with N=3, W=2, and R=2.
It uses vector clocks to track causality.
When read responses contain concurrent, non-dominated vector clocks, it preserves all siblings for the client to resolve.

## Experiments to Try

1. **Change Virtual Node Counts**
   Modify `consistent_hashing.py` to use 1000 virtual nodes per physical node.
   *Prediction:* The coefficient of variation will drop even further, approaching 0%, but the memory overhead for the ring will increase.

2. **Increase Chord Network Size**
   Change `num_nodes` in `chord_routing.py` from 100 to 10000.
   *Prediction:* The average hops will increase from roughly 4 to about 7, staying well within the $O(\log N)$ bound.

3. **Adjust Dynamo Quorum Sizes**
   Change the system in `dynamo_quorums.py` to use R=1 and W=1.
   *Prediction:* The final read will only query one node and may miss the concurrent update, returning a single, potentially stale version instead of a conflict.

## Questions

<details>
<summary>Why does consistent hashing use virtual nodes instead of assigning physical nodes directly to the ring?</summary>
Assigning physical nodes directly to the ring can lead to highly uneven key distribution due to random hash placement.
Virtual nodes allow a single physical machine to occupy multiple pseudo-random positions on the ring.
This evenly spreads the probability space and allows the system to assign more virtual nodes to physical machines with higher capacity.
</details>

<details>
<summary>How does Coral's sloppy DHT design differ from Chord when storing a highly popular key?</summary>
In Chord, a popular key always routes to its exact successor node, which can quickly become overloaded and cause tree-saturation.
Coral relaxes the placement rule.
It stores the data at the closest node to the key that is not yet full, creating a localized cache tree that intercepts requests before they reach the root.
</details>

<details>
<summary>In the Dynamo simulation, why does vector clock `{0: 3}` not dominate `{0: 2, 2: 1}`?</summary>
A vector clock strictly dominates another only if every counter in the first clock is greater than or equal to the corresponding counter in the second clock.
Although `{0: 3}` has a higher count for node 0, it lacks the increment for node 2 present in `{0: 2, 2: 1}`.
Because neither clock has all values greater than or equal to the other, they represent concurrent, divergent histories.
</details>

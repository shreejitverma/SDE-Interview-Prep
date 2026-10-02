---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L09a]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-19-giant-scale: Giant-scale services: DQ, harvest and yield, replication versus partitioning

> [!info] Goal
> Make L09a concrete with real commands and measurements.

See [setup](../setup/README.md) for the VM.

This lab simulates a giant-scale service responding to queries under strict capacity limits. It compares the behavior of a partitioned system versus a replicated system during a node failure, demonstrating the DQ principle (Data per query $\times$ Queries per second = Capacity) and measuring yield versus harvest.

## Prerequisites
- The `aos` VM environment.
- Python 3.12 (standard library `asyncio` is used).

## How it works
The experiment spins up two simulation nodes representing either replicas or partitions. Each node has a hard processing capacity of 5,000 items per second.
- **Replication:** Each node holds all 1,000 data items. A query requires reading 1,000 items from a single node.
- **Partitioning:** Each node holds 500 data items. A query scatters a request for 500 items to *both* nodes and gathers the result.

The client load generator (`client.py`) sends 10 Queries Per Second (QPS) for 2 seconds. The total load on the system is exactly $10 \text{ QPS} \times 1000 \text{ items/query} = 10,000 \text{ items/sec}$. Both architectures gracefully handle this baseline load across the two nodes.

We then simulate a node failure (`kill -9`):
- In the replicated architecture, the load balancer routes all 10 QPS to the single surviving node. The load is 10,000 items/sec, but the node capacity is only 5,000 items/sec. The node becomes overloaded and drops queries. **Yield drops, but harvest remains 100% for successful queries.**
- In the partitioned architecture, the scatter-gather client still requests 500 items from the surviving node and 500 from the dead node. The surviving node only sees 5,000 items/sec of load, which matches its capacity. It processes all queries successfully (returning partial data). **Yield remains 100%, but harvest drops.**

### Round-Robin DNS vs. Layer 4 Load Balancing
In our replicated test, we simulate an ideal Layer 4 (or Layer 7) load balancer that instantly stops routing traffic to the dead node. In reality:
- **Round-Robin DNS** would continue to hand out the IP of the dead node until the TTL expires. Clients connecting to the dead IP would experience connection timeouts, artificially dropping yield even further (or causing high latency tail if they retry).
- **Layer 4 Load Balancers (e.g., LVS, HAProxy, Maglev)** use health checks to detect node failures within seconds. They remove the dead node from the active pool, shifting all load to the survivors (as simulated here), which can cause the cascading overload failure we observe.

## Run Commands
Inside the VM, execute:
```bash
make test
```

## What you should see
You should observe 100% yield and harvest during the baseline. During the degraded test, replicated yield drops (e.g., to ~80.0%), while partitioned harvest drops exactly to 50.0%:

```text
--- Degraded Replicated: Load balancer sends all 10 QPS to remaining node ---
Mode: replicated
Yield: 80.0% (16/20 queries)
Harvest: 80.0% (16000/20000 items)
--- Degraded Partitioned: Scatter-gather to both, Node 1 times out ---
Mode: partitioned
Yield: 100.0% (20/20 queries)
Harvest: 50.0% (10000/20000 items)
```
*(Exact numbers may vary slightly based on scheduling variance, but the trend will be identical).*

## Experiments to try

1. **Vary the node capacity:**
   Change the `5000` argument in `Makefile` to `10000`. Predict what happens to the replicated yield when one node dies.
   > **Prediction:** The single remaining node will now have enough capacity to handle the entire 10,000 items/sec load. Yield will remain 100%.

2. **Increase the QPS:**
   Change the QPS in `Makefile` from `10` to `20`. Predict the baseline behavior for both architectures.
   > **Prediction:** Both architectures will be overloaded even when both nodes are alive. The partitioned system will start dropping queries (Yield drops).

3. **Simulate 4 partitions:**
   Modify `client.py` and `Makefile` to run 4 nodes, each with 2,500 items/sec capacity. Predict the harvest when one node fails.
   > **Prediction:** The harvest will drop to 75%, since 3/4 of the partitions remain available. Yield will remain 100%.

## Questions

<details>
<summary>Why does the harvest in the degraded replicated system remain exactly proportional to the yield, while the degraded partitioned system harvest drops to 50%?</summary>
In a replicated system, a successful query returns the complete dataset (100% harvest). If a query fails due to overload, it returns 0 items. Therefore, the overall harvest percentage exactly tracks the yield percentage. In a partitioned system, every query succeeds but only returns half the data, dropping the harvest but keeping yield at 100%.
</details>

<details>
<summary>How does the DQ principle explain the failure of the degraded replicated system?</summary>
The DQ principle states that Capacity = Data per query $\times$ Queries per second. The surviving replica has a capacity of 5,000 items/sec. The load balancer sends it 10 QPS, and each query demands 1,000 items. The demanded capacity is 10,000 items/sec, which exceeds the node's limit, forcing it to drop queries.
</details>

<details>
<summary>If the partitioned system guarantees 100% yield, why don't all giant-scale systems use partitioning exclusively?</summary>
Partitioning decreases harvest when nodes fail. For some applications (e.g., retrieving a user's exact account balance), partial data (low harvest) is unacceptable or meaningless. Replication guarantees that if a response is returned, it is complete and accurate. Giant-scale systems typically use a combination of both: data is partitioned for scalability, and each partition is replicated for fault tolerance and high harvest.
</details>

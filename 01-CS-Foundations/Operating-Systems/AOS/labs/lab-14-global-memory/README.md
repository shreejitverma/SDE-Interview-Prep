---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L07a]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-14-global-memory: Global memory: cluster LRU with epochs and remote paging costs

> [!info] Goal
> Make L07a concrete with real commands and measurements. Emulate the Global Memory Service (GMS) epoch behavior, MinAge cutoff calculation, and node target selection based on age weights. Contrast local memory fetch latency with a simulated remote fetch over a socket.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Read [L07a Global Memory Systems](../../Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md).
- Understand the difference between the local cache and global cache boundary.

## How it works

This lab provides a C11 simulator that models the GMS (Global Memory Service) distributed caching approach:
1. **Epoch tracking:** Nodes track the age (time since last access) of their pages.
   Periodically, an epoch process calculates a global `MinAge` cutoff.
2. **Min-weight target selection:** Pages older than `MinAge` are counted as "weight".
   A node that faults and needs to place a page into global memory targets the node with the largest weight (i.e., the most idle memory).
3. **Paging cost simulation:** The lab measures the median time for a local 4KB `memcpy` versus fetching 4KB of data across a UNIX domain socket.
   This models the latency difference between accessing local DRAM versus fetching a remote page over the network.

## Run

Run the lab simulation inside the VM:

```bash
../setup/run-in-vm.sh lab-14-global-memory run
```

Or test it directly:

```bash
make clean
make test
```

## What you should see

The output demonstrates the epoch simulation determining `MinAge` and selecting a target node, followed by the latency benchmark. Real numbers from the test run show remote fetch overhead:

```
--- Global Memory Simulator ---
[Epoch] Time: 2000, MinAge: 2495
  Node 0 weight (pages older than MinAge): 0
  Node 1 weight (pages older than MinAge): 1
  Node 2 weight (pages older than MinAge): 0
  Node 3 weight (pages older than MinAge): 50

Node selected to receive global page (largest weight): Node 3
PASS: Found target node with non-zero weight.

[Costs] Local memory copy (4KB): 83 ns
[Costs] Remote socket fetch (4KB): 750 ns
[Costs] Remote is ~9x slower than local
PASS: Cost measurement completed.
```
*Note: Your timing measurements may vary slightly based on host CPU speed and load.*
*Additionally, this VM is configured with a single NUMA node.*
*On real multi-socket hardware, local memory copy costs would vary depending on whether the memory is attached to the local NUMA node or a remote NUMA node, further widening the gap between local, local-NUMA, remote-NUMA, and network-remote memory.*

## Experiments

1. **Alter Node Page Ages:**
   Modify `gms_sim.c` where nodes are initialized with synthetic skew.
   Give Node 0 the oldest pages instead of Node 3.
   *Prediction:* The target selection logic should automatically pivot to selecting Node 0 as the initiator, correctly identifying the cluster's idle memory.
2. **Increase the Socket Payload Size:**
   Change `PAGE_SIZE` to 2MB (huge pages) and observe the cost disparity.
   *Prediction:* The time for both local and remote fetch will increase, but the multiplier might change depending on buffer sizes, context switch overhead, and socket throughput versus pure memory bandwidth.
3. **Adjust the MinAge Percentile:**
   In `trigger_epoch()`, the cutoff index is set to `TOTAL_PAGES / 4` (oldest 25%).
   Change it to `TOTAL_PAGES / 2` (median).
   *Prediction:* `MinAge` will drop, more pages will qualify as "older than MinAge", and the weight distribution among nodes will become less heavily skewed toward the completely idle node.

## Questions

> [!question]- Why does GMS target the node with the *largest* weight for replacement?
> The weight represents the number of pages on that node that are older than the global `MinAge`.
> A larger weight means the node holds more idle or cold pages, making it the best candidate to evict one of its pages to disk (or discard it) and house a global page instead.

> [!question]- How does a ~9x remote fetch cost justify the GMS hierarchy?
> In this lab's cost simulation, fetching a remote 4KB page takes roughly 9x longer than local memory access.
> GMS relies on the premise that fetching a page over the network (represented by the socket) is significantly slower than local memory (~9x in this simulation), but still orders of magnitude faster than a traditional magnetic disk or SSD (which can take tens of microseconds to milliseconds).
> This intermediate latency tier makes "borrowing" remote idle memory attractive.

> [!question]- What happens if the global `MinAge` is calculated to be 0?
> A `MinAge` of 0 indicates extreme memory pressure across the cluster.
> It effectively means "do not forward" because every page is considered hot.
> A node faulting in this scenario will bypass global memory and swap directly to disk.

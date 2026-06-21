# Chapter 100: Kernel-Bypass Networking — DPDK, User-Space Stacks, and RDMA

At the highest packet rates and lowest latencies, even an optimised kernel network stack is too slow: every packet traverses interrupts, the socket layer, protocol processing, copies, and a syscall, costing microseconds the application cannot afford. **Kernel-bypass** networking removes the kernel from the data path entirely — the NIC DMAs packets directly into user-space memory, and the application polls for them. This chapter covers the three pillars (DPDK, user-space stacks, RDMA), the radical cost model that makes them worth their enormous complexity, and the clear-eyed view of when *not* to use them.

## Chapter Roadmap

- 100.1 Why Bypass the Kernel
- 100.2 DPDK: Poll-Mode User-Space Packet Processing
- 100.3 User-Space TCP/IP Stacks
- 100.4 RDMA: Remote Direct Memory Access
- 100.5 The Cost Model and When Not To

---

## 100.1 Why Bypass the Kernel

A packet arriving at a normal Linux socket traverses a long path: a NIC interrupt, the driver, `softirq` processing, the netfilter/routing layers, the TCP/IP stack, a copy into the socket buffer, a wakeup of the blocked thread, and finally a `recv` syscall with another copy into the user buffer. Each step adds latency and per-packet CPU; the total is several microseconds and a hard ceiling of a few million packets/sec/core.

> **Why this matters.** For most applications this is invisible and fine. For a trading system reacting to market data, a 5G base station, or a high-frequency telemetry pipeline, *microseconds per packet* and *interrupt-driven jitter* are unacceptable. Kernel bypass discards the entire path: the NIC is programmed to DMA packets straight into pre-registered user-space buffers (rings), and the application **polls** those rings on a dedicated core (Chapter 96) — no interrupt, no kernel stack, no copy, no syscall. The result is sub-microsecond packet latency and tens of millions of packets/sec/core. The price is that you give up everything the kernel provided — TCP, routing, firewalling, multiplexing — and must supply it yourself.

---

## 100.2 DPDK: Poll-Mode User-Space Packet Processing

**DPDK** (Data Plane Development Kit) is the dominant kernel-bypass framework. It binds the NIC to a user-space **poll-mode driver (PMD)** — removing it from the kernel — and the application reads/writes packets directly from the NIC's descriptor rings.

```cpp
// Min standard: C++11 + DPDK (non-portable; requires hugepages, NIC binding, root). Conceptual.
// rte_eal_init(...);                              // set up hugepage memory pools, lcores
// struct rte_mbuf* bufs[BURST];
// for (;;) {                                       // busy-poll loop on a dedicated lcore
//   uint16_t n = rte_eth_rx_burst(port, queue, bufs, BURST);  // poll NIC ring, NO syscall/interrupt
//   for (uint16_t i = 0; i < n; ++i) process(bufs[i]);        // packet already in user memory
//   rte_eth_tx_burst(port, queue, out, m);                    // transmit directly
// }
```
*Listing 100.1 — DPDK's poll-mode receive loop: no interrupts, no syscalls, packets DMA'd into user memory. Non-portable.*

> **Why this matters / cost model.** DPDK's design choices are a checklist of this volume's disciplines applied to networking: **poll-mode** (no interrupts — Chapter 96's busy-spin), **hugepage-backed mbuf pools** (no TLB misses, no page faults — Chapter 88), **lockless per-core rings** (thread-per-core — Chapter 96), **batched burst APIs** (`rx_burst`/`tx_burst` amortise per-packet overhead), and **NUMA-aware buffer pools** (local memory — Chapter 88). Each `rx_burst` pulls up to N packets already sitting in user memory — no copy, no crossing. The cost is total: the NIC is *removed from the kernel*, so the OS cannot use it; you need hugepages, root/privileged setup, a dedicated polled core burning 100% CPU, and you must implement or import every protocol layer yourself. DPDK is for dedicated packet-processing appliances and the data path of latency-critical systems, not general servers.

---

## 100.3 User-Space TCP/IP Stacks

Bypassing the kernel means losing its TCP/IP stack, so kernel-bypass systems run a **user-space stack** (mTCP, F-Stack, Seastar's native stack, VMA, Onload) on top of DPDK or a similar raw-packet interface. These reimplement TCP/IP entirely in user space, integrated with the application's polling loop and thread-per-core model.

> **Why this matters / cost model.** The kernel TCP stack is general and robust but pays for it: locks, generic buffer management, the socket abstraction, and the syscall boundary on every send/recv. A user-space stack co-designed with the application can be radically faster — zero-copy into application buffers, per-core connection tables with no locking, batched processing, and no syscalls — at the cost of reimplementing one of the most subtle pieces of systems software (congestion control, retransmission, the myriad TCP edge cases) and losing the kernel's decades of hardening. Solutions like Solarflare's **Onload** and Mellanox **VMA** offer a middle path: a `LD_PRELOAD` shim that transparently routes a normal sockets application through a user-space accelerated stack, capturing much of the benefit without an application rewrite. Full custom stacks (Seastar) are for systems built from the ground up around thread-per-core.

---

## 100.4 RDMA: Remote Direct Memory Access

**RDMA** (Remote Direct Memory Access) goes further: it lets one machine read or write another machine's memory *directly*, with the remote CPU not involved at all. The NICs (InfiniBand, or RoCE/iWARP over Ethernet) implement the transport in hardware; the application posts work requests to **queue pairs** and the hardware does the transfer.

```cpp
// Min standard: C++11 + libibverbs (non-portable). Conceptual one-sided RDMA write.
// Register memory regions on both sides (pinned, hardware-accessible).
// ibv_post_send(qp, &wr);   // RDMA WRITE: hardware writes directly into the REMOTE machine's
//                           // registered memory. The remote CPU is NOT involved (one-sided).
// Poll the completion queue for completion — no remote syscall, no remote app code runs.
```
*Listing 100.2 — One-sided RDMA write: the remote CPU is bypassed entirely. Non-portable (libibverbs/InfiniBand/RoCE).*

> **Why this matters / cost model.** RDMA's defining feature is **one-sided operations**: an RDMA `READ`/`WRITE` accesses remote memory without the remote CPU executing a single instruction or being interrupted — the remote NIC's hardware performs the memory access. This collapses remote-memory latency to ~1–2 μs (vs tens of μs for kernel TCP) and offloads the transport entirely to hardware, freeing both CPUs. It is the backbone of HPC interconnects, distributed databases, and disaggregated-memory systems. The costs and constraints are severe: memory must be **registered/pinned** in advance (the NIC needs stable physical addresses — Chapter 88), it requires special hardware (InfiniBand or RoCE-capable NICs and switches with lossless fabric configuration), the programming model (verbs, queue pairs, completion queues) is low-level and unforgiving, and the consistency/ordering semantics of one-sided operations are subtle. RDMA is for tightly-controlled datacenter fabrics where the latency justifies the hardware and complexity.

---

## 100.5 The Cost Model and When Not To

| Approach | Latency | What you give up |
|---|---|---|
| Kernel sockets + `epoll` | ~tens of μs | Nothing — full kernel services |
| Kernel sockets + `io_uring` | ~µs, amortised | Little — still kernel stack |
| DPDK + user-space stack | sub-µs, ~10s Mpps/core | Kernel networking; a dedicated polled core; must DIY protocols |
| RDMA | ~1–2 µs remote | General networking; needs special hardware; pinned memory |

> **Why this matters.** Kernel bypass is the most extreme tool in the volume and the easiest to over-reach for. Its costs are not incremental — they are categorical: a *dedicated core burning 100% CPU* polling, *special hardware* (RDMA) or *exclusive NIC ownership* (DPDK), *root/privileged configuration* (hugepages, NIC binding, lossless fabric), *the loss of all kernel network services* (you reimplement or import TCP, routing, firewalling, monitoring), and a *far higher engineering and operational burden*. It is justified only when the application's latency or packet-rate requirements genuinely exceed what the kernel — even with `io_uring`, busy-polling sockets (`SO_BUSY_POLL`), and tuning — can deliver. Many teams reach for DPDK when a well-tuned `io_uring` or `AF_XDP` (a lighter-weight, partial kernel-bypass that keeps more kernel integration) would have sufficed at a fraction of the complexity.

> **The discipline.** Kernel bypass is the logical endpoint of the OS-boundary cost model: Chapter 98 said the boundary is expensive, Chapter 99 amortised the crossings, and this chapter *removes the kernel entirely* for the data path. But it inverts the usual trade — you take on the kernel's enormous responsibilities (protocol correctness, security, multiplexing) in exchange for latency, and that is only a good trade for a small set of latency-critical, high-rate systems running on dedicated hardware. Reach for it last, after `io_uring` and socket tuning are exhausted, and after measurement proves the kernel path is the bottleneck. For the latency these systems chase, the next chapter provides the other essential: measuring time itself correctly.

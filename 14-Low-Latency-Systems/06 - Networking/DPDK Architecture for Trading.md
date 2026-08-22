---
tags: [trading/networking, trading/kernel-bypass, type/concept]
aliases: [DPDK, Data Plane Development Kit, Poll Mode Driver, PMD, rte_mbuf, rte_mempool, rte_ring, User-Space Networking]
status: evergreen
module: 06
created: 2026-08-22
---

> [!summary]
> The Data Plane Development Kit (DPDK) is an open-source, vendor-neutral framework for ultra-high-throughput user-space packet processing. By combining Poll Mode Drivers (PMDs), 1GB/2MB HugeTLB memory pools (`rte_mempool`), lock-free packet queues (`rte_ring`), and CPU core isolation, DPDK processes tens of millions of packets per second with sub-microsecond determinism across Intel, Mellanox, and Broadcom NICs.

---

## Why it matters
While Solarflare's `ef_vi` offers the lowest absolute latency on Solarflare hardware, large multi-asset trading firms operate across diverse heterogeneous environments:
- Proprietary multi-NIC colocation deployments (Mellanox ConnectX-5/6/7, Intel E810).
- Cloud simulation environments (AWS ENA / Azure MANA).
- Cross-market feed handlers spanning equities, FX, and crypto venues.

DPDK provides a **universal, vendor-agnostic kernel-bypass programming model** that achieves **350–600ns packet processing** while eliminating vendor lock-in.

```mermaid
flowchart TD
    subgraph NIC_Layer ["Physical NIC Hardware (Mellanox / Intel / Broadcom)"]
        NIC["10G / 25G / 100G PCIe NIC (Bound to vfio-pci)"]
    end

    subgraph DPDK_EAL ["DPDK Environment Abstraction Layer (EAL)"]
        MEMPOOL["rte_mempool (Pre-Allocated 1GB HugePages)"]
        PMD["Poll Mode Driver (PMD: Direct RX/TX Ring Polling)"]
    end

    subgraph ApplicationWorkers ["Pinned DPDK Worker lcores"]
        LCORE1["lcore 2: Feed Handler (rte_eth_rx_burst)"]
        RING["rte_ring (Lock-Free Ring Buffer)"]
        LCORE2["lcore 4: Strategy Execution Core"]
        
        LCORE1 -->|Zero-Copy rte_mbuf| RING
        RING --> LCORE2
    end

    NIC ==>|Direct PCIe DMA into HugePages| MEMPOOL
    PMD <==|Spin Poll (Zero Syscalls)| LCORE1
```

---

## Mechanism

### 1. The Core DPDK Building Blocks
- **Environment Abstraction Layer (EAL)**: Initializes physical memory, scans the PCIe bus, binds NICs via `vfio-pci` (using IOMMU for hardware memory protection), and pins execution threads to dedicated logical CPU cores (`lcores`).
- **Poll Mode Drivers (PMD)**: Pure user-space drivers that continuously poll the NIC hardware RX/TX descriptor rings without generating hardware interrupts.
- **Memory Pools (`rte_mempool`)**: Fixed-size memory pools allocated in HugePages. Each lcore maintains a thread-local cache (`rte_mempool_cache`) to allocate and free packet structures with **zero atomic locks or cross-core synchronization**.
- **Packet Structure (`rte_mbuf`)**: A cache-aligned structure containing packet metadata, headroom, data pointers, and hardware offload flags.
- **Lock-Free Queue (`rte_ring`)**: A high-performance, cache-padded, bounded FIFO queue supporting Single/Multi-Producer and Single/Multi-Consumer semantics.

### 2. Burst-Oriented Processing (`rte_eth_rx_burst`)
DPDK processes packets in **bursts** (e.g. 8, 16, or 32 packets at a time):
- Under high volatility, a single call to `rte_eth_rx_burst()` fetches up to 32 packet pointers from the NIC descriptor ring in a single operation.
- Software processes the entire batch in a tight L1-cache loop, amortizing descriptor update overhead across all 32 packets.

---

## In Practice

### High-Throughput DPDK Market Data Polling Engine in C++20

```cpp
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <iostream>
#include <stdexcept>

constexpr uint16_t PORT_ID = 0;
constexpr uint16_t RX_RINGS = 1;
constexpr uint16_t TX_RINGS = 1;
constexpr uint16_t NUM_MBUFS = 8191;
constexpr uint16_t MBUF_CACHE_SIZE = 250;
constexpr uint16_t BURST_SIZE = 32;

class DpdkFeedHandler {
private:
    struct rte_mempool* mbuf_pool_{nullptr};

public:
    void init(int argc, char** argv) {
        // 1. Initialize DPDK Environment Abstraction Layer (EAL)
        int ret = rte_eal_init(argc, argv);
        if (ret < 0) throw std::runtime_error("rte_eal_init failed");

        // 2. Create HugePage-backed Memory Pool for Packet Buffers
        mbuf_pool_ = rte_pktmbuf_pool_create("TRADING_MBUF_POOL", NUM_MBUFS,
                                             MBUF_CACHE_SIZE, 0,
                                             RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
        if (!mbuf_pool_) throw std::runtime_error("Cannot create mbuf pool");

        // 3. Configure Ethernet Device Port
        struct rte_eth_conf port_conf{};
        port_conf.rxmode.max_lro_pkt_size = RTE_ETHER_MAX_LEN;

        if (rte_eth_dev_configure(PORT_ID, RX_RINGS, TX_RINGS, &port_conf) < 0) {
            throw std::runtime_error("Port configure failed");
        }

        // 4. Setup RX Queue
        if (rte_eth_rx_queue_setup(PORT_ID, 0, 1024, rte_eth_dev_socket_id(PORT_ID), NULL, mbuf_pool_) < 0) {
            throw std::runtime_error("RX queue setup failed");
        }

        // 5. Start Ethernet Port & Enable Promiscuous Mode
        if (rte_eth_dev_start(PORT_ID) < 0) throw std::runtime_error("Device start failed");
        rte_eth_promiscuous_enable(PORT_ID);
    }

    // High-speed polling loop running on isolated lcore
    template <typename Callback>
    inline void run_polling_loop(Callback&& on_packet) noexcept {
        struct rte_mbuf* bufs[BURST_SIZE];

        while (true) {
            // Receive burst of packets directly from NIC DMA ring
            uint16_t nb_rx = rte_eth_rx_burst(PORT_ID, 0, bufs, BURST_SIZE);

            for (uint16_t i = 0; i < nb_rx; ++i) {
                struct rte_mbuf* m = bufs[i];
                uint8_t* pkt_data = rte_pktmbuf_mtod(m, uint8_t*);
                uint16_t pkt_len = rte_pktmbuf_pkt_len(m);

                // Zero-copy processing of payload
                on_packet(pkt_data, pkt_len);

                // Recycle packet buffer back to thread-local mempool cache
                rte_pktmbuf_free(m);
            }
        }
    }
};
```

---

## Numbers

*Hardware Baseline: Mellanox ConnectX-6 Dx 25GbE / Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| Operation / Metric | Standard Linux Socket | DPDK Poll Mode Driver (PMD) |
| :--- | :--- | :--- |
| **Max Ingress Throughput** | ~1.5M packets/sec | **>35M packets/sec (Line Rate)**|
| **Median Ingress Latency ($p50$)**| **2,200–3,800 ns** | **~380–580 ns** |
| **Tail Latency Jitter ($p99.99$)**| **45.0–150.0 µs** | **<1.2 µs** |
| **Memory Allocation Overhead** | Dynamic Heap Allocation | **0 ns (Thread-Local Mempool)** |
| **System Calls per Packet** | 1 syscall per `recv()` | **0 syscalls (Pure Memory Poll)**|

---

## Trade-offs

| Framework Choice | Advantages | Limitations / Operational Costs |
| :--- | :--- | :--- |
| **DPDK** | Vendor-neutral; massive ecosystem; line-rate 100G throughput. | Heavy build footprint; requires `vfio-pci` setup and HugePage pools. |
| **Solarflare `ef_vi`** | Lowest absolute latency on x86 (~250ns); leanest C API. | Vendor lock-in; works exclusively on Solarflare NICs. |
| **Linux AF_XDP** | Native Linux kernel integration; does not hijack entire NIC interface. | Slightly higher latency than DPDK (~500–900ns). |

---

> [!warning] Gotchas
> 1. **Mempool Sizing & Core Starvation**: If `NUM_MBUFS` is set too low (e.g. 2,048) and packet processing falls slightly behind during a burst, all mbufs are held by the application, causing `rte_eth_rx_burst()` to return 0 and the NIC to drop packets. *Always size `NUM_MBUFS` to at least $8,192 \times \text{number of RX queues}$.*
> 2. **IOMMU / VFIO Remapping Latency**: When binding NICs with `vfio-pci`, running without hugepages forces the IOMMU to translate 4KB pages continuously, causing IOTLB misses. *Always configure 1GB HugePages (`hugepagesz=1G`) to minimize IOMMU address translation overhead.*

---

## Lab
**Objective**: Build a high-throughput DPDK packet parser that initializes an `rte_mempool`, polls an interface via `rte_eth_rx_burst()`, extracts UDP market data payloads, and measures per-burst processing latency.

**Success Criteria**:
1. Allocate a 2MB/1GB HugePage-backed `rte_mempool`.
2. Process 10,000,000 synthetic network frames in bursts of 32.
3. Verify that average burst processing time is **under 15 nanoseconds per packet**.

---

> [!question]- Self-test
> 1. **What is a Poll Mode Driver (PMD) in DPDK and how does it differ from a standard Linux kernel network driver?**
>    *Answer*: A standard Linux network driver relies on hardware interrupts (MSI-X) to notify the kernel when packets arrive, triggering context switches and interrupt handling. A DPDK PMD is a user-space driver that continuously spin-polls the NIC's RX and TX descriptor rings directly in a tight loop, executing zero interrupts, zero system calls, and zero context switches.
> 2. **How does `rte_mempool` with thread-local caches (`rte_mempool_cache`) prevent cross-core contention during packet allocation and deallocation?**
>    *Answer*: Each logical CPU core (`lcore`) maintains a private, lock-free thread-local cache within the mempool. When an lcore allocates (`rte_pktmbuf_alloc`) or frees (`rte_pktmbuf_free`) a packet buffer, it accesses its private local array without executing atomic CAS operations or locking mutexes, completely eliminating cross-core cache line bouncing.
> 3. **Why does DPDK process packets in bursts (e.g., `rte_eth_rx_burst(..., 32)`) rather than one packet at a time?**
>    *Answer*: Processing packets in bursts amortizes the overhead of reading and writing NIC hardware descriptor rings and updating memory pool pointers over multiple packets. It also maximizes CPU instruction and data cache locality, allowing the CPU to process 32 consecutive packets in a tight, unrolled loop with minimal branch mispredictions.

---

## Related
- [[06 - Networking/Network Interface Card Architecture]]
- [[06 - Networking/Kernel Bypass Technologies Overview]]
- [[06 - Networking/Solarflare ef_vi Zero-Copy API]]
- [[04 - Hardware Mechanical Sympathy/TLB Architecture and Huge Pages]]
- [[06 - Networking/MOC - 06 Networking]]

## Sources
- [[Sources/DPDK Programmer's Guide]]
- [[Sources/DPDK Sample Applications User Guide]]
- [[Sources/Systems Performance by Brendan Gregg]]

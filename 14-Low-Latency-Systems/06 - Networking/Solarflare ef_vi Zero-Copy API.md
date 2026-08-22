---
tags: [trading/networking, trading/kernel-bypass, type/concept]
aliases: [ef_vi, Solarflare ef_vi, Virtual Interface, EVQ, Zero-Copy RX, Hardware Packet Filters]
status: evergreen
module: 06
created: 2026-08-22
---

> [!summary]
> Solarflare `ef_vi` (Ethernet Fabric Virtual Interface) is the gold standard low-level C API for ultra-low-latency kernel-bypass networking on Solarflare / AMD XtremeScale NICs. By exposing hardware descriptor rings, event queues, and zero-copy packet buffers directly to user-space applications, `ef_vi` achieves sub-300ns wire-to-memory packet processing with zero kernel syscalls.

---

## Why it matters
In tier-1 electronic market making, consuming market data over standard Linux sockets adds 2,000 to 4,000 nanoseconds of operating system drag. Even high-performance user-space POSIX stacks (like OpenOnload) incur small software abstraction penalties (**~750–1,100 ns**).

Solarflare `ef_vi` strips away all socket abstractions:
- Application code manages physical NIC **Virtual Interfaces (VIs)** directly.
- HugePage memory buffers are pre-registered with the NIC's DMA engine.
- Packet ingress executes as a direct memory load (**~250–350 ns wire-to-strategy**).
- Outbound execution orders are injected onto the wire in **<150 nanoseconds** via Programmed I/O (PIO).

```mermaid
flowchart TD
    subgraph NIC_Hardware ["Solarflare X2522 25G NIC"]
        RX_RING["Hardware RX Descriptor Ring"]
        EVQ_HW["Hardware Event Queue (EVQ)"]
        FILTER["Hardware 5-Tuple / Multicast Filter"]
    end

    subgraph HostMemory ["Registered HugePage Memory Pool (ef_memreg)"]
        PKT_BUFS["Contiguous 2048-Byte Packet Buffers (Locked in RAM)"]
    end

    subgraph UserSpaceApp ["Low-Latency Market Data Feed Handler (Core 2)"]
        POLL["ef_eventq_poll() Loop (Tight Spin / Zero Syscalls)"]
        EXTRACT["Zero-Copy Direct Cast to ITCH / SBE Struct"]
        REPOST["ef_vi_receive_post() Buffer Recycling"]
        
        POLL --> EXTRACT --> REPOST
    end

    FILTER -->|Hardware DMA Write| PKT_BUFS
    FILTER -->|DMA Write Completion| EVQ_HW
    EVQ_HW <==|Memory Load Poll| POLL
    REPOST ==>|Post Buffer Address| RX_RING
```

---

## Mechanism

### 1. The Core `ef_vi` Architectural Primitives
1. **Virtual Interface (`ef_vi`)**: A dedicated hardware channel on the NIC consisting of an **RX Ring**, a **TX Ring**, and an **Event Queue (EVQ)**.
2. **Memory Registration (`ef_memreg`)**: Registers a chunk of host physical RAM (allocated via `mmap` with `MAP_HUGETLB`) with the NIC driver, translating virtual addresses to physical DMA bus addresses.
3. **Hardware Packet Filters (`ef_filter_spec`)**: Instructs the NIC hardware CAM to steer specific UDP multicast feeds (e.g. `233.54.12.1:15000`) or TCP sessions directly to our private VI.
4. **Event Queue (`EVQ`) Polling**: The application spin-polls the memory-mapped EVQ via `ef_eventq_poll()`. When a packet arrives, the EVQ returns an event describing the packet buffer ID, length, and hardware timestamp.

### 2. The Zero-Copy Ingress Lifecycle
1. **Pre-Allocation**: Software allocates 4,096 packet buffers (2 KB each) in a 2MB HugePage and registers them via `ef_memreg_alloc()`.
2. **Posting Descriptors**: Software posts buffer addresses to the RX ring using `ef_vi_receive_post()`.
3. **Hardware Ingestion**: The NIC receives the Ethernet frame, matches the hardware filter, DMAs bytes directly into the pre-posted buffer, and writes an `EF_EVENT_TYPE_RX` to the EVQ.
4. **Zero-Copy Processing**: The polling loop pops the event, calculates the memory address (`buffer_base + event.offset`), and casts the pointer directly to the domain message struct (`ItchAddOrder*`).
5. **Buffer Recycling**: Once the trade logic finishes, the buffer address is immediately re-posted to the RX ring. **Zero memory allocation occurs.**

---

## In Practice

### High-Speed Zero-Copy `ef_vi` UDP Multicast Receiver in C++20

```cpp
#include <etherfabric/vi.h>
#include <etherfabric/pd.h>
#include <etherfabric/memreg.h>
#include <sys/mman.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>

constexpr size_t PKT_BUF_SIZE = 2048;
constexpr size_t NUM_BUFS = 2048;
constexpr size_t TOTAL_MEM = NUM_BUFS * PKT_BUF_SIZE; // 4MB (2 HugePages)

struct PacketBuffer {
    uint8_t data[PKT_BUF_SIZE];
};

class SolarflareEfviReceiver {
private:
    ef_driver_handle dh_{-1};
    ef_pd pd_;
    ef_vi vi_;
    ef_memreg mr_;
    void* hugepage_mem_{nullptr};
    std::vector<ef_addr> dma_addrs_;

public:
    void init(const char* interface_name, const char* mcast_ip, uint16_t mcast_port) {
        // 1. Open driver handle & allocate Protection Domain (PD)
        if (ef_driver_open(&dh_) < 0) throw std::runtime_error("ef_driver_open failed");
        if (ef_pd_alloc(&pd_, dh_, -1, EF_PD_DEFAULT) < 0) throw std::runtime_error("ef_pd_alloc failed");

        // 2. Allocate Virtual Interface (VI) with hardware timestamping
        if (ef_vi_alloc_from_pd(&vi_, dh_, &pd_, dh_, -1, NUM_BUFS, NUM_BUFS, -1, NULL, -1, EF_VI_FLAGS_DEFAULT) < 0) {
            throw std::runtime_error("ef_vi_alloc failed");
        }

        // 3. Allocate 2MB HugePages and register memory with NIC
        hugepage_mem_ = mmap(nullptr, TOTAL_MEM, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
        if (hugepage_mem_ == MAP_FAILED) throw std::runtime_error("mmap HugePages failed");

        if (ef_memreg_alloc(&mr_, dh_, &pd_, dh_, hugepage_mem_, TOTAL_MEM) < 0) {
            throw std::runtime_error("ef_memreg_alloc failed");
        }

        // 4. Pre-post all packet buffers to RX ring
        dma_addrs_.resize(NUM_BUFS);
        for (size_t i = 0; i < NUM_BUFS; ++i) {
            dma_addrs_[i] = ef_memreg_dma_addr(&mr_, i * PKT_BUF_SIZE);
            ef_vi_receive_post(&vi_, dma_addrs_[i], i); // Buffer ID = i
        }

        // 5. Add Hardware UDP Multicast Filter
        ef_filter_spec filter;
        ef_filter_spec_init(&filter, EF_FILTER_FLAG_NONE);
        sockaddr_in mcast_addr{};
        mcast_addr.sin_family = AF_INET;
        mcast_addr.sin_port = htons(mcast_port);
        inet_pton(AF_INET, mcast_ip, &mcast_addr.sin_addr);

        if (ef_filter_spec_set_ip4_multicast(&filter, (const sockaddr*)&mcast_addr) < 0) {
            throw std::runtime_error("filter set failed");
        }
        if (ef_vi_filter_add(&vi_, dh_, &filter, NULL) < 0) {
            throw std::runtime_error("ef_vi_filter_add failed");
        }
    }

    // High-speed polling loop running on isolated core
    template <typename Callback>
    inline void poll_rx(Callback&& on_packet) noexcept {
        ef_event evs[16];
        int n_evs = ef_eventq_poll(&vi_, evs, 16);

        for (int i = 0; i < n_evs; ++i) {
            if (EF_EVENT_TYPE(evs[i]) == EF_EVENT_TYPE_RX) {
                int buf_id = EF_EVENT_RX_RQ_ID(evs[i]);
                int len = EF_EVENT_RX_BYTES(evs[i]);
                uint8_t* pkt_ptr = static_cast<uint8_t*>(hugepage_mem_) + (buf_id * PKT_BUF_SIZE) + EF_VI_RX_PREFIX_SIZE;

                // Process packet zero-copy directly from registered physical memory
                on_packet(pkt_ptr, len);

                // Immediately re-post buffer to RX ring
                ef_vi_receive_post(&vi_, dma_addrs_[buf_id], buf_id);
            }
        }
    }
};
```

---

## Numbers

*Hardware Baseline: Solarflare XtremeScale X2522 25G on AMD EPYC Genoa / Intel Sapphire Rapids @ 4.0 GHz.*

| Operation / Ingress Stage | Solarflare `ef_vi` Latency | Linux POSIX Socket |
| :--- | :--- | :--- |
| **PHY / MAC Ingress** | **~60–90 ns** | ~60–90 ns |
| **Direct PCIe DMA into Host RAM** | **~150–220 ns** | ~150–220 ns |
| **Event Poll (`ef_eventq_poll`)** | **~10–25 ns** | 1,200–3,500 ns (Syscall + IRQ) |
| **Packet Pointer Extraction** | **~5–10 ns (Zero-Copy)**| 250–500 ns (`copy_to_user`) |
| **Total Wire-to-Strategy Ingress**| **~225–345 ns** | **~1,660–4,310 ns (10x Slower)** |

---

## Trade-offs

| Interface API | Performance Advantage | Operational Limitations |
| :--- | :--- | :--- |
| **Solarflare `ef_vi`** | Absolute lowest software latency on x86 (~250ns); zero-copy. | Solarflare / AMD hardware lock-in; no built-in TCP stack. |
| **Solarflare OpenOnload** | Accelerates standard POSIX sockets transparently; built-in TCP. | 500ns slower than raw `ef_vi`; higher memory footprint. |
| **DPDK** | Broad multi-vendor portability (Intel, Mellanox, Broadcom). | 100–200ns higher latency than `ef_vi`; complex hugepage setup. |

---

> [!warning] Gotchas
> 1. **The `EF_VI_RX_PREFIX_SIZE` Offset Trap**: Solarflare hardware prepends a small hardware metadata header (typically 16–32 bytes containing hardware timestamps and hash flags) at the start of the DMA buffer. Failing to add `EF_VI_RX_PREFIX_SIZE` to the buffer pointer causes the packet parser to read hardware metadata instead of the Ethernet/IP header, corrupting payload parsing.
> 2. **RX Buffer Starvation Deadlocks**: If the user-space thread stalls or forgets to call `ef_vi_receive_post()` to recycle drained buffers, the NIC runs out of pre-posted descriptors, causing subsequent inbound packets to be silently dropped at the MAC layer.

---

## Lab
**Objective**: Build a mock `ef_vi` simulator in C++20 using pre-allocated HugePages, circular event completion queues, and zero-copy packet extraction, verifying sub-30ns user-space event-to-struct processing.

**Success Criteria**:
1. Pre-allocate 2MB HugePage memory pools.
2. Ingest 10,000,000 synthetic network frames into pre-posted descriptor buffers.
3. Verify that zero heap allocations or memory copies occur during steady-state polling.

---

> [!question]- Self-test
> 1. **What is a Virtual Interface (VI) in the Solarflare `ef_vi` architecture and what physical components does it contain?**
>    *Answer*: A Virtual Interface (VI) is a dedicated, user-space accessible hardware queue pair on a Solarflare NIC. It consists of three physical components: (1) an RX descriptor ring (where software posts empty buffer addresses for DMA); (2) a TX descriptor ring (where software posts outbound packet descriptors); and (3) an Event Queue (EVQ) where the NIC writes hardware completion events that the application polls.
> 2. **Why does `ef_vi` require registering HugePage memory buffers via `ef_memreg_alloc()` before receiving packets?**
>    *Answer*: The NIC's hardware DMA controller operates on physical memory (bus) addresses, whereas user-space applications operate on virtual addresses. Registering memory via `ef_memreg_alloc()` pins the physical RAM pages in place (preventing OS paging) and provides the NIC driver with the virtual-to-physical address translation table so the NIC can DMA packets directly into the process's pre-allocated memory.
> 3. **What is the purpose of `EF_VI_RX_PREFIX_SIZE` when extracting packet data from an `ef_vi` buffer?**
>    *Answer*: Solarflare NICs write a small hardware prefix (typically 16 or 32 bytes) at the beginning of each DMA buffer containing hardware packet metadata, including NIC hardware timestamps, packet hash flags, and checksum verification status. The application must offset the buffer pointer by `EF_VI_RX_PREFIX_SIZE` to reach the start of the actual Layer-2 Ethernet frame.

---

## Related
- [[06 - Networking/Network Interface Card Architecture]]
- [[06 - Networking/Kernel Bypass Technologies Overview]]
- [[06 - Networking/UDP Multicast Market Data and A-B Feed Arbitration]]
- [[07 - Time & Measurement/Clock Sources and Hardware Timestamping]]
- [[06 - Networking/MOC - 06 Networking]]

## Sources
- [[Sources/Solarflare ef_vi User Guide]]
- [[Sources/Solarflare Low Latency Ethernet Architecture by David Riddoch]]
- [[Sources/Systems Performance by Brendan Gregg]]

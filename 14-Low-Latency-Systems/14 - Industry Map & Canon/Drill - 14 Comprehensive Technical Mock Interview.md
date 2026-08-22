---
tags: [trading/canon, trading/interviews, type/drill]
aliases: [Drill 14, Mock Interview, Principal Engineer Interview, Comprehensive Technical Interview, C++ Systems Interview Drill]
status: evergreen
module: 14
created: 2026-08-22
---

# Drill 14 — Comprehensive Technical Mock Interview: Staff/Principal Low-Latency Engineer

> [!summary]
> Comprehensive, full-scale 60-minute technical mock interview simulating an elite Tier-1 high-frequency trading firm interview (Citadel Securities, Jane Street, HRT, Jump, Optiver). Attempt each part under strict timed conditions before unfolding the principal-level solutions.

---

## Interview Structure & Overview
- **Duration**: 60 Minutes
- **Target Role**: Staff / Principal Low-Latency C++ & Systems Infrastructure Engineer
- **Sections**:
  1. **Section 1: Hardware Mechanical Sympathy & CPU Internals (15 Mins)**
  2. **Section 2: C++ Memory Model & Lock-Free Concurrency (15 Mins)**
  3. **Section 3: Kernel Bypass Networking & Protocol Engineering (15 Mins)**
  4. **Section 4: End-to-End System Design & Tail Latency Triage (15 Mins)**

---

### Section 1: Hardware Mechanical Sympathy & CPU Internals (15 Minutes)

#### Question 1.1: Cache Coherence & False Sharing
**Interviewer Prompt**:
*"We have a trading engine running on Core 2 that processes market data, and an audit logger running on Core 3 that logs trade events. They share a single telemetry struct:*
```cpp
struct Telemetry {
    uint64_t last_market_seq{0};
    uint64_t last_logged_seq{0};
};
Telemetry g_telemetry;
```
*Core 2 updates `last_market_seq` on every tick, and Core 3 updates `last_logged_seq` on every write. Both cores report 45-nanosecond latency spikes even though they never access the same variable. Walk me through the exact hardware cache coherence transactions (MESI protocol) occurring on the CPU interconnect."*

> [!question]- Unfold Solution (Section 1.1)
> **Hardware Root Cause Analysis (False Sharing via MESI Invalidation)**:
> 1. **Shared Cache Line**: Both `last_market_seq` (8 bytes) and `last_logged_seq` (8 bytes) reside in the same **64-byte L1 data cache line** (`sizeof(Telemetry) = 16\text{ bytes} \le 64\text{ bytes}`).
> 2. **Core 2 Mutation**: When Core 2 writes to `last_market_seq`, its L1d cache controller must transition the cache line from **Shared (`S`)** to **Modified (`M`)**. It broadcasts an **Invalidate (Read-For-Ownership / RFO)** message across the CPU ring interconnect.
> 3. **Core 3 Invalidation**: Core 3 receives the RFO and transitions its local copy of the cache line to **Invalid (`I`)**.
> 4. **Core 3 Read/Write Stall**: When Core 3 subsequently updates `last_logged_seq`, its local cache line is Invalid. It incurs an **L1d cache miss**, forcing the CPU to stall for **40 to 75 nanoseconds** while fetching the modified line from Core 2's L2 cache across the interconnect.
> 5. **Cache Line Bouncing (Ping-Pong)**: As both cores write simultaneously, the 64-byte line constantly bounces back and forth between Core 2 and Core 3, stalling both instruction pipelines.
> 6. **Remediation**: Align each variable to separate 64-byte cache lines using `alignas(64)`:
>    ```cpp
>    struct alignas(64) Telemetry {
>        alignas(64) uint64_t last_market_seq{0};
>        alignas(64) uint64_t last_logged_seq{0};
>    };
>    ```

---

### Section 2: C++ Memory Model & Lock-Free Concurrency (15 Minutes)

#### Question 2.1: Acquire-Release Semantics vs Sequentially Consistent Atomics
**Interviewer Prompt**:
*"Explain the exact difference between `std::memory_order_seq_cst`, `std::memory_order_acquire`, `std::memory_order_release`, and `std::memory_order_relaxed`. On an x86-64 CPU, why does `memory_order_seq_cst` emit an expensive instruction while `acquire` and `release` compile to plain `MOV` instructions?"*

> [!question]- Unfold Solution (Section 2.1)
> **1. Memory Order Definitions**:
> - **`memory_order_relaxed`**: Guarantees atomic read/write of the target variable, but imposes **zero memory ordering constraints** on surrounding memory operations. The compiler and CPU may reorder adjacent loads and stores freely.
> - **`memory_order_release`**: Creates a **one-way release barrier**. No memory reads or writes preceding the store can be reordered *after* this store. Used by a Producer to publish data to a ring buffer.
> - **`memory_order_acquire`**: Creates a **one-way acquire barrier**. No memory reads or writes following the load can be reordered *before* this load. Used by a Consumer to safely observe published data.
> - **`memory_order_seq_cst`**: Guarantees acquire-release semantics plus a **globally consistent total order of operations across all threads**.
>
> **2. x86-64 Hardware Architecture (Total Store Order - TSO)**:
> - x86 hardware natively enforces **Total Store Order (TSO)**. In x86 hardware, loads are never reordered with other loads, stores are never reordered with other stores, and stores are never reordered before older loads.
> - Because x86 hardware already guarantees Acquire semantics on every load and Release semantics on every store, `std::memory_order_acquire` and `std::memory_order_release` compile to **standard single-cycle `MOV` assembly instructions with ZERO instruction overhead**.
> - The *only* reordering x86 hardware allows is a younger Load being reordered ahead of an older Store (Store-Load reordering via CPU store buffers). To enforce `memory_order_seq_cst`, the compiler must prevent Store-Load reordering by emitting an expensive **`MFENCE` or `LOCK XADD` instruction**, stalling the CPU pipeline for **15 to 35 nanoseconds**.

---

### Section 3: Kernel Bypass Networking & Protocol Serialization (15 Minutes)

#### Question 3.1: Zero-Copy Parsing and Unaligned Access
**Interviewer Prompt**:
*"You receive a raw 1500-byte UDP market data packet in a HugePage memory buffer from a Solarflare `ef_vi` descriptor. You need to parse a 48-byte NASDAQ ITCH Add Order message. Write the C++ code to extract the 64-bit Order Reference ID, 32-bit Shares, and 32-bit Price with zero memory copies, and explain how you prevent undefined behavior (UB) and split-cache line penalties."*

> [!question]- Unfold Solution (Section 3.1)
> **C++20 Implementation**:
> ```cpp
> #include <cstdint>
> #include <cstring>
>
> #pragma pack(push, 1)
> struct ItchAddOrderMsg {
>     char     msg_type;
>     uint16_t stock_locate;
>     uint16_t tracking_number;
>     uint8_t  timestamp[6];
>     uint64_t order_ref_id; // Big-Endian
>     char     side;
>     uint32_t shares;       // Big-Endian
>     char     stock[8];
>     uint32_t price;        // Big-Endian (4 decimals)
> };
> #pragma pack(pop)
>
> inline void parse_itch_zero_copy(const uint8_t* dma_buffer, 
>                                  uint64_t& out_order_id, 
>                                  uint32_t& out_shares, 
>                                  uint32_t& out_price) noexcept {
>     // 1. Direct Zero-Copy Pointer Overlay
>     const auto* itch = reinterpret_cast<const ItchAddOrderMsg*>(dma_buffer);
>
>     // 2. Hardware Single-Cycle Byte Swaps (BSWAP)
>     out_order_id = __builtin_bswap64(itch->order_ref_id);
>     out_shares   = __builtin_bswap32(itch->shares);
>     out_price    = __builtin_bswap32(itch->price);
> }
> ```
>
> **UB & Split-Cache Mitigation**:
> 1. **Strict Aliasing UB**: In standard C++, casting `uint8_t*` to `ItchAddOrderMsg*` violates strict aliasing. We compile with `-fno-strict-aliasing` or use `std::bit_cast` in standard-compliant code.
> 2. **Split-Cache Line Penalty**: If `itch->order_ref_id` spans across a 64-byte L1 cache line boundary (e.g. bytes 60..67), the CPU incurs a **15 to 25ns split-cache penalty**. In our network DMA configuration, we enforce that the packet payload begins at an aligned 64-byte offset (`EF_VI_RX_PREFIX_SIZE = 0`), ensuring all 8-byte integers remain within a single cache line.

---

### Section 4: End-to-End System Design & Tail Latency Triage (15 Minutes)

#### Question 4.1: Isolating a 2-Microsecond Tail Latency Spike
**Interviewer Prompt**:
*"Our trading system has a median software turnaround latency of 45 nanoseconds. However, during the 09:30:00 US Market Open, our $p99.9$ latency degrades to 2,800 nanoseconds. We know the core is isolated (`isolcpus=2`) and thread-pinned. Give me your step-by-step forensic diagnostic plan using Linux tools to identify and fix the root cause."*

> [!question]- Unfold Solution (Section 4.1)
> **Step-by-Step Latency Forensics & Remediation Plan**:
>
> 1. **Hardware Counter Triage (`perf stat`)**:
>    - Attach `perf stat -e cycles,instructions,cache-misses,L1-dcache-load-misses,branch-misses,context-switches -p <PID>` during the market open.
>    - If `context-switches > 0`: Check if the core is missing `nohz_full=2` or `rcu_nocbs=2`, or if an un-isolated thread is preempting the core.
>    - If `cache-misses` surge: Indicates memory hierarchy evictions or false sharing.
>
> 2. **Instruction-Level Flamegraph Profiling (`perf record -g`)**:
>    - Profile CPU execution at 99 kHz and inspect the top hot spots.
>    - Check for hidden calls to `malloc()`, `free()`, or `std::string` formatting inside logging routines or order token constructors.
>
> 3. **NIC Hardware Drop Counters (`ethtool -S`)**:
>    - Inspect `rx_nodesc_drops` and `rx_discards`. If `rx_nodesc_drops > 0`, the market open microburst exhausted the host RX descriptor ring because buffer recycling was too slow. Resize rings to 4,096 entries (`ethtool -G eth0 rx 4096`).
>
> 4. **Floating-Point & Memory Alignment Audit**:
>    - Search for 64-bit floating-point divisions (`vdivsd`) in alpha pricing math; replace with fixed-point integer bit-shifts (`>> 16`) to avoid subnormal microcode traps.
>    - Verify that all shared memory pointers are annotated with `alignas(64)` to eliminate false sharing with background threads.

---

## Candidate Scoring Matrix & Evaluation Criteria

```text
===================================================================================
 CANDIDATE EVALUATION SCORING SHEET
===================================================================================
 [ ] SECTION 1 (Hardware & CPU):
     - Score 4/4: Explains MESI RFO invalidations and cache line bouncing in nanoseconds.
     - Score 2/4: Mentions false sharing but cannot explain hardware cache line mechanics.
     - Score 0/4: Suggests using a mutex to fix the issue.

 [ ] SECTION 2 (Memory Model & Atomics):
     - Score 4/4: Explains x86 TSO, store buffers, and why seq_cst emits MFENCE while acquire/release are plain MOVs.
     - Score 2/4: Knows acquire/release pairs but cannot explain hardware instruction emission.
     - Score 0/4: Thinks all atomics lock the CPU bus.

 [ ] SECTION 3 (Networking & Protocols):
     - Score 4/4: Writes packed struct with BSWAP; explains split-cache line penalties and strict aliasing.
     - Score 2/4: Writes working code but forgets byte-swapping or alignment.
     - Score 0/4: Uses std::stringstream or dynamic memory to parse packets.

 [ ] SECTION 4 (System Design & Profiling):
     - Score 4/4: Methodical perf/ethtool investigation diagnosing malloc, false sharing, and microbursts.
     - Score 2/4: Mentions perf but lacks systematic root-cause isolation workflow.
     - Score 0/4: Guesses randomly without measurement data.
===================================================================================
 VERDICT: 15-16 = STRONG HIRE (Principal/Staff) | 12-14 = HIRE (Senior) | <12 = NO HIRE
===================================================================================
```

---

## Related
- [[14 - Industry Map & Canon/The Low-Latency C++ Technical Interview Bar]]
- [[14 - Industry Map & Canon/The Quantitative Trading Firm Landscape]]
- [[08 - Low-Latency Programming/C++ Memory Model and Memory Orders]]
- [[04 - Hardware Mechanical Sympathy/Latency Numbers Every Trading Engineer Knows]]
- [[14 - Industry Map & Canon/MOC - 14 Industry Map & Canon]]

## Sources
- [[Sources/Intel 64 and IA-32 Architectures Software Developer's Manual]]
- [[Sources/C++ Concurrency in Action by Anthony Williams]]
- [[Sources/Systems Performance by Brendan Gregg]]
- [[Sources/How to Build an Exchange by Jane Street]]

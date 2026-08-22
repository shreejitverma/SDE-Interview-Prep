---
tags: [trading/interview, trading/low-latency-cpp, trading/cheatsheet, type/cheatsheet]
aliases: [C++ Interview Cheatsheet, Low-Latency Cheatsheet, HFT C++ Quick Reference, Systems Programming Cheatsheet]
status: evergreen
module: 14
created: 2026-08-22
---

# Core C++ & Low-Latency Systems Interview Cheat Sheet

> [!summary]
> Ultra-dense, rapid-fire technical reference for C++20/23, x86-64 hardware mechanical sympathy, Linux kernel tuning, and low-latency network engineering. Memorize every section before Tier-1 quantitative trading interviews.

---

## 1. C++20 Memory Model & Atomics Quick Reference

| Memory Order | Operations | Hardware Cost (x86-64) | Usage Pattern |
| :--- | :--- | :--- | :--- |
| **`std::memory_order_relaxed`** | Load / Store | Plain `MOV` (0 ns) | Standalone counters, sequence numbers. |
| **`std::memory_order_acquire`** | Load | Plain `MOV` (0 ns) | Consumer polling head/tail; prevents subsequent reads from reordering before load. |
| **`std::memory_order_release`** | Store | Plain `MOV` (0 ns) | Producer publishing data; prevents prior writes from reordering after store. |
| **`std::memory_order_acq_rel`** | RMW (Fetch-Add/CAS)| Plain / `LOCK` (0–10 ns) | Lock-free queues, atomic reference counting. |
| **`std::memory_order_seq_cst`** | All | **`MFENCE` / `LOCK` (15–35 ns)** | Sequential consistency; **avoid on hot execution paths!** |

---

## 2. Essential x86-64 Compiler Intrinsics

```cpp
#include <x86intrin.h>
#include <bit>

// 1. Hardware Single-Cycle Byte Swaps (Big-Endian Network -> Little-Endian Host)
uint64_t le64 = __builtin_bswap64(be64);
uint32_t le32 = __builtin_bswap32(be32);
uint16_t le16 = __builtin_bswap16(be16);

// 2. Sub-Nanosecond Bit-Scan (Top-of-Book BBO Finding)
uint32_t best_level = _tzcnt_u64(price_level_bitmap); // Trailing Zero Count (1 cycle)
uint32_t active_levels = _mm_popcnt_u64(price_level_bitmap); // Population Count (1 cycle)

// 3. Spin-Wait Optimization (Frees CPU Memory Bus & Prevents Pipeline Flushes)
_mm_pause(); // Emits PAUSE instruction (~40-140 cycles delay)

// 4. Non-Temporal Streaming Store (Bypasses L1/L2 Caches to Prevent Cache Pollution)
_mm_stream_si128(reinterpret_cast<__m128i*>(dest_ptr), data_vec);

// 5. Serialized RDTSC Cycle Counter Reads
_mm_lfence();
uint64_t tsc_start = __rdtsc();
_mm_lfence();

// 6. Modern C++20 Safe Type Punning (Zero UB, Compiles to Direct Register Move)
auto msg = std::bit_cast<ItchHeader>(raw_bytes);
```

---

## 3. Hardware Mechanical Sympathy & Cache Alignment Rules

```cpp
// 1. ELIMINATE FALSE SHARING: Pad independent cross-thread atomics to 64 bytes
struct alignas(64) SpscRingPointers {
    alignas(64) std::atomic<size_t> write_head{0}; // Cache Line 0 (Producer writes)
    alignas(64) std::atomic<size_t> read_tail{0};  // Cache Line 1 (Consumer writes)
};

// 2. PACK HOT FIELDS TOGETHER: Ensure hot tick fields fit in a single 64-byte line
struct alignas(64) HotBookLevel {
    uint32_t price;       // 4 bytes
    uint32_t total_qty;   // 4 bytes
    uint32_t order_count; // 4 bytes
    uint32_t padding[13]; // 52 bytes -> Exactly 64 bytes!
};

// 3. FORCE 64-BYTE COMPILER LOOP ALIGNMENT:
// Compile with: -falign-functions=64 -falign-loops=64
```

---

## 4. Linux Real-Time Kernel Hardening Checklist

```bash
# /etc/default/grub -> GRUB_CMDLINE_LINUX:
# 1. Isolate CPU Cores 2-7 from kernel scheduler:
isolcpus=2-7

# 2. Disable timer ticks on isolated cores (Full Tickless Mode):
nohz_full=2-7

# 3. Offload Read-Copy-Update (RCU) callbacks from isolated cores:
rcu_nocbs=2-7

# 4. Disable CPU Idle C-States (Prevents Power Sleep Wakeup Jitter):
idle=poll processor.max_cstate=0 intel_idle.max_cstate=0

# 5. Disable Intel CPU Frequency Scaling & TurboBoost:
intel_pstate=disable

# 6. Allocate 1GB HugePages at Boot:
default_hugepagesz=1G hugepagesz=1G hugepages=32
```

---

## 5. Socket & Network Kernel Bypass Tuning

```cpp
// 1. Disable Nagle's Algorithm (Send packets immediately, no buffering)
int flag = 1;
setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

// 2. Disable Delayed ACKs (Send immediate TCP ACK frames)
setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &flag, sizeof(flag));

// 3. Socket Busy-Polling (Kernel polls NIC directly without interrupts)
int busy_poll_us = 50;
setsockopt(fd, SOL_SOCKET, SO_BUSY_POLL, &busy_poll_us, sizeof(busy_poll_us));

// 4. Lock All Virtual Memory to RAM (Prevent Page Faults during Trading)
mlockall(MCL_CURRENT | MCL_FUTURE);

// 5. Pin Trading Thread to Dedicated Isolated Core (e.g. Core 2)
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(2, &cpuset);
pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
```

---

## 6. The Mandatory Latency Numbers Table

| Subsystem / Operation | Latency (ns) | Cycles (@ 4.0 GHz) |
| :--- | :--- | :--- |
| **CPU Register Read / ALU Op** | **0.25 ns** | **1 cycle** |
| **L1d Data Cache Hit** | **~1.0 ns** | **4 cycles** |
| **L2 Cache Hit** | **~3.5 ns** | **14 cycles** |
| **L3 Cache Hit (Same Socket)** | **~12–15 ns** | **48–60 cycles** |
| **Cross-Core L3 Invalidation (RFO Bounce)**| **~40–75 ns** | **160–300 cycles** |
| **Main Memory (DRAM) Access** | **~60–80 ns** | **240–320 cycles** |
| **NUMA Remote Socket Fetch** | **~100–140 ns** | **400–560 cycles** |
| **Branch Misprediction Penalty (ROB Flush)**| **~4–5 ns** | **16–20 cycles** |
| **Solarflare `ef_vi` User-Space RX DMA**| **~120–180 ns** | **480–720 cycles** |
| **Standard Linux Kernel Socket Read** | **~1,200–2,500 ns** | **4,800–10,000 cycles** |
| **Light in Silica Fiber (Refractive $n=1.47$)**| **4.89 ns per meter** | N/A |
| **Light in Air / Microwave ($n=1.00$)** | **3.33 ns per meter** | N/A |

---

## Related Notes
- [[14 - Industry Map & Canon/The Low-Latency C++ Technical Interview Bar]]
- [[08 - Low-Latency Programming/C++ Memory Model and Memory Orders]]
- [[04 - Hardware Mechanical Sympathy/Latency Numbers Every Trading Engineer Knows]]
- [[05 - OS & Kernel Tuning/Kernel Boot Parameters for Core Isolation]]
- [[14 - Industry Map & Canon/MOC - 14 Industry Map & Canon]]

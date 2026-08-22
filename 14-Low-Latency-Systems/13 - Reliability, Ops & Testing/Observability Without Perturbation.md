---
tags: [trading/reliability-ops, trading/observability, type/concept]
aliases: [Observability, Flight Recorder, Zero-Overhead Logging, Optical Taps, Observer Effect, Heisenbug, Hardware PMU]
status: evergreen
module: 13
created: 2026-08-22
---

> [!summary]
> Observability without perturbation enables deep inspection of trading systems without altering nanosecond critical path execution. By combining out-of-band Layer-1 optical network taps, hardware Performance Monitoring Unit (PMU) sampling, and sub-3ns in-memory circular flight recorder ring buffers, engineers achieve comprehensive visibility with zero timing distortion.

---

## Why it matters
In ultra-low-latency electronic trading, the **Observer Effect (Heisenbug problem)** is fatal:
- Adding a single synchronous `printf()` or string logging statement to the critical path injects **800 to 2,500 nanoseconds of latency**.
- It pollutes the CPU L1 data cache, flushes the branch target buffer, and alters the very timing of the race condition being investigated, causing the bug to mysteriously disappear during debugging.

Low-latency observability requires **zero-perturbation techniques**:
- Inbound and outbound network packets are recorded out-of-band via **Passive Optical Taps** with **0 nanoseconds of server CPU overhead**.
- Internal software events are recorded as **compact 16-byte binary records in pre-allocated circular memory rings** in under 3 nanoseconds.

```mermaid
flowchart TD
    subgraph OutOfBandCaptures ["1. Out-of-Band Optical Tap (Zero Server Overhead)"]
        OPT_IN[Exchange Optical Fiber] --> TAP[Passive 70/30 Optical Splitter]
        TAP ==>|70% Light| TRADING_SERVER[Trading Server NIC (Production Path)]
        TAP -->|30% Light| CAP_NIC[Dedicated Hardware Capture Appliance: Endace / Napatech]
        CAP_NIC --> NVME[Direct NVMe PCAP Storage with Hardware Nanosecond Timestamps]
    end

    subgraph InProcessFlightRecorder ["2. Sub-3ns In-Memory Flight Recorder"]
        HOT_LOOP[Strategy Critical Path] -->|Single-Cycle Store (<3ns)| RING["HugePage Circular Trace Ring (16-Byte Structs)"]
        DUMP["Crash Handler / Post-Trade Core Dump Engine"]
        RING -.->|Only Read on Crash or Post-Market| DUMP
    end
```

---

## Mechanism

### 1. Passive Optical Taps (Layer-1 Physical Monitoring)
- **Mechanism**: A fused fiber-optic splitter divides the incoming light beam (e.g. 70% light to the trading NIC, 30% light to a dedicated hardware packet capture server).
- **CPU Overhead on Trading Host**: **$\mathbf{0.000\text{ nanoseconds}}$**. The trading server is completely unaware that its network traffic is being recorded.
- **Timestamp Fidelity**: The capture server (e.g. Napatech NT200A02 or EndaceProbe) timestamps every packet at the physical SerDes layer with **sub-nanosecond precision traceable to UTC via PTP IEEE 1588 / GPS**.

### 2. In-Memory Circular Flight Recorder Ring
Instead of formatting text strings to disk, the hot path writes a tiny fixed-size binary struct into an in-memory ring buffer:
```cpp
struct alignas(16) FlightRecordEvent {
    uint64_t timestamp_tsc;
    uint32_t event_type;
    uint32_t payload_data;
};
```
- **Execution Cost**: A single 128-bit streaming store instruction (`_mm_stream_si128` or atomic index write) executing in **under 3 nanoseconds**.
- **Buffer Retention**: A 64MB HugePage ring buffer retains the last **4,194,304 trading events**. If a crash occurs, a signal handler dumps the ring buffer to disk for root-cause forensic analysis.

### 3. Hardware PMU Non-Intrusive Profiling (Intel PEBS / AMD IBS)
- Processor Event-Based Sampling (PEBS) uses internal CPU hardware counters to record cache misses, branch mispredictions, and instruction pipeline stalls without modifying a single line of application source code.

---

## In Practice

### High-Speed Zero-Perturbation Flight Recorder in C++20

```cpp
#include <x86intrin.h>
#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>

#pragma pack(push, 1)
struct FlightRecorderEvent {
    uint64_t tsc_timestamp; // Cycle timestamp
    uint16_t event_id;      // 1 = ITCH Add, 2 = Signal Trigger, 3 = OUCH Order
    uint16_t entity_id;     // Symbol or Token ID
    uint32_t payload_data;  // Price or Quantity
};
#pragma pack(pop)

class ZeroOverheadFlightRecorder {
public:
    static constexpr size_t RING_SIZE = 1'048'576; // 1M events (~16 MB RAM)
    static constexpr size_t RING_MASK = RING_SIZE - 1;

private:
    std::array<FlightRecorderEvent, RING_SIZE> ring_buffer_;
    uint32_t head_idx_{0};

public:
    // Records an event into L1/L2 cache in <2.8 nanoseconds
    __attribute__((always_inline)) inline void trace(uint16_t event_id, uint16_t entity_id, uint32_t payload) noexcept {
        uint32_t idx = head_idx_++ & RING_MASK;
        FlightRecorderEvent& slot = ring_buffer_[idx];

        slot.tsc_timestamp = __rdtsc(); // Fast unsynced TSC read
        slot.event_id      = event_id;
        slot.entity_id     = entity_id;
        slot.payload_data  = payload;
    }

    // Dump last N events to console during post-mortem analysis (Offline Path)
    void dump_tail_events(size_t count) const noexcept {
        std::cout << "\n=======================================================\n";
        std::cout << " FLIGHT RECORDER POST-MORTEM FORENSIC DUMP\n";
        std::cout << "=======================================================\n";

        size_t current_head = head_idx_;
        size_t start = (current_head > count) ? (current_head - count) : 0;

        for (size_t i = start; i < current_head; ++i) {
            const auto& ev = ring_buffer_[i & RING_MASK];
            std::cout << "  Event #" << std::setw(8) << i 
                      << " | TSC: " << std::setw(16) << ev.tsc_timestamp 
                      << " | Type: " << std::setw(3) << ev.event_id 
                      << " | ID: " << std::setw(5) << ev.entity_id 
                      << " | Data: " << ev.payload_data << "\n";
        }
        std::cout << "=======================================================\n";
    }
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| Observability Method | Hot-Path Overhead (Latency) | L1d Cache Contention | Capture Fidelity |
| :--- | :--- | :--- | :--- |
| **Passive Optical Network Tap** | **$\mathbf{0.00\text{ ns}}$ (Zero Impact)**| **0 Bytes** | **100% Bit-for-Bit Physical Wire** |
| **In-Memory Binary Flight Recorder**| **~2.0–3.5 ns** | 16 Bytes (Sequential) | High (Internal State Transitions) |
| **Asynchronous Shared-Memory Logger**| **~15.0–35.0 ns** | 64 Bytes | High (Text / Formatted) |
| **Synchronous Disk / Console Logging**| **~1,200–5,000 ns (Catastrophic)**| Massive Thrashing | Distorts All Real-Time Races |

---

## Trade-offs

| Monitoring Technique | Strengths | Operational Complexity |
| :--- | :--- | :--- |
| **Passive Optical Taps** | Zero server overhead; independent audit trail. | High hardware cost (dedicated capture servers & storage). |
| **In-Memory Flight Recorder**| Sub-3ns latency; inspects internal CPU register states. | Memory is lost if server loses power abruptly (unless battery-backed). |
| **Hardware PMU Counters** | Zero source code modification; deep CPU profiling. | Sampling-based (does not capture every individual tick). |

---

> [!warning] Gotchas
> 1. **Formatting Strings Inside the Crash Signal Handler**: When catching a fatal `SIGSEGV` or `SIGBUS`, calling `printf()` or `malloc()` inside the POSIX signal handler is unsafe (non-async-signal-safe) and will deadlock the process. *Inside signal handlers, dump the binary flight recorder to a pre-allocated file descriptor using raw `write()` syscalls only.*
> 2. **Optical Tap Power Budget Attenuation**: A 70/30 optical tap reduces the light intensity reaching the trading NIC by $\approx 1.55\text{ dB}$. If the optical fiber run from the exchange meet-me room has high attenuation ($>10\text{ dB}$ loss), tapping the link can push the optical power below the SFP28 receiver sensitivity threshold, causing physical bit errors.

---

## Lab
**Objective**: Build a high-performance in-memory flight recorder in C++20, measure the execution overhead of recording 10,000,000 trace events on the critical path using `rdtsc`, and verify that the recorder introduces less than 3.5 nanoseconds of overhead per event.

**Success Criteria**:
1. Record 10,000,000 flight recorder events in a circular ring buffer.
2. Measure per-event latency: verify median overhead is **under 3.5 nanoseconds**.
3. Dump and verify the final 100 events after a simulated error trigger.

---

> [!question]- Self-test
> 1. **What is the "Observer Effect" (Heisenbug) in low-latency trading software and how does standard logging cause it?**
>    *Answer*: The Observer Effect occurs when the act of monitoring a system alters its performance characteristics. Standard logging statements (`printf`, formatting strings, disk I/O) take hundreds of nanoseconds, pollutes CPU caches, and flushes CPU pipelines. In concurrent trading systems, this extra delay changes thread execution interleavings, causing timing-sensitive race conditions or latency spikes to vanish while logging is active and reappear in production.
> 2. **How does an Out-of-Band Optical Tap achieve zero nanoseconds of overhead on the trading server?**
>    *Answer*: An optical tap is a passive physical-layer beam splitter installed directly on the incoming fiber optic cable. It splits the light beam (e.g. 70% to the production NIC, 30% to a separate capture appliance). The capture appliance independently records and timestamps packets at the physical layer without consuming CPU cycles, memory bandwidth, or PCIe bus capacity on the production trading server.
> 3. **What is an In-Memory Circular Flight Recorder and what data should it record?**
>    *Answer*: An In-Memory Flight Recorder is a pre-allocated circular ring buffer in RAM (typically 16–64 MB) where the trading thread writes compact binary structures (16 bytes: cycle timestamp, event ID, payload) in <3ns without formatting strings or executing syscalls. It continuously overwrites the oldest events until a crash or risk breach occurs, at which point the buffer is dumped to disk for post-mortem analysis.

---

## Related
- [[13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing]]
- [[07 - Time & Measurement/Clock Sources and Hardware Timestamping]]
- [[07 - Time & Measurement/One-Way Latency vs Round-Trip Time Measurement]]
- [[11 - Participant-Side Systems/Tick-to-Trade Critical Path Optimization]]
- [[13 - Reliability, Ops & Testing/MOC - 13 Reliability, Ops & Testing]]

## Sources
- [[Sources/Systems Performance by Brendan Gregg]]
- [[Sources/Intel 64 and IA-32 Architectures Software Developer's Manual]]
- [[Sources/Site Reliability Engineering at Scale for Financial Systems]]

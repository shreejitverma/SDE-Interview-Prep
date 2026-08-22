---
tags: [trading/participant-systems, type/drill]
aliases: [Drill 11, Bottleneck Hunting Drill, Latency Profiling Drill, Perf Profiling Drill]
status: evergreen
module: 11
created: 2026-08-22
---

# Drill 11 — Tick-to-Trade Pipeline Bottleneck Hunting & Tail Spike Triage

> [!summary]
> Principal-level performance engineering drill simulating an urgent latency investigation: your firm's market making engine exhibits an excellent median software turnaround of 45ns, but suffers intermittent **2.8-microsecond ($p99.9$) tail spikes** during macro volatility bursts. Attempt each diagnostic phase before unfolding the solution.

---

### Incident Scenario: The 2.8µs Volatility Burst Anomaly
**System Profile**:
- Engine: C++20 HFT Strategy running on an isolated CPU core (`isolcpus=2, nohz_full=2`).
- Baseline: $p50 = 45\text{ ns}$, $p90 = 62\text{ ns}$.
- Anomaly: During heavy bursts (e.g. CPI release at 08:30:00), $p99.9$ latency degrades to **$2,850\text{ ns}$**, causing the firm to miss over 65% of cancel-replace races and incur severe adverse selection.

---

### Diagnostic Step 1: Hardware Performance Counter Triage
**Prompt**:
You attach Linux `perf` to the trading process during a simulated 500,000-packet market burst.

**CLI Command**:
```bash
perf stat -e cycles,instructions,cache-misses,L1-dcache-load-misses,branch-misses,context-switches -p $(pgrep hft_engine) -- sleep 5
```

**Output**:
```text
 Performance counter stats for process id '48201':

     19,842,108,124      cycles                    #    3.968 GHz
     35,715,794,623      instructions              #    1.80  insn per cycle
            482,104      cache-misses              #   14.2% of all L1D misses
          3,395,102      L1-dcache-load-misses     #    0.09% of all L1-dcache hits
            184,209      branch-misses             #    0.01% of all branches
                  0      context-switches          #    0.000 / sec
```

**Questions**:
1. Did any OS context switches or CPU thread migrations occur on the core?
2. What do the 482,104 cache misses indicate?
3. Where is the CPU stalling: in branch prediction, memory access, or OS scheduling?

> [!question]- Unfold Solution
> 1. **OS Scheduling**: **Clean (0 context switches)**. Thread pinning and `isolcpus` are operating correctly; no kernel thread preemption occurred.
> 2. **Cache Misses**: **Severe Memory Hierarchy Stalls**. The 482,104 cache misses mean that during burst periods, data structures were not found in L1d or L2 caches, forcing the core to stall and fetch data from L3 / DRAM (~60ns per miss).
> 3. **Primary CPU Bottleneck**: **Memory Access & Data Cache Evictions**. The instruction pipeline is stalled waiting on memory loads rather than branch mispredictions.

---

### Diagnostic Step 2: Instruction-Level Disassembly & Flamegraph Analysis
**Prompt**:
You run `perf record -g` and inspect the top hot spots and disassembly of the critical path function `on_market_tick()`.

**`perf report` Annotated Disassembly**:
```text
Samples: 250K of event 'cycles:u', Event count (approx.): 19842108124
Overhead  Command     Shared Object        Symbol
  42.10%  hft_engine  hft_engine           [.] FastTickToTradePipeline::on_market_packet_ingress
  28.40%  hft_engine  libc.so.6            [.] malloc
  14.20%  hft_engine  hft_engine           [.] std::string::_M_mutate
   8.50%  hft_engine  libc.so.6            [.] free

--- Disassembly of on_market_packet_ingress (Hot Path Snippet) ---
0x4021a0:  mov    rax, QWORD PTR [rdi+0x18]      ; Load order_id
0x4021a4:  call   0x401080 <format_audit_string> ; Calls string formatting!
0x4021a9:  vdivsd xmm0, xmm1, xmm2               ; Floating point division!
0x4021ad:  mov    QWORD PTR [rsi+0x20], rax      ; Write to shared logger struct
```

**Questions**:
1. Why are `malloc`, `free`, and `std::string::_M_mutate` appearing in the `perf report` of an HFT hot path?
2. What is the performance danger of the `call format_audit_string` line?
3. What is the danger of `vdivsd` and the write to `[rsi+0x20]`?

> [!question]- Unfold Solution
> 1. **Hidden Dynamic Memory Allocations**: The strategy code is instantiating temporary `std::string` objects during audit logging or order token formatting, calling `malloc`/`free` on the critical path. During bursts, thread memory allocator contention stalls the pipeline for microseconds.
> 2. **Synchronous String Formatting Overhead**: `format_audit_string` allocates heap buffers synchronously *before* releasing the order packet to the network, injecting 800–1,500ns of latency directly in front of the outbound order!
> 3. **Floating-Point Division & False Sharing**:
>    - `vdivsd`: 64-bit floating-point division takes 14–35 cycles, risking subnormal assist traps.
>    - `mov QWORD PTR [rsi+0x20], rax`: Writing to a shared logging struct that resides on the same 64-byte cache line as the background logger thread causes **False Sharing**, invalidating Core 1's L1 cache line on every tick!

---

### Diagnostic Step 3: Comprehensive Engineering Remediation Plan
**Prompt**:
Formulate the complete, production-grade refactoring plan to eliminate all 2.8µs tail spikes.

**Questions**:
1. How should audit logging be restructured?
2. How should the floating-point calculation be replaced?
3. How should the false sharing memory hazard be resolved?

> [!question]- Unfold Solution
> **Comprehensive Refactoring Plan**:
>
> 1. **Zero-Allocation Asynchronous Ring Buffer Logging**:
>    - Remove all `std::string` allocations and synchronous logging from the critical path.
>    - Pre-allocate a lock-free SPSC circular ring buffer of binary audit structs (`struct AuditEvent { uint64_t ts; uint32_t token; uint32_t price; };`).
>    - Write the binary event to the ring in <5ns; delegate string formatting and disk I/O to a background non-isolated worker thread.
> 2. **Fixed-Point Integer Arithmetic Conversion**:
>    - Replace all `double` variables and `vdivsd` floating-point divisions with 64-bit integer fixed-point arithmetic (`uint64_t` with `>> 16` bit-shifts).
>    - Compile with `-ffast-math` and enable Flush-to-Zero (`FTZ`) mode.
> 3. **Cache Line Isolation & Padding (`alignas(64)`)**:
>    - Enforce `alignas(64)` on the strategy's order state struct and the logging ring buffer write pointer to eliminate false sharing.
>
> **Outcome**: Software turnaround drops to **$<50\text{ ns}$ across all percentiles ($p50=24\text{ ns}$, $p99.9=68\text{ ns}$)**, permanently eliminating the 2.8µs tail spikes.

---

## Related
- [[11 - Participant-Side Systems/Tick-to-Trade Critical Path Optimization]]
- [[04 - Hardware Mechanical Sympathy/False Sharing and Cache Contention]]
- [[08 - Low-Latency Programming/Allocation-Free Steady State Patterns]]
- [[07 - Time & Measurement/Coordinated Omission in Low Latency Systems]]
- [[11 - Participant-Side Systems/MOC - 11 Participant-Side Systems]]

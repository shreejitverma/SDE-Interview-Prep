---
tags: [trading/fpga, type/drill]
aliases: [Drill 12, Hybrid Architecture Drill, System Design Drill, FPGA Partitioning Drill]
status: evergreen
module: 12
created: 2026-08-22
---

# Drill 12 — Hybrid CPU-FPGA Architecture Design & Nanosecond Budgeting

> [!summary]
> Principal-level systems architecture drill: you are tasked with designing an institutional-grade, hybrid CPU-FPGA trading system executing cross-market statistical arbitrage between CME E-mini Futures (Aurora, IL) and NASDAQ Equities (Carteret, NJ). Attempt each design phase before unfolding the solution.

---

### System Design Scenario: CME-to-NASDAQ Cross-Market Lead-Lag Arbitrage
**System Objectives**:
1. Ingest ultra-low-latency microwave market data from CME Aurora (MDP 3.0 SBE) and local NASDAQ Carteret (ITCH 5.0).
2. Detect CME E-mini order book sweeps and instantly sweep resting liquidity on NASDAQ SPY / QQQ ETFs.
3. Achieve a **local wire-to-wire execution latency on NASDAQ of under 180 nanoseconds**.
4. Maintain full compliance with SEC Rule 15c3-5 and CFTC pre-trade risk controls.

---

### Design Phase 1: Hardware vs Software Domain Partitioning
**Prompt**:
Partition the responsibilities of the system between the **FPGA Silicon Fabric** and the **Host Linux CPU Cores**.

**Questions**:
1. Which components belong on the FPGA critical path?
2. Which components belong on the Host CPU asynchronous slow path?
3. How should the CPU push updated pricing models and volatility parameters to the FPGA without stalling the hardware loop?

> [!question]- Unfold Solution
> **Optimal Domain Partitioning**:
>
> 1. **FPGA Silicon Critical Path (Fast Path: Sub-180ns Wire-to-Wire)**:
>    - 25G GTY SerDes & Cut-Through Low-Latency MAC (LL-MAC).
>    - Hardware MDP 3.0 SBE and ITCH 5.0 streaming AXI4-Stream parsers.
>    - On-chip Level-2/Top-of-Book BBO cache in dual-port BRAM.
>    - Hardware Stoikov micro-price & imbalance lead-lag trigger.
>    - "Bump-in-the-wire" pre-trade risk filter (Price collars, max size, CRC poisoning).
>    - OUCH 4.2 binary order serializer & 10G/25G optical transmitter.
> 2. **Host CPU Asynchronous Domain (Slow Path: Microseconds / Milliseconds)**:
>    - Multi-factor quantitative alpha model parameter calibration (Python / C++20).
>    - Continuous volatility surface & skew recalculation.
>    - FIX Drop Copy ingestion, clearing reconciliation, and risk officer GUI.
>    - Historical packet capture (PCAP) logging to NVMe SSDs.
> 3. **CPU-to-FPGA Parameter Synchronization**:
>    - The CPU writes updated pricing thresholds, risk limits, and multiplier weights into **FPGA PCIe BAR0 Memory-Mapped Registers (MMIO Writes)**.
>    - The FPGA strategy reads these registers asynchronously from on-chip shadow registers with **zero pipeline stalls**.

---

### Design Phase 2: Complete Nanosecond Latency Budget Calculation
**Prompt**:
Construct the cycle-accurate and physical-layer nanosecond latency budget for the local NASDAQ Carteret execution path (from local market tick ingress to OUCH order egress).

**Question**:
Provide the exact latency breakdown across all 8 pipeline stages from optical wire ingress to optical wire egress.

> [!question]- Unfold Solution
> **Cycle-Accurate Latency Budget (FPGA Pipeline @ 322.26 MHz / $T = 3.103\text{ ns}$)**:
>
> | Stage | Hardware Component | Clock Cycles | Latency (ns) |
> | :--- | :--- | :--- | :--- |
> | **1. Optical Ingress** | SFP28 Optical Photodiode conversion | Analog Optics | **~2.0 ns** |
> | **2. SerDes PMA/PCS** | GTY Transceiver CDR & 64b/66b Gearbox | Hard Transceiver | **~22.0 ns** |
> | **3. Low-Latency MAC** | Cut-Through Streaming LL-MAC | 2 Cycles | **~6.2 ns** |
> | **4. ITCH 5.0 Parser** | Pipelined 128-bit AXI4-Stream Bit-Slice | 3 Cycles | **~9.3 ns** |
> | **5. Order Book & Signal**| BRAM Top-of-Book + DSP Lead-Lag Trigger | 2 Cycles | **~6.2 ns** |
> | **6. Pre-Trade Risk Gate**| DSP Notional Multiplier & Price Collar | 1 Cycle | **~3.1 ns** |
> | **7. OUCH Serializer** | Fixed-Offset Frame Assembly | 2 Cycles | **~6.2 ns** |
> | **8. Egress MAC & SerDes**| Low-Latency TX MAC + GTY Transmitter | Hard Transceiver | **~26.0 ns** |
> | **TOTAL WIRE-TO-WIRE** | **Complete Silicon Hardware Pipeline** | **10 Cycles + PHY**| **$\mathbf{\approx 81.0\text{ ns}}$** |
>
> *Adding 50m of optical cross-connect fiber ($50\text{ m} \times 4.89\text{ ns/m} \approx 245\text{ ns}$) and ToR cut-through switch transit (~190 ns) yields a total colocation turnaround of **under 520 nanoseconds**.*

---

### Design Phase 3: Failure Modes & Asynchronous Hazard Mitigation
**Prompt**:
Identify the top operational hazards in this hybrid system and design hardware failsafes for each.

**Questions**:
1. What happens if the microwave link from Chicago drops during a storm?
2. How does the system handle an exchange TCP sequence desynchronization in pure hardware?
3. How do you prevent host CPU crashes from leaving open risk positions unhedged?

> [!question]- Unfold Solution
> **Hardware Resilience & Failsafe Engineering**:
>
> 1. **Microwave Rain Fade Failover**:
>    - The FPGA maintains a hardware watchdog timer monitoring CME MDP3 heartbeat intervals.
>    - If no packet arrives for $>100\text{ microseconds}$, the FPGA automatically switches to the backup underground fiber feed and expands its pricing spread to account for the $+6.65\text{ ms}$ fiber latency delta.
> 2. **TCP Session Desynchronization & Retransmit**:
>    - The FPGA hardware TCP offload engine (TOE) handles standard in-order TCP windowing.
>    - If an out-of-order sequence gap or packet loss occurs on the order entry line, the FPGA trips an immediate interrupt to the host CPU, which takes over the TCP socket to perform standard retransmission and historical replay.
> 3. **Host CPU Heartbeat Watchdog Kill-Switch**:
>    - The host CPU must write a monotonic heartbeat counter to an FPGA register every 10 milliseconds.
>    - If the host CPU crashes or enters a kernel freeze, the FPGA hardware watchdog timer expires, **immediately activating the hardware kill-switch and transmitting emergency OUCH mass-cancel frames to all exchanges in <150ns**.

---

## Related
- [[12 - FPGAs & Hardware Acceleration/FPGA vs CPU in Low-Latency Trading]]
- [[12 - FPGAs & Hardware Acceleration/FPGA Feed Handlers and Parsing Pipelines]]
- [[12 - FPGAs & Hardware Acceleration/Hardware Pre-Trade Risk Checks on SmartNICs]]
- [[12 - FPGAs & Hardware Acceleration/PCIe DMA Engine Design for SmartNICs]]
- [[12 - FPGAs & Hardware Acceleration/MOC - 12 FPGAs & Hardware Acceleration]]

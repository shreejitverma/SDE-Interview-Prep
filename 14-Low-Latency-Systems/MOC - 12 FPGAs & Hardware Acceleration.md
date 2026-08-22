---
tags: [trading/fpga, type/moc]
aliases: [FPGA Acceleration MOC, Hardware Acceleration MOC]
status: seed
module: 12
created: 2026-08-22
---

# MOC — 12 FPGAs & Hardware Acceleration

Hardware acceleration in electronic trading: FPGAs, SmartNICs, RTL vs. HLS, line-rate feed decoding, and hardware pre-trade risk checks.

---

## Core Concepts
- [[Notes/FPGA vs CPU in Low-Latency Trading]] — When hardware acceleration pays off: latency predictability, jitter elimination, line-rate processing.
- [[Notes/FPGA Architecture Fundamentals for Trading]] — Logic elements (LUTs), Flip-Flops, Block RAM (BRAM), UltraRAM (URAM), DSP slices, clock domains.
- [[Notes/RTL Verilog-VHDL vs High-Level Synthesis HLS]] — Direct pipelining, initiation interval ($II=1$), timing closure vs development velocity.
- [[Notes/Network MAC-PHY and Transceiver Pipeline]] — Serializer/Deserializer (SerDes), PCS/PMA layers, 10G/25G Ethernet framing, line-rate packet parsing.
- [[Notes/FPGA-Based Feed Handlers]] — Parsing ITCH/SBE directly in hardware registers; tick-to-signal generation in <100ns.
- [[Notes/Hardware Pre-Trade Risk Checks on SmartNICs]] — Dropping non-compliant packets before they hit the physical wire.
- [[Notes/PCIe DMA Engine Design]] — Host-to-card and card-to-host DMA, ring buffers in host RAM, memory-mapped I/O (MMIO) latency.

## Labs & Implementations
- [[Labs/Lab - 12 FPGA Parser and Pre-Trade Risk Filter]] — Design a synthesizable Verilog/HLS module parsing a binary header and filtering invalid order sizes at line rate.

## Drills & War Stories
- [[Drills/Drill - 12 Hybrid CPU-FPGA Architecture Design]] — System design interview: partition a trading system between FPGA (feed handler, risk) and CPU (complex alpha).
- [[Notes/War Story - The FPGA Clock Domain Crossing Glitch]] — Metastability bug in an asynchronous FIFO that corrupted outbound order prices once a week.

## Canonical Sources
- [[Sources/FPGA-Based Trading Systems Architecture]] — Deep dive into hardware pipelining for financial markets.

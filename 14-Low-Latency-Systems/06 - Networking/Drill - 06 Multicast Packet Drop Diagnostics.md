---
tags: [trading/networking, type/drill]
aliases: [Drill 06, Packet Drop Diagnostics, Multicast Troubleshooting, Microburst Diagnostics]
status: evergreen
module: 06
created: 2026-08-22
---

# Drill 06 — Multicast Packet Drop Diagnostics & Microburst Triage

> [!summary]
> Production-grade diagnostic drill simulating an urgent high-stakes trading incident: your market making engine experiences massive UDP packet drops and 15ms gap-fill freezes during the 09:30:00 US Market Open and 08:30:00 macro economic releases. Attempt each diagnostic step before unfolding the solution.

---

### Incident Scenario: The 09:30:00 Opening Cross Packet Vacuum
**Timeline**:
- **09:29:59.990**: Trading engine is quoting normally on NASDAQ Carteret.
- **09:30:00.005**: NASDAQ Opening Cross uncrosses. Market volume surges by 2,000%.
- **09:30:00.012**: Feed handler detects sequence gap at Sequence #842,100 (Missing 1,200 packets).
- **09:30:00.015**: Trading engine halts live quoting and initiates emergency TCP historical replay, taking 85 milliseconds to recover.
- **09:30:00.100**: Competitors pick off your stale resting quotes, resulting in a **\$120,000 trading loss**.

---

### Diagnostic Step 1: Physical Layer & Switch Triage
**Prompt**:
You log into the Top-of-Rack (ToR) cut-through switch (Arista 7150S) and inspect physical port counters.

**CLI Commands**:
```bash
show interfaces Ethernet 1/1 counters errors
show interfaces Ethernet 1/1 counters queue-detail
```

**Output**:
```text
Port       InErrors  OutErrors  FCS_Errors  Align_Errors  Symbol_Errors  RxPause  TxPause
Et1/1             0          0           0             0              0        0        0

Port       TxQueue    Discards (Drops)    MaxQueueDepth (Bytes)
Et1/1      Queue 0              18,420                 16,777,216 (FULL BUFFER)
```

**Questions**:
1. Are there any physical fiber or optical transceiver errors on the link?
2. What caused the 18,420 discards on Port `Et1/1`?
3. Why did the switch buffer overflow despite the switch operating in cut-through mode?

> [!question]- Unfold Solution
> 1. **Physical Link Status**: **100% clean**. `InErrors`, `FCS_Errors`, and `Symbol_Errors` are all 0, proving the optical fiber, SFP28 transceivers, and patch cables are functioning with zero bit corruption.
> 2. **Root Cause of Discards**: **Switch Egress Buffer Exhaustion (Microburst Overflow)**. The matching engine burst millions of packets into the switch faster than the 10Gbps/25Gbps host port could serialize them, filling the switch's 16MB shared SRAM buffer completely.
> 3. **Cut-Through Failure Mechanism**: Cut-Through switching only reduces latency when the egress port is idle. If multiple input ports (or a 100G exchange uplink) burst traffic simultaneously toward a single 10G host egress port, the switch is physically forced to buffer the excess volume. Once the buffer hits 16MB, subsequent packets are dropped.

---

### Diagnostic Step 2: NIC Hardware & Kernel Ring Inspection
**Prompt**:
You log into the trading server running a Solarflare / Mellanox 25G NIC and inspect hardware counters.

**CLI Commands**:
```bash
ethtool -S eth0 | grep -E "drop|discard|overflow|no_desc"
cat /proc/net/snmp | grep -i udp
```

**Output**:
```text
rx_nodesc_drops: 42,910
rx_dropped: 42,910
rx_discards: 0
rx_crc_errors: 0

Udp: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors
Udp: 142095810 0 42910 892015 0
```

**Questions**:
1. What is the specific meaning of `rx_nodesc_drops` on the NIC?
2. Why is `RcvbufErrors` equal to 0 while `rx_nodesc_drops` is 42,910?
3. Where was the packet dropped: in the Linux kernel or on the NIC hardware ASIC?

> [!question]- Unfold Solution
> 1. **Meaning of `rx_nodesc_drops`**: **RX No-Descriptor Drops**. The NIC received a valid Ethernet frame from the wire, but when its internal DMA engine attempted to transfer the packet into host RAM, the host **RX Descriptor Ring was completely empty** (no pre-posted buffers available).
> 2. **Why `RcvbufErrors == 0`**: `RcvbufErrors` measures drops occurring when the Linux kernel's socket receive buffer (`SO_RCVBUF`) overflows. Because the packets were dropped directly at the NIC hardware level before reaching the OS kernel, the kernel socket buffer never saw the packets.
> 3. **Drop Location**: The packet was dropped **on the physical NIC hardware ASIC** before PCIe DMA.

---

### Diagnostic Step 3: Architecture & Configuration Fix
**Prompt**:
Formulate the complete, production-grade engineering remediation plan across the network switch, NIC driver, and C++ feed handler to guarantee zero packet loss during future market opens.

**Questions**:
1. What NIC driver settings must be reconfigured?
2. How should the switch buffer quality-of-service (QoS) and port speeds be adjusted?
3. What architectural change must be made in the C++ feed handler software?

> [!question]- Unfold Solution
> **Comprehensive Production Remediation Plan**:
>
> 1. **NIC Descriptor Ring Expansion & Tuning**:
>    - Maximize hardware RX descriptor rings to 4,096 entries:
>      ```bash
>      sudo ethtool -G eth0 rx 4096 tx 4096
>      ```
>    - Ensure interrupt coalescing is disabled:
>      ```bash
>      sudo ethtool -C eth0 adaptive-rx off adaptive-tx off rx-usecs 0
>      ```
> 2. **Network Switch Ingress & Link Speed Alignment**:
>    - Upgrade the host server uplink to **25GbE / 100GbE** to match the exchange feed rate, eliminating switch serialization bottlenecks.
>    - Configure switch shared-buffer dynamic threshold allocation (`dynamic-threshold alpha = 8`) to grant market data multicast queues priority access to the entire 16MB SRAM buffer.
> 3. **User-Space Feed Handler Architecture (Kernel Bypass + A/B Arbitration)**:
>    - Replace standard Linux sockets with **Solarflare `ef_vi` or DPDK Poll Mode Drivers (PMD)**.
>    - Run an **A/B Feed Arbitrator** polling both Feed A and Feed B across separate physical NICs. If a microburst causes a temporary descriptor stall on Feed A, Feed B seamlessly delivers the packet, guaranteeing **100% zero-loss execution**.

---

## Related
- [[06 - Networking/Network Interface Card Architecture]]
- [[06 - Networking/UDP Multicast Market Data and A-B Feed Arbitration]]
- [[06 - Networking/Solarflare ef_vi Zero-Copy API]]
- [[06 - Networking/Switch Architectures in Trading]]
- [[06 - Networking/MOC - 06 Networking]]

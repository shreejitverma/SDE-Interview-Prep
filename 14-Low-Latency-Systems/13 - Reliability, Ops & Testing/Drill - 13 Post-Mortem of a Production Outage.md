---
tags: [trading/reliability-ops, type/drill]
aliases: [Drill 13, Production Outage Post-Mortem, 2012 BATS IPO Bug, Split-Brain Post-Mortem, Root Cause Analysis]
status: evergreen
module: 13
created: 2026-08-22
---

# Drill 13 — Production Outage Post-Mortem & Root-Cause Engineering

> [!summary]
> Principal SRE and low-latency infrastructure drill: conduct a forensic root-cause analysis (RCA) and formulate permanent engineering remediations for two catastrophic financial outages: the famous 2012 BATS IPO matching engine failure and a dual-gateway split-brain trading disaster. Attempt each section before unfolding the solutions.

---

### Case Study 1: The 2012 BATS IPO Matching Engine Collapse
**Historical Incident Summary**:
- **Date**: March 23, 2012.
- **Venue**: BATS Global Markets (the 3rd largest US stock exchange at the time).
- **Event**: BATS scheduled its own Initial Public Offering (IPO) to list on its own exchange under ticker symbol `BATS`.
- **Failure**: At 10:45:00, when the opening auction for `BATS` executed, the matching engine core crashed and entered an infinite loop, freezing trading in symbols from `A` to `BF` (including Apple `AAPL`), forcing BATS to cancel its own IPO and withdraw from the public markets in global embarrassment.

**Forensic Investigation Prompt**:
You are leading the exchange technical post-mortem.

**Questions**:
1. What was the exact software bug in the matching engine auction logic?
2. Why did the bug affect other unrelated stocks like `AAPL`?
3. Why did standard pre-production testing fail to catch the bug?

> [!question]- Unfold Solution
> **1. Root Cause Mechanism**:
> - **The Unhandled Primary Listing State Bug**: BATS was built as an Alternative Display Facility (ADF / ECN) designed to trade stocks *listed on other exchanges* (NYSE/NASDAQ). When BATS became a primary listing venue, developers added new code for handling primary IPO auction crosses.
> - A software defect in the order book auction resolver failed to handle the condition where an internal crossing order had no corresponding SIP NBBO benchmark. This caused the uncrossing loop to encounter an unchecked `NULL` pointer / state mismatch, entering an **unbounded recursive spin-loop**.
>
> **2. Cross-Symbol Contagion**:
> - The BATS matching engine partitioned stocks alphabetically across server cores (e.g. Core 1 handled symbols `A` through `BF`).
> - When the `BATS` symbol entered the infinite spin-loop, it consumed 100% of Core 1's CPU capacity, starving the event queue for all other symbols on that core (including Apple `AAPL`), bringing all trading in those symbols to a halt.
>
> **3. Testing Failure**:
> - BATS had never executed an actual primary listing IPO in its production matching engine code. The unit tests used mock order feeds that simulated continuous trading, but omitted the exact sequence of auction crosses, cross-orders, and regulatory halt state transitions unique to a primary IPO uncrossing.

---

### Case Study 2: Dual-Gateway Split-Brain Trading Disaster
**Incident Scenario**:
- **Venue**: Proprietary Market Maker colocated in Equinix NY4 (Secaucus, NJ).
- **Architecture**: Active-Active Replicated State Machine (Primary Node in Rack A, Standby Node in Rack B).
- **Incident Timeline**:
  - **14:15:00.000**: A Top-of-Rack switch firmware glitch drops the dedicated optical heartbeat link between Rack A and Rack B.
  - **14:15:00.050**: Standby node times out its 50µs heartbeat lease and **promotes itself to PRIMARY_ACTIVE**.
  - **14:15:00.052**: Primary node in Rack A is still healthy and actively trading.
  - **14:15:00.060**: Both Primary and Standby receive a market sell signal for 5,000 SPY shares. Both nodes independently format and release 5,000-share Buy orders to NASDAQ Carteret.
  - **14:15:05.000**: Both nodes continue firing duplicate orders on every tick, accumulating a **\$120,000,000 unhedged long position**, breaching all risk limits and causing a **\$1.8 million trading loss**.

**Questions**:
1. What architectural failure permitted Split-Brain execution to occur?
2. Why did the firm's pre-trade risk gate fail to block the duplicate position?
3. What 3 permanent hardware and software controls must be implemented to make Split-Brain physically impossible?

> [!question]- Unfold Solution
> **1. Architectural Root Cause**:
> - **Lack of Third-Party Quorum / Fencing (STONITH)**: The Standby relied solely on a direct 2-node heartbeat link. When the link dropped, it assumed the Primary was dead (a classic single-point-of-failure network partition) rather than verifying with a third independent witness node.
>
> **2. Risk Gate Blindspot**:
> - The pre-trade risk gates were inlined independently on each node's local CPU memory. Because each node maintained its own local position accumulator, Node A thought it held $+60\text{M}$ and Node B thought it held $+60\text{M}$ (both within the \$75M single-node limit), while the firm's real aggregate clearing position was $+\$120\text{M}$!
>
> **3. Permanent Engineering Remediations**:
>
> 1. **Hardware STONITH & Power Fencing**:
>    - Before the Standby can unmask its SFP28 optical transmitter, it must issue an out-of-band hardware command via an intelligent Switched PDU (Power Distribution Unit) to **physically cut power to the Primary node (Shoot The Other Node In The Head - STONITH)**.
> 2. **Exchange Single-Session Token Enforcement**:
>    - Configure exchange gateways with single-login session constraints. When the Standby connects, the exchange matching engine immediately severs and drops the Primary's TCP connection.
> 3. **Global Shared Risk Monitor (Drop Copy Clearing Gate)**:
>    - Ingest the exchange's real-time Drop Copy clearing feed on an independent out-of-band supervisory server. If the aggregate firm-wide net position exceeds \$75M, the monitor trips the **Tier 4 Physical Hardware Kill-Switch**, extinguishing all lasers.

---

## Related
- [[13 - Reliability, Ops & Testing/Disaster Recovery and High Availability Topologies]]
- [[13 - Reliability, Ops & Testing/Automated Kill Switches and Risk Circuit Breakers]]
- [[13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing]]
- [[02 - Exchange Architecture/Replicated State Machine Pattern in Exchanges]]
- [[13 - Reliability, Ops & Testing/MOC - 13 Reliability, Ops & Testing]]

---
tags: [trading/reliability-ops, trading/testing, type/concept]
aliases: [Exchange Simulator, Conformance Testing, AutoCert+, Impairment Injection, Microburst Simulator, Mock Matching Engine]
status: evergreen
module: 13
created: 2026-08-22
---

> [!summary]
> Exchange Simulators and Conformance Test Harnesses are software frameworks that emulate full exchange matching engines, gateway session protocols (OUCH, iLink 3, FIX), and market data feeds (ITCH, MDP3). By injecting deterministic network impairments—such as multicast packet drops, microbursts, out-of-order sequences, and socket freezes—they validate client trading system resilience before production deployment.

---

## Why it matters
Before any algorithmic trading firm is permitted to connect to a live financial exchange:
- The firm must complete mandatory **Exchange Conformance Certification** (e.g., CME AutoCert+, NASDAQ Gateway Certification, Cboe Protocol Testing).
- The firm must prove that its software handles all edge-case scenarios: **duplicate order tokens, missing sequence numbers, market halts, broken trades, and unannounced TCP resets**.

Connecting an un-tested trading engine to live exchanges risks catastrophic failures:
- Failing to handle a sequence gap will cause the engine to trade on stale order book state.
- Failing to handle a reject will trigger infinite order resubmission loops, resulting in immediate regulatory fines and exchange disconnection.

```mermaid
flowchart TD
    subgraph ClientUnderTesting ["Trading System Under Test (Participant Gateway)"]
        HFT[C++ HFT Trading Core]
    end

    subgraph ExchangeSimulatorCore ["Deterministic Exchange Simulator Harness"]
        MATCH[Mock Matching Engine Core]
        GW[Mock Gateway Session Manager (OUCH / iLink 3)]
        MKT[Mock ITCH / SBE Multicast Publisher]
        
        MATCH <--> GW
        MATCH --> MKT
    end

    subgraph ImpairmentInjector ["Synthetic Network Impairment Engine"]
        IMP["• 2.5% Multicast Drops (Test A/B Arbitration)\n• 100K Pkt Microbursts (Test Ring Buffer Sizing)\n• Out-of-Order Packet Jitter\n• Abrupt TCP Socket Disconnects"]
    end

    HFT <==|Order Entry TCP| GW
    MKT --> IMP
    IMP ==>|Impaired Market Data Multicast| HFT
```

---

## Mechanism

### 1. Mandatory Exchange Conformance Test Scenarios

| Test Category | Injected Scenario / Fault | Required Trading System Response |
| :--- | :--- | :--- |
| **Session State & Negotiation** | Invalid credentials, duplicate sequence on login. | Clean disconnect; log error; enter standby state. |
| **Duplicate Token Submission** | Submit Order with already-active `order_token`.| Ingest Rejection (`'R'`); alert risk manager. |
| **In-Flight Cancel-vs-Fill**| Exchange fills order simultaneously with Cancel. | Reconcile Fill (`'E'`); handle Cancel Reject (`'R'`). |
| **Multicast Sequence Gap** | Drop 500 contiguous ITCH packets on Feed A. | Seamlessly arbitrate to Feed B or issue TCP Gap-Fill. |
| **Market Halt / LULD Event** | Exchange emits Trading Action (`'H'`). | Freeze aggressive quoting; cancel all resting quotes. |
| **Broken Trade / Bust Print**| Exchange busts previously confirmed execution (`'B'`).| Reverse inventory position; adjust cash balance. |

### 2. The Synthetic Impairment Generator
An exchange simulator must allow scriptable injection of network faults:
- **Packet Drop Rate ($\mathbf{P_{\text{drop}}}$)**: Drops packets independently on Feed A or Feed B to test whether the client's feed arbitrator achieves **zero-loss stream reconstruction**.
- **Microburst Flooding**: Bursts 100,000 Level-2 delta updates within a **1-millisecond window** to verify that the participant's NIC RX descriptor rings do not exhaust (`rx_nodesc_drops == 0`).
- **TCP Zero-Window Freeze**: Halts TCP window acknowledgments for 5 seconds to test whether the participant's order entry socket blocks or handles backpressure gracefully without stalling the main trading thread.

---

## In Practice

### High-Speed Mock Exchange Matching & Conformance Engine in C++20

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include <random>

struct MockOrderRequest {
    uint32_t token;
    uint32_t price;
    uint32_t qty;
    char     side; // 'B' or 'S'
};

struct MockOrderResponse {
    char     msg_type; // 'A' = Accepted, 'E' = Executed, 'R' = Rejected, 'C' = Canceled
    uint32_t token;
    uint32_t exec_price;
    uint32_t exec_qty;
    uint32_t reject_code;
};

class ExchangeSimulator {
private:
    uint32_t best_resting_ask_price_{1500000}; // $150.00
    uint32_t best_resting_ask_qty_{500};
    uint64_t next_match_number_{1};

public:
    // Processes client order and returns deterministic exchange response
    MockOrderResponse submit_order(const MockOrderRequest& req) noexcept {
        // 1. Conformance Validation: Check for invalid zero quantity
        if (__builtin_expect(req.qty == 0, 0)) {
            return MockOrderResponse{'R', req.token, 0, 0, 101}; // Reject: Invalid Qty
        }

        // 2. Matching Logic: Immediate Fill against resting Ask
        if (req.side == 'B' && req.price >= best_resting_ask_price_) {
            uint32_t fill_qty = std::min(req.qty, best_resting_ask_qty_);
            best_resting_ask_qty_ -= fill_qty;

            return MockOrderResponse{'E', req.token, best_resting_ask_price_, fill_qty, 0};
        }

        // 3. Resting Order Acknowledgment
        return MockOrderResponse{'A', req.token, 0, 0, 0};
    }

    // Injects synthetic Multicast Packet Drop for Feed Handler Testing
    static bool should_drop_multicast_packet(double drop_probability, std::mt19937_64& rng) noexcept {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(rng) < drop_probability;
    }
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| Simulator Component | Throughput Capacity | Latency per Simulation Step |
| :--- | :--- | :--- |
| **In-Memory Matching Engine Core** | **>25,000,000 orders/sec** | **~25–45 ns** |
| **Synthetic Multicast Publisher** | **>40,000,000 pkts/sec** | **~15–25 ns** |
| **Network Impairment Injection** | **Line Rate (10G/25G)** | **~5–10 ns per packet** |
| **Full Conformance Suite Execution**| 10,000 Test Cases | **<2.5 Seconds Total** |

---

## Trade-offs

| Testing Environment | Setup Speed | Fidelity to Real Exchange |
| :--- | :--- | :--- |
| **In-Memory C++ Simulator** | **Instantaneous (0 ms setup)**; scriptable. | Does not model multi-participant queue queuing games. |
| **Official Exchange Testnet (AutoCert+)**| Required for final production go-live certification. | Shared environment; variable network latency; slow iterations. |
| **Hardware FPGA Mock Exchange** | Full 25G wire-speed line-rate testing. | Complex RTL development; difficult to script edge-case failures. |

---

> [!warning] Gotchas
> 1. **The Phantom Fill Conformance Trap**: If an exchange simulator confirms an execution report (`'E'`) without deducting liquidity from its simulated ITCH market data feed, the client's feed handler and book reconstructor will desynchronize, causing the client to think liquidity is still resting at that price level.
> 2. **Neglecting Partial-Fill Race Sequencing**: In real exchanges, an order can be partially filled on Tick 1, modified on Tick 2, and canceled on Tick 3. If a mock simulator only tests all-or-nothing fills, the participant's order state manager will fail in production when handling partial fill residual tracking.

---

## Lab
**Objective**: Build a C++20 exchange simulator that implements CME iLink 3 / OUCH order matching, injects 5% synthetic multicast drops and 500-microsecond TCP stalls, and runs a client trading engine through an automated 10-step conformance suite.

**Success Criteria**:
1. Execute complete 10-step conformance test suite.
2. Verify that 100% of invalid token rejections, price collars, and partial fills are correctly handled.
3. Prove that the client engine survives simulated network disconnects without state corruption.

---

> [!question]- Self-test
> 1. **What is an Exchange Conformance Test Harness and why must firms complete it before trading live?**
>    *Answer*: An Exchange Conformance Test Harness is an official testing platform provided by an exchange (e.g. CME AutoCert+) that systematically validates a participant's gateway software against standard and edge-case protocol scenarios (logins, sequence resets, gap fills, price collar rejections, and mass cancels). Completion is legally and operationally mandatory to prove that the firm's software will not corrupt exchange matching engines or flood the market with defective order flow.
> 2. **Why should an exchange simulator include synthetic network impairment injection?**
>    *Answer*: In production, financial networks suffer packet loss, microbursts, out-of-order delivery, and socket buffer stalls during market open surges. An exchange simulator with impairment injection tests the client's recovery mechanisms (A/B multicast feed arbitration, TCP gap-fill requests, non-blocking socket handling) under controlled conditions, ensuring the system remains resilient under extreme market stress.
> 3. **How does an exchange simulator test the client's handling of a "Broken Trade" (Bust) event?**
>    *Answer*: The simulator sends a standard Execution Report (`'E'`) confirming a trade, and then subsequently transmits a Broken Trade message (e.g. OUCH `'B'` or FIX `35=U`). The test verifies that the client's Order State Manager immediately rolls back the executed position, adjusts cash balances, and alerts the risk manager without throwing an unhandled exception or desynchronizing its inventory.

---

## Related
- [[13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing]]
- [[13 - Reliability, Ops & Testing/Disaster Recovery and High Availability Topologies]]
- [[10 - Protocols & Codecs/NASDAQ OUCH 4.2 Protocol Specification]]
- [[10 - Protocols & Codecs/CME iLink 3 Binary Order Entry]]
- [[13 - Reliability, Ops & Testing/MOC - 13 Reliability, Ops & Testing]]

## Sources
- [[Sources/CME AutoCert+ Conformance Testing Documentation]]
- [[Sources/NASDAQ Gateway Certification Guidelines]]
- [[Sources/How to Build an Exchange by Jane Street]]

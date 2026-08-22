---
tags: [trading/exchange-arch, trading/risk-systems, type/concept]
aliases: [Drop Copy, Clearing Feeds, Trade Capture Report, Post-Trade Risk, FIX Drop Copy, Out-of-Band Delivery]
status: evergreen
module: 02
created: 2026-08-22
---

> [!summary]
> Drop Copy and Clearing Feeds are real-time, asynchronous post-trade reporting channels that broadcast consolidated trade confirmations, order cancellations, and regulatory audit records to participant risk managers, compliance officers, and clearing houses without introducing a single nanosecond of backpressure onto the matching engine critical path.

---

## Why it matters
While a trading firm's algorithmic engines communicate over ultra-low-latency binary order entry sessions (e.g. OUCH / iLink), the firm's central risk managers, clearing brokers, and regulatory surveillance teams require a **single consolidated view of all trading activity across all accounts and trading desks**.

If the Drop Copy system is poorly architected:
- **Critical Path Contamination**: If the matching engine synchronously waits for a Drop Copy socket write before acknowledging an order, a slow or disconnected Drop Copy client will **stall the entire exchange**.
- **Risk Blindness**: A delay or sequence gap in Drop Copy delivery leaves risk managers unaware of catastrophic capital accumulation or rogue algorithmic behavior.

Modern exchanges decouple Drop Copy into an **isolated out-of-band subscriber process** consuming from the matching engine's asynchronous shared-memory event ring.

```mermaid
flowchart TD
    subgraph EngineHotPath ["Matching Engine Critical Core (<30 ns)"]
        ME["Matching Engine Core"]
        RING["Asynchronous Shared Memory Ring (Disruptor)"]
        ME ==>|Zero-Copy Push: ~15 ns (Never Blocks)| RING
    end

    subgraph FastPath ["Fast Path to Participant"]
        ME -->|Direct Binary Ack / Fill| TRADER[HFT Algorithmic Engine]
    end

    subgraph OutOfBand ["Out-of-Band Drop Copy & Clearing Subsystem (Isolated Cores)"]
        RING --> DC_ROUTER["Drop Copy Message Router"]
        
        DC_ROUTER --> TCP1["FIX Drop Copy Gateway\n(Participant Risk & Compliance)"]
        DC_ROUTER --> TCP2["Clearing Interface (TCR)\n(DTCC / OCC / CME Clearinghouse)"]
        DC_ROUTER --> JRN["NVMe Regulatory Audit Journal (CAT / OATS)"]
    end
```

---

## Mechanism

### 1. Asynchronous Out-of-Band Decoupling
To protect matching engine determinism and sub-microsecond latency:
1. When a match occurs, the matching engine writes an internal `TradeExecutionEvent` to an **in-memory SPSC / Disruptor ring buffer** in <15 nanoseconds.
2. The matching engine immediately moves to the next order. It **never opens a socket, never formats a FIX string, and never performs I/O**.
3. Dedicated **Drop Copy Gateway processes** running on separate, non-critical CPU cores poll the ring buffer, format messages into standard FIX 4.2 / 4.4 `ExecutionReport` (Tag `35=8`) or Trade Capture Reports (Tag `35=AE`), and manage TCP delivery.

### 2. Consolidated Multi-Account Fan-Out
A single institutional trading firm may operate 50 independent OUCH trading sessions across various strategies. 
The exchange's Drop Copy gateway aggregates execution reports from all 50 sessions and routes them into a **single consolidated Drop Copy TCP connection** assigned to the firm's Chief Risk Officer (CRO).

### 3. Clearing House Integration (Continuous Net Settlement)
- **Equities (DTCC / NSCC)**: Trades are submitted via real-time automated trade matching (Universal Trade Capture - UTC) and cleared via Continuous Net Settlement (CNS) at $T+1$.
- **Derivatives (CME Clearing / OCC)**: Matched futures and options trades are registered instantly with the central counterparty (CCP) for real-time novation and margin calculation.

---

## In Practice

### High-Throughput Asynchronous Drop Copy Dispatcher in C++20

```cpp
#include <cstdint>
#include <vector>
#include <string>
#include <atomic>
#include <iostream>

struct InternalTradeRecord {
    uint64_t match_id;
    uint64_t maker_order_id;
    uint64_t taker_order_id;
    uint32_t symbol_id;
    uint32_t price;
    uint32_t qty;
    uint32_t maker_firm_id;
    uint32_t taker_firm_id;
    uint64_t timestamp_ns;
};

class DropCopyDispatcher {
private:
    uint64_t drop_copy_sequence_{1};

public:
    // Formats and dispatches FIX ExecutionReport (Tag 35=8) asynchronously
    inline size_t format_fix_execution_report(char* dest_buf, 
                                              const InternalTradeRecord& trade, 
                                              uint32_t target_firm_id, 
                                              bool is_maker) noexcept {
        uint64_t order_id = is_maker ? trade.maker_order_id : trade.taker_order_id;
        char side = is_maker ? 'S' : 'B'; // Simplified side logic

        // Zero-allocation integer-to-string formatting into network buffer
        int written = snprintf(dest_buf, 512,
            "8=FIX.4.4\x01"
            "9=000\x01"
            "35=8\x01"                    // MsgType = ExecutionReport
            "34=%" PRIu64 "\x01"          // MsgSeqNum
            "49=EXCHANGE\x01"             // SenderCompID
            "56=FIRM_%u\x01"              // TargetCompID
            "37=%" PRIu64 "\x01"          // OrderID
            "17=MATCH_%" PRIu64 "\x01"    // ExecID
            "150=F\x01"                   // ExecType = Trade
            "39=2\x01"                    // OrdStatus = Filled
            "54=%c\x01"                   // Side
            "38=%u\x01"                   // OrderQty
            "44=%.2f\x01"                 // Price
            "32=%u\x01"                   // LastShares
            "31=%.2f\x01"                 // LastPx
            "10=000\x01",
            drop_copy_sequence_++,
            target_firm_id,
            order_id,
            trade.match_id,
            side,
            trade.qty,
            trade.price / 100.0,
            trade.qty,
            trade.price / 100.0
        );

        return (written > 0) ? static_cast<size_t>(written) : 0;
    }
};
```

---

## Numbers

| Subsystem / Metric | Direct Order Entry (OUCH) | Drop Copy Feed (FIX 4.4) | Regulatory Audit (CAT) |
| :--- | :--- | :--- | :--- |
| **Delivery Latency** | **<1.5 µs** | **50–250 µs** | EOD Batch / 100 ms |
| **Protocol Format** | Fixed-Offset Binary | Tag-Value ASCII FIX | JSON / CSV / Parquet |
| **Transport** | Kernel-Bypass TCP | Standard TCP/IP | Secure SFTP / S3 |
| **Critical Path Priority**| **Highest (Priority 1)** | **Low (Out-of-Band)** | Non-Critical |

---

## Trade-offs

| Drop Copy Architecture | Advantages | Operational Challenges |
| :--- | :--- | :--- |
| **Out-of-Band Shared Memory Ring** | Zero impact on matching engine latency; full fault isolation. | Drop copy process crash requires replaying memory ring. |
| **Consolidated Multi-Session Stream**| Single TCP socket gives risk managers 100% global firm visibility. | High socket bandwidth required during market volatility. |
| **Synchronous In-Line Drop Copy** | Guarantees drop copy packet arrives before trade ack. | **Fatal anti-pattern**: destroys exchange matching latency. |

---

> [!warning] Gotchas
> 1. **TCP Buffer Bloat on Drop Copy Sockets**: If a client's risk management server is under heavy load and stops reading from its Drop Copy TCP socket, the TCP receive window collapses to zero. The exchange's Drop Copy gateway must **buffer or disconnect the slow client**, never allowing TCP backpressure to propagate backward into the shared memory event ring.
> 2. **Sequence Gaps on Reconnection**: When a Drop Copy client disconnects and reconnects, it must issue a `ResendRequest (Tag 35=2)` to recover missed execution reports. The exchange Drop Copy service must maintain a persistent on-disk journal to satisfy gap fills without querying the live matching engine.

---

## Lab
**Objective**: Build an asynchronous Drop Copy dispatcher in C++ that reads execution events from a lock-free ring buffer, formats FIX 4.4 `ExecutionReport` messages, and verifies that slow TCP consumers never block the upstream producer.

**Success Criteria**:
1. Stream 1,000,000 execution records through the ring buffer.
2. Simulate a blocked/slow Drop Copy consumer and verify that the upstream producer completes processing in **<10 nanoseconds per event**.
3. Verify that all 1,000,000 FIX execution reports are accurately formatted with monotonically increasing sequence numbers.

---

> [!question]- Self-test
> 1. **What is a Drop Copy feed and why do institutional trading firms require it alongside direct order entry sessions?**
>    *Answer*: A Drop Copy feed is a real-time, read-only administrative data stream that provides consolidated trade confirmations, order executions, cancellations, and rejections across all trading sessions belonging to a firm. Trading firms require it so that central risk managers, middle-office compliance officers, and clearing operations have a single, unified view of total firm-wide positions and credit exposure in real-time.
> 2. **Why must the Drop Copy generation subsystem be completely decoupled from the matching engine critical path?**
>    *Answer*: If Drop Copy generation were on the critical path, the matching engine would have to format text-based FIX messages and perform TCP socket I/O before processing the next order. A slow, congested, or disconnected Drop Copy client would block the matching engine, injecting catastrophic milliseconds of jitter into live market operations. Decoupling via an asynchronous shared-memory ring isolates the matching engine from all post-trade I/O.
> 3. **How does an exchange Drop Copy session handle sequence gap recovery when a client reconnects after an unexpected network disconnection?**
>    *Answer*: When the client reconnects, it compares the received `MsgSeqNum (Tag 34)` against its expected sequence. If a gap is detected, the client transmits a `ResendRequest (Tag 35=2)` specifying the missing `BeginSeqNo` and `EndSeqNo`. The Drop Copy gateway retrieves the requested historical execution records from its local persistent journal and replays them to the client with the `PossDupFlag (Tag 43=Y)`.

---

## Related
- [[02 - Exchange Architecture/Exchange Gateway Architecture]]
- [[02 - Exchange Architecture/Pre-Trade Risk Checks at Wire Speed]]
- [[10 - Protocols & Codecs/NASDAQ OUCH Protocol Architecture]]
- [[09 - Messaging & IPC/The LMAX Disruptor Architecture]]
- [[02 - Exchange Architecture/MOC - 02 Exchange Architecture]]

## Sources
- [[Sources/How to Build an Exchange by Jane Street]]
- [[Sources/CME Drop Copy 2.0 Specification]]
- [[Sources/FIX Protocol Standards - FIX 4.4 Specification]]

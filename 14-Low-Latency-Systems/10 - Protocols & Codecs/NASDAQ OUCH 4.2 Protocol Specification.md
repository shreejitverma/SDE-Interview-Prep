---
tags: [trading/protocols, trading/order-entry, type/concept]
aliases: [OUCH 4.2, NASDAQ OUCH, Order Entry Protocol, Enter Order, Order Accepted, Binary OUCH]
status: evergreen
module: 10
created: 2026-08-22
---

> [!summary]
> NASDAQ OUCH 4.2 is the industry-standard, lightweight binary point-to-point order entry protocol used for ultra-low-latency order execution on NASDAQ. Designed as a minimal fixed-offset alternative to verbose ASCII FIX, OUCH enables sub-microsecond order submission, cancellation, and replacement over dedicated TCP connections.

---

## Why it matters
While market data is broadcast over UDP ITCH, participant orders must be submitted over a reliable, sequenced order entry protocol.

If a trading system submits orders using standard ASCII FIX 4.2:
- Tag-value string serialization and delimiter scanning consume **350 to 800 nanoseconds**.
- Message sizes exceed 200–350 bytes, causing extra TCP packet fragmentation.

OUCH 4.2 reduces order submission to a **compact 48-byte binary structure** that is formatted and injected into network DMA rings in **under 25 nanoseconds**.

```mermaid
flowchart LR
    subgraph ClientHost ["HFT Order Entry Engine"]
        APP[Trading Strategy Core] -->|48-Byte Binary 'O' Struct| TOE[TCP Kernel Bypass / ef_vi]
    end

    subgraph NASDAQ_Gateway ["NASDAQ OUCH Gateway (Carteret, NJ)"]
        TOE ==>|10G TCP Ingress: ~550 ns| GW[OUCH Line Handler]
        GW -->|Decodes 'O' in 15ns| ME[Matching Engine Core]
        ME -->|Emits 66-Byte 'A' (Accepted)| GW
    end

    GW ==>|TCP Return Egress| TOE
    TOE -->|Order Acknowledged| APP
```

---

## Mechanism

### 1. Inbound Client Messages (Client $\to$ Exchange)

| Type | Name | Size | Key Fields | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| **`'O'`** | **Enter Order** | 48 Bytes | `OrderToken`, `Side`, `Shares`, `Stock`, `Price`, `TIF`, `Display`, `ISO`, `MinQty` | Submit new resting limit, market, or IOC order. |
| **`'X'`** | **Cancel Order** | 19 Bytes | `OrderToken`, `Shares` | Cancel full or partial remaining quantity. |
| **`'U'`** | **Replace Order** | 39 Bytes | `ExistingToken`, `ReplacementToken`, `Shares`, `Price`, `Display`, `ISO` | Atomic price/quantity modification. |

### 2. Outbound Server Messages (Exchange $\to$ Client)

| Type | Name | Size | Key Fields | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| **`'A'`** | **Order Accepted** | 66 Bytes | `OrderToken`, `OrderRefNum`, `Side`, `Shares`, `Stock`, `Price`, `BBOWeight` | Exchange acknowledges and assigns official OrderRefNum. |
| **`'U'`** | **Order Replaced** | 79 Bytes | `ReplacementToken`, `PrevToken`, `Shares`, `Price`, `Display` | Confirmation of successful Replace request. |
| **`'C'`** | **Order Canceled** | 29 Bytes | `OrderToken`, `DecrementShares`, `Reason` | Confirmation of order cancellation (`'U'`=User, `'I'`=IOC). |
| **`'E'`** | **Order Executed** | 41 Bytes | `OrderToken`, `ExecutedShares`, `ExecPrice`, `LiquidityFlag`, `MatchNum` | Fill execution report with maker/taker rebate flag. |
| **`'R'`** | **Order Rejected** | 19 Bytes | `OrderToken`, `Reason` | Rejection notice (`'H'`=Halted, `'C'`=Closing cross). |
| **`'D'`** | **Cancel Pending** | 18 Bytes | `OrderToken` | Gateway received Cancel request, awaiting match core. |

---

## In Practice

### High-Speed Zero-Copy OUCH 4.2 Order Builder in C++20

```cpp
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

#pragma pack(push, 1)
// NASDAQ OUCH 4.2 Enter Order Message (48 bytes)
struct OuchEnterOrderMsg {
    char     message_type;      // 'O'
    uint32_t order_token;       // Client unique integer token
    char     buy_sell_indicator;// 'B' = Buy, 'S' = Sell
    uint32_t shares;            // Big-endian uint32
    char     stock[8];          // Stock right-padded with spaces
    uint32_t price;             // Big-endian fixed point ($100.50 = 1005000)
    uint32_t time_in_force;     // 0 = IOC, 99998 = Market Hours (DAY)
    char     firm[4];           // MPID identifier
    char     display;           // 'Y' = Displayed, 'N' = Non-displayed
    uint32_t capacity;          // 'A' = Agency, 'P' = Principal
    char     iso_eligibility;   // 'Y' = Intermarket Sweep Order (ISO), 'N' = No
    uint32_t min_quantity;      // 0 = No minimum
    char     cross_type;        // 'N' = Continuous, 'O' = Open, 'C' = Close
    char     customer_type;     // 'R' = Retail, 'N' = Non-retail
};

// NASDAQ OUCH 4.2 Order Executed Response (41 bytes)
struct OuchOrderExecutedMsg {
    char     message_type;      // 'E'
    uint8_t  timestamp[6];      // 48-bit nanoseconds
    uint32_t order_token;
    uint32_t executed_shares;
    uint32_t execution_price;
    char     liquidity_flag;    // 'A' = Added (Maker), 'R' = Removed (Taker)
    uint64_t match_number;
};
#pragma pack(pop)

class OuchProtocolEngine {
public:
    // Format 48-byte OUCH Enter Order directly into network TX DMA buffer in <18 ns
    static inline size_t build_enter_order(uint8_t* tx_buf,
                                           uint32_t order_token,
                                           char side,
                                           uint32_t shares,
                                           const char* symbol_8char,
                                           uint32_t price_fixed,
                                           bool is_ioc,
                                           bool is_iso,
                                           const char* firm_4char) noexcept {
        auto* msg = reinterpret_cast<OuchEnterOrderMsg*>(tx_buf);

        msg->message_type = 'O';
        msg->order_token = __builtin_bswap32(order_token);
        msg->buy_sell_indicator = side;
        msg->shares = __builtin_bswap32(shares);
        std::memcpy(msg->stock, symbol_8char, 8);
        msg->price = __builtin_bswap32(price_fixed);
        msg->time_in_force = is_ioc ? 0 : __builtin_bswap32(99998);
        std::memcpy(msg->firm, firm_4char, 4);
        msg->display = 'Y';
        msg->capacity = 'P'; // Principal (Proprietary Trading)
        msg->iso_eligibility = is_iso ? 'Y' : 'N';
        msg->min_quantity = 0;
        msg->cross_type = 'N';
        msg->customer_type = 'N';

        return sizeof(OuchEnterOrderMsg);
    }
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| Protocol / Operation | Wire Size | Serialization Overhead | Processing Method |
| :--- | :--- | :--- | :--- |
| **OUCH 4.2 Enter Order** | **48 Bytes** | **~12–18 ns** | Zero-copy direct struct fill |
| **OUCH 4.2 Cancel Order**| **19 Bytes** | **~8–12 ns** | Single 19-byte memory store |
| **ASCII FIX 4.2 NewOrderSingle**| **240–320 Bytes** | **~250–650 ns** | String formatting + tag parsing |
| **OUCH Ingress Wire-to-Ack**| Round-Trip | **<3.5 µs** | Colocated Carteret Matching |

---

## Trade-offs

| Protocol Choice | Latency Advantage | Ecosystem & Compatibility |
| :--- | :--- | :--- |
| **NASDAQ OUCH 4.2** | Sub-20ns serialization; ultra-compact binary footprint. | NASDAQ-specific; requires custom codec per exchange. |
| **CME iLink 3 (SBE)** | Binary SBE encoding; standardized schema evolution. | CME-specific; complex schema XML code generation. |
| **Standard FIX 4.2** | Universal broker compatibility across 100+ global venues. | **15x–30x slower serialization**; large network payloads. |

---

> [!warning] Gotchas
> 1. **Order Token Reuse Fatal Disconnection**: The `order_token` field is a 32-bit integer that **must be strictly unique across the entire trading session**. Re-using an `order_token` from an earlier canceled or filled order will cause the NASDAQ gateway to reject the order or instantly terminate the TCP session for protocol violation.
> 2. **Endianness on UserRefNum / Token**: The `order_token` in OUCH 4.2 is transmitted in **Big-Endian byte order**. If software writes a native Little-Endian integer without `__builtin_bswap32`, the exchange will interpret Token `1` (`0x00000001`) as Token `16,777,216` (`0x01000000`), corrupting client-side execution tracking.

---

## Lab
**Objective**: Build an OUCH 4.2 order serialization and execution report parsing engine in C++20, measure wire-to-struct encoding latency across 10,000,000 orders, and verify bitwise compliance with NASDAQ specifications.

**Success Criteria**:
1. Format 10,000,000 OUCH Enter Order messages.
2. Verify median serialization latency is **under 20 nanoseconds**.
3. Parse 10,000,000 simulated Order Accepted (`'A'`) and Order Executed (`'E'`) responses with zero memory allocations.

---

> [!question]- Self-test
> 1. **Why is the NASDAQ OUCH protocol significantly faster than standard Tag-Value ASCII FIX for order entry?**
>    *Answer*: OUCH is a fixed-offset binary protocol where every field resides at an exact, predetermined byte location within a compact 48-byte structure. Software builds the entire message by filling a C++ struct in memory with single-cycle byte swaps (`BSWAP`), requiring zero string concatenation, delimiter formatting, or dynamic allocation, executing in <20ns compared to 350–800ns for ASCII FIX.
> 2. **What is the significance of the `LiquidityFlag` field in an OUCH Order Executed (`'E'`) response?**
>    *Answer*: The `LiquidityFlag` indicates whether the execution provided liquidity (`'A'` = Added / Maker) or removed liquidity (`'R'` = Removed / Taker), as well as specific routing attributes (e.g. cross executions or midpoint matches). Trading engines use this flag to calculate real-time net exchange rebate accruals and fee deductions.
> 3. **What happens if a client submits an OUCH Replace Order (`'U'`) while the original order is simultaneously executing at the matching engine?**
>    *Answer*: The matching engine processes the trade execution first and emits an `'E'` (Order Executed) message. When the subsequent Replace request reaches the engine, the engine observes that the remaining quantity is insufficient or zero, rejecting the Replace request with an `'R'` (Rejected) or partial replace confirmation, resolving the race deterministically.

---

## Related
- [[10 - Protocols & Codecs/NASDAQ ITCH 5.0 Protocol Specification]]
- [[10 - Protocols & Codecs/CME iLink 3 Binary Order Entry]]
- [[02 - Exchange Architecture/Exchange Gateway Architecture]]
- [[01 - Market & Microstructure Fundamentals/Order Types and State Transitions]]
- [[10 - Protocols & Codecs/MOC - 10 Protocols & Codecs]]

## Sources
- [[Sources/NASDAQ OUCH 4.2 Specification]]
- [[Sources/How to Build an Exchange by Jane Street]]
- [[Sources/Trading and Exchanges by Larry Harris]]

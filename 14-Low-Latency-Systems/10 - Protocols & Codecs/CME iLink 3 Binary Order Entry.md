---
tags: [trading/protocols, trading/order-entry, type/concept]
aliases: [iLink 3, CME iLink 3, MSGW, SBE Order Entry, SOFH, Simple Open Framing Header, CME Futures Execution]
status: evergreen
module: 10
created: 2026-08-22
---

> [!summary]
> CME iLink 3 is the high-performance binary order entry protocol for CME Group futures and options markets. Replacing legacy ASCII FIX iLink 2 with Simple Binary Encoding (SBE) and Simple Open Framing Headers (SOFH), iLink 3 connects directly to partitioned Market Segment Gateways (MSGW) to achieve sub-microsecond order execution determinism.

---

## Why it matters
CME Group is the largest derivatives exchange in the world (trading S&P 500, Nasdaq 100, Eurodollar/SOFR, US Treasuries, Crude Oil, and Gold).

Under legacy iLink 2:
- Orders were submitted via verbose ASCII FIX text (`35=D|11=ORDER123|...`), requiring **300 to 700 nanoseconds** of parsing and string serialization.
- Gateways were shared across multiple symbols, causing cross-market queue contention.

iLink 3 transformed CME order routing:
- **Market Segment Gateways (MSGW)**: Dedicated, isolated gateway instances per asset class.
- **Binary SBE Serialization**: Sub-20ns in-place memory encoding.
- **Strict Inbound Sequencing**: Guarantees deterministic session sequence tracking with zero ASCII conversions.

```mermaid
flowchart TD
    subgraph ClientTradingCore ["Client HFT Futures Engine"]
        STRAT[Strategy Engine] -->|Constructs iLink 3 SBE Struct| SOFH_ENC[SOFH Framing Layer]
    end

    subgraph CME_MSGW ["CME Market Segment Gateway (Aurora, IL)"]
        SOFH_ENC ==>|Direct TCP Socket: ~500 ns| MSGW_IN[MSGW Line Handler (Core 2)]
        MSGW_IN -->|SBE Template 514 (NewOrderSingle)| RISK[CME Pre-Trade Credit Check]
        RISK --> ME[CME Matching Engine Core]
        ME -->|SBE Template 522 (ExecutionReportNew)| MSGW_OUT[Outbound Egress Handler]
    end

    MSGW_OUT ==>|TCP Ack| STRAT
```

---

## Mechanism

### 1. The Simple Open Framing Header (SOFH)
Every iLink 3 message is preceded by a **4-byte SOFH header**:
- `messageLength` (2 bytes, Big-Endian `uint16_t`): Total byte length of the message *including* the 4-byte SOFH header.
- `encodingType` (2 bytes, Big-Endian `uint16_t`): Identifies the serialization protocol (`0xEB50` represents Simple Binary Encoding - SBE).

### 2. SBE Session Layer State Machine

```text
[ Disconnected ] 
       |
       v  1. Client sends Negotiate (Template 500)
[ Negotiating ] 
       |
       v  2. MSGW returns NegotiateResponse (Template 501)
[ Negotiated ] 
       |
       v  3. Client sends Establish (Template 503)
[ Establishing ] 
       |
       v  4. MSGW returns EstablishAck (Template 504)
[ ESTABLISHED / ACTIVE TRADING ]
```

### 3. Core iLink 3 Business Messages

| Template ID | Name | Direction | Purpose |
| :--- | :--- | :--- | :--- |
| **514** | **NewOrderSingle** | Client $\to$ MSGW | Submits new limit, market, stop, or iceberg order. |
| **515** | **OrderCancelReplaceRequest**| Client $\to$ MSGW | Modifies price or quantity of resting order. |
| **516** | **OrderCancelRequest** | Client $\to$ MSGW | Cancels resting order balance. |
| **522** | **ExecutionReportNew** | MSGW $\to$ Client | Acknowledges that order is resting in book. |
| **524** | **ExecutionReportCancel** | MSGW $\to$ Client | Confirms order cancellation. |
| **525** | **ExecutionReportTradeOut**| MSGW $\to$ Client | Confirms fill execution with matched price and size. |
| **526** | **ExecutionReportReject** | MSGW $\to$ Client | Rejection notice with exact error code. |

---

## In Practice

### High-Speed iLink 3 NewOrderSingle (Template 514) Builder in C++20

```cpp
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

#pragma pack(push, 1)
// Simple Open Framing Header (SOFH: 4 bytes)
struct SofhHeader {
    uint16_t message_length; // Big-Endian
    uint16_t encoding_type;  // Big-Endian: 0xEB50
};

// SBE Message Header (8 bytes)
struct SbeHeader {
    uint16_t block_length;   // Little-Endian
    uint16_t template_id;    // Little-Endian: 514
    uint16_t schema_id;      // Little-Endian
    uint16_t version;        // Little-Endian
};

// CME iLink 3 Template 514: NewOrderSingle Root Block (Fixed fields)
struct iLink3NewOrderSingle {
    char     cl_ord_id[20];          // Unique Client Order ID string
    int64_t  price_mantissa;         // Fixed point price ($5000.25 -> 500025)
    uint32_t order_qty;              // Order quantity
    int32_t  security_id;            // CME Security ID (e.g. E-mini S&P)
    uint8_t  side;                   // 1 = Buy, 2 = Sell
    uint8_t  order_type;             // 2 = Limit, 1 = Market
    uint8_t  time_in_force;          // 0 = DAY, 3 = IOC, 4 = FOK
    uint8_t  manual_order_indicator; // 0 = Automated Algo, 1 = Manual
};
#pragma pack(pop)

class CmeILink3Engine {
public:
    // Builds complete framed iLink 3 NewOrderSingle packet in <22 ns
    static inline size_t build_new_order(uint8_t* dest_buffer,
                                         const char* cl_ord_id,
                                         int64_t price_mantissa,
                                         uint32_t qty,
                                         int32_t security_id,
                                         uint8_t side,
                                         bool is_ioc) noexcept {
        
        size_t payload_size = sizeof(SbeHeader) + sizeof(iLink3NewOrderSingle);
        size_t total_frame_size = sizeof(SofhHeader) + payload_size;

        // 1. Fill SOFH Header (Big-Endian)
        auto* sofh = reinterpret_cast<SofhHeader*>(dest_buffer);
        sofh->message_length = __builtin_bswap16(static_cast<uint16_t>(total_frame_size));
        sofh->encoding_type = __builtin_bswap16(0xEB50); // SBE Encoding

        // 2. Fill SBE Header (Little-Endian)
        auto* sbe = reinterpret_cast<SbeHeader*>(dest_buffer + sizeof(SofhHeader));
        sbe->block_length = sizeof(iLink3NewOrderSingle);
        sbe->template_id = 514; // NewOrderSingle
        sbe->schema_id = 8;     // CME iLink 3 Schema ID
        sbe->version = 1;

        // 3. Fill Business Message (Little-Endian)
        auto* order = reinterpret_cast<iLink3NewOrderSingle*>(dest_buffer + sizeof(SofhHeader) + sizeof(SbeHeader));
        std::memset(order->cl_ord_id, 0, 20);
        std::strncpy(order->cl_ord_id, cl_ord_id, 20);
        order->price_mantissa = price_mantissa;
        order->order_qty = qty;
        order->security_id = security_id;
        order->side = side;
        order->order_type = 2; // Limit
        order->time_in_force = is_ioc ? 3 : 0;
        order->manual_order_indicator = 0; // Automated Algorithmic Flow

        return total_frame_size;
    }
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| Protocol Generation | Wire Size | Serialization Overhead | Deserialization Overhead |
| :--- | :--- | :--- | :--- |
| **CME iLink 3 (Binary SBE)** | **~72 Bytes** | **~15–22 ns** | **~8–14 ns** |
| **CME iLink 2 (ASCII FIX 4.2)**| **~280 Bytes** | **~350–700 ns** | **~250–550 ns** |
| **NASDAQ OUCH 4.2** | **48 Bytes** | **~12–18 ns** | **~6–12 ns** |
| **Colocated CME Order-to-Ack**| Round-Trip | **<4.5 µs** | Aurora Data Center |

---

## Trade-offs

| Gateway Feature | Performance Advantage | Operational Overhead |
| :--- | :--- | :--- |
| **Market Segment Gateways (MSGW)** | Dedicated hardware per asset class; zero cross-symbol queuing. | Requires maintaining independent TCP connections per segment. |
| **Pre-Registered Party Details** | Strips operator and compliance IDs from live order payloads. | Requires registering party IDs during session establishment. |
| **SOFH Framing Layer** | Standard framing allows multiplexing multiple codec types. | 4-byte extra overhead on every packet. |

---

> [!warning] Gotchas
> 1. **The Mixed Endianness SOFH Hazard**: In CME iLink 3, the 4-byte **SOFH header is Big-Endian**, whereas the inner **SBE header and payload are Little-Endian**! Forgetting to byte-swap the SOFH `messageLength` while leaving the SBE payload un-swapped will cause the MSGW to instantly disconnect the session.
> 2. **CFTC Rule 1.35 Operator ID Validation**: Under CFTC regulations, if the `ManualOrderIndicator` is set to `0` (Automated), the order must link to a valid registered Operator ID (Tag 1690). If the Operator ID is missing or unregistered, the CME MSGW will reject the order immediately with code `1001`.

---

## Lab
**Objective**: Build an iLink 3 order generator and execution parser in C++20, format 10,000,000 NewOrderSingle (Template 514) packets with proper SOFH and SBE headers, and measure throughput.

**Success Criteria**:
1. Serialize 10,000,000 iLink 3 NewOrderSingle messages.
2. Verify that median serialization latency is **under 25 nanoseconds**.
3. Parse 10,000,000 ExecutionReportTradeOut (Template 525) messages with zero dynamic allocations.

---

> [!question]- Self-test
> 1. **What is a Market Segment Gateway (MSGW) in the CME iLink 3 architecture and why was it introduced?**
>    *Answer*: An MSGW is a dedicated, physically partitioned order entry gateway assigned to a specific CME market segment (such as CME Equities or CME Interest Rates). It was introduced to eliminate cross-market queuing contention, ensuring that a volume surge in Agricultural futures cannot delay order processing or inject jitter into E-mini S&P or Treasury futures trading.
> 2. **What is the structural role of the Simple Open Framing Header (SOFH) in iLink 3?**
>    *Answer*: SOFH is a 4-byte standardized transport header (`messageLength` + `encodingType`) prepended to every message. It allows network decoders and firewalls to determine the total packet size and identify the serialization protocol (`0xEB50` for SBE) without needing to parse the application-level business payload.
> 3. **Why did CME migrate from ASCII FIX iLink 2 to Binary SBE iLink 3?**
>    *Answer*: ASCII FIX required intensive CPU string parsing, delimiter scanning (`0x01`), and text-to-integer conversions, consuming 300–700ns per message and bloating packet sizes to ~300 bytes. iLink 3 uses binary SBE, which allows direct memory-aligned struct casting on x86 in <20ns and reduces packet sizes to ~72 bytes, drastically lowering network latency and processing jitter.

---

## Related
- [[10 - Protocols & Codecs/CME MDP 3.0 and Simple Binary Encoding SBE]]
- [[10 - Protocols & Codecs/NASDAQ OUCH 4.2 Protocol Specification]]
- [[10 - Protocols & Codecs/Zero-Copy and In-Place Parsing Techniques]]
- [[02 - Exchange Architecture/Exchange Gateway Architecture]]
- [[10 - Protocols & Codecs/MOC - 10 Protocols & Codecs]]

## Sources
- [[Sources/CME iLink 3 Binary Order Entry Specification]]
- [[Sources/CME Simple Binary Encoding SBE Specification]]
- [[Sources/How to Build an Exchange by Jane Street]]

---
tags: [trading/exchange-arch, trading/market-data, trading/networking, type/concept]
aliases: [Market Data Publisher, ITCH Publisher, MDP3 Publisher, Multicast Line Handler, Feed A Feed B, Packet Pacing, Snapshot Channel]
status: evergreen
module: 02
created: 2026-08-22
---

> [!summary]
> The Market Data Publisher converts private matching engine execution events into standardized, zero-copy binary multicast streams (such as NASDAQ ITCH 5.0 and CME MDP 3.0). Operating over redundant UDP Multicast A/B lines with hardware packet pacing, the publisher broadcasts real-time order book deltas, periodic snapshots, and TCP gap-fill recovery streams to thousands of market participants simultaneously in under 500 nanoseconds.

---

## Why it matters
The market data distribution system is the primary public output of an exchange. 

If the market data publisher is poorly engineered:
- **Switch Buffer Bloat & Packet Drops**: A market sweep that generates a sudden microburst of 10,000 ITCH messages can overwhelm network switch output queues, causing dropped packets across all market participants.
- **Serialization Stalls**: Inefficient framing or heap allocation in the publisher delays market updates, giving an unfair advantage to colocation participants who observe matching engine acks directly.
- **Single Point of Network Failure**: Without dual redundant A/B multicast channels, a single physical fiber cut drops market visibility for all connected trading firms.

```mermaid
flowchart TD
    subgraph EngineCore ["Matching Engine Core (Single-Writer Core)"]
        ME["Matching Engine\n(Emits Internal Trade/Order Events)"]
    end

    subgraph PublisherHost ["Market Data Publisher (Dedicated Pinned Core)"]
        RING["Shared Memory IPC Ring"]
        PACKER["Zero-Copy Binary Encoder (ITCH 5.0 / SBE)"]
        PACE["Hardware Packet Pacer / Rate Shaper"]
        
        RING --> PACKER --> PACE
    end

    subgraph MulticastEgress ["Redundant UDP Multicast Distribution"]
        PACE ==>|Feed A (VLAN 101 - Primary Switch)| SW_A[Arista 7130 Switch A]
        PACE ==>|Feed B (VLAN 102 - Secondary Switch)| SW_B[Arista 7130 Switch B]
        
        SW_A -->|UDP Multicast 233.54.12.1| PARTICIPANTS[HFT Trading Desks & Data Feeds]
        SW_B -->|UDP Multicast 233.54.12.2| PARTICIPANTS
    end

    ME ==>|Zero-Copy SHM: ~25 ns| RING
```

---

## Mechanism

### 1. Dual Multicast Feed A / Feed B Architecture
Exchanges publish two identical, independent multicast UDP streams for every market partition:
- **Feed A**: Routed across physical Network Fabric A.
- **Feed B**: Routed across physical Network Fabric B.
Both streams carry the **exact same packet payloads and sequence numbers**. If a participant drops a UDP packet on Feed A due to local buffer congestion or switch transceiver errors, its client feed handler immediately consumes the packet from Feed B with zero recovery latency.

### 2. The Tri-Channel Market Data Model

| Channel Type | Protocol / Transport | Purpose / Scope | Delivery Guarantee |
| :--- | :--- | :--- | :--- |
| **Real-Time Incremental Feed**| UDP Multicast (A/B) | Publishes real-time Add, Modify, Cancel, and Trade events. | Unreliable (NAK/A-B recovery) |
| **Snapshot Feed** | UDP Multicast / Cyclic | Periodically broadcasts full L2/L3 order book images (e.g. every 1 sec). | Cyclic loop for late joiners |
| **Historical TCP Replay** | TCP Unicast / Point-to-Point | Allows clients to request missing historical sequence ranges. | Guaranteed delivery |

### 3. Hardware Packet Pacing & Microburst Prevention
When a market sweep triggers 5,000 executions in 10 microseconds, transmitting all 5,000 UDP frames at maximum 25G line burst rate will overflow downstream Top-of-Rack (ToR) switch buffers (typically only 12–32 MB shared RAM per switch).
- **Packet Pacing**: The publisher or FPGA SmartNIC spaces out consecutive Ethernet frames by a minimum inter-packet gap (e.g. 50–100 nanoseconds), smoothing out microbursts while keeping delivery latency well under 1 microsecond.

---

## In Practice

### High-Speed Zero-Copy ITCH 5.0 Add Order Encoder in C++20

```cpp
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

#pragma pack(push, 1)
// NASDAQ TotalView-ITCH 5.0 'A' (Add Order - No MPID Attribution) Message (36 bytes)
struct ItchAddOrderMsg {
    char     message_type;      // 'A'
    uint16_t stock_locate;      // Locate code identifying symbol
    uint16_t tracking_number;   // Internal tracking number
    uint64_t timestamp_ns;      // Nanoseconds since midnight (6-byte / 8-byte)
    uint64_t order_reference_id;// Unique order reference number
    char     buy_sell_indicator;// 'B' or 'S'
    uint32_t shares;            // Shares quantity
    char     stock[8];          // Stock symbol right-padded with spaces
    uint32_t price;             // Integer price in 1/10000 dollars ($100.50 = 1005000)
};
#pragma pack(pop)

class ItchMarketDataPublisher {
public:
    // Format and serialize ITCH 5.0 Add Order directly into network DMA buffer in <15 ns
    static inline size_t format_add_order(uint8_t* dest_buffer, 
                                          uint16_t stock_locate,
                                          uint64_t timestamp_ns,
                                          uint64_t order_ref_id,
                                          char side,
                                          uint32_t shares,
                                          const char* symbol_8char,
                                          uint32_t price_fixed) noexcept {
        auto* msg = reinterpret_cast<ItchAddOrderMsg*>(dest_buffer);

        msg->message_type = 'A';
        msg->stock_locate = __builtin_bswap16(stock_locate);
        msg->tracking_number = 0;
        msg->timestamp_ns = __builtin_bswap64(timestamp_ns); // Note: ITCH 5.0 uses 48-bit; simplified here
        msg->order_reference_id = __builtin_bswap64(order_ref_id);
        msg->buy_sell_indicator = side;
        msg->shares = __builtin_bswap32(shares);
        std::memcpy(msg->stock, symbol_8char, 8);
        msg->price = __builtin_bswap32(price_fixed);

        return sizeof(ItchAddOrderMsg);
    }
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz with Solarflare XtremeScale 25G NIC.*

| Publishing Pipeline Stage | Duration | Primary Bottleneck / Technology |
| :--- | :--- | :--- |
| **Matching Event to IPC Ring** | **~15–25 ns** | Shared Memory Disruptor Ring |
| **Binary ITCH / SBE Formatting** | **~12–22 ns** | Single-cycle `BSWAP` + struct fill |
| **Kernel Bypass TX Push (`ef_vi`)**| **~180–320 ns** | Direct PCIe DMA ring push to NIC |
| **NIC PHY Egress to Optical Wire**| **~80–140 ns** | SFP28 SerDes transceiver latency |
| **Total Engine-to-Wire Latency** | **~287–507 ns** | **Sub-500ns Total Market Data Egress**|

---

## Trade-offs

| Market Data Protocol / Model | Wire Efficiency | Processing Complexity for Clients |
| :--- | :--- | :--- |
| **Order-by-Order Level 3 (ITCH)** | Maximum transparency; clients reconstruct exact book. | High bandwidth (10G/25G) and high CPU memory tracking. |
| **Price-Level Aggregated L2 (MDP3)**| Low bandwidth; aggregated top 10 price levels. | Lost visibility into individual queue positions and orders. |
| **JSON / REST / WebSocket Feeds** | Simple web integration (Crypto / Retail). | **Catastrophically slow (100–5,000 µs)**; unviable for HFT. |

---

> [!warning] Gotchas
> 1. **Timestamp Byte Truncation in ITCH 5.0**: The official NASDAQ ITCH 5.0 specification defines the timestamp as a **6-byte (48-bit) integer** representing nanoseconds since midnight. Casting an 8-byte `uint64_t` directly without masking or copying the 6 bytes will corrupt adjacent message fields on the wire.
> 2. **Snapshot Desynchronization**: When a client joins late and consumes a Snapshot to build initial state, it must buffer all real-time Incremental messages arriving concurrently. If the client applies an Incremental message whose sequence is *older* than the Snapshot sequence, it will double-count executions and corrupt book state.

---

## Lab
**Objective**: Build a high-throughput ITCH 5.0 binary market data publisher in C++20 that reads execution events from a shared memory ring buffer, serializes 10,000,000 ITCH messages with big-endian byte-swapping, and benchmarks serialization throughput.

**Success Criteria**:
1. Serialize 10,000,000 ITCH 'A' (Add Order) messages.
2. Measure per-message serialization time: verify median latency is **under 15 nanoseconds**.
3. Verify bitwise compliance against the official NASDAQ ITCH 5.0 specification.

---

> [!question]- Self-test
> 1. **Why do tier-1 exchanges publish market data over two redundant UDP Multicast channels (Feed A and Feed B) simultaneously?**
>    *Answer*: UDP is an unreliable, connectionless protocol with zero retransmission overhead. By transmitting identical, sequence-stamped packets across two physically isolated network switches and fiber paths (Feed A and Feed B), client trading systems can merge the two feeds in user-space (A/B arbitration), instantly repairing any single-packet network loss without requesting slow TCP retransmissions.
> 2. **What is the purpose of the cyclic Snapshot Feed and how does a newly connected client use it to initialize its local order book?**
>    *Answer*: The Snapshot Feed periodically broadcasts complete point-in-time order book images for all symbols. A newly connected client subscribes to both the Incremental feed (buffering incoming deltas) and the Snapshot feed. Once it receives a complete Snapshot image with sequence number $N$, it initializes its book from the snapshot, discards buffered incremental messages with sequence $\le N$, and applies incremental messages with sequence $> N$.
> 3. **Why is hardware packet pacing necessary when publishing market data during large market sweeps?**
>    *Answer*: An aggressive market sweep can generate thousands of order execution messages in a fraction of a millisecond. If published at raw maximum wire rate without pacing, the sudden microburst will exceed the packet buffer capacity of downstream network switches and client NICs, causing widespread packet drops. Packet pacing smooths transmission over microsecond intervals to prevent buffer bloat.

---

## Related
- [[06 - Networking/UDP Multicast Market Data and A-B Feed Arbitration]]
- [[10 - Protocols & Codecs/NASDAQ ITCH 5.0 Protocol Architecture]]
- [[10 - Protocols & Codecs/CME MDP 3.0 SBE Protocol Architecture]]
- [[02 - Exchange Architecture/The Sequenced-Stream Architecture]]
- [[02 - Exchange Architecture/MOC - 02 Exchange Architecture]]

## Sources
- [[Sources/NASDAQ TotalView-ITCH 5.0 Specification]]
- [[Sources/CME MDP 3.0 Market Data Specification]]
- [[Sources/How to Build an Exchange by Jane Street]]

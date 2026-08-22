---
tags: [trading/networking, trading/market-data, type/concept]
aliases: [Feed Arbitration, A-B Feed Arbitration, UDP Multicast, Sequence Gap Detection, Packet Reordering, ITCH Arbitration]
status: evergreen
module: 06
created: 2026-08-22
---

> [!summary]
> Electronic exchanges distribute market data over redundant UDP Multicast channels (Feed A and Feed B) across physically distinct network fabrics. A zero-loss Feed Arbitrator ingests both packet streams in user-space, immediately dispatching whichever packet arrives first on the wire and discarding duplicate frames in under 10 nanoseconds—eliminating packet loss without initiating slow TCP recovery requests.

---

## Why it matters
UDP Multicast is the universal transport mechanism for tier-1 exchange market data (CME MDP 3.0, NASDAQ TotalView-ITCH, Cboe PQS, Eurex EMDI).

However, because UDP lacks transmission acknowledgments:
- A single transceiver bit error, switch microburst buffer overflow, or fiber splice bend will permanently drop packets.
- If a trading system must request a **TCP Historical Gap Fill**, it incurs a **15 to 150 millisecond recovery stall**, blinding its trading algorithms during the most volatile, profitable moments of the trading day.

A real-time **A/B Feed Arbitrator** merges dual independent multicast streams in hardware or user-space software, achieving **100% zero-loss packet stream reconstruction with sub-10ns processing overhead**.

```mermaid
flowchart TD
    subgraph ExchangeEgress ["Exchange Multicast Transmitters"]
        ME["Matching Engine Core"]
        TX_A["Feed A Transmitter (VLAN 101)"]
        TX_B["Feed B Transmitter (VLAN 102)"]
        ME --> TX_A & TX_B
    end

    subgraph PhysicalNetworks ["Redundant Physical Switches & Fiber Cables"]
        TX_A ==>|Feed A: UDP Multicast (Packet #101 arrives at t=0ns)| SW_A[Switch Fabric A]
        TX_B ==>|Feed B: UDP Multicast (Packet #101 arrives at t=12ns)| SW_B[Switch Fabric B]
    end

    subgraph ClientHost ["HFT Trading Host (Solarflare / DPDK)"]
        SW_A --> RX_A[NIC Port A / VI A]
        SW_B --> RX_B[NIC Port B / VI B]
        
        subgraph ArbitratorCore ["Zero-Loss Feed Arbitrator (Core 2)"]
            ARB["A/B Feed Arbitrator Engine\n• Sees Packet #101 on Feed A -> FORWARDS to Strategy\n• Sees Packet #101 on Feed B -> DISCARDS as Duplicate in 5ns\n• If Feed A drops Packet #102: Seamlessly consumes from Feed B!"]
        end
        
        RX_A --> ARB
        RX_B --> ARB
        ARB ==>|Clean Monotonic Stream: S1, S2, S3...| STRAT[HFT Pricing & Order Book]
    end
```

---

## Mechanism

### 1. The Monotonic Sequence Invariant
Every market data multicast frame begins with a standardized sequence header (e.g., ITCH sequence or CME MDP3 `MsgSeqNum`):
$$\text{Expected Sequence} = S_{\text{expected}}$$

When packet $P$ with sequence $S$ arrives from either Feed A or Feed B:
1. **Case 1: $S == S_{\text{expected}}$ (In-Order First Arrival)**:
   - Forward packet immediately to the order book reconstructor.
   - Advance expected sequence: $S_{\text{expected}} = S + 1$.
   - Record sequence in arbitration history mask.
2. **Case 2: $S < S_{\text{expected}}$ (Duplicate Arrival from Slower Feed)**:
   - The packet has already been processed from the other feed.
   - **Immediately discard buffer (<5 nanoseconds).**
3. **Case 3: $S > S_{\text{expected}}$ (Sequence Gap Detected)**:
   - A packet was dropped on this feed or arrived out of order.
   - Place packet in a small **Circular Reorder Ring** (e.g. 64-slot window).
   - Check if the missing packet $S_{\text{expected}}$ has arrived on the alternate feed.
   - If both feeds fail to deliver $S_{\text{expected}}$ after a short timeout (e.g., $10\text{ µs}$), trigger an asynchronous **TCP Gap Fill Request**.

---

## In Practice

### High-Speed Zero-Loss A/B Feed Arbitrator in C++20

```cpp
#include <cstdint>
#include <array>
#include <iostream>
#include <cstring>

struct MarketDataHeader {
    uint32_t channel_id;
    uint32_t sequence_number; // Strictly monotonic: 1, 2, 3...
    uint64_t timestamp_ns;
};

class FastFeedArbitrator {
private:
    static constexpr size_t REORDER_WINDOW_SIZE = 128;
    static constexpr size_t REORDER_MASK = REORDER_WINDOW_SIZE - 1;

    uint32_t expected_sequence_{1};
    uint64_t total_duplicates_dropped_{0};
    uint64_t total_gaps_recovered_{0};

    // Circular buffer for temporary out-of-order packets
    struct BufferedPacket {
        uint32_t sequence{0};
        uint16_t length{0};
        uint8_t  data[1500];
    };
    std::array<BufferedPacket, REORDER_WINDOW_SIZE> reorder_buffer_;

public:
    // Process packet from either Feed A or Feed B in <12 nanoseconds
    template <typename Callback>
    inline void process_packet(const uint8_t* raw_buf, uint16_t len, Callback&& on_clean_packet) noexcept {
        const auto* hdr = reinterpret_cast<const MarketDataHeader*>(raw_buf);
        uint32_t seq = hdr->sequence_number;

        // 1. FAST PATH: Exact in-order packet (99.9% of production ticks)
        if (__builtin_expect(seq == expected_sequence_, 1)) {
            expected_sequence_++;
            on_clean_packet(raw_buf + sizeof(MarketDataHeader), len - sizeof(MarketDataHeader));

            // Drain any buffered packets that can now be delivered sequentially
            drain_reorder_buffer(on_clean_packet);
            return;
        }

        // 2. DUPLICATE PACKET: Arrived on slower redundant line
        if (__builtin_expect(seq < expected_sequence_, 1)) {
            total_duplicates_dropped_++;
            return; // Discard immediately
        }

        // 3. SEQUENCE GAP / OUT-OF-ORDER PACKET: Buffer for reordering
        if (seq > expected_sequence_) {
            size_t slot = seq & REORDER_MASK;
            BufferedPacket& b = reorder_buffer_[slot];
            b.sequence = seq;
            b.length = len;
            std::memcpy(b.data, raw_buf, len);
            total_gaps_recovered_++;
        }
    }

private:
    template <typename Callback>
    inline void drain_reorder_buffer(Callback&& on_clean_packet) noexcept {
        while (true) {
            size_t slot = expected_sequence_ & REORDER_MASK;
            BufferedPacket& b = reorder_buffer_[slot];

            if (b.sequence == expected_sequence_) {
                b.sequence = 0; // Clear slot
                on_clean_packet(b.data + sizeof(MarketDataHeader), b.length - sizeof(MarketDataHeader));
                expected_sequence_++;
            } else {
                break;
            }
        }
    }

public:
    [[nodiscard]] uint32_t expected_sequence() const noexcept { return expected_sequence_; }
    [[nodiscard]] uint64_t duplicates_dropped() const noexcept { return total_duplicates_dropped_; }
};
```

---

## Numbers

*Hardware Baseline: AMD EPYC Genoa / Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| Feed Handling Stage | Processing Latency | Operational Impact |
| :--- | :--- | :--- |
| **In-Order First Arrival Match** | **~6–12 ns** | Zero-latency immediate strategy dispatch. |
| **Duplicate Packet Discard (Slower Feed)**| **~3–5 ns** | Single branch comparison + immediate return. |
| **Out-of-Order Reorder Buffer Store**| **~15–25 ns** | In-place cache-hot circular copy. |
| **TCP Historical Replay Gap Fill** | **15,000,000–150,000,000 ns** | **Disastrous 15–150ms trading halt**. |

---

## Trade-offs

| Arbitration Implementation | Latency Advantage | Engineering / Resource Cost |
| :--- | :--- | :--- |
| **Software User-Space Arbitrator** | Sub-12ns latency; highly flexible filtering and reordering logic. | Consumes 1–2 dedicated isolated CPU cores. |
| **Hardware FPGA SmartNIC Arbitrator** | **<50ns Wire-to-Host**; zero CPU load for arbitration. | Complex RTL memory buffer management on FPGA. |
| **Single-Feed Consumer (Feed A Only)**| Saves 50% network bandwidth and 1 NIC port. | **Extreme drop hazard**: single network glitch stalls trading. |

---

> [!warning] Gotchas
> 1. **Reorder Buffer Slot Collisions**: If `REORDER_WINDOW_SIZE` is too small (e.g. 32 slots) and a network burst delivers sequence #160 while the system is stalled waiting for sequence #100, packet #160 will overwrite unread packet #128 in the circular mask. *Size the reorder window to at least 128–256 slots.*
> 2. **Feed A/B Clock Skew Misleading Metrics**: If Feed A consistently arrives 150ns earlier than Feed B due to a shorter optical patch cable, 100% of packets will be processed from Feed A. Do not assume Feed B is dead; verify that Feed B's packet counters are incrementing on the NIC RX rings.

---

## Lab
**Objective**: Build a multi-threaded A/B Multicast Feed Arbitrator in C++20, simulate simultaneous packet streaming across Feed A and Feed B with 2.0% synthetic packet drops and variable jitter, and verify **100% zero-loss reconstruction**.

**Success Criteria**:
1. Stream 10,000,000 packets through simulated Feed A and Feed B.
2. Verify that 100% of sequence numbers from $1$ to $10,000,000$ are dispatched strictly in order.
3. Assert that zero TCP gap-fill requests are triggered despite continuous single-feed packet loss.

---

> [!question]- Self-test
> 1. **Why do tier-1 exchanges broadcast market data over dual redundant UDP Multicast feeds (Feed A and Feed B) across separate physical network switches?**
>    *Answer*: UDP is an unreliable, unacknowledged transport protocol. By broadcasting identical packet streams over two physically isolated networks (Feed A and Feed B), client trading systems can merge the feeds in real-time. If a packet is dropped on Feed A due to switch congestion or fiber noise, the arbitrator seamlessly extracts it from Feed B with zero latency penalty, avoiding slow TCP historical gap fills.
> 2. **What is the fast path decision logic of an A/B Feed Arbitrator when a new packet arrives?**
>    *Answer*: The arbitrator compares the packet's sequence number $S$ against the expected sequence $S_{\text{expected}}$. If $S == S_{\text{expected}}$, the packet is in-order; the arbitrator increments $S_{\text{expected}}$ and immediately forwards the payload to the strategy engine. If $S < S_{\text{expected}}$, the packet has already been received from the alternate feed and is discarded in <5 nanoseconds as a duplicate.
> 3. **How does an out-of-order circular reorder buffer prevent premature TCP gap fills during temporary network switch packet reordering?**
>    *Answer*: If Packet #102 arrives before Packet #101 due to switch queue reordering, immediately requesting a TCP replay would waste milliseconds. Instead, the arbitrator places Packet #102 into a small circular reorder buffer. When Packet #101 arrives on either Feed A or Feed B a few microseconds later, Packet #101 is processed, and Packet #102 is immediately drained from the buffer in sequence, resolving the gap without external network requests.

---

## Related
- [[06 - Networking/Network Interface Card Architecture]]
- [[06 - Networking/Solarflare ef_vi Zero-Copy API]]
- [[06 - Networking/DPDK Architecture for Trading]]
- [[10 - Protocols & Codecs/NASDAQ ITCH 5.0 Protocol Architecture]]
- [[06 - Networking/MOC - 06 Networking]]

## Sources
- [[Sources/NASDAQ TotalView-ITCH 5.0 Specification]]
- [[Sources/CME MDP 3.0 Market Data Specification]]
- [[Sources/Solarflare ef_vi User Guide]]

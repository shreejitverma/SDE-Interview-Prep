---
tags: [trading/protocols, type/moc]
aliases: [Protocols and Codecs MOC, SBE ITCH FIX MOC]
status: seed
module: 10
created: 2026-08-22
---

# MOC — 10 Protocols & Codecs

Financial wire protocols: binary framing, zero-copy deserialization, SIMD string parsing, FIX engines, and NASDAQ/CME formats.

---

## Core Concepts
- [[Notes/NASDAQ ITCH 5.0 Protocol Specification]] — Binary packet framing, message formats (Add, Execute, Cancel, Delete, Cross), byte alignment.
- [[Notes/NASDAQ OUCH 4.2 Protocol Specification]] — Order entry binary protocol, tokenized client order IDs, inbound/outbound sequence tracking.
- [[Notes/CME MDP 3.0 and Simple Binary Encoding SBE]] — SBE framing, schema evolution, repeating groups, direct struct casting.
- [[Notes/CME iLink 3 Binary Order Entry]] — MSGW architecture, SBE session layer, sequence negotiation, party details definitions.
- [[Notes/FIX Protocol and Fast Encoding FAST]] — Tag=Value parsing overhead, FAST compression, dictionary state management.
- [[Notes/Zero-Copy and In-Place Parsing Techniques]] — Direct pointer casting, unaligned memory access penalties, endianness conversions (`bswap`).
- [[Notes/SIMD-Accelerated Text Parsing]] — Vectorized ASCII-to-integer conversion, fast timestamp parsing using AVX2/AVX-512.

## Labs & Implementations
- [[Labs/Lab - 10 Zero-Copy NASDAQ ITCH 5.0 Parser]] — Build an allocation-free, in-place C++20 ITCH parser capable of decoding >25M messages/sec.

## Drills & War Stories
- [[Drills/Drill - 10 Wire Protocol Parsing and Field Decoding]] — Decode raw hex packet captures into domain events under time pressure.
- [[Notes/War Story - The Unaligned Memory Access SIGBUS Crash]] — How casting raw network buffers directly to structs crashed SPARC/ARM and penalized x86.

## Canonical Sources
- [[Sources/NASDAQ TotalView-ITCH 5.0 Specification]] — Official binary market data specification.
- [[Sources/CME Simple Binary Encoding SBE Specification]] — High-performance binary format specification.

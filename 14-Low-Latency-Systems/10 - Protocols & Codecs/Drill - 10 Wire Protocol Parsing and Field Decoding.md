---
tags: [trading/protocols, type/drill]
aliases: [Drill 10, Protocol Decoding Drill, Hex Decoding Drill, Wire Protocol Drill]
status: evergreen
module: 10
created: 2026-08-22
---

# Drill 10 — Wire Protocol Parsing & Hex Field Decoding

> [!summary]
> Principal-level rapid-fire decoding drill testing your ability to manually parse and analyze raw network byte captures across NASDAQ ITCH 5.0, NASDAQ OUCH 4.2, CME MDP 3.0 SBE, and Tag-Value ASCII FIX under time pressure. Attempt each problem before unfolding the solution.

---

### Challenge 1: NASDAQ ITCH 5.0 Add Order (`'A'`) Hex Decode
**Raw Network Hex Capture (36 Bytes)**:
```text
41 00 65 00 00 00 00 05 20 10 00 00 00 00 00 00 04 D2 42 00 00 00 64 41 41 50 4C 20 20 20 20 00 16 F6 B8
```

**Questions**:
1. What is the Message Type, Stock Locate, and Order Reference Number?
2. What are the Side, Shares, Stock Symbol, and Limit Price?
3. What is the Timestamp in nanoseconds since midnight?

> [!question]- Unfold Solution
> **Byte-by-Byte Structural Breakdown**:
> - `[0x00]` **`41`**: Message Type = `'A'` (Add Order).
> - `[0x01..0x02]` **`00 65`**: Stock Locate = `0x0065` $\to$ **`101`**.
> - `[0x03..0x04]` **`00 00`**: Tracking Number = `0`.
> - `[0x05..0x0A]` **`00 00 05 20 10 00`**: 48-bit Big-Endian Timestamp = `0x000005201000` $\to$ **`5,635,328,000 nanoseconds`** ($\approx 5.635\text{ seconds after midnight}$).
> - `[0x0B..0x12]` **`00 00 00 00 00 00 04 D2`**: Order Reference ID = `0x04D2` $\to$ **`1,234`**.
> - `[0x13]` **`42`**: Buy/Sell Indicator = ASCII `'B'` $\to$ **`BUY`**.
> - `[0x14..0x17]` **`00 00 00 64`**: Shares = `0x00000064` $\to$ **`100 shares`**.
> - `[0x18..0x1F]` **`41 41 50 4C 20 20 20 20`**: Stock = `"AAPL    "` $\to$ **`AAPL`**.
> - `[0x20..0x23]` **`00 16 F6 B8`**: Price = `0x0016F6B8` = `1,505,000` (4 decimals) $\to$ **`$150.5000`**.
>
> **Decoded Domain Event**: `BUY 100 AAPL @ $150.50 (Order ID: 1234, Locate: 101)`.

---

### Challenge 2: NASDAQ OUCH 4.2 Order Executed (`'E'`) Hex Decode
**Raw Network Hex Capture (41 Bytes)**:
```text
45 00 00 05 20 14 00 00 00 00 0A 00 00 00 32 00 16 F6 B8 41 00 00 00 00 00 01 86 A0
```

**Questions**:
1. What is the Client Order Token?
2. What are the Executed Shares and Execution Price?
3. What is the Liquidity Flag and what is its fee/rebate implication?

> [!question]- Unfold Solution
> **Byte-by-Byte Structural Breakdown**:
> - `[0x00]` **`45`**: Message Type = `'E'` (Order Executed).
> - `[0x01..0x06]` **`00 00 05 20 14 00`**: Timestamp = `5,635,332,096 ns`.
> - `[0x07..0x0A]` **`00 00 00 0A`**: Order Token = `0x0A` $\to$ **`Token 10`**.
> - `[0x0B..0x0E]` **`00 00 00 32`**: Executed Shares = `0x32` $\to$ **`50 shares`**.
> - `[0x0F..0x12]` **`00 16 F6 B8`**: Execution Price = `1,505,000` $\to$ **`$150.50`**.
> - `[0x13]` **`41`**: Liquidity Flag = ASCII `'A'` $\to$ **`Added Liquidity (Maker Status)`**.
> - `[0x14..0x1B]` **`00 00 00 00 00 01 86 A0`**: Match Number = `0x186A0` $\to$ `100,000`.
>
> **Trading Implication**: The firm provided passive liquidity (Maker) on 50 shares of Token 10 at \$150.50 and will **receive an exchange maker rebate** (e.g. +$0.0020/share).

---

### Challenge 3: CME MDP 3.0 SBE Header & Template ID Decode
**Raw Network Hex Capture (16 Bytes)**:
```text
0B 00 2E 00 01 00 02 00 0B 00 02 00 00 00 00 00
```

**Questions**:
1. What are the `blockLength`, `templateId`, `schemaId`, and `version`?
2. Why are these fields decoded in Little-Endian byte order?
3. What business message is represented by this template ID?

> [!question]- Unfold Solution
> **Byte-by-Byte Structural Breakdown**:
> - `[0x00..0x01]` **`0B 00`**: `blockLength` = Little-Endian `0x000B` $\to$ **`11 bytes`**.
> - `[0x02..0x03]` **`2E 00`**: `templateId` = Little-Endian `0x002E` $\to$ **`Template 46`**.
> - `[0x04..0x05]` **`01 00`**: `schemaId` = Little-Endian `0x0001` $\to$ **`Schema 1`**.
> - `[0x06..0x07]` **`02 00`**: `version` = Little-Endian `0x0002` $\to$ **`Version 2`**.
>
> **Answers**:
> 1. `blockLength = 11`, `templateId = 46`, `schemaId = 1`, `version = 2`.
> 2. **Little-Endian Standard**: Simple Binary Encoding (SBE) natively encodes fields in Little-Endian to allow direct single-cycle memory loads on x86 CPUs with zero `bswap` overhead.
> 3. **Business Message**: Template 46 is the **`MDIncrementalRefreshBook`** message (CME Level-2 Order Book Delta).

---

### Challenge 4: ASCII FIX Checksum Calculation
**Raw FIX String**:
```text
8=FIX.4.2\x019=49\x0135=D\x0149=FIRM\x0156=EXCH\x0134=1\x0155=AAPL\x0154=1\x0138=100\x0144=150.00\x01
```

**Questions**:
1. How is the 3-digit Checksum computed?
2. Compute the exact integer checksum value for this message.
3. What is the formatted trailer string?

> [!question]- Unfold Solution
> 1. **Computation Rule**: Sum the ASCII integer values of every single character in the string from `'8'` up to and including the final `\x01` (SOH), then take $\text{Total Sum} \pmod{256}$.
> 2. **Calculation**:
>    - Summing the 73 ASCII bytes in the string yields total sum = **`5,296`**.
>    - $\text{Modulo 256}: 5296 \pmod{256} = \mathbf{176}$.
> 3. **Formatted Trailer**: **`10=176\x01`**.

---

## Related
- [[10 - Protocols & Codecs/NASDAQ ITCH 5.0 Protocol Specification]]
- [[10 - Protocols & Codecs/NASDAQ OUCH 4.2 Protocol Specification]]
- [[10 - Protocols & Codecs/CME MDP 3.0 and Simple Binary Encoding SBE]]
- [[10 - Protocols & Codecs/FIX Protocol and Fast Encoding FAST]]
- [[10 - Protocols & Codecs/MOC - 10 Protocols & Codecs]]

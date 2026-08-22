---
tags: [trading/protocols, trading/low-latency-cpp, type/concept]
aliases: [Zero-Copy, In-Place Parsing, Unaligned Memory, BSWAP, Split-Cache Penalty, Endianness Conversion]
status: evergreen
module: 10
created: 2026-08-22
---

> [!summary]
> Zero-copy in-place parsing allows financial feed handlers and order gateways to decode network frames by casting raw physical DMA buffers directly to C++ data structures. Eliminating memory copies (`memcpy`), handling memory alignment boundaries to prevent split-cache penalties, and executing single-cycle byte swapping (`BSWAP`) reduces protocol parsing latency to under 8 nanoseconds.

---

## Why it matters
In naive networking applications, parsing an inbound market data packet involves multiple memory copy and deserialization operations:
1. `copy_to_user()` copies from kernel to user space (**~200–400 ns**).
2. Buffer deserialization copies bytes into domain objects or strings (**~150–350 ns**).
3. Heap allocations trigger memory allocator locks and cache evictions.

In low-latency systems:
- Raw packet memory resides directly in **HugePage DMA buffers** mapped into user space.
- The C++ parser treats the memory buffer as a **read-only binary view**, extracting integers and timestamps via direct CPU register loads.
- **Zero dynamic memory allocation and zero buffer copies occur.**

```mermaid
flowchart TD
    subgraph NaiveApproach ["1. Naive Deserialization (~450 - 900 ns)"]
        N1[Raw Network Buffer] -->|memcpy()| N2[Temporary Stack Buffer]
        N2 -->|Field Deserialization| N3[Heap Object: new Order()]
        N3 --> N4[Domain Logic]
    end

    subgraph ZeroCopyApproach ["2. Zero-Copy In-Place Memory View (<8 ns)"]
        Z1["HugePage DMA Buffer (Read-Only)"]
        Z2["reinterpret_cast<const ItchAddOrder*>(buffer)"]
        Z3["Single-Cycle MOV + BSWAP to CPU Registers"]
        
        Z1 --> Z2 --> Z3
    end
```

---

## Mechanism

### 1. Pointer Casting & Zero-Copy Struct Mapping
Instead of copying individual bytes into local variables, the parser overlays a packed C++ struct on top of the raw packet memory:

```cpp
const auto* order = reinterpret_cast<const OuchEnterOrder*>(raw_dma_buffer);
uint32_t price = __builtin_bswap32(order->price);
```

### 2. The Unaligned Memory Access Penalty
Modern x86-64 CPUs permit reading integers from unaligned memory addresses (e.g. reading an 8-byte `uint64_t` from an odd address like `0x1003`).

However:
- If an unaligned 8-byte integer **crosses a 64-byte L1 cache line boundary** (e.g. bytes 60 through 67), the CPU must fetch **two separate cache lines** and execute internal micro-op bit merging.
- **The Split-Line Cache Penalty**: Adds **10 to 25 nanoseconds** of CPU stall time to every unaligned read.
- On strict RISC architectures (ARM / SPARC), an unaligned load can trigger a hardware `SIGBUS` exception, crashing the trading process!

### 3. High-Speed Endianness Conversion (Big-Endian vs Little-Endian)
- **Big-Endian (Network Byte Order)**: Most Significant Byte first (used in ITCH, OUCH, FIX).
- **Little-Endian (Host Byte Order)**: Least Significant Byte first (x86-64 native architecture).

To convert Big-Endian network integers with zero latency, use compiler intrinsics that map directly to the single-cycle x86 `BSWAP` instruction:

| C++ Compiler Intrinsic | x86-64 Assembly Instruction | Execution Latency |
| :--- | :--- | :--- |
| `__builtin_bswap16(val)` | `ROL ax, 8` / `xchg ah, al` | **1 cycle (~0.25 ns)** |
| `__builtin_bswap32(val)` | `BSWAP eax` | **1 cycle (~0.25 ns)** |
| `__builtin_bswap64(val)` | `BSWAP rax` | **1 cycle (~0.25 ns)** |
| `_mm256_shuffle_epi8(v, mask)` | `VPSHUFB ymm` (AVX2) | **1 cycle (Swaps 32 bytes in parallel!)** |

---

## In Practice

### High-Performance In-Place Zero-Copy Parser Pattern in C++20

```cpp
#include <cstdint>
#include <cstring>
#include <iostream>
#include <immintrin.h>

#pragma pack(push, 1)
struct RawBinaryOrderMsg {
    uint8_t  msg_type;
    uint32_t order_id; // Big-Endian
    uint64_t timestamp;// Big-Endian
    uint32_t price;    // Big-Endian
    uint32_t qty;      // Big-Endian
};
#pragma pack(pop)

class InPlaceParser {
public:
    // Decodes packet fields zero-copy directly into CPU registers in <6 nanoseconds
    static inline void process_order_in_place(const uint8_t* raw_buffer) noexcept {
        // Direct pointer cast over DMA buffer (Zero Copy)
        const auto* msg = reinterpret_cast<const RawBinaryOrderMsg*>(raw_buffer);

        // Hardware single-cycle byte swaps into local CPU registers
        uint32_t order_id = __builtin_bswap32(msg->order_id);
        uint64_t timestamp = __builtin_bswap64(msg->timestamp);
        uint32_t price = __builtin_bswap32(msg->price);
        uint32_t qty = __builtin_bswap32(msg->qty);

        // Execute trading logic directly on register values...
        asm volatile("" :: "r"(order_id), "r"(timestamp), "r"(price), "r"(qty) : "memory");
    }

    // Vectorized SIMD byte-swap for 4 simultaneous 64-bit Big-Endian integers
    static inline __m256i simd_bswap_epi64(__m256i input_vec) noexcept {
        // Byte reversal shuffle mask for 64-bit integers
        const __m256i shuffle_mask = _mm256_setr_epi8(
            7, 6, 5, 4, 3, 2, 1, 0,
            15, 14, 13, 12, 11, 10, 9, 8,
            7, 6, 5, 4, 3, 2, 1, 0,
            15, 14, 13, 12, 11, 10, 9, 8
        );
        return _mm256_shuffle_epi8(input_vec, shuffle_mask);
    }
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz.*

| In-Place Parsing Operation | Execution Time (Cycles) | Latency (Time) | Compiler Assembly Emitted |
| :--- | :--- | :--- | :--- |
| **`reinterpret_cast` Pointer Shift**| **0 cycles** | **0.00 ns** | Virtual pointer arithmetic (No CPU op) |
| **Aligned 64-bit Read (`MOV`)** | **1 cycle** | **~0.25 ns** | `MOV rax, [rdi + offset]` |
| **`__builtin_bswap64` (Big $\to$ Little)**| **1 cycle** | **~0.25 ns** | `BSWAP rax` |
| **Unaligned Cache-Split Read** | **15–35 cycles** | **~3.75–8.75 ns** | Two L1d line fetches + micro-op merge |
| **Naive `memcpy` (64 bytes)** | **12–25 cycles** | **~3.00–6.25 ns** | Vector register loads and stores |

---

## Trade-offs

| Engineering Choice | Latency Benefit | Risk / Structural Hazard |
| :--- | :--- | :--- |
| **Direct Struct Pointer Cast** | Sub-6ns execution; zero heap allocations. | Undefined Behavior (UB) if memory is not correctly typed or aligned. |
| **Packed Structs (`#pragma pack(1)`)**| Exact mapping to compact wire protocols. | Potential unaligned access cache penalties across 64-byte boundaries. |
| **Manual Padding Alignment** | Guarantees natural alignment for all fields. | Requires protocol designer to add explicit padding bytes on the wire. |

---

> [!warning] Gotchas
> 1. **C++ Strict Aliasing Violation**: Casting a `char*` or `uint8_t*` buffer to a concrete struct pointer (`OrderMsg*`) technically violates C++ Strict Aliasing rules unless the source type is `char`, `unsigned char`, or `std::byte`. *Always compile with `-fno-strict-aliasing` in low-latency C++ to prevent compiler optimization dead-code eliminations.*
> 2. **Buffer Lifetime Invalidation**: In a zero-copy parser, the returned pointers point directly into the NIC's DMA ring buffer. If the application forwards this pointer to another thread and immediately recycles the DMA buffer, the receiving thread will read overwritten packet bytes! *Complete all field processing before recycling the buffer.*

---

## Lab
**Objective**: Build a C++20 benchmark comparing naive `memcpy` deserialization against zero-copy in-place pointer casting with `BSWAP`, measuring decoding latency and throughput across 20,000,000 messages.

**Success Criteria**:
1. Measure latency of `memcpy` vs in-place struct pointer casting.
2. Demonstrate that zero-copy parsing reduces decoding latency by **$>75\%$**.
3. Verify that zero unaligned split-cache penalties occur.

---

> [!question]- Self-test
> 1. **What is a Split-Line Cache Penalty and how does unaligned memory access cause it?**
>    *Answer*: A split-line cache penalty occurs when an unaligned integer (e.g. an 8-byte `uint64_t` located at memory offset 60) straddles two separate 64-byte L1 cache lines. To read the single 8-byte integer, the CPU's memory execution unit must fetch both 64-byte cache lines from L1d and execute internal micro-ops to merge the split bytes into a single register, injecting a 10 to 25 nanosecond stall into the execution pipeline.
> 2. **Why should low-latency financial systems be compiled with the `-fno-strict-aliasing` flag?**
>    *Answer*: In C++, the Strict Aliasing Rule states that two pointers of different types cannot point to the same memory location (with exceptions for `char*` and `std::byte*`). In zero-copy parsing, casting raw network byte buffers directly to custom struct pointers (`reinterpret_cast<OrderMsg*>(buffer)`) can lead the compiler's optimizer to assume the pointers never alias, resulting in aggressive dead-code elimination or out-of-order memory stores that corrupt data.
> 3. **How does the x86 `BSWAP` instruction convert Big-Endian network integers to native Little-Endian format in a single clock cycle?**
>    *Answer*: The `BSWAP` (Byte Swap) CPU instruction reverses the byte order of a 32-bit or 64-bit general-purpose register in hardware in a single clock cycle (0.25 ns). By using compiler intrinsics (`__builtin_bswap32` / `__builtin_bswap64`), the compiler emits native `BSWAP` instructions rather than generating multi-instruction shift-and-mask loops.

---

## Related
- [[10 - Protocols & Codecs/NASDAQ ITCH 5.0 Protocol Specification]]
- [[10 - Protocols & Codecs/CME MDP 3.0 and Simple Binary Encoding SBE]]
- [[04 - Hardware Mechanical Sympathy/CPU Cache Hierarchy and Line Alignment]]
- [[08 - Low-Latency Programming/Allocation-Free Steady State Patterns]]
- [[10 - Protocols & Codecs/MOC - 10 Protocols & Codecs]]

## Sources
- [[Sources/Intel 64 and IA-32 Architectures Software Developer's Manual]]
- [[Sources/Systems Performance by Brendan Gregg]]
- [[Sources/How to Build an Exchange by Jane Street]]

# Chapter 48: Bit Manipulation and `bit_cast`

> *C++20's `<bit>` header standardizes the bit-twiddling that systems programmers previously wrote with compiler intrinsics, `reinterpret_cast`, or undefined-behavior `union` punning. `std::bit_cast` reinterprets an object's bits as another type **safely and at compile time**; the power-of-two and bit-counting functions (`popcount`, `bit_width`, `countl_zero`, `rotl`, …) expose the CPU instructions that hot code depends on; and `std::endian` finally makes byte order a first-class, queryable property. This chapter covers type punning done right, the bit-counting catalogue, and the endianness facility.*

For decades, "read the bits of a `float` as a `uint32_t`" had no legal spelling in standard C++. `reinterpret_cast<uint32_t&>(f)` violates strict aliasing; the `union` trick is undefined behavior in C++ (legal in C); and `memcpy` is correct but verbose and not `constexpr` before C++20. Meanwhile `popcount`, `clz`, and bit-rotation were available only as `__builtin_*` intrinsics or `_mm_*` instructions, non-portable across compilers. C++20's `<bit>` closes both gaps: `bit_cast` for safe, `constexpr` type punning, and a complete catalogue of bit operations that compile to single CPU instructions.

---

## Table of Contents

- [48.1 std::bit_cast: Type Punning Without UB](#481-stdbit_cast-type-punning-without-ub)
- [48.2 Why bit_cast Beats reinterpret_cast and unions](#482-why-bit_cast-beats-reinterpret_cast-and-unions)
- [48.3 Power-of-Two Operations](#483-power-of-two-operations)
- [48.4 Bit Counting: popcount, countl/countr, bit_width](#484-bit-counting-popcount-countlcountr-bit_width)
- [48.5 Bit Rotation: rotl and rotr](#485-bit-rotation-rotl-and-rotr)
- [48.6 std::endian: Querying Byte Order](#486-stdendian-querying-byte-order)
- [48.7 Putting It Together: A Serialization Primitive](#487-putting-it-together-a-serialization-primitive)
- [48.8 Professional Insights](#488-professional-insights)

---

## 48.1 std::bit_cast: Type Punning Without UB

`std::bit_cast<To>(from)` (header `<bit>`) reinterprets the bit representation of `from` as an object of type `To`. It requires both types to have the **same size** and to be **trivially copyable**, and it is `constexpr` — the canonical, defined replacement for bit-level reinterpretation.

```cpp
// Listing 48.1: reinterpreting the bits of a float as an integer
#include <bit>
#include <cstdint>

// The classic IEEE-754 inspection: read a float's bits as a uint32_t.
float f = 1.0f;
std::uint32_t bits = std::bit_cast<std::uint32_t>(f);   // 0x3F800000

// And back the other way — at compile time:
constexpr float one = std::bit_cast<float>(0x3F800000u);  // 1.0f, in constexpr

// Round-trips are exact because no value conversion occurs, only reinterpretation.
static_assert(std::bit_cast<std::uint32_t>(1.0f) == 0x3F800000u);
```

`bit_cast` produces a *new object* of the destination type whose object representation is copied byte-for-byte from the source — semantically `memcpy` into a fresh `To`, but as an expression and usable in `constexpr`. The requirements (`sizeof(To) == sizeof(From)`, both trivially copyable) are enforced at compile time, so the dangerous cases (mismatched sizes, non-trivial types) are build errors rather than silent corruption.

---

## 48.2 Why bit_cast Beats reinterpret_cast and unions

Each pre-C++20 approach to type punning is either undefined behavior or unusable in a constant expression. `bit_cast` is the only option that is simultaneously defined, `constexpr`, and concise.

```cpp
// Listing 48.2: the four approaches, ranked
#include <bit>
#include <cstdint>
#include <cstring>

float f = 3.14f;

// (1) reinterpret_cast — VIOLATES strict aliasing; UB, may miscompile at -O2.
// std::uint32_t a = *reinterpret_cast<std::uint32_t*>(&f);   // DON'T

// (2) union punning — UB in C++ (reading the inactive member); legal in C only.
// union { float f; std::uint32_t u; } pun{f};                // DON'T (in C++)

// (3) memcpy — DEFINED and correct, but verbose and NOT constexpr (pre-C++20).
std::uint32_t c;
std::memcpy(&c, &f, sizeof c);                                // OK, but clunky

// (4) bit_cast — DEFINED, concise, AND constexpr. The winner.
std::uint32_t d = std::bit_cast<std::uint32_t>(f);           // USE THIS
```

The `reinterpret_cast` and `union` routes are undefined behavior — they happen to work at `-O0` and then break when the optimizer assumes a `float*` and a `uint32_t*` cannot alias. `memcpy` is correct but cannot appear in a constant expression and reads poorly. `bit_cast` is the single approach that is defined, optimizes to the same zero-cost move as the `memcpy` (compilers lower it to a register move or nothing), and works at compile time.

---

## 48.3 Power-of-Two Operations

`<bit>` provides a family of functions for power-of-two reasoning on unsigned integers — essential for allocators, hash tables, and ring buffers that round capacities to powers of two.

```cpp
// Listing 48.3: power-of-two helpers
#include <bit>
#include <cstdint>

std::has_single_bit(8u);     // true  — is it a power of two? (exactly one bit set)
std::has_single_bit(6u);     // false

std::bit_ceil(5u);           // 8     — smallest power of two >= 5
std::bit_ceil(8u);           // 8
std::bit_floor(5u);          // 4     — largest power of two <= 5
std::bit_floor(8u);          // 8

std::bit_width(5u);          // 3     — bits needed to represent 5 (0b101)
std::bit_width(0u);          // 0
std::bit_width(255u);        // 8

// Typical use: round a requested capacity up to the next power of two.
std::size_t round_up_pow2(std::size_t n) {
    return n <= 1 ? 1 : std::bit_ceil(n);
}
```

These replace the error-prone hand-rolled loops and the "smear the high bit down with shifts" idioms. `bit_ceil` is the standard way to size a hash table or ring buffer; `bit_width(x)` equals `floor(log2(x)) + 1` and gives the number of significant bits. All operate on unsigned integer types only (signed inputs are ill-formed), and all are `constexpr`.

---

## 48.4 Bit Counting: popcount, countl/countr, bit_width

The counting functions expose the CPU's population-count and count-leading/trailing-zero instructions portably. They are `constexpr` and compile to single instructions (`POPCNT`, `LZCNT`, `TZCNT` on x86) when available.

```cpp
// Listing 48.4: the bit-counting catalogue
#include <bit>
#include <cstdint>

std::uint8_t x = 0b0010'1100;   // 44

std::popcount(x);        // 3  — number of set (1) bits
std::countl_zero(x);     // 2  — leading (most-significant) zero bits
std::countl_one(x);      // 0  — leading one bits
std::countr_zero(x);     // 2  — trailing (least-significant) zero bits
std::countr_one(x);      // 0  — trailing one bits

// Practical uses:
bool is_pow2  = std::popcount(x) == 1;          // power-of-two test
int  highest  = 7 - std::countl_zero(x);        // index of highest set bit
int  lowest   = std::countr_zero(x);            // index of lowest set bit
```

| Function | Returns |
|----------|---------|
| `popcount(x)` | count of 1-bits (Hamming weight) |
| `countl_zero(x)` | consecutive 0-bits from the most significant end |
| `countr_zero(x)` | consecutive 0-bits from the least significant end |
| `countl_one` / `countr_one` | same, counting 1-bits |
| `bit_width(x)` | `0` if `x==0`, else `1 + floor(log2(x))` |

`popcount` powers sparse-set cardinality, bitboard evaluation (chess engines), and SIMD mask analysis; `countr_zero` finds the lowest set bit for free-list allocators; `countl_zero` computes integer logarithms. Because they map to dedicated instructions, they are dramatically faster than the loop-based equivalents in hot code.

---

## 48.5 Bit Rotation: rotl and rotr

`std::rotl` and `std::rotr` perform **circular** shifts — bits shifted off one end re-enter the other — compiling to the `ROL`/`ROR` instructions. Unlike the naive `(x << n) | (x >> (W - n))` idiom, they are defined for all rotation counts (including 0 and ≥ width) with no UB.

```cpp
// Listing 48.5: circular shifts
#include <bit>
#include <cstdint>

std::uint8_t x = 0b1001'0000;

std::rotl(x, 1);    // 0b0010'0001 — top bit wrapped around to the bottom
std::rotr(x, 1);    // 0b0100'1000 — bottom bit wrapped to the top
std::rotl(x, 0);    // unchanged — defined, no UB
std::rotl(x, 8);    // unchanged (full rotation) — defined
std::rotr(x, 9);    // == rotr(x, 1) — counts are taken modulo the width

// The hand-rolled version is UB when n == 0 (shift by width) — rotl/rotr are not.
```

The manual `(x << n) | (x >> (W - n))` is undefined behavior when `n == 0`, because shifting an integer by its full width is UB — a real bug that `rotl`/`rotr` eliminate by defining the count modulo the bit width. These are the building blocks of hash functions (e.g. xxHash, SipHash), cryptographic primitives, and CRC routines, and the standard versions are both correct and optimal.

---

## 48.6 std::endian: Querying Byte Order

`std::endian` makes byte order a compile-time-queryable enumeration. You compare `std::endian::native` against `std::endian::little`/`big` to branch on the platform's byte order without preprocessor hackery.

```cpp
// Listing 48.6: querying and acting on byte order
#include <bit>
#include <cstdint>

if constexpr (std::endian::native == std::endian::little) {
    // little-endian path (x86, most ARM)
} else if constexpr (std::endian::native == std::endian::big) {
    // big-endian path (some network gear, older architectures)
}

// A portable "host to big-endian (network order)" for a 32-bit value:
constexpr std::uint32_t to_big_endian(std::uint32_t v) {
    if constexpr (std::endian::native == std::endian::big)
        return v;
    else
        return std::byteswap(v);   // NOTE: std::byteswap is C++23, not C++20
}
```

`std::endian` itself is C++20; the `if constexpr` form compiles only the relevant branch, so there is no runtime cost. **Version trap:** `std::byteswap` (used above to reverse bytes) is **C++23**, *not* C++20 — in C++20 you swap bytes manually with shifts and masks or a compiler intrinsic. On a mixed-endian system `native` equals neither `little` nor `big`, which is why explicit comparison (rather than assuming "not little means big") is the safe pattern.

---

## 48.7 Putting It Together: A Serialization Primitive

The `<bit>` facilities combine naturally in the kind of low-level serialization code where they earn their place.

```cpp
// Listing 48.7: reading a big-endian float from a byte buffer (C++20)
#include <bit>
#include <cstdint>
#include <array>
#include <cstring>

float read_be_float(const std::byte* p) {
    // Gather 4 bytes into a uint32 in big-endian order (no byteswap needed).
    std::uint32_t bits =
        (std::uint32_t(std::to_integer<std::uint8_t>(p[0])) << 24) |
        (std::uint32_t(std::to_integer<std::uint8_t>(p[1])) << 16) |
        (std::uint32_t(std::to_integer<std::uint8_t>(p[2])) <<  8) |
        (std::uint32_t(std::to_integer<std::uint8_t>(p[3])));
    // Reinterpret the assembled bits as a float — safely, possibly at compile time.
    return std::bit_cast<float>(bits);
}
```

Assembling the integer with explicit shifts is endian-independent by construction (we place each byte at a known significance), so no `endian` branch or `byteswap` is needed; `bit_cast` then turns the bit pattern into a `float` without UB. This is the modern, portable replacement for the `union`/`reinterpret_cast` serialization code that pervades older network and file-format libraries.

---

## 48.8 Professional Insights

**Replace every `reinterpret_cast`/`union` type pun with `std::bit_cast`.** The old idioms are undefined behavior that survives `-O0` and breaks under optimization when the compiler assumes the two pointer types cannot alias — among the nastiest "works in debug, fails in release" bugs. `bit_cast` is defined, enforces the size and trivial-copyability preconditions at compile time, lowers to the same zero-cost move, and works in `constexpr`. There is no remaining reason to pun bits any other way in C++20.

**Use the bit-counting functions instead of hand-rolled loops in hot paths.** `popcount`, `countl_zero`, `countr_zero`, and friends map to dedicated CPU instructions (`POPCNT`, `LZCNT`, `TZCNT`); the loop equivalents are an order of magnitude slower and the optimizer often cannot recover the intrinsic from the loop. In bitset cardinality, allocator free-list scanning, hash evaluation, and SIMD mask processing this is a direct, measurable win — and the code reads as intent rather than as a clever shift sequence.

**Prefer `rotl`/`rotr` over the shift-or idiom — it is not merely cleaner, it fixes a real UB.** `(x << n) | (x >> (W - n))` is undefined behavior when `n == 0` (a shift by the full width), a latent bug in countless hand-written hash and CRC routines. `std::rotl`/`std::rotr` define the count modulo the width, so all rotation amounts are valid, and they compile to a single `ROL`/`ROR`. Adopt them wherever rotation appears in cryptographic or hashing code.

**Query byte order with `std::endian` and `if constexpr`, never the preprocessor.** `if constexpr (std::endian::native == std::endian::little)` compiles only the taken branch with zero runtime cost and full type checking of both paths, unlike `#ifdef __BYTE_ORDER__` which is compiler-specific and invisible to the type system. Always compare explicitly against both `little` and `big`; mixed-endian platforms make `native` equal to neither, so "not little implies big" is unsound.

**Remember the C++20/23 line in `<bit>`: `std::byteswap` is C++23.** The byte-counting and bit-cast machinery is all C++20, but the convenient `std::byteswap` that reverses an integer's bytes did not arrive until C++23. Under a strict C++20 build you reverse bytes with shifts and masks, a compiler intrinsic (`__builtin_bswap*`), or by assembling bytes in the desired order as in Listing 48.7 — and code copied from C++23 examples calling `std::byteswap` will not compile.

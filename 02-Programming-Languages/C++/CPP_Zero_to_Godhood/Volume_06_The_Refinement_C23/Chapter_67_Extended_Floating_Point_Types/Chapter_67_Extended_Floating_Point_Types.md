# Chapter 67: Extended Floating-Point Types

> For decades C++ offered exactly three floating-point types — `float`, `double`, and `long double` — whose bit widths were implementation-defined. That was fine until machine learning, graphics, and signal-processing hardware standardized on *specific* formats: IEEE half precision, the brain-float format, and explicit 32/64/128-bit floats. C++23's new header **`<stdfloat>`** finally gives portable, fixed-width names for these: `std::float16_t`, `std::float32_t`, `std::float64_t`, `std::float128_t`, and `std::bfloat16_t`. This chapter covers what each type is, when to reach for the narrow ones, and the conversion and performance rules that govern mixing them with the classic types.

## Table of Contents

1. [Why Fixed-Width Floats Were Missing](#671-why-fixed-width-floats-were-missing)
2. [The `<stdfloat>` Types](#672-the-stdfloat-types)
3. [`bfloat16_t` vs `float16_t`: Range vs Precision](#673-bfloat16_t-vs-float16_t-range-vs-precision)
4. [Literals, Conversions, and the Usual Arithmetic Conversions](#674-literals-conversions-and-the-usual-arithmetic-conversions)
5. [Performance and Hardware Acceleration](#675-performance-and-hardware-acceleration)
6. [Professional Insights](#676-professional-insights)

---

## 67.1 Why Fixed-Width Floats Were Missing

The classic floating-point types are defined by *minimum* guarantees, not exact layouts. The standard says `double` has at least as much precision as `float` and `long double` at least as much as `double`, but the actual bit widths are the implementation's choice: `long double` is 80-bit extended on x86 Linux, 64-bit (identical to `double`) on MSVC, and 128-bit on some other platforms. For ordinary numeric code this elasticity is harmless, but it makes three things impossible to write portably:

1. **Binary interchange.** Serializing a `double` and reading it on another platform works only because `double` happens to be IEEE-754 binary64 almost everywhere — a convention, not a guarantee. There was no *named* "exactly binary32" type.
2. **Narrow-precision computing.** ML inference and training run on 16-bit floats for memory and bandwidth, and GPUs/TPUs/CPUs expose half-precision and brain-float instructions. C++ had no standard name for these formats at all, forcing vendor extensions (`__fp16`, `_Float16`) or library wrappers.
3. **Guaranteed wide precision.** Code needing a true 128-bit float could not portably ask for one — `long double` *might* be 128-bit, or might be 64 or 80.

`<stdfloat>` resolves all three by giving each IEEE/industry format a portable, fixed-width type alias.

---

## 67.2 The `<stdfloat>` Types

The header defines up to five optional type aliases. Each is provided only if the implementation supports that format in hardware or software, and each comes with a matching feature-test macro (`__STDCPP_FLOAT16_T__`, `__STDCPP_BFLOAT16_T__`, etc.) so you can detect availability.

| Type | Format | Total bits | Exponent / Mantissa bits | Primary use |
|------|--------|-----------:|--------------------------|-------------|
| `std::float16_t` | IEEE 754 binary16 (half) | 16 | 5 / 10 | Graphics, ML storage, bandwidth-bound data |
| `std::bfloat16_t` | bfloat16 ("brain float") | 16 | 8 / 7 | ML training/inference (wide range) |
| `std::float32_t` | IEEE 754 binary32 | 32 | 8 / 23 | Portable "exactly `float`" |
| `std::float64_t` | IEEE 754 binary64 | 64 | 11 / 52 | Portable "exactly `double`" |
| `std::float128_t` | IEEE 754 binary128 (quad) | 128 | 15 / 112 | High-precision scientific computing |

Two points are essential. First, these are **distinct types**, not merely typedefs to `float`/`double` — even when `std::float32_t` has the same layout as `float`, it is a separate type for overload resolution, so you can write overloads that target exactly binary32. Second, **availability is optional**: a freestanding microcontroller toolchain may define none of them, so portable code must guard on the feature-test macros.

**Listing 67.1: Detecting and using the fixed-width types.**

```cpp
#include <stdfloat>
#include <print>

int main() {
#if defined(__STDCPP_FLOAT32_T__)
    std::float32_t a = 1.5f32;          // exactly IEEE binary32, portably
    std::float64_t b = 3.141592653589793f64;
    std::println("a={} b={}", a, b);
#else
    std::println("float32_t not supported on this implementation");
#endif
}
```

---

## 67.3 `bfloat16_t` vs `float16_t`: Range vs Precision

The two 16-bit types are *not* interchangeable; they make opposite trade-offs within the same 16 bits, and choosing the wrong one is a real correctness hazard in numeric code.

- **`std::float16_t` (IEEE half)** spends 5 bits on the exponent and 10 on the mantissa. It has *more precision* (about 3–4 significant decimal digits) but a *narrow dynamic range* (max ≈ 65504). It is the right choice for normalized data — graphics colors, audio samples, activations bounded to a known range.
- **`std::bfloat16_t` (brain float)** spends 8 bits on the exponent — *exactly the same range as `float`* — and only 7 on the mantissa. It has *less precision* (about 2–3 significant digits) but the *full dynamic range of `float32`*. This is why ML training adopted it: gradients span enormous magnitudes and overflow/underflow is the killer, whereas a little precision loss per element is absorbed by the statistics of training. A bonus is that truncating a `float32` to `bfloat16` is nearly a matter of dropping the low 16 bits, since the exponent fields line up.

**Listing 67.2: The same value, two 16-bit formats.**

```cpp
#include <stdfloat>
#include <print>

int main() {
#if defined(__STDCPP_FLOAT16_T__) && defined(__STDCPP_BFLOAT16_T__)
    // A large magnitude: representable in bfloat16 (float range) but
    // OVERFLOWS IEEE half (max ~65504).
    std::bfloat16_t big_bf = 1.0e30bf16;   // fine: bfloat16 has float's range
    std::float16_t  big_h  = 1000.0f16;    // fine; 1.0e30 would overflow to inf
    std::println("bf={} half={}", static_cast<float>(big_bf),
                                  static_cast<float>(big_h));
#endif
}
```

The rule of thumb: **pick `bfloat16_t` when dynamic range matters (ML, anything that scales widely); pick `float16_t` when precision-per-bit matters and values are bounded (graphics, sensor data).**

---

## 67.4 Literals, Conversions, and the Usual Arithmetic Conversions

Each fixed-width type has a **literal suffix**: `f16`, `bf16`, `f32`, `f64`, and `f128` (e.g. `2.5f32`, `0.1bf16`). The suffixes let you write a literal of exactly the intended type without a conversion.

The conversion rules are deliberately stricter than the classic types' freewheeling implicit promotions, precisely to prevent silent precision loss:

- **Widening conversions** (e.g. `float16_t` → `float32_t` → `float64_t`) are implicit and safe — no information is lost.
- **Narrowing conversions** (e.g. `float64_t` → `float16_t`) require an *explicit* cast; the language will not silently throw away precision.
- **The usual arithmetic conversions** were extended so that mixing a fixed-width type with a classic type, or two different fixed-width types, in one expression has well-defined rules — but where there is *no* common type that can represent both without loss (for instance, mixing `float16_t` with `long double` on some implementations), the expression is **ill-formed** rather than silently converting. You must cast explicitly to say what you mean.

**Listing 67.3: Literals and the widening/narrowing distinction.**

```cpp
#include <stdfloat>

int main() {
#if defined(__STDCPP_FLOAT16_T__) && defined(__STDCPP_FLOAT64_T__)
    std::float16_t h = 1.5f16;
    std::float64_t d = h;                       // OK: widening is implicit

    // std::float16_t back = d;                 // ERROR: narrowing needs a cast
    std::float16_t back = static_cast<std::float16_t>(d);   // explicit, intentional
    (void)back;
#endif
}
```

---

## 67.5 Performance and Hardware Acceleration

The reason these types matter for systems and HPC work is that they map onto **hardware instructions**, not software emulation — when the target supports them:

- **Memory and bandwidth.** A 16-bit type halves the footprint of an array versus `float` and quarters it versus `double`. For bandwidth-bound kernels — and most large numeric kernels are bandwidth-bound, not compute-bound — that translation directly into roughly 2×–4× throughput, because more elements fit in cache and per cache line, and twice as many stream through a SIMD register per instruction.
- **Native SIMD/accelerator paths.** Modern CPUs (AVX-512 FP16, ARM with FP16/BF16 extensions) and every ML accelerator have native half- and brain-float arithmetic. Using `float16_t`/`bfloat16_t` lets the compiler emit those instructions directly instead of widening to `float`, computing, and narrowing back.
- **The emulation cliff.** When the hardware does *not* support a format, the type may still be provided via software emulation — and then arithmetic on it is *dramatically slower* than `float`, not faster. This is the key performance trap: a 16-bit type is a win only as a compact storage format and/or on hardware with native support. On hardware without it, narrow types should be used for *storage* (to save bandwidth) with computation done after widening to `float`.

> **Version-trap flag:** `<stdfloat>` and all five `std::floatNN_t`/`std::bfloat16_t` aliases, plus their `f16`/`bf16`/`f32`/`f64`/`f128` literal suffixes, are **C++23**. Every type is *optional* — guard on `__STDCPP_FLOAT16_T__`, `__STDCPP_BFLOAT16_T__`, `__STDCPP_FLOAT32_T__`, `__STDCPP_FLOAT64_T__`, and `__STDCPP_FLOAT128_T__`. Do not confuse these with the pre-standard vendor spellings `_Float16` / `__fp16` / `__bf16`, which have different rules.

---

## 67.6 Professional Insights

**Use `std::float32_t`/`std::float64_t` whenever the bit layout is part of a contract.** Any time a float crosses a boundary — a wire protocol, a file format, a GPU buffer, a foreign-function interface, a memory-mapped device register — you are implicitly relying on an exact IEEE layout. The classic `float`/`double` give you that only by near-universal convention; the fixed-width aliases give it to you by guarantee, and because they are distinct types you can write overloads and `static_assert`s that enforce the format. For interchange code, the fixed-width names document and enforce the contract that `double` only implied.

**Choose between `bfloat16_t` and `float16_t` by dynamic range, not by name similarity.** They occupy the same 16 bits but trade range against precision in opposite directions, and the wrong pick produces silent infinities or silent precision collapse. `bfloat16_t` keeps `float`'s full exponent range and is the default for machine learning, where overflow is fatal and a little mantissa loss is absorbed by training. `float16_t` keeps more mantissa and is right for bounded, normalized data like graphics and audio. Decide based on the magnitude span your values actually take.

**Treat narrow floats as a storage and bandwidth optimization first, an arithmetic format second.** The reliable, portable win from 16-bit types is halving or quartering memory traffic in bandwidth-bound kernels; the additional win — native half/bfloat arithmetic — exists only where the hardware supports it, and *vanishes into a slowdown* under software emulation. The robust pattern is to store in the narrow type and, on platforms without native support, widen to `float` for the math. Always gate on the feature-test macros so the code compiles and runs correctly on toolchains that provide none of these types.

**Let the stricter conversion rules protect you rather than casting them away reflexively.** The standard makes narrowing between extended floats require an explicit cast precisely because silent precision loss is one of the most insidious numeric bugs. When the compiler demands a `static_cast` to narrow, treat it as a prompt to confirm the loss is intended — and where mixing types is ill-formed, write the explicit conversion that states which precision you actually want, rather than reaching for a wider type to make the error disappear.

# Chapter 44: Attributes for Performance and Safety

> *C++20 adds three attributes that matter to performance-critical and correctness-critical code: `[[likely]]` and `[[unlikely]]` give the optimizer branch-probability hints it can use for code layout, `[[no_unique_address]]` lets empty members occupy zero bytes so wrappers and allocators stop paying for nothing, and `[[nodiscard("reason")]]` attaches an explanatory message to the existing discard warning. This chapter covers what each attribute actually does at the machine level, when it helps, and the traps that make `[[likely]]` in particular easy to misuse.*

Attributes are advisory annotations the compiler may act on. The three covered here are the ones with real, measurable consequences for systems code: branch hints steer the layout of hot and cold paths in the instruction stream; `[[no_unique_address]]` is the difference between a stateless comparator costing a byte (plus padding) and costing nothing; and the `[[nodiscard]]` message turns a generic warning into actionable guidance. Used well they sharpen both speed and safety; used carelessly — especially the branch hints — they can pessimize the very paths they were meant to help.

---

## Table of Contents

- [44.1 [[likely]] and [[unlikely]]](#441-likely-and-unlikely)
- [44.2 What the Branch Hints Actually Do](#442-what-the-branch-hints-actually-do)
- [44.3 When to Use Branch Hints — and When Not To](#443-when-to-use-branch-hints--and-when-not-to)
- [44.4 [[no_unique_address]]](#444-no_unique_address)
- [44.5 [[no_unique_address]] in Practice: EBO Without Inheritance](#445-no_unique_address-in-practice-ebo-without-inheritance)
- [44.6 [[nodiscard]] with a Message](#446-nodiscard-with-a-message)
- [44.7 Attribute Placement and Portability](#447-attribute-placement-and-portability)
- [44.8 Professional Insights](#448-professional-insights)

---

## 44.1 [[likely]] and [[unlikely]]

`[[likely]]` and `[[unlikely]]` mark a statement or branch as the expected (or unexpected) execution path, hinting to the optimizer how to lay out code. They are the standardized form of GCC/Clang's `__builtin_expect`.

```cpp
// Listing 44.1: branch-probability hints on if/else and switch
int process(int x) {
    if (x > 0) [[likely]] {        // hint: this branch is usually taken
        return fast_path(x);
    } else [[unlikely]] {          // hint: this branch is rare
        return slow_path(x);
    }
}

int classify(int code) {
    switch (code) {
        case 0: [[likely]]   return ok();
        case 1: [[unlikely]] return rare_error();
        default:             return other();
    }
}
```

The attribute is placed on the branch statement (or `case` label) whose probability you are asserting. `[[likely]]` says "optimize for this being taken"; `[[unlikely]]` says "this is the exceptional path — move it out of the way." They express a claim about runtime behavior that the compiler otherwise can only guess at without profile data.

---

## 44.2 What the Branch Hints Actually Do

The hints influence **code layout and register/spill decisions**, not (directly) the hardware branch predictor. Modern CPUs predict branches dynamically at runtime; the attributes instead help the compiler arrange the *static* instruction stream.

Concretely, a compiler acting on `[[likely]]`/`[[unlikely]]` typically:

- **Lays the likely path inline (fall-through)** and moves the unlikely path to a cold section at the end of the function, improving instruction-cache density on the hot path — the unlikely code does not pollute the cache lines the hot loop touches.
- **Biases the conditional jump** so the common case is the not-taken (fall-through) direction, which is cheaper and aids the front-end before dynamic prediction warms up.
- **Prioritizes the hot path for register allocation**, spilling in the cold path instead.

```cpp
// Listing 44.2: the mental model — cold paths get exiled
void handle(Request& r) {
    if (r.malformed()) [[unlikely]] {
        log_and_reject(r);    // compiler moves this block out-of-line (cold section)
        return;
    }
    // The common path stays inline, cache-dense, fall-through:
    serve(r);
}
```

The win is real but bounded: it is primarily an **instruction-cache and code-layout** optimization, most visible in tight, frequently-executed functions where keeping the hot path compact matters. It does not make a single branch "faster" in isolation.

---

## 44.3 When to Use Branch Hints — and When Not To

Branch hints are easy to apply and easy to misapply. The discipline:

- **Use them only where you are certain and the path is hot.** Error-checking branches that are virtually never taken (`if (ptr == nullptr) [[unlikely]]`, malformed-input rejection, overflow guards) are the textbook good cases. The hint codifies knowledge the compiler genuinely lacks.
- **Do not sprinkle them speculatively.** A *wrong* hint pessimizes: marking a branch `[[likely]]` that is actually rare exiles the real hot path to the cold section, the opposite of what you want. An incorrect hint is worse than none.
- **Prefer Profile-Guided Optimization (PGO) when available.** PGO feeds the compiler real measured branch frequencies, which are more accurate and more complete than hand annotations. Reserve `[[likely]]`/`[[unlikely]]` for cases where PGO is impractical, or for paths so obviously one-sided (assertions, error exits) that no profiling is needed.
- **Measure.** Because the effect is layout-level, the only proof is a benchmark and, ideally, a look at the generated assembly. Treat an unmeasured branch hint as a hypothesis, not a fact.

In short: branch hints are a precision tool for known-cold error paths and hot inner loops, not a general decoration — and PGO supersedes them whenever you can run a representative workload.

---

## 44.4 [[no_unique_address]]

`[[no_unique_address]]` tells the compiler that a non-static data member **need not have a distinct address** from other members, so an *empty* member can occupy **zero bytes**. This brings the Empty Base Optimization (EBO) to member subobjects, which previously had to occupy at least one byte.

```cpp
// Listing 44.3: an empty member costs zero bytes with [[no_unique_address]]
#include <cstddef>

struct Empty {};   // no data members — size 1 by default (objects must be addressable)

struct WithoutAttr {
    Empty e;       // occupies >= 1 byte, plus padding
    int   value;
};

struct WithAttr {
    [[no_unique_address]] Empty e;   // may occupy 0 bytes, overlapping 'value'
    int   value;
};

static_assert(sizeof(WithoutAttr) >= sizeof(int) + alignof(int));  // padded larger
static_assert(sizeof(WithAttr)    == sizeof(int));                 // Empty vanished
```

Without the attribute, every member must have a unique address, so an empty member forces at least one byte (and usually padding to the next member's alignment). With `[[no_unique_address]]`, the empty member can share an address with the following member and contribute nothing to the object's size. If the member is *not* empty, the attribute has no effect.

---

## 44.5 [[no_unique_address]] in Practice: EBO Without Inheritance

The attribute's killer application is **stateless function objects** — comparators, hashers, deleters, allocators — stored as members. Historically, library authors inherited from these empty types (the EBO trick) to avoid the size cost; `[[no_unique_address]]` achieves the same thing by composition, which is cleaner and more flexible.

```cpp
// Listing 44.4: a container storing a (usually empty) comparator at zero cost
#include <functional>

template<class T, class Compare = std::less<T>>
class SortedBox {
    T* data_;
    std::size_t size_;
    [[no_unique_address]] Compare comp_;   // empty std::less<T> costs nothing

public:
    bool less(const T& a, const T& b) const { return comp_(a, b); }
};

// sizeof(SortedBox<int>) == sizeof(T*) + sizeof(size_t) — the comparator is free.
// If Compare is a stateful lambda or functor, it occupies its real size as normal.
```

This is exactly how standard-library implementations keep `std::vector`'s allocator, `std::map`'s comparator, and `unique_ptr`'s deleter free when they are stateless, without the awkward private-inheritance EBO pattern. For your own generic wrappers — handle types, scope guards, policy-based designs — `[[no_unique_address]]` on the policy member is the idiomatic C++20 way to pay zero for statelessness while keeping the member a normal, named, composed subobject. (Note: MSVC spells it `[[msvc::no_unique_address]]` for ABI reasons; see Section 44.7.)

---

## 44.6 [[nodiscard]] with a Message

C++17's `[[nodiscard]]` warns when a function's return value is ignored. C++20 lets you attach an **explanatory string**: `[[nodiscard("reason")]]`, which the compiler includes in the diagnostic so the caller learns *why* the value matters.

```cpp
// Listing 44.5: [[nodiscard]] with an actionable message
struct [[nodiscard("handle this error code")]] ErrorCode { int value; };

[[nodiscard("leaking this handle leaks the resource")]]
Handle acquire();

[[nodiscard("comparison has no effect if discarded")]]
bool empty() const;

void caller() {
    acquire();        // warning: ignoring return value of 'acquire':
                      //          leaking this handle leaks the resource
    ErrorCode ec = do_work();
    // (void)ec;      // discarding even the type triggers the warning
}
```

The message can be applied to a function or to a **type** (every function returning that type then warns on discard, as with `ErrorCode` above). The value over bare `[[nodiscard]]` is purely communicative: "ignoring return value" is generic, while "leaking this handle leaks the resource" tells the developer the consequence and the fix. Use it on error-code returns, RAII-handle factories, and pure observers (`empty()`, `size()`) whose discarded result almost always signals a bug.

---

## 44.7 Attribute Placement and Portability

Attributes have specific grammatical positions, and two of these three have portability caveats worth knowing:

- **Placement.** `[[likely]]`/`[[unlikely]]` attach to statements and labels; `[[no_unique_address]]` to a non-static data member declaration; `[[nodiscard]]` to a function declaration or a class/enum type. Misplacement is typically ignored (attributes are designed to be safely unknown) but may warn.
- **Unknown attributes are ignored.** The standard requires that an unrecognized standard-form attribute be ignored, not rejected — so code using a newer attribute still compiles on an older toolchain, just without the effect. This is what makes attributes safe to adopt incrementally.
- **`[[no_unique_address]]` and ABI.** Because it changes object layout, `[[no_unique_address]]` is an **ABI-affecting** attribute. MSVC, to preserve its existing ABI, ignores the standard spelling and provides `[[msvc::no_unique_address]]` instead. Cross-platform code that depends on the layout effect must account for this (often via a macro selecting the right spelling).

```cpp
// Listing 44.6: portable spelling for the layout attribute
#if defined(_MSC_VER)
  #define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
  #define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

struct Wrapper {
    NO_UNIQUE_ADDRESS std::less<int> comp;   // zero-cost on all major compilers
    int value;
};
```

The other two attributes are pure hints/diagnostics and carry no ABI implications — they are safe to use unconditionally.

---

## 44.8 Professional Insights

**Apply `[[likely]]`/`[[unlikely]]` only to known-cold error paths and proven-hot loops, and verify with a benchmark.** The attributes are a code-layout optimization, so a wrong hint actively pessimizes by exiling the real hot path. The safe, high-value uses are unambiguous: null checks, overflow guards, malformed-input rejection (`[[unlikely]]`) and the dominant case of a tight dispatch loop (`[[likely]]`). For everything else, prefer Profile-Guided Optimization, which measures rather than guesses — and never ship a branch hint you have not confirmed in the generated assembly or a microbenchmark.

**Reach for `[[no_unique_address]]` on every stateless policy member in generic code.** Comparators, hashers, deleters, and allocators are usually empty, and without the attribute each one silently costs a byte plus alignment padding — multiplied across millions of small objects, that is real memory and cache pressure. The attribute delivers the Empty Base Optimization via clean composition instead of private inheritance. Make it the default for policy members in your containers, smart pointers, and scope guards.

**Guard `[[no_unique_address]]` for MSVC — it is ABI-affecting.** MSVC ignores the standard spelling and offers `[[msvc::no_unique_address]]`; cross-platform layout-sensitive code needs a macro to select the right one. Equally, remember that adding or removing this attribute *changes the ABI* of a type, so it is not a safe drop-in change for a type that crosses a stable binary boundary. Decide on it at design time.

**Always supply a reason string with `[[nodiscard]]`.** The bare attribute produces a generic "ignoring return value" warning; the message form tells the developer the consequence and the remedy ("leaking this handle leaks the resource"). Put `[[nodiscard("…")]]` on error-code returns, RAII-handle factories, and pure observers, and apply it to the *type* when every function returning it should warn — the marginal cost is a few words, and it converts a vague warning into a fix.

**Treat attributes as safe-to-adopt because unknown ones are ignored.** The standard mandates that unrecognized standard attributes be ignored rather than rejected, so newer annotations degrade gracefully on older compilers. This lets you adopt `[[likely]]` and `[[nodiscard("…")]]` across a codebase without version-gating — the worst case on an old toolchain is that the hint or message simply has no effect, never a build failure. The one exception is the ABI-affecting `[[no_unique_address]]`, where the *effect*, not just the recognition, must be considered per platform.

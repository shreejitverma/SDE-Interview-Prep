# Chapter 58: `std::generator` — The First Standard Coroutine

> C++20 shipped coroutines as a *language* feature — `co_await`, `co_yield`, `co_return` — but provided no library types to use them with. To write even a trivial lazy sequence you had to hand-author a promise type, an iterator, and the suspension plumbing: dozens of lines of subtle boilerplate before you could `co_yield` a single value. C++23 fixes the most common case by shipping **`std::generator<T>`**, a ready-made coroutine return type for synchronous, lazy sequences. It is the first concrete coroutine type in the standard library, it models a `view`, and it plugs directly into ranges. For anyone who tried coroutines in C++20 and recoiled at the boilerplate, this chapter is the payoff.

## Table of Contents

1. [The Coroutine Library Gap in C++20](#581-the-coroutine-library-gap-in-c20)
2. [`std::generator<T>` at a Glance](#582-stdgeneratort-at-a-glance)
3. [Writing a Generator: `co_yield`](#583-writing-a-generator-co_yield)
4. [Generators Are Views: Ranges Integration](#584-generators-are-views-ranges-integration)
5. [Reference, Value, and the Two Type Parameters](#585-reference-value-and-the-two-type-parameters)
6. [Recursive Generators with `co_yield ranges::elements_of`](#586-recursive-generators-with-co_yield-ranges-elements_of)
7. [Performance: The Cost of Suspension](#587-performance-the-cost-of-suspension)
8. [Professional Insights](#588-professional-insights)

---

## 58.1 The Coroutine Library Gap in C++20

A coroutine is a function that can suspend and later resume, preserving its local state across the suspension. C++20 standardized the *machinery* — the compiler transformation that splits the function at each `co_await`/`co_yield`, the coroutine frame that stores the locals, and the customization points (`promise_type`, awaiters). What it did **not** standardize was any usable type to return. The language gave you the engine; the standard library gave you no car to put it in.

The practical effect was brutal. To produce the canonical "lazy generator" — a function that yields values one at a time, computing each only when asked — a C++20 programmer had to write the entire support apparatus by hand: a `promise_type` with `yield_value`, `initial_suspend`, `final_suspend`, and exception handling; an `iterator` that resumes the coroutine on `++`; `begin`/`end`; and careful management of the `coroutine_handle`'s lifetime. This is perhaps 60–80 lines of intricate, easy-to-get-wrong code for the *simplest possible* use of a coroutine. Unsurprisingly, most teams either copied a generator from a blog post or avoided coroutines entirely.

`std::generator<T>` (header `<generator>`) is the standard library finally supplying that car.

---

## 58.2 `std::generator<T>` at a Glance

`std::generator<Ref, Value, Allocator>` is a coroutine return type representing a **synchronous** sequence of elements produced lazily via `co_yield`. "Synchronous" distinguishes it from a future asynchronous generator: it never `co_await`s; it only produces values. Its defining properties:

- **It is a `view`.** `std::generator` models `std::ranges::view` and `input_range`, so it composes with the entire ranges pipeline (`| views::take(...)`, range-based `for`, range algorithms).
- **It is move-only.** It owns the coroutine frame; copying would be meaningless, so it is non-copyable but movable.
- **It is single-pass.** Like any `input_range`, you traverse it once; the iterator is an input iterator.
- **It is lazy.** No element is computed until the consumer advances the iterator. An infinite generator is perfectly legal as long as the consumer stops pulling.

You write the function body with `co_yield`, declare the return type as `std::generator<T>`, and the library supplies everything that used to be boilerplate.

---

## 58.3 Writing a Generator: `co_yield`

The mechanics are now trivial. Declaring the return type as `std::generator<T>` turns the function into a coroutine; each `co_yield expr` suspends the coroutine and surfaces `expr` as the next element; resuming continues after the `co_yield`.

**Listing 58.1: An infinite Fibonacci generator.**

```cpp
#include <generator>
#include <ranges>
#include <print>

std::generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {                 // legitimately infinite: it is lazy
        co_yield a;                // suspend here, hand 'a' to the consumer
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    // Pull only the first ten values; the generator never runs past them.
    for (int x : fibonacci() | std::views::take(10))
        std::print("{} ", x);
    std::println("");
}
```

The `while (true)` loop is not a bug: because the generator is lazy and single-pass, it computes exactly as many Fibonacci numbers as the consumer requests. The `views::take(10)` adaptor stops pulling after ten, so the loop body runs ten times and the coroutine is then destroyed mid-flight, its frame cleaned up automatically.

---

## 58.4 Generators Are Views: Ranges Integration

The decision to make `std::generator` model `view` is what makes it powerful rather than merely convenient. Because a generator *is* a range, every range adaptor and algorithm works on it directly — you can pipe a generator through `filter`, `transform`, `take`, `drop`, `chunk`, and the rest, and you can feed it to `std::ranges::for_each`, `ranges::to`, or a range-based `for` loop.

**Listing 58.2: Composing a generator with the ranges pipeline.**

```cpp
#include <generator>
#include <ranges>
#include <vector>
#include <print>

std::generator<int> naturals() {
    for (int i = 1; ; ++i) co_yield i;     // 1, 2, 3, ...
}

int main() {
    // Lazily: first 5 even squares, materialized into a vector.
    auto result = naturals()
        | std::views::filter([](int n){ return n % 2 == 0; })
        | std::views::transform([](int n){ return n * n; })
        | std::views::take(5)
        | std::ranges::to<std::vector>();    // C++23, see Chapter 60

    std::println("{}", result);              // [4, 16, 36, 64, 100]
}
```

Nothing in this pipeline runs eagerly; each stage pulls exactly the elements the downstream `take(5)` demands. This is the C++23 realization of the lazy-sequence programming model that languages like Python (with generators) and C# (with `yield return`) have had for years — now with native code generation and no per-element heap allocation in the pipeline itself.

---

## 58.5 Reference, Value, and the Two Type Parameters

The convenient spelling `std::generator<T>` is shorthand. The full template is `std::generator<Ref, Value = void, Allocator = void>`, and understanding the first two parameters matters when you yield large objects.

- **`std::generator<T>`** — yields elements whose *reference type* is `T&&` and whose *value type* is `T`. Yielding `co_yield x;` produces a reference to `x`; the element is not copied unless the consumer copies it.
- **`std::generator<const T&>`** — yields by `const` reference, useful when each element is an expensive-to-copy object that the consumer only reads.
- **`std::generator<Ref, Value>`** — decouples the reference type from the value type, needed in generic code where the natural reference (e.g. a proxy) differs from the materialized value type.

The key performance consequence: because the reference type defaults to `T&&`, **yielding does not force a copy**. `co_yield big_object;` hands the consumer a reference into the coroutine frame's local; a copy happens only if the consumer asks for one. For generators of large structs this is the difference between a cheap view and a copy per element.

---

## 58.6 Recursive Generators with `co_yield ranges::elements_of`

A naive recursive generator — one that, to yield a subtree, loops over a child generator and re-yields each element — pays an O(depth) cost at every level, because each yielded element bubbles up through every enclosing coroutine. C++23 provides `std::ranges::elements_of` precisely to flatten this: `co_yield std::ranges::elements_of(inner_range);` yields *all* the elements of `inner_range` (which may itself be a generator) directly, with the library short-circuiting the nested resumption.

**Listing 58.3: A recursive tree traversal generator.**

```cpp
#include <generator>
#include <vector>
#include <ranges>
#include <print>

struct Node {
    int value;
    std::vector<Node> children;
};

std::generator<int> preorder(const Node& n) {
    co_yield n.value;
    for (const Node& child : n.children)
        co_yield std::ranges::elements_of(preorder(child));  // flatten the subtree
}

int main() {
    Node tree{1, {{2, {{4, {}}, {5, {}}}}, {3, {}}}};
    for (int v : preorder(tree))
        std::print("{} ", v);          // 1 2 4 5 3
    std::println("");
}
```

`elements_of` makes recursive coroutines composable and avoids the quadratic re-yielding cost that the hand-rolled C++20 equivalent suffered.

> **Version-trap flag:** `std::generator` and `std::ranges::elements_of` are C++23, in `<generator>`. C++20 had the coroutine *keywords* but no `<generator>` header. Library availability lagged the core language here — `std::generator` arrived in libstdc++ and MSVC before libc++; check `__cpp_lib_generator` before relying on it.

---

## 58.7 Performance: The Cost of Suspension

`std::generator` is convenient, but it is not free, and a senior engineer must price it honestly:

- **Coroutine frame allocation.** Each generator owns a frame holding its locals and resumption state. Unless the compiler performs the *Heap Allocation eLision Optimization* (HALO) — which it can do when the generator's lifetime is fully visible and contained within the caller — that frame is a heap allocation. For a generator created and consumed inside one function, HALO often elides it; for one stored or returned, it usually does not.
- **Suspend/resume is an indirect jump, not a function call.** Each `co_yield`/resume cycle saves and restores the coroutine's state and performs an indirect branch. This is cheaper than a thread context switch but more expensive than a plain loop iteration — typically a handful of nanoseconds, dominated by the indirect branch's mispredict risk.
- **It defeats some loop optimizations.** Because control leaves and re-enters the function at each `co_yield`, the compiler cannot vectorize or fully unroll the producer the way it could a plain loop.

The guidance for hot paths: `std::generator` is excellent for *clarity* and for genuinely lazy or infinite sequences, and its per-element cost is small in absolute terms. But in a tight numerical inner loop that a plain `for` would vectorize, a generator's suspension overhead and lost vectorization can cost an order of magnitude. Use it where laziness, composition, or the elimination of materialized intermediate containers is the goal — not as a default replacement for ordinary loops.

---

## 58.8 Professional Insights

**`std::generator` turns coroutines from an expert tool into an everyday one.** The single biggest barrier to coroutine adoption in C++20 was the 60-plus lines of promise/iterator boilerplate required before you could yield a value. With `std::generator`, writing a lazy sequence is as easy as writing a loop with `co_yield` in place of `push_back`. If your codebase avoided coroutines because of the ceremony, C++23 removes that excuse for the common synchronous-generator case.

**Prefer generators to materialized intermediate containers — that is where they pay for themselves.** The clearest win is replacing a function that builds and returns a `std::vector` of results the caller will iterate once. A generator yields each element on demand, eliminating the intermediate allocation and the up-front cost of computing elements the caller may never reach. When you see "build a temporary container, loop over it, discard it," a generator usually expresses the same logic with less memory and earlier results.

**Respect the frame allocation and suspension cost in hot loops.** A generator's per-element overhead — the possible heap frame and the indirect-branch resume — is negligible for I/O-bound or moderately-sized work but can dominate a tight, vectorizable numerical kernel. Measure before threading a generator through a latency-critical inner loop, and lean on HALO by keeping the generator's creation and consumption in the same scope when you do.

**Use `ranges::elements_of` for any recursive or nested generator.** Hand-written recursive coroutines that re-`co_yield` each element of a child quietly become O(depth) per element. `co_yield std::ranges::elements_of(child)` is not just shorter — it is the only version that composes recursively without that quadratic blow-up. Reach for it whenever a generator yields the contents of another range.

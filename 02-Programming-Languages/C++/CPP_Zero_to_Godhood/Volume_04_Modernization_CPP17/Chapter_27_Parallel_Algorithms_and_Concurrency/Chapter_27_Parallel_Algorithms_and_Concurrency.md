# Chapter 27: Parallel Algorithms and Concurrency

> *C++17 made the standard algorithms parallel. Most of `<algorithm>` and a set of new numeric algorithms now accept an execution policy as their first argument, letting `std::sort`, `std::for_each`, and `std::reduce` run across threads or SIMD lanes without you writing a single thread. Alongside, the concurrency primitives gained `std::scoped_lock` for deadlock-free multi-mutex locking and the untimed `std::shared_mutex` reader-writer lock.*

The parallel algorithms are C++17's most ambitious library addition: a uniform, opt-in path to parallelism that reuses the algorithm names you already know. Pass `std::execution::par` and `std::sort` may use every core; pass `std::execution::par_unseq` and it may additionally vectorize. The new **generalized numeric algorithms** — `reduce`, `transform_reduce`, and the scans — exist precisely because their classic counterparts (`accumulate`, `partial_sum`) impose a strict left-to-right order that forbids parallelism; the new versions relax that ordering so the work can be split. This chapter also covers the locking primitives that make correct concurrent code shorter: `scoped_lock` (the multi-mutex successor to `lock_guard`) and `shared_mutex` (the untimed reader-writer lock that supersedes C++14's `shared_timed_mutex`).

---

## Table of Contents

- [27.1 Parallel Algorithms and Execution Policies](#271-parallel-algorithms-and-execution-policies)
- [27.2 The Three Execution Policies](#272-the-three-execution-policies)
- [27.3 Generalized Numeric Algorithms: `reduce` and `transform_reduce`](#273-generalized-numeric-algorithms-reduce-and-transform_reduce)
- [27.4 The Scan Algorithms](#274-the-scan-algorithms)
- [27.5 `std::for_each_n`](#275-stdfor_each_n)
- [27.6 `std::scoped_lock`](#276-stdscoped_lock)
- [27.7 `std::shared_mutex`](#277-stdshared_mutex)
- [27.8 Professional Insights](#278-professional-insights)

---

## 27.1 Parallel Algorithms and Execution Policies

C++17 adds overloads of roughly 60 standard algorithms — `sort`, `transform`, `for_each`, `find`, `copy`, `reduce`, and most of the rest — that take an **execution policy** as their first argument (header `<execution>`). The policy is a hint to the library about *how* the work may be executed: sequentially, across threads, or across threads with vectorization. The algorithm's *meaning* is unchanged; only its execution strategy varies.

```cpp
// Listing 27.1: the same sort, three execution strategies
#include <algorithm>
#include <execution>
#include <vector>

std::vector<int> v(1'000'000);

// Sequential — equivalent to the classic overload:
std::sort(std::execution::seq, v.begin(), v.end());

// Parallel — may distribute the sort across multiple threads:
std::sort(std::execution::par, v.begin(), v.end());

// Parallel + vectorized — threads AND SIMD interleaving permitted:
std::sort(std::execution::par_unseq, v.begin(), v.end());
```

The policy is a *permission*, not a guarantee: an implementation may execute a `par` call sequentially (for a small range, or if it has no thread pool), but it may never execute a `seq` call in parallel. The performance benefit appears on large datasets where the parallel work outweighs the cost of distributing it; for small ranges, the sequential overload is often faster.

---

## 27.2 The Three Execution Policies

The standard defines three policy objects, each a value in `namespace std::execution`:

| Policy | Object | Meaning | Constraint on your code |
|--------|--------|---------|--------------------------|
| Sequenced | `std::execution::seq` | runs on the calling thread, in order | none |
| Parallel | `std::execution::par` | may run on multiple threads | element operations must be **thread-safe** (no data races) |
| Parallel-unsequenced | `std::execution::par_unseq` | multiple threads **and** SIMD/instruction interleaving | element operations must be **vectorization-safe** |

The distinction between `par` and `par_unseq` is the crucial and dangerous one. Under `par`, each element's operation runs to completion on *some* thread, so an ordinary mutex inside the operation is fine. Under `par_unseq`, the library may **interleave instructions from different element operations on the same thread** (to feed SIMD lanes), so the operation must not do anything that assumes it runs as an uninterrupted unit.

> **Caution:** under `par_unseq`, the element operation must be **vector-safe**: it must not acquire mutexes, allocate or free memory, or perform any operation that cannot be safely interleaved with another invocation on the same thread. A mutex lock interleaved with another lock attempt on the same thread is an immediate deadlock. Use `par` (not `par_unseq`) whenever the body locks or allocates.

```cpp
// Listing 27.2: par is safe with a lock; par_unseq is NOT
#include <execution>
#include <algorithm>
#include <mutex>

std::mutex m;

// OK: par — each body runs as a unit on some thread
std::for_each(std::execution::par, v.begin(), v.end(), [&](int& x){
    std::lock_guard lk(m);   // safe under par
    x = transform(x);
});

// WRONG: par_unseq — locking inside a vectorized body can deadlock
// std::for_each(std::execution::par_unseq, ... lock ...);  // undefined behavior
```

---

## 27.3 Generalized Numeric Algorithms: `reduce` and `transform_reduce`

The classic `std::accumulate` folds left-to-right in a strictly defined order, which makes it inherently sequential — the library cannot reorder the additions. C++17 adds **generalized** numeric algorithms (header `<numeric>`) whose combining operation is required to be **associative and commutative**, freeing the implementation to split the range and combine partial results in any order — the prerequisite for parallelism.

**`std::reduce`** is the parallel-friendly `accumulate`:

```cpp
// Listing 27.3: reduce — accumulate without the ordering constraint
#include <numeric>
#include <execution>
#include <vector>

std::vector<int> v(1'000'000, 1);

int total = std::reduce(std::execution::par, v.begin(), v.end());        // sum, parallel
int prod  = std::reduce(std::execution::par, v.begin(), v.end(),
                        1, std::multiplies<>{});                          // product
```

**`std::transform_reduce`** fuses a transform with a reduction — the standard map-reduce, and the idiomatic way to express a dot product or a sum-of-transformed-values in one parallel pass:

```cpp
// Listing 27.4: transform_reduce — fused map-reduce (e.g. a dot product)
#include <numeric>
#include <execution>
#include <vector>

std::vector<double> a(N), b(N);

// inner product: sum over i of a[i] * b[i], computed in parallel
double dot = std::transform_reduce(
    std::execution::par,
    a.begin(), a.end(), b.begin(),
    0.0);                                   // default ops: + and *

// unary form: sum of f(x) over the range
double sumsq = std::transform_reduce(
    std::execution::par,
    a.begin(), a.end(),
    0.0, std::plus<>{},
    [](double x){ return x * x; });
```

Because the reduction order is unspecified, `reduce` and `transform_reduce` can give **slightly different floating-point results** than `accumulate` (floating-point addition is not associative). That is the price of parallelism and is acceptable in the overwhelming majority of numeric code; where bit-exact reproducibility is required, use `accumulate` (or `seq`).

---

## 27.4 The Scan Algorithms

A **scan** (prefix sum) produces a sequence of running totals. `std::partial_sum` did this sequentially; C++17 adds parallel-friendly scans that, like `reduce`, relax the ordering. They come in inclusive/exclusive pairs, each with a plain and a transform-fused form:

- **`std::inclusive_scan`** — each output element includes the corresponding input (`out[i] = in[0] + ... + in[i]`).
- **`std::exclusive_scan`** — each output element excludes the corresponding input and starts from an initial value (`out[i] = init + in[0] + ... + in[i-1]`).
- **`std::transform_inclusive_scan`** / **`std::transform_exclusive_scan`** — apply a unary transform to each input before scanning, in one fused pass.

```cpp
// Listing 27.5: inclusive vs exclusive scan
#include <numeric>
#include <execution>
#include <vector>

std::vector<int> in{1, 2, 3, 4};
std::vector<int> out(in.size());

// inclusive: {1, 3, 6, 10} — out[i] includes in[i]
std::inclusive_scan(std::execution::par, in.begin(), in.end(), out.begin());

// exclusive with init 0: {0, 1, 3, 6} — out[i] excludes in[i]
std::exclusive_scan(std::execution::par, in.begin(), in.end(), out.begin(), 0);

// fused transform + inclusive scan: scan of squares -> {1, 5, 14, 30}
std::transform_inclusive_scan(
    std::execution::par,
    in.begin(), in.end(), out.begin(),
    std::plus<>{}, [](int x){ return x * x; });
```

Scans are the building block of stream compaction, histogram offsets, and parallel partitioning — algorithms where each element's position in the output depends on the count of preceding elements. The exclusive form takes the initial value as a separate argument; the inclusive form takes the binary op (and an optional init) but not a leading init by position — a frequent point of confusion when switching between them.

---

## 27.5 `std::for_each_n`

`std::for_each_n` applies a function to the **first `n` elements** of a range, given only the start iterator and a count — no end iterator. It complements `for_each` for the common case where you know how many elements to process rather than where the range ends, and it accepts an execution policy.

```cpp
// Listing 27.6: for_each_n processes a count, not a range
#include <algorithm>
#include <execution>
#include <vector>

std::vector<int> v(1000);

// Process exactly the first 100 elements, in parallel:
std::for_each_n(std::execution::par, v.begin(), 100, [](int& x){
    x = process(x);
});
```

It returns an iterator past the last processed element, so it composes with further range operations. The count-based form pairs naturally with algorithms like `std::sample` and `std::generate_n` that also work in terms of a count.

---

## 27.6 `std::scoped_lock`

`std::scoped_lock` is a variadic RAII wrapper that locks **one or more** mutexes for the duration of a scope. With multiple mutexes it uses the same deadlock-avoidance algorithm as `std::lock` (acquiring them in a globally consistent order), so two threads locking the same set of mutexes in different source orders cannot deadlock. It is the C++17 successor to both `std::lock_guard` and the verbose C++11 `std::lock` + adopting-`lock_guard` pattern.

```cpp
// Listing 27.7: deadlock-free multi-mutex locking
#include <mutex>

std::mutex m1, m2;

void swap_data() {
    // Locks BOTH m1 and m2 atomically, deadlock-free, and unlocks on scope exit:
    std::scoped_lock lock(m1, m2);
    // ... safely touch data guarded by m1 and m2 ...
}
```

Before C++17, locking two mutexes safely required `std::lock(m1, m2);` followed by two `std::lock_guard{m, std::adopt_lock}` declarations. `scoped_lock` collapses that to one line, and with a single mutex it is a drop-in replacement for `lock_guard`. (Thanks to CTAD from Chapter 24, the mutex types are deduced — no `std::scoped_lock<std::mutex, std::mutex>` needed.)

---

## 27.7 `std::shared_mutex`

`std::shared_mutex` is a **reader-writer lock**: it permits either many concurrent *shared* (reader) owners or a single *exclusive* (writer) owner. It is the untimed counterpart standardized in C++17; C++14 had shipped only `std::shared_timed_mutex` (which adds timed-locking methods at some overhead). When you do not need timed locking, `shared_mutex` is the lighter, preferred choice.

```cpp
// Listing 27.8: reader-writer locking with shared_mutex
#include <shared_mutex>

std::shared_mutex smtx;

void writer() {
    std::unique_lock lock(smtx);    // EXCLUSIVE — blocks all other readers/writers
    // ... modify shared state ...
}

int reader() {
    std::shared_lock lock(smtx);    // SHARED — concurrent with other readers
    // ... read shared state ...
    return value;
}
```

The pairing is fixed by intent: writers take a `std::unique_lock` (exclusive ownership), readers take a `std::shared_lock` (shared ownership). This is the right primitive for data that is **read far more often than written** — a configuration map, a routing table, a cache — where letting readers proceed concurrently is a real throughput win over a plain `mutex`. When writes are frequent, a `shared_mutex`'s extra bookkeeping can be slower than a plain `mutex`; profile before assuming the reader-writer lock wins.

---

## 27.8 Professional Insights

**Reach for `par`, not `par_unseq`, unless the body is provably vector-safe.** `par` only requires thread safety — a mutex or allocation inside the element operation is fine. `par_unseq` additionally permits interleaving instructions from different invocations on one thread, so any lock, allocation, or non-reentrant call inside the body is undefined behavior (a self-deadlock for mutexes). The performance delta from vectorization is real but narrow; the correctness hazard is severe. Default to `par` and promote to `par_unseq` only after confirming the body does nothing it cannot do mid-instruction-stream.

**Prefer `reduce`/`transform_reduce` to `accumulate` when order doesn't matter — and know when it does.** The generalized algorithms parallelize because their combiner is assumed associative and commutative; the cost is that floating-point results may differ slightly from the strict left-to-right `accumulate`. For sums of money, checksums, or anything requiring bit-exact reproducibility, stay with `accumulate` or pass `seq`. For everything else, `transform_reduce` is the idiomatic one-pass map-reduce.

**Measure before parallelizing.** An execution policy is permission, not a promise, and parallel execution has real fixed costs (task distribution, synchronization). On small ranges the sequential overload usually wins; the parallel forms pay off on large datasets with enough work per element. Benchmark the actual data sizes — a `par` `sort` of a thousand elements can be slower than `seq`.

**Make `std::scoped_lock` your default lock guard.** It is a strict superset of `lock_guard`: identical for one mutex, and deadlock-free for many. Standardizing on it removes the entire class of "locked m1 then m2 here, m2 then m1 there" deadlocks and deletes the old `std::lock` + `adopt_lock` boilerplate.

**Use `shared_mutex` only for genuinely read-heavy data, and prefer it over `shared_timed_mutex` when timing isn't needed.** A reader-writer lock pays for itself when reads vastly outnumber writes and reader concurrency is the bottleneck; under frequent writes its bookkeeping can lose to a plain `mutex`. And since C++17's untimed `shared_mutex` omits the timed-locking overhead of C++14's `shared_timed_mutex`, choose it unless you actually call `try_lock_for`/`try_lock_until`.

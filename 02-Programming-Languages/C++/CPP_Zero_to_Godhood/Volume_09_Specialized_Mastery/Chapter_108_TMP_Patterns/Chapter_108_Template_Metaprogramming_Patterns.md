# Chapter 108: Template Metaprogramming Patterns

Where Chapter 74 covered the *mechanics* of template metaprogramming, this chapter covers the *patterns* — the reusable, named designs that experienced C++ engineers build from those mechanics to move computation from runtime to compile time and erase abstraction cost. Expression templates eliminate temporaries from math libraries; type erasure provides polymorphism without inheritance; the detection idiom adapts to a type's capabilities; policy-based design composes behaviour at zero cost. Each is a tool for achieving a runtime goal entirely within the compiler.

## Chapter Roadmap

- 108.1 Why Patterns, Not Just Mechanics
- 108.2 Expression Templates and Lazy Evaluation
- 108.3 Type Erasure: Polymorphism Without Inheritance
- 108.4 The Detection Idiom
- 108.5 Policy-Based Design
- 108.6 Tag Dispatch and Compile-Time Selection
- 108.7 Cost Model and When to Stop

---

## 108.1 Why Patterns, Not Just Mechanics

Moving computation from runtime to compile time saves cycles and enables zero-cost abstractions. But raw TMP mechanics (SFINAE, `if constexpr`, fold expressions — Chapter 74) are a *language*; the patterns in this chapter are the *idioms* written in that language to solve recurring problems: eliminating temporaries, erasing types, querying capabilities, composing policies.

> **Why this matters.** Each pattern targets a specific runtime cost. Expression templates target the *temporary objects and extra passes* that naive operator overloading creates. Type erasure targets the *coupling and per-type code* that inheritance or templates impose. The detection idiom targets the *rigidity* of fixed interfaces. Recognising which pattern fits a problem — and which is overkill — is what separates productive metaprogramming from the unmaintainable template thickets that give TMP its reputation. The cost model is always the same: compile-time effort and code complexity traded for runtime speed.

---

## 108.2 Expression Templates and Lazy Evaluation

The motivating problem: naive operator overloading on a vector/matrix type creates a temporary for *every* sub-expression. `Vector sum = A + B + C;` computes `tmp1 = A + B` (one allocation and one full pass), then `sum = tmp1 + C` (another pass) — two passes and a temporary for what should be one fused loop. **Expression templates** fix this by making `operator+` return a lightweight *expression object* that *describes* the computation rather than performing it; the work happens once, lazily, on assignment.

```cpp
// Min standard: C++14. Portable (simplified). The +  builds a tree; assignment evaluates it once.
template <typename L, typename R>
struct Sum {
    const L& l; const R& r;
    auto operator[](size_t i) const { return l[i] + r[i]; }   // element computed on demand
    size_t size() const { return l.size(); }
};

template <typename L, typename R>
Sum<L, R> operator+(const L& l, const R& r) { return Sum<L, R>{l, r}; }

// Vector result = A + B + C;
//   A + B            -> Sum<Vector, Vector>
//   (A+B) + C        -> Sum<Sum<Vector,Vector>, Vector>     (no work yet, no temporaries)
//   result = ...     -> one loop: result[i] = A[i] + B[i] + C[i]   (single fused pass)
```
*Listing 108.1 — Expression templates: `A + B + C` becomes one fused loop with zero temporaries.*

> **Why this matters / cost model.** Expression templates collapse N chained operations into a *single* pass over the data with *no* intermediate allocations — for large vectors this is the difference between N passes (each a full sweep through DRAM, Chapter 87) and one, plus the elimination of N−1 temporary buffers. This is exactly how **Eigen** and **Blaze** (Chapter 116) achieve hand-tuned-loop performance from natural `A + B + C` syntax. The costs are steep: the technique is intricate, error messages are notoriously bad, and there are real hazards — returning an expression object that holds *references* to temporaries (`auto x = a + b;` where `a` is a temporary) is a dangling-reference bug (Chapter 97), which is why these libraries are careful about lifetimes and why `auto` with expression templates is dangerous. Reserve expression templates for libraries where the syntax-vs-performance payoff justifies the complexity.

---

## 108.3 Type Erasure: Polymorphism Without Inheritance

**Type erasure** provides runtime polymorphism *without* requiring the types to share an inheritance hierarchy — any type that satisfies a duck-typed interface can be stored and dispatched. It is the pattern behind `std::function`, `std::any`, and `std::shared_ptr`'s deleter. The technique: a templated wrapper captures the concrete type, stores it behind an internal abstract base (or function pointers), and exposes a fixed non-template interface.

```cpp
// Min standard: C++14. Portable (simplified). Stores any drawable type without a common base.
#include <memory>
#include <utility>
class Drawable {
    struct Concept { virtual ~Concept() = default; virtual void draw() const = 0; };
    template <typename T>
    struct Model : Concept {                       // erases T behind Concept
        T obj;
        explicit Model(T o) : obj(std::move(o)) {}
        void draw() const override { obj.draw(); } // duck-typed: any T with draw()
    };
    std::unique_ptr<Concept> self_;
public:
    template <typename T>
    Drawable(T obj) : self_(std::make_unique<Model<T>>(std::move(obj))) {}
    void draw() const { self_->draw(); }
};
// std::vector<Drawable> shapes;  // holds Circles, Squares — no common base class required
```
*Listing 108.2 — Type erasure: heterogeneous storage of unrelated types satisfying a `draw()` interface.*

> **Why this matters / cost model.** Type erasure decouples the *interface* from the *hierarchy*: a `Circle` and a `Square` need not inherit a common base (they need not even know `Drawable` exists), yet they can be stored together and dispatched polymorphically. This is more flexible than inheritance (works with types you don't own, including third-party and fundamental types) and avoids the template code-bloat of storing each concrete type separately. The cost is the same as `virtual`: an indirect call and, in this implementation, a heap allocation per object (`make_unique`) — `std::function`'s small-buffer optimisation avoids the allocation for small callables. Use type erasure when you need runtime polymorphism over an *open* set of unrelated types; use templates when the set is closed and the call is hot; use inheritance when the types genuinely share an "is-a" relationship.

---

## 108.4 The Detection Idiom

The **detection idiom** answers "does type `T` support operation `X`?" at compile time, letting generic code *adapt* to a type's capabilities rather than demanding a fixed interface.

```cpp
// Min standard: C++17 (void_t). Portable.
#include <type_traits>
#include <utility>
template <typename, typename = std::void_t<>>
struct has_serialize : std::false_type {};
template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>> : std::true_type {};

// In generic code:
//   if constexpr (has_serialize<T>::value) t.serialize(); else fallback(t);
// C++20: replace the whole trait with  requires(T t){ t.serialize(); }
```
*Listing 108.3 — Detection idiom: query a capability at compile time. In C++20, a `requires` expression is clearer.*

> **Why this matters.** Detection lets one generic function service types of varying capability — call `reserve()` if the container has it, serialize via member if present else via a free function, take an optimised path for trivially-copyable types. It is the mechanism behind much of the standard library's adaptivity. As Chapter 74 noted, **C++20 concepts** express the same intent far more legibly (`requires(T t){ t.serialize(); }`), so reserve the `void_t` detection idiom for pre-C++20 toolchains. The pattern's value is *graceful adaptation* — code that does more when given a more capable type, without failing on a less capable one.

---

## 108.5 Policy-Based Design

**Policy-based design** composes a class's behaviour from orthogonal *policy* parameters, each supplied as a template argument, so behavioural combinations are resolved (and inlined) at compile time.

```cpp
// Min standard: C++11. Portable. A smart pointer composed from independent policies.
template <typename T, typename CheckingPolicy, typename ThreadingPolicy>
class SmartPtr : private CheckingPolicy, private ThreadingPolicy {
    T* ptr_ = nullptr;
public:
    T& operator*() { CheckingPolicy::check(ptr_); return *ptr_; }   // policy decides null-checking
    // ThreadingPolicy decides whether ref-count ops are atomic
};
// User selects: SmartPtr<int, NoCheck, SingleThreaded>  or  <int, AssertCheck, MultiThreaded>
```
*Listing 108.4 — Policy-based design: orthogonal axes (checking, threading) chosen at compile time.*

> **Why this matters / cost model.** Policies turn a combinatorial explosion of behaviours (checking × threading × ownership) into independent, separately-testable units, each resolved at compile time so dispatch is free and stateless policies add zero bytes (the empty base optimisation, Chapter 80). This is how `std::unique_ptr`'s deleter and `std::vector`'s allocator work. The costs are template code bloat (one instantiation per policy combination) and a hard boundary against runtime configuration (a policy chosen by a config file cannot be a template argument). Developed in Chapter 74; here it is the canonical *composition* pattern of zero-cost TMP.

---

## 108.6 Tag Dispatch and Compile-Time Selection

**Tag dispatch** selects an overload at compile time by passing a *tag type* (often derived from a trait) as an extra argument, choosing the implementation without runtime branching. The standard library uses iterator-category tags to pick `O(1)` vs `O(n)` `std::advance`.

```cpp
// Min standard: C++11. Portable. Select an algorithm by a compile-time trait tag.
#include <iterator>
template <typename It>
void advance_impl(It& it, int n, std::random_access_iterator_tag) { it += n; }     // O(1)
template <typename It>
void advance_impl(It& it, int n, std::forward_iterator_tag) { while (n--) ++it; }  // O(n)

template <typename It>
void my_advance(It& it, int n) {
    advance_impl(it, n, typename std::iterator_traits<It>::iterator_category{});    // tag picks overload
}
```
*Listing 108.5 — Tag dispatch: the iterator category tag selects the optimal implementation at compile time.*

> **Why this matters.** Tag dispatch is the pre-`if constexpr` way to branch on a compile-time property, and it remains clearer than `if constexpr` for *dispatching to entirely different implementations* (vs branching within one body). It is zero-cost — the tag is an empty type, the wrong overload is never instantiated, and the call inlines to the chosen implementation. C++17's `if constexpr` subsumes many uses, but tag dispatch still shines when the alternatives are large and naturally separate functions, and it is pervasive in the standard library's internals.

---

## 108.7 Cost Model and When to Stop

| Pattern | Runtime cost eliminated | Price paid |
|---|---|---|
| Expression templates | Temporaries + extra passes | Complexity; lifetime/`auto` hazards |
| Type erasure | Inheritance coupling; template bloat | Indirect call + possible allocation |
| Detection idiom | Fixed-interface rigidity | Compile-time only (use concepts in C++20) |
| Policy-based design | Runtime dispatch | Code bloat; no runtime config |
| Tag dispatch | Runtime branching | Compile-time only |

> **The discipline.** These patterns are how C++ delivers "zero-cost abstraction" in practice — they move real runtime costs (temporaries, dispatch, branching) into the compiler. But each pays in compile time, code complexity, and diagnostic difficulty, and TMP's failure mode is the unmaintainable template thicket. The mastery judgement is *when to stop*: reach for these patterns when the runtime payoff is measured and significant (a hot numeric library, a widely-used container), and prefer the simplest tool that works — `if constexpr` and concepts (C++20) over SFINAE and `void_t`, a plain `virtual` over hand-rolled type erasure when the call is cold. The next chapter applies this same hardware-sympathetic, compile-time mindset to data structures themselves.

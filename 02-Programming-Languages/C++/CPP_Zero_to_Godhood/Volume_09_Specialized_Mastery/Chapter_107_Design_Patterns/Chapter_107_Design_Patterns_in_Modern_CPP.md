# Chapter 107: Design Patterns in Modern C++

Design patterns are named, reusable solutions to recurring design problems — but in C++ the classic Gang-of-Four catalogue is only half the story, because the language's templates, value semantics, and zero-overhead abstractions let you implement many patterns *without* the runtime indirection (virtual calls, heap allocation) that the original object-oriented formulations assume. This chapter covers the patterns that matter in modern C++, and for each asks the question the rest of this book insists on: what does it cost, and is the C++ idiom that achieves the same intent *cheaper*?

## Chapter Roadmap

- 107.1 Patterns as Vocabulary, Not Dogma
- 107.2 Creational: Singleton and Factory
- 107.3 Structural: Adapter, Composite, and Pimpl
- 107.4 Behavioral: Strategy and Observer
- 107.5 The C++ Idioms: CRTP and Static Polymorphism
- 107.6 The Pimpl Idiom and Compilation Firewalls
- 107.7 Cost-Aware Pattern Selection

---

## 107.1 Patterns as Vocabulary, Not Dogma

A **design pattern** is a tested solution shape with a shared name, so that "use a Strategy here" communicates a whole design in three words. C++ leverages strong typing, templates, and the object model to implement these patterns with high performance and flexibility. But patterns are *vocabulary*, not law: the GoF book described patterns for languages where every object is heap-allocated and every method is virtual, and a literal transcription into C++ inherits costs C++ can often avoid.

> **Why this matters.** The mastery move is to recognise the *intent* of a pattern and choose the cheapest C++ mechanism that realises it. The Strategy pattern's intent — interchangeable algorithms — can be a `virtual` interface (runtime-swappable, has dispatch cost) *or* a template parameter (compile-time, zero cost) *or* a `std::function` (runtime, type-erased, allocation possible). All three are "Strategy"; they differ by orders of magnitude in cost. Knowing the pattern tells you the *shape*; knowing C++ tells you which *implementation* to pick for the cost budget.

---

## 107.2 Creational: Singleton and Factory

**Singleton** ensures a class has exactly one instance and a global access point. The modern C++ form uses a function-local `static`, which C++11 guarantees is initialised exactly once, thread-safely:

```cpp
// Min standard: C++11. Portable. The Meyers Singleton — thread-safe, lazy.
class Singleton {
public:
    static Singleton& instance() {
        static Singleton s;     // C++11: initialised once, thread-safe (guarded)
        return s;
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
private:
    Singleton() = default;
};
```
*Listing 107.1 — The Meyers Singleton. The local `static` is the idiomatic, race-free implementation.*

**Factory Method** defines an interface for creating objects while letting the concrete type be decided elsewhere — decoupling construction from use.

> **Why this matters / cost model.** The Meyers Singleton is correct and lazy, but Singleton is the most *over-used* pattern: it is a global in disguise, and globals defeat testing (you cannot substitute a mock), create hidden coupling, and carry an initialisation-order hazard with *other* globals (Chapter 102's static init order). The C++11 thread-safe guarantee costs a one-time atomic check on the first access (and, in some ABIs, a guard check on every access — `constinit` removes it for constant-initialisable singletons). Prefer *dependency injection* (pass the dependency explicitly) over Singleton wherever testability matters; reserve Singleton for genuinely process-global, stateless services. Factories earn their keep when construction is non-trivial or the concrete type varies — but a factory returning `unique_ptr<Base>` reintroduces virtual dispatch and heap allocation, so on a hot path prefer a compile-time selected concrete type.

---

## 107.3 Structural: Adapter, Composite, and Pimpl

- **Adapter** converts one class's interface into another that clients expect — the software equivalent of a plug adapter. In C++ it is often a thin wrapper class (or, for free functions, an overload) that forwards calls, frequently inline-able to zero cost.
- **Composite** composes objects into tree structures to represent part-whole hierarchies, so clients treat individual objects and compositions uniformly (a UI widget tree, a scene graph, an expression AST).
- **Pimpl** (pointer-to-implementation) hides a class's implementation behind an opaque pointer — covered in depth in §107.6 as a compilation firewall.

> **Why this matters.** Adapter and Composite are about *interface shape*, and in C++ they interact with the cost model through dispatch. A Composite tree typically uses `virtual` to treat leaves and branches uniformly — fine for a UI (cold path) but costly for an expression tree evaluated in a hot loop, where expression templates (Chapter 108) achieve the same composition at compile time. The recurring theme: a structural pattern that introduces a `virtual` boundary is free on a cold path and a tax on a hot one.

---

## 107.4 Behavioral: Strategy and Observer

**Strategy** defines a family of interchangeable algorithms behind a common interface, so the algorithm can vary independently of the client that uses it. **Observer** defines a one-to-many dependency so that when one object (the subject) changes state, all its dependents (observers) are notified — the basis of event systems and reactive UIs.

```cpp
// Min standard: C++17. Strategy via std::function (runtime) vs a template (compile-time).
#include <functional>
class Compressor {
    std::function<std::string(const std::string&)> strategy_;   // runtime-swappable; may allocate
public:
    explicit Compressor(std::function<std::string(const std::string&)> s) : strategy_(std::move(s)) {}
    std::string run(const std::string& in) { return strategy_(in); }
};
// vs. compile-time Strategy: template <typename Strategy> class Compressor { Strategy s_; ... };
```
*Listing 107.2 — Strategy as a runtime `std::function` (flexible, has cost) or a template parameter (zero cost).*

> **Why this matters / cost model.** Strategy is the clearest illustration of "one pattern, three costs": a `virtual` interface (vtable dispatch, runtime-swappable), a `std::function` (type-erased, runtime-swappable, may heap-allocate beyond its small-buffer — Chapter 80's SSO/SOO), or a template parameter (resolved and inlined at compile time, zero cost, but *not* runtime-swappable). Choose by whether the strategy must change at runtime and whether the call is hot. Observer's hazard is *lifetime*: an observer destroyed without unregistering leaves a dangling pointer in the subject's list (a use-after-free, Chapter 97) — modern implementations use `weak_ptr` or a connection-token (`scoped_connection`, as in Boost.Signals2) to sever the link automatically. Observer also has a *re-entrancy* hazard: a notification that modifies the observer list mid-iteration is a classic bug.

---

## 107.5 The C++ Idioms: CRTP and Static Polymorphism

Beyond the GoF catalogue, C++ has its own pattern vocabulary. The most important is **CRTP** (Curiously Recurring Template Pattern): a base class parameterised on its derived type, enabling **static polymorphism** — virtual-like dispatch resolved entirely at compile time.

```cpp
// Min standard: C++11. Portable. Static polymorphism with zero dispatch cost.
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();   // compile-time dispatch, inlinable
    }
};
class Concrete : public Base<Concrete> {
public:
    void implementation() { /* ... */ }
};
```
*Listing 107.3 — CRTP: the base calls into the derived type known at compile time.*

> **Why this matters / cost model.** CRTP is the C++ answer to "I want polymorphism but cannot afford the virtual call." It eliminates the vtable load and indirect branch (Chapter 74/86), lets the optimizer inline across the dispatch boundary, and is the implementation technique behind mixins (injecting `operator!=` from `operator==`), `enable_shared_from_this`, and expression templates. Its costs are the inverse of `virtual`'s: no heterogeneous containers (each `Base<D>` is a distinct type) and code bloat (one instantiation per derived type). This is developed fully in Chapter 74; here it is the canonical example that C++ often realises a GoF pattern's *intent* (here, the Template Method / polymorphic dispatch) with a *cheaper* mechanism.

---

## 107.6 The Pimpl Idiom and Compilation Firewalls

The **Pimpl idiom** (pointer to implementation, a.k.a. the compilation firewall or "Cheshire Cat") moves a class's private data members and helpers into a separately-compiled implementation struct, leaving only an opaque pointer in the header.

```cpp
// Min standard: C++11. Portable. Header exposes nothing about the implementation.
// widget.h
#include <memory>
class Widget {
    class Impl;                       // forward declaration only
    std::unique_ptr<Impl> pImpl_;     // opaque pointer
public:
    Widget();
    ~Widget();                        // MUST be defined in the .cpp (Impl is complete there)
    void draw();
};
// widget.cpp
class Widget::Impl { /* real members, includes, helpers — hidden from clients */ };
Widget::Widget() : pImpl_(std::make_unique<Impl>()) {}
Widget::~Widget() = default;          // defined here, where Impl is complete
void Widget::draw() { /* uses pImpl_ */ }
```
*Listing 107.4 — Pimpl: the header reveals no implementation detail, so changing `Impl` does not recompile clients.*

> **Why this matters / cost model.** Pimpl is two patterns in one. As an **ABI-stability** tool (Chapter 102) it presents a fixed class layout regardless of how the implementation changes — essential for shared libraries whose clients must not recompile. As a **build-time** tool it breaks header dependencies: clients include only the slim header, so editing `Impl`'s members or its `#include`s recompiles one `.cpp`, not the whole project — a major win on large codebases. The costs are real and on the hot path you must weigh them: every access goes through a pointer indirection (a likely cache miss, Chapter 87), the object requires a heap allocation, and you lose inlining across the firewall. The critical correctness detail is that the destructor (and any special member) must be *defined in the `.cpp`* where `Impl` is complete — a `unique_ptr<Impl>` cannot destroy an incomplete type, so a defaulted destructor in the header fails to compile. Pimpl is for cold, ABI-facing, or build-bottleneck classes — never for a hot-path value type.

---

## 107.7 Cost-Aware Pattern Selection

| Pattern | Classic (OO) cost | Cheaper C++ idiom |
|---|---|---|
| Strategy | `virtual` dispatch | Template parameter (zero cost) or `std::function` |
| Template Method / polymorphism | `virtual` dispatch | CRTP (static polymorphism) |
| Singleton | Global + init-order hazard | Dependency injection; `constinit` if constant |
| Adapter | Wrapper indirection | Inline forwarding wrapper (zero cost) |
| Composite (hot) | `virtual` tree walk | Expression templates (Ch 108) |
| Observer | Dangling-pointer risk | `weak_ptr` / scoped connections |
| Pimpl | Indirection + heap + no inlining | (keep, but only for cold/ABI/build cases) |

> **The discipline.** Patterns are a vocabulary for *communicating* designs and a checklist for *recognising* recurring problems — invaluable for both. But in C++ the GoF implementations are a starting point, not the destination: each one's runtime cost (virtual dispatch, heap allocation, type erasure) should be weighed against the C++ idiom that achieves the same intent at compile time. The rule is the one that runs through this whole book: use the pattern to name the problem, then pick the implementation whose cost fits the path — `virtual` and `std::function` for cold, runtime-flexible code; templates and CRTP for hot paths where the abstraction must be free. The next chapter takes the compile-time route to its logical extreme with template-metaprogramming patterns.

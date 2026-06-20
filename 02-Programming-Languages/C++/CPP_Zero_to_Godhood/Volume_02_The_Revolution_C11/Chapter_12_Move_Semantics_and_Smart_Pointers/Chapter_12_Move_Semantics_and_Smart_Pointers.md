# Chapter 12: Move Semantics and Smart Pointers

> *The two features that, together, abolished the "performance tax" of C++ and replaced manual memory management with a formal model of ownership.*

C++11 made two intertwined contributions that define modern C++ resource management. **Move semantics** let an object transfer ownership of its internal resources instead of deep-copying them, collapsing expensive copies into pointer swaps. **Smart pointers** encode ownership in the type system, making `new`/`delete` obsolete in user code. This chapter develops both from first principles, then unifies them through perfect forwarding.

---

## Table of Contents

- [12.1 The Move Revolution](#121-the-move-revolution)
- [12.2 Value Categories: lvalue, rvalue, and the Full Taxonomy](#122-value-categories-lvalue-rvalue-and-the-full-taxonomy)
- [12.3 Rvalue References (`T&&`)](#123-rvalue-references-t)
- [12.4 `std::move`: The Shipping Label](#124-stdmove-the-shipping-label)
- [12.5 The Move Constructor and Move Assignment](#125-the-move-constructor-and-move-assignment)
- [12.6 Complexity Optimization: O(n²) to O(n)](#126-complexity-optimization-on²-to-on)
- [12.7 Perfect Forwarding and Reference Collapsing](#127-perfect-forwarding-and-reference-collapsing)
- [12.8 Smart Pointers and RAII](#128-smart-pointers-and-raii)
- [12.9 `std::unique_ptr`: Exclusive Ownership](#129-stdunique_ptr-exclusive-ownership)
- [12.10 `std::shared_ptr`: Shared Ownership](#1210-stdshared_ptr-shared-ownership)
- [12.11 `std::weak_ptr`: Non-Owning Observation](#1211-stdweak_ptr-non-owning-observation)
- [12.12 `enable_shared_from_this` and Smart-Pointer Casts](#1212-enable_shared_from_this-and-smart-pointer-casts)
- [12.13 Professional Insights](#1213-professional-insights)

---

## 12.1 The Move Revolution

In the early 2000s C++ felt "heavy." Given a `std::vector<std::string>` of 10,000 long strings, passing it to a function offered two bad choices:

1. **Pass by pointer** — fast, but ownership is ambiguous and dangerous.
2. **Pass by value** — safe, but the program would *clone* all 10,000 strings, then destroy the originals a microsecond later.

This was the **performance tax** of C++. C++11 abolished it with **move semantics**: when the source of a copy is a temporary (or something you have explicitly given up), its resources can be *stolen* rather than duplicated.

> **Fireside chat — the "magic box."**
> *Student:* "Why does stealing need special syntax?"
> *Architect:* "Because the compiler needs your **permission** to steal. If you are holding a sandwich (an **lvalue**), I cannot take a bite — that is theft. But a sandwich in a trash can marked *FREE* (an **rvalue**) is fair game. `std::move` is how you put the *FREE* sign on a variable you are done with."

Think of memory as a neighborhood: an **lvalue** is a *house* — a permanent address with a name that persists; an **rvalue** is a *shipping box* — temporary, in transit, about to be discarded. In `int x = 10;`, `x` is the house (lvalue) and `10` is the delivery box (rvalue).

---

## 12.2 Value Categories: lvalue, rvalue, and the Full Taxonomy

Every C++ expression has a **value category**. Two independent properties define them:

- **Identity** — does the expression refer to a named object with an address?
- **Movability** — may the expression be implicitly moved from (bound to a `T&&` parameter)?

The standard combines these into three primary categories, plus two umbrella groupings:

| Category | Identity | Movable | Examples |
| :------- | :------: | :-----: | :------- |
| **lvalue** | Yes | No | `x`, `*ptr`, `foo_ref()`, a string literal |
| **xvalue** (eXpiring) | Yes | Yes | `std::move(x)`, `X{4}.n` |
| **prvalue** (pure rvalue) | No | Yes | `42`, `x + 2`, `X{4}`, a lambda, `foo()` returning by value |
| **glvalue** = lvalue ∪ xvalue | Yes | — | anything with identity |
| **rvalue** = xvalue ∪ prvalue | — | Yes | anything movable-from |

```cpp
// Listing 12.1: classifying expressions
struct X { int n; };
extern X x;

4;                   // prvalue: no identity
x;                   // lvalue
x.n;                 // lvalue
std::move(x);        // xvalue: has identity AND is movable
std::forward<X&>(x); // lvalue
X{4};                // prvalue
X{4}.n;              // xvalue
```

The crucial, counter-intuitive rule: **a named rvalue reference is itself an lvalue.**

```cpp
// Listing 12.2: a named rvalue reference is an lvalue
std::string str("init");
std::string &&str_ref = std::move(str); // str_ref is a named variable
std::string test(str_ref);              // COPY — str_ref is an lvalue expression!
std::string test2(std::move(str_ref));  // MOVE — re-apply std::move
```

This is *why* `std::forward` exists (§12.7): inside a function, the parameter is always a named lvalue, even when it was initialized from an rvalue.

---

## 12.3 Rvalue References (`T&&`)

C++11 introduced the **rvalue reference** `T&&`, which binds *only* to rvalues. It is the "box snatcher" — a hook that grabs a temporary before it is destroyed so its contents can be pilfered.

```cpp
// Listing 12.3: rvalue references bind only to rvalues
int x = 10;
int&  lref  = x;   // lvalue reference binds to an lvalue
// int&& bad = x;  // ERROR: cannot bind rvalue ref to an lvalue
int&& rref  = 20;  // OK: 20 is an rvalue
```

The point of having two reference types is to select two behaviors via overload resolution: **lvalue-reference overloads copy; rvalue-reference overloads move.**

---

## 12.4 `std::move`: The Shipping Label

**`std::move` does not move anything.** It is an unconditional cast to an rvalue reference — a *shipping label* you stick on an lvalue that says "this house is now a box; feel free to take the furniture." The actual transfer happens later, inside a move constructor or move assignment operator.

```cpp
// Listing 12.4: std::move enables the move overload
Vector v1(100);
Vector v2 = std::move(v1); // selects the move constructor
// v1 is now in a valid-but-unspecified (typically empty) state
```

`std::move(obj)` by itself changes nothing about `obj`; only the move operation it *enables* does. **Do not read a moved-from object** except to assign to it or destroy it.

---

## 12.5 The Move Constructor and Move Assignment

Instead of copying data (slow), move operations steal pointers (fast) and null out the source so its destructor does no harm.

```cpp
// Listing 12.5: move constructor and move assignment
class BigData {
    int*   buffer;
    size_t size;
public:
    // Move constructor
    BigData(BigData&& other) noexcept
        : buffer(other.buffer), size(other.size) {  // A. steal the data
        other.buffer = nullptr;                     // B. nullify the victim
        other.size   = 0;   // else 'other's destructor double-frees our buffer
    }

    // Move assignment
    BigData& operator=(BigData&& other) noexcept {
        if (this != &other) {
            delete[] buffer;        // free our own resources
            buffer = other.buffer;  // steal
            size   = other.size;
            other.buffer = nullptr; // nullify source
            other.size   = 0;
        }
        return *this;
    }
};
```

### Why `noexcept` is mandatory for moves

If your move constructor is **not** `noexcept`, the standard containers will often *refuse to use it*. When `std::vector` reallocates, it must preserve the **strong exception guarantee**: if moving an element could throw mid-reallocation, the vector could not roll back to a consistent state. So it falls back to *copying* — silently discarding your move optimization. **Always mark moves `noexcept`.**

> The compiler generates move operations for you when the class has no user-declared copy operations, destructor, or move operations. Declaring any of those (the "Rule of Five") suppresses the implicit moves — declare them all, or `= default` them, when managing a resource.

---

## 12.6 Complexity Optimization: O(n²) to O(n)

Moving a container is **O(1)** (steal a pointer); copying is **O(n)**. In code written in an immutable style — where loops are expressed as recursion that logically copies a container each step (e.g. generating a Collatz sequence) — moves can collapse the overall complexity from **O(n²)** to **O(n)**.

```cpp
// Listing 12.6: immutable-style recursion made linear by moves
// (after Andrew Koenig, "Containers That Never Change", Dr. Dobb's, 2013)
std::vector<int> concat(std::vector<int> const& v, int x) {
    auto result = v;        // the only real copy
    result.push_back(x);
    return result;          // returned by move (RVO / implicit move)
}

std::vector<int> collatz_aux(int n, std::vector<int> const& result) {
    if (n == 1) return result;
    auto next = concat(result, n);                 // moved into 'next'
    return (n % 2 == 0) ? collatz_aux(n / 2, next)
                        : collatz_aux(3 * n + 1, next);
}
```

Returning a local by value is automatically a move (or is elided entirely by RVO) — never write `return std::move(local);`, which can *defeat* copy elision.

---

## 12.7 Perfect Forwarding and Reference Collapsing

Generic wrappers must pass arguments onward while preserving their value category (lvalue vs rvalue). This is **perfect forwarding**, and it rests on two mechanisms.

### 12.7.1 Forwarding (Universal) References

When `T` is a *deduced* template parameter, `T&&` is a **forwarding reference**, not a plain rvalue reference — it binds to anything, and `T` encodes whether the argument was an lvalue or rvalue.

### 12.7.2 Reference Collapsing Rules

| Combination | Collapses to |
| :---------- | :----------- |
| `& ` + `& ` | `&` |
| `& ` + `&&` | `&` |
| `&&` + `& ` | `&` |
| `&&` + `&&` | `&&` |

**Mnemonic:** an lvalue reference is a "black hole" — if `&` appears anywhere, the result is `&`. Only `&& + &&` stays `&&`.

### 12.7.3 `std::forward`

`std::forward<T>(arg)` casts `arg` back to an rvalue *only if* `T` was deduced from an rvalue; otherwise it passes it as an lvalue. It undoes the "named rvalue reference is an lvalue" problem from §12.2.

```cpp
// Listing 12.7: a perfectly-forwarding wrapper
template<typename T>
void wrapper(T&& arg) {            // forwarding reference
    func(std::forward<T>(arg));    // preserves value category
}
```

### 12.7.4 The canonical use: factory functions

```cpp
// Listing 12.8: variadic perfect-forwarding factory (this IS make_unique)
template<class T, class... A>
std::unique_ptr<T> make_unique(A&&... args) {
    return std::unique_ptr<T>(new T(std::forward<A>(args)...));
}

struct Foo { Foo(const Foo&); Foo(Foo&&); Foo(int,int,int); };
Foo f;
auto p1 = make_unique<Foo>(f);            // forwards lvalue  -> copy ctor
auto p2 = make_unique<Foo>(std::move(f)); // forwards rvalue  -> move ctor
auto p3 = make_unique<Foo>(1, 2, 3);      // forwards prvalues
```

This is exactly how `emplace_back`, `make_shared`, and `make_unique` avoid extra copies.

---

## 12.8 Smart Pointers and RAII

Manual dynamic memory in C++98 invited four classic defects:

1. **Memory leaks** — forgetting `delete`.
2. **Dangling pointers** — using memory after `delete`.
3. **Double free** — deleting the same memory twice.
4. **Exception-unsafety** — an exception thrown before `delete` leaks.

Smart pointers solve all four through **RAII** (Resource Acquisition Is Initialization): the resource's lifetime is tied to an object's lifetime, so the destructor always releases it.

The single axis that distinguishes the smart pointers is **ownership**:

| Type | Ownership | Copyable? | Main use case |
| :--- | :-------- | :-------- | :------------ |
| `std::unique_ptr` | **Exclusive** | No (move-only) | Default for single-owner resources |
| `std::shared_ptr` | **Shared** | Yes | Multiple co-owners of one resource |
| `std::weak_ptr` | **None** (observer) | Yes | Observation; breaking reference cycles |

---

## 12.9 `std::unique_ptr`: Exclusive Ownership

A non-null `std::unique_ptr` *exclusively* owns its pointee. It cannot be copied — only **moved** — which is how the type enforces a single owner at compile time. It is the lightest smart pointer, with essentially zero overhead over a raw pointer, and should be your **default**.

```cpp
// Listing 12.9: move-only ownership transfer
#include <memory>
#include <utility>

std::unique_ptr<int> make() { return std::unique_ptr<int>(new int(5)); }

int main() {
    auto p1 = make();               // p1 owns the int
    // auto bad = p1;               // ERROR: copy is deleted
    auto p2 = std::move(p1);        // ownership transferred; p1 is now null
    int a = *p2;                    // OK
    // int b = *p1;                 // UB: p1 is nullptr
}
```

Returning a `unique_ptr` is the **preferred C++11 way to write factory functions** — the return type documents that the caller now owns the resource, unlike a raw `int* foo();` where ownership is unclear.

### Custom deleters

`unique_ptr` accepts a deleter type, ideal for C-style resources:

```cpp
// Listing 12.10: custom deleters
auto closer = [](FILE* f){ if (f) fclose(f); };
std::unique_ptr<FILE, decltype(closer)> file(fopen("data.txt", "r"), closer);

// Function-pointer deleter form:
std::unique_ptr<FILE, int(*)(FILE*)> file2(fopen("data.txt","r"), fclose);
```

### Arrays

There is a dedicated array specialization that calls `delete[]` and supports `operator[]`:

```cpp
// Listing 12.11: unique_ptr to an array
std::unique_ptr<int[]> arr(new int[10]);
arr[2] = 10;   // index access on the array specialization
```

### `make_unique` is C++14 — and how to write it in C++11

`std::make_unique` was added in **C++14**, not C++11. It is trivial to provide in C++11:

```cpp
// Listing 12.12: a C++11 make_unique (non-array and array forms)
#include <type_traits>
#include <memory>

template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T> >::type
make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T> >::type
make_unique(std::size_t n) {
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]());
}
```

> `std::unique_ptr` replaced the deeply flawed C++98 `std::auto_ptr`, whose "copy" silently transferred ownership (and which is removed in C++17).

---

## 12.10 `std::shared_ptr`: Shared Ownership

`std::shared_ptr` implements **shared ownership** via **reference counting**. The managed object is destroyed only when the *last* owning `shared_ptr` is destroyed or reassigned.

### The control block

A `shared_ptr` carries two pointers (to the object and to a heap **control block**). The control block stores:

1. the **strong** reference count (owners),
2. the **weak** reference count (observers),
3. the deleter / allocator state,
4. and, with `make_shared`, the object itself (co-located in one allocation).

```cpp
// Listing 12.13: reference counting in action
#include <memory>
auto p1 = std::make_shared<int>(100); // strong count = 1; one allocation
{
    auto p2 = p1;                      // copy -> strong count = 2
    std::cout << p1.use_count();       // 2
}                                      // p2 destroyed -> strong count = 1
// p1 destroyed -> count = 0 -> object freed
```

**Prefer `make_shared`:** it allocates the object and control block in a single block (faster, one cache-friendlier allocation) and is exception-safe. (Its one downside: the object's memory cannot be freed until all *weak* references are also gone, since they share the block.)

### Ownership pitfalls

A `shared_ptr` only coordinates with other `shared_ptr`s **copied from it**. Constructing two independent `shared_ptr`s from the *same raw pointer* creates two separate control blocks and a double free:

```cpp
// Listing 12.14: the double-free trap
Foo* raw = new Foo;
std::shared_ptr<Foo> a(raw);
std::shared_ptr<Foo> b(raw);  // WRONG: independent control block
// when a and b both expire, raw is deleted twice -> UB
```

The **aliasing constructor** lets a `shared_ptr` share ownership of one object while pointing at a sub-object, keeping the whole object alive:

```cpp
// Listing 12.15: aliasing constructor
struct Foo { int x; };
auto p1 = std::make_shared<Foo>();
std::shared_ptr<int> p2(p1, &p1->x); // p2 points at x, co-owns the Foo
```

### A toy implementation

The mechanism is small enough to sketch — note the **atomic** count, which is what makes copies thread-safe (and costly):

```cpp
// Listing 12.16: minimal shared_ptr to show the control block
template<typename T>
class SharedPtr {
    T* ptr;
    struct ControlBlock { std::atomic<int> ref_count{1}; }* cb;
public:
    explicit SharedPtr(T* p) : ptr(p), cb(new ControlBlock()) {}
    SharedPtr(const SharedPtr& o) : ptr(o.ptr), cb(o.cb) {
        if (cb) ++cb->ref_count;
    }
    ~SharedPtr() {
        if (cb && --cb->ref_count == 0) { delete ptr; delete cb; }
    }
};
```

**The cost of sharing:** a `shared_ptr` is twice the size of a raw pointer, and every copy/destruction performs an *atomic* increment/decrement. Use it only when ownership is genuinely shared or indeterminate — never merely because it "feels safer."

---

## 12.11 `std::weak_ptr`: Non-Owning Observation

A `std::weak_ptr` references an object owned by `shared_ptr`s **without** affecting the strong count — so it never keeps the object alive. To use it you must `lock()` it, which atomically produces a `shared_ptr` if the object is still alive, or an empty one if it has expired.

```cpp
// Listing 12.17: observing without owning
auto sp = std::make_shared<int>(42);
std::weak_ptr<int> wp = sp;        // observe; strong count still 1

if (auto locked = wp.lock()) {     // try to acquire temporary ownership
    use(*locked);
}
sp.reset();                        // last owner gone -> object destroyed
if (auto locked = wp.lock()) { /* ... */ }
else { /* object expired */ }
```

### Breaking reference cycles

Two objects that hold `shared_ptr`s to each other form a cycle whose counts never reach zero — a leak. Make the back-reference a `weak_ptr`:

```cpp
// Listing 12.18: weak_ptr breaks an ownership cycle
struct B;
struct A { std::shared_ptr<B> b; };
struct B { std::weak_ptr<A> a; };   // back-edge is weak -> no cycle
```

The rule: if `Parent` owns `Child` via `shared_ptr`, `Child` should refer back to `Parent` via `weak_ptr`.

---

## 12.12 `enable_shared_from_this` and Smart-Pointer Casts

To obtain a `shared_ptr` to `this` from inside a member function — without creating a *second* control block — derive from `std::enable_shared_from_this<T>` and call `shared_from_this()`:

```cpp
// Listing 12.19: shared_from_this
#include <memory>
class Widget : public std::enable_shared_from_this<Widget> {
public:
    void register_self() {
        std::shared_ptr<Widget> self = shared_from_this(); // shares the existing block
        EventManager::add(self);
    }
};
int main() {
    auto w = std::make_shared<Widget>();
    w->register_self();
}
```

Calling `shared_from_this()` on an object **not** already owned by a `shared_ptr` (a stack/global object, or from within the constructor) is undefined behavior (C++17 makes it throw `std::bad_weak_ptr`).

To convert between related `shared_ptr` types while preserving the shared control block, use the dedicated casts: **`std::static_pointer_cast`**, **`std::dynamic_pointer_cast`**, **`std::const_pointer_cast`**.

---

## 12.13 Professional Insights

**Design rules.**
- **Default to `unique_ptr`** — simpler, faster, clearer; zero overhead.
- **Upgrade to `shared_ptr` only when ownership is genuinely shared** or its end is indeterminate.
- **Use `weak_ptr` for non-owning access** and to break cycles.
- **Avoid `new`/`delete`** — prefer `make_unique` (C++14) and `make_shared` (C++11) for exception safety and a single allocation.
- **Think in ownership:** "Who owns this? Who merely uses it? Can two objects accidentally keep each other alive?"

**The `value_ptr` / pImpl pattern.** A `value_ptr` (not standard, but common in expert code) is a smart pointer with *value* semantics: copying it deep-copies the pointee. It is the natural backbone of the **pImpl** ("pointer to implementation") idiom, giving a class value semantics while hiding its implementation behind a forward-declared pointer in the header — cutting compile-time coupling, a real win in large codebases.

**`noexcept` moves and the standard library.** This is not stylistic: `std::vector`'s growth uses `std::move_if_noexcept`. A throwing move constructor forces the slow copy path during reallocation, silently erasing the very optimization you wrote the move constructor for. In latency-sensitive systems, audit that your movable types' move operations are `noexcept`.

**Atomic ref-counts are not free.** Every `shared_ptr` copy is an atomic RMW on the strong count, which serializes cache lines across cores. In hot paths, pass `const shared_ptr&` (or a raw/`weak_ptr` observer) instead of copying, and reserve `shared_ptr` *by value* for the points where ownership genuinely changes hands.

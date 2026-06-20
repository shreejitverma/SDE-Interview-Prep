# Chapter 25: Vocabulary Types

> *C++17 standardizes four "vocabulary types" — `optional`, `variant`, `any`, and `string_view` — that give the language a shared, type-safe way to express ideas every codebase had previously hand-rolled: a value that may be absent, a value that is one of several types, a value of any type, and a non-owning view of a string. Because they are standard, they become a common interface across libraries; because they are value types, they integrate with the type system instead of subverting it the way `void*`, sentinel values, and naked pointers do.*

The term *vocabulary type* means a type so broadly useful that it belongs in the shared vocabulary of every interface — the way `std::string` and `std::vector` already were. Before C++17, "no value" was a null pointer or a magic `-1`, "one of several types" was a tagged union maintained by hand, "any type" was a `void*` with a separate type tag, and "a substring without copying" was a `const char* + length` pair. Each of these workarounds reduced the space of valid values, leaked memory-management concerns, or threw away type safety. The four types in this chapter replace them with safe, efficient, standard alternatives — and the `std::in_place` tag family and `std::monostate` round out the construction and empty-state machinery they depend on.

---

## Table of Contents

- [25.1 `std::string_view`](#251-stdstring_view)
- [25.2 `std::optional`](#252-stdoptional)
- [25.3 `std::variant`](#253-stdvariant)
- [25.4 `std::monostate` and Valueless Variants](#254-stdmonostate-and-valueless-variants)
- [25.5 `std::any`](#255-stdany)
- [25.6 In-Place Construction: `in_place`, `in_place_type`, `in_place_index`](#256-in-place-construction-in_place-in_place_type-in_place_index)
- [25.7 Professional Insights: `std::optional`](#257-professional-insights-stdoptional)
- [25.8 Professional Insights: `std::variant`](#258-professional-insights-stdvariant)
- [25.9 Professional Insights](#259-professional-insights)

---

## 25.1 `std::string_view`

`std::string_view` is a **non-owning reference** to a contiguous sequence of characters — a `const char*` plus a length, wrapped in an interface that mirrors `std::string`'s read-only methods. It enables **zero-copy** string operations: passing, slicing, and searching strings without allocating or copying.

### 25.1.1 Efficiency

The canonical use is a function parameter that accepts "any string-like thing" without forcing an allocation. A `std::string` parameter copies (and may allocate); a `std::string_view` parameter binds to a `std::string`, a string literal, or a `const char*` with no copy at all.

```cpp
// Listing 25.1: string_view parameters avoid copies and allocations
#include <string>
#include <string_view>
#include <iostream>

// Bad: copies the string (potentially an expensive allocation)
void print_str(std::string s) {
    std::cout << s << "\n";
}

// Good: no copy, no allocation
void print_view(std::string_view sv) {
    std::cout << sv << "\n";
}

int main() {
    const char* cstr = "Hello World";
    // print_str(cstr); // would construct a std::string (allocates!)
    print_view(cstr);   // no allocation

    std::string s = "Hello World";
    print_view(s);      // also binds to a std::string, no copy

    // Substrings are cheap — substr() returns another view, not a new buffer:
    std::string_view sub = std::string_view(cstr).substr(0, 5);
    print_view(sub);    // "Hello"
}
```

### 25.1.2 Caveats

A `string_view` borrows; it does not own. Two failure modes follow directly:

- **Non-owning / lifetime:** the viewed character buffer must outlive the view. A `string_view` to a temporary `std::string`, or returned from a function that owned the buffer, dangles.
- **Not null-terminated:** a `string_view` knows its length, not a terminator. `sv.data()` is **not** guaranteed to point at a null-terminated C string, so passing it to a C API (`printf`, `fopen`, `strlen`) that expects null termination is a bug unless you have separately ensured termination.

> **Caution:** never store a `string_view` member that outlives the string it views, and never return a `string_view` to a local string. For ownership, use `std::string`; for a borrowed read-only view bounded by the call, use `string_view`.

---

## 25.2 `std::optional`

`std::optional<T>` represents a value that **may or may not be present**. It replaces null pointers for nullable values and "magic" sentinel values (`-1`, `""`, `0`) with an explicit, type-safe "maybe a `T`." An `optional<int>` either contains an `int` or contains nothing — and, unlike a pointer, it stores its `T` inline with no dynamic allocation.

```cpp
// Listing 25.2: returning "maybe a value" with std::optional
#include <optional>
#include <vector>
#include <iostream>

std::optional<int> find_even(const std::vector<int>& v) {
    for (int x : v) {
        if (x % 2 == 0) return x;
    }
    return std::nullopt;   // or {} — the empty state
}

int main() {
    auto res = find_even({1, 3, 5});
    if (res) {                    // or res.has_value()
        std::cout << *res;        // or res.value() — value() throws if empty
    } else {
        std::cout << "Not found";
    }

    std::cout << res.value_or(0); // the value, or 0 if empty
}
```

The interface is small and deliberate:

- `has_value()` / contextual `bool` — is a value present?
- `operator*` / `operator->` — access the value **without** a check (UB if empty, like dereferencing a pointer).
- `value()` — access the value **with** a check (throws `std::bad_optional_access` if empty).
- `value_or(default)` — the value, or a supplied fallback.
- `std::nullopt` — the empty-state literal; `{}` also constructs an empty optional.

The two distinct accessors encode intent: `*opt` says "I have already checked," `value()` says "check for me." Sections 25.7 develops the design rationale in depth.

---

## 25.3 `std::variant`

`std::variant<Ts...>` is a **type-safe union**: it holds exactly one value, of one of its listed alternative types, and always knows which. It replaces the C `union` + manual type-tag pattern, which the compiler cannot police, with one the type system enforces.

```cpp
// Listing 25.3: a type-safe union and the visitor pattern
#include <variant>
#include <string>
#include <iostream>

int main() {
    std::variant<int, float, std::string> v;
    v = 10;
    v = 3.14f;
    v = "hello";        // now holds a std::string

    // Direct access by type — throws std::bad_variant_access on the wrong type:
    try {
        std::string s = std::get<std::string>(v);
        // int i = std::get<int>(v);  // would throw: active type is string
    } catch (const std::bad_variant_access&) {}

    // std::visit applies a callable to whichever alternative is active:
    std::visit([](auto&& arg) {
        std::cout << arg << "\n";
    }, v);
}
```

The access mechanisms differ in how they handle a wrong guess:

- `std::get<T>(v)` / `std::get<I>(v)` — returns the value, **throws** `std::bad_variant_access` if `T`/index `I` is not active.
- `std::get_if<T>(&v)` — returns a **pointer** to the value, or `nullptr` if `T` is not active (no exception).
- `std::visit(visitor, v)` — calls `visitor` with the active alternative; the compiler requires the visitor to handle every alternative, which is what makes exhaustive dispatch checkable at compile time.
- `v.index()` — the zero-based index of the active alternative.

A `variant` guarantees **no dynamic allocation** beyond whatever its contained types allocate — the storage for the largest alternative is held inline. This makes it suitable for low-latency code where a tagged value must avoid the heap.

---

## 25.4 `std::monostate` and Valueless Variants

A `std::variant` is never "empty" in the `optional` sense — it always holds one of its alternatives. But two situations need handling: a variant whose **first alternative is not default-constructible**, and the rare **valueless** state.

**`std::monostate`** is an empty placeholder type whose sole purpose is to be a default-constructible first alternative. Because a default-constructed `variant` value-initializes its *first* alternative, listing `monostate` first gives the variant a well-defined "no meaningful value yet" state even when none of the real alternatives can be default-constructed.

```cpp
// Listing 25.4: monostate provides a default-constructible empty state
#include <variant>

struct NoDefault { NoDefault() = delete; NoDefault(int) {} };

// std::variant<NoDefault, int> v;  // ERROR: first alternative not default-constructible

std::variant<std::monostate, NoDefault, int> v;  // OK: default-constructs to monostate
// v.index() == 0  -> holds monostate, i.e. "empty / uninitialized"
```

**The valueless state** arises only when an assignment or emplacement throws *while changing the active alternative*, leaving the variant with no valid value. `v.valueless_by_exception()` detects it, and a valueless variant has `index() == std::variant_npos`. This is rare, but `std::visit` on a valueless variant throws `std::bad_variant_access`, so robust code either avoids throwing alternatives or checks for it.

```cpp
// Listing 25.5: detecting the valueless-by-exception state
if (v.valueless_by_exception()) {
    // recovery: reassign a known-good alternative before visiting
}
```

---

## 25.5 `std::any`

`std::any` is a type-safe container for a **single value of any type** — the safe replacement for `void*`. Unlike `variant`, the set of types is not fixed in advance; unlike `void*`, the stored type is remembered and every extraction is checked.

```cpp
// Listing 25.6: type-erased storage with checked extraction
#include <any>
#include <string>
#include <iostream>

int main() {
    std::any a = 1;                       // holds an int
    a = std::string("hello");             // now holds a std::string

    try {
        std::string s = std::any_cast<std::string>(a);   // ok
        // int i = std::any_cast<int>(a);  // throws: active type is string
    } catch (const std::bad_any_cast& e) {
        std::cout << e.what();
    }
}
```

Key points:

- `std::any_cast<T>(a)` extracts the value, **throwing** `std::bad_any_cast` on a type mismatch; the pointer form `std::any_cast<T>(&a)` returns `nullptr` instead.
- `a.has_value()` reports whether anything is stored; `a.type()` returns the `std::type_info` of the stored type.
- `any` typically allocates dynamically for larger stored types (small-object optimization may avoid it for small ones), so it is the **least** suited of the four vocabulary types to a hot path.

Choose between the three "holds something" types by how much you know: `variant` when the alternatives are a fixed, known set (fastest, no allocation); `any` when the type is genuinely open-ended; `optional` when there is one type that is simply present-or-not.

---

## 25.6 In-Place Construction: `in_place`, `in_place_type`, `in_place_index`

The vocabulary types must sometimes construct their contained object **directly inside themselves** from constructor arguments — to avoid a temporary, or to disambiguate which alternative to build. C++17 provides a family of empty **tag types** that select in-place construction:

- **`std::in_place`** (type `std::in_place_t`) — tells `std::optional` to construct its `T` in place from the following arguments, rather than copying or moving an existing `T`.
- **`std::in_place_type<T>`** — tells `std::variant` or `std::any` to construct the alternative *of type `T`* in place.
- **`std::in_place_index<I>`** — tells `std::variant` to construct the alternative *at index `I`* in place (needed when two alternatives share a type, or to be explicit).

```cpp
// Listing 25.7: in-place construction avoids a temporary in optional
#include <optional>
#include <string>

// Without in_place: construct a temporary string, then move it into the optional.
std::optional<std::string> o1{std::string(5, 'x')};

// With in_place: construct the string directly inside the optional from (5, 'x').
std::optional<std::string> o2{std::in_place, 5, 'x'};   // "xxxxx", no temporary
```

For `variant`, the tags resolve ambiguity that argument types alone cannot — especially with `initializer_list` constructors and deleted default constructors:

```cpp
// Listing 25.8: constructing variants with in_place_type / in_place_index
#include <variant>
#include <initializer_list>

struct A {};
struct B { B() = default; B(B const&) = default; B(int) {} };
struct C { C() = delete; C(int) {} C(C const&) = default; };
struct D { D(std::initializer_list<int>) {} D(D const&) = default; D() = default; };

std::variant<A, B>    var_ab0;                              // contains A()
std::variant<A, B>    var_ab1 = 7;                          // contains B(7)
std::variant<A, B>    var_ab2 = var_ab1;                    // contains B(7)
std::variant<A, B, C> var_abc0{std::in_place_type<C>, 7};   // contains C(7)
// std::variant<C>    var_c0;                               // illegal: C has no default ctor
std::variant<A, D>    var_ad0(std::in_place_type<D>, {1,3,3,4});   // contains D{1,3,3,4}
std::variant<A, D>    var_ad1(std::in_place_index<0>);             // contains A{}
std::variant<A, D>    var_ad2(std::in_place_index<1>, {1,3,3,4});  // contains D{1,3,3,4}
```

The same tags appear in `std::any`'s constructor (`std::any a{std::in_place_type<std::string>, 5, 'x'};`). Across all three types the principle is identical: the tag names *what* to build, the trailing arguments are forwarded to *its* constructor, and no intermediate object is created.

---

## 25.7 Professional Insights: `std::optional`

### 25.7.1 Representing the Absence of a Value

Before C++17, a `nullptr` pointer commonly represented "no value." That works for large objects already dynamically allocated and managed by pointers, but works badly for small or primitive types such as `int`, which are rarely heap-allocated or managed by pointers. `std::optional` solves exactly this: a value type that can be present or absent without involving the heap.

Here a `Person` may or may not have a pet, so the `pet` member is wrapped in `std::optional`:

```cpp
// Listing 25.9: optional as an optional member
#include <iostream>
#include <optional>
#include <string>

struct Animal { std::string name; };

struct Person {
    std::string name;
    std::optional<Animal> pet;
};

int main() {
    Person person;
    person.name = "John";
    if (person.pet) {
        std::cout << person.name << "'s pet's name is "
                  << person.pet->name << std::endl;
    } else {
        std::cout << person.name << " is alone." << std::endl;
    }
}
```

### 25.7.2 `optional` as a Return Value

Returning an `optional` is the natural way to express a computation that may have no result:

```cpp
// Listing 25.10: returning the empty optional for an undefined result
std::optional<float> divide(float a, float b) {
    if (b != 0.f) return a / b;
    return {};                         // division undefined -> empty
}
```

A richer case returns an optional *iterator* from a generic search, letting callers test the result directly in an `if`:

```cpp
// Listing 25.11: a find that returns optional<iterator>
template <class Range, class Pred>
auto find_if(Range&& r, Pred&& p) {
    using std::begin; using std::end;
    auto b = begin(r), e = end(r);
    auto it = std::find_if(b, e, p);
    using iterator = decltype(it);
    if (it == e) return std::optional<iterator>();
    return std::optional<iterator>(it);
}

template <class Range, class T>
auto find(Range&& r, T const& t) {
    return find_if(std::forward<Range>(r),
                   [&t](auto&& x){ return x == t; });
}
```

This enables the clean call-site idioms `if (find(vec, 7)) { ... }` and, binding the result, `if (auto oit = find(vec, 7)) { vec.erase(*oit); }` — no separate begin/end juggling.

### 25.7.3 `value_or` Pushes the Default Decision to the Call Site

```cpp
// Listing 25.12: value_or supplies a fallback exactly where it is needed
void print_name(std::ostream& os, std::optional<std::string> const& name) {
    os << "Name is: " << name.value_or("<name missing>") << '\n';
}
```

`value_or` returns the stored value, or its argument when the optional is empty. The design win is that the "what to do when absent" decision is made **at the point of use**, where the right default is known and immediately needed — instead of being baked into some default value deep inside the engine that produced the optional.

### 25.7.4 Why `optional`, Not the Alternatives

`std::optional<T>` is more complete than the three traditional workarounds:

- **vs. a pointer:** a pointer can signal failure with `nullptr`, but only for objects that already exist. `optional`, being a value type, can also *return a new object* with no memory allocation.
- **vs. a sentinel value:** reserving `0`/`-1`/`nullptr` to mean "meaningless" shrinks the space of valid values — you cannot distinguish a valid `0` from a meaningless one — and many types have no natural sentinel.
- **vs. `std::pair<bool, T>`:** this requires `T` to be default-constructible for the failure case, which is impossible for some types and undesirable for others. `optional<T>` constructs nothing in the empty case.

### 25.7.5 Representing the Failure of a Function

Historically a function signaled failure by returning a null pointer, by reserving a special return value (e.g. `unsigned shortest_path_distance` returning `0` for unconnected vertices), or by pairing the value with a `bool`. `optional` subsumes all three. Here `pet_with_name` returns `std::nullopt` when no matching pet exists:

```cpp
// Listing 25.13: optional as a clean function-failure signal
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Animal { std::string name; };

struct Person {
    std::string name;
    std::vector<Animal> pets;
    std::optional<Animal> pet_with_name(const std::string& name) {
        for (const Animal& pet : pets) {
            if (pet.name == name) return pet;
        }
        return std::nullopt;
    }
};

int main() {
    Person john;
    john.name = "John";
    Animal fluffy;  fluffy.name  = "Fluffy";  john.pets.push_back(fluffy);
    Animal furball; furball.name = "Furball"; john.pets.push_back(furball);

    std::optional<Animal> whiskers = john.pet_with_name("Whiskers");
    if (whiskers) {
        std::cout << "John has a pet named Whiskers." << std::endl;
    } else {
        std::cout << "Whiskers must not belong to John." << std::endl;
    }
}
```

---

## 25.8 Professional Insights: `std::variant`

### 25.8.1 Lightweight Type Erasure: Pseudo-Method Pointers

`variant` can drive a form of lightweight type erasure. The following advanced example overloads `operator->*` with a variant on the left, producing something that behaves like a method pointer dispatching across unrelated types — using CTAD (Chapter 24) to deduce the callable's type and `std::visit` to recover the active alternative:

```cpp
// Listing 25.14: a variant-driven "pseudo-method" via operator->*
template <class F>
struct pseudo_method {
    F f;
    // enable C++17 class template argument deduction:
    pseudo_method(F&& fin) : f(std::move(fin)) {}

    // Koenig-lookup operator->*; LHS is a variant:
    template <class Variant>   // (one could add a SFINAE test that LHS is a variant)
    friend decltype(auto) operator->*(Variant&& var, pseudo_method const& method) {
        // returns a lambda that perfect-forwards the call, like a method pointer:
        return [&](auto&&... args) -> decltype(auto) {
            return std::visit(
                [&](auto&& self) -> decltype(auto) {
                    // decltype(x)(x) is perfect forwarding inside a lambda:
                    return method.f(decltype(self)(self), decltype(args)(args)...);
                },
                std::forward<Variant>(var));
        };
    }
};
```

Defining a `print` pseudo-method whose first parameter is `self` lets a single call dispatch to the active alternative's own `print`:

```cpp
// Listing 25.15: dispatching across unrelated types through the variant
pseudo_method print = [](auto&& self, auto&&... args) -> decltype(auto) {
    return decltype(self)(self).print(decltype(args)(args)...);
};

struct A { void print(std::ostream& os) const { os << "A"; } };
struct B { void print(std::ostream& os) const { os << "B"; } };

std::variant<A, B> var = A{};
(var->*print)(std::cout);     // dispatches to A::print; B{} would dispatch to B::print
```

Crucially this is checked at compile time: adding an alternative with no `print` member,

```cpp
struct C {};
std::variant<A, B, C> var2 = A{};
// (var2->*print)(std::cout);   // FAILS TO COMPILE: C has no print(std::ostream&)
```

fails to compile, because `C.print(std::cout)` is ill-formed. The technique extends to detecting free-function `print` overloads, possibly using `if constexpr` (Chapter 23) inside the pseudo-method.

### 25.8.2 Basic Use and the No-Allocation Guarantee

```cpp
// Listing 25.16: storing and accessing a variant
using namespace std::string_literals;

std::variant<int, std::string> var;   // a tagged union of int | string
var = "hello"s;                        // now holds a string

// Access via std::visit with a polymorphic lambda — prints "hello\n":
std::visit([](auto&& e){ std::cout << e << '\n'; }, var);

// If certain of the type, get it (throws on a wrong guess):
auto str = std::get<std::string>(var);

// get_if returns nullptr instead of throwing on a wrong guess:
auto* p = std::get_if<std::string>(&var);
```

A variant performs **no dynamic allocation** beyond what its contained types allocate; only one alternative is stored at a time. In rare cases — an exception while assigning, with no safe way to back out — a variant can become valueless (Section 25.4). Variants are, in short, smart, type-safe unions that store multiple value types in one variable safely and efficiently.

### 25.8.3 Constructing a `std::variant`

The construction rules (allocators aside) follow naturally from the alternative list and the `in_place` tags of Section 25.6. The full set of cases — default construction of the first alternative, conversion to the best-matching alternative, copy, and the `in_place_type`/`in_place_index` disambiguators for deleted-default and `initializer_list` constructors — is shown in Listing 25.8.

---

## 25.9 Professional Insights

**Reach for the vocabulary type that matches what you know about the value.** `optional<T>` for present-or-absent of one type; `variant<Ts...>` for one-of-a-fixed-set (no allocation, fastest); `any` only when the type is genuinely open-ended (and accept its allocation cost). Each replaces an unsafe idiom — null pointers, hand-tagged unions, `void*` — with one the compiler checks.

**Treat `string_view` strictly as a borrowed, call-scoped view.** It is the right parameter type for read-only string input because it copies nothing and accepts every string-like source. But it owns nothing and is not null-terminated: never let one outlive its buffer, never return one to a local string, and never hand `.data()` to a C API expecting termination. For ownership, store a `std::string`.

**Use the two `optional`/`variant` accessor styles to encode intent.** `*opt` and `std::get<T>(v)` say "I have already verified the state"; `opt.value()`, `v.index()`, and `std::get_if` say "check for me." Pick deliberately — an unchecked `*` on an empty optional or a wrong-type `get` is undefined behavior or a thrown exception, respectively.

**Prefer `std::visit` over chains of `get_if` for variants.** Visitation forces you to handle every alternative, turning "forgot a case" into a compile error — the same exhaustiveness guarantee a `switch` over an enum should give but rarely does. Combined with the `overloaded{...}` idiom (Chapter 23), it is the idiomatic, future-proof way to consume a variant.

**Default variants with `std::monostate` first when no alternative is default-constructible, and use the `in_place` tags to construct without temporaries.** `monostate` gives a well-defined empty/uninitialized state; `in_place`, `in_place_type<T>`, and `in_place_index<I>` build the contained object directly inside the optional/variant/any, avoiding a temporary and disambiguating overlapping alternatives — the same efficiency principle as `emplace` for containers.

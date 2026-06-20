# Chapter 05: OOP and Encapsulation

> *Organising state, binding behaviour, and building types that feel like the language itself.*

Procedural C++ gives you functions that operate on data passed through parameters. Object-Oriented Programming gives you something more powerful: the ability to define entirely new *types* that bundle state and behaviour together, enforce invariants through access control, and interact with the language's built-in syntax through operator overloading. This chapter covers the full OOP toolkit of C++98/03 — enumerations, unions, namespaces, class design, constructors, the Rule of Three, operator overloading, templates, and the casting system — everything you need before tackling inheritance and polymorphism in Chapter 6.

---

## Table of Contents

- [5.1 Enumerations](#51-enumerations)
- [5.2 Unions and Shared Memory Layout](#52-unions-and-shared-memory-layout)
- [5.3 Namespaces](#53-namespaces)
- [5.4 The Class Blueprint: Classes vs. Objects](#54-the-class-blueprint-classes-vs-objects)
- [5.5 Access Modifiers and Encapsulation](#55-access-modifiers-and-encapsulation)
- [5.6 Constructors and Destructors](#56-constructors-and-destructors)
- [5.7 The Rule of Three](#57-the-rule-of-three)
- [5.8 Static Members](#58-static-members)
- [5.9 Friend Functions and Friend Classes](#59-friend-functions-and-friend-classes)
- [5.10 Operator Overloading](#510-operator-overloading)
- [5.11 Functors: Overloading `operator()`](#511-functors-overloading-operator)
- [5.12 Generic Programming: Templates](#512-generic-programming-templates)
- [5.13 Type Conversions and Casting](#513-type-conversions-and-casting)
- [5.14 Advanced Class Features](#514-advanced-class-features)
- [5.15 Professional Insights: Name Mangling, ADL, and Copy Semantics](#515-professional-insights-name-mangling-adl-and-copy-semantics)

---

## 5.1 Enumerations

When a variable must represent one of a fixed, named set of states, use an **enumeration** rather than raw integers. A bare integer forces every reader to look up what `3` means; an enum makes intent explicit.

### 5.1.1 C-Style Enums

```cpp
// Listing 5.1: C-style (unscoped) enumeration
enum GameState {
    MENU,       // Implicitly 0
    PLAYING,    // Implicitly 1
    PAUSED,     // Implicitly 2
    GAME_OVER   // Implicitly 3
};

GameState current = PLAYING;

if (current == 1) {              // Valid but terrible practice
    std::cout << "Playing.\n";
}
```

**Danger:** C-style enum names leak into the enclosing scope. Defining `enum VideoState { PLAYING, STOPPED };` in the same translation unit collides with `GameState::PLAYING`. They also convert implicitly to `int`, destroying type safety.

### 5.1.2 Scoped Enums (`enum class`) — Forward Reference: C++11

C++11 introduced scoped enumerations. The names are contained inside the enum's scope and do not convert to integers implicitly.

```cpp
// Listing 5.2: Scoped enumeration (C++11, forward reference)
// enum class GameState { Menu, Playing, Paused, GameOver };
// GameState current = GameState::Playing;
// if (current == GameState::Playing) { ... }  // Must qualify the name
```

In C++98/03 code, avoid the implicit-conversion pitfall by always qualifying enum values explicitly and never comparing them to integer literals.

---

## 5.2 Unions and Shared Memory Layout

A **union** places all its members at the same starting address. The size of a union equals the size of its largest member. Only one member may hold a meaningful value at any given time.

```cpp
// Listing 5.3: Union for low-level byte reinterpretation
union PacketData {
    int   as_integer;
    float as_float;
    char  as_bytes[4];
};

int main() {
    PacketData packet;
    std::cout << "Size: " << sizeof(packet) << "\n"; // 4 bytes

    packet.as_integer = 42;
    std::cout << packet.as_integer << "\n"; // 42

    packet.as_float = 3.14f;
    // as_integer is now garbage — float bits overwrite the same 4 bytes
    std::cout << packet.as_integer << "\n"; // Undefined value
    return 0;
}
```

Unions are essential in network drivers and embedded microcontrollers where the same four bytes must be interpreted as different types without copying. The compiler tracks nothing about which member is "active"; that responsibility falls entirely on the programmer. In modern C++ (`std::variant`, C++17) provides a type-safe alternative, but in C++98/03, disciplined use of unions is the only option.

---

## 5.3 Namespaces

As a codebase grows and third-party libraries are integrated, **naming collisions** become inevitable. Two libraries may both define a class called `Vector3D`. **Namespaces** solve this by partitioning the global scope into named regions.

```cpp
// Listing 5.4: Namespace basics
#include <iostream>
using namespace std;

namespace Math {
    double PI = 3.14159;
    double circle_area(double r) { return PI * r * r; }
}

namespace Graphics {
    double PI = 3.14;
    void draw_circle(double r) {
        cout << "Drawing circle, radius " << r << endl;
    }
}

int main() {
    cout << Math::PI << endl;          // 3.14159
    cout << Graphics::PI << endl;      // 3.14
    cout << Math::circle_area(5) << endl;
    Graphics::draw_circle(5);
    return 0;
}
```

### 5.3.1 Namespace Aliases

Long nested namespace paths are abbreviated with an alias:

```cpp
// Listing 5.5: Namespace alias
namespace Very { namespace Long { namespace Namespace {
    void function() { std::cout << "Long namespace\n"; }
}}}

namespace VLN = Very::Long::Namespace;

int main() {
    VLN::function();
    return 0;
}
```

### 5.3.2 The `using` Directive and Its Dangers

`using namespace std;` pulls every identifier from `std` into the current scope. In `.cpp` files for small programs this is tolerable. In header files it is **catastrophic**: every file that includes the header gets the entire standard library dumped into its global namespace, causing unpredictable collisions across large codebases. Never use `using namespace` in a header.

### 5.3.3 Anonymous Namespaces

An unnamed namespace gives its contents **internal linkage** — the modern, superior replacement for C-style `static` at file scope:

```cpp
// Listing 5.6: Anonymous namespace for internal linkage
// math_helpers.cpp
namespace {
    int secret_internal_calculation(int x) { return x * x; }
}

int public_math_function(int x) {
    return secret_internal_calculation(x);
}
```

`secret_internal_calculation` is invisible to every other translation unit. Use anonymous namespaces liberally for implementation-detail helpers.

---

## 5.4 The Class Blueprint: Classes vs. Objects

A **class** is a blueprint. An **object** (or *instance*) is a concrete realisation of that blueprint built at runtime. You can construct thousands of objects from a single class definition; each occupies its own region of memory and holds its own copy of instance data.

```cpp
// Listing 5.7: Class definition and object instantiation
class Player {
public:
    int health;
    int ammo;

    void shoot() {
        ammo -= 1;
    }
};

int main() {
    Player player1;
    Player player2;

    player1.ammo = 10;
    player2.ammo = 100;

    player1.shoot(); // Only player1 loses ammo
    return 0;
}
```

The `class` keyword and the `struct` keyword produce identical constructs with one difference: members of a `class` default to **private**, while members of a `struct` default to **public**. By convention, use `struct` for passive data aggregates and `class` for types with invariants and behaviour.

---

## 5.5 Access Modifiers and Encapsulation

**Encapsulation** is the practice of hiding internal state behind a public interface. The three access specifiers are:

| Specifier | Accessible from |
| :-------- | :-------------- |
| `public` | Anywhere |
| `protected` | The class itself and derived classes |
| `private` | Only the class itself (and declared friends) |

```cpp
// Listing 5.8: Encapsulated BankAccount
class BankAccount {
private:
    double balance; // Internal state — no direct external access

public:
    void deposit(double amount) {
        if (amount > 0) balance += amount;
    }

    double get_balance() const { return balance; }
};

int main() {
    BankAccount acct;
    // acct.balance = 1000000; // ERROR: private
    acct.deposit(500);
    return 0;
}
```

Separating the *interface* (`deposit`, `get_balance`) from the *implementation* (`balance`) is **decoupling**: callers remain unaffected when internal storage representation changes.

**`struct` vs `class` access in practice:**

```cpp
// Listing 5.9: struct (public default) vs. class (private default)
struct MyStruct { int x; };
class  MyClass  { int x; };

MyStruct s;
s.x = 9;   // OK — x is public

MyClass c;
// c.x = 9; // ill-formed — x is private
```

---

## 5.6 Constructors and Destructors

A **constructor** is a special member function called automatically when an object is created. It has the same name as the class and no return type. A **destructor** is called automatically when the object is destroyed (goes out of scope, or `delete` is called on a heap-allocated instance). It is identified by the prefix `~`.

```cpp
// Listing 5.10: Default, parameterised constructors and destructor
#include <iostream>
#include <string>

class Car {
private:
    std::string brand;
    int*        buffer;

public:
    // Default constructor
    Car() : brand("Unknown"), buffer(new int[10]) {
        std::cout << "Default constructor\n";
    }

    // Parameterised constructor
    Car(const std::string& b) : brand(b), buffer(new int[10]) {
        std::cout << "Param constructor: " << brand << "\n";
    }

    // Copy constructor (deep copy)
    Car(const Car& other) : brand(other.brand), buffer(new int[10]) {
        std::cout << "Copy constructor\n";
    }

    // Destructor
    ~Car() {
        delete[] buffer;
        std::cout << "Destructor: " << brand << "\n";
    }
};

int main() {
    std::cout << "--- Start ---\n";
    {
        Car c1("Toyota");  // Parameterised constructor
    }                      // Destructor called here as c1 leaves scope
    std::cout << "--- End ---\n";
    return 0;
}
```

The **member initialiser list** (`: brand(b), buffer(new int[10])`) is preferred over assignment inside the constructor body because it initialises members directly rather than default-constructing them first and then assigning.

---

## 5.7 The Rule of Three

If your class manages a resource acquired with `new` (or a file handle, network socket, or any other resource that requires manual release), the compiler-generated copy operations perform **shallow copies** — they copy the pointer value, not the pointed-to data. Two objects then point to the same resource, and the destructor of each will attempt to release it, producing a double-free.

**The Rule of Three:** if you explicitly define *any* of the following, you almost certainly need to define all three:

1. **Destructor** — to release the resource.
2. **Copy constructor** — to allocate a new resource and deep-copy the data.
3. **Copy assignment operator** — to handle `a = b` between two live objects.

```cpp
// Listing 5.11: Rule of Three for heap-managed buffer
class Buffer {
    int* ptr;
    int  sz;

public:
    Buffer(int n) : sz(n), ptr(new int[n]) {}

    // Destructor
    ~Buffer() { delete[] ptr; }

    // Copy constructor
    Buffer(const Buffer& other) : sz(other.sz), ptr(new int[other.sz]) {
        for (int i = 0; i < sz; ++i) ptr[i] = other.ptr[i];
    }

    // Copy assignment operator
    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;  // Self-assignment guard

        delete[] ptr;                      // Release old resource
        sz  = other.sz;
        ptr = new int[sz];
        for (int i = 0; i < sz; ++i) ptr[i] = other.ptr[i];
        return *this;
    }
};
```

The self-assignment check (`if (this == &other)`) is mandatory: without it, the assignment operator deletes `ptr` before copying from `other.ptr` — which is the same pointer when `a = a`.

---

## 5.8 Static Members

A **static data member** belongs to the class, not to any individual instance. It is shared by all objects of that class and must be defined (storage allocated) outside the class in exactly one translation unit.

```cpp
// Listing 5.12: Static member for instance counting
class Player {
public:
    static int player_count; // Declaration

    Player()  { ++player_count; }
    ~Player() { --player_count; }
};

int Player::player_count = 0; // Definition — one occurrence in a .cpp file

int main() {
    Player p1, p2;
    std::cout << Player::player_count << "\n"; // 2
    return 0;
}
```

A **static member function** may only access static members; it has no `this` pointer.

---

## 5.9 Friend Functions and Friend Classes

A `friend` declaration grants a specific function or class access to `private` and `protected` members, bypassing normal access control. Friendship is not symmetric (A befriending B does not give B access to A's privates unless B also declares the friendship) and not inherited.

### 5.9.1 Friend Function

```cpp
// Listing 5.13: Friend function accessing private data
class PrivateHolder {
    int private_value;
public:
    PrivateHolder(int v) : private_value(v) {}
    friend void friend_function(PrivateHolder& ph);
};

void friend_function(PrivateHolder& ph) {
    std::cout << ph.private_value << "\n"; // OK — declared as friend
}
```

### 5.9.2 Friend Class

```cpp
// Listing 5.14: Friend class — whole class given access
class Accesser {
public:
    void access1(const PrivateHolder& ph) { /* can see private_value */ }
    void access2(const PrivateHolder& ph) { /* can see private_value */ }
};

class PrivateHolder {
    int private_value;
public:
    PrivateHolder(int v) : private_value(v) {}
    friend class Accesser;
};
```

**Use friends sparingly.** They break encapsulation. The legitimate use cases are: overloading `operator<<` for stream output, and tightly coupled pairs of classes (e.g., an iterator and its container).

---

## 5.10 Operator Overloading

**Operator overloading** allows user-defined types to interact with C++ syntax exactly like built-in types. An overloaded operator is a function with the name `operator` followed by the operator symbol.

### 5.10.1 Binary Arithmetic Operators as Member Functions

```cpp
// Listing 5.15: Complex number with overloaded + operator
class Complex {
private:
    double r, i;
public:
    Complex(double real, double imag) : r(real), i(imag) {}

    Complex operator+(const Complex& other) const {
        return Complex(r + other.r, i + other.i);
    }

    void print() const { std::cout << r << " + " << i << "i\n"; }
};

int main() {
    Complex c1(1.0, 2.0), c2(3.0, 4.0);
    Complex c3 = c1 + c2; // c1.operator+(c2)
    c3.print();            // 4.0 + 6.0i
    return 0;
}
```

### 5.10.2 Non-Member Operators for Symmetric Conversions

When the left-hand operand is not a class instance (e.g., `5.0 + complex_value`), a member function cannot be used — the operator must be a free function, declared `friend` if it needs private access:

```cpp
// Listing 5.16: Non-member operator+ allowing double + Complex
class Complex {
    double r, i;
public:
    Complex(double real, double imag) : r(real), i(imag) {}
    friend Complex operator+(double left, const Complex& right);
};

Complex operator+(double left, const Complex& right) {
    return Complex(left + right.r, right.i);
}
```

### 5.10.3 Comparison Operators

Overloading `==` and `<` enables custom types to be sorted by standard algorithms and stored in ordered containers:

```cpp
// Listing 5.17: Comparison operators
class Player {
public:
    int score;
    explicit Player(int s) : score(s) {}

    bool operator==(const Player& other) const { return score == other.score; }
    bool operator< (const Player& other) const { return score <  other.score; }
};
```

### 5.10.4 The Stream Output Operator `<<`

Because `std::cout` is an `std::ostream` (the left operand), `operator<<` cannot be a member of your class. It must be a non-member friend:

```cpp
// Listing 5.18: Overloading << for stream output
#include <iostream>
#include <string>

class Player {
    std::string name;
    int score;
public:
    Player(const std::string& n, int s) : name(n), score(s) {}

    friend std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << "[" << p.name << " - Score: " << p.score << "]";
        return os; // Return the stream to allow chaining: cout << p1 << p2
    }
};

int main() {
    Player p1("Alice", 99);
    std::cout << p1 << "\n"; // [Alice - Score: 99]
    return 0;
}
```

### 5.10.5 The Assignment Operator and Self-Assignment

The compiler generates an assignment operator that performs a memberwise copy. If your class manages heap resources, you must write one that deep-copies and checks for self-assignment:

```cpp
// Listing 5.19: Copy assignment operator with self-assignment guard
class Buffer {
    int* data;
public:
    Buffer()  { data = new int[10]; }
    ~Buffer() { delete[] data; }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;  // Guard against a = a

        delete[] data;
        data = new int[10];
        for (int i = 0; i < 10; ++i) data[i] = other.data[i];
        return *this;
    }
};
```

---

## 5.11 Functors: Overloading `operator()`

Overloading `operator()` produces a **functor** (function object) — an instance of a class that can be called with `()` syntax. Unlike plain function pointers, functors carry state.

```cpp
// Listing 5.20: Functor with captured state
class Multiplier {
    int factor;
public:
    explicit Multiplier(int f) : factor(f) {}

    int operator()(int value) const { return value * factor; }
};

int main() {
    Multiplier times_five(5);
    std::cout << times_five(10) << "\n"; // 50
    return 0;
}
```

Functors are the primary mechanism for passing custom behaviour to STL algorithms in C++98/03 — before lambdas arrived in C++11.

---

## 5.12 Generic Programming: Templates

**Templates** enable code that is parameterised by type, resolved at compile time. They are the foundation of the Standard Template Library and the mechanism behind zero-cost abstractions in C++.

### 5.12.1 Function Templates

```cpp
// Listing 5.21: Function template deducing type from arguments
#include <iostream>

template <typename T>
T myMax(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    std::cout << myMax(10, 20) << "\n";       // T = int
    std::cout << myMax(3.14, 2.71) << "\n";   // T = double
    // myMax(10, 3.14) would fail — mismatched deduced types
    return 0;
}
```

### 5.12.2 Class Templates

```cpp
// Listing 5.22: Class template for a generic container
#include <iostream>
#include <string>

template <typename T>
class Box {
    T content;
public:
    explicit Box(T val) : content(val) {}
    T getContent() const { return content; }
    void setContent(T val) { content = val; }
};

int main() {
    Box<int>         intBox(123);
    Box<std::string> strBox("Hello");
    std::cout << intBox.getContent() << "\n"; // 123
    return 0;
}
```

### 5.12.3 Multiple Template Parameters

```cpp
// Listing 5.23: Template with two type parameters
template <typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;
    Pair(T1 a, T2 b) : first(a), second(b) {}
};

int main() {
    Pair<std::string, int> p("Alice", 30);
    return 0;
}
```

### 5.12.4 Non-Type Template Parameters

Template parameters can be compile-time constant values, not just types. This is how fixed-size arrays avoid the runtime overhead of dynamic allocation:

```cpp
// Listing 5.24: Non-type template parameter for compile-time size
template <typename T, int N>
class Array {
    T data[N];
public:
    int size() const { return N; }
    T&  operator[](int i) { return data[i]; }
};

int main() {
    Array<int, 5> arr;  // Fixed 5-element array on the stack
    return 0;
}
```

`Array<int, 5>` and `Array<int, 10>` are distinct types; they cannot be assigned to each other.

### 5.12.5 Full Template Specialisation

You can provide a custom implementation for a specific type:

```cpp
// Listing 5.25: Full template specialisation for bool
template <typename T>
class Formatter {
public:
    void format(T val) { std::cout << "General: " << val << "\n"; }
};

template <>  // Full specialisation
class Formatter<bool> {
public:
    void format(bool val) {
        std::cout << "Boolean: " << (val ? "true" : "false") << "\n";
    }
};
```

### 5.12.6 Partial Specialisation (Class Templates Only)

```cpp
// Listing 5.26: Partial specialisation when both types are the same
template <typename T1, typename T2>
class MyMap { /* general implementation */ };

template <typename T>
class MyMap<T, T> { /* optimised implementation for same-typed key/value */ };
```

### 5.12.7 The `typename` Keyword for Dependent Types

Inside a template, a name that depends on a template parameter is *dependent*. The compiler cannot tell whether `T::iterator` is a type or a static member without disambiguation:

```cpp
// Listing 5.27: typename for dependent type
template <typename T>
void print_front(T& container) {
    typename T::iterator it = container.begin(); // 'typename' required
    std::cout << *it << "\n";
}
```

### 5.12.8 Templates vs. Macros

| Property | Templates | Macros |
| :-------- | :-------- | :----- |
| Type safety | Yes — compiler checks types | No — text substitution |
| Debugging | Named in error messages | Invisible after substitution |
| Scoping | Obeys C++ scoping rules | No scoping |
| Performance | Zero overhead after instantiation | Same |

Always prefer templates over macros for code that needs to work with multiple types.

---

## 5.13 Type Conversions and Casting

C++ is strongly typed. When a conversion between types is necessary, it must be explicit and deliberate. C++ provides four named cast operators, each with a well-defined scope of validity.

### 5.13.1 Implicit Conversions

The compiler sometimes inserts conversions automatically. While often convenient, they are a common source of subtle bugs:

```cpp
// Listing 5.28: Implicit truncation
double pi = 3.14159;
int x = pi;          // Silently truncated to 3
```

To prevent implicit construction of your class from unrelated types, mark single-argument constructors `explicit`:

```cpp
// Listing 5.29: explicit constructor prevents silent conversions
class Vector {
public:
    explicit Vector(int size) { /* ... */ }
};

// Vector v = 5;  // ERROR: explicit constructor blocks this
Vector v(5);      // OK: direct initialisation
```

### 5.13.2 C-Style Cast — Avoid

```cpp
double pi = 3.14;
int x = (int)pi;  // Legal but dangerous and unsearchable
```

C-style casts try every possible conversion including dangerous `reinterpret_cast`-style reinterpretations. They produce no diagnostic, are invisible in search results, and should never appear in new C++ code.

### 5.13.3 `static_cast` — The Workhorse

`static_cast` performs conversions that are valid according to compile-time type relationships. It does no runtime checks.

```cpp
// Listing 5.30: static_cast for standard numeric and pointer conversions
#include <iostream>

int main() {
    double gravity = 9.81;
    int g = static_cast<int>(gravity); // Truncates to 9
    std::cout << g << "\n";
    return 0;
}
```

Use it for: numeric type conversions, navigating up the inheritance hierarchy (upcasting), and intentional explicit truncation.

### 5.13.4 `dynamic_cast` — Safe Downcasting

`dynamic_cast` uses **Run-Time Type Information (RTTI)** to validate a downcast at runtime. For pointer targets it returns `nullptr` on failure; for reference targets it throws `std::bad_cast`.

```cpp
// Listing 5.31: dynamic_cast with null check
struct Animal { virtual ~Animal() {} };
struct Dog : Animal { void bark() {} };
struct Cat : Animal {};

int main() {
    Animal* a = new Cat();
    Dog* d = dynamic_cast<Dog*>(a);
    if (d != NULL) {
        d->bark();
    } else {
        std::cout << "Not a Dog.\n"; // Prints this
    }
    delete a;
    return 0;
}
```

`dynamic_cast` requires at least one virtual function in the base class (RTTI is only generated for polymorphic types). It carries runtime overhead; architectures that need it frequently signal a design problem.

### 5.13.5 `const_cast` — Stripping `const`

`const_cast` removes the `const` qualifier. Its sole legitimate use is adapting `const`-correct code to legacy APIs that forgot to declare their parameters `const`:

```cpp
// Listing 5.32: const_cast for legacy API compatibility
void legacy_function(int* p) { /* does not modify *p */ }

int main() {
    const int value = 42;
    legacy_function(const_cast<int*>(&value)); // OK if legacy_function doesn't write
    return 0;
}
```

If the underlying object is actually `const` and the legacy function writes through the stripped pointer, the result is **undefined behaviour**.

### 5.13.6 `reinterpret_cast` — Raw Bit Reinterpretation

`reinterpret_cast` tells the compiler to treat the raw bit pattern at one address as a completely different type. It performs no conversion. It is used for memory-mapped I/O, network packet parsing, and custom allocators.

```cpp
// Listing 5.33: reinterpret_cast for bit-level access
int original = 65;
char* c = reinterpret_cast<char*>(&original);
std::cout << *c << "\n"; // 'A' on little-endian systems (65 == 'A')
```

Use `reinterpret_cast` only when you understand the exact memory layout and the target platform's byte order.

---

## 5.14 Advanced Class Features

### 5.14.1 Pointers to Static Member Functions

A `static` member function has no `this` pointer, so its type is identical to a regular free function:

```cpp
// Listing 5.34: Pointer to static member function
typedef int Fn(int);

class Calc {
public:
    static int doubled(int i) { return 2 * i; }
};

int main() {
    Fn* fn = &Calc::doubled; // Same syntax as pointing to a free function
    std::cout << fn(4) << "\n"; // 8
    return 0;
}
```

### 5.14.2 Pointers to Non-Static Member Functions

Non-static member functions carry an implicit `this` argument, so their pointer type includes the class name. The operators `.*` and `->*` dereference member-function pointers through an instance:

```cpp
// Listing 5.35: Pointer to non-static member function
class Calc {
public:
    int doubled(int a) { return 2 * a; }
    int tripled(int b) { return 3 * b; }
};

int main() {
    Calc c;
    Calc* p = &c;

    int (Calc::*fn)(int) = &Calc::doubled;
    std::cout << (c.*fn)(5)  << "\n"; // 10 — via instance
    fn = &Calc::tripled;
    std::cout << (p->*fn)(5) << "\n"; // 15 — via pointer
    return 0;
}
```

### 5.14.3 Pointers to Member Variables

```cpp
// Listing 5.36: Pointer to data member
class Point {
public:
    int x, y;
};

int main() {
    Point pt;
    int Point::*mp = &Point::x;
    pt.*mp = 10;   // Sets pt.x to 10
    mp = &Point::y;
    pt.*mp = 20;   // Sets pt.y to 20
    return 0;
}
```

### 5.14.4 Nested Classes

A class may define another class inside itself. The nested class is a member of the enclosing class and can be used without qualification from within the enclosing scope. Member functions of the nested class are defined either inside the nested class or in the enclosing namespace — never inside the enclosing class body.

```cpp
// Listing 5.37: Nested class definition
struct Outer {
    struct Inner {
        void do_something();
    };
    Inner in;
};

void Outer::Inner::do_something() {
    std::cout << "Inner function\n";
}
```

### 5.14.5 Member Type Aliases

A class can define `typedef`s as members. This is the C++98/03 idiom for publishing the element type of a container:

```cpp
// Listing 5.38: Member typedef for type-alias publication
template <typename T>
class SimpleContainer {
public:
    typedef T         value_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef size_t    size_type;
};

// Caller can inspect the element type without knowing the template parameter:
template <typename C>
typename C::value_type front(C& container) {
    return container[0];
}
```

---

## 5.15 Professional Insights: Name Mangling, ADL, and Copy Semantics

### 5.15.1 Function Overloading and Name Mangling

The linker must distinguish between overloads of the same function name. C++ compilers accomplish this by **name mangling**: encoding the parameter types into the symbol name in the object file (e.g. `void func(int)` might become `_Z4funci`). The mangling scheme is implementation-defined; GCC and MSVC use different conventions, which is one reason object files from different compilers are generally not interchangeable.

`extern "C"` disables name mangling for a declaration, enabling C linkage:

```cpp
extern "C" void c_compatible_function(int x);
```

### 5.15.2 Copy Constructor vs. Assignment Operator

| Operation | When it fires | Target object state |
| :-------- | :------------ | :------------------ |
| Copy constructor `T a = b;` | Initialising a new object | Not yet alive |
| Assignment `a = b;` | Assigning to an existing object | Already alive — must release old resources |

The self-assignment check in `operator=` is non-optional for resource-owning classes: `a = a` must be a no-op.

### 5.15.3 Operator Overloading: Member vs. Non-Member

- Use **member functions** when the left operand is always your class (e.g. `+=`, `-=`, unary operators).
- Use **non-member (friend) functions** when the left operand may be a different type (e.g. `+`, `-`, `<<`). This enables symmetric conversions — `complex + 1.0` and `1.0 + complex` both work.
- You cannot create new operators, change precedence, change arity, or overload `::`, `.`, `.*`, or `?:`.

# Chapter 06: Polymorphism and Virtualization

> *One interface, many implementations — and the machinery that makes it work.*

Inheritance lets you express "is-a" relationships and reuse code. Polymorphism elevates that to runtime behaviour: a single base-class pointer can point to any derived object and call the correct implementation automatically. This chapter covers how C++98/03 implements polymorphism through virtual functions and vtables, the hazards that accompany it (the virtual-destructor trap, virtual calls in constructors, the diamond problem), the full taxonomy of inheritance types, `const` correctness in class hierarchies, alignment and padding in the object model, and the copy-and-swap idiom for exception-safe assignment.

---

## Table of Contents

- [6.1 Inheritance: The "Is-A" Relationship](#61-inheritance-the-is-a-relationship)
- [6.2 The `protected` Access Modifier](#62-the-protected-access-modifier)
- [6.3 Inheritance Specifiers: public, protected, private](#63-inheritance-specifiers-public-protected-private)
- [6.4 Virtual Functions and Dynamic Dispatch](#64-virtual-functions-and-dynamic-dispatch)
- [6.5 The vTable Mechanism](#65-the-vtable-mechanism)
- [6.6 Pure Virtual Functions and Abstract Classes](#66-pure-virtual-functions-and-abstract-classes)
- [6.7 The Virtual Destructor Trap](#67-the-virtual-destructor-trap)
- [6.8 Virtual Functions in Constructors and Destructors](#68-virtual-functions-in-constructors-and-destructors)
- [6.9 Multiple Inheritance and Thunks](#69-multiple-inheritance-and-thunks)
- [6.10 Virtual Inheritance and the Diamond Problem](#610-virtual-inheritance-and-the-diamond-problem)
- [6.11 Multilevel and Hierarchical Inheritance](#611-multilevel-and-hierarchical-inheritance)
- [6.12 Polymorphic Design: The Shape Pattern](#612-polymorphic-design-the-shape-pattern)
- [6.13 Safe Downcasting in Polymorphic Hierarchies](#613-safe-downcasting-in-polymorphic-hierarchies)
- [6.14 `const` Correctness in OOP](#614-const-correctness-in-oop)
- [6.15 Alignment, Padding, and the Cost of Polymorphism](#615-alignment-padding-and-the-cost-of-polymorphism)
- [6.16 The Copy-and-Swap Idiom](#616-the-copy-and-swap-idiom)
- [6.17 Professional Insights: Virtual Mechanics and Destructor Policy](#617-professional-insights-virtual-mechanics-and-destructor-policy)

---

## 6.1 Inheritance: The "Is-A" Relationship

**Inheritance** models an "is-a" relationship. A `Dog` *is an* `Animal`. A `Sword` *is a* `Weapon`. Inheritance lets a **derived class** acquire the members and methods of a **base class** without copying code.

```cpp
// Listing 6.1: Single public inheritance
#include <iostream>

// Base class
class Animal {
public:
    void eat() {
        std::cout << "Eating food...\n";
    }
};

// Derived class inherits from Animal
class Dog : public Animal {
public:
    void bark() {
        std::cout << "Woof!\n";
    }
};

int main() {
    Dog my_dog;
    my_dog.bark(); // Dog's own method
    my_dog.eat();  // Inherited from Animal
    return 0;
}
```

Because a `Dog` *is an* `Animal`, a `Dog*` or `Dog&` may be passed wherever an `Animal*` or `Animal&` is expected — this is the **Liskov Substitution Principle**:

```cpp
// Listing 6.2: Passing derived as base
void feed_animal(Animal* a) {
    a->eat();
}

int main() {
    Dog* d = new Dog();
    feed_animal(d); // Valid: Dog is-a Animal
    delete d;
    return 0;
}
```

---

## 6.2 The `protected` Access Modifier

`private` members of a base class are inaccessible to derived classes. `protected` members are **private to the outside world** but **fully accessible to derived classes**:

```cpp
// Listing 6.3: protected member shared with derived classes
class Animal {
protected:
    int weight; // Accessible to Animal and all derived classes
                // Hidden from code outside the hierarchy
};

class Dog : public Animal {
public:
    void grow() {
        weight += 5; // Allowed: Dog has access to protected members
    }
};
```

---

## 6.3 Inheritance Specifiers: public, protected, private

The keyword after `:` in a class definition controls what access level inherited members appear as in the derived class:

| Base member | `public` inheritance | `protected` inheritance | `private` inheritance |
| :---------- | :------------------- | :---------------------- | :-------------------- |
| `public` | `public` | `protected` | `private` |
| `protected` | `protected` | `protected` | `private` |
| `private` | inaccessible | inaccessible | inaccessible |

```cpp
// Listing 6.4: Access under the three inheritance modes
struct A {
public:    int p1;
protected: int p2;
private:   int p3;
};

struct B_pub : public A {
    void foo() { p1 = 0; p2 = 0; /* p3 = 0 ill-formed */ }
};

struct B_pri : private A {
    void foo() { p1 = 0; p2 = 0; /* both private in B_pri */ }
};
```

**Public inheritance** models "is-a". **Private inheritance** models "is-implemented-in-terms-of" and is similar in effect to containment. **Protected inheritance** is rarely used; it means derived classes see inherited members as `protected`.

---

## 6.4 Virtual Functions and Dynamic Dispatch

Without virtual functions, method calls on a base-class pointer are resolved at **compile time** (static dispatch) — always calling the base's version regardless of the actual object type:

```cpp
// Listing 6.5: Non-virtual — always calls base version
#include <iostream>

struct X { void f() { std::cout << "X::f()\n"; } };
struct Y : X { void f() { std::cout << "Y::f()\n"; } };

void call(X& a) { a.f(); }

int main() {
    X x; Y y;
    call(x); // X::f()
    call(y); // X::f() — Y::f() is NEVER called!
    return 0;
}
```

Marking the base function `virtual` switches to **dynamic dispatch**: the correct override is located at runtime from the actual object type:

```cpp
// Listing 6.6: virtual — correct override called at runtime
#include <iostream>

class Animal {
public:
    virtual void speak() {
        std::cout << "...\n";
    }
    virtual ~Animal() {}
};

class Dog : public Animal {
public:
    void speak() {    // 'override' (C++11) would confirm signature match
        std::cout << "Woof!\n";
    }
};

class Cat : public Animal {
public:
    void speak() {
        std::cout << "Meow!\n";
    }
};

int main() {
    Animal* pet = new Dog();
    pet->speak(); // Prints "Woof!" — resolved at runtime
    delete pet;
    return 0;
}
```

---

## 6.5 The vTable Mechanism

When a class contains at least one virtual function, the compiler:

1. Creates a **vtable** (virtual table) for the class — a static array of function pointers, one entry per virtual function.
2. Inserts a hidden **vptr** (virtual pointer) into every instance of that class, pointing to its class's vtable.

When `pet->speak()` is called, the CPU:
1. Dereferences `pet` to find the object.
2. Reads the vptr from the object.
3. Indexes into the vtable for `speak`.
4. Calls through that function pointer.

```cpp
// Listing 6.7: vtable overhead illustration
class Base {
    int data;
    virtual void func() {}
};
// On 64-bit: sizeof(Base) = sizeof(vptr=8) + sizeof(int=4) + padding = 16 bytes
// Without virtual: sizeof(Base) = sizeof(int=4) = 4 bytes
```

Each class in the hierarchy has its own vtable. Derived classes that override virtual functions have entries pointing to their overriding functions; entries for unoverridden functions still point to the base implementation.

**Performance note:** dynamic dispatch adds one pointer dereference per virtual call. In tight loops calling thousands of virtual functions per frame, cache misses on the vtable and the called function body can degrade throughput. This is a real concern in game engines and HFT hot paths.

---

## 6.6 Pure Virtual Functions and Abstract Classes

A **pure virtual function** (declared `= 0`) has no default implementation in the base class. Any class with at least one pure virtual function becomes an **abstract class** — it cannot be instantiated. Derived classes must override every pure virtual function to become concrete.

```cpp
// Listing 6.8: Abstract base class with pure virtual functions
#include <iostream>

class Weapon {
public:
    virtual void attack() = 0;   // Pure virtual
    virtual ~Weapon() {}
};

class Sword : public Weapon {
public:
    void attack() { std::cout << "Swing!\n"; }
};

int main() {
    // Weapon w;               // ERROR: cannot instantiate abstract class
    Weapon* w = new Sword();   // OK: Sword is concrete
    w->attack();               // "Swing!"
    delete w;
    return 0;
}
```

A pure virtual function **may** still have a body — defined outside the class. Derived classes can invoke it explicitly via `Base::func()`. This pattern is useful when sharing common implementation across all overrides:

```cpp
// Listing 6.9: Pure virtual with a body
struct DefaultAbstract {
    virtual void configure() = 0;
};

void DefaultAbstract::configure() {
    // Shared setup code all derived classes can call
}

struct Derived : DefaultAbstract {
    void configure() { DefaultAbstract::configure(); /* + specific */ }
};
```

**Interface idiom in C++98/03:** a class with *only* public pure virtual functions and a virtual destructor acts as a pure interface — no data members, no implementation, just a contract.

---

## 6.7 The Virtual Destructor Trap

This is one of the most famous bugs in C++:

```cpp
// Listing 6.10: Non-virtual destructor causes resource leak
class Base {
public:
    ~Base() { std::cout << "Base destroyed.\n"; }
};

class Derived : public Base {
    int* array;
public:
    Derived() { array = new int[100]; }
    ~Derived() { delete[] array; std::cout << "Derived destroyed.\n"; }
};

int main() {
    Base* b = new Derived();
    delete b; // Only prints "Base destroyed." — array is LEAKED!
    return 0;
}
```

Because `Base::~Base` is not `virtual`, `delete b` only calls the statically known destructor — `~Base`. The `~Derived` destructor never runs, and the heap array is permanently leaked.

**The Golden Rule:** if a class has even one `virtual` function, or is intended to be used as a base class, give it a `virtual` destructor:

```cpp
// Listing 6.11: Correct — virtual destructor
class Base {
public:
    virtual ~Base() { std::cout << "Base destroyed.\n"; }
};
```

Now `delete b` looks up the vtable, calls `~Derived` first, which calls `~Base` automatically when the derived part is cleaned up.

**Alternative:** if you want to prevent destruction through a base pointer, make the destructor `protected` (non-virtual). This disables `delete base_ptr` at compile time:

```cpp
// Listing 6.12: Protected destructor to block base-pointer deletion
struct NoDeletionViaBase {
protected:
    ~NoDeletionViaBase() {}
};
```

---

## 6.8 Virtual Functions in Constructors and Destructors

Never call virtual functions during construction or destruction:

```cpp
// Listing 6.13: Virtual call during construction — does not dispatch to derived
#include <iostream>
using namespace std;

class Base {
public:
    Base() { f("base constructor"); }
    ~Base() { f("base destructor"); }

    virtual const char* v() { return "Base::v()"; }

    void f(const char* caller) {
        cout << "From " << caller << ": " << v() << "\n";
    }
};

class Derived : public Base {
public:
    Derived() { f("derived constructor"); }
    ~Derived() { f("derived destructor"); }
    const char* v() { return "Derived::v()"; }
};

int main() {
    Derived d;
    return 0;
}
/* Output:
   From base constructor: Base::v()        <-- NOT Derived::v()!
   From derived constructor: Derived::v()
   From derived destructor: Derived::v()
   From base destructor: Base::v()         <-- NOT Derived::v()! */
```

During `Base::Base()`, the `Derived` subobject has not yet been constructed — calling `Derived::v()` would access uninitialised members. C++ therefore treats the dynamic type of `*this` as `Base` during `Base`'s constructor, and as `Derived` only after the `Derived` constructor body begins.

---

## 6.9 Multiple Inheritance and Thunks

C++ permits a class to inherit from more than one base:

```cpp
// Listing 6.14: Multiple inheritance and pointer adjustment
class A { int a; virtual void f() {} };
class B { int b; virtual void g() {} };

class C : public A, public B { int c; };

int main() {
    C obj;
    A* pa = &obj; // Points to start of obj (A subobject at offset 0)
    B* pb = &obj; // Points to obj + sizeof(A) — B subobject at a non-zero offset!
    return 0;
}
```

When a virtual function from `B` is called through `pb`, the compiler must adjust `this` to point to the correct subobject. It generates a small code fragment called a **thunk** to perform this pointer arithmetic before entering the actual function body.

---

## 6.10 Virtual Inheritance and the Diamond Problem

When two base classes share a common ancestor, the naive layout duplicates the ancestor:

```cpp
// Listing 6.15: Diamond problem without virtual inheritance
struct Top { int t; };
struct Left  : public Top { int l; };
struct Right : public Top { int r; };
struct Bottom : public Left, public Right { int b; };

void f() {
    Bottom obj;
    // obj.t;          // ERROR: ambiguous — is it Left::Top::t or Right::Top::t?
    obj.Left::t = 1;   // Must qualify
    obj.Right::t = 2;  // Two separate copies of 't'
}
```

`virtual` inheritance ensures a single shared instance of the common ancestor:

```cpp
// Listing 6.16: Virtual inheritance solving the diamond
struct Top  { int t; };
struct Left  : virtual public Top { int l; };
struct Right : virtual public Top { int r; };
struct Bottom : public Left, public Right { int b; };

void f() {
    Bottom obj;
    obj.t = 42; // Unambiguous: only one copy of Top
}
```

**Cost:** `Left` and `Right` now contain a hidden **vbptr** (virtual base pointer) pointing to the shared `Top` subobject. Accessing members of `Top` through `Left` or `Right` requires an extra indirection, making it slower than ordinary inheritance. The most-derived class (`Bottom`) becomes responsible for constructing `Top` directly.

---

## 6.11 Multilevel and Hierarchical Inheritance

### 6.11.1 Multilevel Inheritance

A chain of inheritance where each class derives from the previous:

```cpp
// Listing 6.17: Three-level inheritance chain
#include <iostream>
#include <string>
using namespace std;

class Vehicle {
protected:
    string brand;
public:
    Vehicle(const string& b) : brand(b) {}
    virtual void start() { cout << brand << " is starting\n"; }
    virtual ~Vehicle() {}
};

class Car : public Vehicle {
protected:
    int numDoors;
public:
    Car(const string& b, int doors) : Vehicle(b), numDoors(doors) {}
    void start() {
        cout << brand << " car (" << numDoors << " doors) starting\n";
    }
    virtual ~Car() {}
};

class ElectricCar : public Car {
    int batteryPct;
public:
    ElectricCar(const string& b, int doors, int bat)
        : Car(b, doors), batteryPct(bat) {}
    void start() {
        cout << "Electric " << brand << " (battery: "
             << batteryPct << "%) starting\n";
    }
};

int main() {
    ElectricCar tesla("Tesla", 4, 95);
    tesla.start(); // Calls ElectricCar::start
    return 0;
}
```

### 6.11.2 Hierarchical Inheritance

Multiple derived classes sharing a single base class — a common pattern for implementing a family of related types:

```cpp
// Listing 6.18: Hierarchical inheritance — three employee types
#include <iostream>
#include <string>
using namespace std;

class Employee {
protected:
    string name;
    double salary;
public:
    Employee(const string& n, double s) : name(n), salary(s) {}
    virtual void work() = 0;
    virtual ~Employee() {}
};

class Manager : public Employee {
public:
    Manager(const string& n, double s) : Employee(n, s) {}
    void work() { cout << name << " is managing the team\n"; }
};

class Developer : public Employee {
    string language;
public:
    Developer(const string& n, double s, const string& lang)
        : Employee(n, s), language(lang) {}
    void work() { cout << name << " is coding in " << language << "\n"; }
};

class Designer : public Employee {
public:
    Designer(const string& n, double s) : Employee(n, s) {}
    void work() { cout << name << " is designing UI/UX\n"; }
};

int main() {
    Manager   alice("Alice", 80000);
    Developer bob("Bob", 70000, "C++");
    Designer  carol("Carol", 65000);

    alice.work();
    bob.work();
    carol.work();
    return 0;
}
```

---

## 6.12 Polymorphic Design: The Shape Pattern

The canonical illustration of polymorphism — a collection of different shape types manipulated uniformly through a base-class interface:

```cpp
// Listing 6.19: Polymorphic Shape hierarchy
#include <iostream>
#include <string>
using namespace std;

class Shape {
public:
    virtual void draw()   = 0;  // Pure virtual
    virtual double area() = 0;  // Pure virtual
    virtual string name() = 0;  // Pure virtual
    virtual ~Shape() {}
};

class Circle : public Shape {
    double radius;
public:
    explicit Circle(double r) : radius(r) {}
    void   draw()   { cout << "Drawing Circle\n"; }
    double area()   { return 3.14159 * radius * radius; }
    string name()   { return "Circle"; }
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h) : w(w), h(h) {}
    void   draw()   { cout << "Drawing Rectangle\n"; }
    double area()   { return w * h; }
    string name()   { return "Rectangle"; }
};

int main() {
    Shape* shapes[2];
    shapes[0] = new Circle(5.0);
    shapes[1] = new Rectangle(4.0, 6.0);

    for (int i = 0; i < 2; ++i) {
        shapes[i]->draw();
        cout << "Area of " << shapes[i]->name()
             << ": " << shapes[i]->area() << "\n";
        delete shapes[i];
    }
    return 0;
}
```

---

## 6.13 Safe Downcasting in Polymorphic Hierarchies

Upcasting (derived → base) is always implicit and safe. Downcasting (base → derived) requires explicit intent. Use `dynamic_cast` for safety:

```cpp
// Listing 6.20: Safe downcast with dynamic_cast
#include <iostream>
using namespace std;

class Shape {
public:
    virtual double area() const = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
    double radius;
public:
    explicit Circle(double r) : radius(r) {}
    double area() const { return 3.14159 * radius * radius; }
    double diameter() const { return 2.0 * radius; }
};

int main() {
    Shape* s = new Circle(5.0);
    cout << "Area: " << s->area() << "\n";

    // Attempt safe downcast to Circle
    Circle* c = dynamic_cast<Circle*>(s);
    if (c != NULL) {
        cout << "Diameter: " << c->diameter() << "\n";
    } else {
        cout << "Not a Circle.\n";
    }

    delete s;
    return 0;
}
```

`dynamic_cast` requires the base class to be polymorphic (at least one virtual function). `static_cast` performs no runtime check and should only be used for downcasts when you are certain of the actual type.

---

## 6.14 `const` Correctness in OOP

**`const` member functions** promise not to modify any non-`mutable` member of the object. They are callable on `const` instances and `const` references. Non-`const` member functions are only callable on non-`const` instances.

```cpp
// Listing 6.21: const member functions and mutable members
#include <iostream>
#include <string>
using namespace std;

class Person {
    mutable int accessCount; // Modifiable even in const contexts
    string name;
    int age;

public:
    Person(const string& n, int a) : name(n), age(a), accessCount(0) {}

    string getName() const {
        ++accessCount; // OK: accessCount is mutable
        return name;
    }

    const string& getNameRef() const {
        ++accessCount;
        return name;
    }

    void setAge(int a) { age = a; } // Non-const: may modify

    int getAge() const { return age; }

    int getAccessCount() const { return accessCount; }
};

int main() {
    Person p("Alice", 30);
    p.setAge(31);

    const Person cp("Bob", 25);
    cout << cp.getName() << "\n";    // OK: const method
    cout << cp.getAge()  << "\n";    // OK: const method
    // cp.setAge(26);               // ERROR: non-const method on const object

    return 0;
}
```

**`mutable`** permits a member to be modified even inside a `const` member function. Legitimate uses: caching, lazy initialisation, and logging access counts where the observable state of the object does not change.

---

## 6.15 Alignment, Padding, and the Cost of Polymorphism

CPUs read data most efficiently at addresses that are multiples of the data's size. The compiler inserts **padding** bytes to satisfy these **alignment** requirements.

**Rule:** a member of size *N* must reside at an offset divisible by *N*.

```cpp
// Listing 6.22: Struct padding example
struct Mixed {
    char  a;   // 1 byte at offset 0
               // 3 bytes padding
    int   b;   // 4 bytes at offset 4
    short c;   // 2 bytes at offset 8
               // 6 bytes padding (to align struct size to 8)
};
// sizeof(Mixed) = 16 on a 64-bit platform
```

**Optimisation:** sort members largest-to-smallest to minimise padding:

```cpp
// Listing 6.23: Padding-minimised layout
struct Optimised {
    int   b;   // 4 bytes at offset 0
    short c;   // 2 bytes at offset 4
    char  a;   // 1 byte at offset 6
               // 1 byte padding
};
// sizeof(Optimised) = 8
```

**vptr overhead:** every class with at least one virtual function includes a hidden vptr. On 64-bit systems the vptr occupies 8 bytes and may add alignment padding:

```cpp
// Listing 6.24: vptr adds size to the object
class WithVirtual {
    int data;
    virtual void f() {}
};
// sizeof(WithVirtual) = 16 (vptr=8, int=4, padding=4) on 64-bit
```

---

## 6.16 The Copy-and-Swap Idiom

The naive implementation of copy assignment has two problems: it does not satisfy the **strong exception guarantee** (if `new` throws, the object is left in an invalid state), and it contains duplicated logic from the copy constructor. The **copy-and-swap idiom** solves both:

```cpp
// Listing 6.25: Copy-and-swap for exception-safe assignment
#include <cstring>
#include <algorithm>

class Person {
    char* name;
    int   age;

public:
    Person(const char* n, int a)
        : name(new char[std::strlen(n) + 1]), age(a) {
        std::strcpy(name, n);
    }

    ~Person() { delete[] name; }

    // Copy constructor (may throw, but object is not yet alive)
    Person(const Person& other)
        : name(new char[std::strlen(other.name) + 1]), age(other.age) {
        std::strcpy(name, other.name);
    }

    // Swap: exchange all resources
    friend void swap(Person& lhs, Person& rhs) {
        std::swap(lhs.name, rhs.name);
        std::swap(lhs.age,  rhs.age);
    }

    // Assignment: copy into a temporary, then swap
    // If copy-construction throws, *this is untouched
    Person& operator=(Person rhs) { // rhs is a copy (copied before entry)
        swap(*this, rhs);
        return *this;
    }   // rhs (the old state) is destroyed here
};
```

When `a = b` executes:
1. `rhs` is copy-constructed from `b`. If `new` throws, the assignment operator body is never entered and `a` is unchanged.
2. `swap(*this, rhs)` exchanges resources in two pointer swaps — cannot throw.
3. The old resources of `a` are destroyed when `rhs` leaves scope.

Self-assignment (`a = a`) is handled correctly but less efficiently than a manual check; for typical use cases this trade-off is acceptable.

---

## 6.17 Professional Insights: Virtual Mechanics and Destructor Policy

### 6.17.1 Virtual Destructor Policy

| Scenario | Required destructor |
| :-------- | :------------------ |
| Class designed to be a polymorphic base | `virtual ~Base()` |
| Class designed to prevent base-pointer deletion | `protected ~Base()` (non-virtual) |
| Concrete leaf class, not used polymorphically | Non-virtual destructor (implicit or explicit) |
| Abstract interface (pure virtual only) | `virtual ~Interface() = 0` with out-of-line definition |

### 6.17.2 `override` and `final` — Forward Reference: C++11

C++11 introduced two contextual keywords:
- `override` — instructs the compiler to verify the function actually overrides a base virtual function. A signature mismatch becomes a compile error instead of a silent new overload.
- `final` — prevents further overriding of a virtual function, or prevents subclassing of an entire class.

In C++98/03, a mismatched signature silently creates a new overload instead of overriding, a notoriously hard-to-find bug.

### 6.17.3 Never Call Virtual Functions in Constructors or Destructors

During a base class constructor, the vtable entry for the object points to the **base** class's implementation, not the derived class's. Calling a virtual function will invoke the base version — not the override. The same applies during destruction in reverse order. Design your constructors to not rely on virtual dispatch.

### 6.17.4 The Liskov Substitution Principle

Public inheritance is correctly used only when an instance of the derived class can be substituted for an instance of the base class in every context without breaking program correctness. Private inheritance ("is-implemented-in-terms-of") should be used when you want the implementation but not the interface of the base class.

# Chapter 06: OOP & Encapsulation

# OBJECT-ORIENTED PROGRAMMING: ENCAPSULATION & DESIGN

Welcome to the world of objects. In the previous chapters, we were writing "Procedural" code—essentially a long list of instructions for the computer to follow. Now, we’re going to start thinking about **things**.

### The Blueprint vs. The House

Think of a **Class** as a **Blueprint** for a house. 
*   The blueprint isn't a house. You can't live in it, and it doesn't take up any space in Mem-City. 
*   It just describes *what* a house should have (windows, doors, rooms) and *what* it can do (open doors, turn on lights).

An **Object** is the actual **House** built from that blueprint. 
*   You can build 1,000 houses from a single blueprint. 
*   Each house has its own address in Mem-City, and each house can have different colored walls (data).

***

### Encapsulation: The Smart TV Analogy

Why do we make data `private`? 

Imagine your Smart TV. It has a lot of complex wiring and circuit boards inside. If the manufacturer left all those wires exposed, you might accidentally pull one out or touch a high-voltage capacitor. 

Instead, they **Encapsulate** the TV. They put all the dangerous, complex stuff inside a plastic shell and give you a **Remote Control** (the `public` functions).

1.  **Private**: The circuit boards and wires. Only the TV itself (the class) can touch these.
2.  **Public**: The Power button, Volume Up, and Netflix button. These are the only things the user (the caller) is allowed to touch.

> **There are no dumb questions...**
>
> **Q: If I want to change the volume, why can't I just go inside and move the volume wire manually?**
> **A:** Because if the manufacturer changes how the volume works (replaces a wire with a chip), your "manual" way will break the TV. If you use the remote control, you don't care how it works inside. This is called **Decoupling**.
>
> **Q: Is a `struct` just a `class` with everything public?**
> **A:** Almost exactly! In C++, the only technical difference is that `struct` members are public by default, while `class` members are private by default. By convention, we use `struct` for simple data containers and `class` for objects with complex behavior.

***

# OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS (C++98)

## CLASSES & OBJECTS

### 1.1 Basic Class Structure

A class is a blueprint for objects. It encapsulates data and behavior.

```cpp
#include <iostream>
#include <string>

class Person {
public:      // Access modifier: Public
    std::string name;
    int age;

    // Member function (Method)
    void introduce() {
        std::cout << "I am " << name << ", age " << age << "\n";
    }
};

int main() {
    Person p;
    p.name = "Alice";
    p.age = 30;
    p.introduce();
    return 0;
}
```

***
### Professional Insights: Overloading Mastery

#### 1. Function Overloading and Name Mangling

Function overloading allows multiple functions with the same name but different parameter lists.
*   **Resolution**: The compiler chooses the best match based on argument types.
*   **Name Mangling**: C++ compilers encode parameter types into the function's name in the object file (e.g., `void func(int)` might become `_Z4funci`). This allows the linker to distinguish between overloads.
*   **`extern "C"`**: Disables name mangling for a function, allowing it to be called from C code.

#### 2. Operator Overloading: Giving Syntax to Objects

Operator overloading allows custom types to behave like built-in types.
*   **Member vs. Non-member**: Overload operators like `+`, `-` as non-member friend functions to allow symmetric conversions (e.g., `complex + 1.0` and `1.0 + complex`).
*   **Rules**: You cannot create new operators, change precedence, or overload operators for built-in types only.

```cpp
class Complex {
    double r, i;
public:
    Complex(double r, double i) : r(r), i(i) {}
    // Overloading + as member
    Complex operator+(const Complex& other) const {
        return Complex(r + other.r, i + other.i);
    }
    // Overloading << for output
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        os << "(" << c.r << ", " << c.i << "i)";
        return os;
    }
};
```

#### 3. Copying vs. Assignment

*   **Copy Constructor**: Initializes a *new* object from an existing one (`T a = b;`).
*   **Assignment Operator**: Modifies an *existing* object from another existing one (`a = b;`).
*   **Self-Assignment**: Always check for `if (this == &other)` in `operator=` to prevent deleting your own data before copying.

***

### 1.2 Access Modifiers & Encapsulation

Encapsulation hides internal state to prevent invalid access.

*   `public`: Accessible from anywhere.
*   `private`: Accessible only from within the class.
*   `protected`: Accessible from the class and its derived classes.

```cpp
class BankAccount {
private:
    double balance; // Hidden data

public:
    void deposit(double amount) {
        if (amount > 0) balance += amount;
    }

    double get_balance() const { // Read-only access
        return balance;
    }
};
```

### 1.3 Constructors & Destructors

Constructors initialize objects. Destructors clean up resources.

```cpp
class Car {
    std::string brand;
    int* buffer;

public:
    // Default Constructor
    Car() : brand("Unknown"), buffer(new int[10]) {
        std::cout << "Default Constructor\n";
    }

    // Parameterized Constructor
    Car(const std::string& b) : brand(b), buffer(new int[10]) {
        std::cout << "Param Constructor\n";
    }

    // Copy Constructor (Deep Copy)
    Car(const Car& other) : brand(other.brand) {
        buffer = new int[10];
        // memcpy or loop to copy buffer content
        std::cout << "Copy Constructor\n";
    }

    // Destructor
    ~Car() {
        delete[] buffer; // Cleanup
        std::cout << "Destructor\n";
    }
};
```

### 1.4 The Rule of Three (C++98)

If you explicitly define one of the following, you likely need all three to manage resources correctly:
1.  **Destructor**: To free resources (e.g., `delete` memory).
2.  **Copy Constructor**: To perform deep copies.
3.  **Copy Assignment Operator**: To handle assignment (`a = b`).

```cpp
class Buffer {
    int* ptr;
public:
    Buffer() { ptr = new int[10]; }
    ~Buffer() { delete[] ptr; }

    // Copy Constructor
    Buffer(const Buffer& other) {
        ptr = new int[10];
        // copy data...
    }

    // Assignment Operator
    Buffer& operator=(const Buffer& other) {
        if (this != &other) { // Self-assignment check
            delete[] ptr;     // Free old
            ptr = new int[10]; // Allocate new
            // copy data...
        }
        return *this;
    }
};
```

***

## INHERITANCE & POLYMORPHISM

### 2.1 Inheritance Basics

Inheritance allows a class to derive features from another.

```cpp
class Animal {
public:
    void eat() { std::cout << "Eating...\n"; }
};

class Dog : public Animal { // Dog inherits from Animal
public:
    void bark() { std::cout << "Woof!\n"; }
};
```

### 2.2 Virtual Functions (Polymorphism)

Polymorphism allows objects to be treated as instances of their base class, but behave like their actual derived class.

```cpp
class Shape {
public:
    // Virtual function: Can be overridden
    virtual void draw() { std::cout << "Drawing Shape\n"; }

    // Virtual Destructor (Crucial for inheritance)
    virtual ~Shape() {}
};

class Circle : public Shape {
public:
    void draw() { std::cout << "Drawing Circle\n"; } // Override
};

int main() {
    Shape* s = new Circle();
    s->draw(); // Prints "Drawing Circle" (Dynamic Dispatch)
    delete s;  // Properly calls ~Circle then ~Shape
    return 0;
}
```

### 2.3 Pure Virtual Functions (Abstract Classes)

A pure virtual function (`= 0`) makes a class **Abstract**. It cannot be instantiated and must be subclassed.

```cpp
class Interface {
public:
    virtual void execute() = 0; // Pure virtual
    virtual ~Interface() {}
};

class Concrete : public Interface {
public:
    void execute() { std::cout << "Executed.\n"; }
};
```

***

## ADVANCED CLASS FEATURES

### 3.1 Static Members

Shared by all instances of the class.

```cpp
class User {
public:
    static int userCount; // Declaration
    User() { userCount++; }
};

// Definition (Must be outside class)
int User::userCount = 0;
```

### 3.2 Friend Classes/Functions

Friends can access private members.

```cpp
class Box {
    int width;
public:
    Box(int w) : width(w) {}
    friend void printWidth(Box& b);
};

void printWidth(Box& b) {
    // Can access private 'width'
    std::cout << b.width << "\n";
}
```

***

## PART 2: GENERIC PROGRAMMING (TEMPLATES)

Templates are the foundation of Generic Programming in C++. They allow writing code that works with any data type. This is how the STL is implemented.

### 4.1 Function Templates

A function template defines a family of functions.

```cpp
// Template declaration
template <typename T>
T myMax(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    std::cout << myMax(10, 20) << "\n";       // T is int
    std::cout << myMax(3.14, 2.71) << "\n";   // T is double
    // std::cout << myMax(10, 3.14) << "\n";  // Error: Mismatched types
    return 0;
}
```

### 4.2 Class Templates

Classes can also be templated to hold data of any type.

```cpp
template <typename T>
class Box {
private:
    T content;
public:
    Box(T val) : content(val) {}

    T getContent() const { return content; }
    void setContent(T val) { content = val; }
};

int main() {
    Box<int> intBox(123);
    Box<std::string> strBox("Hello");

    std::cout << intBox.getContent() << "\n";
    return 0;
}
```

### 4.3 Multiple Template Parameters

Templates can accept multiple types.

```cpp
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

### 4.4 Non-Type Template Parameters

Templates can take values (integers, pointers) as parameters, not just types.

```cpp
// N is a compile-time constant
template <typename T, int N>
class Array {
    T data[N];
public:
    int size() const { return N; }
};

int main() {
    Array<int, 5> arr; // Fixed size array of 5 ints
    // Array<int, 10> arr2 = arr; // Error: Different types
    return 0;
}
```

### 4.5 Template Specialization

You can define specific implementations for specific types.

#### Full Specialization

```cpp
template <typename T>
class Formatter {
public:
    void format(T val) { std::cout << "General: " << val << "\n"; }
};

// Specialized for bool
template <>
class Formatter<bool> {
public:
    void format(bool val) {
        std::cout << "Boolean: " << (val ? "true" : "false") << "\n";
    }
};
```

#### Partial Specialization (Class Templates Only)

```cpp
template <typename T1, typename T2>
class MyMap { /* ... */ };

// Partial specialization for when both types are the same
template <typename T>
class MyMap<T, T> { /* Optimized implementation */ };
```

### 4.6 The `typename` Keyword

When a type depends on a template parameter (dependent type), you must use `typename`.

```cpp
template <typename T>
void func() {
    // T::iterator *iter; // Ambiguous: Is iterator a type or a static member?

    typename T::iterator *iter; // Correct: Tells compiler iterator is a type
}
```

### 4.7 Templates vs Macros

Templates are type-safe and processed by the compiler. Macros are text substitution processed by the preprocessor. Always prefer templates.

**Summary:**
- **Specialization** allows handling specific types differently.

<!-- Merged content from Chapter_14_NAMESPACES.md -->

# NAMESPACES

## 13.1 Namespace Basics

```cpp
#include <iostream>

using namespace std;

// Define namespace
namespace Math {
    double PI = 3.14159;

    double circle_area(double radius) {
        return PI * radius * radius;
    }
}

namespace Graphics {
    double PI = 3.14;  // Different PI

    void draw_circle(double radius) {
        cout << "Drawing circle with radius: " << radius << endl;
    }
}

int main() {
    // Access with namespace::name
    cout << Math::PI << endl;
    cout << Graphics::PI << endl;

    cout << Math::circle_area(5) << endl;
    Graphics::draw_circle(5);

    return 0;
}
```

## 13.2 Namespace Aliases

```cpp
#include <iostream>

using namespace std;

namespace Very {
    namespace Long {
        namespace Namespace {
            void function() {
                cout << "Long namespace function" << endl;
            }
        }
    }
}

int main() {
    // Use alias to shorten
    namespace VLN = Very::Long::Namespace;

    VLN::function();

    // or use using
    using namespace Very::Long::Namespace;
    function();

    return 0;
}
```

***

## Professional Insights: Friend keyword

Well-designed classes encapsulate their functionality, hiding their implementation while providing a clean,
documented interface. This allows redesign or change so long as the interface is unchanged.
In a more complex scenario, multiple classes that rely on each others' implementation details may be required.
Friend classes and functions allow these peers access to each others' details, without compromising the
encapsulation and information hiding of the documented interface.
Section 19.1: Friend function
A class or a structure may declare any function it's friend. If a function is a friend of a class, it may access all it's
protected and private members:
// Forward declaration of functions.
void friend_function();
void non_friend_function();
```cpp
class PrivateHolder {
public:
    PrivateHolder(int val) : private_value(val) {}
private:
    int private_value;
    // Declare one of the function as a friend.
    friend void friend_function();
};
void non_friend_function() {
    PrivateHolder ph(10);
    // Compilation error: private_value is private.
    std::cout << ph.private_value << std::endl;
}
void friend_function() {
    // OK: friends may access private values.
    PrivateHolder ph(10);
    std::cout << ph.private_value << std::endl;
}
```

Access modiﬁers do not alter friend semantics. Public, protected and private declarations of a friend are equivalent.
Friend declarations are not inherited. For example, if we subclass PrivateHolder:
```cpp
class PrivateHolderDerived : public PrivateHolder {
public:
    PrivateHolderDerived(int val) : PrivateHolder(val) {}
private:
    int derived_private_value = 0;
};
and try to access it's members, we'll get the following:
void friend_function() {
    PrivateHolderDerived pd(20);
    // OK.
    std::cout << pd.private_value << std::endl;
    // Compilation error: derived_private_value is private.

    std::cout << pd.derived_private_value << std::endl;
}
Note that PrivateHolderDerived member function cannot access PrivateHolder::private_value, while friend
function can do it.
Section 19.2: Friend method
Methods may declared as friends as well as functions:
class Accesser {
public:
    void private_accesser();
};
class PrivateHolder {
public:
    PrivateHolder(int val) : private_value(val) {}
    friend void Accesser::private_accesser();
private:
    int private_value;
};
void Accesser::private_accesser() {
    PrivateHolder ph(10);
    // OK: this method is declares as friend.
    std::cout << ph.private_value << std::endl;
}
Section 19.3: Friend class
A whole class may be declared as friend. Friend class declaration means that any member of the friend may access
private and protected members of the declaring class:
class Accesser {
public:
    void private_accesser1();
    void private_accesser2();
};
class PrivateHolder {
public:
    PrivateHolder(int val) : private_value(val) {}
    friend class Accesser;
private:
    int private_value;
};
void Accesser::private_accesser1() {
    PrivateHolder ph(10);
    // OK.
    std::cout << ph.private_value << std::endl;
}
void Accesser::private_accesser2() {
    PrivateHolder ph(10);
    // OK.
    std::cout << ph.private_value + 1 << std::endl;

}
Friend class declaration is not reﬂexive. If classes need private access in both directions, both of them need friend
declarations.
class Accesser {
public:
    void private_accesser1();
    void private_accesser2();
private:
    int private_value = 0;
};
class PrivateHolder {
public:
    PrivateHolder(int val) : private_value(val) {}
    // Accesser is a friend of PrivateHolder
    friend class Accesser;
    void reverse_accesse() {
        // but PrivateHolder cannot access Accesser's members.
        Accesser a;
        std::cout << a.private_value;
    }
private:
    int private_value;
};
```


## Professional Insights: Pointers to members

Section 31.1: Pointers to static member functions
A static member function is just like an ordinary C/C++ function, except with scope:
It is inside a class, so it needs its name decorated with the class name;
It has accessibility, with public, protected or private.
So, if you have access to the static member function and decorate it correctly, then you can point to the function
like any normal function outside a class:
typedef int Fn(int); // Fn is a type-of function that accepts an int and returns an int
// Note that MyFn() is of type 'Fn'
```cpp
int MyFn(int i) { return 2*i; }
class Class {
public:
    // Note that Static() is of type 'Fn'
    static int Static(int i) { return 3*i; }
}; // Class
int main() {
    Fn *fn;    // fn is a pointer to a type-of Fn
    fn = &MyFn;          // Point to one function
    fn(3);               // Call it
    fn = &Class::Static; // Point to the other function
    fn(4);               // Call it
 } // main()
Section 31.2: Pointers to member functions
To access a member function of a class, you need to have a "handle" to the particular instance, as either the
instance itself, or a pointer or reference to it. Given a class instance, you can point to various of its members with a
pointer-to-member, IF you get the syntax correct! Of course, the pointer has to be declared to be of the same type
as what you are pointing to...
typedef int Fn(int); // Fn is a type-of function that accepts an int and returns an int
class Class {
public:
    // Note that A() is of type 'Fn'
    int A(int a) { return 2*a; }
    // Note that B() is of type 'Fn'
    int B(int b) { return 3*b; }
}; // Class
int main() {
    Class c;          // Need a Class instance to play with
    Class *p = &c;    // Need a Class pointer to play with
    Fn Class::*fn;    // fn is a pointer to a type-of Fn within Class
    fn = &Class::A;   // fn now points to A within any Class
    (c.*fn)(5);       // Pass 5 to c's function A (via fn)

    fn = &Class::B;   // fn now points to B within any Class
    (p->*fn)(6);      // Pass 6 to c's (via p) function B (via fn)
} // main()
Unlike pointers to member variables (in the previous example), the association between the class instance and the
member pointer need to be bound tightly together with parentheses, which looks a little strange (as though the .*
and ->* aren't strange enough!)
Section 31.3: Pointers to member variables
To access a member of a class, you need to have a "handle" to the particular instance, as either the instance itself,
or a pointer or reference to it. Given a class instance, you can point to various of its members with a pointer-to-
member, IF you get the syntax correct! Of course, the pointer has to be declared to be of the same type as what you
are pointing to...
class Class {
public:
    int x, y, z;
    char m, n, o;
}; // Class
int x;  // Global variable
int main() {
    Class c;        // Need a Class instance to play with
    Class *p = &c;  // Need a Class pointer to play with
    int *p_i;       // Pointer to an int
    p_i = &x;       // Now pointing to x
    p_i = &c.x;     // Now pointing to c's x
    int Class::*p_C_i; // Pointer to an int within Class
    p_C_i = &Class::x; // Point to x within any Class
    int i = c.*p_C_i;  // Use p_c_i to fetch x from c's instance
    p_C_i = &Class::y; // Point to y within any Class
    i = c.*p_C_i;      // Use p_c_i to fetch y from c's instance
    p_C_i = &Class::m; // ERROR! m is a char, not an int!
    char Class::*p_C_c = &Class::m; // That's better...
} // main()
The syntax of pointer-to-member requires some extra syntactic elements:
To deﬁne the type of the pointer, you need to mention the base type, as well as the fact that it is inside a
class: int Class::*ptr;.
If you have a class or reference and want to use it with a pointer-to-member, you need to use the .* operator
(akin to the . operator).
If you have a pointer to a class and want to use it with a pointer-to-member, you need to use the ->*
operator (akin to the -> operator).
Section 31.4: Pointers to static member variables
A static member variable is just like an ordinary C/C++ variable, except with scope:

It is inside a class, so it needs its name decorated with the class name;
```

It has accessibility, with public, protected or private.
So, if you have access to the static member variable and decorate it correctly, then you can point to the variable
like any normal variable outside a class:
```cpp
class Class {
public:
    static int i;
}; // Class
int Class::i = 1; // Define the value of i (and where it's stored!)
int j = 2;   // Just another global variable
int main() {
    int k = 3; // Local variable
    int *p;
    p = &k;   // Point to k
    *p = 2;   // Modify it
    p = &j;   // Point to j
    *p = 3;   // Modify it
    p = &Class::i; // Point to Class::i
    *p = 4;   // Modify it
} // main()
```


## Professional Insights: Classes/Structures

Section 34.1: Class basics
A class is a user-deﬁned type. A class is introduced with the class, struct or union keyword. In colloquial usage, the
term "class" usually refers only to non-union classes.
A class is a collection of class members, which can be:
member variables (also called "ﬁelds"),
member functions (also called "methods"),
member types or typedefs (e.g. "nested classes"),
member templates (of any kind: variable, function, class or alias template)
The class and struct keywords, called class keys, are largely interchangeable, except that the default access
speciﬁer for members and bases is "private" for a class declared with the class key and "public" for a class declared
with the struct or union key (cf. Access modiﬁers).
For example, the following code snippets are identical:
```cpp
struct Vector
{
    int x;
    int y;
    int z;
};
// are equivalent to
class Vector
{
public:
    int x;
    int y;
    int z;
};
By declaring a class` a new type is added to your program, and it is possible to instantiate objects of that class by
Vector my_vector;
```

Members of a class are accessed using dot-syntax.
my_vector.x = 10;
my_vector.y = my_vector.x + 1; // my_vector.y = 11;
my_vector.z = my_vector.y - 4; // my:vector.z = 7;
Section 34.2: Final classes and structs
Version ≥ C++11
Deriving a class may be forbidden with final speciﬁer. Let's declare a ﬁnal class:
class A final {
};
Now any attempt to subclass it will cause a compilation error:

// Compilation error: cannot derive from final class:
```cpp
class B : public A {
};
Final class may appear anywhere in class hierarchy:
class A {
};
// OK.
class B final : public A {
};
// Compilation error: cannot derive from final class B.
class C : public B {
};
Section 34.3: Access speciﬁers
There are three keywords that act as access speciﬁers. These limit the access to class members following the
speciﬁer, until another speciﬁer changes the access level again:
Keyword
public
Everyone has access
Description
protected Only the class itself, derived classes and friends have access
private Only the class itself and friends have access
When the type is deﬁned using the class keyword, the default access speciﬁer is private, but if the type is deﬁned
using the struct keyword, the default access speciﬁer is public:
struct MyStruct { int x; };
class MyClass { int x; };
MyStruct s;
s.x = 9; // well formed, because x is public
MyClass c;
c.x = 9; // ill-formed, because x is private
Access speciﬁers are mostly used to limit access to internal ﬁelds and methods, and force the programmer to use a
speciﬁc interface, for example to force use of getters and setters instead of referencing a variable directly:
class MyClass {
public: /* Methods: */
    int x() const noexcept { return m_x; }
    void setX(int const x) noexcept { m_x = x; }
private: /* Fields: */
    int m_x;
};
Using protected is useful for allowing certain functionality of the type to be only accessible to the derived classes,
for example, in the following code, the method calculateValue() is only accessible to classes deriving from the

base class Plus2Base, such as FortyTwo:
struct Plus2Base {
    int value() noexcept { return calculateValue() + 2; }
protected: /* Methods: */
    virtual int calculateValue() noexcept = 0;
};
struct FortyTwo: Plus2Base {
protected: /* Methods: */
    int calculateValue() noexcept final override { return 40; }
};
Note that the friend keyword can be used to add access exceptions to functions or types for accessing protected
and private members.
```

The public, protected, and private keywords can also be used to grant or limit access to base class subobjects.
See the Inheritance example.
Section 34.4: Inheritance
Classes/structs can have inheritance relations.
If a class/struct B inherits from a class/struct A, this means that B has as a parent A. We say that B is a derived
class/struct from A, and A is the base class/struct.
```cpp
struct A
{
public:
    int p1;
protected:
    int p2;
private:
    int p3;
};
//Make B inherit publicly (default) from A
struct B : A
{
};
There are 3 forms of inheritance for a class/struct:
public
private
protected
Note that the default inheritance is the same as the default visibility of members: public if you use the struct
keyword, and private for the class keyword.
It's even possible to have a class derive from a struct (or vice versa). In this case, the default inheritance is
controlled by the child, so a struct that derives from a class will default to public inheritance, and a class that
derives from a struct will have private inheritance by default.
public inheritance:
struct B : public A // or just `struct B : A`
{
    void foo()
    {
        p1 = 0; //well formed, p1 is public in B
        p2 = 0; //well formed, p2 is protected in B
        p3 = 0; //ill formed, p3 is private in A
    }
};
B b;
b.p1 = 1; //well formed, p1 is public
b.p2 = 1; //ill formed, p2 is protected
b.p3 = 1; //ill formed, p3 is inaccessible
private inheritance:
struct B : private A
{
    void foo()
    {
        p1 = 0; //well formed, p1 is private in B
        p2 = 0; //well formed, p2 is private in B
        p3 = 0; //ill formed, p3 is private in A
    }
};
B b;
b.p1 = 1; //ill formed, p1 is private
b.p2 = 1; //ill formed, p2 is private
b.p3 = 1; //ill formed, p3 is inaccessible
protected inheritance:
struct B : protected A
{
    void foo()
    {
        p1 = 0; //well formed, p1 is protected in B
        p2 = 0; //well formed, p2 is protected in B
        p3 = 0; //ill formed, p3 is private in A
    }
};
B b;
b.p1 = 1; //ill formed, p1 is protected
b.p2 = 1; //ill formed, p2 is protected
b.p3 = 1; //ill formed, p3 is inaccessible
Note that although protected inheritance is allowed, the actual use of it is rare. One instance of how protected
inheritance is used in application is in partial base class specialization (usually referred to as "controlled
polymorphism").
When OOP was relatively new, (public) inheritance was frequently said to model an "IS-A" relationship. That is,
public inheritance is correct only if an instance of the derived class is also an instance of the base class.
This was later reﬁned into the Liskov Substitution Principle: public inheritance should only be used when/if an
instance of the derived class can be substituted for an instance of the base class under any possible circumstance
(and still make sense).
Private inheritance is typically said to embody a completely diﬀerent relationship: "is implemented in terms of"

(sometimes called a "HAS-A" relationship). For example, a Stack class could inherit privately from a Vector class.
```

Private inheritance bears a much greater similarity to aggregation than to public inheritance.
Protected inheritance is almost never used, and there's no general agreement on what sort of relationship it
embodies.
Section 34.5: Friendship
The friend keyword is used to give other classes and functions access to private and protected members of the
class, even through they are deﬁned outside the class's scope.
```cpp
class Animal{
private:
    double weight;
    double height;
public:
    friend void printWeight(Animal animal);
    friend class AnimalPrinter;
    // A common use for a friend function is to overload the operator<< for streaming.
    friend std::ostream& operator<<(std::ostream& os, Animal animal);
};
void printWeight(Animal animal)
{
    std::cout << animal.weight << "\n";
}
class AnimalPrinter
{
public:
    void print(const Animal& animal)
    {
        // Because of the `friend class AnimalPrinter;" declaration, we are
        // allowed to access private members here.
        std::cout << animal.weight << ", " << animal.height << std::endl;
    }
};
std::ostream& operator<<(std::ostream& os, Animal animal)
{
    os << "Animal height: " << animal.height << "\n";
    return os;
}
int main() {
    Animal animal = {10, 5};
    printWeight(animal);
    AnimalPrinter aPrinter;
    aPrinter.print(animal);
    std::cout << animal;
}
```

Output:
```text
10, 5
Animal height: 5
```

Section 34.6: Virtual Inheritance
When using inheritance, you can specify the virtual keyword:
```cpp
struct A{};
struct B: public virtual A{};
```
When class B has virtual base A it means that A will reside in most derived class of inheritance tree, and thus that
most derived class is also responsible for initializing that virtual base:
```cpp
struct A
{
    int member;
    A(int param)
    {
        member = param;
    }
};
struct B: virtual A
{
    B(): A(5){}
};
struct C: B
{
    C(): /*A(88)*/ {}
};
void f()
{
    C object; //error since C is not initializing it's indirect virtual base `A`
}
```
If we un-comment /*A(88)*/ we won't get any error since C is now initializing it's indirect virtual base A.
Also note that when we're creating variable object, most derived class is C, so C is responsible for creating(calling
constructor of) A and thus value of A::member is 88, not 5 (as it would be if we were creating object of type B).
It is useful when solving the diamond problem.:
  A                                        A   A
 / \                                       |   |
B   C                                      B   C
 \ /                                        \ /
  D                                          D
```cpp
virtual inheritance                   normal inheritance
B and C both inherit from A, and D inherits from B and C, so there are 2 instances of A in D! This results in ambiguity
when you're accessing member of A through D, as the compiler has no way of knowing from which class do you
want to access that member (the one which B inherits, or the one that is inherited byC?).
Virtual inheritance solves this problem: Since virtual base resides only in most derived object, there will be only one
instance of A in D.
struct A
{
    void foo() {}

};
struct B : public /*virtual*/ A {};
struct C : public /*virtual*/ A {};
struct D : public B, public C
{
    void bar()
    {
        foo(); //Error, which foo? B::foo() or C::foo()? - Ambiguous
    }
};
```

Removing the comments resolves the ambiguity.
Section 34.7: Private inheritance: restricting base class
interface
Private inheritance is useful when it is required to restrict the public interface of the class:
```cpp
class A {
public:
    int move();
    int turn();
};
class B : private A {
public:
    using A::turn;
};
B b;
b.move();  // compile error
b.turn();  // OK
This approach eﬃciently prevents an access to the A public methods by casting to the A pointer or reference:
B b;
A& a = static_cast<A&>(b); // compile error
In the case of public inheritance such casting will provide access to all the A public methods despite on alternative
ways to prevent this in derived B, like hiding:
class B : public A {
private:
    int move();
};
or private using:
class B : public A {
private:
    using A::move;
};
then for both cases it is possible:
B b;

A& a = static_cast<A&>(b); // OK for public inheritance
a.move(); // OK
Section 34.8: Accessing class members
To access member variables and member functions of an object of a class, the . operator is used:
struct SomeStruct {
  int a;
  int b;
  void foo() {}
};
SomeStruct var;
// Accessing member variable a in var.
std::cout << var.a << std::endl;
// Assigning member variable b in var.
var.b = 1;
// Calling a member function.
var.foo();
When accessing the members of a class via a pointer, the -> operator is commonly used. Alternatively, the instance
can be dereferenced and the . operator used, although this is less common:
struct SomeStruct {
  int a;
  int b;
  void foo() {}
};
SomeStruct var;
SomeStruct *p = &var;
// Accessing member variable a in var via pointer.
std::cout << p->a << std::endl;
std::cout << (*p).a << std::endl;
// Assigning member variable b in var via pointer.
p->b = 1;
(*p).b = 1;
// Calling a member function via a pointer.
p->foo();
(*p).foo();
When accessing static class members, the :: operator is used, but on the name of the class instead of an instance
of it. Alternatively, the static member can be accessed from an instance or a pointer to an instance using the . or ->
operator, respectively, with the same syntax as accessing non-static members.
struct SomeStruct {
  int a;
  int b;
  void foo() {}
  static int c;
  static void bar() {}
};
int SomeStruct::c;
SomeStruct var;
SomeStruct* p = &var;
// Assigning static member variable c in struct SomeStruct.

SomeStruct::c = 5;
// Accessing static member variable c in struct SomeStruct, through var and p.
var.a = var.c;
var.b = p->c;
// Calling a static member function.
SomeStruct::bar();
var.bar();
p->bar();
Background
The -> operator is needed because the member access operator . has precedence over the dereferencing operator
*.
One would expect that *p.a would dereference p (resulting in a reference to the object p is pointing to) and then
accessing its member a. But in fact, it tries to access the member a of p and then dereference it. I.e. *p.a is
equivalent to *(p.a). In the example above, this would result in a compiler error because of two facts: First, p is a
pointer and does not have a member a. Second, a is an integer and, thus, can't be dereferenced.
The uncommonly used solution to this problem would be to explicitly control the precedence: (*p).a
Instead, the -> operator is almost always used. It is a short-hand for ﬁrst dereferencing the pointer and then
accessing it. I.e. (*p).a is exactly the same as p->a.
The :: operator is the scope operator, used in the same manner as accessing a member of a namespace. This is
because a static class member is considered to be in that class' scope, but isn't considered a member of instances
of that class. The use of normal . and -> is also allowed for static members, despite them not being instance
members, for historical reasons; this is of use for writing generic code in templates, as the caller doesn't need to be
concerned with whether a given member function is static or non-static.
Section 34.9: Member Types and Aliases
A class or struct can also deﬁne member type aliases, which are type aliases contained within, and treated as
members of, the class itself.
struct IHaveATypedef {
    typedef int MyTypedef;
};
struct IHaveATemplateTypedef {
    template<typename T>
    using MyTemplateTypedef = std::vector<T>;
};
Like static members, these typedefs are accessed using the scope operator, ::.
IHaveATypedef::MyTypedef i = 5; // i is an int.
IHaveATemplateTypedef::MyTemplateTypedef<int> v; // v is a std::vector<int>.
As with normal type aliases, each member type alias is allowed to refer to any type deﬁned or aliased before, but
not after, its deﬁnition. Likewise, a typedef outside the class deﬁnition can refer to any accessible typedefs within
the class deﬁnition, provided it comes after the class deﬁnition.
template<typename T>
struct Helper {

    T get() const { return static_cast<T>(42); }
};
struct IHaveTypedefs {
//    typedef MyTypedef NonLinearTypedef; // Error if uncommented.
    typedef int MyTypedef;
    typedef Helper<MyTypedef> MyTypedefHelper;
};
IHaveTypedefs::MyTypedef        i; // x_i is an int.
IHaveTypedefs::MyTypedefHelper hi; // x_hi is a Helper<int>.
typedef IHaveTypedefs::MyTypedef TypedefBeFree;
TypedefBeFree ii;                  // ii is an int.
```

Member type aliases can be declared with any access level, and will respect the appropriate access modiﬁer.
```cpp
class TypedefAccessLevels {
    typedef int PrvInt;
  protected:
    typedef int ProInt;
  public:
    typedef int PubInt;
};
TypedefAccessLevels::PrvInt prv_i; // Error: TypedefAccessLevels::PrvInt is private.
TypedefAccessLevels::ProInt pro_i; // Error: TypedefAccessLevels::ProInt is protected.
TypedefAccessLevels::PubInt pub_i; // Good.
class Derived : public TypedefAccessLevels {
    PrvInt prv_i; // Error: TypedefAccessLevels::PrvInt is private.
    ProInt pro_i; // Good.
    PubInt pub_i; // Good.
};
This can be used to provide a level of abstraction, allowing a class' designer to change its internal workings without
breaking code that relies on it.
class Something {
    friend class SomeComplexType;
    short s;
    // ...
  public:
    typedef SomeComplexType MyHelper;
    MyHelper get_helper() const { return MyHelper(8, s, 19.5, "shoe", false); }
    // ...
};
// ...
Something s;
Something::MyHelper hlp = s.get_helper();
In this situation, if the helper class is changed from SomeComplexType to some other type, only the typedef and the

friend declaration would need to be modiﬁed; as long as the helper class provides the same functionality, any code
that uses it as Something::MyHelper instead of specifying it by name will usually still work without any
modiﬁcations. In this manner, we minimise the amount of code that needs to be modiﬁed when the underlying
implementation is changed, such that the type name only needs to be changed in one location.
```

This can also be combined with decltype, if one so desires.
```cpp
class SomethingElse {
    AnotherComplexType<bool, int, SomeThirdClass> helper;
  public:
    typedef decltype(helper) MyHelper;
  private:
    InternalVariable<MyHelper> ivh;
    // ...
  public:
    MyHelper& get_helper() const { return helper; }
    // ...
};
In this situation, changing the implementation of SomethingElse::helper will automatically change the typedef for
us, due to decltype. This minimises the number of modiﬁcations necessary when we want to change helper, which
minimises the risk of human error.
As with everything, however, this can be taken too far. If the typename is only used once or twice internally and
zero times externally, for example, there's no need to provide an alias for it. If it's used hundreds or thousands of
times throughout a project, or if it has a long enough name, then it can be useful to provide it as a typedef instead
of always using it in absolute terms. One must balance forwards compatibility and convenience with the amount of
unnecessary noise created.
```

This can also be used with template classes, to provide access to the template parameters from outside the class.
```cpp
template<typename T>
class SomeClass {
    // ...
  public:
    typedef T MyParam;
    MyParam getParam() { return static_cast<T>(42); }
};
template<typename T>
typename T::MyParam some_func(T& t) {
    return t.getParam();
}
SomeClass<int> si;
int i = some_func(si);
This is commonly used with containers, which will usually provide their element type, and other helper types, as
member type aliases. Most of the containers in the C++ standard library, for example, provide the following 12
helper types, along with any other special types they might need.
template<typename T>

class SomeContainer {
    // ...
  public:
    // Let's provide the same helper types as most standard containers.
    typedef T                                     value_type;
    typedef std::allocator<value_type>            allocator_type;
    typedef value_type&                           reference;
    typedef const value_type&                     const_reference;
    typedef value_type*                           pointer;
    typedef const value_type*                     const_pointer;
    typedef MyIterator<value_type>                iterator;
    typedef MyConstIterator<value_type>           const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;
};
Prior to C++11, it was also commonly used to provide a "template typedef" of sorts, as the feature wasn't yet
available; these have become a bit less common with the introduction of alias templates, but are still useful in some
situations (and are combined with alias templates in other situations, which can be very useful for obtaining
individual components of a complex type such as a function pointer). They commonly use the name type for their
type alias.
template<typename T>
struct TemplateTypedef {
    typedef T type;
}
TemplateTypedef<int>::type i; // i is an int.
This was often used with types with multiple template parameters, to provide an alias that deﬁnes one or more of
the parameters.
template<typename T, size_t SZ, size_t D>
class Array { /* ... */ };
template<typename T, size_t SZ>
struct OneDArray {
    typedef Array<T, SZ, 1> type;
};
template<typename T, size_t SZ>
struct TwoDArray {
    typedef Array<T, SZ, 2> type;
};
template<typename T>
struct MonoDisplayLine {
    typedef Array<T, 80, 1> type;
};
OneDArray<int, 3>::type     arr1i; // arr1i is an Array<int, 3, 1>.
TwoDArray<short, 5>::type   arr2s; // arr2s is an Array<short, 5, 2>.
MonoDisplayLine<char>::type arr3c; // arr3c is an Array<char, 80, 1>.

Section 34.10: Nested Classes/Structures
A class or struct can also contain another class/struct deﬁnition inside itself, which is called a "nested class"; in
this situation, the containing class is referred to as the "enclosing class". The nested class deﬁnition is considered to
be a member of the enclosing class, but is otherwise separate.
struct Outer {
    struct Inner { };
};
From outside of the enclosing class, nested classes are accessed using the scope operator. From inside the
enclosing class, however, nested classes can be used without qualiﬁers:
struct Outer {
    struct Inner { };
    Inner in;
};
// ...
Outer o;
Outer::Inner i = o.in;
As with a non-nested class/struct, member functions and static variables can be deﬁned either within a nested
class, or in the enclosing namespace. However, they cannot be deﬁned within the enclosing class, due to it being
considered to be a diﬀerent class than the nested class.
// Bad.
struct Outer {
    struct Inner {
        void do_something();
    };
    void Inner::do_something() {}
};
// Good.
struct Outer {
    struct Inner {
        void do_something();
    };
};
void Outer::Inner::do_something() {}
As with non-nested classes, nested classes can be forward declared and deﬁned later, provided they are deﬁned
before being used directly.
class Outer {
    class Inner1;
    class Inner2;
    class Inner1 {};
    Inner1 in1;
```


##### Operator Overloading

In C++, it is possible to deﬁne operators such as + and -> for user-deﬁned types. For example, the <string> header
deﬁnes a + operator to concatenate strings. This is done by deﬁning an operator function using the operator
keyword.
Section 36.1: Arithmetic operators
You can overload all basic arithmetic operators:
+ and +=
- and -=
* and *=
/ and /=
& and &=
| and |=
^ and ^=
>> and >>=
<< and <<=
Overloading for all operators is the same. Scroll down for explanation
Overloading outside of class/struct:
//operator+ should be implemented in terms of operator+=
T operator+(T lhs, const T& rhs)
{
    lhs += rhs;
    return lhs;
}
T& operator+=(T& lhs, const T& rhs)
{
    //Perform addition
    return lhs;
}
Overloading inside of class/struct:
//operator+ should be implemented in terms of operator+=
T operator+(const T& rhs)
{
    *this += rhs;
    return *this;
}
T& operator+=(const T& rhs)
{
    //Perform addition
    return *this;
}
Note: operator+ should return by non-const value, as returning a reference wouldn't make sense (it returns a new
object) nor would returning a const value (you should generally not return by const). The ﬁrst argument is passed
by value, why? Because
1.
You can't modify the original object (Object foobar = foo + bar; shouldn't modify foo after all, it wouldn't
make sense)
2.
You can't make it const, because you will have to be able to modify the object (because operator+ is
implemented in terms of operator+=, which modiﬁes the object)
Passing by const& would be an option, but then you will have to make a temporary copy of the passed object. By
passing by value, the compiler does it for you.
operator+= returns a reference to the itself, because it is then possible to chain them (don't use the same variable
though, that would be undeﬁned behavior due to sequence points).
The ﬁrst argument is a reference (we want to modify it), but not const, because then you wouldn't be able to
modify it. The second argument should not be modiﬁed, and so for performance reason is passed by const&
(passing by const reference is faster than by value).
Section 36.2: Array subscript operator
You can even overload the array subscript operator [].
You should always (99.98% of the time) implement 2 versions, a const and a not-const version, because if the
object is const, it should not be able to modify the object returned by [].
The arguments are passed by const& instead of by value because passing by reference is faster than by value, and
const so that the operator doesn't change the index accidentally.
The operators return by reference, because by design you can modify the object [] return, i.e:
```cpp
std::vector<int> v{ 1 };
v[0] = 2; //Changes value of 1 to 2
          //wouldn't be possible if not returned by reference
You can only overload inside a class/struct:
//I is the index type, normally an int
T& operator[](const I& index)
{
    //Do something
    //return something
}
//I is the index type, normally an int
const T& operator[](const I& index) const
{
    //Do something
    //return something
}
Multiple subscript operators, [][]..., can be achieved via proxy objects. The following example of a simple row-
major matrix class demonstrates this:
template<class T>
class matrix {
    // class enabling [][] overload to access matrix elements
    template <class C>
    class proxy_row_vector {
        using reference = decltype(std::declval<C>()[0]);
        using const_reference = decltype(std::declval<C const>()[0]);
    public:
        proxy_row_vector(C& _vec, std::size_t _r_ind, std::size_t _cols)
            : vec(_vec), row_index(_r_ind), cols(_cols) {}
        const_reference operator[](std::size_t _col_index) const {
            return vec[row_index*cols + _col_index];
        }
        reference operator[](std::size_t _col_index) {
            return vec[row_index*cols + _col_index];
        }
    private:
        C& vec;
        std::size_t row_index; // row index to access
        std::size_t cols; // number of columns in matrix
    };
    using const_proxy = proxy_row_vector<const std::vector<T>>;
    using proxy = proxy_row_vector<std::vector<T>>;
public:
    matrix() : mtx(), rows(0), cols(0) {}
    matrix(std::size_t _rows, std::size_t _cols)
        : mtx(_rows*_cols), rows(_rows), cols(_cols) {}
    // call operator[] followed by another [] call to access matrix elements
    const_proxy operator[](std::size_t _row_index) const {
        return const_proxy(mtx, _row_index, cols);
    }
    proxy operator[](std::size_t _row_index) {
        return proxy(mtx, _row_index, cols);
    }
private:
    std::vector<T> mtx;
    std::size_t rows;
    std::size_t cols;
};
Section 36.3: Conversion operators
```

You can overload type operators, so that your type can be implicitly converted into the speciﬁed type.
The conversion operator must be deﬁned in a class/struct:
operator T() const { /* return something */ }
Note: the operator is const to allow const objects to be converted.
Example:
```cpp
struct Text
{
    std::string text;
    // Now Text can be implicitly converted into a const char*
    /*explicit*/ operator const char*() const { return text.data(); }
    // ^^^^^^^
    // to disable implicit conversion
};
Text t;
t.text = "Hello world!";
//Ok
const char* copyoftext = t;
Section 36.4: Complex Numbers Revisited
The code below implements a very simple complex number type for which the underlying ﬁeld is automatically
promoted, following the language's type promotion rules, under application of the four basic operators (+, -, *, and
/) with a member of a diﬀerent ﬁeld (be it another complex<T> or some scalar type).
```

This is intended to be a holistic example covering operator overloading alongside basic use of templates.
```cpp
#include <type_traits>
namespace not_std{
using std::decay_t;
//----------------------------------------------------------------
// complex< value_t >
//----------------------------------------------------------------
template<typename value_t>
struct complex
{
    value_t x;
    value_t y;
    complex &operator += (const value_t &x)
    {
        this->x += x;
        return *this;
    }
    complex &operator += (const complex &other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    complex &operator -= (const value_t &x)
    {
        this->x -= x;
        return *this;
    }
    complex &operator -= (const complex &other)
    {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
    complex &operator *= (const value_t &s)
    {
        this->x *= s;
        this->y *= s;
        return *this;
    }
    complex &operator *= (const complex &other)
    {
        (*this) = (*this) * other;
        return *this;
    }
    complex &operator /= (const value_t &s)
    {
        this->x /= s;
        this->y /= s;
        return *this;
    }
    complex &operator /= (const complex &other)
    {
        (*this) = (*this) / other;
        return *this;
    }
    complex(const value_t &x, const value_t &y)
    : x{x}
    , y{y}
    {}
    template<typename other_value_t>
    explicit complex(const complex<other_value_t> &other)
    : x{static_cast<const value_t &>(other.x)}
    , y{static_cast<const value_t &>(other.y)}
    {}
    complex &operator = (const complex &) = default;
    complex &operator = (complex &&) = default;
    complex(const complex &) = default;
    complex(complex &&) = default;
    complex() = default;
};
// Absolute value squared
template<typename value_t>
value_t absqr(const complex<value_t> &z)
{ return z.x*z.x + z.y*z.y; }
//----------------------------------------------------------------
// operator - (negation)
//----------------------------------------------------------------
template<typename value_t>
complex<value_t> operator - (const complex<value_t> &z)
{ return {-z.x, -z.y}; }
//----------------------------------------------------------------
// operator +
//----------------------------------------------------------------
template<typename left_t,typename right_t>
auto operator + (const complex<left_t> &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a.x + b.x)>>
{ return{a.x + b.x, a.y + b.y}; }
template<typename left_t,typename right_t>
auto operator + (const left_t &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a + b.x)>>
{ return{a + b.x, b.y}; }
template<typename left_t,typename right_t>
auto operator + (const complex<left_t> &a, const right_t &b)
-> complex<decay_t<decltype(a.x + b)>>
{ return{a.x + b, a.y}; }
//----------------------------------------------------------------
// operator -
//----------------------------------------------------------------
template<typename left_t,typename right_t>
auto operator - (const complex<left_t> &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a.x - b.x)>>
{ return{a.x - b.x, a.y - b.y}; }
template<typename left_t,typename right_t>
auto operator - (const left_t &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a - b.x)>>
{ return{a - b.x, - b.y}; }
template<typename left_t,typename right_t>
auto operator - (const complex<left_t> &a, const right_t &b)
-> complex<decay_t<decltype(a.x - b)>>
{ return{a.x - b, a.y}; }
//----------------------------------------------------------------
// operator *
//----------------------------------------------------------------
template<typename left_t, typename right_t>
auto operator * (const complex<left_t> &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a.x * b.x)>>
{
    return {
        a.x*b.x - a.y*b.y,
        a.x*b.y + a.y*b.x
        };
}
template<typename left_t, typename right_t>
auto operator * (const left_t &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a * b.x)>>
{ return {a * b.x, a * b.y}; }
template<typename left_t, typename right_t>
auto operator * (const complex<left_t> &a, const right_t &b)
-> complex<decay_t<decltype(a.x * b)>>
{ return {a.x * b, a.y * b}; }
//----------------------------------------------------------------
// operator /
//----------------------------------------------------------------
template<typename left_t, typename right_t>
auto operator / (const complex<left_t> &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a.x / b.x)>>
{
    const auto r = absqr(b);
    return {
        ( a.x*b.x + a.y*b.y) / r,
        (-a.x*b.y + a.y*b.x) / r
        };
}
template<typename left_t, typename right_t>
auto operator / (const left_t &a, const complex<right_t> &b)
-> complex<decay_t<decltype(a / b.x)>>
{
    const auto s = a/absqr(b);
    return {
         b.x * s,
        -b.y * s
        };
}
template<typename left_t, typename right_t>
auto operator / (const complex<left_t> &a, const right_t &b)
-> complex<decay_t<decltype(a.x / b)>>
{ return {a.x / b, a.y / b}; }
}// namespace not_std
int main(int argc, char **argv)
{
    using namespace not_std;
    complex<float> fz{4.0f, 1.0f};
    // makes a complex<double>
    auto dz = fz * 1.0;
    // still a complex<double>
    auto idz = 1.0f/dz;
    // also a complex<double>
    auto one = dz * idz;
    // a complex<double> again
    auto one_again = fz * idz;
    // Operator tests, just to make sure everything compiles.
    complex<float> a{1.0f, -2.0f};
    complex<double> b{3.0, -4.0};
    // All of these are complex<double>
    auto c0 = a + b;
    auto c1 = a - b;
    auto c2 = a * b;
    auto c3 = a / b;
    // All of these are complex<float>
    auto d0 = a + 1;
    auto d1 = 1 + a;
    auto d2 = a - 1;
    auto d3 = 1 - a;
    auto d4 = a * 1;
    auto d5 = 1 * a;
    auto d6 = a / 1;
    auto d7 = 1 / a;
    // All of these are complex<double>
    auto e0 = b + 1;
    auto e1 = 1 + b;
    auto e2 = b - 1;
    auto e3 = 1 - b;
    auto e4 = b * 1;
    auto e5 = 1 * b;
    auto e6 = b / 1;
    auto e7 = 1 / b;
    return 0;
}
Section 36.5: Named operators
You can extend C++ with named operators that are "quoted" by standard C++ operators.
First we start with a dozen-line library:
namespace named_operator {
  template<class D>struct make_operator{constexpr make_operator(){}};
  template<class T, char, class O> struct half_apply { T&& lhs; };
  template<class Lhs, class Op>
  half_apply<Lhs, '*', Op> operator*( Lhs&& lhs, make_operator<Op> ) {
    return {std::forward<Lhs>(lhs)};
  }
  template<class Lhs, class Op, class Rhs>
  auto operator*( half_apply<Lhs, '*', Op>&& lhs, Rhs&& rhs )
  -> decltype( named_invoke( std::forward<Lhs>(lhs.lhs), Op{}, std::forward<Rhs>(rhs) ) )
  {
    return named_invoke( std::forward<Lhs>(lhs.lhs), Op{}, std::forward<Rhs>(rhs) );
  }
}
this doesn't do anything yet.
First, appending vectors
namespace my_ns {
  struct append_t : named_operator::make_operator<append_t> {};
  constexpr append_t append{};
  template<class T, class A0, class A1>
  std::vector<T, A0> named_invoke( std::vector<T, A0> lhs, append_t, std::vector<T, A1> const& rhs
) {
      lhs.insert( lhs.end(), rhs.begin(), rhs.end() );
      return std::move(lhs);
  }
}
using my_ns::append;
std::vector<int> a {1,2,3};
std::vector<int> b {4,5,6};
auto c = a *append* b;
The core here is that we deﬁne an append object of type append_t:named_operator::make_operator<append_t>.
```

We then overload named_invoke( lhs, append_t, rhs ) for the types we want on the right and left.
The library overloads lhs*append_t, returning a temporary half_apply object. It also overloads half_apply*rhs to
call named_invoke( lhs, append_t, rhs ).
We simply have to create the proper append_t token and do an ADL-friendly named_invoke of the proper signature,
and everything hooks up and works.
For a more complex example, suppose you want to have element-wise multiplication of elements of a std::array:
```cpp
template<class=void, std::size_t...Is>
auto indexer( std::index_sequence<Is...> ) {
  return [](auto&& f) {
    return f( std::integral_constant<std::size_t, Is>{}... );
  };
}
template<std::size_t N>
auto indexer() { return indexer( std::make_index_sequence<N>{} ); }
namespace my_ns {
  struct e_times_t : named_operator::make_operator<e_times_t> {};
  constexpr e_times_t e_times{};
  template<class L, class R, std::size_t N,
    class Out=std::decay_t<decltype( std::declval<L const&>()*std::declval<R const&>() )>
  >
  std::array<Out, N> named_invoke( std::array<L, N> const& lhs, e_times_t, std::array<R, N> const&
rhs ) {
    using result_type = std::array<Out, N>;
    auto index_over_N = indexer<N>();
    return index_over_N([&](auto...is)->result_type {
      return {{
        (lhs[is] * rhs[is])...
      }};
    });
  }
}
live example.
This element-wise array code can be extended to work on tuples or pairs or C-style arrays, or even variable length
containers if you decide what to do if the lengths don't match.
```

You could also an element-wise operator type and get lhs *element_wise<'+'>* rhs.
Writing a *dot* and *cross* product operators are also obvious uses.
The use of * can be extended to support other delimiters, like +. The delimeter precidence determines the
precidence of the named operator, which may be important when translating physics equations over to C++ with
minimal use of extra ()s.
With a slight change in the library above, we can support ->*then* operators and extend std::function prior to
the standard being updated, or write monadic ->*bind*. It could also have a stateful named operator, where we
carefully pass the Op down to the ﬁnal invoke function, permitting:
named_operator<'*'> append = [](auto lhs, auto&& rhs) {
```cpp
  using std::begin; using std::end;
  lhs.insert( end(lhs), begin(rhs), end(rhs) );
  return std::move(lhs);
};
generating a named container-appending operator in C++17.
Section 36.6: Unary operators
You can overload the 2 unary operators:
++foo and foo++
--foo and foo--
Overloading is the same for both types (++ and --). Scroll down for explanation
Overloading outside of class/struct:
//Prefix operator ++foo
T& operator++(T& lhs)
{
    //Perform addition
    return lhs;
}
//Postfix operator foo++ (int argument is used to separate pre- and postfix)
//Should be implemented in terms of ++foo (prefix operator)
T operator++(T& lhs, int)
{
    T t(lhs);
    ++lhs;
    return t;
}
Overloading inside of class/struct:
//Prefix operator ++foo
T& operator++()
{
    //Perform addition
    return *this;
}
//Postfix operator foo++ (int argument is used to separate pre- and postfix)
//Should be implemented in terms of ++foo (prefix operator)
T operator++(int)
{
    T t(*this);
    ++(*this);
    return t;
}
Note: The preﬁx operator returns a reference to itself, so that you can continue operations on it. The ﬁrst argument
is a reference, as the preﬁx operator changes the object, that's also the reason why it isn't const (you wouldn't be
able to modify it otherwise).
The postﬁx operator returns by value a temporary (the previous value), and so it cannot be a reference, as it would
be a reference to a temporary, which would be garbage value at the end of the function, because the temporary
variable goes out of scope). It also cannot be const, because you should be able to modify it directly.
The ﬁrst argument is a non-const reference to the "calling" object, because if it were const, you wouldn't be able to
modify it, and if it weren't a reference, you wouldn't change the original value.
It is because of the copying needed in postﬁx operator overloads that it's better to make it a habit to use preﬁx ++
instead of postﬁx ++ in for loops. From the for loop perspective, they're usually functionally equivalent, but there
might be a slight performance advantage to using preﬁx ++, especially with "fat" classes with a lot of members to
copy. Example of using preﬁx ++ in a for loop:
for (list<string>::const_iterator it = tokens.begin();
     it != tokens.end();
     ++it) { // Don't use it++
}
Section 36.7: Comparison operators
You can overload all comparison operators:
== and !=
> and <
>= and <=
The recommended way to overload all those operators is by implementing only 2 operators (== and <) and then
using those to deﬁne the rest. Scroll down for explanation
Overloading outside of class/struct:
//Only implement those 2
bool operator==(const T& lhs, const T& rhs) { /* Compare */ }
bool operator<(const T& lhs, const T& rhs) { /* Compare */ }
//Now you can define the rest
bool operator!=(const T& lhs, const T& rhs) { return !(lhs == rhs); }
bool operator>(const T& lhs, const T& rhs) { return rhs < lhs; }
bool operator<=(const T& lhs, const T& rhs) { return !(lhs > rhs); }
bool operator>=(const T& lhs, const T& rhs) { return !(lhs < rhs); }
Overloading inside of class/struct:
//Note that the functions are const, because if they are not const, you wouldn't be able
//to call them if the object is const
//Only implement those 2
bool operator==(const T& rhs) const { /* Compare */ }
bool operator<(const T& rhs) const { /* Compare */ }
//Now you can define the rest
bool operator!=(const T& rhs) const { return !(*this == rhs); }
bool operator>(const T& rhs) const { return rhs < *this; }
bool operator<=(const T& rhs) const { return !(*this > rhs); }
bool operator>=(const T& rhs) const { return !(*this < rhs); }
The operators obviously return a bool, indicating true or false for the corresponding operation.
All of the operators take their arguments by const&, because the only thing that does operators do is compare, so
they shouldn't modify the objects. Passing by & (reference) is faster than by value, and to make sure that the
operators don't modify it, it is a const-reference.
Note that the operators inside the class/struct are deﬁned as const, the reason for this is that without the
functions being const, comparing const objects would not be possible, as the compiler doesn't know that the
operators don't modify anything.
Section 36.8: Assignment operator
The assignment operator is one of the most important operators because it allows you to change the status of a
variable.
If you do not overload the assignment operator for your class/struct, it is automatically generated by the
compiler: the automatically-generated assignment operator performs a "memberwise assignment", ie by invoking
assignment operators on all members, so that one object is copied to the other, a member at time. The assignment
operator should be overloaded when the simple memberwise assignment is not suitable for your class/struct, for
example if you need to perform a deep copy of an object.
```

Overloading the assignment operator = is easy, but you should follow some simple steps.
1.
Test for self-assignment. This check is important for two reasons:
a self-assignment is a needless copy, so it does not make sense to perform it;
the next step will not work in the case of a self-assignment.
2.
Clean the old data. The old data must be replaced with new ones. Now, you can understand the second
reason of the previous step: if the content of the object was destroyed, a self-assignment will fail to perform
the copy.
3.
Copy all members. If you overload the assignment operator for your class or your struct, it is not
automatically generated by the compiler, so you will need to take charge of copying all members from the
other object.
4.
Return *this. The operator returns by itself by reference, because it allows chaining (i.e. int b = (a = 6) +
4; //b == 10).
//T is some type
T& operator=(const T& other)
{
    //Do something (like copying values)
    return *this;
}
Note: other is passed by const&, because the object being assigned should not be changed, and passing by
reference is faster than by value, and to make sure than operator= doesn't modify it accidentally, it is const.
The assignment operator can only to be overloaded in the class/struct, because the left value of = is always the
class/struct itself. Deﬁning it as a free function doesn't have this guarantee, and is disallowed because of that.
When you declare it in the class/struct, the left value is implicitly the class/struct itself, so there is no problem
with that.
Section 36.9: Function call operator
You can overload the function call operator ():
Overloading must be done inside of a class/struct:
//R -> Return type
//Types -> any different type
R operator()(Type name, Type2 name2, ...)
{
    //Do something
    //return something
}
//Use it like this (R is return type, a and b are variables)
R foo = object(a, b, ...);
For example:
```cpp
struct Sum
{
    int operator()(int a, int b)
    {
        return a + b;
    }
};
//Create instance of struct
Sum sum;
int result = sum(1, 1); //result == 2
Section 36.10: Bitwise NOT operator
Overloading the bitwise NOT (~) is fairly simple. Scroll down for explanation
Overloading outside of class/struct:
T operator~(T lhs)
{
    //Do operation
    return lhs;
}
Overloading inside of class/struct:
T operator~()
{
    T t(*this);
    //Do operation
    return t;
}
Note: operator~ returns by value, because it has to return a new value (the modiﬁed value), and not a reference to
the value (it would be a reference to the temporary object, which would have garbage value in it as soon as the
operator is done). Not const either because the calling code should be able to modify it afterwards (i.e. int a = ~a
+ 1; should be possible).
Inside the class/struct you have to make a temporary object, because you can't modify this, as it would modify
the original object, which shouldn't be the case.
Section 36.11: Bit shift operators for I/O
The operators << and >> are commonly used as "write" and "read" operators:
std::ostream overloads << to write variables to the underlying stream (example: std::cout)
std::istream overloads >> to read from the underlying stream to a variable (example: std::cin)
The way they do this is similar if you wanted to overload them "normally" outside of the class/struct, except that
specifying the arguments are not of the same type:
Return type is the stream you want to overload from (for example, std::ostream) passed by reference, to
allow chaining (Chaining: std::cout << a << b;). Example: std::ostream&
lhs would be the same as the return type
rhs is the type you want to allow overloading from (i.e. T), passed by const& instead of value for performance
reason (rhs shouldn't be changed anyway). Example: const Vector&.
Example:
//Overload std::ostream operator<< to allow output from Vector's
std::ostream& operator<<(std::ostream& lhs, const Vector& rhs)
{
    lhs << "x: " << rhs.x << " y: " << rhs.y << " z: " << rhs.z << '\n';
    return lhs;
}
Vector v = { 1, 2, 3};
//Now you can do
std::cout << v;



***
```




##### const keyword

Section 17.1: Avoiding duplication of code in const and non-
const getter methods
In C++ methods that diﬀers only by const qualiﬁer can be overloaded. Sometimes there may be a need of two
versions of getter that return a reference to some member.
Let Foo be a class, that has two methods that perform identical operations and returns a reference to an object of
type Bar:
```cpp
class Foo
{
public:
    Bar& GetBar(/* some arguments */)
    {
        /* some calculations */
        return bar;
    }
    const Bar& GetBar(/* some arguments */) const
    {
        /* some calculations */
        return bar;
    }
    // ...
};
The only diﬀerence between them is that one method is non-const and return a non-const reference (that can be
use to modify object) and the second is const and returns const reference.
To avoid the code duplication, there is a temptation to call one method from another. However, we can not call
non-const method from the const one. But we can call const method from non-const one. That will require as to
use 'const_cast' to remove the const qualiﬁer.
The solution is:
struct Foo
{
    Bar& GetBar(/*arguments*/)
    {
        return const_cast<Bar&>(const_cast<const Foo*>(this)->GetBar(/*arguments*/));
    }
    const Bar& GetBar(/*arguments*/) const
    {
        /* some calculations */
        return foo;
    }
};
In code above, we call const version of GetBar from the non-const GetBar by casting this to const type:
const_cast<const Foo*>(this). Since we call const method from non-const, the object itself is non-const, and
casting away the const is allowed.
Examine the following more complete example:
#include <iostream>
class Student
{
public:
    char& GetScore(bool midterm)
    {
        return const_cast<char&>(const_cast<const Student*>(this)->GetScore(midterm));
    }
    const char& GetScore(bool midterm) const
    {
        if (midterm)
        {
            return midtermScore;
        }
        else
        {
            return finalScore;
        }
    }
private:
    char midtermScore;
    char finalScore;
};
int main()
{
    // non-const object
    Student a;
    // We can assign to the reference. Non-const version of GetScore is called
    a.GetScore(true) = 'B';
    a.GetScore(false) = 'A';
    // const object
    const Student b(a);
    // We still can call GetScore method of const object,
    // because we have overloaded const version of GetScore
    std::cout << b.GetScore(true) << b.GetScore(false) << '\n';
}
Section 17.2: Const member functions
Member functions of a class can be declared const, which tells the compiler and future readers that this function
will not modify the object:
class MyClass
{
private:
    int myInt_;
public:
    int myInt() const { return myInt_; }
    void setMyInt(int myInt) { myInt_ = myInt; }
};
In a const member function, the this pointer is eﬀectively a const MyClass * instead of a MyClass *. This means
that you cannot change any member variables within the function; the compiler will emit a warning. So setMyInt
could not be declared const.
You should almost always mark member functions as const when possible. Only const member functions can be
called on a const MyClass.
static methods cannot be declared as const. This is because a static method belongs to a class and is not called
on object; therefore it can never modify object's internal variables. So declaring static methods as const would be
redundant.
Section 17.3: Const local variables
```

Declaration and usage.
// a is const int, so it can't be changed
const int a = 15;  
a = 12;           // Error: can't assign new value to const variable
a += 1;           // Error: can't assign new value to const variable
Binding of references and pointers
int &b = a;       // Error: can't bind non-const reference to const variable
const int &c = a; // OK; c is a const reference
int *d = &a;      // Error: can't bind pointer-to-non-const to const variable
const int *e = &a // OK; e is a pointer-to-const
```cpp
int f = 0;
e = &f;           // OK; e is a non-const pointer-to-const,
                  // which means that it can be rebound to new int* or const int*
*e = 1            // Error: e is a pointer-to-const which means that
                  // the value it points to can't be changed through dereferencing e
int *g = &f;
*g = 1;           // OK; this value still can be changed through dereferencing
                  // a pointer-not-to-const
Section 17.4: Const pointers
int a = 0, b = 2;
const int* pA = &a; // pointer-to-const. `a` can't be changed through this
int* const pB = &a; // const pointer. `a` can be changed, but this pointer can't.
const int* const pC = &a; // const pointer-to-const.
//Error: Cannot assign to a const reference
*pA = b;
pA = &b;
*pB = b;
//Error: Cannot assign to const pointer
pB = &b;
//Error: Cannot assign to a const reference
*pC = b;
//Error: Cannot assign to const pointer
pC = &b;
```




#### Class Member Variables & Methods (C++98)

```cpp
#include <iostream>

class BankAccount {
private:
    double balance;
    std::string owner;
public:
    // Constructor
    BankAccount(const std::string& name, double initial) 
        : owner(name), balance(initial) {}
    
    // Methods
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }
    
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
        }
    }
    
    double get_balance() const {  // const = doesn't modify
        return balance;
    }
    
    std::string get_owner() const {
        return owner;
    }
};

int main() {
    BankAccount account("Alice", 1000);
    account.deposit(500);
    account.withdraw(200);
    
    std::cout << account.get_owner() << ": $" 
              << account.get_balance() << "\n";
    
    return 0;
}
```



#### What is a Class?

A **class** is a blueprint for creating objects. It defines:
- **Attributes** (member variables) - what the object has
- **Methods** (member functions) - what the object does



#### What is an Object?

An **object** is an instance of a class - a concrete entity with specific values.



### The Four Pillars of OOP

Object-Oriented Programming is built on four fundamental concepts that distinguish it from procedural programming:



#### 4. **Abstraction** - Simplified Interface

```cpp
#include <iostream>
#include <string>
using namespace std;

// Class definition
class Car {
public:  // Public members accessible from outside
    // Member variables
    string brand;
    string color;
    int year;
    int speed;
    
    // Member functions
    void accelerate() {
        speed += 10;
        cout << brand << " accelerates to " << speed << " mph\n";
    }
    
    void brake() {
        if (speed >= 10) {
            speed -= 10;
        } else {
            speed = 0;
        }
        cout << brand << " brakes to " << speed << " mph\n";
    }
    
    void displayInfo() {
        cout << year << " " << color << " " << brand 
             << " at " << speed << " mph\n";
    }
};

int main() {
    // Creating objects (instances)
    Car car1;
    car1.brand = "Toyota";
    car1.color = "Red";
    car1.year = 2020;
    car1.speed = 0;
    
    Car car2;
    car2.brand = "BMW";
    car2.color = "Blue";
    car2.year = 2022;
    car2.speed = 0;
    
    // Using objects
    car1.displayInfo();
    car1.accelerate();
    car1.accelerate();
    car1.brake();
    
    car2.displayInfo();
    car2.accelerate();
    car2.accelerate();
    car2.accelerate();
    
    return 0;
}
```

**Output:**
```
2020 Red Toyota at 0 mph
Toyota accelerates to 10 mph
Toyota accelerates to 20 mph
Toyota brakes to 10 mph
2022 Blue BMW at 0 mph
BMW accelerates to 10 mph
BMW accelerates to 20 mph
BMW accelerates to 30 mph
```

***



#### Access Levels (Access Modifiers)

```cpp
#include <iostream>
#include <string>
using namespace std;

class BankAccount {
private:  // Only accessible within this class
    double balance;
    string accountNumber;
    
    // Private helper function
    bool validateAmount(double amount) {
        return amount > 0;
    }
    
public:  // Accessible from anywhere
    // Constructor
    BankAccount(string accNum, double initialBalance)
        : accountNumber(accNum), balance(initialBalance) {
        cout << "Account created: " << accountNumber << "\n";
    }
    
    // Public methods to access private data
    void deposit(double amount) {
        if (validateAmount(amount)) {
            balance += amount;
            cout << "Deposited: $" << amount 
                 << ". New balance: $" << balance << "\n";
        } else {
            cout << "Invalid deposit amount\n";
        }
    }
    
    void withdraw(double amount) {
        if (validateAmount(amount) && amount <= balance) {
            balance -= amount;
            cout << "Withdrew: $" << amount 
                 << ". New balance: $" << balance << "\n";
        } else {
            cout << "Cannot withdraw that amount\n";
        }
    }
    
    double getBalance() const {  // const = doesn't modify
        return balance;
    }
    
    string getAccountNumber() const {
        return accountNumber;
    }

protected:  // Accessible in this class and derived classes
    void logTransaction(string type) {
        cout << "Transaction (" << type << ") logged\n";
    }
};

int main() {
    BankAccount account("ACC123456", 1000);
    
    account.deposit(500);
    account.withdraw(200);
    account.withdraw(2000);  // Won't work - insufficient funds
    
    cout << "Final balance: $" << account.getBalance() << "\n";
    
    // These would cause compile errors:
    // account.balance = 10000;        // Error: private
    // account.validateAmount(100);    // Error: private
    // account.accountNumber = "123";  // Error: private
    
    return 0;
}
```

**Output:**
```
Account created: ACC123456
Deposited: $500. New balance: $1500
Withdrew: $200. New balance: $1300
Cannot withdraw that amount
Final balance: $1300
```

**Why Encapsulation?**
- **Data Protection**: Prevents invalid states
- **Control**: You control how data is modified
- **Flexibility**: Can change internal implementation without affecting users
- **Security**: Sensitive data is hidden
- **Maintainability**: Easy to modify implementation later

***



#### Types of Constructors

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
private:
    string name;
    int rollNumber;
    double gpa;
    
public:
    // 1. Default Constructor (no parameters)
    Student() : name("Unknown"), rollNumber(0), gpa(0.0) {
        cout << "Default constructor called\n";
    }
    
    // 2. Parameterized Constructor
    Student(string n, int roll, double g) 
        : name(n), rollNumber(roll), gpa(g) {
        cout << "Parameterized constructor called\n";
    }
    
    // 3. Copy Constructor (copies another object)
    Student(const Student& other) 
        : name(other.name), rollNumber(other.rollNumber), gpa(other.gpa) {
        cout << "Copy constructor called\n";
    }
    
    // 4. Move Constructor (C++11) - transfers ownership
    Student(Student&& other) noexcept
        : name(move(other.name)), rollNumber(other.rollNumber), gpa(other.gpa) {
        other.rollNumber = 0;
        other.gpa = 0.0;
        cout << "Move constructor called\n";
    }
    
    // Destructor - called when object is destroyed
    ~Student() {
        cout << "Destructor called for " << name << "\n";
    }
    
    // Getter methods
    void display() const {
        cout << "Name: " << name << ", Roll: " << rollNumber 
             << ", GPA: " << gpa << "\n";
    }
};

int main() {
    cout << "--- Creating objects ---\n";
    
    // Default constructor
    Student s1;
    s1.display();
    cout << "\n";
    
    // Parameterized constructor
    Student s2("Alice", 101, 3.8);
    s2.display();
    cout << "\n";
    
    // Copy constructor (explicit)
    Student s3 = s2;  // Calls copy constructor
    s3.display();
    cout << "\n";
    
    // Move constructor
    Student s4 = move(s2);  // Calls move constructor
    s4.display();
    cout << "\n";
    
    cout << "--- Objects going out of scope ---\n";
    
    return 0;  // Destructors called here for all objects
}
```

**Output:**
```
--- Creating objects ---
Default constructor called
Name: Unknown, Roll: 0, GPA: 0

Parameterized constructor called
Name: Alice, Roll: 101, GPA: 3.8

Copy constructor called
Name: Alice, Roll: 101, GPA: 3.8

Move constructor called
Name: Alice, Roll: 101, GPA: 3.8

--- Objects going out of scope ---
Destructor called for Alice
Destructor called for Alice
Destructor called for Unknown
Destructor called for Unknown
```



#### Initialization Lists (Member Initializer List)

```cpp
#include <iostream>
using namespace std;

class Rectangle {
private:
    int width;
    int height;

public:
    // Using initialization list
    // This is ALWAYS better than assignment in constructor body
    Rectangle(int w, int h) : width(w), height(h) {
        cout << "Rectangle created\n";
    }
    
    // Alternative (NOT recommended) - less efficient
    // Rectangle(int w, int h) {
    //     width = w;    // Assignment, not initialization
    //     height = h;
    // }
    
    int getArea() const {
        return width * height;
    }
};

int main() {
    Rectangle rect(5, 10);
    cout << "Area: " << rect.getArea() << "\n";
    
    return 0;
}
```

**Why initialization lists?**
- **Efficiency**: Initializes variables once (not create then assign)
- **Const members**: Can only be initialized with initializer list
- **Reference members**: Must use initializer list
- **Base class initialization**: Only way to initialize base class

***



### Inheritance - All Types

**Inheritance** allows a class to inherit properties and methods from another class.



#### Multilevel Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

// Level 1: Base class
class Vehicle {
protected:
    string brand;
    
public:
    Vehicle(string b) : brand(b) {}
    virtual void start() {
        cout << brand << " is starting\n";
    }
    virtual ~Vehicle() {}
};

// Level 2: Derived from Vehicle
class Car : public Vehicle {
protected:
    int numDoors;
    
public:
    Car(string b, int doors) : Vehicle(b), numDoors(doors) {}
    void start() override {
        cout << brand << " car with " << numDoors 
             << " doors is starting\n";
    }
    virtual ~Car() {}
};

// Level 3: Derived from Car
class ElectricCar : public Car {
private:
    int batteryPercentage;
    
public:
    ElectricCar(string b, int doors, int battery)
        : Car(b, doors), batteryPercentage(battery) {}
    
    void start() override {
        cout << "Electric " << brand << " (Battery: " 
             << batteryPercentage << "%) starting\n";
    }
};

int main() {
    ElectricCar tesla("Tesla", 4, 95);
    tesla.start();
    
    return 0;
}
```



#### Hierarchical Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

class Employee {
protected:
    string name;
    double salary;
    
public:
    Employee(string n, double s) : name(n), salary(s) {}
    virtual void work() = 0;
    virtual ~Employee() {}
};

class Manager : public Employee {
public:
    Manager(string n, double s) : Employee(n, s) {}
    void work() override {
        cout << name << " is managing the team\n";
    }
};

class Developer : public Employee {
private:
    string language;
    
public:
    Developer(string n, double s, string lang)
        : Employee(n, s), language(lang) {}
    void work() override {
        cout << name << " is coding in " << language << "\n";
    }
};

class Designer : public Employee {
public:
    Designer(string n, double s) : Employee(n, s) {}
    void work() override {
        cout << name << " is designing UI/UX\n";
    }
};

int main() {
    Manager manager("Alice", 80000);
    Developer dev("Bob", 70000, "C++");
    Designer designer("Carol", 65000);
    
    manager.work();
    dev.work();
    designer.work();
    
    return 0;
}
```

***



##### 3. Template Specialization

```cpp
#include <iostream>
#include <string>
using namespace std;

// Generic template
template <typename T>
class Printer {
public:
    void print(T value) {
        cout << "Generic: " << value << "\n";
    }
};

// Template specialization for bool
template <>
class Printer<bool> {
public:
    void print(bool value) {
        cout << "Boolean: " << (value ? "true" : "false") << "\n";
    }
};

// Template specialization for string
template <>
class Printer<string> {
public:
    void print(string value) {
        cout << "String: \"" << value << "\"\n";
    }
};

int main() {
    Printer<int> intPrinter;
    intPrinter.print(42);
    
    Printer<double> doublePrinter;
    doublePrinter.print(3.14);
    
    Printer<bool> boolPrinter;
    boolPrinter.print(true);
    
    Printer<string> stringPrinter;
    stringPrinter.print("Hello, World!");
    
    return 0;
}
```



##### Virtual Functions & Override

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Shape {
public:
    virtual void draw() = 0;           // Pure virtual
    virtual double area() = 0;
    virtual string getName() = 0;
    virtual ~Shape() {}                // Virtual destructor (important!)
};

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        cout << "Drawing Circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
    
    string getName() override {
        return "Circle";
    }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        cout << "Drawing Rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
    
    string getName() override {
        return "Rectangle";
    }
};

class Triangle : public Shape {
private:
    double base, height;
    
public:
    Triangle(double b, double h) : base(b), height(h) {}
    
    void draw() override {
        cout << "Drawing Triangle\n";
    }
    
    double area() override {
        return 0.5 * base * height;
    }
    
    string getName() override {
        return "Triangle";
    }
};

int main() {
    // Using smart pointers (C++11)
    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>(5));
    shapes.push_back(make_unique<Rectangle>(4, 6));
    shapes.push_back(make_unique<Triangle>(3, 4));
    
    cout << "=== Drawing all shapes ===\n";
    for (auto& shape : shapes) {
        shape->draw();
        cout << "Area of " << shape->getName() << ": " 
             << shape->area() << "\n\n";
    }
    
    return 0;  // Smart pointers automatically deleted
}
```

**Output:**
```
=== Drawing all shapes ===
Drawing Circle
Area of Circle: 78.5385

Drawing Rectangle
Area of Rectangle: 24

Drawing Triangle
Area of Triangle: 6
```



##### Abstract Classes & Interfaces

```cpp
#include <iostream>
#include <string>
using namespace std;

// Abstract class (interface-like)
class DatabaseConnection {
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;
    virtual ~DatabaseConnection() {}
};

class MySQLConnection : public DatabaseConnection {
private:
    bool connected;
    
public:
    MySQLConnection() : connected(false) {}
    
    void connect() override {
        cout << "Connecting to MySQL...\n";
        connected = true;
        cout << "Connected to MySQL\n";
    }
    
    void disconnect() override {
        cout << "Disconnecting from MySQL...\n";
        connected = false;
    }
    
    bool isConnected() override {
        return connected;
    }
};

class PostgreSQLConnection : public DatabaseConnection {
private:
    bool connected;
    
public:
    PostgreSQLConnection() : connected(false) {}
    
    void connect() override {
        cout << "Connecting to PostgreSQL...\n";
        connected = true;
        cout << "Connected to PostgreSQL\n";
    }
    
    void disconnect() override {
        cout << "Disconnecting from PostgreSQL...\n";
        connected = false;
    }
    
    bool isConnected() override {
        return connected;
    }
};

int main() {
    // Cannot create DatabaseConnection directly
    // DatabaseConnection db;  // Error: abstract class
    
    // Create through derived classes
    MySQLConnection mysql;
    PostgreSQLConnection postgres;
    
    // Use polymorphically
    DatabaseConnection* db1 = &mysql;
    DatabaseConnection* db2 = &postgres;
    
    db1->connect();
    cout << "MySQL connected: " << (db1->isConnected() ? "Yes" : "No") << "\n\n";
    
    db2->connect();
    cout << "PostgreSQL connected: " << (db2->isConnected() ? "Yes" : "No") << "\n\n";
    
    db1->disconnect();
    db2->disconnect();
    
    return 0;
}
```

***



#### Public, Private, Protected

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    int publicData = 1;
    void publicMethod() {
        cout << "Public method\n";
    }
    
protected:
    int protectedData = 2;
    void protectedMethod() {
        cout << "Protected method\n";
    }
    
private:
    int privateData = 3;
    void privateMethod() {
        cout << "Private method\n";
    }
};

class Derived : public Base {
public:
    void testAccess() {
        cout << "Public data: " << publicData << "\n";
        cout << "Protected data: " << protectedData << "\n";
        // cout << "Private data: " << privateData << "\n";  // Error!
    }
};

int main() {
    Base b;
    cout << "Public: " << b.publicData << "\n";
    // cout << "Protected: " << b.protectedData << "\n";  // Error!
    // cout << "Private: " << b.privateData << "\n";      // Error!
    
    Derived d;
    d.testAccess();
    
    return 0;
}
```



#### Friend Functions and Classes

```cpp
#include <iostream>
using namespace std;

class MyClass {
private:
    int secretValue;
    
public:
    MyClass(int value) : secretValue(value) {}
    
    // Friend function - can access private members
    friend void revealSecret(MyClass& obj);
    
    // Friend class - can access private members
    friend class FriendClass;
};

// Friend function
void revealSecret(MyClass& obj) {
    cout << "Secret value: " << obj.secretValue << "\n";
}

// Friend class
class FriendClass {
public:
    void accessPrivate(MyClass& obj) {
        cout << "Friend class accessing: " << obj.secretValue << "\n";
    }
};

int main() {
    MyClass obj(42);
    revealSecret(obj);
    
    FriendClass friend;
    friend.accessPrivate(obj);
    
    return 0;
}
```

***



#### Static Variables & Methods

```cpp
#include <iostream>
#include <string>
using namespace std;

class Employee {
private:
    string name;
    int id;
    static int nextId;      // Shared by all instances
    static int totalCount;  // Count of employees
    
public:
    Employee(string n) : name(n), id(nextId++) {
        totalCount++;
    }
    
    ~Employee() {
        totalCount--;
    }
    
    // Static method - can only access static members
    static void displayStats() {
        cout << "Total employees: " << totalCount << "\n";
        cout << "Next ID will be: " << nextId << "\n";
    }
    
    void display() {
        cout << "ID: " << id << ", Name: " << name << "\n";
    }
};

// Initialize static members
int Employee::nextId = 1;
int Employee::totalCount = 0;

int main() {
    cout << "Creating employees...\n";
    Employee emp1("Alice");
    emp1.display();
    
    Employee emp2("Bob");
    emp2.display();
    
    Employee emp3("Carol");
    emp3.display();
    
    // Call static method
    Employee::displayStats();
    
    {
        Employee emp4("David");
        emp4.display();
        Employee::displayStats();
    }
    
    cout << "\nAfter scope ends:\n";
    Employee::displayStats();
    
    return 0;
}
```

**Output:**
```
Creating employees...
ID: 1, Name: Alice
ID: 2, Name: Bob
ID: 3, Name: Carol
Total employees: 3
Next ID will be: 4
ID: 4, Name: David
Total employees: 4
Next ID will be: 5

After scope ends:
Total employees: 3
Next ID will be: 5
```

***



### Const Correctness in OOP

```cpp
#include <iostream>
#include <string>
using namespace std;

class Person {
private:
    mutable int accessCount;  // Can be modified even in const functions
    string name;
    int age;
    
public:
    Person(string n, int a) : name(n), age(a), accessCount(0) {}
    
    // Const method - cannot modify member variables
    string getName() const {
        accessCount++;  // OK because accessCount is mutable
        return name;
    }
    
    // Const method returning const reference
    const string& getNameRef() const {
        accessCount++;
        return name;
    }
    
    // Non-const method - can modify members
    void setAge(int newAge) {
        age = newAge;  // OK in non-const method
    }
    
    // Const method
    int getAge() const {
        return age;
    }
    
    int getAccessCount() const {
        return accessCount;
    }
    
    void display() const {
        cout << "Name: " << name << ", Age: " << age << "\n";
    }
};

int main() {
    Person p("Alice", 30);
    
    // Can call both const and non-const methods
    cout << p.getName() << "\n";
    cout << p.getAge() << "\n";
    p.setAge(31);
    
    // Const object
    const Person cp("Bob", 25);
    
    // Can only call const methods
    cout << cp.getName() << "\n";
    cout << cp.getAge() << "\n";
    // cp.setAge(26);  // Error: cannot call non-const method on const object
    
    cp.display();
    cout << "Access count: " << cp.getAccessCount() << "\n";
    
    return 0;
}
```

***



#### Overloadable Operators

```cpp
#include <iostream>
using namespace std;

class Vector {
private:
    int x, y;
    
public:
    Vector(int x = 0, int y = 0) : x(x), y(y) {}
    
    // Arithmetic operators
    Vector operator+(const Vector& v) const {
        return Vector(x + v.x, y + v.y);
    }
    
    Vector operator-(const Vector& v) const {
        return Vector(x - v.x, y - v.y);
    }
    
    Vector operator*(int scalar) const {
        return Vector(x * scalar, y * scalar);
    }
    
    // Comparison operators
    bool operator==(const Vector& v) const {
        return x == v.x && y == v.y;
    }
    
    bool operator!=(const Vector& v) const {
        return !(*this == v);
    }
    
    // Assignment operator
    Vector& operator=(const Vector& v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
        }
        return *this;
    }
    
    // Unary operators
    Vector operator-() const {
        return Vector(-x, -y);
    }
    
    // Increment/Decrement
    Vector& operator++() {  // Pre-increment
        x++; y++;
        return *this;
    }
    
    Vector operator++(int) {  // Post-increment
        Vector temp = *this;
        x++; y++;
        return temp;
    }
    
    // Subscript operator
    int operator[](int index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        throw out_of_range("Invalid index");
    }
    
    // Stream operators (must be friend or free functions)
    friend ostream& operator<<(ostream& os, const Vector& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
    
    friend istream& operator>>(istream& is, Vector& v) {
        is >> v.x >> v.y;
        return is;
    }
};

int main() {
    Vector v1(3, 4);
    Vector v2(1, 2);
    
    cout << "v1 = " << v1 << "\n";
    cout << "v2 = " << v2 << "\n";
    
    Vector v3 = v1 + v2;
    cout << "v1 + v2 = " << v3 << "\n";
    
    Vector v4 = v1 - v2;
    cout << "v1 - v2 = " << v4 << "\n";
    
    Vector v5 = v1 * 2;
    cout << "v1 * 2 = " << v5 << "\n";
    
    cout << "v1 == v2: " << (v1 == v2 ? "true" : "false") << "\n";
    
    cout << "-v1 = " << (-v1) << "\n";
    
    cout << "v1[0] = " << v1[0] << ", v1[1] = " << v1[1] << "\n";
    
    Vector v6 = v1;
    cout << "After v6 = v1: v6 = " << v6 << "\n";
    
    return 0;
}
```

***



#### S - Single Responsibility Principle

```cpp
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// BAD: Class doing multiple things
// class User {
//     void save() { }      // Saving logic
//     void sendEmail() { } // Sending email
//     void validate() { }  // Validation
// };

// GOOD: Each class has one responsibility
class User {
private:
    string name, email;
    
public:
    User(string n, string e) : name(n), email(e) {}
    string getName() { return name; }
    string getEmail() { return email; }
};

class UserValidator {
public:
    bool isValid(const User& user) {
        return !user.getName().empty() && 
               !user.getEmail().empty();
    }
};

class UserRepository {
public:
    void save(const User& user) {
        // Save to database or file
        cout << "Saving user: " << user.getName() << "\n";
    }
};

class EmailService {
public:
    void sendWelcomeEmail(const User& user) {
        cout << "Sending welcome email to: " << user.getEmail() << "\n";
    }
};

int main() {
    User user("Alice", "alice@example.com");
    UserValidator validator;
    UserRepository repo;
    EmailService emailService;
    
    if (validator.isValid(user)) {
        repo.save(user);
        emailService.sendWelcomeEmail(user);
    }
    
    return 0;
}
```



#### O - Open/Closed Principle

```cpp
#include <iostream>
#include <vector>
using namespace std;

// GOOD: Open for extension, closed for modification
class Shape {
public:
    virtual double area() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() override {
        return 3.14 * radius * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() override {
        return width * height;
    }
};

// New shape can be added without modifying existing code
class Triangle : public Shape {
private:
    double base, height;
public:
    Triangle(double b, double h) : base(b), height(h) {}
    double area() override {
        return 0.5 * base * height;
    }
};

class AreaCalculator {
public:
    double totalArea(vector<Shape*> shapes) {
        double total = 0;
        for (Shape* shape : shapes) {
            total += shape->area();
        }
        return total;
    }
};

int main() {
    Circle c(5);
    Rectangle r(4, 6);
    Triangle t(3, 4);
    
    vector<Shape*> shapes = {&c, &r, &t};
    AreaCalculator calc;
    
    cout << "Total area: " << calc.totalArea(shapes) << "\n";
    
    return 0;
}
```



#### I - Interface Segregation Principle

```cpp
#include <iostream>
using namespace std;

// BAD: Worker interface too bloated
// class Worker {
//     virtual void work() = 0;
//     virtual void eat() = 0;
//     virtual void sleep() = 0;
// };

// GOOD: Segregated interfaces
class Workable {
public:
    virtual void work() = 0;
    virtual ~Workable() {}
};

class Eatable {
public:
    virtual void eat() = 0;
    virtual ~Eatable() {}
};

class Sleepable {
public:
    virtual void sleep() = 0;
    virtual ~Sleepable() {}
};

class Human : public Workable, public Eatable, public Sleepable {
public:
    void work() override {
        cout << "Human working\n";
    }
    void eat() override {
        cout << "Human eating\n";
    }
    void sleep() override {
        cout << "Human sleeping\n";
    }
};

class Robot : public Workable {
public:
    void work() override {
        cout << "Robot working\n";
    }
};

int main() {
    Human human;
    human.work();
    human.eat();
    human.sleep();
    
    Robot robot;
    robot.work();
    // robot.eat();  // Robot doesn't have eat, which is correct!
    
    return 0;
}
```



#### D - Dependency Inversion Principle

```cpp
#include <iostream>
#include <memory>
using namespace std;

// Abstraction
class DataStore {
public:
    virtual void save(const string& data) = 0;
    virtual ~DataStore() {}
};

// Concrete implementations
class DatabaseStore : public DataStore {
public:
    void save(const string& data) override {
        cout << "Saving to database: " << data << "\n";
    }
};

class FileStore : public DataStore {
public:
    void save(const string& data) override {
        cout << "Saving to file: " << data << "\n";
    }
};

// High-level module depends on abstraction, not concrete class
class UserService {
private:
    shared_ptr<DataStore> dataStore;
    
public:
    UserService(shared_ptr<DataStore> store) : dataStore(store) {}
    
    void registerUser(const string& username) {
        cout << "Registering user: " << username << "\n";
        dataStore->save(username);
    }
};

int main() {
    // Can easily switch implementations
    auto dbStore = make_shared<DatabaseStore>();
    UserService service1(dbStore);
    service1.registerUser("Alice");
    
    cout << "\n";
    
    auto fileStore = make_shared<FileStore>();
    UserService service2(fileStore);
    service2.registerUser("Bob");
    
    return 0;
}
```
***



#### Constructors (C++98)

```cpp
#include <iostream>
#include <string>

class Car {
private:
    std::string brand;
    int year;
public:
    // Default constructor (no parameters)
    Car() : brand("Unknown"), year(2000) {
        std::cout << "Default constructor called\n";
    }
    
    // Parameterized constructor
    Car(const std::string& b, int y) : brand(b), year(y) {
        std::cout << "Constructor called\n";
    }
    
    // Copy constructor
    Car(const Car& other) : brand(other.brand), year(other.year) {
        std::cout << "Copy constructor called\n";
    }
    
    void show() const {
        std::cout << brand << " (" << year << ")\n";
    }
};

int main() {
    Car c1;                        // Calls default
    Car c2("Toyota", 2020);        // Calls parameterized
    Car c3 = c2;                   // Calls copy
    
    c1.show();
    c2.show();
    c3.show();
    
    return 0;
}
```



##### Inheriting Constructors

Using `using` to expose base class constructors.

```cpp
class Base {
public:
    Base(int x) { std::cout << "Base(int)\n"; }
};

class Derived : public Base {
public:
    using Base::Base; // Inherits Base(int)
    // Implicitly generates Derived(int x) : Base(x) {}
};
```



#### 2.3 Construction & Destruction Order

The order is strict and deterministic (Stack logic: LIFO).

**Construction Order:**
1.  Base Classes (in order of inheritance)
2.  Member Objects (in order of declaration in class)
3.  Constructor Body

**Destruction Order:**
1.  Destructor Body
2.  Member Objects (reverse order of declaration)
3.  Base Classes (reverse order of inheritance)

```cpp
class Base { public: Base() { cout << "Base "; } ~Base() { cout << "~Base "; } };
class Member { public: Member() { cout << "Member "; } ~Member() { cout << "~Member "; } };

class Derived : public Base {
    Member m;
public:
    Derived() { cout << "Derived "; }
    ~Derived() { cout << "~Derived "; }
};

int main() {
    Derived d; // Output: Base Member Derived
    // End of scope: ~Derived ~Member ~Base
}
```



#### 2.4 The Rule of Three, Five, and Zero

This is the cornerstone of resource management.

1.  **Rule of Three (C++98)**: If you implement one of: Destructor, Copy Constructor, Copy Assignment Operator; you likely need to implement all three.
2.  **Rule of Five (C++11)**: For Move semantics, add Move Constructor and Move Assignment Operator.
3.  **Rule of Zero**: If your class uses RAII types (`std::string`, `std::vector`, `std::unique_ptr`), do **NOT** declare any of the special member functions. Let the compiler generate them.

```cpp
// Rule of Zero Example (Best Practice)
class User {
    std::string name; // Manages its own memory
    std::vector<int> scores; // Manages its own memory
    // No destructor needed!
};
```

***



#### Basic Inheritance (C++98)

```cpp
#include <iostream>
#include <string>

// Base class
class Animal {
protected:
    std::string name;
public:
    Animal(const std::string& n) : name(n) {}
    
    virtual void speak() {  // virtual allows override
        std::cout << name << " makes a sound\n";
    }
    
    virtual ~Animal() {}
};

// Derived class
class Dog : public Animal {
public:
    Dog(const std::string& n) : Animal(n) {}
    
    void speak() override {  // override keyword (C++11)
        std::cout << name << " barks\n";
    }
};

class Cat : public Animal {
public:
    Cat(const std::string& n) : Animal(n) {}
    
    void speak() override {
        std::cout << name << " meows\n";
    }
};

int main() {
    Dog dog("Rex");
    Cat cat("Whiskers");
    
    dog.speak();
    cat.speak();
    
    // Polymorphism
    Animal* animals[2] = {&dog, &cat};
    for (int i = 0; i < 2; i++) {
        animals[i]->speak();
    }
    
    return 0;
}
```



#### Virtual Functions (C++98)

```cpp
#include <iostream>
#include <vector>
#include <memory>

class Shape {
public:
    virtual void draw() = 0;  // Pure virtual (abstract)
    virtual double area() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        std::cout << "Drawing circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        std::cout << "Drawing rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
};

int main() {
    std::vector<Shape*> shapes;
    shapes.push_back(new Circle(5));
    shapes.push_back(new Rectangle(4, 6));
    
    for (Shape* s : shapes) {
        s->draw();
        std::cout << "Area: " << s->area() << "\n";
    }
    
    // Cleanup
    for (Shape* s : shapes) {
        delete s;
    }
    
    return 0;
}
```



##### 1. Virtual Destructors (CRITICAL)

If you delete a derived object through a base pointer, the base destructor must be virtual. Otherwise, the derived destructor is **never called**, causing memory leaks.

```cpp
class Base {
public:
    // virtual ~Base() {} // Correct
    ~Base() {} // Dangerous!
};

class Derived : public Base {
    int* ptr;
public:
    Derived() { ptr = new int[100]; }
    ~Derived() { delete[] ptr; }
};

Base* b = new Derived();
delete b; // If ~Base is not virtual, ~Derived is NOT called! Leak!
```



##### 2. Covariant Return Types

An override can return a pointer/reference to a *derived* class, not just the base.

```cpp
class Shape {
public:
    virtual Shape* clone() = 0;
};

class Circle : public Shape {
public:
    // Returns Circle* instead of Shape* - Valid!
    Circle* clone() override { return new Circle(*this); }
};
```



##### 3. RTTI & dynamic_cast

Run-Time Type Information allows safe downcasting. It uses the `vptr` to check the actual type.

```cpp
Shape* s = new Circle(5);

// Safe cast: returns nullptr if s is not a Circle
if (Circle* c = dynamic_cast<Circle*>(s)) {
    c->special_circle_method();
} else {
    std::cout << "Not a circle\n";
}
```



##### 4. Static Polymorphism (CRTP)

Curiously Recurring Template Pattern. Faster than virtual functions (compile-time resolution).

```cpp
template<typename Derived>
class Shape {
public:
    void draw() {
        // Compile-time dispatch
        static_cast<Derived*>(this)->draw_impl();
    }
};

class Circle : public Shape<Circle> {
public:
    void draw_impl() { cout << "Circle\n"; }
};
```

***



## Project structure

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)



## Find dependencies

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)



## src/CMakeLists.txt - Main library

add_library(mylib
    domain/user.cpp
    domain/order.cpp
    application/user_service.cpp
    infrastructure/user_repository.cpp
)

target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(mylib
    PUBLIC Boost::system
    PRIVATE Boost::thread
)



## tests/CMakeLists.txt - Test suite

add_executable(tests
    test_user_service.cpp
    test_user_repository.cpp
)

target_link_libraries(tests
    PRIVATE mylib Catch2::Catch2WithMain
)

add_test(NAME AllTests COMMAND tests)



### 2.2 Header Organization

```cpp
// include/mylib/version.h
#ifndef MYLIB_VERSION_H
#define MYLIB_VERSION_H

#define MYLIB_VERSION_MAJOR 1
#define MYLIB_VERSION_MINOR 0
#define MYLIB_VERSION_PATCH 0

namespace mylib {
    struct Version {
        static constexpr int major = MYLIB_VERSION_MAJOR;
        static constexpr int minor = MYLIB_VERSION_MINOR;
        static constexpr int patch = MYLIB_VERSION_PATCH;
    };
}

#endif
// include/mylib/mylib.h - Main header
#ifndef MYLIB_H
#define MYLIB_H

// Version
#include "mylib/version.h"

// Core components
#include "mylib/domain/user.h"
#include "mylib/domain/order.h"

// Services
#include "mylib/application/user_service.h"
#include "mylib/application/order_service.h"

// Infrastructure
#include "mylib/infrastructure/database.h"

// Re-export main classes
namespace mylib {
    using domain::User;
    using domain::Order;
    using application::UserService;
}

#endif
```


### 2.3 Modern CMake with Modules (C++20)

Using C++20 Modules requires CMake 3.28+.


## Library with modules

add_library(math_engine)
target_sources(math_engine
    PUBLIC
        FILE_SET CXX_MODULES FILES
            src/math.cppm
            src/vector.cppm
)



## Executable consuming modules

add_executable(app main.cpp)
target_link_libraries(app PRIVATE math_engine)
```

***

```

## BUILD SYSTEMS & COMPILATION

### 3.1 Conan Package Manager


## conanfile.txt

[requires]
boost/1.81.0
fmt/9.1.0
nlohmann_json/3.11.2
catch2/3.3.2

[generators]
CMakeDeps
CMakeToolchain

[options]
boost/*:shared=False



## conanfile.py - Advanced

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake

class MyProjectConan(ConanFile):
    name = "myproject"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    requires = "boost/1.81.0", "fmt/9.1.0"
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()



### 3.2 Incremental Build Optimization


## Enable ccache for faster rebuilds

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()



## Unity builds for faster compilation

set_target_properties(mylib PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 8
)



## CMakeLists.txt - AddressSanitizer configuration

option(ENABLE_SANITIZER "Enable AddressSanitizer" OFF)

if(ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()
```

***

```

## VERSION CONTROL & COLLABORATION

### 6.1 Git Workflow


## .gitignore - Standard C++ project

build/
dist/
*.o
*.a
*.so
*.exe
.vscode/
.idea/
*.swp
CMakeLists.txt.user
*.qbs.user



## Git configuration - .git/config

[user]
    name = Developer
    email = dev@company.com

[core]
    editor = vim
    autocrlf = input

[pull]
    rebase = true

[branch]
    autosetuprebase = always



### 6.2 Feature Branch Workflow


## Main branch is production-ready

git checkout main
git pull origin main



## Create feature branch

git checkout -b feature/user-authentication



## Commit with conventional commits

git add src/
git commit -m "feat: implement JWT authentication"



## Create PR and request review

git push origin feature/user-authentication
```

***

```

## DOCUMENTATION & KNOWLEDGE TRANSFER

### 7.1 Code Documentation with Doxygen

```cpp
/**
 * @file user_service.h
 * @brief User service implementation for managing user lifecycle
 * @author John Doe
 * @version 1.0.0
 * @date 2024-01-15
 */

namespace application {
    /**
     * @class UserService
     * @brief Service for managing user operations
     * 
     * UserService provides business logic for user management including
     * creation, deletion, and modification. It validates input and
     * persists data through the UserRepository.
     * 
     * @example
     * @code
     * UserService service(repository);
     * auto user = service.create("John Doe", "john@example.com");
     * @endcode
     */
    class UserService {
    public:
        /**
         * @brief Creates a new user
         * 
         * @param name The user's full name
         * @param email The user's email address
         * 
         * @return User The created user object with assigned ID
         * 
         * @throws InvalidNameException if name is empty
         * @throws InvalidEmailException if email is invalid
         * @throws DuplicateEmailException if email already exists
         * 
         * @complexity O(n) where n is the number of existing users
         */
        User create(const std::string& name, const std::string& email);
        
        /**
         * @brief Updates user information
         * 
         * @param id User ID
         * @param name New name
         * @param email New email
         * 
         * @return User Updated user object
         * 
         * @throws UserNotFoundException if user doesn't exist
         * @throws InvalidEmailException if email is invalid
         */
        User update(int id, const std::string& name, const std::string& email);
    };
}
```




### 7.2 Architecture Decision Records (ADR)


### Alternatives Considered

- Hexagonal (Ports & Adapters) - More complex, chosen layered for simplicity
- Microservices - Future consideration for scalability
```

***

```

## SECURITY & SAFETY

### 8.1 Input Validation

```cpp
#include <regex>
#include <stdexcept>

namespace security {
    class InputValidator {
    public:
        static void validate_email(const std::string& email) {
            static const std::regex email_regex(
                R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
            );
            
            if (!std::regex_match(email, email_regex)) {
                throw std::invalid_argument("Invalid email format");
            }
        }
        
        static void validate_length(const std::string& str, 
                                   size_t min_length, size_t max_length) {
            if (str.length() < min_length || str.length() > max_length) {
                throw std::invalid_argument(
                    std::format("String length must be between {} and {}", 
                               min_length, max_length)
                );
            }
        }
        
        static void validate_integer_range(int value, int min, int max) {
            if (value < min || value > max) {
                throw std::out_of_range(
                    std::format("Value must be between {} and {}", min, max)
                );
            }
        }
    };
}
```


### 8.2 SQL Injection Prevention

```cpp
#include <sqlite3.h>

namespace database {
    class SafeQuery {
    private:
        sqlite3* db;
        
    public:
        std::vector<User> find_by_email_safe(const std::string& email) {
            const char* sql = "SELECT * FROM users WHERE email = ?";
            sqlite3_stmt* stmt;
            
            // Prepare statement (prevents SQL injection)
            int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                throw std::runtime_error("SQL error");
            }
            
            // Bind parameters safely
            sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
            
            std::vector<User> results;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                // Extract results
                int id = sqlite3_column_int(stmt, 0);
                const char* name = (const char*)sqlite3_column_text(stmt, 1);
                
                results.emplace_back(id, name, email);
            }
            
            sqlite3_finalize(stmt);
            return results;
        }
    };
}
```


***

## PERFORMANCE ENGINEERING

### 9.1 Benchmarking Framework

```cpp
#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = "hello world";
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StringCreation);

static void BM_VectorOperations(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < 1000; i++) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_VectorOperations);

BENCHMARK_MAIN();
```


### 9.2 Load Testing

```cpp
class LoadTester {
public:
    struct Result {
        int total_requests;
        std::chrono::milliseconds total_time;
        double requests_per_second;
        double avg_latency_ms;
        double p99_latency_ms;
    };
    
    Result run_load_test(
        std::function<void()> operation,
        int num_threads,
        int requests_per_thread
    ) {
        std::vector<std::thread> threads;
        std::vector<std::chrono::nanoseconds> latencies;
        std::mutex latencies_mutex;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int t = 0; t < num_threads; t++) {
            threads.emplace_back([&, t] {
                for (int r = 0; r < requests_per_thread; r++) {
                    auto op_start = std::chrono::high_resolution_clock::now();
                    operation();
                    auto op_end = std::chrono::high_resolution_clock::now();
                    
                    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        op_end - op_start
                    );
                    
                    {
                        std::lock_guard lock(latencies_mutex);
                        latencies.push_back(latency);
                    }
                }
            });
        }
        
        for (auto& t : threads) t.join();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Calculate percentiles
        std::sort(latencies.begin(), latencies.end());
        int p99_idx = (latencies.size() * 99) / 100;
        
        Result result;
        result.total_requests = num_threads * requests_per_thread;
        result.total_time = total_time;
        result.requests_per_second = result.total_requests / (total_time.count() / 1000.0);
        
        long long sum = 0;
        for (auto l : latencies) sum += l.count();
        result.avg_latency_ms = (sum / latencies.size()) / 1e6;
        result.p99_latency_ms = latencies[p99_idx].count() / 1e6;
        
        return result;
    }
};
```


***

## ERROR HANDLING & RECOVERY

### 10.1 Exception Hierarchy

```cpp
#include <stdexcept>

namespace application {
    // Base exception
    class ApplicationError : public std::runtime_error {
    protected:
        int error_code;
        std::string error_context;
        
    public:
        ApplicationError(const std::string& msg, int code = -1)
            : std::runtime_error(msg), error_code(code) {}
        
        int get_error_code() const { return error_code; }
        void set_context(const std::string& ctx) { error_context = ctx; }
    };
    
    // Domain exceptions
    class DomainError : public ApplicationError {
    public:
        DomainError(const std::string& msg) 
            : ApplicationError(msg, 1001) {}
    };
    
    class ValidationError : public DomainError {
    public:
        ValidationError(const std::string& msg) 
            : DomainError("Validation: " + msg) {}
    };
    
    class InvalidEmailException : public ValidationError {
    public:
        InvalidEmailException(const std::string& email)
            : ValidationError("Invalid email: " + email) {}
    };
    
    // Repository exceptions
    class RepositoryError : public ApplicationError {
    public:
        RepositoryError(const std::string& msg) 
            : ApplicationError(msg, 2001) {}
    };
    
    class UserNotFoundException : public RepositoryError {
    public:
        UserNotFoundException(int id)
            : RepositoryError("User not found: " + std::to_string(id)) {}
    };
}
```


### 10.2 Error Recovery Patterns

```cpp
class RetryPolicy {
public:
    struct Config {
        int max_retries = 3;
        std::chrono::milliseconds initial_delay{100};
        double backoff_multiplier = 2.0;
        int max_delay_ms = 10000;
    };
    
    template<typename F>
    static auto execute_with_retry(F operation, const Config& config)
        -> std::invoke_result_t<F> {
        
        std::exception_ptr last_exception;
        auto delay = config.initial_delay;
        
        for (int attempt = 0; attempt <= config.max_retries; attempt++) {
            try {
                return operation();
            } catch (const std::exception& e) {
                last_exception = std::current_exception();
                
                if (attempt < config.max_retries) {
                    LOG_INFO("Retry attempt {} after {}ms", 
                            attempt + 1, delay.count());
                    std::this_thread::sleep_for(delay);
                    
                    delay = std::chrono::milliseconds(
                        std::min(static_cast<int>(delay.count() * config.backoff_multiplier),
                                config.max_delay_ms)
                    );
                }
            }
        }
        
        std::rethrow_exception(last_exception);
    }
};

// Usage
auto user = RetryPolicy::execute_with_retry(
    [&]() { return repository.find_user(id); },
    RetryPolicy::Config{.max_retries = 5}
);
```


***

## DEPLOYMENT & DEVOPS

### 11.1 Docker Containerization


## Dockerfile - Multi-stage build

FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    libboost-all-dev

WORKDIR /build
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)
RUN cmake --install build --prefix /install



## Runtime stage

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    ca-certificates

COPY --from=builder /install /usr/local

EXPOSE 8080
CMD ["/usr/local/bin/myapp"]



### 11.2 Kubernetes Deployment


## deployment.yaml

apiVersion: apps/v1
kind: Deployment
metadata:
  name: myapp
  labels:
    app: myapp
spec:
  replicas: 3
  selector:
    matchLabels:
      app: myapp
  template:
    metadata:
      labels:
        app: myapp
    spec:
      containers:
      - name: myapp
        image: myapp:1.0.0
        ports:
        - containerPort: 8080
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 5



### 11.3 CI/CD Pipeline (GitHub Actions)

Automate building and testing.

```yaml
name: C++ CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install Dependencies
      run: sudo apt-get install -y libboost-dev cmake
      
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      
    - name: Build
      run: cmake --build build
      
    - name: Test
      run: cd build && ctest --output-on-failure
```

***

## CODE REVIEW & QUALITY

### 12.1 Code Review Checklist


### Code Quality

- [ ] Variable/function names are clear
- [ ] Code is DRY (Don't Repeat Yourself)
- [ ] Functions are appropriately sized
- [ ] Complexity is reasonable



## CMakeLists.txt - Clang-Tidy integration

find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}"
        "-checks=*"
        "-header-filter=.*"
        "-fix"
    )
endif()



## Cppcheck integration

find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK}
            --enable=all
            --suppress=missingIncludeSystem
            ${CMAKE_SOURCE_DIR}/src
    )
endif()
```

***

```

## TECHNICAL DEBT MANAGEMENT

### 13.1 Tracking Technical Debt

```cpp
// Technical debt marker
namespace technical_debt {
    /**
     * @deprecated Refactor this when performance is not critical.
     * Use optimized_algorithm_v2 instead.
     * 
     * @todo Refactor by Q2 2024
     * @complexity O(n²) - needs optimization
     */
    void inefficient_sort(std::vector<int>& data) {
        // Bubble sort - inefficient but simple
        for (size_t i = 0; i < data.size(); i++) {
            for (size_t j = 0; j < data.size() - 1; j++) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
    
    /**
     * @deprecated Temporary workaround for issue #123.
     * Remove when backend API is fixed.
     * @todo Track: https://github.com/team/project/issues/123
     */
    std::string get_user_name(int id) {
        // Workaround: return hardcoded values until backend is fixed
        static const std::map<int, std::string> workaround{
            {1, "John Doe"},
            {2, "Jane Smith"}
        };
        return workaround.at(id);
    }
}
```




### 13.2 Refactoring Strategy

```cpp
// Old code
class User {
public:
    void save_to_db(const std::string& connection_string) {
        // Direct database access - tightly coupled
    }
    
    void send_email(const std::string& subject, const std::string& body) {
        // Email sending logic mixed with domain logic
    }
};

// Refactored code
class User {
private:
    int id;
    std::string name;
    std::string email;
    
public:
    int get_id() const { return id; }
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};

// Separated concerns
class UserRepository {
public:
    void save(const User& user);
};

class UserEmailService {
public:
    void send_notification(const User& user, const std::string& message);
};
```


***


## LEGACY CODE MODERNIZATION

### 14.1 Incremental Modernization

```cpp
// Legacy code (C++98 style)
class LegacyUser {
public:
    char* name;      // Raw pointer
    char* email;     // Raw pointer
    
    LegacyUser(const char* n, const char* e) {
        name = new char[strlen(n) + 1];
        strcpy(name, n);  // Unsafe
        email = new char[strlen(e) + 1];
        strcpy(email, e);  // Unsafe
    }
    
    ~LegacyUser() {
        delete[] name;
        delete[] email;
    }
};

// Step 1: Add modern wrapper
class ModernUserWrapper {
private:
    LegacyUser* legacy;
    
public:
    ModernUserWrapper(const std::string& name, const std::string& email) {
        legacy = new LegacyUser(name.c_str(), email.c_str());
    }
    
    ~ModernUserWrapper() { delete legacy; }
    
    std::string_view get_name() const { return legacy->name; }
    std::string_view get_email() const { return legacy->email; }
};

// Step 2: Full modernization
class ModernUser {
private:
    std::string name;
    std::string email;
    
public:
    ModernUser(std::string n, std::string e)
        : name(std::move(n)), email(std::move(e)) {}
    
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};
```


***


## LEADERSHIP & TEAM MANAGEMENT

### 15.1 Mentoring Framework


#### Junior Developer (0-1 year)

- Focus: Understanding fundamentals
- Guidance: Pair programming, code reviews, architecture training
- Goals: Contribute to features, improve C++ knowledge



#### Mid-Level Developer (1-3 years)

- Focus: Mastering patterns and best practices
- Guidance: Design reviews, mentoring juniors, taking ownership
- Goals: Lead features, improve design decisions



#### Senior Developer (3+ years)

- Focus: Architecture, leadership, knowledge sharing
- Guidance: Strategic decisions, cross-team collaboration
- Goals: Shape team direction, mentor other seniors



### Mentoring Actions

1. Code review with detailed feedback
2. Design discussions before implementation
3. Pair programming sessions
4. Knowledge sharing sessions
5. Challenge with progressively harder problems



### 15.2 Technical Decision Making


### Title

Brief description of the proposal



### Detailed Design

Technical approach and architecture


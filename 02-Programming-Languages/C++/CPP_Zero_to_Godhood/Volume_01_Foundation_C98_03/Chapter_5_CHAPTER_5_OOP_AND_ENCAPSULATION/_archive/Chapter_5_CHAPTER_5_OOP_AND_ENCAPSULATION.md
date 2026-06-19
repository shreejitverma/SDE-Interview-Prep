# CHAPTER 5: OOP AND ENCAPSULATION


# OBJECT-ORIENTED PROGRAMMING: ENCAPSULATION & DESIGN

Welcome to the world of objects. In the previous chapters, we were writing "Procedural" codeessentially a long list of instructions for the computer to follow. Now, were going to start thinking about **things**.

### The Blueprint vs. The House

Think of a **Class** as a **Blueprint** for a house. 
*   The blueprint isn't a house. You can't live in it, and it doesn't take up any space in Mem-City. 
*   It just describes *what* a house should have (windows, doors, rooms) and *what* it can do (open doors, turn on lights).

An **Object** is the actual **House** built from that blueprint. 
*   You can build 1,000 houses from a single blueprint. 
*   Each house has its own address in Mem-City, and each house can have different colored walls (data).

---

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

---

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

---
### Professional Notes: Overloading Mastery

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

---

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

---

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

---

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

---

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
- **Templates** allow code reuse for different types.
- **Function Templates** deduce types automatically.
- **Class Templates** require explicit type arguments.
- **Specialization** allows handling specific types differently.

```

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

---

# Professional Notes: Chapter 31: Pointers to members

Section 31.1: Pointers to static member functions
Section 31.2: Pointers to member functions
Section 31.3: Pointers to member variables
Section 31.4: Pointers to static member variables

# Professional Notes: Chapter 34: Classes/Structures

Section 34.1: Class basics
Section 34.2: Final classes and structs
Section 34.3: Access speciers
Section 34.4: Inheritance
Section 34.5: Friendship
Section 34.6: Virtual Inheritance
Section 34.7: Private inheritance: restricting base class interface
Section 34.8: Accessing class members
Section 34.9: Member Types and Aliases
Section 34.10: Nested Classes/Structures
Section 34.11: Unnamed struct/class
Section 34.12: Static class members
Section 34.13: Multiple Inheritance
Section 34.14: Non-static member functions

# Professional Notes: Chapter 19: Friend keyword

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
Access modiers do not alter friend semantics. Public, protected and private declarations of a friend are equivalent.
Friend declarations are not inherited. For example, if we subclass PrivateHolder:
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
Friend class declaration is not reexive. If classes need private access in both directions, both of them need friend
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

# Professional Notes: Chapter 31: Pointers to members

Section 31.1: Pointers to static member functions
A static member function is just like an ordinary C/C++ function, except with scope:
It is inside a class, so it needs its name decorated with the class name;
It has accessibility, with public, protected or private.
So, if you have access to the static member function and decorate it correctly, then you can point to the function
like any normal function outside a class:
typedef int Fn(int); // Fn is a type-of function that accepts an int and returns an int
// Note that MyFn() is of type 'Fn'
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
To dene the type of the pointer, you need to mention the base type, as well as the fact that it is inside a
class: int Class::*ptr;.
If you have a class or reference and want to use it with a pointer-to-member, you need to use the .* operator
(akin to the . operator).
If you have a pointer to a class and want to use it with a pointer-to-member, you need to use the ->*
operator (akin to the -> operator).
Section 31.4: Pointers to static member variables
A static member variable is just like an ordinary C/C++ variable, except with scope:
It is inside a class, so it needs its name decorated with the class name;
It has accessibility, with public, protected or private.
So, if you have access to the static member variable and decorate it correctly, then you can point to the variable
like any normal variable outside a class:
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

# Professional Notes: Chapter 34: Classes/Structures

Section 34.1: Class basics
A class is a user-dened type. A class is introduced with the class, struct or union keyword. In colloquial usage, the
term "class" usually refers only to non-union classes.
A class is a collection of class members, which can be:
member variables (also called "elds"),
member functions (also called "methods"),
member types or typedefs (e.g. "nested classes"),
member templates (of any kind: variable, function, class or alias template)
The class and struct keywords, called class keys, are largely interchangeable, except that the default access
specier for members and bases is "private" for a class declared with the class key and "public" for a class declared
with the struct or union key (cf. Access modiers).
For example, the following code snippets are identical:
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
Members of a class are accessed using dot-syntax.
my_vector.x = 10;
my_vector.y = my_vector.x + 1; // my_vector.y = 11;
my_vector.z = my_vector.y - 4; // my:vector.z = 7;
Section 34.2: Final classes and structs
Version  C++11
Deriving a class may be forbidden with final specier. Let's declare a nal class:
class A final {
};
Now any attempt to subclass it will cause a compilation error:
// Compilation error: cannot derive from final class:
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
Section 34.3: Access speciers
There are three keywords that act as access speciers. These limit the access to class members following the
specier, until another specier changes the access level again:
Keyword
public
Everyone has access
Description
protected Only the class itself, derived classes and friends have access
private Only the class itself and friends have access
When the type is dened using the class keyword, the default access specier is private, but if the type is dened
using the struct keyword, the default access specier is public:
struct MyStruct { int x; };
class MyClass { int x; };
MyStruct s;
s.x = 9; // well formed, because x is public
MyClass c;
c.x = 9; // ill-formed, because x is private
Access speciers are mostly used to limit access to internal elds and methods, and force the programmer to use a
specic interface, for example to force use of getters and setters instead of referencing a variable directly:
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
The public, protected, and private keywords can also be used to grant or limit access to base class subobjects.
See the Inheritance example.
Section 34.4: Inheritance
Classes/structs can have inheritance relations.
If a class/struct B inherits from a class/struct A, this means that B has as a parent A. We say that B is a derived
class/struct from A, and A is the base class/struct.
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
This was later rened into the Liskov Substitution Principle: public inheritance should only be used when/if an
instance of the derived class can be substituted for an instance of the base class under any possible circumstance
(and still make sense).
Private inheritance is typically said to embody a completely dierent relationship: "is implemented in terms of"
(sometimes called a "HAS-A" relationship). For example, a Stack class could inherit privately from a Vector class.
Private inheritance bears a much greater similarity to aggregation than to public inheritance.
Protected inheritance is almost never used, and there's no general agreement on what sort of relationship it
embodies.
Section 34.5: Friendship
The friend keyword is used to give other classes and functions access to private and protected members of the
class, even through they are dened outside the class`s scope.
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
}
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
10, 5
Animal height: 5
Section 34.6: Virtual Inheritance
When using inheritance, you can specify the virtual keyword:
struct A{};
struct B: public virtual A{};
When class B has virtual base A it means that A will reside in most derived class of inheritance tree, and thus that
most derived class is also responsible for initializing that virtual base:
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
If we un-comment /*A(88)*/ we won't get any error since C is now initializing it's indirect virtual base A.
Also note that when we're creating variable object, most derived class is C, so C is responsible for creating(calling
constructor of) A and thus value of A::member is 88, not 5 (as it would be if we were creating object of type B).
It is useful when solving the diamond problem.:
  A                                        A   A
 / \                                       |   |
B   C                                      B   C
 \ /                                        \ /
  D                                          D
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
Removing the comments resolves the ambiguity.
Section 34.7: Private inheritance: restricting base class
interface
Private inheritance is useful when it is required to restrict the public interface of the class:
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
This approach eciently prevents an access to the A public methods by casting to the A pointer or reference:
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
Instead, the -> operator is almost always used. It is a short-hand for rst dereferencing the pointer and then
accessing it. I.e. (*p).a is exactly the same as p->a.
The :: operator is the scope operator, used in the same manner as accessing a member of a namespace. This is
because a static class member is considered to be in that class' scope, but isn't considered a member of instances
of that class. The use of normal . and -> is also allowed for static members, despite them not being instance
members, for historical reasons; this is of use for writing generic code in templates, as the caller doesn't need to be
concerned with whether a given member function is static or non-static.
Section 34.9: Member Types and Aliases
A class or struct can also dene member type aliases, which are type aliases contained within, and treated as
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
As with normal type aliases, each member type alias is allowed to refer to any type dened or aliased before, but
not after, its denition. Likewise, a typedef outside the class denition can refer to any accessible typedefs within
the class denition, provided it comes after the class denition.
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
Member type aliases can be declared with any access level, and will respect the appropriate access modier.
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
friend declaration would need to be modied; as long as the helper class provides the same functionality, any code
that uses it as Something::MyHelper instead of specifying it by name will usually still work without any
modications. In this manner, we minimise the amount of code that needs to be modied when the underlying
implementation is changed, such that the type name only needs to be changed in one location.
This can also be combined with decltype, if one so desires.
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
us, due to decltype. This minimises the number of modications necessary when we want to change helper, which
minimises the risk of human error.
As with everything, however, this can be taken too far. If the typename is only used once or twice internally and
zero times externally, for example, there's no need to provide an alias for it. If it's used hundreds or thousands of
times throughout a project, or if it has a long enough name, then it can be useful to provide it as a typedef instead
of always using it in absolute terms. One must balance forwards compatibility and convenience with the amount of
unnecessary noise created.
This can also be used with template classes, to provide access to the template parameters from outside the class.
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
This was often used with types with multiple template parameters, to provide an alias that denes one or more of
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
A class or struct can also contain another class/struct denition inside itself, which is called a "nested class"; in
this situation, the containing class is referred to as the "enclosing class". The nested class denition is considered to
be a member of the enclosing class, but is otherwise separate.
struct Outer {
    struct Inner { };
};
From outside of the enclosing class, nested classes are accessed using the scope operator. From inside the
enclosing class, however, nested classes can be used without qualiers:
struct Outer {
    struct Inner { };
    Inner in;
};
// ...
Outer o;
Outer::Inner i = o.in;
As with a non-nested class/struct, member functions and static variables can be dened either within a nested
class, or in the enclosing namespace. However, they cannot be dened within the enclosing class, due to it being
considered to be a dierent class than the nested class.
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
As with non-nested classes, nested classes can be forward declared and dened later, provided they are dened
before being used directly.
class Outer {
    class Inner1;
    class Inner2;
    class Inner1 {};
    Inner1 in1;

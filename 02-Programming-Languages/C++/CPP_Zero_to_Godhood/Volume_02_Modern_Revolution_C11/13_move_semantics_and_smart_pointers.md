# Chapter 13: Move Semantics & Smart Pointers

### 1. The Move Revolution

In the early 2000s, C++ was starting to feel "heavy." If you had a `std::vector<std::string>` with 10,000 long strings and wanted to pass it to another function, you had two bad choices:
1.  **Pass by Pointer**: Fast, but dangerous. Who owns the memory?
2.  **Pass by Value**: Safe, but **Incredibly Slow**. C++ would spend 10ms "Cloning" all 10,000 strings, only to destroy the original set 1 microsecond later.

This was the **"Performance Tax"** of C++. C++11 finally abolished this tax with **Move Semantics**.

***

### Fireside Chat: The "Magic Box" of Rvalues

**Student**: "You said an Rvalue is like a temporary shipping box. But why do we need special syntax for it?"

**The Architect**: "Because the compiler needs your **Permission** to steal. If I see you holding a sandwich (**Lvalue**), I can't just take a bite. That's theft! But if I see a sandwich sitting in a trash can marked 'FREE' (**Rvalue**), I can take the whole thing. `std::move` is how you put the 'FREE' sign on your variables."

***

### 1.1 Understanding the Players: Lvalues vs. Rvalues

Think of your memory as a neighborhood:
*   **Lvalue**: A **House**. It has a permanent address, a name (like `x`), and it persists.
*   **Rvalue**: A **Shipping Box**. It’s temporary. It’s on the move. It’s about to be opened and discarded.

When you see `int x = 10;`:
*   `x` is an **Lvalue** (The house where the data lives).
*   `10` is an **Rvalue** (The temporary box used to deliver the number 10).

#### Rvalue References (`T&&`): The "Box Snatcher"

An rvalue reference is a special hook that lets you grab these temporary boxes before they are thrown away. It says: "Hey! Don't delete that box! I want to steal the contents!"

### 1.2 The Secret of `std::move`

**`std::move` does not move anything.** It is merely a **Shipping Label** (a cast to an rvalue reference).
*   It sticks a label on an **Lvalue** that says: "This house is now a shipping box. Feel free to steal the furniture."
*   The actual "move" happens inside the **Move Constructor** or **Move Assignment Operator**.

### 1.3 The Move Constructor & Assignment (The Heist)

Instead of copying data (slow), we steal pointers (fast).

```cpp
class BigData {
    int* buffer;
    size_t size;
public:
    // 1. Move Constructor
    BigData(BigData&& other) noexcept 
        : buffer(other.buffer), size(other.size) { // A. STEAL THE DATA

        // B. THE CRITICAL STEP: Set the victim to null!
        // If we don't do this, 'other' will delete our stolen buffer
        // when it goes out of scope (Double Free).
        other.buffer = nullptr;
        other.size = 0;
    }

    // 2. Move Assignment
    BigData& operator=(BigData&& other) noexcept {
        if (this != &other) {
            delete[] buffer;       // Free own resources
            buffer = other.buffer; // Steal resources
            size = other.size;
            other.buffer = nullptr;// Nullify source
            other.size = 0;
        }
        return *this;
    }
};
```

#### Why `noexcept` is Godhood Required

If your Move Constructor doesn't have `noexcept`, the STL (like `std::vector`) will often **refuse to use it**. If a move fails halfway through, the vector can't "undo" the move safely. It will revert to the slow "Copy" method just to be safe. **Always mark your moves `noexcept`.**

### 1.4 Complexity Optimization: O(n²) to O(n)

Moving a container is an **O(1)** operation (stealing a pointer), whereas copying is **O(n)**. In algorithms that logically copy containers multiple times (like generating a Collatz sequence or recursive string builders), move semantics can collapse the complexity from **O(n²)** to **O(n)**.

***


**Warning:** Do not use `b1` after moving from it.

***

## 2. Smart Pointers (RAII)

Manual `new`/`delete` is prone to leaks and ownership ambiguity. C++11 introduces a formal ownership model through smart pointers.

### 2.1 The Philosophy: Ownership as the Only Axis

The key difference between the standard smart pointers is **ownership**: unique ownership, shared ownership, and non-owning observation. If you internalize this one axis first, the rest of their behavior becomes much easier to reason about.

**Ownership Model:**
*   **Owning Pointer**: Responsible for eventually releasing a resource.
*   **Observing Pointer**: Allowed to look at an object but must not delete it.

| Type | Ownership | Copyable? | Main Use Case |
| :--- | :--- | :--- | :--- |
| `std::unique_ptr` | **Exclusive** | No (Move-only) | Default choice for single-owner resources. |
| `std::shared_ptr` | **Shared** | Yes | Multiple objects co-owning a resource. |
| `std::weak_ptr` | **None** | Yes | Observers; breaking reference cycles. |

***

### 2.2 std::unique_ptr (Exclusive Ownership)

A non-null `std::unique_ptr` exclusively owns what it points to. It cannot be copied; ownership transfers only through a move (e.g., `std::move()`).

It is the lightest smart pointer, introducing essentially zero overhead over raw pointers. It should be your **default choice** for resource management.

#### Example: Moving Ownership

```cpp
#include <memory>
#include <utility>

struct OrderBook {
    void reset() {}
};

std::unique_ptr<OrderBook> make_book() {
    return std::make_unique<OrderBook>(); // C++14 factory
}

int main() {
    auto p1 = make_book();          // p1 owns the object
    // auto p2 = p1;                // ERROR: cannot copy
    auto p2 = std::move(p1);        // Ownership transferred to p2; p1 is now null
    
    if (p2) p2->reset();
}
```

#### Key Features:

*   **Custom Deleters**: Supports custom cleanup logic (e.g., `SDL_FreeSurface`).
*   **Arrays**: Specialized as `std::unique_ptr<T[]>`.
*   **Preferred usage**: Use it when ownership is hierarchical and obvious (e.g., "Engine owns Strategy", "Session owns Socket").

***

### 2.3 std::shared_ptr (Shared Ownership)

`std::shared_ptr` implements shared ownership via **reference counting**. The managed object is destroyed only when the last owning `shared_ptr` is destroyed or reassigned.

#### How it works:

It uses a **Control Block** on the heap which stores:
1.  Strong reference count.
2.  Weak reference count.
3.  The actual pointer (or the object itself if using `make_shared`).
4.  Custom deleter/allocator state.

```cpp
#include <iostream>
#include <memory>

struct FeedHandler {
    int id{7};
};

int main() {
    auto sp1 = std::make_shared<FeedHandler>(); // Control block + object in 1 allocation
    auto sp2 = sp1;                             // Reference count increases to 2

    std::cout << sp1.use_count() << '\n';       // Prints: 2
    std::cout << sp2->id << '\n';
}
```

#### The Cost of Sharing:

*   **Size**: Double the size of a raw pointer (pointer to object + pointer to control block).
*   **Performance**: Atomic updates to reference counts on every copy/destruction.
*   **Guidance**: Use `shared_ptr` only when the lifetime is genuinely shared or indeterminate, not just because it feels "safer".

***

### 2.4 std::weak_ptr (Non-owning Observation)

A `std::weak_ptr` holds a non-owning reference to an object managed by `std::shared_ptr`. It does **not** increase the strong reference count.

To use it, you must "upgrade" it to a `shared_ptr` via `lock()`. If the object has already been deleted, `lock()` returns an empty `shared_ptr`.

```cpp
#include <iostream>
#include <memory>

struct Session {
    int seq{42};
};

int main() {
    auto sp = std::make_shared<Session>();
    std::weak_ptr<Session> wp = sp; // Observe without owning

    if (auto locked = wp.lock()) {  // Try to acquire temporary ownership
        std::cout << locked->seq << '\n';
    }

    sp.reset();                     // Last owner gone; object destroyed

    if (auto locked = wp.lock()) {
        std::cout << locked->seq << '\n';
    } else {
        std::cout << "Object expired\n";
    }
}
```

#### Primary Use Cases:

1.  **Observation**: Looking at an object without keeping it alive.
2.  **Breaking Cycles**: Preventing memory leaks in circular relationships. If `Parent` owns `Child` via `shared_ptr`, `Child` should refer to `Parent` via `weak_ptr`.

***

### 2.5 Design Rules for Smart Pointers

*   **Default to `unique_ptr`**: It is simpler, faster, and clearer.
*   **Upgrade to `shared_ptr` only when necessary**: When multiple objects have equal claim to a resource's lifetime.
*   **Use `weak_ptr` wherever you need access without ownership** or need to break a cycle.
*   **Avoid `new`**: Use `std::make_unique` (C++14) and `std::make_shared` (C++11) for exception safety and performance.
*   **Think in Ownership**: "Who owns this? Who merely uses it? Can two objects accidentally keep each other alive?"

***

### 2.6 Professional Patterns & Advanced Smart Pointers

#### Custom Deleters for C Interfaces

Many C interfaces have their own deletion functions (e.g., `fclose`, `SDL_FreeSurface`).
```cpp
// Unique ownership with a function pointer deleter
std::unique_ptr<FILE, int(*)(FILE*)> f(fopen("test.txt", "r"), fclose);

// Shared ownership with a lambda deleter
auto surf = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(...), SDL_FreeSurface);
```

#### `std::enable_shared_from_this<T>`

If you need a `shared_ptr` to `this` from inside a member function, your class must inherit from `std::enable_shared_from_this<T>`.
```cpp
class Widget : public std::enable_shared_from_this<Widget> {
public:
    void Register() {
        // Returns a shared_ptr that shares ownership with existing owners
        auto self = shared_from_this(); 
        EventManager::Add(self);
    }
};
```

#### Casting Smart Pointers

Use specialized casts to maintain ownership tracking:
*   `std::static_pointer_cast`
*   `std::dynamic_pointer_cast`
*   `std::const_pointer_cast`

***

## 3. Perfect Forwarding & Reference Collapsing

Used in templates to preserve the "value category" (lvalue vs rvalue) of arguments.

### 3.1 The Reference Collapsing Rules

*   `&`  + `&`   -> `&`
*   `&`  + `&&`  -> `&`
*   `&&` + `&`   -> `&`
*   `&&` + `&&` -> `&&`

**Analogy**: The "Lvalue" is like a "Black Hole." If an Lvalue (`&`) touches anything else, the whole thing becomes an Lvalue. The only way to stay an Rvalue (`&&`) is if both sides are Rvalues.

### 3.2 `std::forward`

`std::forward` passes the argument as an lvalue if it was given an lvalue, and as an rvalue if it was given an rvalue.

```cpp
template<typename T>
void wrapper(T&& arg) { // Universal Reference
    func(std::forward<T>(arg)); // Perfect Forwarding
}
```

***

## Professional Insights: The "Value Pointer" Pattern

A `value_ptr` (not in the standard library, but common in expert code) is a smart pointer that behaves like a value. When copied, it copies its contents (Deep Copy). This is useful for **pImpl** (Pointer to Implementation) patterns where you want value semantics but header-file isolation.

***





##### Smart Pointers

Section 33.1: Unique ownership (std::unique_ptr)
Version ≥ C++11
A std::unique_ptr is a class template that manages the lifetime of a dynamically stored object. Unlike for
std::shared_ptr, the dynamic object is owned by only one instance of a std::unique_ptr at any time,
// Creates a dynamic int with value of 20 owned by a unique pointer
```cpp
std::unique_ptr<int> ptr = std::make_unique<int>(20);
(Note: std::unique_ptr is available since C++11 and std::make_unique since C++14.)
Only the variable ptr holds a pointer to a dynamically allocated int. When a unique pointer that owns an object
goes out of scope, the owned object is deleted, i.e. its destructor is called if the object is of class type, and the
memory for that object is released.
To use std::unique_ptr and std::make_unique with array-types, use their array specializations:
// Creates a unique_ptr to an int with value 59
std::unique_ptr<int> ptr = std::make_unique<int>(59);
// Creates a unique_ptr to an array of 15 ints
std::unique_ptr<int[]> ptr = std::make_unique<int[]>(15);
You can access the std::unique_ptr just like a raw pointer, because it overloads those operators.
You can transfer ownership of the contents of a smart pointer to another pointer by using std::move, which will
cause the original smart pointer to point to nullptr.
// 1. std::unique_ptr
std::unique_ptr<int> ptr = std::make_unique<int>();
// Change value to 1
*ptr = 1;
// 2. std::unique_ptr (by moving 'ptr' to 'ptr2', 'ptr' doesn't own the object anymore)
std::unique_ptr<int> ptr2 = std::move(ptr);
int a = *ptr2; // 'a' is 1
int b = *ptr;  // undefined behavior! 'ptr' is 'nullptr'
               // (because of the move command above)
Passing unique_ptr to functions as parameter:
void foo(std::unique_ptr<int> ptr)
{
    // Your code goes here
}
std::unique_ptr<int> ptr = std::make_unique<int>(59);
foo(std::move(ptr))
Returning unique_ptr from functions. This is the preferred C++11 way of writing factory functions, as it clearly
conveys the ownership semantics of the return: the caller owns the resulting unique_ptr and is responsible for it.
std::unique_ptr<int> foo()
{
    std::unique_ptr<int> ptr = std::make_unique<int>(59);
    return ptr;
}
std::unique_ptr<int> ptr = foo();
Compare this to:
int* foo_cpp03();
int* p = foo_cpp03(); // do I own p? do I have to delete it at some point?
                      // it's not readily apparent what the answer is.
Version < C++14
The class template make_unique is provided since C++14. It's easy to add it manually to C++11 code:
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&&... args)
{ return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
// Use make_unique for arrays
template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(size_t n)
{ return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]()); }
Version ≥ C++11
Unlike the dumb smart pointer (std::auto_ptr), unique_ptr can also be instantiated with vector allocation (not
std::vector). Earlier examples were for scalar allocations. For example to have a dynamically allocated integer
array for 10 elements, you would specify int[] as the template type (and not just int):
std::unique_ptr<int[]> arr_ptr = std::make_unique<int[]>(10);
Which can be simpliﬁed with:
auto arr_ptr = std::make_unique<int[]>(10);
Now, you use arr_ptr as if it is an array:
arr_ptr[2] =  10; // Modify third element
You need not to worry about de-allocation. This template specialized version calls constructors and destructors
appropriately. Using vectored version of unique_ptr or a vector itself - is a personal choice.
In versions prior to C++11, std::auto_ptr was available. Unlike unique_ptr it is allowed to copy auto_ptrs, upon
which the source ptr will lose the ownership of the contained pointer and the target receives it.
Section 33.2: Sharing ownership (std::shared_ptr)
The class template std::shared_ptr deﬁnes a shared pointer that is able to share ownership of an object with
other shared pointers. This contrasts to std::unique_ptr which represents exclusive ownership.
The sharing behavior is implemented through a technique known as reference counting, where the number of
shared pointers that point to the object is stored alongside it. When this count reaches zero, either through the
destruction or reassignment of the last std::shared_ptr instance, the object is automatically destroyed.
// Creation: 'firstShared' is a shared pointer for a new instance of 'Foo'
std::shared_ptr<Foo> firstShared = std::make_shared<Foo>(/*args*/);
To create multiple smart pointers that share the same object, we need to create another shared_ptr that aliases
the ﬁrst shared pointer. Here are 2 ways of doing it:
std::shared_ptr<Foo> secondShared(firstShared);  // 1st way: Copy constructing
std::shared_ptr<Foo> secondShared;
secondShared = firstShared;                      // 2nd way: Assigning
Either of the above ways makes secondShared a shared pointer that shares ownership of our instance of Foo with
firstShared.
The smart pointer works just like a raw pointer. This means, you can use * to dereference them. The regular ->
operator works as well:
secondShared->test(); // Calls Foo::test()
```

Finally, when the last aliased shared_ptr goes out of scope, the destructor of our Foo instance is called.
Warning: Constructing a shared_ptr might throw a bad_alloc exception when extra data for shared ownership
semantics needs to be allocated. If the constructor is passed a regular pointer it assumes to own the object pointed
to and calls the deleter if an exception is thrown. This means shared_ptr<T>(new T(args)) will not leak a T object if
allocation of shared_ptr<T> fails. However, it is advisable to use make_shared<T>(args) or
allocate_shared<T>(alloc, args), which enable the implementation to optimize the memory allocation.
Allocating Arrays([]) using shared_ptr
Version ≥ C++11 Version < C++17
Unfortunately, there is no direct way to allocate Arrays using make_shared<>.
It is possible to create arrays for shared_ptr<> using new and std::default_delete.
For example, to allocate an array of 10 integers, we can write the code as
shared_ptr<int> sh(new int[10], std::default_delete<int[]>());
Specifying std::default_delete is mandatory here to make sure that the allocated memory is correctly cleaned up
using delete[].
If we know the size at compile time, we can do it this way:
```cpp
template<class Arr>
struct shared_array_maker {};
template<class T, std::size_t N>
struct shared_array_maker<T[N]> {
  std::shared_ptr<T> operator()const{
    auto r = std::make_shared<std::array<T,N>>();
    if (!r) return {};
    return {r.data(), r};
  }
};
template<class Arr>
auto make_shared_array()
-> decltype( shared_array_maker<Arr>{}() )
{ return shared_array_maker<Arr>{}(); }
then make_shared_array<int[10]> returns a shared_ptr<int> pointing to 10 ints all default constructed.
Version ≥ C++17
With C++17, shared_ptr gained special support for array types. It is no longer necessary to specify the array-deleter
explicitly, and the shared pointer can be dereferenced using the [] array index operator:
std::shared_ptr<int[]> sh(new int[10]);
sh[0] = 42;
Shared pointers can point to a sub-object of the object it owns:
struct Foo { int x; };
std::shared_ptr<Foo> p1 = std::make_shared<Foo>();
std::shared_ptr<int> p2(p1, &p1->x);
Both p2 and p1 own the object of type Foo, but p2 points to its int member x. This means that if p1 goes out of
scope or is reassigned, the underlying Foo object will still be alive, ensuring that p2 does not dangle.
Important: A shared_ptr only knows about itself and all other shared_ptr that were created with the alias
constructor. It does not know about any other pointers, including all other shared_ptrs created with a reference to
the same Foo instance:
Foo *foo = new Foo;
std::shared_ptr<Foo> shared1(foo);
std::shared_ptr<Foo> shared2(foo); // don't do this
shared1.reset(); // this will delete foo, since shared1
                 // was the only shared_ptr that owned it
shared2->test(); // UNDEFINED BEHAVIOR: shared2's foo has been
                 // deleted already!!
Ownership Transfer of shared_ptr
By default, shared_ptr increments the reference count and doesn't transfer the ownership. However, it can be
made to transfer the ownership using std::move:
shared_ptr<int> up = make_shared<int>();
// Transferring the ownership
shared_ptr<int> up2 = move(up);
// At this point, the reference count of up = 0 and the
// ownership of the pointer is solely with up2 with reference count = 1
Section 33.3: Sharing with temporary ownership
(std::weak_ptr)
Instances of std::weak_ptr can point to objects owned by instances of std::shared_ptr while only becoming
temporary owners themselves. This means that weak pointers do not alter the object's reference count and
therefore do not prevent an object's deletion if all of the object's shared pointers are reassigned or destroyed.
In the following example instances of std::weak_ptr are used so that the destruction of a tree object is not
inhibited:
```




### 2. UNIQUE_PTR (Exclusive Ownership)

`std::unique_ptr` represents exclusive ownership. An object can have only one `unique_ptr` pointing to it. When the `unique_ptr` is destroyed, the object is deleted.



### 3. SHARED_PTR (Shared Ownership)

`std::shared_ptr` allows multiple pointers to own the same resource. The resource is deleted only when the *last* `shared_ptr` is destroyed.



##### Value Categories

Section 74.1: Value Category Meanings
Expressions in C++ are assigned a particular value category, based on the result of those expressions. Value
categories for expressions can aﬀect C++ function overload resolution.
Value categories determines two important-but-separate properties about an expression. One property is whether
the expression has identity. An expression has identity if it refers to an object that has a variable name. The variable
name may not be involved in the expression, but the object can still have one.
The other property is whether it is legal to implicitly move from the expression's value. Or more speciﬁcally,
whether the expression, when used as a function parameter, will bind to r-value parameter types or not.
C++ deﬁnes 3 value categories which represent the useful combination of these properties: lvalue (expressions with
identity but not movable from), xvalue (expressions with identity that are moveable from), and prvalue (expressions
without identity that are moveable from). C++ does not have expressions which have no identity and cannot be
moved from.
C++ deﬁnes two other value categories, each based solely on one of these properties: glvalue (expressions with
identity) and rvalue (expressions that can be moved from). These act as useful groupings of the prior categories.
This graph serves as an illustration:
Section 74.2: rvalue
An rvalue expression is any expression which can be implicitly moved from, regardless of whether it has identity.
More precisely, rvalue expressions may be used as the argument to a function that takes a parameter of type T &&
(where T is the type of expr). Only rvalue expressions may be given as arguments to such function parameters; if a
non-rvalue expression is used, then overload resolution will pick any function that does not use an rvalue reference
parameter. And if none exist, then you get an error.
The category of rvalue expressions includes all xvalue and prvalue expressions, and only those expressions.
The standard library function std::move exists to explicitly transform a non-rvalue expression into an rvalue. More
speciﬁcally, it turns the expression into an xvalue, since even if it was an identity-less prvalue expression before, by
passing it as a parameter to std::move, it gains identity (the function's parameter name) and becomes an xvalue.
Consider the following:
```cpp
std::string str("init");                       //1
std::string test1(str);                        //2
std::string test2(std::move(str));             //3
str = std::string("new value");                //4
std::string &&str_ref = std::move(str);        //5
std::string test3(str_ref);                    //6
std::string has a constructor which takes a single parameter of type std::string&&, commonly called a "move
constructor". However, the value category of the expression str is not an rvalue (speciﬁcally it is an lvalue), so it
cannot call that constructor overload. Instead, it calls the const std::string& overload, the copy constructor.
Line 3 changes things. The return value of std::move is a T&&, where T is the base type of the parameter passed in.
So std::move(str) returns std::string&&. A function call who's return value is an rvalue reference is an rvalue
expression (speciﬁcally an xvalue), so it may call the move constructor of std::string. After line 3, str has been
moved from (who's contents are now undeﬁned).
Line 4 passes a temporary to the assignment operator of std::string. This has an overload which takes a
std::string&&. The expression std::string("new value") is an rvalue expression (speciﬁcally a prvalue), so it
may call that overload. Thus, the temporary is moved into str, replacing the undeﬁned contents with speciﬁc
contents.
Line 5 creates a named rvalue reference called str_ref that refers to str. This is where value categories get
confusing.
See, while str_ref is an rvalue reference to std::string, the value category of the expression str_ref is not an
rvalue. It is an lvalue expression. Yes, really. Because of this, one cannot call the move constructor of std::string
with the expression str_ref. Line 6 therefore copies the value of str into test3.
To move it, we would have to employ std::move again.
Section 74.3: xvalue
An xvalue (eXpiring value) expression is an expression which has identity and represents an object which can be
implicitly moved from. The general idea with xvalue expressions is that the object they represent is going to be
destroyed soon (hence the "eXpiring" part), and therefore implicitly moving from them is ﬁne.
Given:
struct X { int n; };
extern X x;
4;                   // prvalue: does not have an identity
x;                   // lvalue
x.n;                 // lvalue
std::move(x);        // xvalue
std::forward<X&>(x); // lvalue
X{4};                // prvalue: does not have an identity
X{4}.n;              // xvalue: does have an identity and denotes resources
                     // that can be reused
Section 74.4: prvalue
A prvalue (pure-rvalue) expression is an expression which lacks identity, whose evaluation is typically used to
initialize an object, and which can be implicitly moved from. These include, but are not limited to:
Expressions that represent temporary objects, such as std::string("123").
A function call expression that does not return a reference
A literal (except a string literal - those are lvalues), such has 1, true, 0.5f, or 'a'
A lambda expression
```

The built-in addressof operator (&) cannot be applied on these expressions.
Section 74.5: lvalue
An lvalue expression is an expression which has identity, but cannot be implicitly moved from. Among these are
expressions that consist of a variable name, function name, expressions that are built-in dereference operator uses
and expressions that refer to lvalue references.
The typical lvalue is simply a name, but lvalues can come in other ﬂavors as well:
```cpp
struct X { ... };
X x;         // x is an lvalue
X* px = &x;  // px is an lvalue
*px = X{};   // *px is also an lvalue, X{} is a prvalue
X* foo_ptr();  // foo_ptr() is a prvalue
X& foo_ref();  // foo_ref() is an lvalue
```

Additionally, while most literals (e.g. 4, 'x', etc.) are prvalues, string literals are lvalues.
Section 74.6: glvalue
A glvalue (a "generalized lvalue") expression is any expression which has identity, regardless of whether it can be
moved from or not. This category includes lvalues (expressions that have identity but can't be moved from) and
xvalues (expressions that have identity, and can be moved from), but excludes prvalues (expressions without
identity).
If an expression has a name, it's a glvalue:
```cpp
struct X { int n; };
X foo();
X x;
x; // has a name, so it's a glvalue
std::move(x); // has a name (we're moving from "x"), so it's a glvalue
              // can be moved from, so it's an xvalue not an lvalue
foo(); // has no name, so is a prvalue, not a glvalue
X{};   // temporary has no name, so is a prvalue, not a glvalue
X{}.n; // HAS a name, so is a glvalue. can be moved from, so it's an xvalue
```




##### Move Semantics

Section 106.1: Move semantics
Move semantics are a way of moving one object to another in C++. For this, we empty the old object and place
everything it had in the new object.
For this, we must understand what an rvalue reference is. An rvalue reference (T&& where T is the object type) is not
much diﬀerent than a normal reference (T&, now called lvalue references). But they act as 2 diﬀerent types, and so,
we can make constructors or functions that take one type or the other, which will be necessary when dealing with
move semantics.
The reason why we need two diﬀerent types is to specify two diﬀerent behaviors. Lvalue reference constructors are
related to copying, while rvalue reference constructors are related to moving.
To move an object, we will use std::move(obj). This function returns an rvalue reference to the object, so that we
can steal the data from that object into a new one. There are several ways of doing this which are discussed below.
Important to note is that the use of std::move creates just an rvalue reference. In other words the statement
std::move(obj) does not change the content of obj, while auto obj2 = std::move(obj) (possibly) does.
Section 106.2: Using std::move to reduce complexity from
O(n²) to O(n)
C++11 introduced core language and standard library support for moving an object. The idea is that when an
object o is a temporary and one wants a logical copy, then its safe to just pilfer o's resources, such as a dynamically
allocated buﬀer, leaving o logically empty but still destructible and copyable.
The core language support is mainly
the rvalue reference type builder &&, e.g., std::string&& is an rvalue reference to a std::string, indicating
that that referred to object is a temporary whose resources can just be pilfered (i.e. moved)
special support for a move constructor T( T&& ), which is supposed to eﬃciently move resources from the
speciﬁed other object, instead of actually copying those resources, and
special support for a move assignment operator auto operator=(T&&) -> T&, which also is supposed to
move from the source.
The standard library support is mainly the std::move function template from the <utility> header. This function
produces an rvalue reference to the speciﬁed object, indicating that it can be moved from, just as if it were a
temporary.
For a container actual copying is typically of O(n) complexity, where n is the number of items in the container, while
moving is O(1), constant time. And for an algorithm that logically copies that container n times, this can reduce the
complexity from the usually impractical O(n²) to just linear O(n).
In his article “Containers That Never Change” in Dr. Dobbs Journal in September 19 2013, Andrew Koenig presented
an interesting example of algorithmic ineﬃciency when using a style of programming where variables are
immutable after initialization. With this style loops are generally expressed using recursion. And for some
algorithms such as generating a Collatz sequence, the recursion requires logically copying a container:
// Based on an example by Andrew Koenig in his Dr. Dobbs Journal article
// “Containers That Never Change” September 19, 2013, available at
// <url: http://www.drdobbs.com/cpp/containters-that-never-change/240161543>
// Includes here, e.g. <vector>
namespace my {
```cpp
    template< class Item >
    using Vector_ = /* E.g. std::vector<Item> */;
    auto concat( Vector_<int> const& v, int const x )
        -> Vector_<int>
    {
        auto result{ v };
        result.push_back( x );
        return result;
    }
    auto collatz_aux( int const n, Vector_<int> const& result )
        -> Vector_<int>
    {
        if( n == 1 )
        {
            return result;
        }
        auto const new_result = concat( result, n );
        if( n % 2 == 0 )
        {
            return collatz_aux( n/2, new_result );
        }
        else
        {
            return collatz_aux( 3*n + 1, new_result );
        }
    }
    auto collatz( int const n )
        -> Vector_<int>
    {
        assert( n != 0 );
        return collatz_aux( n, Vector_<int>() );
    }
}  // namespace my
```




### 5. PERFECT FORWARDING

Used in templates to preserve the value category (lvalue vs rvalue) of arguments.



##### Perfect Forwarding

Section 101.1: Factory functions
Suppose we want to write a factory function that accepts an arbitrary list of arguments and passes those
arguments unmodiﬁed to another function. An example of such a function is make_unique, which is used to safely
construct a new instance of T and return a unique_ptr<T> that owns the instance.
The language rules regarding variadic templates and rvalue references allows us to write such a function.
```cpp
template<class T, class... A>
unique_ptr<T> make_unique(A&&... args)
{
    return unique_ptr<T>(new T(std::forward<A>(args)...));
}
The use of ellipses ... indicate a parameter pack, which represents an arbitrary number of types. The compiler will
expand this parameter pack to the correct number of arguments at the call site. These arguments are then passed
to T's constructor using std::forward. This function is required to preserve the ref-qualiﬁers of the arguments.
struct foo
{
    foo() {}
    foo(const foo&) {}                    // copy constructor
    foo(foo&&) {}                         // copy constructor
    foo(int, int, int) {}
};
foo f;
auto p1 = make_unique<foo>(f);            // calls foo::foo(const foo&)
auto p2 = make_unique<foo>(std::move(f)); // calls foo::foo(foo&&)
auto p3 = make_unique<foo>(1, 2, 3);
```




#### 19.2 Implementing my::shared_ptr

Understanding the Control Block.

```cpp
template<typename T>
class SharedPtr {
    T* ptr;
    struct ControlBlock {
        std::atomic<int> ref_count{1};
    } *cb;
    
public:
    SharedPtr(T* p) : ptr(p), cb(new ControlBlock()) {}
    
    SharedPtr(const SharedPtr& other) {
        ptr = other.ptr;
        cb = other.cb;
        if (cb) cb->ref_count++;
    }
    
    ~SharedPtr() {
        if (cb && --cb->ref_count == 0) {
            delete ptr;
            delete cb;
        }
    }
};
```

***



##### Smart Pointers

Section 33.1: Unique ownership (std::unique_ptr)
Version ≥ C++11
A std::unique_ptr is a class template that manages the lifetime of a dynamically stored object. Unlike for
std::shared_ptr, the dynamic object is owned by only one instance of a std::unique_ptr at any time,
// Creates a dynamic int with value of 20 owned by a unique pointer
```cpp
std::unique_ptr<int> ptr = std::make_unique<int>(20);
(Note: std::unique_ptr is available since C++11 and std::make_unique since C++14.)
Only the variable ptr holds a pointer to a dynamically allocated int. When a unique pointer that owns an object
goes out of scope, the owned object is deleted, i.e. its destructor is called if the object is of class type, and the
memory for that object is released.
To use std::unique_ptr and std::make_unique with array-types, use their array specializations:
// Creates a unique_ptr to an int with value 59
std::unique_ptr<int> ptr = std::make_unique<int>(59);
// Creates a unique_ptr to an array of 15 ints
std::unique_ptr<int[]> ptr = std::make_unique<int[]>(15);
You can access the std::unique_ptr just like a raw pointer, because it overloads those operators.
You can transfer ownership of the contents of a smart pointer to another pointer by using std::move, which will
cause the original smart pointer to point to nullptr.
// 1. std::unique_ptr
std::unique_ptr<int> ptr = std::make_unique<int>();
// Change value to 1
*ptr = 1;
// 2. std::unique_ptr (by moving 'ptr' to 'ptr2', 'ptr' doesn't own the object anymore)
std::unique_ptr<int> ptr2 = std::move(ptr);
int a = *ptr2; // 'a' is 1
int b = *ptr;  // undefined behavior! 'ptr' is 'nullptr'
               // (because of the move command above)
Passing unique_ptr to functions as parameter:
void foo(std::unique_ptr<int> ptr)
{
    // Your code goes here
}
std::unique_ptr<int> ptr = std::make_unique<int>(59);
foo(std::move(ptr))
Returning unique_ptr from functions. This is the preferred C++11 way of writing factory functions, as it clearly
conveys the ownership semantics of the return: the caller owns the resulting unique_ptr and is responsible for it.
std::unique_ptr<int> foo()
{
    std::unique_ptr<int> ptr = std::make_unique<int>(59);
    return ptr;
}
std::unique_ptr<int> ptr = foo();
Compare this to:
int* foo_cpp03();
int* p = foo_cpp03(); // do I own p? do I have to delete it at some point?
                      // it's not readily apparent what the answer is.
Version < C++14
The class template make_unique is provided since C++14. It's easy to add it manually to C++11 code:
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&&... args)
{ return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
// Use make_unique for arrays
template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(size_t n)
{ return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]()); }
Version ≥ C++11
Unlike the dumb smart pointer (std::auto_ptr), unique_ptr can also be instantiated with vector allocation (not
std::vector). Earlier examples were for scalar allocations. For example to have a dynamically allocated integer
array for 10 elements, you would specify int[] as the template type (and not just int):
std::unique_ptr<int[]> arr_ptr = std::make_unique<int[]>(10);
Which can be simpliﬁed with:
auto arr_ptr = std::make_unique<int[]>(10);
Now, you use arr_ptr as if it is an array:
arr_ptr[2] =  10; // Modify third element
You need not to worry about de-allocation. This template specialized version calls constructors and destructors
appropriately. Using vectored version of unique_ptr or a vector itself - is a personal choice.
In versions prior to C++11, std::auto_ptr was available. Unlike unique_ptr it is allowed to copy auto_ptrs, upon
which the source ptr will lose the ownership of the contained pointer and the target receives it.
Section 33.2: Sharing ownership (std::shared_ptr)
The class template std::shared_ptr deﬁnes a shared pointer that is able to share ownership of an object with
other shared pointers. This contrasts to std::unique_ptr which represents exclusive ownership.
The sharing behavior is implemented through a technique known as reference counting, where the number of
shared pointers that point to the object is stored alongside it. When this count reaches zero, either through the
destruction or reassignment of the last std::shared_ptr instance, the object is automatically destroyed.
// Creation: 'firstShared' is a shared pointer for a new instance of 'Foo'
std::shared_ptr<Foo> firstShared = std::make_shared<Foo>(/*args*/);
To create multiple smart pointers that share the same object, we need to create another shared_ptr that aliases
the ﬁrst shared pointer. Here are 2 ways of doing it:
std::shared_ptr<Foo> secondShared(firstShared);  // 1st way: Copy constructing
std::shared_ptr<Foo> secondShared;
secondShared = firstShared;                      // 2nd way: Assigning
Either of the above ways makes secondShared a shared pointer that shares ownership of our instance of Foo with
firstShared.
The smart pointer works just like a raw pointer. This means, you can use * to dereference them. The regular ->
operator works as well:
secondShared->test(); // Calls Foo::test()
```

Finally, when the last aliased shared_ptr goes out of scope, the destructor of our Foo instance is called.
Warning: Constructing a shared_ptr might throw a bad_alloc exception when extra data for shared ownership
semantics needs to be allocated. If the constructor is passed a regular pointer it assumes to own the object pointed
to and calls the deleter if an exception is thrown. This means shared_ptr<T>(new T(args)) will not leak a T object if
allocation of shared_ptr<T> fails. However, it is advisable to use make_shared<T>(args) or
allocate_shared<T>(alloc, args), which enable the implementation to optimize the memory allocation.
Allocating Arrays([]) using shared_ptr
Version ≥ C++11 Version < C++17
Unfortunately, there is no direct way to allocate Arrays using make_shared<>.
It is possible to create arrays for shared_ptr<> using new and std::default_delete.
For example, to allocate an array of 10 integers, we can write the code as
shared_ptr<int> sh(new int[10], std::default_delete<int[]>());
Specifying std::default_delete is mandatory here to make sure that the allocated memory is correctly cleaned up
using delete[].
If we know the size at compile time, we can do it this way:
```cpp
template<class Arr>
struct shared_array_maker {};
template<class T, std::size_t N>
struct shared_array_maker<T[N]> {
  std::shared_ptr<T> operator()const{
    auto r = std::make_shared<std::array<T,N>>();
    if (!r) return {};
    return {r.data(), r};
  }
};
template<class Arr>
auto make_shared_array()
-> decltype( shared_array_maker<Arr>{}() )
{ return shared_array_maker<Arr>{}(); }
then make_shared_array<int[10]> returns a shared_ptr<int> pointing to 10 ints all default constructed.
Version ≥ C++17
With C++17, shared_ptr gained special support for array types. It is no longer necessary to specify the array-deleter
explicitly, and the shared pointer can be dereferenced using the [] array index operator:
std::shared_ptr<int[]> sh(new int[10]);
sh[0] = 42;
Shared pointers can point to a sub-object of the object it owns:
struct Foo { int x; };
std::shared_ptr<Foo> p1 = std::make_shared<Foo>();
std::shared_ptr<int> p2(p1, &p1->x);
Both p2 and p1 own the object of type Foo, but p2 points to its int member x. This means that if p1 goes out of
scope or is reassigned, the underlying Foo object will still be alive, ensuring that p2 does not dangle.
Important: A shared_ptr only knows about itself and all other shared_ptr that were created with the alias
constructor. It does not know about any other pointers, including all other shared_ptrs created with a reference to
the same Foo instance:
Foo *foo = new Foo;
std::shared_ptr<Foo> shared1(foo);
std::shared_ptr<Foo> shared2(foo); // don't do this
shared1.reset(); // this will delete foo, since shared1
                 // was the only shared_ptr that owned it
shared2->test(); // UNDEFINED BEHAVIOR: shared2's foo has been
                 // deleted already!!
Ownership Transfer of shared_ptr
By default, shared_ptr increments the reference count and doesn't transfer the ownership. However, it can be
made to transfer the ownership using std::move:
shared_ptr<int> up = make_shared<int>();
// Transferring the ownership
shared_ptr<int> up2 = move(up);
// At this point, the reference count of up = 0 and the
// ownership of the pointer is solely with up2 with reference count = 1
Section 33.3: Sharing with temporary ownership
(std::weak_ptr)
Instances of std::weak_ptr can point to objects owned by instances of std::shared_ptr while only becoming
temporary owners themselves. This means that weak pointers do not alter the object's reference count and
therefore do not prevent an object's deletion if all of the object's shared pointers are reassigned or destroyed.
In the following example instances of std::weak_ptr are used so that the destruction of a tree object is not
inhibited:
```




## include <memory> // enable_shared_from_this

class Widget : public std::enable_shared_from_this< Widget >
{
public:
```cpp
    void DoSomething()
    {
        std::shared_ptr< Widget > self = shared_from_this();
        someEvent -> Register( self );
    }
private:
};
int main()
{
    auto w = std::make_shared< Widget >();
    w -> DoSomething();
}
If you use shared_from_this() on an object not owned by a shared_ptr, such as a local automatic object or a
global object, then the behavior is undeﬁned. Since C++17 it throws std::bad_alloc instead.
Using shared_from_this() from a constructor is equivalent to using it on an object not owned by a shared_ptr,
because the objects is possessed by the shared_ptr after the constructor returns.
```



C++11 revolutionized memory management by introducing smart pointers, which strictly define ownership semantics and automate memory reclamation, effectively making `new` and `delete` unnecessary in user code.

***



### 1. THE PROBLEM WITH RAW POINTERS

In C++98, dynamic memory required manual management:
1.  **Memory Leaks**: Forgetting `delete`.
2.  **Dangling Pointers**: Accessing deleted memory.
3.  **Double Free**: Deleting the same memory twice.
4.  **Exception Safety**: If an exception throws before `delete`, memory leaks.

Smart pointers solve these by using **RAII (Resource Acquisition Is Initialization)**.

***



### 2. UNIQUE_PTR (Exclusive Ownership)

`std::unique_ptr` represents exclusive ownership. An object can have only one `unique_ptr` pointing to it. When the `unique_ptr` is destroyed, the object is deleted.



#### 2.2 Move Only

You cannot copy a `unique_ptr`. You must **move** it. This ensures uniqueness.

```cpp
std::unique_ptr<int> p1(new int(5));
// std::unique_ptr<int> p2 = p1; // Error! Copy deleted.

std::unique_ptr<int> p2 = std::move(p1); // OK. p1 is now empty/null.
```



#### 2.3 Custom Deleters

Useful for managing C-style resources (files, sockets).

```cpp
auto deleter = [](FILE* f) { fclose(f); };
std::unique_ptr<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
```

***



### 3. SHARED_PTR (Shared Ownership)

`std::shared_ptr` allows multiple pointers to own the same resource. The resource is deleted only when the *last* `shared_ptr` is destroyed.



#### 3.1 Reference Counting

It maintains a "control block" with a reference count.

```cpp
auto p1 = std::make_shared<int>(100); // Ref count = 1
{
    auto p2 = p1; // Copy allowed. Ref count = 2
} // p2 destroyed. Ref count = 1

// p1 destroyed. Ref count = 0. Memory freed.
```



#### 3.2 Performance Cost

`shared_ptr` is heavier than `unique_ptr` (2x size usually, plus atomic ref-count increment/decrement overhead). Use only when ownership is truly shared.

***



### 4. WEAK_PTR (Non-Owning Reference)

`std::weak_ptr` observes a `shared_ptr` without keeping it alive. It breaks **circular references**.



#### 4.1 Circular Reference Problem

If A has a `shared_ptr` to B, and B has a `shared_ptr` to A, the reference count never drops to zero.



#### 4.2 Using weak_ptr

```cpp
struct B;
struct A {
    std::shared_ptr<B> b_ptr;
};
struct B {
    std::weak_ptr<A> a_ptr; // Use weak_ptr back to A
};
```

To use a `weak_ptr`, you must convert it to `shared_ptr` via `.lock()`.

```cpp
if (auto shared = weak.lock()) {
    // safe to use shared
} else {
    // object died
}
```

***



### 5. BEST PRACTICES

1.  **Prefer `unique_ptr`** by default. It has zero overhead.
2.  **Use `make_unique`** (C++14) and **`make_shared`**. They are cleaner and exception-safe. `make_shared` is also more efficient (allocates object and control block in one chunk).
3.  **Avoid `new` and `delete`**.



##### Value Categories

Section 74.1: Value Category Meanings
Expressions in C++ are assigned a particular value category, based on the result of those expressions. Value
categories for expressions can aﬀect C++ function overload resolution.
Value categories determines two important-but-separate properties about an expression. One property is whether
the expression has identity. An expression has identity if it refers to an object that has a variable name. The variable
name may not be involved in the expression, but the object can still have one.
The other property is whether it is legal to implicitly move from the expression's value. Or more speciﬁcally,
whether the expression, when used as a function parameter, will bind to r-value parameter types or not.
C++ deﬁnes 3 value categories which represent the useful combination of these properties: lvalue (expressions with
identity but not movable from), xvalue (expressions with identity that are moveable from), and prvalue (expressions
without identity that are moveable from). C++ does not have expressions which have no identity and cannot be
moved from.
C++ deﬁnes two other value categories, each based solely on one of these properties: glvalue (expressions with
identity) and rvalue (expressions that can be moved from). These act as useful groupings of the prior categories.
This graph serves as an illustration:
Section 74.2: rvalue
An rvalue expression is any expression which can be implicitly moved from, regardless of whether it has identity.
More precisely, rvalue expressions may be used as the argument to a function that takes a parameter of type T &&
(where T is the type of expr). Only rvalue expressions may be given as arguments to such function parameters; if a
non-rvalue expression is used, then overload resolution will pick any function that does not use an rvalue reference
parameter. And if none exist, then you get an error.
The category of rvalue expressions includes all xvalue and prvalue expressions, and only those expressions.
The standard library function std::move exists to explicitly transform a non-rvalue expression into an rvalue. More
speciﬁcally, it turns the expression into an xvalue, since even if it was an identity-less prvalue expression before, by
passing it as a parameter to std::move, it gains identity (the function's parameter name) and becomes an xvalue.
Consider the following:
```cpp
std::string str("init");                       //1
std::string test1(str);                        //2
std::string test2(std::move(str));             //3
str = std::string("new value");                //4
std::string &&str_ref = std::move(str);        //5
std::string test3(str_ref);                    //6
std::string has a constructor which takes a single parameter of type std::string&&, commonly called a "move
constructor". However, the value category of the expression str is not an rvalue (speciﬁcally it is an lvalue), so it
cannot call that constructor overload. Instead, it calls the const std::string& overload, the copy constructor.
Line 3 changes things. The return value of std::move is a T&&, where T is the base type of the parameter passed in.
So std::move(str) returns std::string&&. A function call who's return value is an rvalue reference is an rvalue
expression (speciﬁcally an xvalue), so it may call the move constructor of std::string. After line 3, str has been
moved from (who's contents are now undeﬁned).
Line 4 passes a temporary to the assignment operator of std::string. This has an overload which takes a
std::string&&. The expression std::string("new value") is an rvalue expression (speciﬁcally a prvalue), so it
may call that overload. Thus, the temporary is moved into str, replacing the undeﬁned contents with speciﬁc
contents.
Line 5 creates a named rvalue reference called str_ref that refers to str. This is where value categories get
confusing.
See, while str_ref is an rvalue reference to std::string, the value category of the expression str_ref is not an
rvalue. It is an lvalue expression. Yes, really. Because of this, one cannot call the move constructor of std::string
with the expression str_ref. Line 6 therefore copies the value of str into test3.
To move it, we would have to employ std::move again.
Section 74.3: xvalue
An xvalue (eXpiring value) expression is an expression which has identity and represents an object which can be
implicitly moved from. The general idea with xvalue expressions is that the object they represent is going to be
destroyed soon (hence the "eXpiring" part), and therefore implicitly moving from them is ﬁne.
Given:
struct X { int n; };
extern X x;
4;                   // prvalue: does not have an identity
x;                   // lvalue
x.n;                 // lvalue
std::move(x);        // xvalue
std::forward<X&>(x); // lvalue
X{4};                // prvalue: does not have an identity
X{4}.n;              // xvalue: does have an identity and denotes resources
                     // that can be reused
Section 74.4: prvalue
A prvalue (pure-rvalue) expression is an expression which lacks identity, whose evaluation is typically used to
initialize an object, and which can be implicitly moved from. These include, but are not limited to:
Expressions that represent temporary objects, such as std::string("123").
A function call expression that does not return a reference
A literal (except a string literal - those are lvalues), such has 1, true, 0.5f, or 'a'
A lambda expression
```

The built-in addressof operator (&) cannot be applied on these expressions.
Section 74.5: lvalue
An lvalue expression is an expression which has identity, but cannot be implicitly moved from. Among these are
expressions that consist of a variable name, function name, expressions that are built-in dereference operator uses
and expressions that refer to lvalue references.
The typical lvalue is simply a name, but lvalues can come in other ﬂavors as well:
```cpp
struct X { ... };
X x;         // x is an lvalue
X* px = &x;  // px is an lvalue
*px = X{};   // *px is also an lvalue, X{} is a prvalue
X* foo_ptr();  // foo_ptr() is a prvalue
X& foo_ref();  // foo_ref() is an lvalue
```

Additionally, while most literals (e.g. 4, 'x', etc.) are prvalues, string literals are lvalues.
Section 74.6: glvalue
A glvalue (a "generalized lvalue") expression is any expression which has identity, regardless of whether it can be
moved from or not. This category includes lvalues (expressions that have identity but can't be moved from) and
xvalues (expressions that have identity, and can be moved from), but excludes prvalues (expressions without
identity).
If an expression has a name, it's a glvalue:
```cpp
struct X { int n; };
X foo();
X x;
x; // has a name, so it's a glvalue
std::move(x); // has a name (we're moving from "x"), so it's a glvalue
              // can be moved from, so it's an xvalue not an lvalue
foo(); // has no name, so is a prvalue, not a glvalue
X{};   // temporary has no name, so is a prvalue, not a glvalue
X{}.n; // HAS a name, so is a glvalue. can be moved from, so it's an xvalue
```




##### Move Semantics

Section 106.1: Move semantics
Move semantics are a way of moving one object to another in C++. For this, we empty the old object and place
everything it had in the new object.
For this, we must understand what an rvalue reference is. An rvalue reference (T&& where T is the object type) is not
much diﬀerent than a normal reference (T&, now called lvalue references). But they act as 2 diﬀerent types, and so,
we can make constructors or functions that take one type or the other, which will be necessary when dealing with
move semantics.
The reason why we need two diﬀerent types is to specify two diﬀerent behaviors. Lvalue reference constructors are
related to copying, while rvalue reference constructors are related to moving.
To move an object, we will use std::move(obj). This function returns an rvalue reference to the object, so that we
can steal the data from that object into a new one. There are several ways of doing this which are discussed below.
Important to note is that the use of std::move creates just an rvalue reference. In other words the statement
std::move(obj) does not change the content of obj, while auto obj2 = std::move(obj) (possibly) does.
Section 106.2: Using std::move to reduce complexity from
O(n²) to O(n)
C++11 introduced core language and standard library support for moving an object. The idea is that when an
object o is a temporary and one wants a logical copy, then its safe to just pilfer o's resources, such as a dynamically
allocated buﬀer, leaving o logically empty but still destructible and copyable.
The core language support is mainly
the rvalue reference type builder &&, e.g., std::string&& is an rvalue reference to a std::string, indicating
that that referred to object is a temporary whose resources can just be pilfered (i.e. moved)
special support for a move constructor T( T&& ), which is supposed to eﬃciently move resources from the
speciﬁed other object, instead of actually copying those resources, and
special support for a move assignment operator auto operator=(T&&) -> T&, which also is supposed to
move from the source.
The standard library support is mainly the std::move function template from the <utility> header. This function
produces an rvalue reference to the speciﬁed object, indicating that it can be moved from, just as if it were a
temporary.
For a container actual copying is typically of O(n) complexity, where n is the number of items in the container, while
moving is O(1), constant time. And for an algorithm that logically copies that container n times, this can reduce the
complexity from the usually impractical O(n²) to just linear O(n).
In his article “Containers That Never Change” in Dr. Dobbs Journal in September 19 2013, Andrew Koenig presented
an interesting example of algorithmic ineﬃciency when using a style of programming where variables are
immutable after initialization. With this style loops are generally expressed using recursion. And for some
algorithms such as generating a Collatz sequence, the recursion requires logically copying a container:
// Based on an example by Andrew Koenig in his Dr. Dobbs Journal article
// “Containers That Never Change” September 19, 2013, available at
// <url: http://www.drdobbs.com/cpp/containters-that-never-change/240161543>
// Includes here, e.g. <vector>
namespace my {
```cpp
    template< class Item >
    using Vector_ = /* E.g. std::vector<Item> */;
    auto concat( Vector_<int> const& v, int const x )
        -> Vector_<int>
    {
        auto result{ v };
        result.push_back( x );
        return result;
    }
    auto collatz_aux( int const n, Vector_<int> const& result )
        -> Vector_<int>
    {
        if( n == 1 )
        {
            return result;
        }
        auto const new_result = concat( result, n );
        if( n % 2 == 0 )
        {
            return collatz_aux( n/2, new_result );
        }
        else
        {
            return collatz_aux( 3*n + 1, new_result );
        }
    }
    auto collatz( int const n )
        -> Vector_<int>
    {
        assert( n != 0 );
        return collatz_aux( n, Vector_<int>() );
    }
}  // namespace my
```




#### 1.1 Lvalues

An **lvalue** (locator value) represents an object that occupies an identifiable location in memory (has an address).
- Example: `int x = 5;` (`x` is lvalue).
- You can take its address: `&x`.



#### 1.2 Rvalues

An **rvalue** is everything else: temporary values, literals, or results of expressions.
- Example: `5`, `x + 2`, `funcReturningVal()`.
- You cannot take its address.

***



### 2. RVALUE REFERENCES

C++11 introduced the rvalue reference: `T&&`. It binds *only* to rvalues.

```cpp
int x = 10;
int& lref = x;      // Lvalue ref binds to lvalue
// int&& rref = x;  // Error: cannot bind rvalue ref to lvalue

int&& rref2 = 20;   // OK: 20 is rvalue
```

***



### 3. MOVE CONSTRUCTOR & ASSIGNMENT

This allows a class to steal resources from a temporary object instead of making a deep copy.



#### 3.2 Move Constructor (The C++11 Way)

```cpp
    // Move Constructor
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        // Steal the pointer
        other.data = nullptr; // Null out source
        other.size = 0;
    }
```

If `other` is a temporary, the compiler selects the Move Constructor. This is O(1) instead of O(N).

***



### 4. STD::MOVE

`std::move(x)` does exactly one thing: it casts `x` to an rvalue reference (`T&&`). It essentially says, "I am done with this object, you can steal from it."

```cpp
Vector v1(100);
Vector v2 = std::move(v1); // Calls Move Constructor
// v1 is now empty (if implemented correctly)
```

***



### 5. PERFECT FORWARDING

Used in templates to preserve the value category (lvalue vs rvalue) of arguments.



#### 5.1 Universal References (Forwarding References)

If `T` is a template parameter, `T&&` is a **universal reference**, not just an rvalue reference. It can bind to anything.



#### 5.2 std::forward

```cpp
template<typename T>
void wrapper(T&& arg) {
    func(std::forward<T>(arg));
}
```

- If `wrapper` is called with lvalue, `arg` is lvalue, `forward` keeps it lvalue.
- If `wrapper` is called with rvalue, `arg` is lvalue (as a named variable), but `forward` casts it back to rvalue.

This enables `emplace_back` to work efficiently.



##### Perfect Forwarding

Section 101.1: Factory functions
Suppose we want to write a factory function that accepts an arbitrary list of arguments and passes those
arguments unmodiﬁed to another function. An example of such a function is make_unique, which is used to safely
construct a new instance of T and return a unique_ptr<T> that owns the instance.
The language rules regarding variadic templates and rvalue references allows us to write such a function.
```cpp
template<class T, class... A>
unique_ptr<T> make_unique(A&&... args)
{
    return unique_ptr<T>(new T(std::forward<A>(args)...));
}
The use of ellipses ... indicate a parameter pack, which represents an arbitrary number of types. The compiler will
expand this parameter pack to the correct number of arguments at the call site. These arguments are then passed
to T's constructor using std::forward. This function is required to preserve the ref-qualiﬁers of the arguments.
struct foo
{
    foo() {}
    foo(const foo&) {}                    // copy constructor
    foo(foo&&) {}                         // copy constructor
    foo(int, int, int) {}
};
foo f;
auto p1 = make_unique<foo>(f);            // calls foo::foo(const foo&)
auto p2 = make_unique<foo>(std::move(f)); // calls foo::foo(foo&&)
auto p3 = make_unique<foo>(1, 2, 3);
```



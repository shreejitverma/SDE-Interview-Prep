# MOVE SEMANTICS AND SMART POINTERS


# MOVE SEMANTICS & SMART POINTERS

## 1. Rvalue References & Move Semantics

C++11 solves the problem of unnecessary copying with **Move Semantics**.

### 1.1 Lvalues vs Rvalues
*   **Lvalue (Left Value):** Has a name, has an address (identity). Persists beyond the expression.
*   **Rvalue (Right Value):** Temporary, no name (or about to die). Cannot take address.

```cpp
int x = 10; // x is lvalue, 10 is rvalue
int y = x;  // y is lvalue
int z = x + y; // (x+y) is rvalue
```

### 1.2 Rvalue References (`T&&`)
A reference that binds *only* to rvalues. Represents an object we can "steal" from.

```cpp
void f(int& x)  { cout << "Lvalue ref"; }
void f(int&& x) { cout << "Rvalue ref"; }

int a = 5;
f(a); // Lvalue ref
f(5); // Rvalue ref
```

### 1.3 Move Constructor & Move Assignment
Instead of copying data (slow), we steal pointers (fast).

```cpp
class Buffer {
    int* data;
    size_t size;
public:
    // Move Constructor
    Buffer(Buffer&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr; // Nullify source to prevent double free
        other.size = 0;
    }

    // Move Assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data;       // Free own resources
            data = other.data;   // Steal resources
            size = other.size;
            other.data = nullptr;// Nullify source
            other.size = 0;
        }
        return *this;
    }
};
```

### 1.4 `std::move`
Casts an lvalue to an rvalue, enabling move semantics.

```cpp
Buffer b1;
Buffer b2 = std::move(b1); // Calls Move Constructor
// b1 is now in valid but unspecified state (empty)
```

**Warning:** Do not use `b1` after moving from it.

---

## 2. Smart Pointers (RAII)

Manual `new`/`delete` is prone to leaks. C++11 introduces strict ownership models.

### 2.1 `std::unique_ptr`
*   **Ownership:** Exclusive. Only one pointer owns the object.
*   **Copying:** Disabled (`delete`).
*   **Moving:** Allowed.
*   **Overhead:** Zero (same as raw pointer).

```cpp
#include <memory>

std::unique_ptr<int> p1(new int(5));
// std::unique_ptr<int> p2 = p1; // ERROR: Cannot copy
std::unique_ptr<int> p3 = std::move(p1); // OK: Ownership transferred
```

### 2.2 `std::shared_ptr`
*   **Ownership:** Shared (Reference Counted).
*   **Destruction:** Object deleted when last `shared_ptr` dies.
*   **Overhead:** Control block on heap (ref counts).

```cpp
auto sp1 = std::make_shared<int>(10); // Efficient allocation
auto sp2 = sp1; // Ref count = 2
```

---
### Professional Notes: Memory & Resource Management

#### 1. RAII: Resource Acquisition Is Initialization
RAII is the core of modern C++ resource management. It ensures that resources (memory, file handles, sockets) are tied to object lifetime.
*   **Acquisition**: In the constructor.
*   **Release**: In the destructor.
*   **Benefit**: Exception safety. If an exception occurs, stack unwinding automatically calls destructors, preventing leaks.

#### 2. The C++11 Memory Model and Atomics
Before C++11, the language had no formal definition of threads or shared memory.
*   **The Problem**: Compilers and CPUs often reorder instructions for performance, which can break multi-threaded logic.
*   **The Solution**: `std::atomic<T>` and memory barriers. They ensure that operations are visible across threads in a predictable order (`std::memory_order`).
*   **Data Races**: Accessing the same memory location from multiple threads where at least one is a write is **Undefined Behavior** unless synchronized.

#### 3. Smart Pointer Gotchas
*   **`std::make_shared` vs. `new`**: `make_shared` is faster and more exception-safe because it performs a single allocation for both the object and the control block.
*   **Circular References**: If two `shared_ptr` objects point to each other, they will never be deleted. Use `std::weak_ptr` for one of the links to break the cycle.
*   **`this` as shared**: Use `std::enable_shared_from_this<T>` if you need to pass a `shared_ptr` to the current object from inside a member function.

---

### 2.3 `std::weak_ptr`
*   **Ownership:** Non-owning observer of `shared_ptr`.
*   **Use Case:** Break cyclic references (A->B, B->A).

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(42);
std::weak_ptr<int> wp = sp;

if (auto locked = wp.lock()) { // Check if object exists
    std::cout << *locked;
}
```

### 2.4 `std::make_shared` vs `new`
Always use `std::make_shared`. It allocates the object and control block in a single heap allocation (cache efficient).

---

## 3. Perfect Forwarding

Used in templates to preserve value category (lvalue vs rvalue).

```cpp
template<typename T>
void wrapper(T&& arg) {
    // std::forward passes arg as lvalue if given lvalue,
    // rvalue if given rvalue.
    func(std::forward<T>(arg)); 
}
```


# Professional Notes: Chapter 33: Smart Pointers

Section 33.1: Unique ownership (std::unique_ptr) 
Section 33.2: Sharing ownership (std::shared_ptr) 
Section 33.3: Sharing with temporary ownership (std::weak_ptr) 
Section 33.4: Using custom deleters to create a wrapper to a C interface 
Section 33.5: Unique ownership without move semantics (auto_ptr) 
Section 33.6: Casting std::shared_ptr pointers 
Section 33.7: Writing a smart pointer: value_ptr 
Section 33.8: Getting a shared_ptr referring to this 

# Professional Notes: Chapter 106: Move Semantics

Section 106.1: Move semantics 
Section 106.2: Using std::move to reduce complexity from O(n) to O(n) 
Section 106.3: Move constructor 
Section 106.4: Re-use a moved object 
Section 106.5: Move assignment 
Section 106.6: Using move semantics on containers 

# Professional Notes: Chapter 33: Smart Pointers

Section 33.1: Unique ownership (std::unique_ptr)
Version  C++11
A std::unique_ptr is a class template that manages the lifetime of a dynamically stored object. Unlike for
std::shared_ptr, the dynamic object is owned by only one instance of a std::unique_ptr at any time,
// Creates a dynamic int with value of 20 owned by a unique pointer
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
Version  C++11
Unlike the dumb smart pointer (std::auto_ptr), unique_ptr can also be instantiated with vector allocation (not
std::vector). Earlier examples were for scalar allocations. For example to have a dynamically allocated integer
array for 10 elements, you would specify int[] as the template type (and not just int):
std::unique_ptr<int[]> arr_ptr = std::make_unique<int[]>(10);
Which can be simplied with:
auto arr_ptr = std::make_unique<int[]>(10);
Now, you use arr_ptr as if it is an array:
arr_ptr[2] =  10; // Modify third element
You need not to worry about de-allocation. This template specialized version calls constructors and destructors
appropriately. Using vectored version of unique_ptr or a vector itself - is a personal choice.
In versions prior to C++11, std::auto_ptr was available. Unlike unique_ptr it is allowed to copy auto_ptrs, upon
which the source ptr will lose the ownership of the contained pointer and the target receives it.
Section 33.2: Sharing ownership (std::shared_ptr)
The class template std::shared_ptr denes a shared pointer that is able to share ownership of an object with
other shared pointers. This contrasts to std::unique_ptr which represents exclusive ownership.
The sharing behavior is implemented through a technique known as reference counting, where the number of
shared pointers that point to the object is stored alongside it. When this count reaches zero, either through the
destruction or reassignment of the last std::shared_ptr instance, the object is automatically destroyed.
// Creation: 'firstShared' is a shared pointer for a new instance of 'Foo'
std::shared_ptr<Foo> firstShared = std::make_shared<Foo>(/*args*/);
To create multiple smart pointers that share the same object, we need to create another shared_ptr that aliases
the rst shared pointer. Here are 2 ways of doing it:
std::shared_ptr<Foo> secondShared(firstShared);  // 1st way: Copy constructing
std::shared_ptr<Foo> secondShared;
secondShared = firstShared;                      // 2nd way: Assigning
Either of the above ways makes secondShared a shared pointer that shares ownership of our instance of Foo with
firstShared.
The smart pointer works just like a raw pointer. This means, you can use * to dereference them. The regular ->
operator works as well:
secondShared->test(); // Calls Foo::test()
Finally, when the last aliased shared_ptr goes out of scope, the destructor of our Foo instance is called.
Warning: Constructing a shared_ptr might throw a bad_alloc exception when extra data for shared ownership
semantics needs to be allocated. If the constructor is passed a regular pointer it assumes to own the object pointed
to and calls the deleter if an exception is thrown. This means shared_ptr<T>(new T(args)) will not leak a T object if
allocation of shared_ptr<T> fails. However, it is advisable to use make_shared<T>(args) or
allocate_shared<T>(alloc, args), which enable the implementation to optimize the memory allocation.
Allocating Arrays([]) using shared_ptr
Version  C++11 Version < C++17
Unfortunately, there is no direct way to allocate Arrays using make_shared<>.
It is possible to create arrays for shared_ptr<> using new and std::default_delete.
For example, to allocate an array of 10 integers, we can write the code as
shared_ptr<int> sh(new int[10], std::default_delete<int[]>());
Specifying std::default_delete is mandatory here to make sure that the allocated memory is correctly cleaned up
using delete[].
If we know the size at compile time, we can do it this way:
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
Version  C++17
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
#include <memory>
#include <vector>
struct TreeNode {
    std::weak_ptr<TreeNode> parent;
    std::vector< std::shared_ptr<TreeNode> > children;
};
int main() {
    // Create a TreeNode to serve as the root/parent.
    std::shared_ptr<TreeNode> root(new TreeNode);
    // Give the parent 100 child nodes.
    for (size_t i = 0; i < 100; ++i) {
        std::shared_ptr<TreeNode> child(new TreeNode);
        root->children.push_back(child);
        child->parent = root;
    }
    // Reset the root shared pointer, destroying the root object, and
    // subsequently its child nodes.
    root.reset();
}
As child nodes are added to the root node's children, their std::weak_ptr member parent is set to the root node.
The member parent is declared as a weak pointer as opposed to a shared pointer such that the root node's
reference count is not incremented. When the root node is reset at the end of main(), the root is destroyed. Since
the only remaining std::shared_ptr references to the child nodes were contained in the root's collection children,
all child nodes are subsequently destroyed as well.
Due to control block implementation details, shared_ptr allocated memory may not be released until shared_ptr
reference counter and weak_ptr reference counter both reach zero.
#include <memory>
int main()
{
    {
         std::weak_ptr<int> wk;
         {
             // std::make_shared is optimized by allocating only once
             // while std::shared_ptr<int>(new int(42)) allocates twice.
             // Drawback of std::make_shared is that control block is tied to our integer
             std::shared_ptr<int> sh = std::make_shared<int>(42);
             wk = sh;
             // sh memory should be released at this point...
         }
         // ... but wk is still alive and needs access to control block
     }
     // now memory is released (sh and wk)
}
Since std::weak_ptr does not keep its referenced object alive, direct data access through a std::weak_ptr is not
possible. Instead it provides a lock() member function that attempts to retrieve a std::shared_ptr to the
referenced object:
#include <cassert>
#include <memory>
int main()
{
    {
         std::weak_ptr<int> wk;
         std::shared_ptr<int> sp;
         {
             std::shared_ptr<int> sh = std::make_shared<int>(42);
             wk = sh;
             // calling lock will create a shared_ptr to the object referenced by wk
             sp = wk.lock();
             // sh will be destroyed after this point, but sp is still alive
         }
         // sp still keeps the data alive.
         // At this point we could even call lock() again
         // to retrieve another shared_ptr to the same data from wk
         assert(*sp == 42);
         assert(!wk.expired());
         // resetting sp will delete the data,
         // as it is currently the last shared_ptr with ownership
         sp.reset();
         // attempting to lock wk now will return an empty shared_ptr,
         // as the data has already been deleted
         sp = wk.lock();
         assert(!sp);
         assert(wk.expired());
     }
}
Section 33.4: Using custom deleters to create a wrapper to a
C interface
Many C interfaces such as SDL2 have their own deletion functions. This means that you cannot use smart pointers
directly:
std::unique_ptr<SDL_Surface> a; // won't work, UNSAFE!
Instead, you need to dene your own deleter. The examples here use the SDL_Surface structure which should be
freed using the SDL_FreeSurface() function, but they should be adaptable to many other C interfaces.
The deleter must be callable with a pointer argument, and therefore can be e.g. a simple function pointer:
std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> a(pointer, SDL_FreeSurface);
Any other callable object will work, too, for example a class with an operator():
struct SurfaceDeleter {
    void operator()(SDL_Surface* surf) {
        SDL_FreeSurface(surf);
    }
};
std::unique_ptr<SDL_Surface, SurfaceDeleter> a(pointer, SurfaceDeleter{}); // safe
std::unique_ptr<SDL_Surface, SurfaceDeleter> b(pointer); // equivalent to the above
                                                         // as the deleter is value-initialized
This not only provides you with safe, zero overhead (if you use unique_ptr) automatic memory management, you
also get exception safety.
Note that the deleter is part of the type for unique_ptr, and the implementation can use the empty base
optimization to avoid any change in size for empty custom deleters. So while std::unique_ptr<SDL_Surface,
SurfaceDeleter> and std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> solve the same problem in a
similar way, the former type is still only the size of a pointer while the latter type has to hold two pointers: both the
SDL_Surface* and the function pointer! When having free function custom deleters, it is preferable to wrap the
function in an empty type.
In cases where reference counting is important, one could use a shared_ptr instead of an unique_ptr. The
shared_ptr always stores a deleter, this erases the type of the deleter, which might be useful in APIs. The
disadvantages of using shared_ptr over unique_ptr include a higher memory cost for storing the deleter and a
performance cost for maintaining the reference count.
// deleter required at construction time and is part of the type
std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> a(pointer, SDL_FreeSurface);
// deleter is only required at construction time, not part of the type
std::shared_ptr<SDL_Surface> b(pointer, SDL_FreeSurface);
Version  C++17
With template auto, we can make it even easier to wrap our custom deleters:
template <auto DeleteFn>
struct FunctionDeleter {
    template <class T>
    void operator()(T* ptr) {
        DeleteFn(ptr);
    }
};
template <class T, auto DeleteFn>
using unique_ptr_deleter = std::unique_ptr<T, FunctionDeleter<DeleteFn>>;
With which the above example is simply:
unique_ptr_deleter<SDL_Surface, SDL_FreeSurface> c(pointer);
Here, the purpose of auto is to handle all free functions, whether they return void (e.g. SDL_FreeSurface) or not
(e.g. fclose).
Section 33.5: Unique ownership without move semantics
(auto_ptr)
Version < C++11
NOTE: std::auto_ptr has been deprecated in C++11 and will be removed in C++17. You should only use this if you
are forced to use C++03 or earlier and are willing to be careful. It is recommended to move to unique_ptr in
combination with std::move to replace std::auto_ptr behavior.
Before we had std::unique_ptr, before we had move semantics, we had std::auto_ptr. std::auto_ptr provides
unique ownership but transfers ownership upon copy.
As with all smart pointers, std::auto_ptr automatically cleans up resources (see RAII):
{
    std::auto_ptr<int> p(new int(42));
    std::cout << *p;
} // p is deleted here, no memory leaked
but allows only one owner:
std::auto_ptr<X> px = ...;
std::auto_ptr<X> py = px;
  // px is now empty
This allows to use std::auto_ptr to keep ownership explicit and unique at the danger of losing ownership
unintended:
void f(std::auto_ptr<X> ) {
    // assumes ownership of X
    // deletes it at end of scope
};
std::auto_ptr<X> px = ...;
f(px); // f acquires ownership of underlying X
       // px is now empty
px->foo(); // NPE!
// px.~auto_ptr() does NOT delete
The transfer of ownership happened in the "copy" constructor. auto_ptr's copy constructor and copy assignment
operator take their operands by non-const reference so that they could be modied. An example implementation
might be:
template <typename T>
class auto_ptr {
    T* ptr;
public:
    auto_ptr(auto_ptr& rhs)
    : ptr(rhs.release())
    { }
    auto_ptr& operator=(auto_ptr& rhs) {
        reset(rhs.release());
        return *this;
    }
    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }
    void reset(T* tmp = nullptr) {
        if (ptr != tmp) {
            delete ptr;
            ptr = tmp;
        }
    }
    /* other functions ... */
};
This breaks copy semantics, which require that copying an object leaves you with two equivalent versions of it. For
any copyable type, T, I should be able to write:
T a = ...;
T b(a);
assert(b == a);
But for auto_ptr, this is not the case. As a result, it is not safe to put auto_ptrs in containers.
Section 33.6: Casting std::shared_ptr pointers
It is not possible to directly use static_cast, const_cast, dynamic_cast and reinterpret_cast on
std::shared_ptr to retrieve a pointer sharing ownership with the pointer being passed as argument. Instead, the
functions std::static_pointer_cast, std::const_pointer_cast, std::dynamic_pointer_cast and
std::reinterpret_pointer_cast should be used:
struct Base { virtual ~Base() noexcept {}; };
struct Derived: Base {};
auto derivedPtr(std::make_shared<Derived>());
auto basePtr(std::static_pointer_cast<Base>(derivedPtr));
auto constBasePtr(std::const_pointer_cast<Base const>(basePtr));
auto constDerivedPtr(std::dynamic_pointer_cast<Derived const>(constBasePtr));
Note that std::reinterpret_pointer_cast is not available in C++11 and C++14, as it was only proposed by N3920
and adopted into Library Fundamentals TS in February 2014. However, it can be implemented as follows:
template <typename To, typename From>
inline std::shared_ptr<To> reinterpret_pointer_cast(
    std::shared_ptr<From> const & ptr) noexcept
{ return std::shared_ptr<To>(ptr, reinterpret_cast<To *>(ptr.get())); }
Section 33.7: Writing a smart pointer: value_ptr
A value_ptr is a smart pointer that behaves like a value. When copied, it copies its contents. When created, it
creates its contents.
// Like std::default_delete:
template<class T>
struct default_copier {
  // a copier must handle a null T const* in and return null:
  T* operator()(T const* tin)const {
    if (!tin) return nullptr;
    return new T(*tin);
  }
  void operator()(void* dest, T const* tin)const {
    if (!tin) return;
    return new(dest) T(*tin);
  }
};
// tag class to handle empty case:
struct empty_ptr_t {};
constexpr empty_ptr_t empty_ptr{};
// the value pointer type itself:
template<class T, class Copier=default_copier<T>, class Deleter=std::default_delete<T>,
  class Base=std::unique_ptr<T, Deleter>
>
struct value_ptr:Base, private Copier {
  using copier_type=Copier;
  // also typedefs from unique_ptr
  using Base::Base;
  value_ptr( T const& t ):
    Base( std::make_unique<T>(t) ),
    Copier()
  {}
  value_ptr( T && t ):
    Base( std::make_unique<T>(std::move(t)) ),
    Copier()
  {}
  // almost-never-empty:
      value_ptr():
    Base( std::make_unique<T>() ),
    Copier()
  {}
  value_ptr( empty_ptr_t ) {}
  value_ptr( Base b, Copier c={} ):
    Base(std::move(b)),
    Copier(std::move(c))
  {}
  Copier const& get_copier() const {
    return *this;
  }
  value_ptr clone() const {
    return {
      Base(
        get_copier()(this->get()),
        this->get_deleter()
      ),
      get_copier()
    };
  }
  value_ptr(value_ptr&&)=default;
  value_ptr& operator=(value_ptr&&)=default;
  value_ptr(value_ptr const& o):value_ptr(o.clone()) {}
  value_ptr& operator=(value_ptr const&o) {
    if (o && *this) {
      // if we are both non-null, assign contents:
      **this = *o;
    } else {
      // otherwise, assign a clone (which could itself be null):
      *this = o.clone();
    }
    return *this;
  }
  value_ptr& operator=( T const& t ) {
    if (*this) {
      **this = t;
    } else {
      *this = value_ptr(t);
    }
    return *this;
  }
  value_ptr& operator=( T && t ) {
    if (*this) {
      **this = std::move(t);
    } else {
      *this = value_ptr(std::move(t));
    }
    return *this;
  }
  T& get() { return **this; }
  T const& get() const { return **this; }
  T* get_pointer() {
    if (!*this) return nullptr;
    return std::addressof(get());
  }
  T const* get_pointer() const {
    if (!*this) return nullptr;
    return std::addressof(get());
  }
  // operator-> from unique_ptr
};
template<class T, class...Args>
value_ptr<T> make_value_ptr( Args&&... args ) {
  return {std::make_unique<T>(std::forward<Args>(args)...)};
}
This particular value_ptr is only empty if you construct it with empty_ptr_t or if you move from it. It exposes the fact
it is a unique_ptr, so explicit operator bool() const works on it. .get() has been changed to return a
reference (as it is almost never empty), and .get_pointer() returns a pointer instead.
This smart pointer can be useful for pImpl cases, where we want value-semantics but we also don't want to expose
the contents of the pImpl outside of the implementation le.
With a non-default Copier, it can even handle virtual base classes that know how to produce instances of their
derived and turn them into value-types.
Section 33.8: Getting a shared_ptr referring to this
enable_shared_from_this enables you to get a valid shared_ptr instance to this.
By deriving your class from the class template enable_shared_from_this, you inherit a method shared_from_this
that returns a shared_ptr instance to this.
Note that the object must be created as a shared_ptr in rst place:
#include <memory>
class A: public enable_shared_from_this<A> {
};
A* ap1 =new A();
shared_ptr<A> ap2(ap1); // First prepare a shared pointer to the object and hold it!
// Then get a shared pointer to the object from the object itself
shared_ptr<A> ap3 = ap1->shared_from_this();
int c3 =ap3.use_count(); // =2: pointing to the same object
Note(2) you cannot call enable_shared_from_this inside the constructor.
#include <memory> // enable_shared_from_this
class Widget : public std::enable_shared_from_this< Widget >
{
public:
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
global object, then the behavior is undened. Since C++17 it throws std::bad_alloc instead.
Using shared_from_this() from a constructor is equivalent to using it on an object not owned by a shared_ptr,
because the objects is possessed by the shared_ptr after the constructor returns.

# Professional Notes: Chapter 106: Move Semantics

Section 106.1: Move semantics
Move semantics are a way of moving one object to another in C++. For this, we empty the old object and place
everything it had in the new object.
For this, we must understand what an rvalue reference is. An rvalue reference (T&& where T is the object type) is not
much dierent than a normal reference (T&, now called lvalue references). But they act as 2 dierent types, and so,
we can make constructors or functions that take one type or the other, which will be necessary when dealing with
move semantics.
The reason why we need two dierent types is to specify two dierent behaviors. Lvalue reference constructors are
related to copying, while rvalue reference constructors are related to moving.
To move an object, we will use std::move(obj). This function returns an rvalue reference to the object, so that we
can steal the data from that object into a new one. There are several ways of doing this which are discussed below.
Important to note is that the use of std::move creates just an rvalue reference. In other words the statement
std::move(obj) does not change the content of obj, while auto obj2 = std::move(obj) (possibly) does.
Section 106.2: Using std::move to reduce complexity from
O(n) to O(n)
C++11 introduced core language and standard library support for moving an object. The idea is that when an
object o is a temporary and one wants a logical copy, then its safe to just pilfer o's resources, such as a dynamically
allocated buer, leaving o logically empty but still destructible and copyable.
The core language support is mainly
the rvalue reference type builder &&, e.g., std::string&& is an rvalue reference to a std::string, indicating
that that referred to object is a temporary whose resources can just be pilfered (i.e. moved)
special support for a move constructor T( T&& ), which is supposed to eciently move resources from the
specied other object, instead of actually copying those resources, and
special support for a move assignment operator auto operator=(T&&) -> T&, which also is supposed to
move from the source.
The standard library support is mainly the std::move function template from the <utility> header. This function
produces an rvalue reference to the specied object, indicating that it can be moved from, just as if it were a
temporary.
For a container actual copying is typically of O(n) complexity, where n is the number of items in the container, while
moving is O(1), constant time. And for an algorithm that logically copies that container n times, this can reduce the
complexity from the usually impractical O(n) to just linear O(n).
In his article Containers That Never Change in Dr. Dobbs Journal in September 19 2013, Andrew Koenig presented
an interesting example of algorithmic ineciency when using a style of programming where variables are
immutable after initialization. With this style loops are generally expressed using recursion. And for some
algorithms such as generating a Collatz sequence, the recursion requires logically copying a container:
// Based on an example by Andrew Koenig in his Dr. Dobbs Journal article
// Containers That Never Change September 19, 2013, available at
// <url: http://www.drdobbs.com/cpp/containters-that-never-change/240161543>
// Includes here, e.g. <vector>
namespace my {
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
#include <iostream>
using namespace std;
auto main() -> int
{
    for( int const x : my::collatz( 42 ) )
    {
        cout << x << ' ';
    }
    cout << '\n';
}
Output:
42 21 64 32 16 8 4 2
The number of item copy operations due to copying of the vectors is here roughly O(n), since it's the sum 1 + 2 + 3
+ ... n.
In concrete numbers, with g++ and Visual C++ compilers the above invocation of collatz(42) resulted in a Collatz
sequence of 8 items and 36 item copy operations (8*7/2 = 28, plus some) in vector copy constructor calls.
All of these item copy operations can be removed by simply moving vectors whose values are not needed anymore.
To do this it's necessary to remove const and reference for the vector type arguments, passing the vectors by value.
The function returns are already automatically optimized. For the calls where vectors are passed, and not used
again further on in the function, just apply std::move to move those buers rather than actually copying them:
using std::move;
auto concat( Vector_<int> v, int const x )
    -> Vector_<int>
{
    v.push_back( x );
    // warning: moving a local object in a return statement prevents copy elision [-Wpessimizing-
move]
    // See https://stackoverflow.com/documentation/c%2b%2b/2489/copy-elision
    // return move( v );
    return v;
}
auto collatz_aux( int const n, Vector_<int> result )
    -> Vector_<int>
{
    if( n == 1 )
    {
        return result;
    }
    auto new_result = concat( move( result ), n );
    struct result;      // Make absolutely sure no use of `result` after this.
    if( n % 2 == 0 )
    {
        return collatz_aux( n/2, move( new_result ) );
    }
    else
    {
        return collatz_aux( 3*n + 1, move( new_result ) );
    }
}
auto collatz( int const n )
    -> Vector_<int>
{
    assert( n != 0 );
    return collatz_aux( n, Vector_<int>() );
}
Here, with g++ and Visual C++ compilers, the number of item copy operations due to vector copy constructor
invocations, was exactly 0.
The algorithm is necessarily still O(n) in the length of the Collatz sequence produced, but this is a quite dramatic
improvement: O(n)  O(n).
With some language support one could perhaps use moving and still express and enforce the immutability of a
variable between its initialization and nal move, after which any use of that variable should be an error. Alas, as of
C++14 C++ does not support that. For loop-free code the no use after move can be enforced via a re-declaration of
the relevant name as an incomplete struct, as with struct result; above, but this is ugly and not likely to be
understood by other programmers; also the diagnostics can be quite misleading.
Summing up, the C++ language and library support for moving allows drastic improvements in algorithm
complexity, but due the support's incompleteness, at the cost of forsaking the code correctness guarantees and
code clarity that const can provide.
For completeness, the instrumented vector class used to measure the number of item copy operations due to copy
constructor invocations:
template< class Item >
class Copy_tracking_vector
{
private:
    static auto n_copy_ops()
        -> int&
    {
        static int value;
        return value;
    }
    vector<Item>    items_;
public:
    static auto n() -> int { return n_copy_ops(); }
    void push_back( Item const& o ) { items_.push_back( o ); }
    auto begin() const { return items_.begin(); }
    auto end() const { return items_.end(); }
    Copy_tracking_vector(){}
    Copy_tracking_vector( Copy_tracking_vector const& other )
        : items_( other.items_ )
    { n_copy_ops() += items_.size(); }
    Copy_tracking_vector( Copy_tracking_vector&& other )
        : items_( move( other.items_ ) )
    {}
};
Section 106.3: Move constructor
Say we have this code snippet.
class A {
public:
    int a;
    int b;
    A(const A &other) {
        this->a = other.a;
        this->b = other.b;
    }
};
To create a copy constructor, that is, to make a function that copies an object and creates a new one, we normally
would choose the syntax shown above, we would have a constructor for A that takes an reference to another object
of type A, and we would copy the object manually inside the method.
Alternatively, we could have written A(const A &) = default; which automatically copies over all members,
making use of its copy constructor.
To create a move constructor, however, we will be taking an rvalue reference instead of an lvalue reference, like
here.
class Wallet {
public:
    int nrOfDollars;
    Wallet() = default; //default ctor
    Wallet(Wallet &&other) {
        this->nrOfDollars = other.nrOfDollars;
        other.nrOfDollars = 0;
    }
};
Please notice that we set the old values to zero. The default move constructor (Wallet(Wallet&&) = default;)
copies the value of nrOfDollars, as it is a POD.
As move semantics are designed to allow 'stealing' state from the original instance, it is important to consider how
the original instance should look like after this stealing. In this case, if we would not change the value to zero we
would have doubled the amount of dollars into play.
Wallet a;
a.nrOfDollars = 1;
Wallet b (std::move(a)); //calling B(B&& other);
std::cout << a.nrOfDollars << std::endl; //0
std::cout << b.nrOfDollars << std::endl; //1
Thus we have move constructed an object from an old one.
While the above is a simple example, it shows what the move constructor is intended to do. It becomes more useful
in more complex cases, such as when resource management is involved.
    // Manages operations involving a specified type.
    // Owns a helper on the heap, and one in its memory (presumably on the stack).
    // Both helpers are DefaultConstructible, CopyConstructible, and MoveConstructible.
    template<typename T,
             template<typename> typename HeapHelper,
             template<typename> typename StackHelper>
    class OperationsManager {
        using MyType = OperationsManager<T, HeapHelper, StackHelper>;
        HeapHelper<T>* h_helper;
        StackHelper<T> s_helper;
        // ...
      public:
        // Default constructor & Rule of Five.
        OperationsManager() : h_helper(new HeapHelper<T>) {}
        OperationsManager(const MyType& other)
          : h_helper(new HeapHelper<T>(*other.h_helper)), s_helper(other.s_helper) {}
        MyType& operator=(MyType copy) {
            swap(*this, copy);
            return *this;
        }
        ~OperationsManager() {
            if (h_helper) { delete h_helper; }
        }
        // Move constructor (without swap()).
        // Takes other's HeapHelper<T>*.
        // Takes other's StackHelper<T>, by forcing the use of StackHelper<T>'s move constructor.
        // Replaces other's HeapHelper<T>* with nullptr, to keep other from deleting our shiny
        //  new helper when it's destroyed.
        OperationsManager(MyType&& other) noexcept
          : h_helper(other.h_helper),
            s_helper(std::move(other.s_helper)) {
            other.h_helper = nullptr;
        }
        // Move constructor (with swap()).
        // Places our members in the condition we want other's to be in, then switches members
        //  with other.
        // OperationsManager(MyType&& other) noexcept : h_helper(nullptr) {
        //     swap(*this, other);
        // }
        // Copy/move helper.
        friend void swap(MyType& left, MyType& right) noexcept {
            std::swap(left.h_helper, right.h_helper);
            std::swap(left.s_helper, right.s_helper);
        }
    };
Section 106.4: Re-use a moved object
You can re-use a moved object:
void consumingFunction(std::vector<int> vec) {
    // Some operations
}
int main() {
    // initialize vec with 1, 2, 3, 4
    std::vector<int> vec{1, 2, 3, 4};
    // Send the vector by move
    consumingFunction(std::move(vec));
    // Here the vec object is in an indeterminate state.
    // Since the object is not destroyed, we can assign it a new content.
    // We will, in this case, assign an empty value to the vector,
    // making it effectively empty
    vec = {};
    // Since the vector as gained a determinate value, we can use it normally.
    vec.push_back(42);
    // Send the vector by move again.
    consumingFunction(std::move(vec));
}
Section 106.5: Move assignment
Similarly to how we can assign a value to an object with an lvalue reference, copying it, we can also move the values
from an object to another without constructing a new one. We call this move assignment. We move the values from
one object to another existing object.
For this, we will have to overload operator =, not so that it takes an lvalue reference, like in copy assignment, but
so that it takes an rvalue reference.
class A {
    int a;
    A& operator= (A&& other) {
        this->a = other.a;
        other.a = 0;
        return *this;
    }
};
This is the typical syntax to dene move assignment. We overload operator = so that we can feed it an rvalue
reference and it can assign it to another object.
A a;
a.a = 1;
A b;
b = std::move(a); //calling A& operator= (A&& other)
std::cout << a.a << std::endl; //0
std::cout << b.a << std::endl; //1
Thus, we can move assign an object to another one.
Section 106.6: Using move semantics on containers
You can move a container instead of copying it:
void print(const std::vector<int>& vec) {
    for (auto&& val : vec) {
        std::cout << val << ", ";
    }
    std::cout << std::endl;
}
int main() {
    // initialize vec1 with 1, 2, 3, 4 and vec2 as an empty vector
    std::vector<int> vec1{1, 2, 3, 4};
    std::vector<int> vec2;
    // The following line will print 1, 2, 3, 4
    print(vec1);
    // The following line will print a new line
    print(vec2);
    // The vector vec2 is assigned with move assingment.
    // This will "steal" the value of vec1 without copying it.
    vec2 = std::move(vec1);
    // Here the vec1 object is in an indeterminate state, but still valid.
    // The object vec1 is not destroyed,
    // but there's is no guarantees about what it contains.
    // The following line will print 1, 2, 3, 4
    print(vec2);
}

---

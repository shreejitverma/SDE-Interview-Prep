# C++17 VOCABULARY TYPES

These types provide standard ways to represent optionality, alternatives, and type-erased values, replacing many custom implementations.

## 1. `std::string_view`

A non-owning reference to a string (or substring). **Zero-copy** string operations.

### 1.1 Efficiency

```cpp
// Bad: Copies string (potentially expensive allocation)
void print_str(std::string s) {
    std::cout << s << "\n";
}

// Good: No copy, no allocation
void print_view(std::string_view sv) {
    std::cout << sv << "\n";
}

int main() {
    const char* cstr = "Hello World";
    // print_str(cstr); // Creates std::string (allocates!)
    print_view(cstr);   // No allocation
    
    std::string s = "Hello World";
    print_view(s);      // Works with std::string too
    
    // Substrings are cheap!
    std::string_view sub = std::string_view(cstr).substr(0, 5); 
    print_view(sub);
}
```

### 1.2 Caveats
*   **Non-owning:** Ensure the underlying string outlives the view.
*   **Not null-terminated:** Do not pass `.data()` to C APIs unless you are sure.

## 2. `std::optional`

Represents a value that may or may not be present. Replaces pointers for nullable values or "magic values" (-1, "").

```cpp
#include <optional>

std::optional<int> find_even(const std::vector<int>& v) {
    for (int x : v) {
        if (x % 2 == 0) return x;
    }
    return std::nullopt; // or {}
}

int main() {
    auto res = find_even({1, 3, 5});
    if (res) { // or res.has_value()
        std::cout << *res; // or res.value() (throws if empty)
    } else {
        std::cout << "Not found";
    }
    
    // Value or default
    std::cout << res.value_or(0); 
}
```

## 3. `std::variant`

A type-safe union. Can hold one of several distinct types.

```cpp
#include <variant>

std::variant<int, float, std::string> v;

v = 10;
v = 3.14f;
v = "hello";

// Accessing
try {
    std::string s = std::get<std::string>(v);
    // int i = std::get<int>(v); // Throws std::bad_variant_access
} catch (...) {}

// std::visit (The Visitor Pattern)
std::visit([](auto&& arg) {
    std::cout << arg << "\n";
}, v);
```

## 4. `std::any`

A type-safe container for *single* values of any type. (Like `void*` but safe).

```cpp
#include <any>

std::any a = 1;
a = std::string("hello");

try {
    std::string s = std::any_cast<std::string>(a);
} catch (const std::bad_any_cast& e) {
    std::cout << e.what();
}
```


# Professional Notes: Chapter 51: std::optional

Section 51.1: Using optionals to represent the absence of a value 
Section 51.2: optional as return value 
Section 51.3: value_or 
Section 51.4: Introduction 
Section 51.5: Using optionals to represent the failure of a function 

# Professional Notes: Chapter 56: std::variant

Section 56.1: Create pseudo-method pointers 
Section 56.2: Basic std::variant use 
Section 56.3: Constructing a `std::variant` 

# Professional Notes: Chapter 51: std::optional

Section 51.1: Using optionals to represent the absence of a
value
Before C++17, having pointers with a value of nullptr commonly represented the absence of a value. This is a good
solution for large objects that have been dynamically allocated and are already managed by pointers. However, this
solution does not work well for small or primitive types such as int, which are rarely ever dynamically allocated or
managed by pointers. std::optional provides a viable solution to this common problem.
In this example, struct Person is deﬁned. It is possible for a person to have a pet, but not necessary. Therefore,
the pet member of Person is declared with an std::optional wrapper.
#include <iostream>
#include <optional>
#include <string>
struct Animal {
    std::string name;
};
struct Person {
    std::string name;
    std::optional<Animal> pet;
};
int main() {
    Person person;
    person.name = "John";
    if (person.pet) {
        std::cout << person.name << "'s pet's name is " <<
            person.pet->name << std::endl;
    }
    else {
        std::cout << person.name << " is alone." << std::endl;
    }
}
Section 51.2: optional as return value
std::optional<float> divide(float a, float b) {
  if (b!=0.f) return a/b;
  return {};
}
Here we return either the fraction a/b, but if it is not deﬁned (would be inﬁnity) we instead return the empty
optional.
A more complex case:
template<class Range, class Pred>
auto find_if( Range&& r, Pred&& p ) {
  using std::begin; using std::end;
  auto b = begin(r), e = end(r);
  auto r = std::find_if(b, e , p );
  using iterator = decltype(r);
  if (r==e)
    return std::optional<iterator>();
  return std::optional<iterator>(r);
}
template<class Range, class T>
auto find( Range&& r, T const& t ) {
  return find_if( std::forward<Range>(r), [&t](auto&& x){return x==t;} );
}
find( some_range, 7 ) searches the container or range some_range for something equal to the number 7.
find_if does it with a predicate.
It returns either an empty optional if it was not found, or an optional containing an iterator tothe element if it was.
This allows you to do:
if (find( vec, 7 )) {
  // code
}
or even
if (auto oit = find( vec, 7 )) {
  vec.erase(*oit);
}
without having to mess around with begin/end iterators and tests.
Section 51.3: value_or
void print_name( std::ostream& os, std::optional<std::string> const& name ) {
  std::cout "Name is: " << name.value_or("<name missing>") << '\n';
}
value_or either returns the value stored in the optional, or the argument if there is nothing store there.
This lets you take the maybe-null optional and give a default behavior when you actually need a value. By doing it
this way, the "default behavior" decision can be pushed back to the point where it is best made and immediately
needed, instead of generating some default value deep in the guts of some engine.
Section 51.4: Introduction
Optionals (also known as Maybe types) are used to represent a type whose contents may or may not be present.
They are implemented in C++17 as the std::optional class. For example, an object of type std::optional<int>
may contain some value of type int, or it may contain no value.
Optionals are commonly used either to represent a value that may not exist or as a return type from a function that
can fail to return a meaningful result.
Other approaches to optional
There are many other approach to solving the problem that std::optional solves, but none of them are quite
complete: using a pointer, using a sentinel, or using a pair<bool, T>.
Optional vs Pointer
In some cases, we can provide a pointer to an existing object or nullptr to indicate failure. But this is limited to
those cases where objects already exist - optional, as a value type, can also be used to return new objects without
resorting to memory allocation.
Optional vs Sentinel
A common idiom is to use a special value to indicate that the value is meaningless. This may be 0 or -1 for integral
types, or nullptr for pointers. However, this reduces the space of valid values (you cannot diﬀerentiate between a
valid 0 and a meaningless 0) and many types do not have a natural choice for the sentinel value.
Optional vs std::pair<bool, T>
Another common idiom is to provide a pair, where one of the elements is a bool indicating whether or not the
value is meaningful.
This relies upon the value type being default-constructible in the case of error, which is not possible for some types
and possible but undesirable for others. An optional<T>, in the case of error, does not need to construct anything.
Section 51.5: Using optionals to represent the failure of a
function
Before C++17, a function typically represented failure in one of several ways:
A null pointer was returned.
e.g. Calling a function Delegate *App::get_delegate() on an App instance that did not have a
delegate would return nullptr.
This is a good solution for objects that have been dynamically allocated or are large and managed by
pointers, but isn't a good solution for small objects that are typically stack-allocated and passed by
copying.
A speciﬁc value of the return type was reserved to indicate failure.
e.g. Calling a function unsigned shortest_path_distance(Vertex a, Vertex b) on two vertices that
are not connected may return zero to indicate this fact.
The value was paired together with a bool to indicate is the returned value was meaningful.
e.g. Calling a function std::pair<int, bool> parse(const std::string &str) with a string
argument that is not an integer would return a pair with an undeﬁned int and a bool set to false.
In this example, John is given two pets, Fluﬀy and Furball. The function Person::pet_with_name() is then called to
retrieve John's pet Whiskers. Since John does not have a pet named Whiskers, the function fails and std::nullopt is
returned instead.
#include <iostream>
#include <optional>
#include <string>
#include <vector>
struct Animal {
    std::string name;
};
struct Person {
    std::string name;
    std::vector<Animal> pets;
    std::optional<Animal> pet_with_name(const std::string &name) {
        for (const Animal &pet : pets) {
            if (pet.name == name) {
                return pet;
            }
        }
        return std::nullopt;
    }
};
int main() {
    Person john;
    john.name = "John";
    Animal fluffy;
    fluffy.name = "Fluffy";
    john.pets.push_back(fluffy);
    Animal furball;
    furball.name = "Furball";
    john.pets.push_back(furball);
    std::optional<Animal> whiskers = john.pet_with_name("Whiskers");
    if (whiskers) {
        std::cout << "John has a pet named Whiskers." << std::endl;
    }
    else {
        std::cout << "Whiskers must not belong to John." << std::endl;
    }
}

# Professional Notes: Chapter 56: std::variant

Section 56.1: Create pseudo-method pointers
This is an advanced example.
You can use variant for light weight type erasure.
template<class F>
struct pseudo_method {
  F f;
  // enable C++17 class type deduction:
  pseudo_method( F&& fin ):f(std::move(fin)) {}
  // Koenig lookup operator->*, as this is a pseudo-method it is appropriate:
  template<class Variant> // maybe add SFINAE test that LHS is actually a variant.
  friend decltype(auto) operator->*( Variant&& var, pseudo_method const& method ) {
    // var->*method returns a lambda that perfect forwards a function call,
    // behaving like a method pointer basically:
    return [&](auto&&...args)->decltype(auto) {
      // use visit to get the type of the variant:
      return std::visit(
        [&](auto&& self)->decltype(auto) {
          // decltype(x)(x) is perfect forwarding in a lambda:
          return method.f( decltype(self)(self), decltype(args)(args)... );
        },
        std::forward<Var>(var)
      );
    };
  }
};
this creates a type that overloads operator->* with a Variant on the left hand side.
// C++17 class type deduction to find template argument of `print` here.
// a pseudo-method lambda should take `self` as its first argument, then
// the rest of the arguments afterwards, and invoke the action:
pseudo_method print = [](auto&& self, auto&&...args)->decltype(auto) {
  return decltype(self)(self).print( decltype(args)(args)... );
};
Now if we have 2 types each with a print method:
struct A {
  void print( std::ostream& os ) const {
    os << "A";
  }
};
struct B {
  void print( std::ostream& os ) const {
    os << "B";
  }
};
note that they are unrelated types. We can:
std::variant<A,B> var = A{};
(var->*print)(std::cout);
and it will dispatch the call directly to A::print(std::cout) for us. If we instead initialized the var with B{}, it would
dispatch to B::print(std::cout).
If we created a new type C:
struct C {};
then:
std::variant<A,B,C> var = A{};
(var->*print)(std::cout);
will fail to compile, because there is no C.print(std::cout) method.
Extending the above would permit free function prints to be detected and used, possibly with use of if constexpr
within the print pseudo-method.
live example currently using boost::variant in place of std::variant.
Section 56.2: Basic std::variant use
This creates a variant (a tagged union) that can store either an int or a string.
std::variant< int, std::string > var;
We can store one of either type in it:
var = "hello"s;
And we can access the contents via std::visit:
// Prints "hello\n":
visit( [](auto&& e) {
  std::cout << e << '\n';
}, var );
by passing in a polymorphic lambda or similar function object.
If we are certain we know what type it is, we can get it:
auto str = std::get<std::string>(var);
but this will throw if we get it wrong. get_if:
auto* str  = std::get_if<std::string>(&var);
returns nullptr if you guess wrong.
Variants guarantee no dynamic memory allocation (other than which is allocated by their contained types). Only
one of the types in a variant is stored there, and in rare cases (involving exceptions while assigning and no safe way
to back out) the variant can become empty.
Variants let you store multiple value types in one variable safely and eﬃciently. They are basically smart, type-safe
unions.
Section 56.3: Constructing a `std::variant`
This does not cover allocators.
struct A {};
struct B { B()=default; B(B const&)=default; B(int){}; };
struct C { C()=delete; C(int) {}; C(C const&)=default; };
struct D { D( std::initializer_list<int> ) {}; D(D const&)=default; D()=default; };
std::variant<A,B> var_ab0; // contains a A()
std::variant<A,B> var_ab1 = 7; // contains a B(7)
std::variant<A,B> var_ab2 = var_ab1; // contains a B(7)
std::variant<A,B,C> var_abc0{ std::in_place_type<C>, 7 }; // contains a C(7)
std::variant<C> var_c0; // illegal, no default ctor for C
std::variant<A,D> var_ad0( std::in_place_type<D>, {1,3,3,4} ); // contains D{1,3,3,4}
std::variant<A,D> var_ad1( std::in_place_index<0> ); // contains A{}
std::variant<A,D> var_ad2( std::in_place_index<1>, {1,3,3,4} ); // contains D{1,3,3,4}
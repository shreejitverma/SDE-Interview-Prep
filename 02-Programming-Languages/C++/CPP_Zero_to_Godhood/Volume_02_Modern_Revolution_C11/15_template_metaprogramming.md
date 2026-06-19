# Chapter 15: Template Metaprogramming (Variadics)

# TEMPLATE METAPROGRAMMING & POWER FEATURES

## 1. Variadic Templates

Templates that accept an arbitrary number of parameters.

```cpp
// Base case (recursion terminator)
void print() {}

// Recursive step
template<typename T, typename... Args>
void print(T first, Args... args) {
    std::cout << first << " ";
    print(args...); // Unpack
}

int main() {
    print(1, "hello", 3.14);
}
```

***
### Professional Insights: Metaprogramming Depth

#### 1. Recursive Template Evaluation
Templates are effectively a functional language evaluated at compile time.
*   **Base Case**: Essential to stop the infinite recursion (as seen in `print()`).
*   **Arithmetic Metaprogramming**: Computing values at compile time using constant expressions and template specialization.
```cpp
template<int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};
template<>
struct Factorial<0> {
    static const int value = 1;
};
// Factorial<5>::value is computed as 120 by the compiler.
```

#### 2. Advanced Parameter Packs
*   **Fold Expressions (C++17)**: Binary operators can be applied to all elements of a pack without manual recursion:
```cpp
template<typename... Args>
auto sum(Args... args) {
    return (... + args); // Unary left fold
}
```
*   **Perfect Forwarding**: Use `std::forward<Args>(args)...` when passing packs to another function to preserve lvalue/rvalue properties.

#### 3. SFINAE (Substitution Failure Is Not An Error)
A core principle of C++ templates. If a template argument substitution results in an invalid type or expression, the compiler doesn't throw an error—it simply discards that overload.
*   **`std::void_t` (C++17)**: A powerful helper for creating traits that check for the existence of members or types within a class.

***

### 1.1 Parameter Packing (`...`)
*   `typename... Args`: Template parameter pack.
*   `Args... args`: Function parameter pack.
*   `sizeof...(Args)`: Number of arguments.

***

## 2. Type Traits

Compile-time type inspection and modification.

```cpp
#include <type_traits>

static_assert(std::is_integral<int>::value, "Must be int");
static_assert(std::is_pointer<int*>::value, "Must be ptr");

// Conditional compilation (SFINAE)
template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
process(T x) {
    // Only compiles for integers
}
```

***

## 3. `using` Aliases

Replaces `typedef`, works with templates.

```cpp
template<typename T>
using Dictionary = std::map<std::string, T>;

Dictionary<int> scores;
```

***

## 4. `std::tuple`

Generalization of `std::pair` to N elements.

```cpp
#include <tuple>

std::tuple<int, double, std::string> t(1, 3.14, "hi");
int i = std::get<0>(t);
```

## Professional Insights: Metaprogramming

In C++ Metaprogramming refers to the use of macros or templates to generate code at compile-time.
In general, macros are frowned upon in this role and templates are preferred, although they are not as generic.
Template metaprogramming often makes use of compile-time computations, whether via templates or constexpr
functions, to achieve its goals of generating code, however compile-time computations are not metaprogramming
per se.
Section 16.1: Calculating Factorials
Factorials can be computed at compile-time using template metaprogramming techniques.
```cpp
#include <iostream>
template<unsigned int n>
struct factorial
{
    enum
    {
        value = n * factorial<n - 1>::value
    };
};
template<>
struct factorial<0>
{
    enum { value = 1 };
};
int main()
{
    std::cout << factorial<7>::value << std::endl;    // prints "5040"
}
```
factorial is a struct, but in template metaprogramming it is treated as a template metafunction. By convention,
template metafunctions are evaluated by checking a particular member, either ::type for metafunctions that result
in types, or ::value for metafunctions that generate values.
In the above code, we evaluate the factorial metafunction by instantiating the template with the parameters we
want to pass, and using ::value to get the result of the evaluation.
The metafunction itself relies on recursively instantiating the same metafunction with smaller values. The
factorial<0> specialization represents the terminating condition. Template metaprogramming has most of the
restrictions of a functional programming language, so recursion is the primary "looping" construct.
Since template metafunctions execute at compile time, their results can be used in contexts that require compile-
time values. For example:
```cpp
int my_array[factorial<5>::value];
```
Automatic arrays must have a compile-time deﬁned size. And the result of a metafunction is a compile-time
constant, so it can be used here.
Limitation: Most of the compilers won't allow recursion depth beyond a limit. For example, g++ compiler by default

limits recursion depeth to 256 levels. In case of g++, programmer can set recursion depth using -ftemplate-depth-
X option.
Version ≥ C++11
Since C++11, the std::integral_constant template can be used for this kind of template computation:
```cpp
#include <iostream>
#include <type_traits>
template<long long n>
struct factorial :
  std::integral_constant<long long, n * factorial<n - 1>::value> {};
template<>
struct factorial<0> :
  std::integral_constant<long long, 1> {};
int main()
{
    std::cout << factorial<7>::value << std::endl;    // prints "5040"
}
```
Additionally, constexpr functions become a cleaner alternative.
#include <iostream>
constexpr long long factorial(long long n)
{
  return (n == 0) ? 1 : n * factorial(n - 1);
}
int main()
{
  char test[factorial(3)];
  std::cout << factorial(7) << '\\n';
}
The body of factorial() is written as a single statement because in C++11 constexpr functions can only use a
quite limited subset of the language.
Version ≥ C++14
Since C++14, many restrictions for constexpr functions have been dropped and they can now be written much
more conveniently:
```cpp
constexpr long long factorial(long long n)
{
  if (n == 0)
    return 1;
  else
    return n * factorial(n - 1);
}
```
Or even:
```cpp
constexpr long long factorial(int n)
{
  long long result = 1;

  for (int i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}
```
Version ≥ C++17
Since c++17 one can use fold expression to calculate factorial:
#include <iostream>
#include <utility>
template <class T, T N, class I = std::make_integer_sequence<T, N>>
struct factorial;
template <class T, T N, T... Is>
struct factorial<T,N,std::index_sequence<T, Is...>> {
   static constexpr T value = (static_cast<T>(1) * ... * (Is + 1));
};
int main() {
   std::cout << factorial<int, 5>::value << std::endl;
}
Section 16.2: Iterating over a parameter pack
Often, we need to perform an operation over every element in a variadic template parameter pack. There are many
ways to do this, and the solutions get easier to read and write with C++17. Suppose we simply want to print every
element in a pack. The simplest solution is to recurse:
Version ≥ C++11
```cpp
void print_all(std::ostream& os) {
    // base case
}
template <class T, class... Ts>
void print_all(std::ostream& os, T const& first, Ts const&... rest) {
    os << first;
    print_all(os, rest...);
}
```
We could instead use the expander trick, to perform all the streaming in a single function. This has the advantage of
not needing a second overload, but has the disadvantage of less than stellar readability:
Version ≥ C++11
```cpp
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    using expander = int[];
    (void)expander{0,
        (void(os << args), 0)...
    };
}
```
For an explanation of how this works, see T.C's excellent answer.
Version ≥ C++17
With C++17, we get two powerful new tools in our arsenal for solving this problem. The ﬁrst is a fold-expression:

```cpp
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    ((os << args), ...);
}
```
And the second is if constexpr, which allows us to write our original recursive solution in a single function:
```cpp
template <class T, class... Ts>
void print_all(std::ostream& os, T const& first, Ts const&... rest) {
    os << first;
    if constexpr (sizeof...(rest) > 0) {
        // this line will only be instantiated if there are further
        // arguments. if rest... is empty, there will be no call to
        // print_all(os).
        print_all(os, rest...);
    }
}
```
Section 16.3: Iterating with std::integer_sequence
Since C++14, the standard provides the class template
```cpp
template <class T, T... Ints>
class integer_sequence;
template <std::size_t... Ints>
using index_sequence = std::integer_sequence<std::size_t, Ints...>;
```
and a generating metafunction for it:
```cpp
template <class T, T N>
using make_integer_sequence = std::integer_sequence<T, /* a sequence 0, 1, 2, ..., N-1 */ >;
template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;
```
While this comes standard in C++14, this can be implemented using C++11 tools.
We can use this tool to call a function with a std::tuple of arguments (standardized in C++17 as std::apply):
```cpp
namespace detail {
    template <class F, class Tuple, std::size_t... Is>
    decltype(auto) apply_impl(F&& f, Tuple&& tpl, std::index_sequence<Is...> ) {
        return std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(tpl))...);
    }
}
template <class F, class Tuple>
decltype(auto) apply(F&& f, Tuple&& tpl) {
    return detail::apply_impl(std::forward<F>(f),
        std::forward<Tuple>(tpl),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
// this will print 3
int f(int, char, double);

auto some_args = std::make_tuple(42, 'x', 3.14);
int r = apply(f, some_args); // calls f(42, 'x', 3.14)
```
Section 16.4: Tag Dispatching
A simple way of selecting between functions at compile time is to dispatch a function to an overloaded pair of
functions that take a tag as one (usually the last) argument. For example, to implement std::advance(), we can
dispatch on the iterator category:
```cpp
namespace details {
    template <class RAIter, class Distance>
    void advance(RAIter& it, Distance n, std::random_access_iterator_tag) {
        it += n;
    }
    template <class BidirIter, class Distance>
    void advance(BidirIter& it, Distance n, std::bidirectional_iterator_tag) {
        if (n > 0) {
            while (n--) ++it;
        }
        else {
            while (n++) --it;
        }
    }
    template <class InputIter, class Distance>
    void advance(InputIter& it, Distance n, std::input_iterator_tag) {
        while (n--) {
            ++it;
        }
    }
}
template <class Iter, class Distance>
void advance(Iter& it, Distance n) {
    details::advance(it, n,
            typename std::iterator_traits<Iter>::iterator_category{} );
}
```
The std::XY_iterator_tag arguments of the overloaded details::advance functions are unused function
parameters. The actual implementation does not matter (actually it is completely empty). Their only purpose is to
allow the compiler to select an overload based on which tag class details::advance is called with.
In this example, advance uses the iterator_traits<T>::iterator_category metafunction which returns one of
the iterator_tag classes, depending on the actual type of Iter. A default-constructed object of the
iterator_category<Iter>::type then lets the compiler select one of the diﬀerent overloads of details::advance.
(This function parameter is likely to be completely optimized away, as it is a default-constructed object of an empty
struct and never used.)
Tag dispatching can give you code that's much easier to read than the equivalents using SFINAE and enable_if.
Note: while C++17's if constexpr may simplify the implementation of advance in particular, it is not suitable for open
implementations unlike tag dispatching.
Section 16.5: Detect Whether Expression is Valid
It is possible to detect whether an operator or function can be called on a type. To test if a class has an overload of

std::hash, one can do this:
```cpp
#include <functional> // for std::hash
#include <type_traits> // for std::false_type and std::true_type
#include <utility> // for std::declval
template<class, class = void>
struct has_hash
    : std::false_type
{};
template<class T>
struct has_hash<T, decltype(std::hash<T>()(std::declval<T>()), void())>
    : std::true_type
{};
```
Version ≥ C++17
Since C++17, std::void_t can be used to simplify this type of construct
```cpp
#include <functional> // for std::hash
#include <type_traits> // for std::false_type, std::true_type, std::void_t
#include <utility> // for std::declval
template<class, class = std::void_t<> >
struct has_hash
    : std::false_type
{};
template<class T>
struct has_hash<T, std::void_t< decltype(std::hash<T>()(std::declval<T>())) > >
    : std::true_type
{};
```
where std::void_t is deﬁned as:
```cpp
template< class... > using void_t = void;
```
For detecting if an operator, such as operator< is deﬁned, the syntax is almost the same:
```cpp
template<class, class = void>
struct has_less_than
    : std::false_type
{};
template<class T>
struct has_less_than<T, decltype(std::declval<T>() < std::declval<T>(), void())>
    : std::true_type
{};
```
These can be used to use a std::unordered_map<T> if T has an overload for std::hash, but otherwise attempt to
use a std::map<T>:
```cpp
template <class K, class V>
using hash_invariant_map = std::conditional_t<
    has_hash<K>::value,
    std::unordered_map<K, V>,
    std::map<K,V>>;
```

Section 16.6: If-then-else
Version ≥ C++11
The type std::conditional in the standard library header <type_traits> can select one type or the other, based
on a compile-time boolean value:
```cpp
template<typename T>
struct ValueOrPointer
{
    typename std::conditional<(sizeof(T) > sizeof(void*)), T*, T>::type vop;
};
```
This struct contains a pointer to T if T is larger than the size of a pointer, or T itself if it is smaller or equal to a
pointer's size. Therefore sizeof(ValueOrPointer) will always be <= sizeof(void*).
Section 16.7: Manual distinction of types when given any type
T
When implementing SFINAE using std::enable_if, it is often useful to have access to helper templates that
determines if a given type T matches a set of criteria.
To help us with that, the standard already provides two types analog to true and false which are std::true_type
and std::false_type.
The following example show how to detect if a type T is a pointer or not, the is_pointer template mimic the
behavior of the standard std::is_pointer helper:
```cpp
template <typename T>
struct is_pointer_: std::false_type {};
template <typename T>
struct is_pointer_<T*>: std::true_type {};
template <typename T>
struct is_pointer: is_pointer_<typename std::remove_cv<T>::type> { }
```
There are three steps in the above code (sometimes you only need two):
1.
The ﬁrst declaration of is_pointer_ is the default case, and inherits from std::false_type. The default case
should always inherit from std::false_type since it is analogous to a "false condition".
2.
The second declaration specialize the is_pointer_ template for pointer T* without caring about what T is
really. This version inherits from std::true_type.
3.
The third declaration (the real one) simply remove any unnecessary information from T (in this case we
remove const and volatile qualiﬁers) and then fall backs to one of the two previous declarations.
Since is_pointer<T> is a class, to access its value you need to either:
Use ::value, e.g. is_pointer<int>::value – value is a static class member of type bool inherited from
std::true_type or std::false_type;
Construct an object of this type, e.g. is_pointer<int>{} – This works because std::is_pointer inherits its
default constructor from std::true_type or std::false_type (which have constexpr constructors) and both
std::true_type and std::false_type have constexpr conversion operators to bool.

It is a good habit to provides "helper helper templates" that let you directly access the value:
```cpp
template <typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;
```
Version ≥ C++17
In C++17 and above, most helper templates already provide a _v version, e.g.:
```cpp
template< class T > constexpr bool is_pointer_v = is_pointer<T>::value;
template< class T > constexpr bool is_reference_v = is_reference<T>::value;
```
Section 16.8: Calculating power with C++11 (and higher)
With C++11 and higher calculations at compile time can be much easier. For example calculating the power of a
given number at compile time will be following:
```cpp
template <typename T>
constexpr T calculatePower(T value, unsigned power) {
    return power == 0 ? 1 : value * calculatePower(value, power-1);
}
```
Keyword constexpr is responsible for calculating function in compilation time, then and only then, when all the
requirements for this will be met (see more at constexpr keyword reference) for example all the arguments must
be known at compile time.
Note: In C++11 constexpr function must compose only from one return statement.
Advantages: Comparing this to the standard way of compile time calculation, this method is also useful for runtime
calculations. It means, that if the arguments of the function are not known at the compilation time (e.g. value and
power are given as input via user), then function is run in a compilation time, so there's no need to duplicate a code
(as we would be forced in older standards of C++).
E.g.
```cpp
void useExample() {
    constexpr int compileTimeCalculated = calculatePower(3, 3); // computes at compile time,
                               // as both arguments are known at compilation time
                               // and used for a constant expression.
    int value;
    std::cin >> value;
    int runtimeCalculated = calculatePower(value, 3);  // runtime calculated,
                                    // because value is known only at runtime.
}
```
Version ≥ C++17
Another way to calculate power at compile time can make use of fold expression as follows:
```cpp
#include <iostream>
#include <utility>
template <class T, T V, T N, class I = std::make_integer_sequence<T, N>>
struct power;
template <class T, T V, T N, T... Is>
struct power<T, V, N, std::integer_sequence<T, Is...>> {
   static constexpr T value = (static_cast<T>(1) * ... * (V * static_cast<bool>(Is + 1)));
};

int main() {
   std::cout << power<int, 4, 2>::value << std::endl;
}
```
Section 16.9: Generic Min/Max with variable argument count
Version > C++11
It's possible to write a generic function (for example min) which accepts various numerical types and arbitrary
argument count by template meta-programming. This function declares a min for two arguments and recursively
for more.
```cpp
template <typename T1, typename T2>
auto min(const T1 &a, const T2 &b)
-> typename std::common_type<const T1&, const T2&>::type
{
    return a < b ? a : b;
}
template <typename T1, typename T2, typename ... Args>
auto min(const T1 &a, const T2 &b, const Args& ... args)
-> typename std::common_type<const T1&, const T2&, const Args& ...>::type
{
    return min(min(a, b), args...);
}
auto minimum = min(4, 5.8f, 3, 1.8, 3, 1.1, 9);
```



##### SFINAE (Substitution Failure Is

Not An Error)
Section 103.1: What is SFINAE
SFINAE stands for Substitution Failure Is Not An Error. Ill-formed code that results from substituting types (or
values) to instantiate a function template or a class template is not a hard compile error, it is only treated as a
deduction failure.
Deduction failures on instantiating function templates or class template specializations remove that candidate from
the set of consideration - as if that failed candidate did not exist to begin with.
template <class T>
auto begin(T& c) -> decltype(c.begin()) { return c.begin(); }
template <class T, size_t N>
T* begin(T (&arr)[N]) { return arr; }
int vals[10];
begin(vals); // OK. The first function template substitution fails because
             // vals.begin() is ill-formed. This is not an error! That function
             // is just removed from consideration as a viable overload candidate,
             // leaving us with the array overload.
Only substitution failures in the immediate context are considered deduction failures, all others are considered
hard errors.
template <class T>
void add_one(T& val) { val += 1; }
int i = 4;
add_one(i); // ok
std::string msg = "Hello";
add_one(msg); // error. msg += 1 is ill-formed for std::string, but this
              // failure is NOT in the immediate context of substituting T
Section 103.2: void_t
Version ≥ C++11
void_t is a meta-function that maps any (number of) types to type void. The primary purpose of void_t is to
facilitate writing of type traits.
std::void_t will be part of C++17, but until then, it is extremely straightforward to implement:
template <class...> using void_t = void;
Some compilers require a slightly diﬀerent implementation:
template <class...>
struct make_void { using type = void; };
template <typename... T>
using void_t = typename make_void<T...>::type;
The primary application of void_t is writing type traits that check validity of a statement. For example, let's check if
a type has a member function foo() that takes no arguments:
template <class T, class=void>
struct has_foo : std::false_type {};
template <class T>
struct has_foo<T, void_t<decltype(std::declval<T&>().foo())>> : std::true_type {};
How does this work? When I try to instantiate has_foo<T>::value, that will cause the compiler to try to look for the
best specialization for has_foo<T, void>. We have two options: the primary, and this secondary one which involves
having to instantiate that underlying expression:
If T does have a member function foo(), then whatever type that returns gets converted to void, and the
specialization is preferred to the primary based on the template partial ordering rules. So
has_foo<T>::value will be true
If T doesn't have such a member function (or it requires more than one argument), then substitution fails for
the specialization and we only have the primary template to fallback on. Hence, has_foo<T>::value is false.
A simpler case:
template<class T, class=void>
struct can_reference : std::false_type {};
template<class T>
struct can_reference<T, std::void_t<T&>> : std::true_type {};
this doesn't use std::declval or decltype.
You may notice a common pattern of a void argument. We can factor this out:
struct details {
  template<template<class...>class Z, class=void, class...Ts>
  struct can_apply:
    std::false_type
  {};
  template<template<class...>class Z, class...Ts>
  struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...>:
    std::true_type
  {};
};
template<template<class...>class Z, class...Ts>
using can_apply = details::can_apply<Z, void, Ts...>;
which hides the use of std::void_t and makes can_apply act like an indicator whether the type supplied as the
ﬁrst template argument is well-formed after substituting the other types into it. The previous examples may now be
rewritten using can_apply as:
template<class T>
using ref_t = T&;
template<class T>
using can_reference = can_apply<ref_t, T>;    // Is T& well formed for T?
and:
template<class T>
using dot_foo_r = decltype(std::declval<T&>().foo());
template<class T>
using can_dot_foo = can_apply< dot_foo_r, T >;    // Is T.foo() well formed for T?
which seems simpler than the original versions.
There are post-C++17 proposals for std traits similar to can_apply.
The utility of void_t was discovered by Walter Brown. He gave a wonderful presentation on it at CppCon 2016.
Section 103.3: enable_if
std::enable_if is a convenient utility to use boolean conditions to trigger SFINAE. It is deﬁned as:
template <bool Cond, typename Result=void>
struct enable_if { };
template <typename Result>
struct enable_if<true, Result> {
    using type = Result;
};
That is, enable_if<true, R>::type is an alias for R, whereas enable_if<false, T>::type is ill-formed as that
specialization of enable_if does not have a type member type.
std::enable_if can be used to constrain templates:
int negate(int i) { return -i; }
template <class F>
auto negate(F f) { return -f(); }
Here, a call to negate(1) would fail due to ambiguity. But the second overload is not intended to be used for
integral types, so we can add:
int negate(int i) { return -i; }
template <class F, class = typename std::enable_if<!std::is_arithmetic<F>::value>::type>
auto negate(F f) { return -f(); }
Now, instantiating negate<int> would result in a substitution failure since !std::is_arithmetic<int>::value is
false. Due to SFINAE, this is not a hard error, this candidate is simply removed from the overload set. As a result,
negate(1) only has one single viable candidate - which is then called.
When to use it
It's worth keeping in mind that std::enable_if is a helper on top of SFINAE, but it's not what makes SFINAE work in
the ﬁrst place. Let's consider these two alternatives for implementing functionality similar to std::size, i.e. an
overload set size(arg) that produces the size of a container or array:
// for containers
template<typename Cont>
auto size1(Cont const& cont) -> decltype( cont.size() );
// for arrays
template<typename Elt, std::size_t Size>
std::size_t size1(Elt const(&arr)[Size]);
// implementation omitted
template<typename Cont>
struct is_sizeable;
// for containers
template<typename Cont, std::enable_if_t<std::is_sizeable<Cont>::value, int> = 0>
auto size2(Cont const& cont);
// for arrays
template<typename Elt, std::size_t Size>
std::size_t size2(Elt const(&arr)[Size]);
Assuming that is_sizeable is written appropriately, these two declarations should be exactly equivalent with
respect to SFINAE. Which is the easiest to write, and which is the easiest to review and understand at a glance?
Now let's consider how we might want to implement arithmetic helpers that avoid signed integer overﬂow in favour
of wrap around or modular behaviour. Which is to say that e.g. incr(i, 3) would be the same as i += 3 save for
the fact that the result would always be deﬁned even if i is an int with value INT_MAX. These are two possible
alternatives:
// handle signed types
template<typename Int>
auto incr1(Int& target, Int amount)
-> std::void_t<int[static_cast<Int>(-1) < static_cast<Int>(0)]>;
// handle unsigned types by just doing target += amount
// since unsigned arithmetic already behaves as intended
template<typename Int>
auto incr1(Int& target, Int amount)
-> std::void_t<int[static_cast<Int>(0) < static_cast<Int>(-1)]>;
template<typename Int, std::enable_if_t<std::is_signed<Int>::value, int> = 0>
void incr2(Int& target, Int amount);
template<typename Int, std::enable_if_t<std::is_unsigned<Int>::value, int> = 0>
void incr2(Int& target, Int amount);
Once again which is the easiest to write, and which is the easiest to review and understand at a glance?
A strength of std::enable_if is how it plays with refactoring and API design. If is_sizeable<Cont>::value is
meant to reﬂect whether cont.size() is valid then just using the expression as it appears for size1 can be more
concise, although that could depend on whether is_sizeable would be used in several places or not. Contrast that
with std::is_signed which reﬂects its intention much more clearly than when its implementation leaks into the
declaration of incr1.
Section 103.4: is_detected
To generalize type_trait creation:based on SFINAE there are experimental traits detected_or, detected_t,
is_detected.
With template parameters typename Default, template <typename...> Op and typename ... Args:
is_detected: alias of std::true_type or std::false_type depending of the validity of Op<Args...>
detected_t: alias of Op<Args...> or nonesuch depending of validity of Op<Args...>.
detected_or: alias of a struct with value_t which is is_detected, and type which is Op<Args...> or Default
depending of validity of Op<Args...>
which can be implemented using std::void_t for SFINAE as following:
Version ≥ C++17
namespace detail {
    template <class Default, class AlwaysVoid,
              template<class...> class Op, class... Args>
    struct detector
    {
        using value_t = std::false_type;
        using type = Default;
    };
    template <class Default, template<class...> class Op, class... Args>
    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
    {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };
} // namespace detail
// special type to indicate detection failure
struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};
template <template<class...> class Op, class... Args>
using is_detected =
    typename detail::detector<nonesuch, void, Op, Args...>::value_t;
template <template<class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;
template <class Default, template<class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;
Traits to detect presence of method can then be simply implemented:
typename <typename T, typename ...Ts>
using foo_type = decltype(std::declval<T>().foo(std::declval<Ts>()...));
struct C1 {};
struct C2 {
    int foo(char) const;
};
template <typename T>
using has_foo_char = is_detected<foo_type, T, char>;
static_assert(!has_foo_char<C1>::value, "Unexpected");
static_assert(has_foo_char<C2>::value, "Unexpected");
static_assert(std::is_same<int, detected_t<foo_type, C2, char>>::value,
              "Unexpected");
static_assert(std::is_same<void, // Default
                           detected_or<void, foo_type, C1, char>>::value,
              "Unexpected");
static_assert(std::is_same<int, detected_or<void, foo_type, C2, char>>::value,
              "Unexpected");
Section 103.5: Overload resolution with a large number of
options
If you need to select between several options, enabling just one via enable_if<> can be quite cumbersome, since
several conditions needs to be negated too.
The ordering between overloads can instead be selected using inheritance, i.e. tag dispatch.
Instead of testing for the thing that needs to be well-formed, and also testing the negation of all the other versions
conditions, we instead test just for what we need, preferably in a decltype in a trailing return.
This might leave several option well formed, we diﬀerentiate between those using 'tags', similar to iterator-trait tags
(random_access_tag et al). This works because a direct match is better that a base class, which is better that a base
class of a base class, etc.



### 5.2 The Death of SFINAE?

`if constexpr` replaces SFINAE for *implementation details*, but not for *overload resolution*.

**SFINAE (Old Way - C++11/14):**
```cpp
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
gcd(T a, T b) { return b == 0 ? a : gcd(b, a % b); }

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type
gcd(T a, T b) { static_assert(std::is_integral<T>::value, "GCD not for floats"); }
```

**if constexpr (New Way - C++17):**
```cpp
template <typename T>
T gcd(T a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return b == 0 ? a : gcd(b, a % b);
    } else {
        static_assert(always_false<T>, "GCD not for floats");
    }
}
```
*Result*: Much cleaner, easier to debug errors.

***



### 2.1 SFINAE - Substitution Failure Is Not An Error

SFINAE enables overload resolution based on template parameter substitution.



### 3. CONSTEXPR (INTRODUCTION)

`constexpr` functions can be evaluated at compile-time.

```cpp
constexpr int square(int x) { return x * x; }

int array[square(5)]; // Valid! Size 25 at compile time.
```

In C++11, `constexpr` functions were very limited (single return statement). C++14 relaxed this.


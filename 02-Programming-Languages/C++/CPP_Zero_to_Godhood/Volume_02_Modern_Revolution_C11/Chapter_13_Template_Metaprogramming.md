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

### 1.1 Parameter Packing (`...`)
*   `typename... Args`: Template parameter pack.
*   `Args... args`: Function parameter pack.
*   `sizeof...(Args)`: Number of arguments.

---

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

---

## 3. `using` Aliases

Replaces `typedef`, works with templates.

```cpp
template<typename T>
using Dictionary = std::map<std::string, T>;

Dictionary<int> scores;
```

---

## 4. `std::tuple`

Generalization of `std::pair` to N elements.

```cpp
#include <tuple>

std::tuple<int, double, std::string> t(1, 3.14, "hi");
int i = std::get<0>(t);
```


# Professional Notes: Chapter 13: C++ Streams

Section 13.1: String streams
std::ostringstream is a class whose objects look like an output stream (that is, you can write to them via
operator<<), but actually store the writing results, and provide them in the form of a stream.
Consider the following short code:
#include <sstream>
#include <string>                                                                                  
using namespace std;
int main()
{
    ostringstream ss;
    ss << "the answer to everything is " << 42;
    const string result = ss.str();
}  
The line
ostringstream ss;
creates such an object. This object is ﬁrst manipulated like a regular stream:
ss << "the answer to everything is " << 42;
Following that, though, the resulting stream can be obtained like this:
const string result = ss.str();
(the string result will be equal to "the answer to everything is 42").
This is mainly useful when we have a class for which stream serialization has been deﬁned, and for which we want a
string form. For example, suppose we have some class
class foo
{  
    // All sort of stuff here.
};  
ostream &operator<<(ostream &os, const foo &f);
To get the string representation of a foo object,
foo f;
we could use
ostringstream ss;
ss << f;
const string result = ss.str();        
Then result contains the string representation of the foo object.
Section 13.2: Printing collections with iostream
Basic printing
std::ostream_iterator allows to print contents of an STL container to any output stream without explicit loops.
The second argument of std::ostream_iterator constructor sets the delimiter. For example, the following code:
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " ! "));
will print
1 ! 2 ! 3 ! 4 !
Implicit type cast
std::ostream_iterator allows to cast container's content type implicitly. For example, let's tune std::cout to print
ﬂoating-point values with 3 digits after decimal point:
std::cout << std::setprecision(3);
std::fixed(std::cout);
and instantiate std::ostream_iterator with float, while the contained values remain int:
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<float>(std::cout, " ! "));
so the code above yields
1.000 ! 2.000 ! 3.000 ! 4.000 !
despite std::vector holds ints.
Generation and transformation
std::generate, std::generate_n and std::transform functions provide a very powerful tool for on-the-ﬂy data
manipulation. For example, having a vector:
std::vector<int> v = {1,2,3,4,8,16};
we can easily print boolean value of "x is even" statement for each element:
std::boolalpha(std::cout); // print booleans alphabetically
std::transform(v.begin(), v.end(), std::ostream_iterator<bool>(std::cout, " "),
[](int val) {
    return (val % 2) == 0;
});
or print the squared element:
std::transform(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});
Printing N space-delimited random numbers:
const int N = 10;
std::generate_n(std::ostream_iterator<int>(std::cout, " "), N, std::rand);
Arrays
As in the section about reading text ﬁles, almost all these considerations may be applied to native arrays. For
example, let's print squared values from a native array:
int v[] = {1,2,3,4,8,16};
std::transform(v, std::end(v), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});

# Professional Notes: Chapter 16: Metaprogramming

Section 16.1: Calculating Factorials 
Section 16.2: Iterating over a parameter pack 
Section 16.3: Iterating with std::integer_sequence 
Section 16.4: Tag Dispatching 
Section 16.5: Detect Whether Expression is Valid 
Section 16.6: If-then-else 
Section 16.7: Manual distinction of types when given any type T 
Section 16.8: Calculating power with C++11 (and higher) 
Section 16.9: Generic Min/Max with variable argument count 

# Professional Notes: Chapter 16: Metaprogramming

In C++ Metaprogramming refers to the use of macros or templates to generate code at compile-time.
In general, macros are frowned upon in this role and templates are preferred, although they are not as generic.
Template metaprogramming often makes use of compile-time computations, whether via templates or constexpr
functions, to achieve its goals of generating code, however compile-time computations are not metaprogramming
per se.
Section 16.1: Calculating Factorials
Factorials can be computed at compile-time using template metaprogramming techniques.
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
int my_array[factorial<5>::value];
Automatic arrays must have a compile-time deﬁned size. And the result of a metafunction is a compile-time
constant, so it can be used here.
Limitation: Most of the compilers won't allow recursion depth beyond a limit. For example, g++ compiler by default
limits recursion depeth to 256 levels. In case of g++, programmer can set recursion depth using -ftemplate-depth-
X option.
Version ≥ C++11
Since C++11, the std::integral_constant template can be used for this kind of template computation:
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
Additionally, constexpr functions become a cleaner alternative.
#include <iostream>
constexpr long long factorial(long long n)
{
  return (n == 0) ? 1 : n * factorial(n - 1);
}
int main()
{
  char test[factorial(3)];
  std::cout << factorial(7) << '\n';
}
The body of factorial() is written as a single statement because in C++11 constexpr functions can only use a
quite limited subset of the language.
Version ≥ C++14
Since C++14, many restrictions for constexpr functions have been dropped and they can now be written much
more conveniently:
constexpr long long factorial(long long n)
{
  if (n == 0)
    return 1;
  else
    return n * factorial(n - 1);
}
Or even:
constexpr long long factorial(int n)
{
  long long result = 1;
  for (int i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}
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
void print_all(std::ostream& os) {
    // base case
}
template <class T, class... Ts>
void print_all(std::ostream& os, T const& first, Ts const&... rest) {
    os << first;
    print_all(os, rest...);
}
We could instead use the expander trick, to perform all the streaming in a single function. This has the advantage of
not needing a second overload, but has the disadvantage of less than stellar readability:
Version ≥ C++11
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    using expander = int[];
    (void)expander{0,
        (void(os << args), 0)...
    };
}
For an explanation of how this works, see T.C's excellent answer.
Version ≥ C++17
With C++17, we get two powerful new tools in our arsenal for solving this problem. The ﬁrst is a fold-expression:
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    ((os << args), ...);
}
And the second is if constexpr, which allows us to write our original recursive solution in a single function:
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
Section 16.3: Iterating with std::integer_sequence
Since C++14, the standard provides the class template
template <class T, T... Ints>
class integer_sequence;
template <std::size_t... Ints>
using index_sequence = std::integer_sequence<std::size_t, Ints...>;
and a generating metafunction for it:
template <class T, T N>
using make_integer_sequence = std::integer_sequence<T, /* a sequence 0, 1, 2, ..., N-1 */ >;
template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;
While this comes standard in C++14, this can be implemented using C++11 tools.
We can use this tool to call a function with a std::tuple of arguments (standardized in C++17 as std::apply):
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
Section 16.4: Tag Dispatching
A simple way of selecting between functions at compile time is to dispatch a function to an overloaded pair of
functions that take a tag as one (usually the last) argument. For example, to implement std::advance(), we can
dispatch on the iterator category:
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
Version ≥ C++17
Since C++17, std::void_t can be used to simplify this type of construct
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
where std::void_t is deﬁned as:
template< class... > using void_t = void;
For detecting if an operator, such as operator< is deﬁned, the syntax is almost the same:
template<class, class = void>
struct has_less_than
    : std::false_type
{};
template<class T>
struct has_less_than<T, decltype(std::declval<T>() < std::declval<T>(), void())>
    : std::true_type
{};
These can be used to use a std::unordered_map<T> if T has an overload for std::hash, but otherwise attempt to
use a std::map<T>:
template <class K, class V>
using hash_invariant_map = std::conditional_t<
    has_hash<K>::value,
    std::unordered_map<K, V>,
    std::map<K,V>>;    
Section 16.6: If-then-else
Version ≥ C++11
The type std::conditional in the standard library header <type_traits> can select one type or the other, based
on a compile-time boolean value:
template<typename T>
struct ValueOrPointer
{
    typename std::conditional<(sizeof(T) > sizeof(void*)), T*, T>::type vop;
};
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
template <typename T>
struct is_pointer_: std::false_type {};
template <typename T>
struct is_pointer_<T*>: std::true_type {};
template <typename T>
struct is_pointer: is_pointer_<typename std::remove_cv<T>::type> { }
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
template <typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;
Version ≥ C++17
In C++17 and above, most helper templates already provide a _v version, e.g.:
template< class T > constexpr bool is_pointer_v = is_pointer<T>::value;
template< class T > constexpr bool is_reference_v = is_reference<T>::value;
Section 16.8: Calculating power with C++11 (and higher)
With C++11 and higher calculations at compile time can be much easier. For example calculating the power of a
given number at compile time will be following:
template <typename T>
constexpr T calculatePower(T value, unsigned power) {
    return power == 0 ? 1 : value * calculatePower(value, power-1);
}
Keyword constexpr is responsible for calculating function in compilation time, then and only then, when all the
requirements for this will be met (see more at constexpr keyword reference) for example all the arguments must
be known at compile time.
Note: In C++11 constexpr function must compose only from one return statement.
Advantages: Comparing this to the standard way of compile time calculation, this method is also useful for runtime
calculations. It means, that if the arguments of the function are not known at the compilation time (e.g. value and
power are given as input via user), then function is run in a compilation time, so there's no need to duplicate a code
(as we would be forced in older standards of C++).
E.g.
void useExample() {
    constexpr int compileTimeCalculated = calculatePower(3, 3); // computes at compile time,
                               // as both arguments are known at compilation time
                               // and used for a constant expression.
    int value;
    std::cin >> value;
    int runtimeCalculated = calculatePower(value, 3);  // runtime calculated,
                                    // because value is known only at runtime.
}
Version ≥ C++17
Another way to calculate power at compile time can make use of fold expression as follows:
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
Section 16.9: Generic Min/Max with variable argument count
Version > C++11
It's possible to write a generic function (for example min) which accepts various numerical types and arbitrary
argument count by template meta-programming. This function declares a min for two arguments and recursively
for more.
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
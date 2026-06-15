# CHAPTER 12: FUNCTIONAL PROGRAMMING


# FUNCTIONAL PROGRAMMING IN C++11

## 1. Lambda Expressions

Anonymous functions defined inline.

### 1.1 Basic Syntax
`[captures](params) -> return_type { body }`

```cpp
auto add = [](int a, int b) { return a + b; };
int result = add(2, 3);
```

### 1.2 Capture Lists
Controls access to outer scope variables.

*   `[]`: No capture.
*   `[x]`: Capture `x` by value (copy).
*   `[&x]`: Capture `x` by reference.
*   `[=]`: Capture all by value.
*   `[&]`: Capture all by reference.
*   `[this]`: Capture class members.

```cpp
int x = 10;
auto addX = [x](int y) { return x + y; }; // x is read-only inside
```

### 1.3 Mutable Lambdas
By default, value captures are `const`. `mutable` allows modification.

```cpp
int x = 0;
auto increment = [x]() mutable { return ++x; }; // Modifies local copy
```

---

## 2. `std::function`

A polymorphic wrapper for any callable (function pointer, lambda, functor).

```cpp
#include <functional>

void freeFunc(int) {}

std::function<void(int)> f;
f = freeFunc;
f = [](int x) {};
```

**Cost:** Can incur heap allocation and virtual call overhead.

---

## 3. `std::bind`

Binds arguments to function parameters (Partial Application).

```cpp
int add(int a, int b) { return a + b; }

// Creates a function taking 1 argument (placeholder _1)
auto add5 = std::bind(add, 5, std::placeholders::_1);
// add5(10) calls add(5, 10)
```

*Note: Lambdas largely replace `std::bind` in modern C++ due to readability and optimization.*

# Professional Notes: Chapter 52: std::function: To wrap any element that is callable

Section 52.1: Simple usage
Section 52.2: std::function used with std::bind
Section 52.3: Binding std::function to a dierent callable types
Section 52.4: Storing function arguments in std::tuple
Section 52.5: std::function with lambda and std::bind
Section 52.6: `function` overhead

# Professional Notes: Chapter 73: Lambdas

Section 73.1: What is a lambda expression?
Section 73.2: Specifying the return type
Section 73.3: Capture by value
Section 73.4: Recursive lambdas
Section 73.5: Default capture
Section 73.6: Class lambdas and capture of this
Section 73.7: Capture by reference
Section 73.8: Generic lambdas
Section 73.9: Using lambdas for inline parameter pack unpacking
Section 73.10: Generalized capture
Section 73.11: Conversion to function pointer
Section 73.12: Porting lambda functions to C++03 using functors

# Professional Notes: Chapter 52: std::function: To wrap any

element that is callable
Section 52.1: Simple usage
#include <iostream>
#include <functional>
std::function<void(int , const std::string&)> myFuncObj;
void theFunc(int i, const std::string& s)
{
    std::cout << s << ": " << i << std::endl;
}
int main(int argc, char *argv[])
{
    myFuncObj = theFunc;
    myFuncObj(10, "hello world");
}
Section 52.2: std::function used with std::bind
Think about a situation where we need to callback a function with arguments. std::function used with std::bind
gives a very powerful design construct as shown below.
class A
{
public:
    std::function<void(int, const std::string&)> m_CbFunc = nullptr;
    void foo()
    {
        if (m_CbFunc)
        {
            m_CbFunc(100, "event fired");
        }
    }
};
class B
{
public:
    B()
    {
        auto aFunc = std::bind(&B::eventHandler, this, std::placeholders::_1,
std::placeholders::_2);
        anObjA.m_CbFunc = aFunc;
    }
    void eventHandler(int i, const std::string& s)
    {
        std::cout << s << ": " << i << std::endl;
    }
    void DoSomethingOnA()
    {
        anObjA.foo();
    }
    A anObjA;
};
int main(int argc, char *argv[])
{
     B anObjB;
     anObjB.DoSomethingOnA();
}
Section 52.3: Binding std::function to a dierent callable
types
/*
 * This example show some ways of using std::function to call
 *  a) C-like function
 *  b) class-member function
 *  c) operator()
 *  d) lambda function
 *
 * Function call can be made:
 *  a) with right arguments
 *  b) argumens with different order, types and count
 */
#include <iostream>
#include <functional>
#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using namespace std::placeholders;
// simple function to be called
double foo_fn(int x, float y, double z)
{
  double res = x + y + z;
  std::cout << "foo_fn called with arguments: "
            << x << ", " << y << ", " << z
            << " result is : " << res
            << std::endl;
  return res;
}
// structure with member function to call
struct foo_struct
{
    // member function to call
    double foo_fn(int x, float y, double z)
    {
        double res = x + y + z;
        std::cout << "foo_struct::foo_fn called with arguments: "
                << x << ", " << y << ", " << z
                << " result is : " << res
                << std::endl;
        return res;
    }
    // this member function has different signature - but it can be used too
    // please not that argument order is changed too
    double foo_fn_4(int x, double z, float y, long xx)
    {
        double res = x + y + z + xx;
        std::cout << "foo_struct::foo_fn_4 called with arguments: "
                << x << ", " << z << ", " << y << ", " << xx
                << " result is : " << res
                << std::endl;
        return res;
    }
    // overloaded operator() makes whole object to be callable
    double operator()(int x, float y, double z)
    {
        double res = x + y + z;
        std::cout << "foo_struct::operator() called with arguments: "
                << x << ", " << y << ", " << z
                << " result is : " << res
                << std::endl;
        return res;
    }
};
int main(void)
{
  // typedefs
  using function_type = std::function<double(int, float, double)>;
  // foo_struct instance
  foo_struct fs;
  // here we will store all binded functions
  std::vector<function_type> bindings;
  // var #1 - you can use simple function
  function_type var1 = foo_fn;
  bindings.push_back(var1);
  // var #2 - you can use member function
  function_type var2 = std::bind(&foo_struct::foo_fn, fs, _1, _2, _3);
  bindings.push_back(var2);
  // var #3 - you can use member function with different signature
  // foo_fn_4 has different count of arguments and types
  function_type var3 = std::bind(&foo_struct::foo_fn_4, fs, _1, _3, _2, 0l);
  bindings.push_back(var3);
  // var #4 - you can use object with overloaded operator()
  function_type var4 = fs;
  bindings.push_back(var4);
  // var #5 - you can use lambda function
  function_type var5 = [](int x, float y, double z)
    {
        double res = x + y + z;
        std::cout << "lambda  called with arguments: "
                << x << ", " << y << ", " << z
                << " result is : " << res
                << std::endl;
        return res;
    };
  bindings.push_back(var5);
  std::cout << "Test stored functions with arguments: x = 1, y = 2, z = 3"
            << std::endl;
  for (auto f : bindings)
      f(1, 2, 3);
}
Live
Output:
Test stored functions with arguments: x = 1, y = 2, z = 3
foo_fn called with arguments: 1, 2, 3 result is : 6
foo_struct::foo_fn called with arguments: 1, 2, 3 result is : 6
foo_struct::foo_fn_4 called with arguments: 1, 3, 2, 0 result is : 6
foo_struct::operator() called with arguments: 1, 2, 3 result is : 6
lambda  called with arguments: 1, 2, 3 result is : 6
Section 52.4: Storing function arguments in std::tuple
Some programs need so store arguments for future calling of some function.
This example shows how to call any function with arguments stored in std::tuple
#include <iostream>
#include <functional>
#include <tuple>
#include <iostream>
// simple function to be called
double foo_fn(int x, float y, double z)
{
   double res =  x + y + z;
   std::cout << "foo_fn called. x = " << x << " y = " << y << " z = " << z
             << " res=" << res;
   return res;
}
// helpers for tuple unrolling
template<int ...> struct seq {};
template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};
template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };
// invocation helper
template<typename FN, typename P, int ...S>
double call_fn_internal(const FN& fn, const P& params, const seq<S...>)
{
   return fn(std::get<S>(params) ...);
}
// call function with arguments stored in std::tuple
template<typename Ret, typename ...Args>
Ret call_fn(const std::function<Ret(Args...)>& fn,
            const std::tuple<Args...>& params)
{
    return call_fn_internal(fn, params, typename gens<sizeof...(Args)>::type());
}
int main(void)
{
  // arguments
  std::tuple<int, float, double> t = std::make_tuple(1, 5, 10);
  // function to call
  std::function<double(int, float, double)> fn = foo_fn;
  // invoke a function with stored arguments
  call_fn(fn, t);
}
Live
Output:
foo_fn called. x = 1 y = 5 z = 10 res=16
Section 52.5: std::function with lambda and std::bind
#include <iostream>
#include <functional>
using std::placeholders::_1; // to be used in std::bind example
int stdf_foobar (int x, std::function<int(int)> moo)
{
    return x + moo(x); // std::function moo called
}
int foo (int x) { return 2+x; }
int foo_2 (int x, int y) { return 9*x + y; }
int main()
{
    int a = 2;
    /* Function pointers */
    std::cout << stdf_foobar(a, &foo) << std::endl; // 6 ( 2 + (2+2) )
    // can also be: stdf_foobar(2, foo)
    /* Lambda expressions */
    /* An unnamed closure from a lambda expression can be
     * stored in a std::function object:
     */
    int capture_value = 3;
    std::cout << stdf_foobar(a,
                             [capture_value](int param) -> int { return 7 + capture_value * param;
})
              << std::endl;
    // result: 15 ==  value + (7 * capture_value * value) == 2 + (7 + 3 * 2)
    /* std::bind expressions */
    /* The result of a std::bind expression can be passed.
     * For example by binding parameters to a function pointer call:
     */
    int b = stdf_foobar(a, std::bind(foo_2, _1, 3));
    std::cout << b << std::endl;
    // b == 23 == 2 + ( 9*2 + 3 )
    int c = stdf_foobar(a, std::bind(foo_2, 5, _1));
    std::cout << c << std::endl;
    // c == 49 == 2 + ( 9*5 + 2 )
    return 0;
}
Section 52.6: `function` overhead
std::function can cause signicant overhead. Because std::function has [value semantics][1], it must copy or
move the given callable into itself. But since it can take callables of an arbitrary type, it will frequently have to
allocate memory dynamically to do this.
Some function implementations have so-called "small object optimization", where small types (like function
pointers, member pointers, or functors with very little state) will be stored directly in the function object. But even
this only works if the type is noexcept move constructible. Furthermore, the C++ standard does not require that all
implementations provide one.
Consider the following:
//Header file
using MyPredicate = std::function<bool(const MyValue &, const MyValue &)>;
void SortMyContainer(MyContainer &C, const MyPredicate &pred);
//Source file
void SortMyContainer(MyContainer &C, const MyPredicate &pred)
{
    std::sort(C.begin(), C.end(), pred);
}
A template parameter would be the preferred solution for SortMyContainer, but let us assume that this is not
possible or desirable for whatever reason. SortMyContainer does not need to store pred beyond its own call. And
yet, pred may well allocate memory if the functor given to it is of some non-trivial size.
function allocates memory because it needs something to copy/move into; function takes ownership of the
callable it is given. But SortMyContainer does not need to own the callable; it's just referencing it. So using function
here is overkill; it may be ecient, but it may not.
There is no standard library function type that merely references a callable. So an alternate solution will have to be
found, or you can choose to live with the overhead.
Also, function has no eective means to control where the memory allocations for the object come from. Yes, it
has constructors that take an allocator, but [many implementations do not implement them correctly... or even at
all][2].
Version  C++17
The function constructors that take an allocator no longer are part of the type. Therefore, there is no way to
manage the allocation.
Calling a function is also slower than calling the contents directly. Since any function instance could hold any
callable, the call through a function must be indirect. The overhead of calling function is on the order of a virtual
function call.

# Professional Notes: Chapter 73: Lambdas

Parameter
default-capture
Details
Species how all non-listed variables are captured. Can be = (capture by value) or & (capture by
reference). If omitted, non-listed variables are inaccessible within the lambda-body. The default-
capture must precede the capture-list.
capture-list
Species how local variables are made accessible within the lambda-body. Variables without
prex are captured by value. Variables prexed with & are captured by reference. Within a class
method, this can be used to make all its members accessible by reference. Non-listed variables
are inaccessible, unless the list is preceded by a default-capture.
argument-list
Species the arguments of the lambda function.
mutable
(optional) Normally variables captured by value are const. Specifying mutable makes them non-
const. Changes to those variables are retained between calls.
throw-specication
(optional) Species the exception throwing behavior of the lambda function. For example:
noexcept or throw(std::exception).
attributes
(optional) Any attributes for the lambda function. For example, if the lambda-body always throws
an exception then [[noreturn]] can be used.
-> return-type
(optional) Species the return type of the lambda function. Required when the return type
cannot be determined by the compiler.
lambda-body
A code block containing the implementation of the lambda function.
Section 73.1: What is a lambda expression?
A lambda expression provides a concise way to create simple function objects. A lambda expression is a prvalue
whose result object is called closure object, which behaves like a function object.
The name 'lambda expression' originates from lambda calculus, which is a mathematical formalism invented in the
1930s by Alonzo Church to investigate questions about logic and computability. Lambda calculus formed the basis
of LISP, a functional programming language. Compared to lambda calculus and LISP, C++ lambda expressions share
the properties of being unnamed, and to capture variables from the surrounding context, but they lack the ability to
operate on and return functions.
A lambda expression is often used as an argument to functions that take a callable object. That can be simpler than
creating a named function, which would be only used when passed as the argument. In such cases, lambda
expressions are generally preferred because they allow dening the function objects inline.
A lambda consists typically of three parts: a capture list [], an optional parameter list () and a body {}, all of which
can be empty:
[](){}                // An empty lambda, which does and returns nothing
Capture list
[] is the capture list. By default, variables of the enclosing scope cannot be accessed by a lambda. Capturing a
variable makes it accessible inside the lambda, either as a copy or as a reference. Captured variables become a part
of the lambda; in contrast to function arguments, they do not have to be passed when calling the lambda.
int a = 0;                       // Define an integer variable
auto f = []()   { return a*9; }; // Error: 'a' cannot be accessed
auto f = [a]()  { return a*9; }; // OK, 'a' is "captured" by value
auto f = [&a]() { return a++; }; // OK, 'a' is "captured" by reference
                                 //      Note: It is the responsibility of the programmer
                                 //      to ensure that a is not destroyed before the

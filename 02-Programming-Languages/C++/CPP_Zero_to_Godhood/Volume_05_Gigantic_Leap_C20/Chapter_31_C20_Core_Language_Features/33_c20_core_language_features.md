# Chapter 33: C++20 Core Language Features

# C++20 CORE LANGUAGE UPGRADES

Beyond the "Big Four," C++20 added essential tools for performance, safety, and syntactic clarity.

### 1. Comparison & Constant Expressions

*   **Three-way comparison (<=>)**: The "spaceship operator" compares two values and returns ordering (`strong_ordering`, etc.). Defaulting it generates all 6 comparison operators.
```cpp
    struct S {
        int x;
        auto operator<=>(const S&) const = default;
    };
```

*   **consteval**: Declares a function as an "immediate function" that MUST be evaluated at compile time.
```cpp
    consteval int sq(int x) { return x * x; }
```
*   **constinit**: Ensures a variable is initialized with a constant expression at static initialization; unlike `const`, it remains mutable.
```cpp
    constinit int counter = 0;
```
*   **constexpr virtual functions**: Virtual functions can now be `constexpr`, enabling compile-time polymorphism.
```cpp
    struct B { constexpr virtual int get() const = 0; };
```
*   **constexpr try-catch blocks**: `try/catch` is allowed in `constexpr` functions; the catch block is ignored at compile time.
*   **constexpr dynamic_cast and typeid**: Allowed inside `constexpr` evaluation.
*   **constexpr allocations**: `new/delete` are allowed in `constexpr` functions as long as memory is freed before evaluation ends.

### 2. Syntactic Ergonomics

*   **Designated initializers**: Struct members can be initialized by name in brace-init, matching C99 syntax.
```cpp
    Point p{.x = 1, .y = 2};
```
*   **Init-statements for range-based for**: A range-based for loop can have an initializer statement before the range expression.
```cpp
    for (auto& data = getData(); auto& x : data) { /* ... */ }
```
*   **using enum**: Injects all enumerator names from an enum class into the current scope.
```cpp
    using enum Color; 
    auto c = Red;
```


*   **Conditionally explicit constructors**: `explicit(bool_expr)` allows conditional explicitness.
```cpp
    template<class T> struct W { 
        explicit(!std::is_convertible_v<T,int>) W(T); 
    };
```


*   **Array size deduction in new-expressions**: `int* p = new int[]{1, 2, 3};`

### 3. Lambda Improvements

*   **Template parameter list for generic lambdas**: Explicit template syntax for finer control.
```cpp
    []<typename T>(std::vector<T> v) { /* use T */ };
```
*   **Lambda [=, this] capture**: Explicitly capture `this` by reference with implicit by-value captures.
*   **Pack expansion in lambda init-capture**: Direct capture of parameter packs.
```cpp
    [...args = std::forward<Args>(args)](){ f(args...); }
```
*   **Lambdas in unevaluated contexts**: Lambdas can appear in `decltype`, `sizeof`, etc.
*   **Default-constructible stateless lambdas**: Allows using lambda types as comparators directly in containers.
```cpp
    std::map<std::string, int, decltype([](auto& a, auto& b){ return a < b; })> m;
```



### 4. Attributes & Hardware Sympathy

*   **[[likely]] / [[unlikely]]**: Hints to the optimizer about branch probability.
```cpp
    if (x > 0) [[likely]] { fast_path(); }
```
*   **[[no_unique_address]]**: Allows a non-static data member to share address with others (optimization).
*   **[[nodiscard]] with message**: `[[nodiscard("check error code")]]`.
*   **char8_t**: A distinct type for UTF-8 character data.
*   **Signed integers are two's complement**: Now mandated by the standard.
*   **Deprecate some uses of volatile**: Compound assignment and increment on `volatile` are deprecated.

### 5. Template Power

*   **Class types as non-type template params**: Literal class types can be non-type template arguments.

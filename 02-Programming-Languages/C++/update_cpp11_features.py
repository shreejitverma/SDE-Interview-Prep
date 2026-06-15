import re

def update_chapters():
    with open("02-Programming-Languages/C++/Complete-CPP-Zero-to-Godhood.md", 'r', encoding='utf-8') as f:
        content = f.read()

    # --- CHAPTER 10 ---
    ch10_new = """## CHAPTER 10: THE MODERN C11 CORE

# THE MODERN C++11 CORE: SYNTAX & TYPE SYSTEM

C++11 was the most significant update to the language, fundamentally changing how we write and reason about code. Below are the definitive language features that established the modern era.

### 1. Type Deduction & Metadata
*   **auto type deduction**: The compiler deduces the type of a variable from its initializer; reduces verbosity especially with iterators.
    ```cpp
    auto x = 42; 
    auto it = v.begin();
    ```
*   **decltype**: Queries the declared type of an expression without evaluating it.
    ```cpp
    int x = 0; 
    decltype(x) y = 1;
    ```
*   **Trailing return types**: Return type is written after the parameter list using `->`, useful when the return type depends on parameters or for template consistency.
    ```cpp
    auto add(int a, int b) -> int { return a + b; }
    ```
*   **static_assert**: Compile-time assertion that stops compilation with a message if a condition is false.
    ```cpp
    static_assert(sizeof(int) >= 4, "int too small");
    ```
*   **Type aliases (using)**: A cleaner, more readable alternative to `typedef` that also supports alias templates.
    ```cpp
    using ll = long long; 
    template<class T> using Vec = std::vector<T>;
    ```
*   **sizeof on non-static members**: `sizeof` can now be used on a class member without needing an actual object instance.
    ```cpp
    sizeof(MyClass::member)
    ```

### 2. Modern Literals & Data Types
*   **nullptr**: A new null pointer constant replacing `0` and `NULL`, eliminating overload resolution ambiguity.
    ```cpp
    int* p = nullptr;
    ```
*   **Strongly typed enums (enum class)**: Scoped enumerations that don't leak names into the surrounding scope and prevent implicit integer conversion.
    ```cpp
    enum class Color { Red, Green, Blue }; 
    Color c = Color::Red;
    ```
*   **Strongly scoped enum with underlying type**: Enums can now specify their underlying storage type for explicit memory layout control.
    ```cpp
    enum class Status : uint8_t { OK=0, Err=1 };
    ```
*   **Enum forward declarations**: You can now forward-declare an enum as long as the underlying type is specified.
    ```cpp
    enum class Status : int;
    ```
*   **Raw string literals**: Strings without backslash escaping using `R"(...)"`.
    ```cpp
    std::string s = R"(C:\\temp\\new\\file.txt)";
    ```
*   **char16_t and char32_t**: Dedicated types for Unicode UTF-16 and UTF-32 code units.
    ```cpp
    char16_t c = u'a'; char32_t d = U'\\U00010348'; // 𐍈
    ```
*   **New string literals (u8, u, U)**: Prefix string literals for UTF-8, UTF-16, and UTF-32 encoding.
    ```cpp
    u8"hello"; u"hello"; U"hello";
    ```
*   **User-defined literals**: Attach custom meaning to literal suffixes for your own types.
    ```cpp
    long double operator"" _km(long double x){ return x*1000; }
    ```

### 3. Object & Constructor Improvements
*   **Initializer lists / uniform initialization**: Consistent brace initialization syntax for all types, introducing `std::initializer_list`.
    ```cpp
    std::vector<int> v{1,2,3}; 
    int a[]{4,5,6};
    ```
*   **Delegating constructors**: A constructor can now call another constructor in the same class to avoid code duplication.
    ```cpp
    struct A { 
        A() : A(0) {} 
        A(int x) : v(x) {} 
        int v; 
    };
    ```
*   **Inherited constructors**: `using Base::Base` imports all constructors from a base class into the derived class.
    ```cpp
    struct D : B { using B::B; };
    ```
*   **Non-static data member initializers**: Data members can be initialized directly where they are declared.
    ```cpp
    struct A { int x = 10; std::string s{"hi"}; };
    ```
*   **Defaulted functions (= default)**: Asks the compiler to generate the standard implementation of a special member function.
    ```cpp
    struct A { A() = default; A(const A&) = default; };
    ```
*   **Deleted functions (= delete)**: Explicitly forbids a function from being used (replaces private-unimplemented pattern).
    ```cpp
    struct A { A(const A&) = delete; };
    ```

### 4. Control Flow & Language Mechanics
*   **Range-based for loop**: Clean iteration over containers and arrays without explicit iterators.
    ```cpp
    for (auto& x : v) x *= 2;
    ```
*   **constexpr**: Variables, functions, and constructors evaluated at compile time; enables stronger optimization.
    ```cpp
    constexpr int square(int x) { return x*x; }
    constexpr double pi = 3.14159;
    ```
*   **noexcept**: Marks a function as non-throwing; critical for efficient move operations and optimizer hints.
    ```cpp
    void h() noexcept {}
    ```
*   **override**: Ensures a virtual function in a derived class actually overrides a base class method.
    ```cpp
    struct D:B { void f() override; };
    ```
*   **final**: Prevents further overriding of a virtual function or further inheritance from a class.
    ```cpp
    struct B { virtual void f() final; };
    ```
*   **Explicit conversion operators**: Conversion operators that only trigger when explicitly cast, not implicitly.
    ```cpp
    struct A { explicit operator bool() const { return true; } };
    ```
*   **Ref-qualified member functions**: Member functions can be overloaded based on whether `*this` is an lvalue or an rvalue.
    ```cpp
    struct S { void f() & {} void f() && {} };
    ```
*   **[[attributes]] syntax**: Standard double-bracket attribute syntax for metadata on declarations.
    ```cpp
    [[noreturn]] void fail(){ throw 1; }
    ```
*   **Right-angle bracket fix**: `>>` in nested templates no longer needs to be written as `> >`.
    ```cpp
    std::vector<std::vector<int>> grid;
    ```
*   **alignas / alignof**: Control and query alignment requirements of types or objects.
    ```cpp
    struct alignas(16) Vec4 { float x,y,z,w; };
    ```
*   **Inline namespaces**: Names in an inline namespace are visible from the enclosing namespace; useful for library versioning.
    ```cpp
    namespace api { inline namespace v1 { void f(){} } }
    ```
*   **Unrestricted unions**: Unions can now contain types with non-trivial members (except references).
    ```cpp
    union U { int i; double d; U():i(0){} };
    ```
"""
    # Replace content between Ch 10 and Ch 11
    content = re.sub(r'## CHAPTER 10: THE MODERN C11 CORE.*?## CHAPTER 11: MOVE SEMANTICS AND SMART POINTERS', 
                    ch10_new + "\n## CHAPTER 11: MOVE SEMANTICS AND SMART POINTERS", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 11 additions ---
    # Many are already there, I'll add the specific library features 41-46
    ch11_add = """### 2.5 Modern Smart Pointer Library
*   **std::unique_ptr**: Sole-ownership smart pointer with RAII; replaces raw new/delete.
    ```cpp
    auto p = std::unique_ptr<int>(new int(5));
    ```
*   **std::shared_ptr**: Reference-counted shared ownership smart pointer.
    ```cpp
    auto p = std::make_shared<int>(10);
    ```
*   **std::weak_ptr**: Non-owning observer of a `shared_ptr`; breaks reference cycles.
    ```cpp
    std::weak_ptr<int> w = p;
    ```
*   **std::make_shared**: Creates a `shared_ptr` with a single combined allocation (faster than new).
    ```cpp
    auto p = std::make_shared<MyClass>(args);
    ```
*   **std::move**: Casts an object to an rvalue so move construction/assignment is selected.
    ```cpp
    std::string b = std::move(a);
    ```
*   **std::forward**: Preserves the lvalue/rvalue-ness of an argument in forwarding-reference code.
    ```cpp
    template<class T> void wrap(T&& x){ use(std::forward<T>(x)); }
    ```
"""
    content = re.sub(r'## 3\. Perfect Forwarding.*?(?=\n# Professional Notes)', 
                    r'## 3. Perfect Forwarding\n\n' + ch11_add + "\n", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 12 additions ---
    ch12_add = """### 4. C++11 Functional Toolkit
*   **Lambda expressions**: Anonymous inline function objects with capture lists.
    ```cpp
    auto sq = [](int x){ return x * x; }; 
    auto add = [y](int x){ return x + y; };
    ```
*   **std::function**: Type-erased wrapper for any callable (function pointer, lambda, functor).
    ```cpp
    std::function<int(int)> f = [](int x){ return x; };
    ```
*   **std::bind**: Binds arguments to a callable, returning a new callable.
    ```cpp
    auto f = std::bind(std::plus<int>{}, _1, 10);
    ```
"""
    content = re.sub(r'## 3\. `std::bind`.*?(?=\n# Professional Notes)', 
                    r'## 3. `std::bind`\n\n' + ch12_add + "\n", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 13 additions ---
    ch13_add = """### 5. Advanced Template Features
*   **Variadic templates**: Templates accepting any number of type or function arguments via parameter packs.
    ```cpp
    template<class... Ts> void log(Ts... xs) {}
    ```
*   **Type traits (<type_traits>)**: Compile-time type property queries for metaprogramming.
    ```cpp
    static_assert(std::is_integral<int>::value, "");
    ```
*   **std::is_* type traits**: Traits like `is_pointer`, `is_class`, `is_same`, `is_base_of`, etc.
    ```cpp
    std::is_same<int, int>::value // true
    ```
*   **std::enable_if**: SFINAE helper to conditionally enable/disable templates.
    ```cpp
    template<class T, class=std::enable_if_t<std::is_integral_v<T>>> void f(T);
    ```
*   **std::declval**: Creates a fake reference to a type for use in `decltype` without constructing it.
    ```cpp
    decltype(std::declval<T>().member)
    ```
*   **Local/unnamed types as template args**: Local structs and anonymous types can now be used as template arguments.
    ```cpp
    void f(){ struct Local{}; std::vector<Local> v; }
    ```
*   **Extern templates**: Suppresses implicit template instantiation in a translation unit to reduce compile times.
    ```cpp
    extern template class std::vector<int>;
    ```
"""
    content = re.sub(r'## 4\. `std::tuple`.*?(?=\n# Professional Notes)', 
                    r'## 4. `std::tuple`\n\n' + ch13_add + "\n", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 14 additions ---
    ch14_add = """## 6. Comprehensive C++11 Library Features
*   **std::chrono**: Strongly typed clocks, durations, and time points.
    ```cpp
    auto t0 = std::chrono::steady_clock::now();
    ```
*   **std::tuple**: Heterogeneous fixed-size collection of values.
    ```cpp
    auto t = std::make_tuple(1, 2.5, "hi");
    ```
*   **std::tie**: Unpacks a tuple into named variables.
    ```cpp
    int a; double b; std::tie(a, b) = std::make_tuple(1, 2.5);
    ```
*   **std::array**: Fixed-size STL-style array with zero overhead over raw arrays.
    ```cpp
    std::array<int,3> a{{1,2,3}};
    ```
*   **std::forward_list**: Singly linked list optimized for minimal memory use.
    ```cpp
    std::forward_list<int> xs = {1,2,3};
    ```
*   **std::unordered_map / set**: Hash-table based containers with average O(1) lookup.
    ```cpp
    std::unordered_map<std::string,int> mp; mp["a"]=1;
    ```
*   **std::regex**: Standard regular expression library for matching and searching.
    ```cpp
    std::regex r("\\\\d+"); std::smatch m;
    ```
*   **std::begin / std::end**: Generic free functions working on arrays and containers.
    ```cpp
    auto it = std::begin(arr);
    ```
*   **std::to_string**: Converts numeric types to `std::string`.
    ```cpp
    std::string s = std::to_string(123);
    ```
*   **std::stoi / std::stof etc.**: String to numeric type conversions.
    ```cpp
    int n = std::stoi("42");
    ```
*   **std::initializer_list**: Sequence of elements used to initialize containers and user types via `{}`.
    ```cpp
    void f(std::initializer_list<int> il){ for(auto x:il){} }
    ```
*   **std::numeric_limits improvements**: Extended compile-time numeric boundary constants.
    ```cpp
    std::numeric_limits<double>::infinity()
    ```
*   **Random number library (<random>)**: Professional engines (mt19937) and distributions.
    ```cpp
    std::mt19937 rng(42); std::uniform_int_distribution<> dist(1,6);
    ```
*   **std::ratio**: Compile-time rational arithmetic used by `std::chrono`.
    ```cpp
    using half = std::ratio<1, 2>;
    ```
"""
    content = re.sub(r'## 5\. `std::random`.*?(?=\n## CHAPTER 15)', 
                    r'## 5. `std::random`\n\n' + ch14_add + "\n", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 15 additions ---
    ch15_add = """### 6. Modern Concurrency Tools
*   **std::thread**: Standard portable threads.
    ```cpp
    std::thread t([]{ work(); }); t.join();
    ```
*   **std::mutex**: Basic mutual exclusion primitive.
    ```cpp
    std::mutex m; std::lock_guard<std::mutex> lk(m);
    ```
*   **std::recursive_mutex**: Mutex that can be locked multiple times by the same thread.
    ```cpp
    std::recursive_mutex m; m.lock(); m.lock();
    ```
*   **std::timed_mutex**: Mutex with `try_lock_for` and `try_lock_until`.
    ```cpp
    m.try_lock_for(std::chrono::milliseconds(10));
    ```
*   **std::lock_guard**: RAII wrapper that locks on construction, unlocks on destruction.
    ```cpp
    std::lock_guard<std::mutex> lk(m);
    ```
*   **std::unique_lock**: Flexible mutex ownership wrapper supporting deferred/timed locking.
    ```cpp
    std::unique_lock<std::mutex> lk(m, std::defer_lock);
    ```
*   **std::condition_variable**: Allows threads to wait until notified by another thread.
    ```cpp
    cv.wait(lock, []{ return ready; });
    ```
*   **std::atomic<T>**: Atomic types for lock-free access to shared variables.
    ```cpp
    std::atomic<int> cnt{0}; cnt.fetch_add(1);
    ```
*   **std::future / std::promise**: Communicate results asynchronously between threads.
    ```cpp
    std::promise<int> p; auto f = p.get_future(); p.set_value(42);
    ```
*   **std::async**: Launches a callable asynchronously and returns a future.
    ```cpp
    auto f = std::async([]{ return compute(); });
    ```
*   **std::packaged_task**: Wraps a callable so its result can be retrieved via a future.
    ```cpp
    std::packaged_task<int()> task(compute);
    ```
*   **std::exception_ptr**: Stores and transfers exception objects between threads.
    ```cpp
    auto ep = std::current_exception(); std::rethrow_exception(ep);
    ```
"""
    content = re.sub(r'## 5\. Atomics.*?(?=\n---)', 
                    r'## 5. Atomics\n\n' + ch15_add, 
                    content, flags=re.DOTALL)

    with open("02-Programming-Languages/C++/Complete-CPP-Zero-to-Godhood.md", 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == "__main__":
    update_chapters()
    print("C++11 features integrated.")

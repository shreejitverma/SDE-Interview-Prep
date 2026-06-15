# C14 FUNCTIONS AND LAMBDAS


# C++14 FUNCTIONS & LAMBDAS

C++11 introduced lambdas, but C++14 made them "First Class Citizens." They gained the ability to be generic and to handle move-only types, making them indispensable for modern asynchronous and functional programming.

## 1. Generic Lambdas

In C++11, lambda parameters required concrete types. C++14 allows `auto` parameters, making the lambda's call operator a template.

### 1.1 The Internal Mechanics
When you write a generic lambda, the compiler generates a closure object with a templated `operator()`.

```cpp
auto sum = [](auto a, auto b) {
    return a + b;
};

// Effectively becomes:
struct __lambda_unique_name {
    template<typename T, typename U>
    auto operator()(T a, U b) const {
        return a + b;
    }
};
```

### 1.2 Polymorphic Behavior
Generic lambdas enable elegant, type-agnostic code without the boilerplate of traditional templates.

```cpp
auto printer = [](const auto& container) {
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << "\n";
};

std::vector<int> v = {1, 2, 3};
std::list<std::string> l = {"A", "B"};

printer(v); // Works for vector
printer(l); // Works for list
```

## 2. Lambda Init-Capture (Generalized Capture)

This is arguably the most important lambda upgrade. It allows you to create new variables in the capture clause, and more importantly, it enables **capturing move-only types** like `std::unique_ptr`.

### 2.1 Moving into a Lambda
In C++11, you couldn't move a `unique_ptr` into a lambda without ugly workarounds. C++14 solves this.

```cpp
auto data = std::make_unique<LargeBuffer>();

// Capture by move: 'p' is initialized by moving 'data'
auto task = [p = std::move(data)]() {
    p->process();
}; 

// 'data' is now null; 'p' lives inside the lambda object
```

### 2.2 Renaming Captures
You can also rename variables or capture the result of an expression.

```cpp
int x = 10;
auto check = [val = x + 5](int input) {
    return input > val;
};
```

## 3. Return Type Deduction & `decltype(auto)`

C++14 expanded return type deduction to all functions, not just lambdas.

### 3.1 Rules for `auto` Return Types
The function body must be visible to the compiler at the call site. If there are multiple `return` statements, they must all deduce to the same type.

```cpp
auto get_value(bool flag) {
    if (flag) return 42;    // Deduces int
    else      return 0;     // Deduces int
    // return 3.14;         // ERROR: inconsistent types (int vs double)
}
```

### 3.2 The `decltype(auto)` Powerhouse
Standard `auto` return type deduction uses template argument deduction rules, which means **references are stripped (decayed)**. `decltype(auto)` preserves the exact type, including references and const-qualifiers.

```cpp
int global_val = 100;

int& get_ref() { return global_val; }

// Returns by value (int)
auto proxy1() { return get_ref(); } 

// Returns by reference (int&) - Perfect Forwarding of Return Type
decltype(auto) proxy2() { return get_ref(); }

void test() {
    proxy1() = 200; // ERROR: modifying a temporary
    proxy2() = 200; // SUCCESS: modifies global_val
}
```

**Deep Dive:** Use `decltype(auto)` primarily in wrapper functions or generic code where you want to pass through the return type of another function exactly as it is, without knowing whether it returns by value or reference.

---

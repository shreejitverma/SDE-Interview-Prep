# Chapter 22: Standard Library Enhancements

# C++14 STANDARD LIBRARY ENHANCEMENTS

The C++14 library updates were targeted at consistency and fixing omissions from C++11. Most notably, it finally gave us `std::make_unique` and introduced the first standardized reader-writer lock.

## 1. `std::make_unique`: Completing the Set

In C++11, we had `std::make_shared` but no `std::make_unique`. This was a strange omission that forced developers to use `new` for unique pointers.

### 1.1 Why use `make_unique`?

1.  **Exception Safety:** Prevents memory leaks in complex expressions where multiple allocations occur.
2.  **No `new` Keyword:** Keeps code clean and adheres to the "No Raw New/Delete" modern C++ philosophy.
3.  **Efficiency:** While it doesn't offer the control-block optimization of `make_shared`, it is the standard way to construct unique pointers.

```cpp
// BAD: Potential leak if foo() throws
process(std::unique_ptr<T>(new T()), foo());

// GOOD: Exception safe
process(std::make_unique<T>(), foo());
```

## 2. `std::exchange`: Move Semantics Utility

`std::exchange` replaces the value of an object with a new value and returns the old value. It is particularly useful for implementing move constructors and move assignment operators.

```cpp
struct Node {
    int* data;
    Node(Node&& other) noexcept
        : data(std::exchange(other.data, nullptr)) {}

    Node& operator=(Node&& other) noexcept {
        if (this != &other) {
            delete data;
            data = std::exchange(other.data, nullptr);
        }
        return *this;
    }
};
```

## 3. `std::shared_timed_mutex` (Reader-Writer Lock)

One of the most requested features: a mutex that allows multiple readers OR one writer.

### 3.1 Performance Considerations

Use a shared mutex when:
-   Reads are frequent and cheap.
-   Writes are infrequent and expensive.

```cpp
#include <shared_mutex>
#include <map>

class ThreadSafeMap {
    std::map<int, std::string> data;
    mutable std::shared_timed_mutex mtx;

public:
    std::string get(int key) const {
        std::shared_lock lock(mtx); // Shared lock (Read)
        return data.at(key);
    }

    void set(int key, std::string val) {
        std::unique_lock lock(mtx); // Exclusive lock (Write)
        data[key] = std::move(val);
    }
};
```

## 4. `std::quoted`: Stream Parsing Hero

Parsing CSVs or logs with quoted strings used to be a nightmare of manual character escaping. `std::quoted` handles this automatically.

```cpp
#include <iomanip>
#include <sstream>

void test_quoted() {
    std::stringstream ss;
    std::string s = "Hello \"C++14\" World";

    ss << std::quoted(s);
    // ss now contains: "Hello \"C++14\" World" (with quotes and escaping)

    std::string output;
    ss >> std::quoted(output);
    // output now contains original string: Hello "C++14" World
}
```

# VOLUME 03: GODHOOD SUMMARY

C++14 was the release where **Modern C++ became "Fluid."**

1.  **Constexpr is King:** Logic migrated from runtime to compile-time. If it doesn't involve I/O or dynamic allocation, it should probably be `constexpr`.
2.  **Lambdas are Complete:** With Generic Lambdas and Init-Capture, lambdas are now the preferred tool for almost all local logic, closures, and callback patterns.
3.  **Standard Consistency:** `std::make_unique` and `_t/_v` aliases removed the "boilerplate friction" that made C++11 feel verbose.

**The Golden Rule of C++14:** If you find yourself writing a manual loop in a template or a manual move in a constructor, check if a C++14 utility (`integer_sequence`, `std::exchange`) can do it for you in one line.

# VOLUME 04 SIMPLIFICATION MODERNIZATION C17


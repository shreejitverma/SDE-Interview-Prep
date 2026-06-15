# CHAPTER 30: C20 CORE LANGUAGE FEATURES


# C++20 CORE LANGUAGE FEATURES

## 1. Three-Way Comparison (`<=>`)

The "Spaceship Operator". Generates all 6 comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`) automatically.

```cpp
#include <compare>

struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2}, p2{1, 3};
// p1 < p2 works!
// p1 == p2 works!
```

Returns `strong_ordering`, `weak_ordering`, or `partial_ordering`.

## 2. Designated Initializers

C-style struct initialization syntax.

```cpp
struct Config {
    int id;
    std::string name;
    bool active;
};

Config c = {
    .id = 1,
    .active = true // Order must match declaration! Name is default constructed.
};
```

## 3. `consteval` (Immediate Functions)

Functions that *must* be executed at compile time.

```cpp
consteval int square(int n) {
    return n * n;
}

int x = square(5); // OK: computed at compile time
int runtime_val = 10;
// int y = square(runtime_val); // Error: argument not constant
```

## 4. `constinit`

Ensures a variable has static initialization (no runtime overhead, no static initialization order fiasco).

```cpp
constinit int g_val = 42; // OK
// constinit int g_rand = rand(); // Error
```

## 5. Non-Type Template Parameters (NTTP) Enhancements

You can now use floating-point types and structural types (classes with public members) as template parameters.

```cpp
template<double Threshold>
bool check(double val) { return val > Threshold; }

struct FixedString { char buf[16]; };
template<FixedString S>
void print() { std::cout << S.buf; }
```

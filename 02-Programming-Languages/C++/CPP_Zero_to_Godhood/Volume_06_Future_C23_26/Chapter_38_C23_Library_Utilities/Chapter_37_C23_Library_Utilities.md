# C++23 LIBRARY UTILITIES

## 1. `std::unreachable`

Marks code as unreachable. If executed, Undefined Behavior (allows optimization).

```cpp
#include <utility>

enum Color { Red, Green };

int get_value(Color c) {
    switch (c) {
        case Red: return 1;
        case Green: return 2;
    }
    std::unreachable();
}
```

## 2. `std::to_underlying`

Safely cast an `enum class` to its underlying integer type.

```cpp
enum class Flags : unsigned char { A = 1 };
auto val = std::to_underlying(Flags::A); // unsigned char
```

## 3. `std::byteswap`

Endianness reversal.

```cpp
#include <bit>
uint32_t x = 0x12345678;
uint32_t y = std::byteswap(x); // 0x78563412
```

## 4. `std::stdatomic.h`

C compatibility header for atomics.

## 5. `std::move_only_function`

A replacement for `std::function` that works with move-only callables (like lambdas capturing `unique_ptr`).

```cpp
#include <functional>

std::move_only_function<void()> f;
auto ptr = std::make_unique<int>(42);

f = [p = std::move(ptr)]() { /*...*/ }; // OK (std::function would fail)
```

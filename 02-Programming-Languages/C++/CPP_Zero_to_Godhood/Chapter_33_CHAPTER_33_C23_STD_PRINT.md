# CHAPTER 33: C23 STD PRINT


# C++23 STD::PRINT & I/O

## 1. `std::print` and `std::println`

C++ finally gets high-performance, type-safe, and readable console output, replacing `printf` and `iostream`.

### 1.1 Basic Usage

```cpp
#include <print>

int main() {
    int id = 42;
    std::string name = "Alice";

    std::println("User {} has ID {}", name, id);
    // Output: User Alice has ID 42
}
```

### 1.2 Performance

`std::print` writes directly to the unicode-aware buffer, avoiding stream overhead and allocation. It is often faster than `printf`.

### 1.3 Formatting

Uses the same format specifications as `std::format` (C++20).

```cpp
std::println("Pi: {:.2f}", 3.14159); // Pi: 3.14
std::println("Hex: {:#x}", 255);     // Hex: 0xff
```

### 1.4 Output to Streams

```cpp
#include <fstream>

std::ofstream file("log.txt");
std::println(file, "Error code: {}", 500);
```

## 2. `std::spanstream`

A stream wrapper around a fixed buffer (`std::span`), replacing the deprecated `std::strstream`. No allocation.

```cpp
#include <spanstream>
#include <iostream>

char buffer[128];
std::spanstream ss(buffer);

ss << "Hello " << 123;
std::string_view result = ss.span(); // "Hello 123"
```

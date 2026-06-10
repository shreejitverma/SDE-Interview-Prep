# C20 STANDARD LIBRARY ADDITIONS


# C++20 STANDARD LIBRARY ADDITIONS

## 1. `std::format` (Python-style Formatting)

Type-safe, fast, and readable string formatting. Replaces `printf` and `iostream`.

```cpp
#include <format>
#include <iostream>

std::string s = std::format("Hello, {}! The answer is {}.", "World", 42);
// "Hello, World! The answer is 42."

// Positional arguments
std::string s2 = std::format("{1} {0}", "World", "Hello"); 
```

## 2. `std::span`

A non-owning view of a contiguous sequence (array, vector). Replaces `pointer + size`.

```cpp
#include <span>

void process(std::span<int> data) {
    for (int& x : data) x *= 2;
}

int arr[] = {1, 2, 3};
std::vector<int> vec = {4, 5, 6};

process(arr); // Works
process(vec); // Works
```

## 3. `std::jthread` (Joinable Thread)

Automatically joins on destruction. Supports cooperative interruption (`stop_token`).

```cpp
#include <thread>

void worker(std::stop_token st) {
    while (!st.stop_requested()) {
        // work...
    }
}

{
    std::jthread t(worker);
} // t requests stop and joins automatically here
```

## 4. Concurrency Primitives (`<semaphore>`, `<barrier>`, `<latch>`)

*   `std::binary_semaphore`, `std::counting_semaphore`: Lightweight signaling.
*   `std::latch`: Single-use countdown barrier.
*   `std::barrier`: Reusable barrier for phases of work.

## 5. Mathematical Constants (`<numbers>`)

```cpp
#include <numbers>
double pi = std::numbers::pi;
double e = std::numbers::e;
```

## 6. Bit Manipulation (`<bit>`)

*   `std::popcount`: Count set bits.
*   `std::bit_ceil`: Next power of 2.
*   `std::endian`: Check system endianness.


---


# VOLUME 06 FUTURE C23 26

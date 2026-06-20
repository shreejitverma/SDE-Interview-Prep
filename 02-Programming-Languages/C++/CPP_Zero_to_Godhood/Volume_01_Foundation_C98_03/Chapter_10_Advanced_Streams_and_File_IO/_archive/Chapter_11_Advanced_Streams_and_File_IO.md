# Chapter 10: Advanced Streams & File I/O

This chapter explores the full power of the C++ `<iostream>`, `<fstream>`, and `<sstream>` libraries, deconstructing how they interact with the filesystem and formatted data.

## 10.1 File I/O Foundations

Working with files involves the `std::fstream`, `std::ifstream`, and `std::ofstream` classes.

### 1. Opening Modes
*   `std::ios::in`: Open for reading.
*   `std::ios::out`: Open for writing (overwrites existing).
*   `std::ios::app`: Append to end of file.
*   `std::ios::ate`: Open and seek to end.
*   `std::ios::binary`: Binary mode (no CRLF translation).

### 2. Binary vs Text Mode
In text mode, special characters like `\n` might be translated (e.g., to `\r\n` on Windows). Binary mode ensures that exactly what you write is what appears on disk.
```cpp
std::ofstream file("data.bin", std::ios::binary);
double d = 3.14;
file.write(reinterpret_cast<const char*>(&d), sizeof(d));
```

---

## 10.2 Stream Manipulators

Manipulators allow you to change how data is formatted on the fly.

### 1. Numeric Formatting
*   `std::hex`, `std::oct`, `std::dec`: Change base.
*   `std::setprecision(n)`: Set floating point precision (requires `<iomanip>`).
*   `std::fixed`, `std::scientific`: Change notation.

### 2. Padding and Alignment
*   `std::setw(n)`: Set width of next field.
*   `std::setfill(c)`: Set fill character.
*   `std::left`, `std::right`, `std::internal`: Change alignment.

---

## 10.3 String Streams (`sstream`)

`std::stringstream` allows you to treat a string like a stream, enabling easy conversion between types and strings.
```cpp
#include <sstream>
std::stringstream ss;
ss << "The answer is " << 42;
std::string s = ss.str();
```

---
### Professional Notes: Stream Architecture

#### 1. The `ios_base` State Machine
Every stream inherits from `ios_base`, which maintains internal flags for formatting and errors.
*   **Performance Trap**: Streams are significantly slower than C's `printf`/`scanf` due to the overhead of object construction and virtual function calls.
*   **Optimization**: `std::ios::sync_with_stdio(false);` disables the synchronization between C++ and C streams, making `std::cin` as fast as `scanf`.

#### 2. Stream Buffering (`streambuf`)
The actual data transfer is handled by a buffer object (`rdbuf`).
*   **Redirection**: You can redirect `cout` to a file by swapping its buffer:
```cpp
std::ofstream out("log.txt");
std::streambuf *coutbuf = std::cout.rdbuf(); 
std::cout.rdbuf(out.rdbuf()); // cout now writes to log.txt
```

#### 3. Custom Manipulators
You can create your own manipulators by defining functions that take and return a reference to a stream:
```cpp
std::ostream& tab(std::ostream& os) {
    return os << "\t";
}
std::cout << "Data1" << tab << "Data2" << std::endl;
```

---

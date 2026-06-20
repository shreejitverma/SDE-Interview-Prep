# Chapter 26: Filesystem & I/O

# C++17 FILESYSTEM AND IO

## 1. `std::filesystem`

Standardized file system operations. Based on `boost::filesystem`.

### 1.1 Paths

```cpp
#include <filesystem>

namespace fs = std::filesystem;

fs::path p = "/home/user/data.txt";

std::cout << p.filename();      // "data.txt"
std::cout << p.extension();     // ".txt"
std::cout << p.parent_path();   // "/home/user"
```

### 1.2 Iterating Directories

```cpp
for (const auto& entry : fs::directory_iterator("/home/user")) {
    std::cout << entry.path() << "\n";
}

// Recursive
for (const auto& entry : fs::recursive_directory_iterator("/home/user")) {
    // ...
}
```

### 1.3 Operations

```cpp
fs::create_directory("sandbox");
fs::copy("a.txt", "b.txt");
fs::rename("b.txt", "c.txt");
fs::remove("c.txt"); // Returns true if removed
bool exists = fs::exists("sandbox");
uintmax_t size = fs::file_size("a.txt");
```

## 2. Polymorphic Allocators (`std::pmr`)

Memory resource management that is detached from the type.

```cpp
#include <memory_resource>
#include <vector>

char buffer[1024];
std::pmr::monotonic_buffer_resource pool(buffer, 1024);
std::pmr::vector<int> v(&pool); // Uses stack buffer!

v.push_back(1);
// No heap allocation happens until buffer is exhausted.
```


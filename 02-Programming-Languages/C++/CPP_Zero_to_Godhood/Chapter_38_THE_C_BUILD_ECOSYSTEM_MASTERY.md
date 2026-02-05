# THE C++ BUILD ECOSYSTEM MASTERY


Writing code is half the battle. Building and debugging it is the rest.

### 30.1 Package Managers Deep Dive

#### vcpkg (Manifest Mode)
Create `vcpkg.json` in your root:
```json
{
  "name": "my-app",
  "version": "1.0.0",
  "dependencies": [
    "fmt",
    "nlohmann-json"
  ]
}
```
CMake integration:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake
```

#### Conan (conanfile.txt)
```ini
[requires]
fmt/9.1.0
nlohmann_json/3.11.2

[generators]
CMakeDeps
CMakeToolchain
```

### 30.2 Sanitizers: The Developer's Best Friend

#### AddressSanitizer (ASan)
Detects out-of-bounds, use-after-free.
`clang++ -fsanitize=address -g main.cpp`

**Example: Use-After-Free**
```cpp
int* p = new int(5);
delete p;
*p = 10; // ASan catches this instantly!
```

#### ThreadSanitizer (TSan)
Detects data races.
`clang++ -fsanitize=thread -g main.cpp`

**Example: Data Race**
```cpp
int counter = 0;
std::thread t1([&]{ counter++; });
std::thread t2([&]{ counter++; }); // TSan catches this race
t1.join(); t2.join();
```

#### UndefinedBehaviorSanitizer (UBSan)
Detects overflow, null dereference, alignment issues.
`clang++ -fsanitize=undefined -g main.cpp`

### 30.3 Profiling Tools

*   **perf (Linux)**: `perf record -g ./app` -> `perf report`.
*   **Valgrind (Massif)**: Heap profiler. `valgrind --tool=massif ./app`.
*   **Hotspot**: UI for perf.

---


# CHAPTER 27: C20 MODULES


# C++20 MODULES

## 1. The Death of Headers

Headers (`#include`) are text substitution. They are slow, fragile, and leak macros/symbols. Modules (`import`) are compiled components.

### 1.1 Problems with Headers
*   **Slow Compilation:** A 10,000 line header included in 100 files is parsed 100 times.
*   **Macro Leaks:** Macros defined in one header affect all subsequent code.
*   **ODR Violations:** Different compiler flags for different TUs can break binary compatibility.

## 2. Basic Module Syntax

### 2.1 Interface Unit (`.cppm` or `.ixx`)

```cpp
export module math; // Module declaration

export int add(int a, int b) { // Exported function
    return a + b;
}

int internal_helper() { // Not exported (private)
    return 42;
}
```

### 2.2 Importing a Module

```cpp
import math;
import <iostream>; // Import header unit (if supported)

int main() {
    std::cout << add(1, 2) << "\n";
    // internal_helper(); // Error: undeclared identifier
}
```

## 3. Module Partitions

Large modules can be split into partitions.

`math_impl.cppm`:
```cpp
module math:impl; // Partition

int heavy_computation() { return 100; }
```

`math.cppm`:
```cpp
export module math;
import :impl; // Import partition

export int compute() {
    return heavy_computation();
}
```

## 4. The Global Module Fragment

Used to include legacy headers within a module.

```cpp
module;
#include <vector>
#include <cmath>

export module geometry;

export double distance(double x, double y) {
    return std::sqrt(x*x + y*y);
}
```

## 5. Build System Implications

Modules require a dependency graph to be built *before* compilation (unlike headers). Modern build systems (CMake 3.26+, Build2, XMake) support this.

```

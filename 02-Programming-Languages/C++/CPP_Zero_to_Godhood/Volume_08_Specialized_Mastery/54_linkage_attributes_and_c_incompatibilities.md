# Chapter 54: Linkage, Attributes, and C Incompatibilities

This chapter covers the rules for how identifiers are shared across files, how to give hints to the compiler, and the subtle ways C++ differs from its parent language, C.

## 51.1 Linkage Specifications

Linkage determines whether a name refers to the same entity across different scopes or translation units.

### 1. Types of Linkage

*   **External Linkage**: The name can be referred to from other translation units (e.g., non-static global variables, non-inline functions).
*   **Internal Linkage**: The name is only visible within its own translation unit (e.g., `static` globals, variables in anonymous namespaces).
*   **No Linkage**: The name is local to its scope (e.g., local variables).

### 2. `extern "C"`

Tells the C++ compiler to use C-style linkage (no name mangling). This is essential for calling C functions from C++ or vice versa.

***

## 51.2 C++ Attributes (`[[...]]`)

Attributes provide a standardized way to provide extra information to the compiler to improve optimization or warnings.

### Common Attributes:

*   `[[nodiscard]]`: Warns if the return value of a function is ignored.
*   `[[maybe_unused]]`: Suppresses warnings for unused variables.
*   `[[deprecated("reason")]]`: Marks an entity as obsolete.
*   `[[fallthrough]]`: Signals intentional fallthrough in a switch statement.
*   `[[likely]]` / `[[unlikely]]` (C++20): Hints to the optimizer about branch probability.

***

## 51.3 C Incompatibilities

While C++ is mostly a superset of C, there are several "breaking" differences.

### 1. Implicit Conversions

*   **C**: Allows implicit conversion from `void*` to any other pointer type.
*   **C++**: Requires an explicit cast.

### 2. Struct Definitions

*   **C**: Requires the `struct` keyword every time you refer to the type (unless `typedef`'d).
*   **C++**: The struct name becomes a type name automatically.

### 3. Functions with No Arguments

*   **C**: `int func()` means a function taking an *unspecified* number of arguments.
*   **C++**: `int func()` means a function taking *no* arguments (equivalent to `int func(void)`).

***
### Professional Insights: Linking & Tooling

#### 1. Static vs. Dynamic Linking

*   **Static**: Object code is copied into the executable at build time. Leads to larger binaries but no external dependencies.
*   **Dynamic**: Code is loaded at runtime from `.so` or `.dll` files. Allows for smaller binaries and shared updates.

#### 2. Linker Symbols and Mangling

Use tools like `nm` or `objdump` on Linux, or `dumpbin` on Windows, to inspect the symbols in your object files. Use `c++filt` to demangle names.

***


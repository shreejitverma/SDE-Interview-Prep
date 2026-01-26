# Memory Profiling Guide

## 1. Valgrind (Linux)
The gold standard for detecting memory leaks in C/C++.

### Usage
```bash
# 1. Compile with debug symbols (-g)
g++ -g memory_leak_demo.cpp -o app

# 2. Run with Valgrind
valgrind --leak-check=full --track-origins=yes ./app
```

### Interpreting Output
*   **Definitely Lost:** Memory leaked and you have no pointer to it anymore. (Fix ASAP)
*   **Indirectly Lost:** Memory leaked because the pointer to it was inside another leaked object.
*   **Still Reachable:** Memory not freed, but program ended. (Usually okay, OS reclaims it).

## 2. AddressSanitizer (ASan)
Faster than Valgrind, built into GCC/Clang.

### Usage
```bash
g++ -fsanitize=address -g memory_leak_demo.cpp -o app
./app
```

## 3. Python Profiling
*   **memory_profiler:** Decorate functions with `@profile` to see line-by-line usage.
*   **tracemalloc:** Track memory blocks.

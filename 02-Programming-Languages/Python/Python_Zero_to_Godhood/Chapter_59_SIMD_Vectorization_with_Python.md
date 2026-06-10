# SIMD Vectorization with Python


SIMD (Single Instruction, Multiple Data) allows a single CPU instruction to perform the same operation on multiple values simultaneously (e.g., adding 4 floats in one cycle).

### 71.1 AVX and SSE in the Standard Library
While the CPython interpreter loop doesn't use SIMD, many of its underlying C-extensions do.
*   **`base64`**: Uses SIMD to accelerate bit-shifting operations.
*   **`hashlib`**: Modern SHA implementations use hardware-accelerated instructions available on Intel (SHA-NI) and ARM (NEON).

### 71.2 Vectorizing with NumPy
NumPy's universal functions (`ufuncs`) are compiled with SIMD support. When you run `arr1 + arr2`, the underlying C code uses vector registers to process chunks of the arrays at once, achieving throughput that pure Python loops can never match.

---

# ADVANCED SIMD (AVX2 & AVX-512)


Data Parallelism: Processing 8 or 16 numbers in a single CPU cycle.

### 32.1 SIMD Basics & Registers
*   **SSE**: 128-bit (4 floats). XMM registers.
*   **AVX2**: 256-bit (8 floats). YMM registers.
*   **AVX-512**: 512-bit (16 floats). ZMM registers.

### 32.2 Intrinsics Example (Vector Addition)
Using `<immintrin.h>`.

```cpp
#include <immintrin.h>

void add_avx2(float* a, float* b, float* c, int N) {
    // Process 8 floats at a time
    for (int i = 0; i < N; i += 8) {
        // Load
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // Operation
        __m256 vc = _mm256_add_ps(va, vb);
        
        // Store
        _mm256_storeu_ps(&c[i], vc);
    }
}
```
*   `_mm256_loadu_ps`: Load Unaligned Packed Single-precision.
*   `_mm256_add_ps`: Add packed singles.

### 32.3 Measurable Outcome
*   **Objective**: Convert a scalar loop to AVX2.
*   **Success Metric**: 4x-8x speedup on large arrays (memory bandwidth permitting).

---

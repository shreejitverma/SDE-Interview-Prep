# CHAPTER 61: REAL-TIME AUDIO  SIGNAL PROCESSING


# REAL-TIME AUDIO & SIGNAL PROCESSING

**The Golden Rule:** In the audio callback, thou shalt not:
1.  **Block** (No Mutexes).
2.  **Allocate** (No `malloc`/`new`).
3.  **Perform I/O** (No file reads, no `printf`).

### 1. Lock-Free IPC (SPSC Queue)
Communication between the UI Thread (writes parameters) and Audio Thread (reads parameters) must be wait-free.
*   **Structure:** Single-Producer Single-Consumer Circular Buffer.
*   **Atomic Indices:** `head` (write) and `tail` (read) are `std::atomic<size_t>`.

### 2. SIMD for DSP
Digital Signal Processing (Filters, FFT) is purely math-bound.
*   **Auto-vectorization:** Help the compiler (restrict pointers, fixed loop bounds).
*   **Intrinsics:** Manually using `<immintrin.h>` for AVX-512 processing of 16 samples per cycle.
*   **Biquad Filter:** The workhorse of EQ.
    ```cpp
    // Processing 4 stereo channels (8 floats) at once with AVX
    __m256 samples = _mm256_load_ps(input_ptr);
    __m256 result = _mm256_add_ps(_mm256_mul_ps(samples, b0), ...);
    ```

### 3. Double Buffering
For spectral analysis (FFT) which requires blocks (e.g., 1024 samples), while audio comes in small chunks (e.g., 64 samples).
*   **Technique:** Fill Buffer A. When full, signal worker thread to process A, swap to filling Buffer B.

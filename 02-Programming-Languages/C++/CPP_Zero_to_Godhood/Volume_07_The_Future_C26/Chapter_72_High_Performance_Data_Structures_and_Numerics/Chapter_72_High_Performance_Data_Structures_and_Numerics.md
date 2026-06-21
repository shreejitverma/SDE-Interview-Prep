# Chapter 72: High-Performance Data Structures and Numerics

While concurrency and metaprogramming define the architecture of a system, the raw throughput of an application is dictated by how it lays out and processes data in memory. Modern CPUs are incredibly fast at math but abysmally slow at fetching data from RAM. To achieve "Godhood" performance, you must write code that is mechanically sympathetic to the hardware.

C++26 brings a massive overhaul to data processing, focusing on three areas:
1. **Memory Movement:** `std::is_trivially_relocatable`
2. **SIMD Vectorization:** `std::simd`
3. **Specialized Containers:** `std::inplace_vector` and `std::hive`
4. **Numerics:** Saturation arithmetic and `std::linalg`

---

## 72.1 The Move Semantic Bottleneck and Trivial Relocatability

C++11 move semantics revolutionized performance by turning expensive copies (e.g., copying a million-element vector) into cheap pointer swaps. However, move semantics still have a hidden cost: the "move-and-destroy" dance.

When a `std::vector<std::string>` resizes, it allocates a new buffer. It then iterates through the old buffer, calling the move constructor for each `std::string`, and finally calls the destructor for each `std::string` in the old buffer.

For a `std::string`, this involves copying three pointers (data, size, capacity) and then nulling out the source pointers, followed by an empty destructor call. Multiplied by millions of elements, this loop is incredibly slow.

### 72.1.1 `std::is_trivially_relocatable`

C++26 formalizes the concept of **Trivial Relocatability**. An object is trivially relocatable if moving it from address A to address B is mathematically equivalent to simply calling `memcpy` or `memmove` and skipping the destructor on the old object.

```cpp
#include <type_traits>
#include <string>
#include <memory>

// Most standard library types are trivially relocatable
static_assert(std::is_trivially_relocatable_v<std::string>);
static_assert(std::is_trivially_relocatable_v<std::unique_ptr<int>>);

// Types with self-referential pointers or system-registered handles are NOT
struct SelfRef {
    int data;
    int* ptr_to_data;
    SelfRef() : data(0), ptr_to_data(&data) {}
    // Moving this breaks 'ptr_to_data' because it still points to the old address.
};
static_assert(!std::is_trivially_relocatable_v<SelfRef>);
```

### 72.1.2 The Performance Impact on `std::vector`

Because `std::vector` is now mandated to check `is_trivially_relocatable_v`, vector reallocation operations (like `push_back` causing a resize, or `insert`, `erase`) compile down to a single call to `memmove`.

This completely eliminates the loop of move-constructors and destructors. In benchmarks, `std::vector<std::unique_ptr<T>>` resizes up to 10x faster in C++26 compared to C++20.

---

## 72.2 Hardware-Agnostic Vectorization: `std::simd`

CPUs have supported Single Instruction, Multiple Data (SIMD) for decades (SSE, AVX-512, ARM NEON). Previously, using SIMD required writing highly non-portable compiler intrinsics (e.g., `_mm256_add_ps`).

C++26 introduces `std::simd` in the `<simd>` header, providing a portable, type-safe wrapper around hardware vectors.

### 72.2.1 Basic SIMD Arithmetic

```cpp
#include <simd>
#include <vector>

namespace stdx = std::experimental; // often in experimental namespace during TS phase

void fast_add(float* a, float* b, float* result, size_t n) {
    using V = stdx::native_simd<float>; // Chooses best SIMD width for target arch (e.g., 8 floats on AVX2)
    constexpr size_t step = V::size();
    
    size_t i = 0;
    for (; i + step <= n; i += step) {
        // Load, add, and store multiple floats in a single instruction!
        V va(a + i, stdx::element_aligned);
        V vb(b + i, stdx::element_aligned);
        V vr = va + vb;
        vr.copy_to(result + i, stdx::element_aligned);
    }
    
    // Handle remaining elements...
}
```

By compiling this with `-mavx2`, the compiler generates exact `vaddps` instructions. If you compile for ARM, it generates NEON instructions. The code remains exactly the same.

---

## 72.3 Specialized Containers

The standard library expands beyond `vector` and `map` to include highly specialized, zero-allocation or stable-pointer containers.

### 72.3.1 `std::inplace_vector`

In embedded systems and kernel modules, dynamic allocation (`new`/`malloc`) is often strictly forbidden. Standard `std::vector` cannot be used.

`std::inplace_vector<T, N>` is a vector that allocates exactly `N` elements on the stack (or inside the containing struct). It has a dynamic size (like `std::vector`), but a hard capacity limit (like `std::array`).

```cpp
#include <inplace_vector>

void process_events() {
    // Allocates exactly 64 integers on the stack. No heap allocation!
    std::inplace_vector<int, 64> buffer; 
    
    buffer.push_back(1);
    buffer.push_back(2);
    
    if (buffer.size() == buffer.capacity()) {
        // buffer full
    }
}
```

### 72.3.2 `std::hive` (Plf::hive)

`std::hive` is a block-allocated, unordered container designed for scenarios where elements are frequently added and erased (like a particle system or an entity component system).

Unlike `std::list`, it maintains cache locality by allocating memory in large blocks.
Unlike `std::vector`, elements are never moved when erased, meaning pointers and iterators to elements remain strictly valid until the element itself is erased.

---

## 72.4 Numerics: `add_sat` and `std::linalg`

### 72.4.1 Saturation Arithmetic

When integers overflow in standard C++, they wrap around (for unsigned) or cause Undefined Behavior (for signed). In graphics or audio processing, overflow should "saturate" (cap at the maximum value).

```cpp
#include <numeric>
#include <iostream>

void audio_mix() {
    uint8_t sample1 = 250;
    uint8_t sample2 = 10;
    
    // Standard wrap: 250 + 10 = 4 (Audio glitch!)
    // C++26 Saturation: caps at 255 (Max volume)
    uint8_t mixed = std::add_sat(sample1, sample2);
    std::cout << (int)mixed << '
'; // 255
}
```

### 72.4.2 Linear Algebra: `std::linalg`

C++26 brings a standardized interface to BLAS (Basic Linear Algebra Subprograms). Instead of dealing with naked pointers and Fortran memory layouts, `std::linalg` operates on `std::mdspan` (multidimensional views).

```cpp
#include <linalg>
#include <mdspan>
#include <vector>

void matrix_multiply() {
    std::vector<double> A_data(100, 1.0);
    std::vector<double> B_data(100, 2.0);
    std::vector<double> C_data(100, 0.0);
    
    // View raw memory as 10x10 matrices
    std::mdspan<double, std::extents<size_t, 10, 10>> A(A_data.data());
    std::mdspan<double, std::extents<size_t, 10, 10>> B(B_data.data());
    std::mdspan<double, std::extents<size_t, 10, 10>> C(C_data.data());
    
    // Standardized Matrix Multiplication
    // Maps down to highly optimized dgemm calls if a BLAS backend is linked
    std::linalg::matrix_multiply(A, B, C);
}
```

This transforms C++ into a first-class language for Data Science and Machine Learning, removing the need to drop into Python or complex third-party math libraries.


## 72.5 Deep Dive: AVX-512 and SIMD Alignment
To maximize `std::simd` throughput, memory alignment is critical. AVX-512 instructions operate on 64-byte aligned boundaries...


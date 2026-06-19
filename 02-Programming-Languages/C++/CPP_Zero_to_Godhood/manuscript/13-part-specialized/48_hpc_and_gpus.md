# Chapter 48: High-Performance Computing and GPUs

> *Making the fast code faster.*

We have reached the absolute bleeding edge of C++ performance. High-Performance Computing (HPC) is the domain of climate simulators, physics engines, and Artificial Intelligence. 

In this domain, the standard library is often too slow. Object-Oriented Programming is banned. Every single CPU cycle is accounted for, and when the CPU is no longer fast enough, the workload is offloaded to the Graphics Processing Unit (GPU).

---

## 48.1 CPU vs. GPU Architecture

To understand HPC, you must understand the hardware difference between a CPU and a GPU.

*   **CPU (Optimized for Latency):** A modern CPU has 8 to 24 extremely complex cores. They are designed to execute unpredictable code (like an Operating System) as fast as possible. They have massive caches and advanced Branch Prediction logic to guess what `if` statement you will take next.
*   **GPU (Optimized for Throughput):** A modern GPU has 5,000 to 10,000 extremely simple cores. They have no branch prediction and very small caches. They are designed to do the exact same mathematical operation on 10,000 different numbers at the exact same time. 

## 48.2 Extreme CPU Optimization

Before moving to the GPU, you must exhaust the CPU. The golden rule of HPC is **Cache Locality**. 

As we saw in the previous chapter with Entity-Component-Systems, the CPU prefers reading data in straight, contiguous lines (like a `std::vector`). If you force the CPU to chase pointers across the heap (like a `std::list` or a tree), the CPU will sit idle for hundreds of cycles waiting for RAM. This is called a **Cache Miss**.

### Branch Prediction
CPUs try to guess which branch of an `if` statement will be taken. If they guess wrong, they have to throw away their work and start over (a Pipeline Flush). 
In HPC, you avoid branches entirely using math.

```cpp
// Bad (Branchy):
for (int i = 0; i < N; ++i) {
    if (data[i] > 0) result[i] = 1;
    else result[i] = 0;
}

// Godhood (Branchless):
for (int i = 0; i < N; ++i) {
    result[i] = (data[i] > 0); // Boolean evaluates to 1 or 0
}
```

## 48.3 SIMD (Single Instruction, Multiple Data)

A CPU core usually adds two numbers together at a time. But modern CPUs contain incredibly wide 512-bit registers (AVX-512) that can hold sixteen 32-bit `float`s at once.

Using **SIMD Intrinsics**, you can instruct the CPU to add 16 pairs of numbers together in a single clock cycle.

```cpp
#include <immintrin.h> // Intel AVX intrinsics

void add_arrays_simd(float* a, float* b, float* result, int N) {
    // Process 8 floats at a time (256-bit registers)
    for (int i = 0; i < N; i += 8) {
        // Load 8 floats from memory into CPU vector registers
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // Add them simultaneously in one clock cycle
        __m256 vc = _mm256_add_ps(va, vb);
        
        // Store the 8 results back to memory
        _mm256_storeu_ps(&result[i], vc);
    }
}
```
Writing intrinsic functions by hand is tedious and non-portable. Libraries like `std::experimental::simd` (coming in future C++ standards) aim to make this easier.

## 48.4 Scientific Computing: Eigen

For linear algebra (matrices, vectors, solving systems of equations), the industry standard C++ library is **Eigen**.

Eigen uses advanced Template Metaprogramming (specifically, Expression Templates) to completely eliminate temporary variables.

```cpp
#include <Eigen/Dense>

void do_math() {
    Eigen::Matrix4f A = Eigen::Matrix4f::Random();
    Eigen::Matrix4f B = Eigen::Matrix4f::Identity();
    
    // Because of Expression Templates, Eigen does not create a 
    // temporary matrix for (A + B). It fuses the loops together at compile time!
    Eigen::Matrix4f C = (A + B) * 2.0f;
}
```

## 48.5 GPU Computing with CUDA

When 24 CPU cores and AVX-512 aren't enough, we turn to the GPU. **CUDA** is an extension of C++ created by NVIDIA that allows you to write functions (called **Kernels**) that execute directly on the graphics card.

A CUDA program has two parts:
1.  **Host Code:** Standard C++ running on the CPU. It allocates memory on the GPU (VRAM) and copies data over.
2.  **Device Code:** C++ running on the GPU.

```cpp
// 1. DEVICE CODE (The Kernel)
// The __global__ keyword tells the compiler this runs on the GPU
__global__ void vectorAdd(float* A, float* B, float* C, int N) {
    // Every GPU thread has a unique ID. We use it as the array index.
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    
    // 10,000 threads execute this exact same line simultaneously
    if (i < N) {
        C[i] = A[i] + B[i];
    }
}

// 2. HOST CODE
void launch() {
    int N = 10000;
    float *d_A, *d_B, *d_C;
    
    // Allocate memory on the GPU
    cudaMalloc(&d_A, N * sizeof(float)); 
    cudaMalloc(&d_B, N * sizeof(float));
    cudaMalloc(&d_C, N * sizeof(float));
    
    // ... Copy data from CPU to GPU using cudaMemcpy ...

    // Launch the Kernel! Tell the GPU to spawn 10,000 threads.
    int threadsPerBlock = 256;
    int blocks = (N + threadsPerBlock - 1) / threadsPerBlock;
    vectorAdd<<<blocks, threadsPerBlock>>>(d_A, d_B, d_C, N);
    
    // ... Copy results back to CPU ...
}
```

This is the technology that powers the modern AI revolution. Deep Learning frameworks like PyTorch and TensorFlow are entirely written in C++ and CUDA under the hood.

---

We have traversed the entire landscape of C++, from the humble `int main()` to globally distributed server clusters, from bare-metal microcontrollers to massively parallel Supercomputers.

It is time to bring it all together. We move to the final Phase of this book: **Part XIV: Mastery**, where we provide the Ultimate Algorithm Reference and design our final Capstone Project.

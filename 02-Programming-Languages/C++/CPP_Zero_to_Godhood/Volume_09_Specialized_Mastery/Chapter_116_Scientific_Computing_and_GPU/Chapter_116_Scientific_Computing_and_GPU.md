# Chapter 116: Scientific Computing and GPU

C++ is the language beneath nearly all high-performance numerical computing — the linear-algebra libraries, the physics simulations, the deep-learning kernels — because scientific computing is dominated by the two things C++ exposes and manages best: arithmetic throughput and memory layout. This chapter covers the CPU side (template-based linear algebra via Eigen, exploiting expression templates and SIMD) and the GPU side (CUDA's massively-parallel model), unified by one principle: numerical performance is about *keeping the arithmetic units fed* — on the CPU through cache-friendly layout and vectorisation, on the GPU through massive parallelism and memory coalescing.

## Chapter Roadmap

- 116.1 Why C++ Dominates Numerical Computing
- 116.2 Eigen: Expression Templates for Linear Algebra
- 116.3 The GPU Execution Model
- 116.4 CUDA Kernels and the Host/Device Boundary
- 116.5 GPU Memory: The Real Bottleneck
- 116.6 The Numerical Performance Discipline

---

## 116.1 Why C++ Dominates Numerical Computing

Scientific computing is the purest expression of the *compute* cost model (Chapter 85): dense arithmetic over large arrays, where performance is bounded by how fully you use the arithmetic units (CPU SIMD lanes, GPU cores) and how efficiently you feed them from memory. C++ wins here because it gives direct control over both — memory layout (Chapter 90) and vectorisation (Chapter 92) — with zero-overhead abstractions that let libraries express natural math syntax while generating optimal code.

> **Why this matters.** Python, R, and MATLAB are the *interfaces* to scientific computing, but the *engines* are C++ (and Fortran): NumPy, SciPy, PyTorch, and TensorFlow are thin scripting layers over C++/CUDA kernels (Chapters 111, 117). The reason is that an interpreted loop over a million elements is hopeless, while a C++ kernel vectorises and parallelises. C++'s specific advantages — control over data layout, expression templates to fuse operations, and direct access to SIMD and GPU intrinsics — are exactly what numerical performance requires. The domain is where the compute-cost techniques of Volume 8 are most directly applied.

---

## 116.2 Eigen: Expression Templates for Linear Algebra

**Eigen** is the leading header-only C++ linear-algebra library. Its defining technique is **expression templates** (Chapter 108): an expression like `A + B + C` builds a lightweight expression tree and evaluates it in a *single fused loop* with no temporary matrices.

```cpp
// Min standard: C++14 + Eigen (non-portable: requires the library). Natural syntax, fused evaluation.
#include <Eigen/Dense>
using Eigen::MatrixXd;
using Eigen::VectorXd;

void solve_system() {
    MatrixXd A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 10;
    VectorXd b(3);
    b << 3, 3, 4;
    VectorXd x = A.colPivHouseholderQr().solve(b);   // solves Ax = b
}
```
*Listing 116.1 — Eigen: natural matrix syntax, but `A + B + C` compiles to one fused loop. Non-portable.*

> **Why this matters / cost model.** A naive matrix library computes `A + B + C` as two passes with a temporary matrix between them — for large matrices, two full sweeps through DRAM plus an allocation (Chapter 87). Eigen's expression templates collapse this to *one* pass with no temporary, and the resulting loop auto-vectorises (Chapter 92) — so natural mathematical syntax produces hand-tuned-loop performance. Eigen further exploits the cache via *blocking* (operating on cache-sized tiles, Chapter 87) for matrix multiplication, and provides explicit SIMD paths. The lesson is that the abstraction (write the math naturally) and the performance (fused, vectorised, cache-blocked) coexist *because* of the TMP techniques of Chapter 108 — Eigen is their canonical real-world payoff. The hazard is also Chapter 108's: `auto x = A + B;` captures an expression holding *references*, which dangles if the operands are temporaries — a real Eigen gotcha.

---

## 116.3 The GPU Execution Model

When the CPU's SIMD parallelism is not enough, the **GPU** offers thousands of cores executing in lockstep — a fundamentally different model. The GPU is a *throughput* machine: it hides memory latency not with caches and out-of-order execution (Chapter 86) but with *massive thread oversubscription* — when one group of threads stalls on memory, the scheduler instantly runs another, keeping the arithmetic units busy.

> **Why this matters.** The GPU inverts the CPU's design priorities. A CPU core is *latency-optimised*: large caches, branch prediction, out-of-order execution to make a single thread fast. A GPU is *throughput-optimised*: thousands of simple cores, tiny per-thread resources, and latency hidden by having far more threads than cores so there is always work to run. This is why GPUs crush CPUs on *data-parallel* problems (the same operation over millions of elements — matrix multiply, convolution, ray tracing) and lose on *latency-sensitive, branchy, or serial* ones. The fit determines everything: a problem that maps to "do this identical arithmetic to every element of a huge array" is a GPU problem; one full of data-dependent branches and pointer chasing is not.

---

## 116.4 CUDA Kernels and the Host/Device Boundary

**CUDA** is NVIDIA's C++ extension for GPU programming. You write a **kernel** — a function annotated `__global__` that runs on the GPU — and launch it across a grid of thousands of threads; each thread computes its own index and processes one (or a few) elements.

```cpp
// Min standard: CUDA C++ (non-portable: requires nvcc + NVIDIA GPU). Vector addition on the GPU.
__global__ void vectorAdd(const float* A, const float* B, float* C, int N) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;   // this thread's unique global index
    if (i < N) C[i] = A[i] + B[i];                   // each thread does ONE element
}
void launch(const float* dA, const float* dB, float* dC, int N) {
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    vectorAdd<<<blocksPerGrid, threadsPerBlock>>>(dA, dB, dC, N);   // launch the grid
}
```
*Listing 116.2 — A CUDA kernel: each of N threads computes one element. `<<<...>>>` launches the grid. Non-portable.*

> **Why this matters / cost model.** The CUDA model is *data parallelism made explicit*: you express the per-element computation once, and the hardware runs it across thousands of threads. The critical cost is the **host/device boundary**: the CPU (host) and GPU (device) have separate memory, so data must be copied across the PCIe bus *to* the GPU before the kernel and *back* after — and that transfer is slow (tens of GB/s, far less than on-GPU bandwidth). This is the GPU analogue of the syscall/copy cost (Chapters 98–99): the transfer often dominates, so the discipline is to move data to the GPU *once*, do *many* operations there, and move results back *once* — never ping-pong per operation. A kernel that is faster on the GPU but copies its data across PCIe every call is usually slower overall than staying on the CPU.

---

## 116.5 GPU Memory: The Real Bottleneck

Just as on the CPU, GPU performance is usually *memory-bound*, not compute-bound — and the GPU's key memory concept is **coalescing**: when the threads of a group access *consecutive* memory addresses, the hardware combines them into one wide transaction; when they access *scattered* addresses, it issues many transactions, wasting bandwidth.

> **Why this matters / cost model.** Memory coalescing is the GPU's version of cache-line utilisation (Chapter 87) and the single most important GPU optimization. A kernel where thread *i* reads element *i* (consecutive, coalesced) achieves full memory bandwidth; one where thread *i* reads a strided or scattered location (e.g. a column of a row-major matrix) achieves a fraction of it — the *same* arithmetic, many times slower. This is exactly the AoS-vs-SoA / access-pattern lesson of Chapter 90, transposed to the GPU: lay data out so that adjacent threads touch adjacent memory. GPUs also have a fast on-chip **shared memory** (a programmer-managed cache per thread block) used to stage data for reuse, and the same blocking/tiling techniques (Chapter 87) apply. The recurring truth holds across both processors: *feed the arithmetic units from memory efficiently, and layout dominates*.

---

## 116.6 The Numerical Performance Discipline

| Concern | CPU (Eigen/SIMD) | GPU (CUDA) |
|---|---|---|
| Parallelism | SIMD lanes + cores | Thousands of threads |
| Fusing operations | Expression templates | Kernel fusion |
| Memory layout | Cache lines, SoA, blocking | Coalescing, shared memory, tiling |
| The expensive boundary | DRAM bandwidth | PCIe host↔device transfer |
| Best fit | Moderate data, latency matters | Massive data-parallel arithmetic |

> **The discipline.** Scientific computing is the compute-cost model in its purest form, and the same principle governs both processors: *performance is keeping the arithmetic units fed*. On the CPU, that means expression templates to fuse operations (no temporaries), cache-friendly layout (SoA, blocking), and vectorisation (Chapters 90, 92) — which Eigen embodies. On the GPU, it means massive data parallelism, minimising host/device transfers, and coalesced memory access. The unifying judgement is *fit*: use the GPU when the problem is massively data-parallel and the data justifies the transfer cost; stay on the CPU (vectorised) otherwise. And on both, remember the lesson that runs through this entire book — layout and memory bandwidth, not raw FLOPs, usually decide the speed. The next chapter builds directly on this: the machine-learning infrastructure that is, at bottom, scientific computing at scale.

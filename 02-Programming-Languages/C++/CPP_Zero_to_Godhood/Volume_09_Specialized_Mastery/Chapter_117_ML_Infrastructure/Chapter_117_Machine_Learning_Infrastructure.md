# Chapter 117: Machine Learning Infrastructure

Deep-learning frameworks present a Python face, but PyTorch, TensorFlow, and JAX are C++ engines — the tensors, the automatic differentiation, the operator kernels, and the GPU dispatch are all native code, with Python merely orchestrating. Understanding that C++ core is what separates an engineer who *uses* a framework from one who can *optimise, extend, or build* one. This chapter covers the four pillars of an ML engine — tensor memory layout, broadcasting, automatic differentiation, and operator fusion — each an application of the layout and compute disciplines this book has built.

## Chapter Roadmap

- 117.1 Frameworks Are C++ Engines
- 117.2 The Tensor: Memory Plus a View
- 117.3 Broadcasting via Zero Strides
- 117.4 Automatic Differentiation
- 117.5 Operator Fusion and Memory Bandwidth
- 117.6 The ML Infrastructure Discipline

---

## 117.1 Frameworks Are C++ Engines

A deep-learning framework is a Python (or other) scripting layer over a C++ engine: the user describes a model in Python, but every actual computation — matrix multiply, convolution, gradient — runs in C++/CUDA kernels (Chapters 111, 116). Python builds and orchestrates the computation graph; C++ executes it.

> **Why this matters.** This architecture is the interop pattern of Chapter 111 at scale: a thin, ergonomic Python API (where productivity matters) over a heavy C++/CUDA core (where performance matters), with the boundary crossed *coarsely* — one Python call dispatches a whole tensor operation over millions of elements, never per-element. Knowing the engine is C++ explains the framework's behaviour: why operations are lazy or eager, why moving data between CPU and GPU is explicit and costly (Chapter 116's host/device boundary), why custom operators are written in C++/CUDA, and why performance tuning ultimately means understanding the kernels. The ML "infrastructure" engineer works on this C++ layer.

---

## 117.2 The Tensor: Memory Plus a View

The central abstraction is the **tensor**: not a multidimensional array in the naive sense, but a flat block of memory plus a **view** describing how to interpret it — a shape and a set of **strides** (how many elements to skip to advance one step in each dimension).

```cpp
// Min standard: C++11. The tensor's core: data + shape + strides. Element access is a dot product.
struct Tensor {
    float* data;                    // flat contiguous (or shared) buffer
    std::vector<int> shape;         // e.g. {rows, cols}
    std::vector<int> strides;       // elements to skip per dimension
    float& at(int i, int j) {
        return data[i * strides[0] + j * strides[1]];   // address = base + i*stride_i + j*stride_j
    }
};
// Transpose: SWAP the strides (and shape). Touches ZERO data — just a different view.
```
*Listing 117.1 — A tensor is memory + strides; access is `base + Σ index·stride`. Transpose just swaps strides.*

> **Why this matters / cost model.** The data-plus-view design is what makes tensor operations cheap: **transposing** a tensor (`A.T`) does not move a single byte — it swaps the strides and shape, producing a new *view* of the same memory (the same trick as the slot map's indirection, Chapter 109). Slicing, reshaping (when compatible), and broadcasting (§117.3) are likewise zero-copy view manipulations. This is the layout discipline of Chapter 90 formalised: the *logical* shape (what the math sees) is decoupled from the *physical* layout (what is in memory), connected by strides. The crucial performance consequence is **contiguity**: a kernel over a contiguous tensor streams memory efficiently (coalesced/prefetched, Chapters 87, 116), while a kernel over a transposed (non-contiguous, strided) view accesses memory in a cache-hostile pattern — so frameworks sometimes *materialise* a contiguous copy before an operation, trading a copy for fast access.

---

## 117.3 Broadcasting via Zero Strides

**Broadcasting** lets operations combine tensors of different shapes by virtually expanding the smaller — adding a `[32, 1]` bias vector to a `[32, 100]` matrix, for instance. The implementation is elegant: set the **stride of the broadcast dimension to zero**, so advancing along that dimension reads the *same* element repeatedly.

```cpp
// Min standard: C++11. Broadcasting: a zero stride makes one element appear repeated.
// To add a [32,1] vector to a [32,100] matrix:
//   the vector's stride along the size-100 dimension is set to 0
//   so vector.at(i, j) reads vector.data[i * stride_i + j * 0] = vector.data[i * stride_i]
//   -> the same value for all j, with NO copy and NO extra memory.
```
*Listing 117.2 — Broadcasting via a zero stride: the broadcast dimension re-reads one element, allocation-free.*

> **Why this matters.** Broadcasting is a textbook example of solving a problem through *layout cleverness* rather than computation or allocation. The naive implementation would *materialise* the `[32, 1]` vector into a full `[32, 100]` matrix (32× the memory and a copy) before adding; the zero-stride trick achieves the identical result with *no* extra memory and *no* copy — the broadcast dimension simply re-reads the same value. This is the same philosophy as expression templates (Chapter 108) and the tensor view (§117.2): avoid temporaries and copies by manipulating *how memory is interpreted*. It is also why broadcasting "just works" in NumPy/PyTorch with no performance cliff — it is free at the memory level.

---

## 117.4 Automatic Differentiation

Training a neural network requires gradients, and frameworks compute them via **automatic differentiation (autograd)** — specifically *reverse-mode* (backpropagation). The engine records the operations of the forward pass into a **computational graph** (a DAG), then walks it backward applying the chain rule.

```cpp
// Min standard: C++11. The autograd node: forward records; backward propagates gradients.
struct Node {
    virtual ~Node() = default;
    virtual Tensor forward() = 0;
    virtual Tensor backward(const Tensor& grad_output) = 0;   // chain rule: dL/dinput from dL/doutput
    std::vector<Node*> inputs;        // edges in the computational DAG
};
// Forward pass: compute outputs, store intermediates (the "tape").
// Backward pass: from the loss, call backward() in reverse topological order, accumulating gradients.
```
*Listing 117.3 — Reverse-mode autograd: a DAG of operations, each knowing its local gradient.*

> **Why this matters / cost model.** Reverse-mode autograd is what makes training tractable: it computes the gradient of a scalar loss with respect to *all* parameters in a *single* backward pass (cost proportional to the forward pass), versus forward-mode which would need one pass per parameter — for a model with millions of parameters, this is the difference between feasible and impossible. The engineering is a graph of `Node` objects (or a recorded "tape") where each operation knows how to propagate gradients backward through itself (the chain rule). The cost model is *memory*: the forward pass must *store* its intermediate activations because the backward pass needs them — for large models this activation memory dominates GPU RAM, driving techniques like gradient checkpointing (recompute activations instead of storing them, trading compute for memory). This is the classic time/space trade-off (Chapter 82's GC echoes it) at the heart of training large models.

---

## 117.5 Operator Fusion and Memory Bandwidth

ML kernels are overwhelmingly **memory-bandwidth-bound**: an operation like `ReLU(Add(MatMul(A, B), C))` reads and writes the full tensor to GPU memory at *each* step, and the memory traffic — not the arithmetic — is the bottleneck. **Operator fusion** combines a chain of operations into a *single* kernel that does all of them while the data is in fast registers/shared memory, writing to main memory only once.

> **Why this matters / cost model.** This is exactly the expression-template lesson (Chapters 108, 116) applied to GPU kernels: `ReLU(Add(MatMul...))` naively launches three kernels, each reading its input from and writing its output to GPU global memory (slow, Chapter 116's coalescing/bandwidth concern) — three round trips through memory. A *fused* kernel computes the matmul, adds `C`, and applies `ReLU` all while the intermediate values sit in registers, touching global memory only to read the inputs and write the final result — eliminating the intermediate reads/writes and often *halving or better* the runtime for bandwidth-bound chains. This is why frameworks invest enormously in fusion (TorchScript, XLA, TensorRT) and why the memory-bandwidth cost model (Chapters 85, 116), not the FLOP count, governs ML kernel performance. The arithmetic is often "free"; the memory traffic is the cost.

---

## 117.6 The ML Infrastructure Discipline

| Pillar | Mechanism | Cost model echo |
|---|---|---|
| Tensor | Data + shape + strides; transpose = swap strides | View vs layout (Ch 90, 109) |
| Broadcasting | Zero stride re-reads one element | Avoid copies via layout (Ch 108) |
| Autograd | Reverse-mode DAG + stored activations | Time/space trade-off; activation memory |
| Operator fusion | One kernel, intermediates in registers | Memory bandwidth dominates (Ch 116) |

> **The discipline.** Machine-learning infrastructure is scientific computing (Chapter 116) at industrial scale, and the same truths govern it: the framework is a C++/CUDA engine behind a Python face (Chapter 111); tensors decouple logical shape from physical layout via strides (Chapter 90); copies and temporaries are avoided through clever views and fusion (Chapter 108); and performance is bounded by *memory bandwidth*, not arithmetic (Chapters 85, 116). The ML-infrastructure engineer optimises by minimising memory traffic — fusing operators, exploiting contiguity, trading activation storage for recomputation — exactly the disciplines this book has built, applied to the most compute-intensive workload of the era. The next chapters return to hard real-time, where the same allocation-free, predictable-latency disciplines govern audio and robotics.

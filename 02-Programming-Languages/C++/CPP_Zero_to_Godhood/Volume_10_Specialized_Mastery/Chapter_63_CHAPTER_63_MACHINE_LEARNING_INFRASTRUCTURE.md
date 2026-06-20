# CHAPTER 63: MACHINE LEARNING INFRASTRUCTURE


# MACHINE LEARNING INFRASTRUCTURE

Deep Learning frameworks (PyTorch, TensorFlow) are C++ engines wrapped in Python.

### 1. Tensor Memory Layout
A Tensor is a block of memory + a "View".
*   **Strides:** The number of elements to skip to reach the next index in a dimension.
    *   `Element(i, j) = data[i * stride_i + j * stride_j]`
*   **Contiguity:** Transposing a tensor (`A.T`) usually just modifies strides, touching **zero** data.

### 2. Broadcasting Implementation
How to add `[32, 1]` vector to `[32, 100]` matrix?
*   **Virtual Expansion:** Set stride for the dimension of size `1` to `0`. Accessing that dimension repeatedly reads the same value.

### 3. Automatic Differentiation (Autograd)
*   **Computational Graph:** Directed Acyclic Graph (DAG) of operations.
*   **Reverse Mode (Backprop):**
    1.  Forward pass: Compute `y = f(x)`, store intermediate values (tape).
    2.  Backward pass: Compute `dL/dx` using stored values and chain rule.
*   **Implementation:** `Node` class with `virtual Tensor backward(Tensor grad)`.

### 4. Operator Fusion
Optimizing `ReLU(Add(MatMul(A, B), C))` into a single kernel launch to minimize VRAM bandwidth.

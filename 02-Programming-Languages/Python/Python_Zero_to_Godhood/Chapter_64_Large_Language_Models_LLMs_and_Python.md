# Large Language Models (LLMs) and Python


Python is the primary language for training and deploying LLMs.

### 81.1 PyTorch Architecture
PyTorch is a tensor library with a focus on deep learning.
*   **Dynamic Computation Graph**: The graph is built on-the-fly during the forward pass.
*   **The ATen Library**: The C++ backend that handles tensor operations and autograd.

### 81.2 Hugging Face Transformers
The `transformers` library provides a standardized API for LLMs.
*   **Tokenization**: High-performance tokenizers implemented in Rust/C++.
*   **Model Sharding**: Techniques like DeepSpeed or FSDP (Fully Sharded Data Parallel) allow for training models that are too large to fit in a single GPU's memory.

---

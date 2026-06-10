# Phase XIX: Scientific Computing Internals

Python's dominance in science is due to its ability to wrap high-performance Fortran and C libraries.

# Chapter 86: NumPy Internals: Memory Strides and UFuncs

### 86.1 The `ndarray` C-Struct
A NumPy array is a C-struct that points to a block of data.
*   **Data**: Pointer to the raw memory.
*   **Dimensions**: Shape of the array.
*   **Strides**: The number of bytes to skip in memory to reach the next element in a dimension. This allows for $O(1)$ reshaping and slicing without copying data.

### 86.2 Universal Functions (UFuncs)
UFuncs are C-loops that operate on `ndarray` data. They handle type dispatching and SIMD acceleration (Chapter 71) automatically.

---

# Chapter 87: SciPy: Optimization and Linear Algebra Backends

SciPy builds on NumPy, providing interfaces to legacy but highly optimized libraries like **LAPACK** and **BLAS**.
*   **Sparse Matrices**: Storing large matrices with many zeros using CSR (Compressed Sparse Row) or CSC (Compressed Sparse Column) formats to save memory.
*   **Optimization**: Implementations of algorithms like BFGS and Nelder-Mead in C/Fortran.

---

# Chapter 88: Matplotlib: The Artist Layer and Backend Architecture

Matplotlib uses a three-layer architecture:
1.  **Backend Layer**: Handles the actual rendering to a file (PNG, PDF) or screen (Qt, Tk).
2.  **Artist Layer**: Manages the hierarchy of objects (Figures, Axes, Lines).
3.  **Scripting Layer (`pyplot`)**: Provides the familiar state-machine interface.

---

# Phase XX: Web Framework Architectures

# Chapter 89: WSGI vs. ASGI: The Evolution of Web Interfaces

### 89.1 WSGI (Web Server Gateway Interface)
Defined in PEP 3333, WSGI is synchronous. The server calls a function for every request and waits for the response.
*   **Servers**: Gunicorn, uWSGI.

### 89.2 ASGI (Asynchronous Server Gateway Interface)
ASGI (PEP 3112) is the asynchronous successor, supporting WebSockets and long-lived connections.
*   **Servers**: Uvicorn, Daphne.

---

# Chapter 90: Django Internals: The ORM and Migration Engine

Django is the "batteries-included" web framework.
*   **The ORM**: Translates Python class definitions into SQL. It uses a complex tree-based query generator to handle joins and filters.
*   **Migrations**: Uses the `ast` module to analyze changes in models and generate the minimal SQL required to update the database schema.

---

# Chapter 91: FastAPI and Pydantic: Type-Safe Web Development

FastAPI leverages modern Python features for performance.
*   **Pydantic**: Uses Python type hints (Chapter 67) to generate JSON schemas and perform validation at the C-level (via Pydantic-Core in Rust).
*   **Dependency Injection**: Uses `inspect.signature` to resolve dependencies at startup, minimizing per-request overhead.

---

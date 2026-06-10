# The Python Packaging Ecosystem: PEP 517 to Wheels


Understanding how Python code is distributed is essential for senior engineering.

### 68.1 The Evolution of Installation

1.  **Legacy (`setup.py install`)**: Executed a script that performed arbitrary actions. This was insecure and non-reproducible.
2.  **Modern (PEP 517/518)**: Decouples the build backend (e.g., `setuptools`, `flit`, `poetry`) from the frontend (`pip`).
*   **`pyproject.toml`**: The source of truth for build requirements.
*   **Build Isolation**: `pip` creates a temporary virtual environment to build your package, ensuring that build dependencies don't pollute your system.

### 68.2 The Wheel Format (PEP 427)

A "Wheel" (`.whl`) is a built distribution format.
*   **Internals**: It is a ZIP file (Chapter 50) containing the code and a `.dist-info` directory with metadata (dependencies, entry points).
*   **Platform Tags**: Wheels for C extensions include tags like `manylinux2014_x86_64` to specify exactly which OS and architecture they are compatible with, avoiding the need for the end-user to have a C compiler installed.

---

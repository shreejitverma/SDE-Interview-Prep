# Virtual Environments (`venv`)


Dependency isolation is the bedrock of reproducible software engineering. Python's `venv` module provides the standard way to create isolated environments, leveraging the interpreter's flexible search path machinery.

### 60.1 How `venv` Works: The `pyvenv.cfg` Secret

A virtual environment is not a full copy of the Python interpreter. It is a lightweight directory structure that "tricks" the Python binary into looking for libraries in a specific location.

#### 1. The `pyvenv.cfg` File
Every venv contains a `pyvenv.cfg` file. When the Python binary starts, it looks for this file in its parent directory.
*   **`home`**: Points to the original Python binary that created the venv.
*   **`include-system-site-packages`**: A boolean flag.
*   **`version`**: The Python version.

#### 2. `sys.prefix` and `sys.base_prefix`
*   **`sys.base_prefix`**: Points to the original global Python installation.
*   **`sys.prefix`**: In a venv, this is updated to point to the venv directory.
*   **Internals**: The `site.py` module (run automatically during startup) reads `pyvenv.cfg` and updates `sys.path` to include the venv's `site-packages` directory before the global ones.

### 60.2 `ensurepip`: Bootstraping the Ecosystem

The `ensurepip` module provides a way to install `pip` into an environment without needing an internet connection. It contains bundled "wheel" files of `pip` and `setuptools`.

---

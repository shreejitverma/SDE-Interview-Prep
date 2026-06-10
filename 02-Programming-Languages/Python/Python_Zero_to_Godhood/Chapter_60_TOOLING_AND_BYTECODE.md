# Chapter 60: Virtual Environments (`venv`)

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

# Chapter 61: Python Execution Archives (`zipapp`)

Python has the unique ability to execute a zip file containing code as if it were a single script. This is formalized in PEP 441 and the `zipapp` module.

### 61.1 The Shebang Trick

A `zipapp` is a zip archive with a "shebang" line (e.g., `#!/usr/bin/env python3`) prepended to the binary data.
*   **The ZIP Parser**: The ZIP file format (as seen in Chapter 50) looks for its directory at the *end* of the file. This means the ZIP parser doesn't care if there is extra data (like a shebang) at the *start* of the file.
*   **The OS**: The OS sees the shebang and executes the file using the Python interpreter.
*   **The Interpreter**: Python recognizes it's a zip file, mounts it, and executes the `__main__.py` file inside.

### 61.2 Creating a `zipapp`
```python
import zipapp
zipapp.create_archive('myapp_dir', 'myapp.pyz', interpreter='/usr/bin/python3', main='myapp:main')
```
This produces a single, portable executable file that contains all your code and non-binary dependencies.

---

# Chapter 62: The Disassembler (`dis`)

To reach "Godhood," you must be able to read the machine code of the Python Virtual Machine: **Bytecode**.

### 62.1 The Python VM: A Stack Machine

The CPython VM is a **Stack Machine**. Operations push values onto a stack and pop them to perform calculations.

#### 1. Dissecting an Operation
```python
def add(a, b):
    return a + b

import dis
dis.dis(add)
```
*Output:*
```
  2           0 LOAD_FAST                0 (a)
              2 LOAD_FAST                1 (b)
              4 BINARY_ADD
              6 RETURN_VALUE
```
*   **`LOAD_FAST`**: Pushes the value of a local variable onto the stack.
*   **`BINARY_ADD`**: Pops the top two values, adds them (using the `tp_as_number->nb_add` C slot), and pushes the result back.
*   **`RETURN_VALUE`**: Pops the top value and returns it to the caller.

### 62.2 Bytecode Specialization (Python 3.11+)

In modern Python, you may see `RESUME` or "Specialized" opcodes like `BINARY_OP_ADD_INT`.
*   **Inline Caching**: If the VM sees that a specific `BINARY_ADD` is always adding two integers, it replaces the generic opcode with a specialized version that skips the type-checking overhead, resulting in significant speedups (as discussed in Chapter 17).

---

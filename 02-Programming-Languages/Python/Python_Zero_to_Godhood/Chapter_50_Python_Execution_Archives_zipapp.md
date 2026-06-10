# Python Execution Archives (`zipapp`)


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

# Appendix U: Exhaustive Python Built-in Warnings


Warnings are usually emitted in situations where it is useful to alert the user of some condition in a program, but the condition doesn't warrant raising an exception and terminating the program.

### U.1 Warning Hierarchy
```text
Warning
 +-- UserWarning
 +-- DeprecationWarning
 +-- PendingDeprecationWarning
 +-- SyntaxWarning
 +-- RuntimeWarning
 +-- FutureWarning
 +-- ImportWarning
 +-- UnicodeWarning
 +-- BytesWarning
 +-- EncodingWarning
 +-- ResourceWarning
```

### U.2 Technical Descriptions
*   **`DeprecationWarning`**: Base class for warnings about deprecated features when those warnings are intended for other Python developers.
*   **`FutureWarning`**: Base class for warnings about deprecated features when those warnings are intended for end users of applications that are written in Python.
*   **`RuntimeWarning`**: Base class for warnings about dubious runtime behavior.
*   **`SyntaxWarning`**: Base class for warnings about dubious syntax.
*   **`ImportWarning`**: Base class for warnings about probable mistakes in module imports.
*   **`UnicodeWarning`**: Base class for warnings related to Unicode.
*   **`BytesWarning`**: Base class for warnings related to `bytes` and `bytearray`.
*   **`ResourceWarning`**: Base class for warnings related to resource usage (e.g., unclosed files).

---

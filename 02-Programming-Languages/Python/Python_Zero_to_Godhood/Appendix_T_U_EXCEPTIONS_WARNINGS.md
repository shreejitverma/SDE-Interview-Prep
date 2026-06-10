# Appendix T: Exhaustive Python Built-in Exceptions

This appendix provides a complete hierarchy and description of all built-in exceptions in Python 3.13.

### T.1 Exception Hierarchy
```text
BaseException
 +-- SystemExit
 +-- KeyboardInterrupt
 +-- GeneratorExit
 +-- Exception
      +-- StopIteration
      +-- StopAsyncIteration
      +-- ArithmeticError
      |    +-- FloatingPointError
      |    +-- OverflowError
      |    +-- ZeroDivisionError
      +-- AssertionError
      +-- AttributeError
      +-- BufferError
      +-- EOFError
      +-- ImportError
      |    +-- ModuleNotFoundError
      +-- LookupError
      |    +-- IndexError
      |    +-- KeyError
      +-- MemoryError
      +-- NameError
      |    +-- UnboundLocalError
      +-- OSError
      |    +-- BlockingIOError
      |    +-- ChildProcessError
      |    +-- ConnectionError
      |    |    +-- BrokenPipeError
      |    |    +-- ConnectionAbortedError
      |    |    +-- ConnectionRefusedError
      |    |    +-- ConnectionResetError
      |    +-- FileExistsError
      |    +-- FileNotFoundError
      |    +-- InterruptedError
      |    +-- IsADirectoryError
      |    +-- NotADirectoryError
      |    +-- PermissionError
      |    +-- ProcessLookupError
      |    +-- TimeoutError
      +-- ReferenceError
      +-- RuntimeError
      |    +-- NotImplementedError
      |    +-- RecursionError
      +-- SyntaxError
      |    +-- IndentationError
      |         +-- TabError
      +-- SystemError
      +-- TypeError
      +-- ValueError
      |    +-- UnicodeError
      |         +-- UnicodeDecodeError
      |         +-- UnicodeEncodeError
      |         +-- UnicodeTranslateError
      +-- Warning (See Appendix U)
```

### T.2 Technical Descriptions
*   **`ArithmeticError`**: Base class for all errors that occur for numeric calculations.
*   **`AssertionError`**: Raised when an `assert` statement fails.
*   **`AttributeError`**: Raised when an attribute reference or assignment fails.
*   **`ImportError`**: Raised when the `import` statement has troubles loading a module.
*   **`LookupError`**: Base class for the errors that occur when a key or index used on a mapping or sequence is invalid.
*   **`MemoryError`**: Raised when an operation runs out of memory but the condition may still be rescued (e.g., by deleting some objects).
*   **`NameError`**: Raised when a local or global name is not found.
*   **`OSError`**: Raised when a system function returns a system-related error.
*   **`RuntimeError`**: Raised when an error is detected that doesn't fall in any of the other categories.
*   **`TypeError`**: Raised when an operation or function is applied to an object of inappropriate type.
*   **`ValueError`**: Raised when a built-in operation or function receives an argument that has the right type but an inappropriate value.

---

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

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

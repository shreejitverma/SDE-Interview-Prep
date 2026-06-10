# EXCEPTION GROUPS AND TRACEBACK ENHANCEMENTS


### 19.1 Exception Groups (`ExceptionGroup` & `BaseExceptionGroup` / PEP 654)
Introduced in Python 3.11, `ExceptionGroup` and `BaseExceptionGroup` enable raising and handling multiple unrelated exceptions simultaneously. This is critical for concurrent frameworks (such as `asyncio` TaskGroups) where multiple background operations can crash concurrently.

#### 1. Exception Group Class Layout
An `ExceptionGroup` wraps a descriptive string and a list of sub-exceptions:
```python
class ExceptionGroup(BaseException):
    def __init__(self, message: str, exceptions: Sequence[BaseException]) -> None:
        self.message = message
        self.exceptions = list(exceptions)
```

---

### 19.2 The `except*` Clause and Exception Tree Filtering
To handle individual branches of an exception group, Python 3.11 introduced the `except*` statement. Unlike a traditional `except` statement, which catches a single exception instance, `except*` extracts a matching subset from the Exception Group hierarchy, letting unmatched exceptions propagate.

#### 1. Execution Trace of Exception Splitting
Consider the following exception structure and matching logic:

```
            ExceptionGroup("Main Group")
            /                        \
    ValueError("Error A")       TypeError("Error B")
```

```python
try:
    raise ExceptionGroup("Main Group", [ValueError("Error A"), TypeError("Error B")])
except* ValueError as eg:
    print("Caught Val:", eg.exceptions)
```

During execution, the runtime:
1. Catches the `ExceptionGroup`.
2. Filters out the `ValueError("Error A")` exception and wraps it in a new sub-ExceptionGroup.
3. Binds this sub-ExceptionGroup to the local variable `eg`.
4. Leaves the `TypeError("Error B")` exception in the original group, which continues propagating up the exception handler chain.

---

### 19.3 Traceback Representation and `add_note()`
*   **Traceback Trees**: Because exception groups represent hierarchical trees of errors, CPython's traceback generator is modified to format tracebacks as tree diagrams.
*   **PEP 678 Exception Notes**: The `add_note(note)` method allows adding custom text strings directly to exceptions. These notes are appended to the exception's traceback output when printed, aiding debugging without modifying the exception's arguments.

---


# SYNTAX ERGONOMICS: F-STRINGS & TYPE ANNOTATIONS


### 12.1 PEP 498: Formatted String Literals (f-strings)
Introduced in Python 3.6, **PEP 498** added formatted string literals (f-strings). These are significantly faster than legacy formatting methods like `%` or `.format()`:
*   **Legacy Overheads**: `.format()` requires a lookup attribute call (`__getattribute__`) and executes a full function call frame.
*   **f-string Compilation**: The compiler evaluates f-string expressions at compile time and emits optimized bytecodes:
    - `FORMAT_VALUE`: Formats a single value on the stack.
    - `BUILD_STRING`: Concatenates values directly in C, bypassing function call overhead.

```python
import dis

def legacy_format(name, age):
    return "Name: {}, Age: {}".format(name, age)

def fstring_format(name, age):
    return f"Name: {name}, Age: {age}"

print("Bytecode for f-string:")
dis.dis(fstring_format)
```
```
  2           0 LOAD_CONST                1 ('Name: ')
              2 LOAD_FAST                0 (name)
              4 FORMAT_VALUE             0
              6 LOAD_CONST                2 (', Age: ')
              8 LOAD_FAST                1 (age)
             10 FORMAT_VALUE             0
             12 BUILD_STRING             4
             14 RETURN_VALUE
```
The f-string compiles to a flat sequence of `FORMAT_VALUE` and `BUILD_STRING` instructions, achieving performance comparable to manual string concatenation.

### 12.2 PEP 526: Variable & Function Type Annotations
*   **The Static Boundary**: Annotations (introduced for functions in PEP 3107 and variables in PEP 526) are static metadata. They are not checked at runtime and do not impact execution speed.
*   **The Annotation Store**: When a module is loaded, CPython compiles annotations and stores them in the class or module namespace under the `__annotations__` dictionary.
*   **Inspection**: Type checkers (like Mypy) read this metadata statically, while runtime libraries (like Pydantic) use the `__annotations__` dictionary or `inspect.get_type_hints()` to enforce types dynamically.

---

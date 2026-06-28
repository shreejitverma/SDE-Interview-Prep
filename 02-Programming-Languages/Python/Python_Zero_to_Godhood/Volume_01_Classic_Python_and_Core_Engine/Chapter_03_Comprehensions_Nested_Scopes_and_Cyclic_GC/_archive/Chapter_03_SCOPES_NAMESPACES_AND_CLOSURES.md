# SCOPES, NAMESPACES, AND CLOSURES


### 3.1 The LEGB Name Resolution Engine
When CPython decodes variables at runtime, it evaluates namespaces sequentially:
1.  **L (Local)**: Fast array lookup inside the active `PyFrameObject` (for variables defined inside the current function frame).
2.  **E (Enclosing)**: Looks up variables in active parent frames (closures).
3.  **G (Global)**: Lookup inside the module's dictionary `__dict__`.
4.  **B (Built-in)**: Lookup inside the standard built-in namespace module dictionary `__builtins__`.

*   **Bytecode Indicators**: The compiler dictates lookup speeds:
    - `LOAD_FAST`: Direct index read from the local frame variables array. Extremely fast.
    - `LOAD_DEREF`: Loads a variable enclosed in a cell (closure).
    - `LOAD_GLOBAL`: Searches module global dict, followed by built-in dict.
    - `LOAD_NAME`: Fallback lookup used at the module top level.

```python
import dis

def fast_local_demo(x):
    y = x + 1
    return y

print("Bytecode optimizing local lookups:")
dis.dis(fast_local_demo)
```
```
  2           0 LOAD_FAST                0 (x)
              2 LOAD_CONST                1 (1)
              4 BINARY_ADD
              6 STORE_FAST               1 (y)

  3           8 LOAD_FAST                1 (y)
             10 RETURN_VALUE
```
Notice the compiler emits `LOAD_FAST` and `STORE_FAST` for local scopes rather than dictionary calls.

### 3.2 Closure Mechanics: PyCellObject and Free Variables
When a function executes, its frame object is popped off the stack, and local variables are reclaimed. However, closures require variables from enclosing scopes to remain alive.
*   **The Cell Structure**: Python solves this using `PyCellObject` structs.
    - If a variable is enclosed by an inner function, the compiler marks it as a "cell variable".
    - Instead of placing the raw value on the local frame stack, CPython wraps it inside a `PyCellObject` (which lives on the heap).
    - Both outer and inner functions store pointers to this shared cell, keeping the variable alive even after the outer function terminates.

```
Closure Cell Layout:
[Outer Frame] ---> [Cell: PyCellObject] <--- [Inner Frame]
                         |
                         v
                  [Value: PyObject]
```

Let's trace cell internals using the `inspect` module:

```python
import inspect

def outer_scope(multiplier):
    secret_value = 100
    def inner_scope(val):
        # 'secret_value' and 'multiplier' are free variables
        return val * multiplier + secret_value
    return inner_scope

closure_fn = outer_scope(5)

# Inspect closure cells
for i, cell in enumerate(closure_fn.__closure__):
    print(f"Cell {i} contents:", cell.cell_contents)

print("Free variables list (co_freevars):", closure_fn.__code__.co_freevars)
```

### 3.3 Scope Execution Properties
We can verify which variables are mapped to cells versus free variables by checking the compiled code object lists:
*   `co_cellvars`: Tuple containing names of local variables referenced by nested functions.
*   `co_freevars`: Tuple containing names of variables referenced from enclosing scopes.

```python
def parent():
    cell_var = "shared"
    def child():
        print(cell_var) # free variable
    return child

print("Parent cell vars:", parent.__code__.co_cellvars)
print("Child free vars:", parent().__code__.co_freevars)
```

---

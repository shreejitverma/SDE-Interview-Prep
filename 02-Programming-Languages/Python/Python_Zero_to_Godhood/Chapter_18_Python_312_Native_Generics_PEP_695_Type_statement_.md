# Python 3.12: Native Generics (PEP 695), Type statement, and Subinterpreters


### 18.1 PEP 695 Type Parameter Syntax
Python 3.12 introduced a clean, native syntax for generic classes, generic functions, and type aliases using type parameters enclosed in square brackets.

#### 1. Transition from Legacy Declarations
Prior to Python 3.12, defining generic types required importing and manually instantiating `TypeVar` variables, along with explicitly defining variance constraints (covariant, contravariant, or invariant):
```python
# Legacy Python 3.11-
from typing import TypeVar, Generic

T = TypeVar('T', covariant=True)  # Manual variance specification

class Stack(Generic[T]):
    pass
```

PEP 695 replaces this boiler-plate with a cleaner syntax:
```python
# Modern Python 3.12+
class Stack[T]:
    pass
```
Under this syntax:
*   CPython automatically creates the type variable `T` (instance of `typing.TypeVar`) and binds it to the class scope.
*   **Static Auto-Variance**: Static type checkers (like Mypy/Pyright) automatically infer the variance of `T` by analyzing how the variable is used inside the class definition, completely eliminating the need for manual `covariant=True` or `contravariant=True` flags.

#### 2. Bound and Constraint Specifications
Type parameter bounds and constraints are defined directly inside the square brackets:
*   **Bound Syntax (`T: bound_type`)**: Restricts the type variable to a specific subclass or type.
    ```python
    def process[T: int](val: T) -> T:
        return val
    ```
*   **Constraint Syntax (`T: (type1, type2, ...)`)**: Restricts the type variable to one of a set of explicit types.
    ```python
    def parse[T: (str, bytes)](data: T) -> T:
        return data
    ```

---

### 18.2 Annotation Scopes and Lazy Evaluation
In Python 3.11 and below, type parameter bounds and type aliases were evaluated eagerly at module import time. This caused circular import errors (if a type variable referenced a class defined later in another module) and increased startup memory footprints.
PEP 695 resolves this by introducing **Annotation Scopes** and lazy evaluation.

#### 1. Annotation Scopes
When the compiler encounters type parameters or the `type` statement, it wraps their evaluation code inside a new lexical scope called an **Annotation Scope**:
*   An annotation scope is a nested compiler-generated scope (similar to a hidden function block).
*   Variables and bounds defined inside this scope are evaluated **lazily**only when they are explicitly queried at runtime.

#### 2. Bytecode Disassembly of the `type` Statement
Let's analyze how CPython compiles a modern type alias:
```python
type Vector3D[T] = tuple[T, T, T]
```

##### Compiled Bytecode:
```
  1           0 LOAD_CONST               0 ('Vector3D')
              2 LOAD_CONST               1 ('T')
              4 LOAD_CONST               2 (<code object Vector3D at 0x...>)
              6 MAKE_FUNCTION            0
              8 SET_FUNCTION_ATTRIBUTE   8 (closure/annotations helper)
             10 CALL_FUNCTION            0
             12 BUILD_TYPEALIAS
             14 STORE_NAME               0 (Vector3D)
```
*   **`MAKE_FUNCTION`**: Creates a lazy evaluation function from the nested code object representing the alias value `tuple[T, T, T]`.
*   **`BUILD_TYPEALIAS`**: Pops the evaluation function, type parameters, and name, then constructs an instance of `typing.TypeAliasType`.
*   **`STORE_NAME`**: Binds the type alias object to `Vector3D`.

#### 3. C-Level Representation of `TypeAliasType`
At the C level, `TypeAliasType` is represented by a dedicated struct wrapping the properties of the type alias:
*   `__name__`: Name of the alias.
*   `__type_params__`: A tuple of type parameters.
*   `__value__`: The aliased type. It uses a C descriptor getter that evaluates the lazy function code object on the first access, caching the result to avoid redundant evaluations.

---

### 18.3 PEP 684: Per-Interpreter GIL and Subinterpreters
CPython has supported subinterpreters via its C-API since Python 1.5. However, because they all shared a single Global Interpreter Lock (GIL), only one interpreter could execute bytecode at a time, preventing true multi-core parallel execution.
Python 3.12 introduced **PEP 684**, providing a dedicated, isolated GIL for each subinterpreter.

#### 1. C-Level Structural Separation
CPython separates runtime state into two major structures:
*   **`PyRuntimeState`** (`pycore_runtime.h`): Process-wide, shared global resources, including system memory allocators, signal handlers, and GC arena lists.
*   **`PyInterpreterState`** (`pycore_pystate.h`): Interpreter-specific resources. 

Under PEP 684, each `PyInterpreterState` is allocated its own dedicated GIL struct (`struct _gil_runtime_state`). This allows multiple interpreter instances to execute bytecode in parallel on separate threads, leveraging separate CPU cores.

```
CPython Process Layout with Per-Interpreter GIL:

======================= [ PyRuntimeState ] =======================
  /                                                             \
 [ PyInterpreterState A ]              [ PyInterpreterState B ]
    - sys.modules                         - sys.modules
    - Private Object Heap                 - Private Object Heap
    - Dedicated GIL A                     - Dedicated GIL B
        |                                     |
        v                                     v
   ( OS Thread 1 )                       ( OS Thread 2 )
==================================================================
```

#### 2. Isolation Boundaries
To prevent concurrency race conditions, interpreters maintain strict boundaries:
*   **Private Module Cache**: Each subinterpreter has its own `sys.modules` dictionary.
*   **Heap Isolation**: Python heap objects (`PyObject` references) cannot be shared directly between interpreters. If interpreter A attempts to read or write a `PyObject` allocated in interpreter B:
    1.  Reference counting checks (`Py_INCREF`/`Py_DECREF`) will trigger race conditions.
    2.  Garbage collection passes running in interpreter A will attempt to collect memory belonging to interpreter B, causing memory corruption.
*   **Communication Channel**: Subinterpreters communicate exclusively through serialized message channels (such as `_xxsubinterpreters`). Data is serialized (e.g. converted to raw bytes or using shared memory structures via the C buffer protocol) and then re-materialized as fresh objects in the target interpreter's private heap.

#### 3. Parallel Execution Code Example
We can spawn subinterpreters and execute code concurrently using the `_xxsubinterpreters` module:

```python
import _xxsubinterpreters as interpreters
import threading

def run_in_subinterpreter(interp_id, script):
    # Run script inside the isolated interpreter context
    interpreters.run_string(interp_id, script)

# Create a new subinterpreter with an isolated GIL
interp_id = interpreters.create()

script = """
import time
# Executes concurrently on a separate CPU core
result = sum(i * i for i in range(10_000_000))
print("Subinterpreter result calculation complete:", result)
"""

# Spawn a separate OS thread to run the subinterpreter in parallel
thread = threading.Thread(target=run_in_subinterpreter, args=(interp_id, script))
thread.start()
thread.join()

# Destroy the subinterpreter and release resources
interpreters.destroy(interp_id)
```

---

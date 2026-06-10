# Python 3.5: Native Async/Await Coroutines and Matrix Operations


### 11.1 PEP 492: Native Coroutines (`async/await` Internals)
Python 3.5 introduced native coroutines via PEP 492, establishing a clear separation between standard generators and asynchronous tasks to prevent logical developer errors.

#### 1. The Generator-Coroutine Limitation
In Python 3.4, coroutines were decorated generators (`@asyncio.coroutine`). Under the hood, these coroutines were instances of `PyGenObject`. Because they shared the same C type as standard generators, they exposed the standard iterator protocol. Developers could mistakenly iterate over a coroutine using a `for` loop or call `next()` on it directly, causing unexpected runtime behavior.
Additionally, this lack of syntactic separation meant that:
* The interpreter could not perform static analysis to detect missing `await`/`yield from` calls on coroutine objects.
* It was possible to call `next(coro())` directly, stepping the generator and bypassing the event loop's state machine.
* The `yield from` syntax was overloaded, being used for both yielding from standard generators and awaiting asynchronous tasks, creating a high degree of cognitive load and making code refactoring prone to errors.

#### 2. CPython Representation: `PyCoroObject`
Native coroutines declared using `async def` compile directly to a specialized C structure, `PyCoroObject` (`Include/genobject.h`):
```c
typedef struct {
    PyObject_HEAD
    struct _frame *cr_frame;     /* Frame object executing coroutine code */
    PyObject *cr_code;          /* Code object compiled from async def */
    PyObject *cr_name;          /* Coroutine name */
    PyObject *cr_qualname;      /* Qualified name */
    PyObject *cr_origin;        /* Awaited task or future origin trace */
    PyObject *cr_weakreflist;   /* Weak reference list pointer */
    char cr_running;            /* Flag indicating if coroutine is active */
} PyCoroObject;
```

Let's break down the role of each field in the CPython runtime:
* **`PyObject_HEAD`**: The standard object header containing the reference counter (`ob_refcnt`) and the pointer to the type object (`ob_type`). In this case, `ob_type` points to the `PyCoro_Type` struct.
* **`cr_frame`**: A pointer to the execution frame (`PyFrameObject`). It holds the local execution state, the bytecode pointer, the local evaluation stack, and local variable cells.
* **`cr_code`**: Points to the `PyCodeObject` representing the compiled bytecode.
* **`cr_name`**: The coroutine's name (represented by a `PyUnicodeObject`).
* **`cr_qualname`**: The qualified name (e.g. `ClassName.method_name`).
* **`cr_origin`**: If debugging is enabled via `sys.set_coroutine_origin_tracking_depth()`, this pointer stores a traceback tuple showing where the coroutine was instantiated, enabling deep debugging of orphaned or un-awaited coroutines.
* **`cr_weakreflist`**: Head of a doubly-linked list of weak reference objects pointing to this coroutine.
* **`cr_running`**: A single-byte boolean flag. If `cr_running` is true, it indicates the coroutine frame is currently executing. Any attempt to resume the coroutine from another context will raise a `RuntimeError: coroutine already running`, protecting the runtime from re-entrant frame execution.

Because `PyCoroObject` is a distinct C type, it does not implement sequence or iterator slots (`tp_iter` is NULL), preventing standard iteration errors.

#### 3. The `am_await` Slot Protocol
Instead of iteration slots, `PyCoroObject` implements the `tp_as_async` protocol table, specifically the `am_await` slot:
```c
typedef struct {
    unaryfunc am_await;         /* __await__ implementation pointer */
    unaryfunc am_aiter;         /* __aiter__ implementation pointer */
    unaryfunc am_anext;         /* __anext__ implementation pointer */
} PyAsyncMethods;
```

When an object is awaited via `await expr`, the CPython interpreter executes the following internal evaluation loop logic:
1. It checks if the object's type has `tp_as_async` populated and if `tp_as_async->am_await` is non-NULL.
2. If yes, it calls `am_await(expr)`. This slot function MUST return an iterator (an object that implements `tp_iternext`).
3. For native coroutines (`PyCoroObject`), their type has a custom `am_await` slot that wraps the coroutine in a `PyCoroWrapper` (or returns a wrapper that exposes standard iterator operations for the event loop to drive).
4. For user-defined classes, a custom `__await__` method is defined. At the C level, this maps to `am_await` resolving the python-level `__await__` method. It must return an iterator.

```python
# User-level custom awaitable implementation
class DatabaseConnection:
    def __await__(self):
        # We yield control back to the event loop if the socket is not ready
        while not self.is_connected():
            yield None
        return self._connection
```

#### 4. Bytecode Compilation and Tracing: `GET_AWAITABLE`
When the compiler parses an `await expression` statement, it emits a specialized **`GET_AWAITABLE`** bytecode instruction:
1. `GET_AWAITABLE` pops the expression result off the evaluation stack.
2. The VM checks if the object implements the `am_await` slot (either natively or via class overrides).
3. If valid, the VM calls the slot to retrieve the awaitable iterator and pushes it onto the stack. If not valid, it raises a `TypeError`.
4. This is followed by a delegation loop (similar to `yield from`) that yields control back to the event loop if the awaitable is pending.

Let's trace the compiled bytecodes of an asynchronous execution path. Consider the following code:
```python
async def get_val():
    return 42

async def main():
    val = await get_val()
```

The CPython 3.5 compiler generates the following disassemblies:

##### Disassembly of `get_val`:
```
  2           0 LOAD_CONST               1 (42)
              3 RETURN_VALUE
```

##### Disassembly of `main`:
```
  5           0 LOAD_GLOBAL              0 (get_val)
              3 CALL_FUNCTION            0
              6 GET_AWAITABLE            0
              9 LOAD_CONST               0 (None)
             12 YIELD_FROM               0
             15 STORE_FAST               0 (val)
             18 LOAD_CONST               0 (None)
             21 RETURN_VALUE
```

##### Step-by-Step VM Execution Trace of `main()`:
1. **`LOAD_GLOBAL 0`**: Resolves the identifier `get_val` from the global namespace dictionary and pushes the function object onto the evaluation stack.
2. **`CALL_FUNCTION 0`**: Invokes `get_val()`. Since it was defined with `async def`, the VM immediately creates a new `PyCoroObject` and pushes it onto the evaluation stack. Note that the body of `get_val` does not execute yet.
3. **`GET_AWAITABLE`**:
   * Pops the `PyCoroObject` off the stack.
   * Accesses `ob_type->tp_as_async->am_await`.
   * Executes the slot, which wraps the native coroutine or verifies it is ready to be driven.
   * Pushes the resulting awaitable iterator back onto the stack.
4. **`LOAD_CONST 0`**: Pushes `None` onto the stack as the priming value.
5. **`YIELD_FROM`**:
   * Performs a loop that mimics `yield from` behavior.
   * It pops the value (`None`) and sends it to the awaitable iterator by invoking its `tp_iternext` (or equivalent `send` slot).
   * The sub-frame `get_val` runs and reaches `RETURN_VALUE`. The runtime raises a `StopIteration` containing the return value `42`.
   * `YIELD_FROM` catches `StopIteration`, extracts `42` from the exception's `value` attribute, and pushes it onto the evaluation stack.
6. **`STORE_FAST 0`**: Pops the value `42` off the stack and stores it in local variable `val`.

```
Evaluation Stack Transitions during `await`:

Step 1: [ get_val (fn) ]  <-- LOAD_GLOBAL
Step 2: [ PyCoroObject ]  <-- CALL_FUNCTION
Step 3: [ CoroWrapper ]   <-- GET_AWAITABLE
Step 4: [ CoroWrapper ]   <-- LOAD_CONST (None)
        [    None     ]
Step 5: [     42      ]   <-- YIELD_FROM (catches StopIteration(42))
Step 6: [             ]   <-- STORE_FAST (val = 42)
```

---

### 11.2 Matrix Multiplication Operator (`@` / PEP 465)
Python 3.5 introduced the binary operator `@` (and its in-place version `@=`) to support clean mathematical syntax for matrix multiplication.

#### 1. Low-Level C Slots Mapping
CPython maps the matrix multiplication operator to two new function pointer slots inside the `PyNumberMethods` struct (`Include/object.h`):
```c
typedef struct {
    /* ... */
    binaryfunc nb_matrix_multiply;          /* Corresponds to __matmul__ */
    binaryfunc nb_inplace_matrix_multiply;  /* Corresponds to __imatmul__ */
} PyNumberMethods;
```

When evaluating `A @ B`, CPython's binary operation dispatch system invokes `PyNumber_MatrixMultiply(A, B)`:
1. **Left-to-Right Dispatch**: If the type of `A` defines `tp_as_number->nb_matrix_multiply`, it executes the slot function.
2. **Right-to-Left Fallback**: If `A`'s slot is NULL, or it returns `Py_NotImplemented`, CPython checks if the type of `B` defines `nb_matrix_multiply`.
3. **Subclass Precedence**: If `B` is a subclass of `A` and overrides `nb_matrix_multiply`, `B`'s slot is checked and called *before* `A`'s slot, allowing specialized subclass multiplication overrides.
4. **TypeError**: If neither slot returns a valid object (or both return `Py_NotImplemented`), the VM raises a `TypeError: unsupported operand type(s) for @`.

#### 2. Python-Level Interface and Numeric Implementations
Developers can customize this operator behavior by implementing the Python magic methods `__matmul__`, `__rmatmul__`, and `__imatmul__`:

```python
class CustomMatrix:
    def __init__(self, grid):
        self.grid = grid

    def __matmul__(self, other):
        if not isinstance(other, CustomMatrix):
            return NotImplemented
        # Compute the dot product matrix
        rows_A, cols_A = len(self.grid), len(self.grid[0])
        rows_B, cols_B = len(other.grid), len(other.grid[0])
        assert cols_A == rows_B, "Dimension mismatch!"
        
        result = [[0] * cols_B for _ in range(rows_A)]
        for i in range(rows_A):
            for j in range(cols_B):
                result[i][j] = sum(self.grid[i][k] * other.grid[k][j] for k in range(cols_A))
        return CustomMatrix(result)
```

#### 3. High-Performance Implementations
This dedicated operator allowed numerical libraries (like NumPy) to bypass verbose nested function call chains. Consider this linear algebra comparison:
```python
# Pre-Python 3.5 (Verbose and nesting-heavy)
result = A.dot(B).dot(C) + D.dot(E)

# Python 3.5+ (Clean, mathematical representation)
result = (A @ B @ C) + (D @ E)
```
NumPy defines C-level slots mapping for `nb_matrix_multiply` on its `ndarray` types, executing optimized BLAS or LAPACK matrix routines underneath. By pushing the matrix multiplication to these compiled C/Fortran libraries, they release the Global Interpreter Lock (GIL) during compilation of large arrays, permitting true multi-threaded parallel computation across CPU cores.

---

### 11.3 Extended Unpacking Generalizations (PEP 448)
PEP 448 expanded the capabilities of the unpacking operators `*` (iterable unpacking) and `**` (dictionary unpacking), allowing multiple unpacks inside collection literals and function calls.

#### 1. Bytecode Compilation and Unpacking Instructions
Prior to Python 3.5, unpacking was limited to a single occurrence. To support multiple unpacking operations, the CPython compiler was updated to emit specialized group unpacking bytecodes:
* **`BUILD_LIST_UNPACK`**: Pops multiple sequences off the stack, converts them to tuples/lists, and concatenates them into a single list.
* **`BUILD_TUPLE_UNPACK`**: Merges multiple popped sequences into a tuple.
* **`BUILD_SET_UNPACK`**: Merges sequences into a set, deduplicating elements.
* **`BUILD_MAP_UNPACK`**: Pops multiple dictionaries/mappings from the stack and merges them into a single dictionary.

*Note on Compilation Evolution*: In later versions of CPython (e.g., Python 3.9+), these unpacking instructions were replaced by highly specialized, lower-overhead instructions such as `LIST_EXTEND`, `DICT_MERGE`, and `DICT_UPDATE` to avoid the temporary list creation overhead for every unpacked component.

#### 2. Detailed Bytecode Traces

##### Case A: List Unpacking with Multiple Iterables
Consider compiling this list literal containing mixed values and unpacking:
```python
[1, *[2, 3], 4]
```

The Python 3.5 compiler generates the following bytecode:
```
  1           0 LOAD_CONST               0 (1)
              3 BUILD_LIST               1              /* Push list [1] onto stack */
              6 LOAD_CONST               1 (2)
              9 LOAD_CONST               2 (3)
             12 BUILD_LIST               2              /* Push list [2, 3] onto stack */
             15 LOAD_CONST               3 (4)
             18 BUILD_LIST               1              /* Push list [4] onto stack */
             21 BUILD_LIST_UNPACK        3              /* Merge the 3 list elements on the stack */
```

`BUILD_LIST_UNPACK 3` pops the three constructed list elements off the stack, performs dynamic sequence iteration to concatenate their values, and pushes the final unified list `[1, 2, 3, 4]` back onto the evaluation stack.

##### Case B: Dictionary Unpacking with Multiple Mappings
Consider this dictionary literal merging two mappings:
```python
y = {**d1, **d2, 'key': 42}
```

The Python 3.5 compiler generates the following bytecode:
```
  1           0 LOAD_FAST                0 (d1)         /* Push dictionary d1 */
              3 LOAD_FAST                1 (d2)         /* Push dictionary d2 */
              6 LOAD_CONST               2 ('key')
              9 LOAD_CONST               3 (42)
             12 BUILD_MAP                1              /* Push dictionary {'key': 42} */
             15 BUILD_MAP_UNPACK         3              /* Merge the 3 dict elements on the stack */
             18 STORE_FAST               2 (y)
```

During execution:
* `BUILD_MAP_UNPACK 3` evaluates the three mappings popped from the stack.
* It iterates through their keys, updating the target dictionary.
* In Python 3.5, key collisions do not raise an error for dictionary literals; rather, keys to the right overwrite keys to the left (e.g., `d2` overrides `d1`, and `'key'` overrides any prior matching key).
* In contrast, if multiple unpacking overlaps occur during keyword argument calls (e.g. `func(**d1, **d2)`), duplicate keys trigger a runtime `TypeError` exception.

---


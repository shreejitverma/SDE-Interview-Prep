# THE PYTHON 3.0 PARADIGM SHIFT


### 7.1 The Print Statement Redesign
In Python 2, `print` was a keyword statement. This limited extensibility because it could not be passed as a callback or configured dynamically.
*   **Legacy Bytecode**: The Python 2 compiler generated specialized bytecodes `PRINT_ITEM` and `PRINT_NEWLINE` to write directly to standard output.
*   **The Modern print() Function**: In Python 3, `print()` is a built-in function loaded via standard lookup processes. The call signature accepts keyword configurations:
    `print(*objects, sep=' ', end='\n', file=None, flush=False)`
    Under the hood, `file` defaults to standard sys.stdout wrapper, and `flush=True` immediately flushes the underlying buffer, invoking the stream's write routine.

### 7.2 Division Unification
Python 2 used C-style integer division for the `/` operator (i.e. $5 / 2 = 2$), which led to silent bugs.
*   **CPython Numeric Slots**: Python 3 mapped operations to distinct type object slots:
    - True Division (`/`): Routes to the slot `nb_true_divide`, which converts inputs to floats and returns the exact decimal division ($5 / 2 = 2.5$).
    - Floor Division (`//`): Routes to the slot `nb_floor_divide`, executing division and applying floor rounding ($5 // 2 = 2$).

### 7.3 Lazy Iterators & Resource Conservation
To prevent allocating large lists in memory, Python 3 converted core built-ins to return dynamic iterators:
*   **Built-in Conversions**: `map`, `filter`, and `zip` return custom lazy iterator types (`map`, `filter`, `zip` objects) that yield items on demand.
*   **The range() Overhaul**: In Python 2, `range()` returned a fully allocated list, while `xrange()` was a custom sequence wrapper. Python 3 merged these: `range()` behaves like `xrange()`, representing an immutable sequence of numbers that computes values lazily, requiring only $O(1)$ memory.

### 7.4 Dictionary View Objects
Python 2's dictionary inspection methods (`dict.keys()`, `dict.values()`, `dict.items()`) returned copies of the keys/values as separate list objects. In large applications, this caused high allocation overhead.
*   **Dictionary Views**: Python 3 replaced these with **view objects** (`dict_keys`, `dict_values`, `dict_items`). View objects do not copy dictionary items; instead, they maintain a direct pointer to the dictionary's internal entries table. Any modifications to the dictionary are immediately reflected in the view, keeping memory usage at $O(1)$.

---

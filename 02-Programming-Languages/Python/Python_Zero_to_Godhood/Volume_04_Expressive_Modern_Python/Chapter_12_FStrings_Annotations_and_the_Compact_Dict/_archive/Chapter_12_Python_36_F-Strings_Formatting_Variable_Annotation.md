# Python 3.6: F-Strings Formatting, Variable Annotations, and Compact Dicts


### 12.1 PEP 498: Formatted String Literals (f-strings)
Python 3.6 introduced Formatted String Literals, commonly referred to as **f-strings**, to provide a clean, readable, and highly performant mechanism for string interpolation.

#### 1. Legacy Formatting Overheads
Prior to Python 3.6, developers relied on `%` (printf-style) formatting or the `.format()` string method. Both approaches introduce significant runtime overhead:
*   **Percent (`%`) Formatting**: Uses ancient C-style printf routines. It is structurally rigid, struggles to handle complex container types gracefully, and requires allocating temporary tuples for positional parameters.
*   **Format Method (`.format()`)**: Introduces two major layers of overhead:
    1.  **Attribute Resolution**: Evaluating `"{}".format(x)` requires calling the `__getattribute__` method on the string object to locate the `.format` method.
    2.  **Function Call Overhead**: Invoking `.format()` pushes a new CPython call frame onto the stack, builds temporary positional arguments tuple `*args` and keyword arguments dictionary `**kwargs`, and parses the format string *at runtime* on every single invocation.

#### 2. f-String Compilation Mechanics
f-strings bypass these overheads by converting interpolation directly into optimized bytecode operations at compile time. When the CPython parser encounters an f-string (e.g. `f"Name: {name}, Age: {age}"`), it parses the literal components and the embedded expressions into separate AST nodes:
1.  **Static Literal Sections**: Treated as constant string literals.
2.  **Embedded Expressions**: Extracted, parsed as separate AST sub-trees, and compiled into standard bytecode instruction sequences.

The compiler generates two primary bytecode instructions to handle f-strings:
*   **`FORMAT_VALUE`**: Pops a value off the stack, applies formatting parameters (str/repr/ascii conversion or formatting specifiers), and pushes the formatted string back onto the stack.
*   **`BUILD_STRING`**: Pops a specified number of strings off the stack, performs a high-performance C-level string concatenation (`PyUnicode_Concat` or pre-allocated buffer copy), and pushes the final concatenated string onto the stack.

##### CPython `FORMAT_VALUE` Flag Specification:
The `FORMAT_VALUE` instruction takes an 8-bit integer argument representing rendering flags:
*   `0x00`: No conversion (calls `__format__` directly).
*   `0x01`: Call `str()` (corresponding to the `!s` flag).
*   `0x02`: Call `repr()` (corresponding to the `!r` flag).
*   `0x04`: Call `ascii()` (corresponding to the `!a` flag).
*   `0x08`: Have a format specifier. If this bit is set, the instruction pops two values from the stack: the format specifier string (e.g., `".2f"`) followed by the value to format.

#### 3. Bytecode Disassembly Analysis
Let's analyze the difference in compilation between `.format()` and an f-string:

```python
import dis

def legacy_format(name, age):
    return "Name: {}, Age: {}".format(name, age)

def fstring_format(name, age):
    return f"Name: {name!r:10}, Age: {age}"
```

##### Disassembly of `legacy_format`:
```
  2           0 LOAD_CONST               1 ('Name: {}, Age: {}')
              3 LOAD_ATTR                0 (format)
              6 LOAD_FAST                0 (name)
              9 LOAD_FAST                1 (age)
             12 CALL_FUNCTION            2
             15 RETURN_VALUE
```
*Note*: `LOAD_ATTR` and `CALL_FUNCTION` demonstrate the high overhead of method lookup and frame creation at runtime.

##### Disassembly of `fstring_format`:
```
  5           0 LOAD_CONST               1 ('Name: ')
              3 LOAD_FAST                0 (name)
              6 FORMAT_VALUE             2              /* repr (!r) conversion flag */
              9 LOAD_CONST               2 ('10')       /* Format specifier string */
             12 FORMAT_VALUE             10             /* Flag 0x02 (repr) + 0x08 (has spec) = 10 */
             15 LOAD_CONST               3 (', Age: ')
             18 LOAD_FAST                1 (age)
             21 FORMAT_VALUE             0              /* Default formatting */
             24 BUILD_STRING             5              /* Concatenate the 5 stack items */
             27 RETURN_VALUE
```
Because the VM calculates the lengths of all strings in the stack, `BUILD_STRING 5` pre-allocates a single contiguous memory block in the C heap and copies the characters directly, completely avoiding intermediate string allocations and Python-level function calls.

---

### 12.2 PEP 526: Syntax for Variable Annotations
PEP 526 introduced variable type annotations to complement the function parameter annotations defined in PEP 3107.

#### 1. Class and Module Level Annotations
At the module or class level, type annotations are evaluated at module import or class definition time.
1.  The compiler evaluates the annotated type expression.
2.  It creates or updates a dictionary named `__annotations__` in the module or class namespace.
3.  The type annotation metadata is stored inside this dictionary: `{'variable_name': type_object}`.

Let's examine how the CPython compiler compiles a class with variable annotations:
```python
class Profile:
    name: str = "Anonymous"
    age: int
```

##### Compiled Bytecode for `Profile` class namespace creation:
```
  1           0 LOAD_NAME                0 (__name__)
              3 STORE_NAME               1 (__module__)
              6 LOAD_CONST               0 ('Profile')
              9 STORE_NAME               2 (__qualname__)
             12 SETUP_ANNOTATIONS                       /* Initializes class __annotations__ */
             15 LOAD_NAME                3 (str)
             18 LOAD_NAME                4 (__annotations__)
             21 LOAD_CONST               1 ('name')
             24 STORE_SUBSCR                            /* Store name: str in __annotations__ */
             25 LOAD_CONST               2 ('Anonymous')
             28 STORE_NAME               5 (name)
             31 LOAD_NAME                6 (int)
             34 LOAD_NAME                4 (__annotations__)
             37 LOAD_CONST               3 ('age')
             40 STORE_SUBSCR                            /* Store age: int in __annotations__ */
             41 LOAD_CONST               4 (None)
             44 RETURN_VALUE
```
*   **`SETUP_ANNOTATIONS`**: Emitted by the compiler to initialize the `__annotations__` dictionary in the active namespace if it does not already exist.
*   **`STORE_SUBSCR`**: Updates `__annotations__` dynamically at runtime, showing that class-level annotations do carry a small runtime initialization overhead.

#### 2. Function-Level Variable Annotations
In contrast to classes and modules, variable annotations defined inside function scopes are **completely ignored** at runtime:
```python
def process():
    x: int = 42
```

##### Disassembly of `process`:
```
  2           0 LOAD_CONST               1 (42)
              3 STORE_FAST               0 (x)
              6 LOAD_CONST               0 (None)
              9 RETURN_VALUE
```
Notice that there are no type checks, no `SETUP_ANNOTATIONS`, and no `__annotations__` dictionary overhead. The type annotation metadata `int` is entirely stripped by the compiler. This ensures that function execution paths (which are executed frequently) maintain maximum speed and zero memory allocation overhead.

#### 3. Forward References and Runtime Inspection
Because class/module annotations are executed at definition time, declaring a type that has not yet been defined will raise a `NameError`:
```python
class Node:
    parent: Node  # Raises NameError: name 'Node' is not defined
```
To bypass this, developers use string literals (forward references):
```python
class Node:
    parent: 'Node'  # Compiles successfully as a string literal
```
At runtime, frameworks inspect these annotations using `inspect.get_type_hints()` or `typing.get_type_hints()`. These utilities automatically resolve string-based forward references by evaluating them within the global and local namespace of the target class or module.

---

### 12.3 CPython Compact Dictionary Design
Python 3.6 replaced CPython's legacy dictionary layout with a compact, ordered representation proposed by Raymond Hettinger.

#### 1. Legacy Dictionary Layout (Pre-3.6)
Historically, a CPython dictionary was implemented as a single, large sparse hash table. Every slot in the table contained a 24-byte `PyDictKeyEntry` structure:
```c
typedef struct {
    Py_hash_t me_hash;   /* 8 bytes */
    PyObject *me_key;    /* 8 bytes */
    PyObject *me_value;  /* 8 bytes */
} PyDictKeyEntry;
```
To maintain $O(1)$ lookups, the hash table was kept sparse with a maximum fill factor of 2/3. For a dictionary containing 8 entries, CPython had to allocate a table of at least 16 slots.
*   **Memory Overhead**: Each empty slot in the table required 24 bytes of memory. If a dictionary was large, the amount of wasted memory in unallocated slots was massive:
$$\text{Wasted Memory} = \text{Empty Slots} \times 24 \text{ bytes}$$

##### Legacy Sparse Array Layout:
```
Indices/Entries Table (Sparse):
[ Slot 0: Hash | Key* | Val* ] (24 bytes)
[ Slot 1: NULL | NULL | NULL ] (Wasted 24 bytes)
[ Slot 2: Hash | Key* | Val* ] (24 bytes)
[ Slot 3: NULL | NULL | NULL ] (Wasted 24 bytes)
[ Slot 4: Hash | Key* | Val* ] (24 bytes)
```

#### 2. The Compact Dictionary Layout
The new design splits the dictionary into two separate arrays:
1.  **`dk_indices` (Sparse Index Array)**: A small, sparse array containing integers (indices pointing into the dense array). Depending on the dictionary size, each index is stored as a 1-byte (`int8_t`), 2-byte (`int16_t`), or 4-byte (`int32_t`) integer.
2.  **`dk_entries` (Dense Entry Array)**: A dense, contiguous array containing `PyDictKeyEntry` structures (24 bytes each). Every slot in this array is fully packed in the order keys are inserted.

##### Compact Array Layout (CPython 3.6+):
```
dk_indices (Sparse Array of int8_t):
[ 0 | -1 | 1 | -1 | 2 ]  (5 bytes, where -1 represents an empty slot)

dk_entries (Dense Array of PyDictKeyEntry):
[ Slot 0: Hash | Key* | Val* ] (24 bytes) - Inserts first
[ Slot 1: Hash | Key* | Val* ] (24 bytes) - Inserts second
[ Slot 2: Hash | Key* | Val* ] (24 bytes) - Inserts third
```

##### Dynamic Lookup Walkthrough:
To look up a key (e.g. looking up `key` with hash value mapped to sparse index `2`):
1.  CPython computes the key's hash and hashes it to a slot in `dk_indices`.
2.  If `dk_indices[hash_slot]` is `1`, it retrieves the entry at `dk_entries[1]`.
3.  It compares the keys. If matched, it returns the value. If there is a collision, it continues probing within the sparse `dk_indices` array.

#### 3. Core Structural Implications
*   **Memory Savings**: Instead of wasting 24 bytes per empty slot, CPython now only wastes 1, 2, or 4 bytes per empty slot in `dk_indices`. This layout reduces dictionary memory footprints by **30% to 40%** in real-world workloads.
*   **Preservation of Insertion Order**: Because entries are appended to `dk_entries` contiguously, the elements are stored in their exact insertion order. Iteration over the dictionary simply traverses the dense `dk_entries` array sequentially.
*   **Faster Iteration**: Traversal does not require skipping empty slots, making dictionary iteration significantly faster due to CPU cache line friendliness.
*   **Language Specification Guarantee**: While insertion order was introduced as an implementation detail in Python 3.6, it was officially codified as a language specification guarantee in Python 3.7.

---

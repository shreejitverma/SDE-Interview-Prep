# Python 3.0: The Unicode Paradigm Shift and Text vs. Bytes Separation


### 7.1 The Unicode Paradigm Shift and Text vs. Bytes Separation (PEP 358 & PEP 3112)
Python 3.0 introduced a structural boundary between textual characters and binary data. In Python 2.x, `str` served double-duty as both raw bytes and text, causing silent, non-deterministic bugs when ASCII decoding failed implicitly during concatenation. Python 3.0 solved this by establishing a strict boundary between `str` (text) and `bytes`/`bytearray` (binary).

#### 1. The Python 2.x Concatenation Trap and Coercion Ban
In Python 2.x, mixed operations between `str` (raw 8-bit characters) and `unicode` (arbitrary code points) were implicitly resolved. When executing `"rsum" + u" (French)"`, CPython attempted to promote the `str` by decoding it via the default codec (usually ASCII):
```c
coerced = PyUnicode_FromEncodedObject(str_obj, "ascii", "strict");
```
If the raw string contained non-ASCII bytes (e.g., `\xe9` for Latin-1, or `\xc3\xa9` for UTF-8), this implicit decoding step raised a `UnicodeDecodeError` at runtime. Python 3.0 eliminated this by raising an unconditional `TypeError` on any implicit mixed-type operation:
```python
# Python 3.0 runtime coercion ban
>>> b"data" + "string"
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: can't concat bytes to str
```
At the C-API level, binary and text domains are kept completely disjoint. `PyUnicode_Compare` returns a failure state if one operand is a bytes object, and `PyBytes_Concat` does not accept unicode inputs.

#### 2. C-Level Representations of Binary Objects: `PyBytesObject` & `PyByteArrayObject`
CPython represents the immutable `bytes` type at the C-level as `PyBytesObject`, defined in `Include/bytesobject.h`:
```c
typedef struct {
    PyObject_VAR_HEAD
    Py_hash_t ob_shash;         /* Cached hash value of bytes; -1 if uncomputed */
    char ob_sval[1];            /* Contiguous byte vector; dynamically sized to [ob_size + 1] */
} PyBytesObject;
```
Here, `ob_sval` is a character array that is allocated inline with the rest of the object structure, terminating with a trailing `\0` to remain compatible with standard C library functions (e.g., `strlen` and `strcpy`).

The mutable sibling, `bytearray`, is represented by `PyByteArrayObject` defined in `Include/bytearrayobject.h`:
```c
typedef struct {
    PyObject_VAR_HEAD
    Py_ssize_t ob_alloc;        /* Number of bytes allocated in the buffer */
    char *ob_bytes;             /* Pointer to the start of the allocated memory block */
    char *ob_start;             /* Pointer to the start of the logical byte sequence */
    Py_ssize_t ob_exports;      /* Reference count of active buffer exports (locking) */
} PyByteArrayObject;
```
`ob_bytes` and `ob_start` are split to allow efficient head-deletion/prepends. When a user calls `del array[0]`, `ob_start` is simply incremented and `ob_size` decremented, avoiding an $O(n)$ memory shift. Memory buffer growth is managed via an overallocation growth factor:
$$\text{ob\_alloc} = \text{new\_size} + (\text{new\_size} \gg 3) + (\text{new\_size} < 9 \,?\, 3 : 6)$$
This guarantees that append operations have an amortized $O(1)$ time complexity. `ob_exports` tracks references from active `memoryview` objects; if `ob_exports > 0`, any operation that attempts to resize or reallocate the underlying `ob_bytes` buffer will raise a `BufferError`.

#### 3. Low-Level C Representation: Pre-PEP 393 `PyUnicodeObject`
Prior to the string optimization introduced in Python 3.3 (PEP 393), CPython represented Unicode strings using a uniform array of `Py_UNICODE` units. `PyUnicodeObject` was defined in `Include/unicodeobject.h` as:
```c
typedef struct {
    PyObject_HEAD
    Py_ssize_t length;          /* Number of code points */
    Py_UNICODE *str;            /* Pointer to the character array */
    Py_hash_t hash;             /* Cached hash value; -1 if uncomputed */
    PyObject *defenc;           /* Cached UTF-8 encoded bytes representation */
} PyUnicodeObject;
```
The representation type `Py_UNICODE` was defined at compile-time as:
* **UCS-2 (Narrow Build)**: Compiled with `wchar_t` as a 16-bit type (size 2 bytes). Characters beyond `U+FFFF` (e.g., emojis) had to be represented as surrogate pairs, causing index and len calculations to mismatch.
* **UCS-4 (Wide Build)**: Compiled with `wchar_t` as a 32-bit type (size 4 bytes). Every Unicode code point mapped to exactly one array index, but ASCII-only strings consumed 4 times the memory they required.

#### 4. PEP 383: The `surrogateescape` Error Handler Mechanics
To allow POSIX filesystems (which use arbitrary null-terminated byte sequences for paths) to round-trip pathnames containing invalid UTF-8 bytes without throwing exceptions, PEP 383 introduced the `surrogateescape` error handler.
During decoding, any invalid byte (which does not form a valid UTF-8 sequence) is mapped to a high-surrogate code point in the range `U+DC80` to `U+DCFF` via:
$$\text{code\_point} = 0\text{xDC00} + \text{byte\_value}$$
During encoding, these specific surrogate code points are mapped back to their original raw bytes:
$$\text{byte\_value} = \text{code\_point} - 0\text{xDC00}$$
This allows arbitrary binary data to be round-tripped through Unicode `str` representations:
```python
# Raw undecodable path round-trip simulation
raw_bytes = b"bad_\xff_path.txt"
decoded_str = raw_bytes.decode("utf-8", "surrogateescape")
# decoded_str contains code point U+DCFF where \xff was.
reencoded_bytes = decoded_str.encode("utf-8", "surrogateescape")
assert reencoded_bytes == raw_bytes
```
The CPython implementation in `Objects/unicodeobject.c` handles this matching logic:
```c
/* Pseudocode of surrogateescape decoding branch */
if (status == INVALID_BYTE) {
    *unicode_ptr++ = 0xDC00 + (unsigned char)input_byte;
}
```

---

### 7.2 The `print()` Function Redesign

#### 1. Bytecode Comparison
In Python 2.x, `print` was a core language statement with specialized bytecodes. In Python 3.0, it was unified into a standard built-in function.

##### Python 2.7 compiler translation for `print "hello"`:
```
1           0 LOAD_CONST               0 ('hello')
            3 PRINT_ITEM
            4 PRINT_NEWLINE
```
The VM execution loop (`ceval.c`) maps `PRINT_ITEM` directly to standard output stream writing operations, meaning the behavior was fixed at compilation.

##### Python 3.0 compiler translation for `print("hello")`:
```
1           0 LOAD_GLOBAL              0 (print)
            3 LOAD_CONST               1 ('hello')
            6 CALL_FUNCTION            1
```
The interpreter dynamically resolves `print` at runtime via standard namespace lookups. This enables runtime overriding:
```python
import builtins
def custom_print(*args, **kwargs):
    builtins.print("[LOG]", *args, **kwargs)
builtins.print = custom_print
```

#### 2. C-Level stream writing & TextIOWrapper
The C-level entrypoint for the `print()` function is `builtin_print` inside `Python/bltinmodule.c`:
```c
static PyObject *
builtin_print(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"sep", "end", "file", "flush", 0};
    PyObject *sep = Py_None, *end = Py_None, *file = Py_None;
    int flush = 0;
    // Parsing keywords using PyArg_ParseTupleAndKeywords...
    ...
}
```
* **Stream Resolution**: If `file` is `Py_None` or omitted, the function queries `sys.stdout` via `PySys_GetObject("stdout")`.
* **String Conversions**: For each positional argument, `builtin_print` calls `PyObject_Str(arg)` to compute its string representation.
* **Buffer Flushing**: After calling `file.write()`, if `flush=True` is set, CPython attempts to invoke the `flush()` method of the `file` object. If `sys.stdout` wraps a standard output descriptor (stdout), this triggers `fflush(stdout)` in the underlying C library, executing a synchronous write syscall.

---

### 7.3 Division Unification (PEP 238)

#### 1. The Division Operators and Bytecodes
Python 2.x's division operator `/` mapped to the `BINARY_DIVIDE` bytecode, which executed floor division if both operands were integers, and true division if either was a float. This led to silent precision loss bugs.
Python 3.0 unified this by assigning `/` to true division and introducing `//` for floor division:
* **True Division (`/`)** maps to the `BINARY_TRUE_DIVIDE` bytecode, which always delegates to the `nb_true_divide` slot.
* **Floor Division (`//`)** maps to the `BINARY_FLOOR_DIVIDE` bytecode, delegating to the `nb_floor_divide` slot.

#### 2. C-Level Integer Division Mechanics
CPython defines number methods in the `PyNumberMethods` struct inside `Include/object.h`:
```c
typedef struct {
    binaryfunc nb_add;
    binaryfunc nb_subtract;
    ...
    binaryfunc nb_floor_divide;
    binaryfunc nb_true_divide;
} PyNumberMethods;
```
For integer types, these slots point to C functions in `Objects/longobject.c`:
* `nb_floor_divide` points to `long_div`. It computes the integer quotient and rounds towards negative infinity.
* `nb_true_divide` points to `long_true_divide`.

The execution path of `long_true_divide(x, y)`:
1. It extracts the size of integers $x$ and $y$. If they fit within C double precision (53 bits of mantissa), it converts them to double:
   $$x_{\text{double}} = (\text{double})x; \quad y_{\text{double}} = (\text{double})y;$$
2. If the integer values exceed double limits, CPython runs an arbitrary-precision float conversion algorithm (`_PyLong_Format` or manual bit shift scaling) to extract the most significant 53 bits.
3. It performs the float division and instantiates a new `PyFloatObject` to hold the output:
   $$\text{result} = x_{\text{double}} / y_{\text{double}}$$
4. If $y = 0$, it raises a `ZeroDivisionError` via `PyErr_SetString`.

---

### 7.4 Lazy Iterators & Dictionary View Objects

#### 1. Lazy Conversions & `rangeobject` Memory Layout
Python 3.0 replaced list-producing functions with lazy iterators. The `range()` built-in returns a `rangeobject` defined in `Objects/rangeobject.c`:
```c
typedef struct {
    PyObject_HEAD
    PyObject *start;
    PyObject *stop;
    PyObject *step;
    PyObject *length;
} rangeobject;
```
Because the sequence elements are not pre-allocated, a `rangeobject` consumes $O(1)$ memory. To compute the value at a specific index $i$, the type's sequence method (`range_item`) performs a direct arithmetic step calculation:
$$\text{value} = \text{start} + i \times \text{step}$$
This math enables $O(1)$ index access and $O(1)$ containment checks for numeric targets:
$$\text{remainder} = (\text{target} - \text{start}) \pmod{\text{step}}$$
If $\text{remainder} == 0$ and $\text{start} \le \text{target} < \text{stop}$ (or reverse for negative steps), the value is in the range.

#### 2. Dictionary View Objects
Methods like `keys()`, `values()`, and `items()` return dynamic views containing a direct reference back to the parent dictionary, instead of copying elements into a new list. The underlying struct is `PyDictViewObject`:
```c
typedef struct {
    PyObject_HEAD
    PyDictObject *dv_dict;      /* Pointer to parent dictionary struct */
} PyDictViewObject;
```
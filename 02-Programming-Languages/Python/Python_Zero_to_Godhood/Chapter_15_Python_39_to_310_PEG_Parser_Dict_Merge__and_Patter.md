# Python 3.9 to 3.10: PEG Parser, Dict Merge (|), and Pattern Matching


### 15.1 CPython Parser Evolution: LL(1) to PEG (PEP 617)
In Python 3.9, CPython replaced its concrete parser generator (`pgen`), which had been used since Python 1.0, with a modern **PEG (Parsing Expression Grammar)** parser.

#### 1. Limitations of the Classic LL(1) Parser
The legacy compiler utilized an LL(1) (Left-to-right, Leftmost derivation with 1-token lookahead) parsing grammar:
*   **Lookahead Constraints**: With only one token of lookahead, the parser could not resolve ambiguities between grammar rules that looked similar initially.
*   **Left-Recursion Prohibitions**: LL(1) cannot parse left-recursive rules (e.g., $A \to A \alpha$), requiring complex grammar rewrites that made grammar maintenance difficult.
*   **Grammar Hacks**: To support complex syntax, developers had to implement custom AST-level hacks and validation passes after parsing, which bloated parser code.

#### 2. The PEG Parser Architecture
PEG parsers resolve these limitations through two core features:
*   **Ordered Choice (`e1 / e2`)**: Unlike CFGs (Context-Free Grammars) where choice is ambiguous, a PEG ordered choice evaluates `e1` first. If `e1` matches, the parser commits to it and completely ignores `e2`, resolving parsing ambiguities deterministically.
*   **Infinite Lookahead via Packrat Parsing**: PEG can look ahead arbitrarily far. To maintain linear time complexity $O(N)$ (where $N$ is input length), the parser utilizes **Packrat Parsing** (memoization). It caches parser rules evaluation results at every character index offset in a memoization table (`ParserState` in `Parser/pegen.c`).

#### 3. Enabling Syntax Improvements
The infinite lookahead and recursive capabilities of PEG enabled new syntax structures in Python 3.10, such as **parenthesized context managers**:
```python
with (
    CtxManager1() as ctx1,
    CtxManager2() as ctx2,
):
    pass
```
Under the old LL(1) parser, parsing this construct was impossible because it could not distinguish between a parenthesized tuple and a grouped context manager block without infinite lookahead.

---

### 15.2 PEP 584: Dictionary Union Operators (`|` and `|=`)
Python 3.9 introduced the binary operators `|` (merge) and `|=` (update) directly on the `dict` class.

#### 1. Legacy Workarounds
Prior to Python 3.9, merging dictionaries required verbose and slow operations:
*   `merged = {**d1, **d2}`: Highly performant but syntactically verbose and unreadable for larger expressions.
*   `merged = d1.copy(); merged.update(d2)`: Required multiple lines of statements, making it impossible to merge inline inside list comprehensions or lambda bodies.

#### 2. Low-Level C-Level Slot Mappings
CPython implements `|` and `|=` by overloading the numeric bitwise OR slots inside `PyDict_Type` (`Objects/dictobject.c`):
```c
/* Dict Type definition slot mappings */
PyTypeObject PyDict_Type = {
    /* ... */
    &dict_as_number,            /* tp_as_number */
    /* ... */
};

static PyNumberMethods dict_as_number = {
    .nb_or = (binaryfunc)dict_or,
    .nb_inplace_or = (binaryfunc)dict_inplace_or,
};
```

When evaluating `A | B`:
1.  CPython calls `dict_or(A, B)`.
2.  The C function allocates a new dictionary object: `PyObject *new_dict = PyDict_Copy(A);`.
3.  It then calls the dictionary update routine `PyDict_Update(new_dict, B);` to merge the elements from the second mapping.
4.  It returns the new dictionary. Right-hand elements overwrite duplicate keys present in the left-hand dictionary.

#### 3. Type Constraints and Subclassing
*   **Return Type**: `dict_or` always returns a standard `dict` instance, even if `A` or `B` is a subclass of `dict`, preserving typing boundaries.
*   **Operand Support**: The left-hand operand `A` must be a `dict` (or subclass). The right-hand operand `B` can be any object that implements the mapping protocol (exposes a `.keys()` and key retrieval interface). If `B` is not a mapping, the C-level function returns `Py_NotImplemented`, triggering a runtime `TypeError`.

---

### 15.3 PEP 634: Structural Pattern Matching (Decision Trees)
Python 3.10 introduced Structural Pattern Matching (`match`/`case`).

#### 1. Decision DAGs vs. Sequential Linear Scans
Unlike chains of `if/elif/else` statements, which execute linearly ($\mathcal{O}(N)$ lookup time), the CPython compiler compiles a `match` statement into a **Directed Acyclic Graph (DAG) decision tree**. 
The compiler groups cases matching the same pattern categories, checking target type boundaries and structural lengths once rather than executing redundant checks.

#### 2. CPython Pattern Matching Bytecodes
CPython implements pattern matching using specialized stack-manipulation bytecodes:
*   **`MATCH_SEQUENCE`**: Checks if the object on the stack is a sequence (excluding `str`, `bytes`, and `bytearray`).
*   **`MATCH_MAPPING`**: Checks if the object is an instance of `collections.abc.Mapping`.
*   **`MATCH_KEYS`**: Pops a tuple of keys and a subject mapping. If all keys exist in the mapping, it pushes a tuple containing their values; otherwise, it pushes `None`.
*   **`MATCH_CLASS`**: Evaluates class-level matches.

#### 3. Class Patterns and `MATCH_CLASS` Disassembly Trace
To match a class pattern:
```python
class Point:
    __match_args__ = ('x', 'y')
    def __init__(self, x, y):
        self.x = x
        self.y = y

def check_point(pt):
    match pt:
        case Point(1, y):
            return y
```

During execution, `case Point(1, y)` requires verifying that `pt` is an instance of `Point`, matching the first positional argument `x` to `1`, and binding the second argument `y` to local scope.

##### Disassembly of `check_point`:
```
  8           0 LOAD_FAST                0 (pt)
              2 LOAD_GLOBAL              0 (Point)
              4 MATCH_CLASS              1              /* Match Point class with 1 positional arg */
              6 DUP_TOP
              8 LOAD_CONST               0 (None)
             10 COMPARE_OP               9 (is not)
             12 POP_JUMP_IF_FALSE       32 (to case mismatch)
             14 UNPACK_SEQUENCE          2              /* Unpack matched x and y values */
             16 LOAD_CONST               1 (1)
             18 COMPARE_OP               2 (==)         /* Check if x == 1 */
             20 POP_JUMP_IF_FALSE       28 (to case clean stack)
             22 STORE_FAST               1 (y)          /* Bind y to local scope */
             24 LOAD_FAST                1 (y)
             26 RETURN_VALUE

        >>   28 POP_TOP                                 /* Clean remaining unpacked values */
        >>   30 POP_TOP
        >>   32 POP_TOP                                 /* Fallback case mismatch target */
             34 LOAD_CONST               0 (None)
             36 RETURN_VALUE
```

##### C-Level Execution of `MATCH_CLASS`:
1.  **Type Validation**: Checks `isinstance(pt, Point)`. If false, pushes `None` and exits.
2.  **Positional Resolution**: Reads the class's `__match_args__` tuple (`('x', 'y')`). The compiler indicated `1` positional argument, mapping the first argument to attribute `x`.
3.  **Keyword/Positional Extraction**: Extracts `pt.x` and `pt.y` via C-level attribute lookups.
4.  **Stack Result**: Pushes a tuple `(pt.x, pt.y)` onto the stack. If any attribute extraction fails, it pushes `None`.
5.  **Fail-Fast Stack Cleanup**: If `None` is pushed, `COMPARE_OP` determines a mismatch, jumps to the fallback, and pops the stack, leaving no bindings.

---

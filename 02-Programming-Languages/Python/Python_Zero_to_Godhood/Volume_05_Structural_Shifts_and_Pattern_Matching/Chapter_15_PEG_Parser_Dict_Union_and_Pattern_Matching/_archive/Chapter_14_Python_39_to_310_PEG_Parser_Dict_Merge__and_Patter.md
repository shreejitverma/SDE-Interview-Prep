# Python 3.9 to 3.10: PEG Parser, Dict Merge (|), and Pattern Matching


### 15.1 PEP 634 Decision Trees vs. Linear Scans
Introduced in Python 3.10, Structural Pattern Matching (`match`/`case`) provides a powerful declarative pattern engine. Unlike a chain of `if/elif/else` statements, which executes sequentially ($\mathcal{O}(N)$ lookup runtime complexity), the CPython compiler translates a `match` statement into a directed decision tree (DAG). The compiler groups checks by target type and attributes to avoid redundant attribute extraction and evaluation.

---

### 15.2 Pattern Matching Bytecodes in CPython
CPython implements pattern matching using a suite of dedicated bytecode instructions that interact with the value stack.

#### 1. `MATCH_SEQUENCE`
This instruction checks whether the object on the top of the stack is a sequence.
*   **Implementation**: It checks if the object is an instance of `collections.abc.Sequence`. It explicitly excludes `str`, `bytes`, and `bytearray` to prevent unexpected character matching behavior.
*   **Execution**:
    *   Input: `subject` on stack.
    *   Output: `subject` and a boolean `(True/False)` indicating whether it is a sequence.

#### 2. `MATCH_MAPPING`
Similar to `MATCH_SEQUENCE`, this instruction checks whether the object on the top of the stack is an instance of `collections.abc.Mapping`.

#### 3. `MATCH_KEYS`
Used in mapping patterns to match key-value pairs.
*   **Execution**:
    *   Input: `subject` and a tuple of expected `keys` on stack.
    *   Operation: Resolves whether all keys exist in the mapping.
    *   Output: If keys exist, it pushes a tuple of corresponding values; otherwise, it pushes `None`.

#### 4. `MATCH_CLASS`
Validates class types and positional/keyword attribute mappings.
*   **Execution**:
    *   Input: `subject`, `class_object`, a tuple of attribute names, and a count of positional attributes.
    *   Operation: The runtime checks `isinstance(subject, class_object)`. If true, it extracts attributes based on positional mappings defined by the class's `__match_args__` attribute, followed by named keyword lookups.
    *   Output: A tuple of extracted attribute values if the match succeeds, or `None` on failure.

#### 5. AST/Bytecode Translation Trace
Consider a pattern match on a list:

```python
match data:
    case [1, y]:
        pass
```

The compiled bytecode sequence matches the pattern via:
```
  2 LOAD_FAST                0 (data)
  4 MATCH_SEQUENCE
  6 POP_JUMP_IF_FALSE       32 (to case mismatch fallback)
  8 GET_LEN
 10 LOAD_CONST               0 (2)
 12 COMPARE_OP               2 (==)
 14 POP_JUMP_IF_FALSE       32 (to case mismatch fallback)
 16 UNPACK_SEQUENCE          2
 18 LOAD_CONST               1 (1)
 20 COMPARE_OP               2 (==)
 22 POP_JUMP_IF_FALSE       32 (to next case or fallback)
 24 STORE_FAST               1 (y)
```

This stack-level execution ensures that subpatterns (like length check and index-0 constant equality) fail fast, avoiding binding variables unless the matching tree path succeeds.

---

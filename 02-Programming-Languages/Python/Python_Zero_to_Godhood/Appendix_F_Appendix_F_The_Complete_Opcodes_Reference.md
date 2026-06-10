# Appendix F: The Complete Opcodes Reference


This table provides a reference for the most common opcodes in the CPython 3.13 Virtual Machine.

| Opcode | Args | Description |
| :--- | :--- | :--- |
| `CACHE` | 0 | Specialized inline cache entry (skipped by interpreter). |
| `RESUME` | 0 | Internal entry point for functions/generators. |
| `LOAD_CONST` | const_idx | Pushes `co_consts[const_idx]` onto the stack. |
| `LOAD_FAST` | var_num | Pushes local variable `var_num` onto the stack. |
| `STORE_FAST` | var_num | Pops TOS and stores it in local variable `var_num`. |
| `LOAD_GLOBAL` | name_idx | Pushes `co_names[name_idx]` from global/builtin namespace. |
| `BINARY_OP` | op_id | Pops two items, performs operation `op_id` (e.g., ADD, SUB). |
| `BUILD_LIST` | count | Pops `count` items and pushes a new list. |
| `CALL` | argc | Calls a function with `argc` arguments. |
| `COMPARE_OP` | op_id | Performs comparison (e.g., `==`, `<`). |
| `JUMP_FORWARD` | delta | Increments instruction pointer by `delta`. |
| `POP_JUMP_IF_FALSE` | target | Pops TOS; if false, jumps to `target`. |
| `RETURN_VALUE` | 0 | Returns TOS to the caller. |
| `YIELD_VALUE` | 0 | Yields TOS from a generator. |

---

**This concludes the technical reference.**

---

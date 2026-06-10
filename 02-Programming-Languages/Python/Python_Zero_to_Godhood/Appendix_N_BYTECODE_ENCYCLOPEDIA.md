# Appendix N: The Python Bytecode Encyclopedia

This appendix provides an exhaustive reference for the CPython 3.13 instruction set. For each opcode, we provide its numerical value, its stack transition, and a technical description of its C-level implementation.

### N.1 Stack Notation
*   `TOS`: Top of Stack.
*   `TOS1`: Second item on stack.
*   `TOS2`: Third item on stack.
*   `NULL`: A sentinel value.

### N.2 Data Movement Opcodes

| Opcode | Transition | Description |
| :--- | :--- | :--- |
| `LOAD_CONST` | `( -> const)` | Pushes a constant from the code object's `co_consts` tuple onto the stack. |
| `LOAD_FAST` | `( -> val)` | Pushes a local variable onto the stack. Extremely fast as it uses a simple array index in the frame object. |
| `STORE_FAST` | `(val -> )` | Pops the top of the stack and stores it in a local variable. |
| `LOAD_GLOBAL` | `( -> val)` | Pushes a global or builtin name onto the stack. Uses the `co_names` tuple and performs a hash table lookup. |
| `STORE_GLOBAL` | `(val -> )` | Stores the top of the stack in the global namespace. |
| `DELETE_GLOBAL`| `( -> )` | Deletes a global name. |

### N.3 Arithmetic and Bitwise Opcodes

| Opcode | Transition | Description |
| :--- | :--- | :--- |
| `BINARY_OP` | `(TOS1, TOS -> result)` | General opcode for binary operations (ADD, SUB, MUL, etc.). In 3.11+, this is specialized for types (e.g., `BINARY_OP_ADD_INT`). |
| `UNARY_NEGATIVE`| `(TOS -> -TOS)` | Negates the top of the stack. |
| `UNARY_NOT` | `(TOS -> not TOS)`| Performs logical NOT. |
| `UNARY_INVERT` | `(TOS -> ~TOS)` | Performs bitwise inversion. |

### N.4 Collection Opcodes

| Opcode | Transition | Description |
| :--- | :--- | :--- |
| `BUILD_LIST` | `(TOSn, ... -> list)` | Creates a new list from the top $n$ items on the stack. |
| `BUILD_TUPLE` | `(TOSn, ... -> tuple)` | Creates a new tuple. |
| `BUILD_SET` | `(TOSn, ... -> set)` | Creates a new set. |
| `BUILD_MAP` | `(TOS2n, ... -> dict)` | Creates a new dictionary from $n$ key-value pairs. |
| `LIST_APPEND` | `(list, val -> list)` | Appends a value to a list (used in comprehensions). |
| `MAP_ADD` | `(dict, key, val -> dict)` | Adds a key-value pair to a dict. |

### N.5 Control Flow and Function Calls

| Opcode | Transition | Description |
| :--- | :--- | :--- |
| `JUMP_FORWARD` | `( -> )` | Increments the instruction pointer. |
| `POP_JUMP_IF_FALSE` | `(bool -> )` | Jumps if the top of the stack is false. |
| `CALL` | `(func, args -> result)`| Calls a function. In 3.11+, this is the "mega-opcode" that handles all function calls, including those with keyword arguments. |
| `RETURN_VALUE` | `(val -> )` | Returns the top of the stack to the caller's frame. |
| `RAISE_VARARGS` | `(val -> )` | Raises an exception. |

### N.6 Specialized and Modern Opcodes

| Opcode | Transition | Description |
| :--- | :--- | :--- |
| `RESUME` | `( -> )` | A no-op at runtime, but serves as an entry point for tracing and generator resumption. |
| `SEND` | `(gen, val -> res)` | Sends a value into a generator or coroutine. |
| `COPY_FREE_VARS` | `( -> )` | Copies free variables from the closure into the new frame (used in nested functions). |

---
**This encyclopedia serves as the definitive reference for the Python Virtual Machine's internal language.**
---

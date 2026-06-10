# The Disassembler (`dis`)


To reach "Godhood," you must be able to read the machine code of the Python Virtual Machine: **Bytecode**.

### 62.1 The Python VM: A Stack Machine

The CPython VM is a **Stack Machine**. Operations push values onto a stack and pop them to perform calculations.

#### 1. Dissecting an Operation
```python
def add(a, b):
    return a + b

import dis
dis.dis(add)
```
*Output:*
```
  2           0 LOAD_FAST                0 (a)
              2 LOAD_FAST                1 (b)
              4 BINARY_ADD
              6 RETURN_VALUE
```
*   **`LOAD_FAST`**: Pushes the value of a local variable onto the stack.
*   **`BINARY_ADD`**: Pops the top two values, adds them (using the `tp_as_number->nb_add` C slot), and pushes the result back.
*   **`RETURN_VALUE`**: Pops the top value and returns it to the caller.

### 62.2 Bytecode Specialization (Python 3.11+)

In modern Python, you may see `RESUME` or "Specialized" opcodes like `BINARY_OP_ADD_INT`.
*   **Inline Caching**: If the VM sees that a specific `BINARY_ADD` is always adding two integers, it replaces the generic opcode with a specialized version that skips the type-checking overhead, resulting in significant speedups (as discussed in Chapter 17).

---



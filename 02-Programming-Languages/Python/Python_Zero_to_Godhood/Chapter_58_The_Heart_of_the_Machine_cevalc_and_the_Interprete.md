# The Heart of the Machine: `ceval.c` and the Interpreter Loop


To understand Python execution is to understand the main evaluation loop. In CPython, this resides in `Python/ceval.c`, specifically in the function `_PyEval_EvalFrameDefault`.

### 69.1 The Mega-Switch Statement

Historically, the Python interpreter loop was a giant C `switch` statement inside a `while` loop.
```c
for (;;) {
    opcode = NEXTOPARG();
    switch (opcode) {
        case TARGET(LOAD_FAST):
            // ... load local variable ...
            FAST_DISPATCH();
        case TARGET(BINARY_ADD):
            // ... add two objects ...
            FAST_DISPATCH();
    }
}
```

#### 1. Computed Gotos
On compilers that support it (like GCC and Clang), CPython uses "Computed Gotos." Instead of a switch statement (which requires a jump table lookup and a bounds check for every instruction), it uses a table of memory addresses. At the end of each opcode's C code, it jumps directly to the address of the next opcode. This reduces CPU branch mispredictions and significantly improves performance.

### 69.2 The Evaluation Stack

Python is a stack-based VM.
*   **The Stack Pointer**: `stack_pointer` in C.
*   **PUSH/POP**: These are simple pointer increments/decrements in C.
*   **Value Stack**: An array of `PyObject *`. When you add two numbers, the pointers to the numbers are popped, the addition is performed, and the pointer to the new result object is pushed.

### 69.3 Handling Interrupts and the GIL

The interpreter loop is not just for math; it is the system's heartbeat.
*   **Signal Checking**: Every $N$ instructions (the "check interval"), the loop checks if a signal has arrived from the OS.
*   **Thread Switching**: This is also where the GIL is released and re-acquired, allowing other threads to take their turn in the VM.

---

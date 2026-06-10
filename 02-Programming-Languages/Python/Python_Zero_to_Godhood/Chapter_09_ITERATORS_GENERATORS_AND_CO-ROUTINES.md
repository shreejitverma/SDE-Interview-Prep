# ITERATORS, GENERATORS, AND CO-ROUTINES


### 9.1 The Iterator Protocol at the Bytecode Level
An iterator is defined by implementing `__iter__()` (which returns the iterator object itself) and `__next__()` (which returns the next value or raises `StopIteration`).
*   **Bytecode Compilation**: The compiler optimizes loops targeting iterators by emitting specialized bytecodes:
    - `GET_ITER`: Pops the iterable from the stack, calls its `__iter__()`, and pushes the resulting iterator back onto the stack.
    - `FOR_ITER`: Calls `__next__()` on the iterator. If a value is returned, it is pushed onto the stack and execution jumps to the loop body. If `StopIteration` is caught, the exception is cleared, and the instruction pointer jumps past the loop.

### 9.2 Generator Execution Frame Suspension
Standard function calls allocate a stack frame, execute, and release the frame. Generator functions behave differently:
*   **Heap Preservation**: When a function containing the `yield` keyword is called, it returns a generator object wrapping a `PyFrameObject`.
*   **Yield Mechanism**: When `yield` is evaluated, the interpreter saves the current state (evaluation stack values and the next instruction pointer `f_lasti` in the frame) and returns the yielded value. The frame is **not** popped from the call stack; it remains allocated on the heap.
*   **Next Mechanism**: When `next()` is called again on the generator, the CPython evaluation loop loads the suspended frame, restores its registers and evaluation stack, and continues execution from the saved `f_lasti`.

```
Generator Frame Suspension Flow:
[Call generator] -> [Allocates PyFrameObject on Heap]
                           |
                           v
[Run bytecode] ------> [Hits YIELD]
   ^                       |
   |                       v
   |                  [Save frame state (f_lasti)] -> [Return value to caller]
   |                       |
   +---[Call next()]-------+
```

### 9.3 Generator Communication Protocol
Generators support bidirectional data exchange via the methods:
*   `send(value)`: Resumes execution and passes `value` back into the generator as the result of the `yield` expression.
*   `throw(type, value)`: Raises an exception at the generator's current execution point. The generator can catch this exception or propagate it.
*   `close()`: Raises a `GeneratorExit` exception in the generator to run cleanup blocks (e.g. `try/finally` resource releases) and terminates the generator.

```python
# Bidirectional Communication Trace
def bidirectional_generator():
    try:
        val = yield "Ready"
        print("Received value inside generator:", val)
        yield f"Echo: {val}"
    except ValueError:
        print("Caught ValueError in generator")
        yield "Recovered"

gen = bidirectional_generator()
print(next(gen)) # Start generator (yields "Ready")
print(gen.send("Hello World")) # Sends "Hello World", yields "Echo: Hello World"

gen_err = bidirectional_generator()
next(gen_err)
print(gen_err.throw(ValueError)) # Raises error inside generator, yields "Recovered"
```

### 9.4 PEP 380: Generator Delegation via yield from
Introduced in Python 3.3, `yield from <iterable>` delegates operations to another generator (the sub-generator):
*   **Transparent Channel**: It establishes a direct, bidirectional channel between the outer caller and the sub-generator.
*   **Automated Propagation**: 
    - Values sent via `send()` are routed directly to the sub-generator.
    - Exceptions thrown via `throw()` are raised inside the sub-generator.
    - `StopIteration` raised by the sub-generator is caught automatically, and its `value` attribute becomes the result of the `yield from` expression.

---

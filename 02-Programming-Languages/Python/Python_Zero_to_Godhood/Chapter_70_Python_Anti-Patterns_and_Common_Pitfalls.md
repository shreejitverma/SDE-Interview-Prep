# Python Anti-Patterns and Common Pitfalls


A "Godhood" level engineer is defined by the bugs they *don't* write. This chapter deconstructs the most common mistakes in the Python ecosystem.

### 93.1 Mutable Default Arguments
```python
def append_to(element, to=[]): # DANGER!
    to.append(element)
    return to
```
*   **The Trap**: The default list `[]` is created once at **definition time**, not call time. Every call shares the same list.
*   **The Fix**: Use `to=None` and initialize inside the function.

### 93.2 Late Binding in Closures
```python
def create_multipliers():
    return [lambda x: i * x for i in range(5)] # DANGER!
```
*   **The Trap**: The lambda captures the variable `i`, not its value. When the lambdas are called, they all see the final value of `i` (4).
*   **The Fix**: Use default arguments to capture the value: `lambda x, i=i: i * x`.

### 93.3 The `is` vs. `==` Confusion
*   **`==` (Equality)**: Calls `__eq__`, checks if values are the same.
*   **`is` (Identity)**: Checks if the memory addresses (`id()`) are the same.
*   **Interning Pitfall**: Python interns small integers (-5 to 256) and short strings. `x = 10; y = 10; x is y` might be True, but `x = 1000; y = 1000; x is y` is usually False. **Never use `is` for value comparison.**

---

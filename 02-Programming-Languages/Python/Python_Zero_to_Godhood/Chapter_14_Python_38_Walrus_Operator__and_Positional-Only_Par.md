# Python 3.8: Walrus Operator (:=) and Positional-Only Parameters (/)


### 14.1 The Assignment Expression (`:=` / PEP 572)
Introduced in Python 3.8, the assignment expression operator (colloquially called the **walrus operator**) allows assigning values to variables inside expressions. This departs from Python's historical strict division between statements and expressions.

#### 1. AST Representation
A standard assignment is a statement represented by the `Assign` node in the Abstract Syntax Tree (AST), which does not return a value. The walrus operator compiles to a `NamedExpr` node, which is an expression node returning the bound value.
```
      [Assign Statement]                  [NamedExpr Expression]
         /         \                            /         \
   Name(x)      Constant(1)               Name(x)      Constant(1)
   (Returns void/no value)              (Returns 1 to parent context)
```

#### 2. Bytecode & Stack Operations
Let's examine how CPython processes a normal assignment vs. an assignment expression.
Consider the following source codes and their corresponding bytecodes:

```python
# Standard Assignment
x = 1
```

```
2 LOAD_CONST               0 (1)
4 STORE_FAST               0 (x)
```

```python
# Assignment Expression
(x := 1)
```

##### CPython 3.8 Bytecode:
```
2 LOAD_CONST               0 (1)
4 DUP_TOP
6 STORE_FAST               0 (x)
```

##### CPython 3.11+ Optimized Bytecode:
```
2 LOAD_CONST               0 (1)
4 COPY                     1
6 STORE_FAST               0 (x)
```

The execution steps for the walrus operator stack operation are:
1.  **`LOAD_CONST`**: Pushes the reference to the constant integer `1` onto the value stack.
2.  **`DUP_TOP` / `COPY 1`**: Replicates the top element of the stack, resulting in two references to `1` on the stack.
3.  **`STORE_FAST`**: Pops one of the references and binds it to the local variable name `x`.
4.  **Stack Residual**: The remaining reference to `1` remains on the top of the stack, allowing parent expressions (such as `if` conditionals or loops) to consume it.

#### 3. Scoping Rules and Symbol Table Traversal
To prevent variable leakage and namespace pollution, PEP 572 specifies complex scoping rules, especially within comprehensions:
*   **Comprehensions**: Python list, set, and dict comprehensions, as well as generator expressions, are executed in a nested function scope.
*   **Variable Binding (Hoisting)**: An assignment expression `x := ...` inside a comprehension binds the variable `x` in the *surrounding* (parent) scope, not the local comprehension scope.
*   **Symbol Table Resolution**: During compilation, the symbol table builder (`symtable.c`) traverses variables inside comprehensions. When it encounters a `NamedExpr`, it marks the target variable as a **free variable** in the comprehension scope and a **cell variable** in the parent scope. This hoists the reference out of the comprehension's inner namespace.

Let's examine this hoisting in action. Consider the following code:
```python
def parent_func(data):
    result = [y for x in data if (y := x * 2) > 2]
    return result, y
```

##### Disassembly of `parent_func`:
```
  2           0 LOAD_CLOSURE             0 (y)          /* Load the cell reference for y */
              2 BUILD_TUPLE              1
              4 LOAD_CONST               1 (<code object <listcomp>...>)
              6 LOAD_CONST               2 ('parent_func.<locals>.<listcomp>')
              8 MAKE_FUNCTION            8 (closure)    /* Bind cell to function closure */
             10 LOAD_FAST                0 (data)
             12 GET_ITER
             14 CALL_FUNCTION            1
             16 STORE_FAST               1 (result)

  3          18 LOAD_FAST                1 (result)
             20 LOAD_DEREF               0 (y)          /* Load y from closure cell */
             22 BUILD_TUPLE              2
             24 RETURN_VALUE
```

##### Disassembly of the nested `<listcomp>` code object:
```
  2           0 BUILD_LIST               0
              2 LOAD_FAST                0 (.0)         /* Load incoming iterator */
        >>    4 FOR_ITER                26 (to 32)
              6 STORE_FAST               1 (x)
              8 LOAD_FAST                1 (x)
             10 LOAD_CONST               1 (2)
             12 BINARY_MULTIPLY
             14 DUP_TOP
             16 STORE_DEREF              0 (y)          /* Store into outer scope cell */
             18 LOAD_CONST               2 (2)
             20 COMPARE_OP               4 (>)
             22 POP_JUMP_IF_FALSE        4
             24 LOAD_DEREF               0 (y)          /* Load y from outer cell */
             26 LIST_APPEND              2
             28 JUMP_ABSOLUTE            4
        >>   32 RETURN_VALUE
```
By using `STORE_DEREF` and `LOAD_DEREF`, the nested list comprehension writes directly to the parent scope cell, allowing `y` to survive after the list comprehension has finished executing.

#### 4. Compiler Restrictions
To prevent syntactically ambiguous or unstable code, the compiler enforces strict boundaries:
*   **Iteration Variable Conflicts**: A target variable cannot share a name with a comprehension iteration variable. E.g. `[i for i in range(10) if (i := 2)]` raises `SyntaxError: assignment expression cannot rebind comprehension iteration variable`.
*   **Class Scope Prohibitions**: The walrus operator is explicitly blocked inside comprehensions in class bodies:
    ```python
    class MyClass:
        data = [1, 2, 3]
        result = [y for x in data if (y := x * 2) > 2]
    ```
    This raises `SyntaxError: assignment expression within a comprehension cannot be used in a class body`.
    *Why?* Class namespaces are evaluated as temporary dict namespaces, not standard function frames. They do not support closure cells (`PyCellObject`). Since the comprehension runs as a separate nested helper function, it cannot bind closures to class body locals, creating a scoping mismatch.

---

### 14.2 Positional-Only Parameters (`/` / PEP 570)
Python 3.8 introduced the `/` marker to define positional-only parameters. Any parameters declared before the `/` marker cannot be passed as keyword arguments.

#### 1. C-level Representation in `PyCodeObject`
The compiler encodes parameter boundaries directly into the function's bytecode descriptor struct. In `code.h`, the `PyCodeObject` structure contains specific integer fields to track positional boundaries:

```c
typedef struct {
    PyObject_HEAD
    int co_argcount;            /* Number of positional and positional-only arguments */
    int co_posonlyargcount;     /* Number of positional-only arguments */
    int co_kwonlyargcount;      /* Number of keyword-only arguments */
    /* ... additional fields ... */
} PyCodeObject;
```

For a function definition:
```python
def func(a, b, /, c, d, *, e, f):
    pass
```
The compiler maps the arguments as follows:
*   `a, b`: Positional-only. (`co_posonlyargcount = 2`)
*   `c, d`: Positional-or-keyword. (Total positional `co_argcount = 4`, representing positional-only + positional-or-keyword)
*   `e, f`: Keyword-only. (`co_kwonlyargcount = 2`)

#### 2. Argument Parsing and Vectorcall Optimization (PEP 590)
When a function is called, CPython passes arguments using the **Vectorcall** calling protocol introduced in Python 3.8:
```c
PyObject *vectorcall(PyObject *callable, PyObject *const *args, size_t nargsf, PyObject *kwnames);
```
Where:
*   `args` is a pointer to a contiguous array containing all passed argument values.
*   `nargsf` specifies the number of positional arguments.
*   `kwnames` is a tuple of strings containing the keyword argument names. The keyword values are stored in the `args` array starting at index `nargsf`.

##### The Vectorcall Matching Algorithm:
1.  **Keyword Verification**: CPython parses the keyword strings in `kwnames`. If any string matches a parameter name located before index `co_posonlyargcount` (e.g. trying to pass `a=1`), the interpreter raises a `TypeError`.
2.  **Bypassing Dictionary Checks**: Because positional-only parameters are guaranteed to be passed positionally, CPython completely bypasses string-matching hash loops for the first `co_posonlyargcount` arguments.
3.  **Direct Array Offset Writing**: The VM copies references directly from the `args` array into the local frame variable cells by array index offset:
    $$\text{LocalFrameOffset}[i] = args[i] \quad \text{for } 0 \le i < co\_posonlyargcount$$
    This direct offset copy cuts down calling overhead by **10% to 15%**, which is highly beneficial for builtins and low-level utility functions that are called repeatedly.

---

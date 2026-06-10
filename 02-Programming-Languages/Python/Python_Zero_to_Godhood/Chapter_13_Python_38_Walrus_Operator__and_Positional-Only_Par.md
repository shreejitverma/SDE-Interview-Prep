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

```
2 LOAD_CONST               0 (1)
4 DUP_TOP
6 STORE_FAST               0 (x)
```
*Note: In Python 3.11+, the compiler utilizes specialized stack manipulation instructions (like `COPY 1`) instead of `DUP_TOP`, but the mechanism remains identical.* 

The execution steps for the walrus operator stack operation are:
1. `LOAD_CONST` pushes the reference to the constant integer `1` onto the value stack.
2. `DUP_TOP` replicates the top element of the stack, resulting in two references to `1` on the stack.
3. `STORE_FAST` pops one of the references and binds it to the local variable name `x`.
4. The remaining reference to `1` remains on the top of the stack, allowing parent expressions (such as `if` conditionals or loops) to consume it.

#### 3. Scoping Rules and Symbol Table Traversal
To prevent variable leakage and namespace pollution, PEP 572 specifies complex scoping rules, especially within comprehensions:
*   **Comprehensions**: Python list, set, and dict comprehensions, as well as generator expressions, are executed in a nested function scope.
*   **Variable Binding**: An assignment expression `x := ...` inside a comprehension binds the variable `x` in the *surrounding* (parent) scope, not the local comprehension scope.
*   **Symbol Table Resolution**: During compilation, the symbol table builder (`symtable.c`) traverses variables inside comprehensions. When it encounters a `NamedExpr`, it marks the target variable as free/cell or nonlocal/global depending on the parent scope, hoisting the reference out of the comprehension's inner namespace.
*   **Restrictions**: A target variable cannot share a name with a `.0` parameter (the iterator variable) or an explicit comprehension loop target (e.g., `[i for i in range(10) if (i := 2)]` is a syntax error).

---

### 14.2 Positional-Only Parameters (`/` / PEP 570)
Python 3.8 introduced the `/` marker to define positional-only parameters. Any parameters declared before the `/` marker cannot be passed as keyword arguments.

#### 1. C-level Representation in `PyCodeObject`
The compiler encodes parameter boundaries directly into the function's bytecode descriptor struct. In `code.h`, the `PyCodeObject` structure contains specific integer fields:

```c
typedef struct {
    PyObject_HEAD
    int co_argcount;            /* Number of positional arguments */
    int co_posonlyargcount;     /* Number of positional-only arguments */
    int co_kwonlyargcount;      /* Number of keyword-only arguments */
    /* ... additional fields ... */
} PyCodeObject;
```

#### 2. Argument Parsing & Calling Execution Path
When a function is called, CPython passes arguments through the internal calling mechanism (`_PyEval_EvalCodeWithName` or `_PyFunction_Vectorcall`). 
1. If the caller provides arguments via keyword dict mappings, the interpreter extracts the argument name and performs lookup check.
2. The interpreter compares the number of passed positional arguments against `co_posonlyargcount`.
3. If keyword arguments matching a parameter name before `co_posonlyargcount` are detected, the interpreter raises a runtime `TypeError`: `TypeError: f() got some positional-only arguments passed as keyword arguments`.

#### 3. Micro-Performance Optimization
Using positional-only parameters offers clear performance gains:
1. **Bypassing Hash Lookups**: When keyword arguments are eliminated, the interpreter bypasses dictionary keys extraction and hash checks for the positional-only segment of the arguments tuple.
2. **Direct Array Writes**: The VM copies references directly from the incoming contiguous array of positional arguments (vectorcall array) straight into the local frame variable storage.
$$\text{Offset Index} = \text{Passed Index}$$

---

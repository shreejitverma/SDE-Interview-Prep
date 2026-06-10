# Lexical Analysis and the Execution Model


Python is often described as an interpreted language, but this is a high-level abstraction. Under the hood, CPython follows a classic compiler-interpreter pipeline: Lexical Analysis $\rightarrow$ Parsing $\rightarrow$ Abstract Syntax Tree (AST) $\rightarrow$ Bytecode Generation $\rightarrow$ Virtual Machine Execution. While Chapter 1 introduced the LL(1) pipeline and Chapter 15 the PEG transition, this chapter formalizes the execution model and the mechanics of dynamic evaluation.

### 31.1 Lexical Analysis: From Raw Bytes to Tokens

The first stage of execution is the **Tokenizer**. In CPython, the tokenizer is implemented in C (`Parser/tokenizer.c`). Its job is to break the stream of source code (or bytes) into a stream of logical units called **Tokens**.

#### 1. Token Types and the `tokenize` Module
Python exposes its internal tokenizer via the `tokenize` standard library module.
```python
import tokenize
from io import BytesIO

code = "x = 5 + 10"
tokens = tokenize.tokenize(BytesIO(code.encode('utf-8')).readline)
for token in tokens:
    print(token)
```
Each token contains:
*   **Type**: `NAME`, `NUMBER`, `OP`, `NEWLINE`, `INDENT`, `DEDENT`.
*   **String**: The actual text (e.g., "x", "5").
*   **Start/End Pos**: Line and column numbers for error reporting.

#### 2. The Indentation Stack
Unlike most languages, Python's tokenizer is stateful regarding whitespace. It maintains an **Indentation Stack**.
*   When a line has more leading whitespace than the top of the stack, it emits an `INDENT` token and pushes the new level onto the stack.
*   When it has less, it pops from the stack and emits one or more `DEDENT` tokens until the levels match.

### 31.2 The AST and the Compilation Pipeline

Once tokens are parsed into a tree structure (PEG parser), CPython transforms the Concrete Syntax Tree (CST) into an **Abstract Syntax Tree (AST)**.

#### 1. The `ast` Module
The AST is the representation that Python's optimizer and code generator actually use. You can inspect it using the `ast` module:
```python
import ast
tree = ast.parse("x = 5 + 10")
print(ast.dump(tree, indent=4))
```
The output shows a `Module` containing an `Assign` node, with a `Name` target and a `BinOp` (Add) value.

#### 2. Constant Folding and Peephole Optimization
During the transition from AST to bytecode, CPython performs simple optimizations:
*   **Constant Folding**: Expressions like `1 + 2` are evaluated at compile-time and replaced with `3`.
*   **Dead Code Elimination**: Code following a `return` or `raise` that is unreachable is stripped.

### 31.3 Dynamic Execution: `eval()` vs. `exec()`

Python provides two primary built-ins for dynamic code execution. Their difference lies in what they accept and what they return.

#### 1. `eval(expression, globals=None, locals=None)`
*   **Input**: A single Python expression (something that can be on the right side of an assignment).
*   **Output**: The value of the expression.
*   **Internals**: Compiles the string to a code object with the `eval` mode, then executes it in the provided namespaces.

#### 2. `exec(object, globals=None, locals=None)`
*   **Input**: A block of Python code (statements, class/function definitions).
*   **Output**: Always `None`.
*   **Internals**: Compiles the code in `exec` mode. It modifies the `locals` dictionary (if provided) to include newly defined variables.

### 31.4 The `compile()` Function and Code Objects

Both `eval` and `exec` use `compile()` under the hood. For performance, you should pre-compile code if you intend to run it multiple times.

```python
code_str = "print('Hello, Godhood')"
# Modes: 'exec' for blocks, 'eval' for expressions, 'single' for REPL-style
code_obj = compile(code_str, filename="<string>", mode="exec")

# Inspection
print(code_obj.co_code)      # Raw bytecode
print(code_obj.co_consts)    # Constants used in the code
print(code_obj.co_names)     # Global/Builtin names used
```

`PyCodeObject` is the C struct that holds this data. When `exec(code_obj)` is called, the CPython VM pushes a new `PyFrameObject` onto the evaluation stack and hands the code object to the interpreter loop (`_PyEval_EvalFrameDefault`).

---


---
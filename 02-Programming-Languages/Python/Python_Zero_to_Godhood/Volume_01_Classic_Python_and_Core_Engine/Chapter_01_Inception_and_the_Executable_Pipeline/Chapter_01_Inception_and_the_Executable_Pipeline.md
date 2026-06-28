# Chapter 1: Inception and the Executable Pipeline (Python 1.0–1.6)

Python's surface is famously simple; its execution model is not. To master the language
you must first hold one mental picture firmly: **source text is not run — it is compiled,
ahead of time, into an immutable `PyCodeObject`, which a stack-based virtual machine then
interprets.** This chapter follows a single line of source from the tokenizer to the
evaluation loop, establishing the vocabulary — names, bindings, scopes, cells, code
objects, frames, opcodes — that every later chapter assumes. We anchor in Python's 1.x
origins because the design decisions made then (names as bindings, everything-is-an-object,
a static scope-analysis pass before execution) still govern how CPython 3.13 behaves today.

## Section Index
- **1.1** Inception and the core design philosophy
- **1.2** Names are bindings, not storage: the object model and the senior-engineer contrast
- **1.3** The execution pipeline, end to end
- **1.4** Parser evolution: LL(1) to PEG (PEP 617)
- **1.5** The symbol table and static scope classification
- **1.6** Closures and cell objects
- **1.7** `PyCodeObject`: the compiled blueprint
- **1.8** The virtual machine: frames and the evaluation loop
- **1.9** Performance model and anti-patterns
- **1.10** Summary and cross-references

---

## 1.1 Inception and the core design philosophy

**Why this exists.** Python was conceived in December 1989 by Guido van Rossum at CWI
(Centrum Wiskunde & Informatica) in the Netherlands, and first released publicly as
version 0.9.0 in February 1991, reaching 1.0 in January 1994. It was a deliberate
successor to the **ABC language** — elegant for teaching, but crippled in practice by
three architectural decisions Python set out to reverse:

1. **A monolithic, unextensible runtime.** ABC could not load native libraries or be
   driven from C. Python was built from the start around a C-extension boundary: the
   interpreter *is* a C library, and any data structure or algorithm can be dropped to C
   without changing the Python-level interface. This single decision is why NumPy, the
   `re` engine, and `asyncio`'s selectors can exist at all.
2. **A global, filesystem-backed state model.** ABC persisted variables in a global
   database, which made it slow and impossible to compose. Python uses ordinary in-memory
   namespaces (dictionaries) with lexical scoping.
3. **A closed ecosystem.** ABC could not reach the operating system. Python exposed system
   calls, signals, and process control as first-class library surface from its earliest
   versions.

Three design commitments fell out of these reversals and never changed:

- **Significant whitespace.** CPython's tokenizer emits explicit `INDENT` and `DEDENT`
  tokens; block structure is part of the grammar, not a convention. The intent is that the
  *visual* structure a reader sees is, by construction, the *execution* structure the
  compiler sees — the two cannot drift apart, as they can in brace languages.
- **A unified object model.** Every value — an integer, a function, a class, a module, a
  stack frame — is a heap-allocated object reachable through a `PyObject *`. There is no
  separate world of "primitives" as in Java or C#. This uniformity is what makes
  introspection (`x.__class__`, `dis`, `inspect`) total rather than partial.
- **Names as bindings.** `x = 5` does not create a typed memory slot named `x`; it binds
  the *name* `x`, in some namespace, to the object `5`. This is the most consequential
  decision in the language, and §1.2 unpacks it.

> **What the interpreter actually does at startup.** Running `python script.py` initializes
> a single process-wide runtime (`PyInterpreterState`) and at least one thread state
> (`PyThreadState`), imports the `builtins`, `sys`, and `__main__` modules, then compiles
> and executes `script.py` in the `__main__` module's namespace. "Running a script" is
> therefore "compile a module body to a code object, then evaluate it in a fresh frame."

---

## 1.2 Names are bindings, not storage: the object model and the senior-engineer contrast

**The model.** A Python name lives in a namespace — a mapping from `str` to `PyObject *`.
Assignment rebinds the name; it never copies or mutates the object the name previously
referred to. Every object carries, at minimum, a **reference count** and a **type
pointer** (Chapter 2 dissects the `PyObject` header). Assignment is therefore a constant
two-step: bind the name, and increment the target object's refcount.

**The senior-engineer contrast.** This is where intuition from systems languages misleads:

| Concept | C / C++ | Java | Python (CPython) |
|---|---|---|---|
| What a variable *is* | typed storage at an address | a typed slot holding a primitive *or* a reference | a name bound to an object reference; the name has no type |
| `a = b` | copies bytes (value or pointer) per the declared type | copies a primitive, or copies a reference | binds `a` to the same object `b` refers to; no copy |
| Type | belongs to the variable, fixed at compile time | belongs to the variable | belongs to the **object**, not the name |
| Lifetime | scope / `new`/`delete` / RAII | tracing GC | refcount → immediate free, plus a cyclic collector (Chapter 3) |

The practical consequences are immediate and are the source of most surprises for
engineers arriving from C++ or Java:

```python
# Caption: assignment binds; it does not copy. Mutating through one name is visible
# through every name bound to the same object.
a = [1, 2, 3]
b = a            # b is NOT a copy; a and b name the same list object
b.append(4)
print(a)         # [1, 2, 3, 4]
print(a is b)    # True  -> identity, not equality
```

Verified output (CPython 3.13.5):

```text
[1, 2, 3, 4]
True
```

`is` compares **identity** (are these the same object?), `==` compares **value**. The
distinction is meaningless in C where `a == b` on pointers *is* identity; in Python the two
are separate operators because a name and an object are separate things. We return to the
mutable-default-argument trap and copy semantics this implies in Chapter 5; for now, fix
the model: **a Python program is a graph of objects, and names are merely labelled edges
into it.**

---

## 1.3 The execution pipeline, end to end

**Why this exists.** Python feels interpreted — you can type a line and see it run — but
nothing executes until it has been compiled to bytecode. Understanding the pipeline tells
you *where* each kind of error is produced (a `SyntaxError` cannot occur at runtime; a
`NameError` cannot occur at compile time) and *why* certain constructs are fast or slow.

The path from text to result has five stages:

```text
[ source text ]
      | tokenizer  (Parser/tokenizer.c): emits a token stream, including INDENT/DEDENT
      v
[ token stream ]
      | parser  (Parser/, PEG since 3.9): builds the Abstract Syntax Tree (AST)
      v
[   AST   ]  ( the ast module exposes this tree )
      | symbol-table pass  (Python/symtable.c): classifies every name into a scope
      v
[ symbol table ]
      | compiler  (Python/compile.c): emits bytecode + metadata
      v
[ PyCodeObject ]   ( immutable: bytecode, constants, names, flags )
      | evaluation loop  (Python/ceval.c, _PyEval_EvalFrameDefault): runs per frame
      v
[  result  ]
```

The first four stages are **ahead-of-time compilation**; only the last is "interpretation."
You can observe each boundary from the standard library: `tokenize` for stage 1, `ast` for
stages 2–3, `symtable` for stage 4, `compile()` and `dis` for stage 5's input, and the
running program for the loop itself. Sections 1.4–1.8 take each in turn.

---

## 1.4 Parser evolution: LL(1) to PEG (PEP 617)

**Why this exists.** For most of its life CPython used a hand-tunable **LL(1)** parser —
**L**eft-to-right scan, **L**eftmost derivation, **1** token of lookahead. LL(1) is fast
and simple but structurally limited, and those limits shaped the grammar (and therefore
the language) for two decades. **PEP 617** replaced it with a **PEG** parser in Python 3.9.

**The LL(1) limitation: left recursion.** A top-down LL(1) parser cannot directly handle a
**left-recursive** rule — one whose right-hand side begins with the non-terminal itself:

$$A \rightarrow A\,\alpha \;\mid\; \beta$$

Asked to expand $A$, the parser would expand $A$ again before consuming any input,
recursing forever:

$$A \rightarrow A\alpha \rightarrow A\alpha\alpha \rightarrow A\alpha\alpha\alpha \rightarrow \cdots$$

The classic workaround is to mechanically rewrite the rule into an equivalent
right-recursive pair:

$$A \rightarrow \beta A' \qquad A' \rightarrow \alpha A' \;\mid\; \varepsilon$$

This is provably equivalent as a *language*, but it distorts the grammar: the natural
left-associative shape of expressions like `a - b - c` has to be reconstructed after the
fact, and the grammar becomes harder to read and to evolve.

**The PEG parser.** A **Parsing Expression Grammar** parser is also top-down, but it adds:

- **Ordered choice.** Alternatives are tried in order; the first match wins. This removes
  the ambiguity LL(1) had to avoid.
- **Unlimited lookahead** via backtracking, so productions that LL(1) could not distinguish
  with one token become expressible.
- **Packrat memoization.** Each (rule, input-position) result is cached, so backtracking
  does not blow up to exponential time, and **left recursion is supported directly** by
  seeding the memo table and iterating to a fixed point.
- **Direct AST construction**, skipping the intermediate Concrete Syntax Tree (CST) the old
  pipeline built and then walked.

```text
LL(1) pipeline (pre-3.9):
[source] -> [tokenizer] -> [Concrete Syntax Tree] -> [AST builder] -> [AST]

PEG pipeline (3.9+, PEP 617):
[source] -> [tokenizer] -> [PEG parser (memoized)] -> [AST]
```

**Old-vs-new, why it mattered.** The PEG switch was not cosmetic. The **structural pattern
matching** of `match`/`case` (PEP 634, Chapter 15) is expressible largely *because* the
grammar is no longer bound by LL(1). The cost is that the grammar can now encode rules
whose meaning is order-dependent, which places more responsibility on the grammar authors —
but for the language user, the result is strictly more expressive syntax and better error
messages.

> **Implementation detail, not language semantics.** The parser is entirely a CPython
> concern. The *language* is defined by the grammar in the reference; whether it is parsed
> LL(1) or PEG is invisible to a correct program (with the narrow exception of newly
> expressible syntax). We cover the formal grammar and lexical rules in Vol X.

---

## 1.5 The symbol table and static scope classification

**Why this exists.** Before emitting a single opcode, the compiler walks the AST to decide,
**statically**, what every name *means*: is it local to this function, a parameter, a
global, a builtin, or a free variable captured from an enclosing function? This
classification is what lets CPython use array-indexed local access (`LOAD_FAST`) instead of
a dictionary lookup for every variable — the single largest reason Python functions are not
even slower than they are.

CPython classifies each name binding into one of these scopes (`Python/symtable.c`):

1. **Local** — bound inside the current function.
2. **Global, explicit** — declared with `global x`.
3. **Global, implicit** — referenced but never bound in the function; resolved at runtime
   against module globals, then builtins.
4. **Enclosing / free** — bound in an outer function and read (or, with `nonlocal`,
   rebound) by a nested function.

The `symtable` module exposes exactly this pass:

```python
# Caption: inspect CPython's static scope analysis without running the code.
import symtable

src = """
def outer_func(x):
    z = 10
    def inner_func():
        nonlocal z
        return x + z
    return inner_func
"""

table = symtable.symtable(src, filename="<string>", compile_type="exec")
outer = table.lookup("outer_func").get_namespace()
print("outer identifiers:", sorted(outer.get_identifiers()))
print("'z' is_local in outer:", outer.lookup("z").is_local())
print("'x' is_parameter in outer:", outer.lookup("x").is_parameter())

inner = outer.lookup("inner_func").get_namespace()
print("inner free vars (closure):", inner.get_frees())
print("'z' is_free in inner:", inner.lookup("z").is_free())
```

Verified output (CPython 3.13.5):

```text
outer identifiers: ['inner_func', 'x', 'z']
'z' is_local in outer: True
'x' is_parameter in outer: True
inner free vars (closure): ('z', 'x')
'z' is_free in inner: True
```

> **Correctness note.** Earlier drafts of this material called `Symbol.is_cell()`. That
> method does not exist in the `symtable` API (verified on 3.13.5; it raises
> `AttributeError`). The supported predicates are `is_local`, `is_global`, `is_free`,
> `is_parameter`, `is_namespace`, `is_nonlocal`, `is_declared_global`, `is_assigned`,
> `is_referenced`, `is_imported`, `is_annotated`. To see which of an enclosing function's
> locals were promoted to **cells**, read the *compiled code object's* `co_cellvars`, as in
> §1.6 — that is the authoritative view.

---

## 1.6 Closures and cell objects

**Why this exists.** A nested function may outlive the call that created it, yet still read
the enclosing function's locals. Those locals cannot live in the outer frame's fast-locals
array, because that array dies when the frame is popped. CPython solves this by promoting a
captured local to a **cell** — a tiny heap object shared by reference between the enclosing
and nested functions.

```c
/* Illustrative; Objects/cellobject.c. A cell is a one-slot indirection on the heap. */
typedef struct {
    PyObject_HEAD
    PyObject *ob_ref;   /* the captured object, shared by closure and enclosing scope */
} PyCellObject;
```

The compiler records, on each code object, which locals are cells and which names are free:

- `co_cellvars` — locals of *this* function that are captured by a nested function (boxed
  into cells).
- `co_freevars` — names *this* function reads from an enclosing scope (read through cells
  passed in at closure-construction time).

```python
# Caption: the code-object view of cells and free variables (authoritative).
src = """
def outer_func(x):
    z = 10
    def inner_func():
        nonlocal z
        return x + z
    return inner_func
"""
top = compile(src, "<string>", "exec")
(outer_code,) = [c for c in top.co_consts if getattr(c, "co_name", None) == "outer_func"]
(inner_code,) = [c for c in outer_code.co_consts if getattr(c, "co_name", None) == "inner_func"]
print("outer_func co_cellvars:", outer_code.co_cellvars)
print("inner_func co_freevars:", inner_code.co_freevars)
```

Verified output (CPython 3.13.5):

```text
outer_func co_cellvars: ('x', 'z')
inner_func co_freevars: ('x', 'z')
```

Both `x` and `z` are cells in `outer_func` (each is captured by `inner_func`), and both are
free variables of `inner_func`. The shared cell is why a closure sees *live* updates to a
captured variable, not a snapshot — a frequent source of the "late-binding closure in a
loop" bug, which we dissect in Chapter 3 alongside comprehension scope.

---

## 1.7 `PyCodeObject`: the compiled blueprint

**Why this exists.** The output of compilation is a `PyCodeObject`: an **immutable**,
shareable description of a unit of executable code (a module body, a function, a `lambda`,
a comprehension, a class body). It is immutable so it can be cached, pickled by reference,
shared across calls and threads, and reasoned about statically. A *function* is a thin,
mutable wrapper (`__defaults__`, `__closure__`, `__globals__`) around an immutable code
object.

```c
/* Illustrative; fields from Include/cpython/code.h. The real struct has changed across
   versions (notably the 3.11 frame/codeobject rework); these fields are stable and
   observable from Python as co_* attributes. */
struct PyCodeObject {
    PyObject_HEAD
    int co_argcount;        /* positional parameters */
    int co_posonlyargcount; /* positional-only parameters (PEP 570, 3.8+) */
    int co_kwonlyargcount;  /* keyword-only parameters */
    int co_nlocals;         /* number of local variables */
    int co_stacksize;       /* max evaluation-stack depth the VM must reserve */
    int co_flags;           /* compiler flags (CO_OPTIMIZED, CO_NEWLOCALS, CO_GENERATOR…) */
    PyObject *co_consts;    /* tuple of literal constants */
    PyObject *co_names;     /* tuple of global/attribute names */
    PyObject *co_varnames;  /* tuple of local variable names */
    PyObject *co_freevars;  /* tuple of free (captured) names */
    PyObject *co_cellvars;  /* tuple of local names boxed into cells */
    /* bytecode, line table, exception table, etc. */
};
```

Every one of those `co_*` fields is readable from Python:

```python
# Caption: read the compiled blueprint of a live function.
def example_fn(a, b):
    local_val = 42
    return a + b + local_val

co = example_fn.__code__
print("co_varnames:", co.co_varnames)
print("co_consts:", co.co_consts)
print("co_argcount:", co.co_argcount, "co_nlocals:", co.co_nlocals,
      "co_stacksize:", co.co_stacksize)
print("co_flags:", bin(co.co_flags))
```

Verified output (CPython 3.13.5):

```text
co_varnames: ('a', 'b', 'local_val')
co_consts: (None, 42)
co_argcount: 2 co_nlocals: 3 co_stacksize: 2
co_flags: 0b11
```

Three details worth internalizing:

- `co_consts` contains `None` even though the source never mentions it: it is the implicit
  return value's constant. The literal `42` is interned into the pool; the bytecode refers
  to it by index, not by value.
- `co_flags == 0b11` sets `CO_OPTIMIZED | CO_NEWLOCALS` — the flags that say "this is a real
  function: use fast array-indexed locals and a fresh locals namespace per call." A module
  or class body does **not** set these, which is precisely why module-level code uses
  dictionary-based `LOAD_NAME` and is slower (§1.9).
- **`EXTENDED_ARG`.** An opcode's inline argument is only one byte. To reference, say, the
  300th constant, the compiler prefixes an `EXTENDED_ARG` opcode that supplies the
  high-order bits; the VM shifts and combines them, allowing arguments up to 32 bits wide.
  This is why you may see `EXTENDED_ARG` in disassembly of large functions.

---

## 1.8 The virtual machine: frames and the evaluation loop

**Why this exists.** CPython is a **stack-based virtual machine**. Each call creates a
**frame** holding the executing code object, the local variables, and an **evaluation
stack** onto which operands are pushed and from which results are popped. A single
monolithic C function, `_PyEval_EvalFrameDefault` in `Python/ceval.c`, dispatches opcodes
in a loop until the frame returns.

```c
/* Illustrative shape of the dispatch loop, Python/ceval.c. The modern loop uses
   computed-goto threaded dispatch and inline caches, but the skeleton is this. */
PyObject *_PyEval_EvalFrameDefault(PyThreadState *tstate,
                                   _PyInterpreterFrame *frame, int throwflag) {
    for (;;) {
        _Py_CODEUNIT word = *next_instr++;
        switch (word.op.code) {
            case LOAD_FAST: {                 /* push a local by array index  */
                PyObject *v = GETLOCAL(word.op.arg);
                Py_INCREF(v); PUSH(v); DISPATCH();
            }
            case BINARY_OP: {                 /* pop two, push their combination */
                PyObject *r = POP(), *l = POP();
                PyObject *res = PyNumber_BinaryOp(l, r, word.op.arg);
                Py_DECREF(l); Py_DECREF(r); PUSH(res); DISPATCH();
            }
            case RETURN_VALUE:
                return POP();
        }
    }
}
```

Do not trust the textbook picture of the bytecode, though — read what *this* interpreter
actually emits. The `dis` module disassembles a code object into the real instruction
stream:

```python
# Caption: the actual bytecode CPython 3.13 runs for example_fn.
def example_fn(a, b):
    local_val = 42
    return a + b + local_val

import dis
dis.dis(example_fn)
```

Verified output (CPython 3.13.5):

```text
  1           RESUME                   0

  2           LOAD_CONST               1 (42)
              STORE_FAST               2 (local_val)

  3           LOAD_FAST_LOAD_FAST      1 (a, b)
              BINARY_OP                0 (+)
              LOAD_FAST                2 (local_val)
              BINARY_OP                0 (+)
              RETURN_VALUE
```

This single listing demolishes several outdated mental models:

- **`RESUME 0`** opens every code object since 3.11. It is a no-op hook used by the
  specializing adaptive interpreter and by debuggers/profilers; its presence is your cue
  that you are reading modern bytecode.
- **`BINARY_OP 0 (+)`**, not `BINARY_ADD`. Python 3.11 collapsed the family of
  per-operator binary opcodes into a single `BINARY_OP` whose argument selects the
  operation. Material that still shows `BINARY_ADD` predates 3.11.
- **`LOAD_FAST_LOAD_FAST 1 (a, b)`** is a **superinstruction**: the compiler fused two
  adjacent `LOAD_FAST`s into one opcode to cut dispatch overhead. The VM is no longer one
  opcode per source operation.
- There is **no trailing `LOAD_CONST None` / `RETURN_VALUE`** pair: because the function
  ends in an explicit `return`, the compiler does not append the implicit `return None`.

Tracing the evaluation stack for `a + b` makes the stack discipline concrete:

```text
  LOAD_FAST_LOAD_FAST(a,b)   BINARY_OP(+)        ...        RETURN_VALUE
  +---------+                +---------+                    +---------+
  |    b    |                |  a + b  |                    | (empty) |
  +---------+                +---------+        -->         +---------+
  |    a    |       -->      | (empty) |                    | (empty) |
  +---------+                +---------+                    +---------+
```

> **3.11+ reality: the specializing adaptive interpreter.** Modern CPython does not execute
> a fixed opcode stream forever. As code runs hot, generic opcodes are **specialized** in
> place into faster variants (e.g. `BINARY_OP` over two `int`s can specialize to an
> integer-add path that skips the general dispatch), with **inline caches** stored in the
> code units themselves. This is *not* the JIT (PEP 744, Vol VII) — it is interpretation
> that rewrites itself. We cover it in depth in Vol VI, Ch 17.

---

## 1.9 Performance model and anti-patterns

The pipeline is not academic — it dictates the constant factors of real code.

**Local access is array indexing; global/builtin access is dictionary lookup.** Inside an
optimized function, a local read is `LOAD_FAST` (an index into a C array). A global or
builtin read is `LOAD_GLOBAL`, which probes the module `__dict__` and then the `builtins`
dict. The classic hot-loop optimization — binding a global or method to a local before the
loop — is *purely* an exploit of this difference:

```python
# Caption: hoisting a global lookup to a local turns N dict probes into N array reads.
import math

def slow(xs):
    return [math.sqrt(x) for x in xs]      # LOAD_GLOBAL math; LOAD_ATTR sqrt -- each item

def fast(xs):
    sqrt = math.sqrt                       # one lookup, hoisted
    return [sqrt(x) for x in xs]           # LOAD_FAST sqrt -- each item
```

Both are correct; `fast` simply pays the attribute/global lookup once. On large inputs the
difference is measurable, and the *reason* is visible in the disassembly. (This is a
constant-factor win, not an algorithmic one — reach for it only in genuine hot loops; see
the profiling discipline in Vol IX.)

**The dynamic-namespace trap.** If the compiler cannot statically know a function's locals
— because it contains `exec()` against an implicit namespace, or `from module import *` at
function scope (the latter is in fact a `SyntaxError` in a function in Python 3) — it must
abandon fast locals and fall back to the dictionary-based `LOAD_NAME`, which searches local,
then global, then builtin namespaces at runtime. The lesson is structural: **keep function
scopes statically analyzable.** Dynamic features defeat the single most important
optimization the compiler performs.

**`SyntaxError` vs `NameError`, and where they live.** Because parsing and symbol analysis
happen before any execution, a malformed expression is rejected at *compile* time and can
never reach the VM. Conversely, an unbound name is a *runtime* `NameError`, because name
resolution against globals/builtins is a runtime operation. Knowing which stage owns an
error tells you where to look for it.

**Don't fight immutability of code objects.** Code objects are immutable by design;
"patching bytecode at runtime" is a stunt, not an optimization. The supported lever is the
*function* wrapper around the code object — decorators, `functools.partial`, closures — not
the blueprint itself.

---

## 1.10 Summary and cross-references

- A Python program is a **graph of heap objects**; names are **bindings** (labelled edges),
  not typed storage. Assignment rebinds and adjusts refcounts; it never copies. `is` is
  identity, `==` is value.
- Source is **compiled ahead of time** through tokenizer → PEG parser (PEP 617, 3.9+) →
  symbol table → compiler, producing an immutable **`PyCodeObject`**. Only the final
  **evaluation loop** is interpretation.
- The **symbol table** statically classifies every name (local / global / free), which
  enables array-indexed `LOAD_FAST` locals and **cell**-based closures.
- The real bytecode of modern CPython (3.11+) includes `RESUME`, the unified `BINARY_OP`,
  and fused **superinstructions** like `LOAD_FAST_LOAD_FAST`, and it **specializes itself**
  as it runs — read `dis` output, not folklore.
- Performance is governed by the pipeline: **locals beat globals**, dynamic namespaces
  defeat optimization, and errors are owned by the stage that produces them.

**Cross-references.** The `PyObject` header and reference counting → Chapter 2. Cyclic
garbage collection and comprehension/closure scope → Chapter 3. The specializing adaptive
interpreter in depth → Vol VI, Ch 17. The copy-and-patch JIT (PEP 744) → Vol VII. The
formal grammar, lexical rules, and the data model → Vol X. The `ceval.c` evaluation loop in
full → Vol VIII.

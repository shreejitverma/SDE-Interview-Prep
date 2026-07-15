# Chapter 17: Faster CPython — the Specializing Adaptive Interpreter (Python 3.11)

Python 3.11 was the first release of the multi-year "Faster CPython" project, and it made typical
programs **10–60% faster** without changing a line of your code. The engine behind that is **PEP
659: the specializing adaptive interpreter** — the interpreter *rewrites its own bytecode at
runtime*, replacing generic opcodes with type-specialized ones backed by **inline caches**. This is
not a JIT (that is PEP 744, Vol VII): no machine code is generated; the interpreter loop simply gets
much smarter about the common case. This chapter explains the mechanism, demonstrates specialization
*happening* via `dis(..., adaptive=True)`, and derives the coding guidelines that keep your hot paths
on the fast track.

## Section Index
- **17.1** PEP 659: the specialization lifecycle
- **17.2** Inline caches and type versioning
- **17.3** Specialization families and deoptimization
- **17.4** The rest of the 3.11 leap: zero-cost exceptions, cheaper frames, PEP 657
- **17.5** Performance and anti-patterns
- **17.6** Summary and cross-references

---

## 17.1 PEP 659: the specialization lifecycle

**Why this exists.** A generic opcode like `LOAD_ATTR` must handle *any* object: walk the type's
MRO, consult `__getattribute__`, probe instance and class dictionaries. But real code is
overwhelmingly **monomorphic** — a given `p.x` almost always sees the *same* type at that exact call
site, run after run. PEP 659 exploits this: observe what actually flows through each instruction, and
once it is stable, **patch the bytecode in memory** with a specialized opcode that assumes that shape
and falls back safely if the assumption breaks.

Each adaptive instruction moves through a lifecycle:

```text
[ generic ]  LOAD_ATTR
     │  first executions: gather type info, decrement a counter
     ▼
[ adaptive / warming ]  (counter counting down)
     │  types stayed consistent → counter hits 0
     ▼
[ specialized ]  LOAD_ATTR_INSTANCE_VALUE   ← fast path, validated by a cheap guard
     │  guard fails (a different type appears)
     ▼
[ deoptimize ]  → run the generic slow path; if it keeps missing, give up specializing
```

The critical property: a specialized opcode is **always guarded**. It first checks a cheap
invariant (a cached type version, §17.2); if the guard holds it takes the fast path, and if not it
**deoptimizes** to the generic behavior. Correctness is never at risk — only speed.

---

## 17.2 Inline caches and type versioning

**What the interpreter actually does.** The specialized opcode needs somewhere to store "what I
learned" — the expected type and the attribute's offset. CPython stores this in an **inline cache**:
extra `_Py_CODEUNIT` slots placed *directly after the instruction* in the code array, so the data is
in the same cache line the interpreter is already reading. For `LOAD_ATTR` the cache is an
`_PyAttrCache`:

```c
/* Illustrative; Include/internal/pycore_code.h */
typedef struct {
    uint16_t counter;        /* adaptive counter: when it hits 0, try to specialize */
    uint16_t version[2];     /* the target type's tp_version_tag (cache guard) */
    uint16_t index;          /* the attribute's offset in the instance values array */
} _PyAttrCache;
```

The guard is the type's **`tp_version_tag`** — a per-type id that CPython **bumps whenever the class
is mutated** (a method added, a class attribute set, the MRO changed). A specialized `LOAD_ATTR`
checks the live object's type version against the cached one; if they match, it reads
`obj_values[index]` directly — no dict lookup, no MRO walk. If the class was mutated, the tag no
longer matches and the opcode deoptimizes. (`LOAD_GLOBAL` works the same way against the module and
builtins dictionaries' *keys version*.)

You can watch specialization happen. Warm a function by running it, then disassemble it with
`adaptive=True`:

```python
# Caption: after warm-up, generic opcodes have been patched to specialized ones.
import dis

class P:
    def __init__(self, x):
        self.x = x

def hot(p):
    return p.x + p.x

for _ in range(1000):       # make the call site hot and monomorphic
    hot(P(5))

dis.dis(hot, adaptive=True)
```

Verified output (CPython 3.13.5):

```text
  9   LOAD_FAST                 0 (p)
      LOAD_ATTR_INSTANCE_VALUE  0 (x)     # specialized: direct offset read, no dict lookup
      LOAD_FAST                 0 (p)
      LOAD_ATTR_INSTANCE_VALUE  0 (x)
      BINARY_OP_ADD_INT         0 (+)     # specialized: int + int fast path
      RETURN_VALUE
```

(The function also opens with `RESUME_CHECK`, the specialized form of the `RESUME` from Chapter 1.)
Both the attribute load *and* the addition specialized: `LOAD_ATTR_INSTANCE_VALUE` reads the slot by
offset, and `BINARY_OP_ADD_INT` runs the int-only add that skips the general `nb_add` dispatch:

```python
# Caption: confirm the binary-op specialization in isolation.
import dis
def addints(a, b):
    return a + b
for _ in range(1000):
    addints(1, 2)
print([i.opname for i in dis.get_instructions(addints, adaptive=True)
       if i.opname.startswith("BINARY_OP")])
```

Verified output (CPython 3.13.5):

```text
['BINARY_OP_ADD_INT']
```

---

## 17.3 Specialization families and deoptimization

PEP 659 specializes the opcodes that dominate real workloads. The common families:

| Generic | Specialized examples | Fast path |
|---|---|---|
| `LOAD_ATTR` | `LOAD_ATTR_INSTANCE_VALUE`, `LOAD_ATTR_SLOT`, `LOAD_ATTR_METHOD_*` | direct offset / slot read |
| `LOAD_GLOBAL` | `LOAD_GLOBAL_MODULE`, `LOAD_GLOBAL_BUILTIN` | dict value-array read, no hashing |
| `BINARY_OP` | `BINARY_OP_ADD_INT`, `BINARY_OP_ADD_FLOAT`, `BINARY_OP_ADD_UNICODE` | typed arithmetic/concat |
| `BINARY_SUBSCR` | `BINARY_SUBSCR_LIST_INT`, `BINARY_SUBSCR_DICT` | typed indexing |
| `CALL` | `CALL_PY_EXACT_ARGS`, `CALL_BUILTIN_*` | skip argument re-binding |
| `STORE_ATTR`, `COMPARE_OP`, `FOR_ITER`, `TO_BOOL`, `CONTAINS_OP`, `SEND` | various | type-specific |

**Deoptimization and thrashing.** Specialization is a bet on monomorphism. If a call site is
**polymorphic** — `p.x` sees a `Dog` then a `Cat` whose `.x` lives at a different offset, or `a + b`
sees ints then strings — the guard keeps failing. The opcode deoptimizes to the slow path, may
re-warm, re-specialize for the new type, miss again, and **thrash**, paying the *combined* cost of
guard checks, slow lookups, and repeated re-specialization. The worst case is *worse* than the
pre-3.11 generic interpreter for that site.

**Coding for the specializer** (the practical payoff):
- **Keep hot call sites monomorphic.** Don't funnel objects of structurally different types through
  the same hot function or attribute access; split into separate functions if needed.
- **Don't mutate classes at runtime** in steady state — adding a method or class attribute bumps
  `tp_version_tag` and **invalidates every inline cache** that depended on it. Configure behavior at
  class-definition time.
- **`__slots__` specializes well** (`LOAD_ATTR_SLOT`) and removes the per-instance dict — a double
  win for hot, numerous objects (Chapter 13).

These are guidelines for *hot* code only; for everything else, write clear code and let the
interpreter do its job.

---

## 17.4 The rest of the 3.11 leap: zero-cost exceptions, cheaper frames, PEP 657

Specialization is the headline, but 3.11's speedup is the sum of several changes:

- **Zero-cost exceptions** (Chapter 6): the runtime block stack was replaced by a static exception
  table, so a `try` that doesn't raise costs **nothing** on the happy path.
- **Cheaper, "lazy" frames.** Frame objects were slimmed and are created more cheaply (allocated
  inline on a per-thread data stack), making Python-to-Python calls markedly faster — the foundation
  `CALL_PY_EXACT_ARGS` builds on.
- **Fine-grained error locations (PEP 657).** Tracebacks now point a caret at the *exact
  sub-expression* that failed, not just the line:

```python
# Caption: PEP 657 — the traceback marks which sub-expression raised.
import traceback
def f():
    a, b, c = 1, 2, 0
    return a / b / c
try:
    f()
except ZeroDivisionError:
    for line in traceback.format_exc().splitlines():
        if "a / b" in line or "~" in line or "^" in line:
            print(line.strip())
```

Verified output (CPython 3.13.5):

```text
return a / b / c
~~~~~~^~~
```

The carets isolate `(a / b) / c`'s failing division — invaluable for debugging compound expressions
and long attribute chains. (This location data lives in the code object's line table; it costs a
little memory, disable-able via `-X no_debug_ranges`.)

---

## 17.5 Performance and anti-patterns

- **Let the interpreter warm up before benchmarking.** A cold function runs generic opcodes;
  measure steady-state (use `timeit`/`pyperf`, Vol IX), not the first call.
- **Monomorphism is the lever, not micro-tweaks.** The biggest 3.11+ wins come from consistent types
  at hot sites; chasing opcode counts by hand is usually wasted effort.
- **Runtime class mutation is a cache killer** — monkeypatching a class in a hot loop deoptimizes
  every dependent site globally.
- **This is interpretation, not compilation.** Specialization removes interpreter overhead on the
  fast path but does not vectorize, inline across calls, or escape-analyze. For order-of-magnitude
  numeric speed you still drop to NumPy/Cython/C (Vol IX) — or, in 3.13+, the experimental JIT
  (Vol VII), which builds *on top of* these specialized opcodes.
- **Don't fight `tp_version_tag`.** Stable class shapes keep caches valid; dynamic attribute schemas
  defeat the optimizer.

---

## 17.6 Summary and cross-references

- **PEP 659** makes the interpreter **specialize its own bytecode at runtime**: generic → adaptive →
  specialized, with a cheap **guard** and safe **deoptimization** — *not* a JIT, no machine code.
- Specialized opcodes are backed by **inline caches** adjacent to the bytecode, guarded by the
  type's **`tp_version_tag`** (and dict keys versions); `dis(..., adaptive=True)` shows them
  (`LOAD_ATTR_INSTANCE_VALUE`, `BINARY_OP_ADD_INT`, …).
- **Polymorphic** call sites **deoptimize** and can **thrash**; keep hot paths **monomorphic** and
  avoid runtime class mutation. `__slots__` specializes especially well.
- 3.11 also brought **zero-cost exceptions**, **cheaper frames**, and **PEP 657** caret-precise
  tracebacks.

**Cross-references.** The base interpreter loop and `RESUME` → Chapter 1. Zero-cost exceptions →
Chapter 6. `__slots__` and attribute storage → Chapters 4, 13 and Vol VIII. The copy-and-patch JIT
(PEP 744) that consumes these specialized opcodes → Vol VII. `ceval.c` and the full opcode set →
Vol VIII. Benchmarking discipline → Vol IX. PEP 695 generics and exception groups (the rest of
3.11–3.12) → Chapters 18 and 19.

# Chapter 12: F-Strings, Variable Annotations, and the Compact Dict (Python 3.6)

Python 3.6 is the release that made modern Python *feel* modern. Three changes did it:
**f-strings** (PEP 498) replaced `%` and `.format()` with compile-time interpolation; **variable
annotations** (PEP 526) extended type hints from function signatures to any binding, building the
runway for the whole typing ecosystem; and the **compact dict** turned the core mapping type
ordered and 30–40% smaller — an "implementation detail" in 3.6 that became a language guarantee in
3.7 and reshaped how Python code is written. We teach each with the *current* mechanics, including
the **PEP 701** f-string overhaul (3.12) that the original 3.6 descriptions predate.

## Section Index
- **12.1** Formatted string literals (PEP 498) and the PEP 701 overhaul
- **12.2** Variable annotations (PEP 526)
- **12.3** The compact, ordered dictionary
- **12.4** Performance and anti-patterns
- **12.5** Summary and cross-references

---

## 12.1 Formatted string literals (PEP 498) and the PEP 701 overhaul

**Why this exists.** The pre-3.6 options both pay runtime cost. `%`-formatting uses rigid C
printf routines and builds a temporary tuple; `"...".format(x)` does an attribute lookup for
`.format`, a function call (new frame, `*args`/`**kwargs`), *and* re-parses the format string on
every call. **f-strings** move interpolation to **compile time**: the parser splits the literal
into constant chunks and embedded expression AST sub-trees, and the compiler emits inline opcodes —
no method lookup, no call frame, no runtime format parsing.

```python
# Caption: an f-string compiles to inline formatting opcodes — no .format() call.
import dis
def fstring_format(name, age):
    return f"Name: {name!r:>10}, Age: {age}"
dis.dis(fstring_format)
```

Verified output (CPython 3.13.5):

```text
  6   LOAD_CONST        1 ('Name: ')
      LOAD_FAST         0 (name)
      CONVERT_VALUE     2 (repr)        # the !r conversion
      LOAD_CONST        2 ('>10')
      FORMAT_WITH_SPEC                  # apply the :>10 format spec
      LOAD_CONST        3 (', Age: ')
      LOAD_FAST         1 (age)
      FORMAT_SIMPLE                     # no conversion, no spec
      BUILD_STRING      4               # C-level concat of the 4 pieces
      RETURN_VALUE
```

**This is modern (3.13) bytecode, and it differs from every pre-3.12 text.** The original 3.6
implementation used a single `FORMAT_VALUE` opcode with packed flag bits. **PEP 701 (Python 3.12)**
reimplemented f-strings in the PEG grammar and split formatting into clearer opcodes:
`CONVERT_VALUE` (`!s`/`!r`/`!a`), `FORMAT_SIMPLE` (plain `{x}`), and `FORMAT_WITH_SPEC` (`{x:spec}`),
finished by `BUILD_STRING` (a single C-level concatenation that pre-sizes the buffer). If you see
`FORMAT_VALUE` in a disassembly, you are reading pre-3.12 output.

**The feature surface.** f-strings support `!r`/`!s`/`!a` conversions, full format specs, and the
`{expr=}` self-documenting form (3.8) that prints both the expression text and its value:

```python
# Caption: format specs, conversions, the = debug form, and PEP 701 relaxations.
x = 3.14159
print("spec:", f"{x:.2f}")
print("conv:", f"{'hi'!r}")
print("debug:", f"{x=}")
val = {"k": "v"}
print("nested same-quotes (PEP 701, 3.12+):", f"{val["k"]}")
print("multiline expression (PEP 701):", f"{
    1 + 2
}")
```

Verified output (CPython 3.13.5):

```text
spec: 3.14
conv: 'hi'
debug: x=3.14159
nested same-quotes (PEP 701, 3.12+): v
multiline expression (PEP 701): 3
```

**PEP 701 lifted the old restrictions.** Before 3.12 you could not reuse the f-string's own quote
character inside the expression, embed backslashes, write multiline expressions, or put comments
inside the braces. All of that now works (`f"{val["k"]}"` no longer needs a different quote). The
trade-off to remember is the same as always: f-strings format *eagerly*, so do not use them where
formatting should be deferred (logging, §12.4).

---

## 12.2 Variable annotations (PEP 526)

**Why this exists.** PEP 3107 (Python 3.0) allowed annotations on *function parameters and return
values*; **PEP 526** extended the syntax to *any* binding — module, class, and local variables —
giving static type checkers a place to read types and the typing ecosystem (mypy, pyright,
dataclasses, pydantic) its foundation.

**The static boundary — annotations are metadata, not checks.** The interpreter never type-checks
an annotation. Where it *stores* them depends on scope:

- **Module/class level**: the compiler emits `SETUP_ANNOTATIONS` and records each annotation in an
  `__annotations__` dict — they are introspectable at runtime.
- **Function-local level**: annotations are **discarded entirely** — no `__annotations__`, no
  storage, zero runtime cost — because locals are executed on hot paths.

```python
# Caption: class annotations are stored; function-local annotations are stripped.
import dis

class Profile:
    name: str = "Anon"
    age: int                      # annotation only, no value

print("Profile.__annotations__:", Profile.__annotations__)

def process():
    y: int = 42                   # the ': int' is discarded by the compiler
    return y
dis.dis(process)
```

Verified output (CPython 3.13.5):

```text
Profile.__annotations__: {'name': <class 'str'>, 'age': <class 'int'>}
 26   LOAD_CONST  1 (42)
      STORE_FAST  0 (y)
 27   LOAD_FAST   0 (y)
      RETURN_VALUE
```

The class keeps `{'name': str, 'age': int}`; the function body is just `LOAD_CONST`/`STORE_FAST` —
the `: int` left no trace.

**Forward references.** Because class/module annotations are *evaluated* at definition time, naming
a type that does not yet exist raises `NameError`. The classic case is a self-referential class:

```python
# Caption: an unresolved forward reference raises at class-definition time; quote it to defer.
try:
    class Node:
        parent: Node              # Node is not bound yet during its own body
except NameError as e:
    print("NameError:", e)

class NodeOK:
    parent: "NodeOK"              # string forward reference — stored unevaluated
print("string forward ref:", NodeOK.__annotations__)
```

Verified output (CPython 3.13.5):

```text
NameError: name 'Node' is not defined
string forward ref: {'parent': 'NodeOK'}
```

Frameworks resolve string annotations later with `typing.get_type_hints()` / `inspect.get_type_hints()`,
which evaluate them in the right namespace. Two evolutions follow this chapter: **PEP 563**
(`from __future__ import annotations`, stringizing *all* annotations) and **PEP 649** (lazy,
on-demand annotation evaluation, becoming the default in 3.14) — both covered in Vol V. The type
*system* (variance, protocols, narrowing, mypy/pyright) is Vol XV.

---

## 12.3 The compact, ordered dictionary

**Why this exists.** `dict` is the most-used container in Python and the backbone of every
namespace, object `__dict__`, and keyword-args bundle, so its memory layout matters enormously.
Before 3.6, a dict was *one* sparse hash table of 24-byte `PyDictKeyEntry` slots kept ≤2/3 full —
so a dict held large numbers of empty 24-byte slots, wasting memory and scattering entries across
cache lines.

**The compact design (Raymond Hettinger).** 3.6 split the table in two:

- **`dk_indices`** — a small, sparse array of *integer indices* (1/2/4/8 bytes each depending on
  size), the actual hash table.
- **`dk_entries`** — a *dense*, contiguous array of `(hash, key, value)` entries, appended in
  insertion order.

A lookup hashes into `dk_indices`, reads the small integer there, and indexes into the dense
`dk_entries`. Empty slots now cost 1–8 bytes (in `dk_indices`), not 24, cutting dict memory
**~30–40%**. And because entries are appended densely, **iteration order = insertion order**, for
free:

```python
# Caption: the compact dict preserves insertion order (impl detail in 3.6, guaranteed since 3.7).
d = {}
for k in ["z", "a", "m", "b"]:
    d[k] = 1
print("insertion order:", list(d))
```

Verified output (CPython 3.13.5):

```text
insertion order: ['z', 'a', 'm', 'b']
```

**Insertion order was an implementation detail in 3.6 and a *language guarantee* in 3.7** — code
may now rely on it. Iteration is also faster: traversing the dense array touches contiguous memory
with no empty-slot skipping (cache-friendly). The senior-engineer contrast: this is what Java
provides only via a *separate* `LinkedHashMap`; in Python the *default* `dict` is ordered, and the
ordering costs nothing extra because it falls out of the compact layout.

---

## 12.4 Performance and anti-patterns

- **f-strings are the fastest interpolation** — but they format **eagerly**. In logging, write
  `logging.info("x=%s", x)` (the `%`-args are formatted only if the level is enabled), *not*
  `logging.info(f"x={x}")` (formats unconditionally, even when the log is suppressed).
- **Never f-string untrusted format specs or user data into SQL/HTML/shell.** f-strings are string
  interpolation, not escaping — use parameterized queries / proper escaping.
- **Annotations are not runtime validation.** `x: int = "nope"` runs fine. If you need runtime
  enforcement, use pydantic/`dataclasses` validators or a runtime checker (Vol XV).
- **Function-local annotations cost nothing; class/module ones cost a little.** Don't annotate hot
  module-level constants expecting zero cost — `SETUP_ANNOTATIONS` + `STORE_SUBSCR` run at import.
- **Relying on dict order is now safe** (3.7+) — but don't rely on order for *sets*, which are
  unordered, or assume order survives a round-trip through an unordered structure.

---

## 12.5 Summary and cross-references

- **f-strings** (PEP 498) interpolate at **compile time**; 3.13 emits `CONVERT_VALUE`/
  `FORMAT_SIMPLE`/`FORMAT_WITH_SPEC`/`BUILD_STRING`. **PEP 701 (3.12)** removed the nesting,
  multiline, backslash, and comment restrictions. They format eagerly — avoid in logging.
- **Variable annotations** (PEP 526) are **metadata, not checks**: stored in `__annotations__` at
  module/class scope, **stripped** at function scope. Forward references need quoting (or PEP
  563/649, Vol V).
- The **compact dict** (3.6) splits a sparse index array from a dense entry array, cutting memory
  ~30–40% and making **insertion order** intrinsic — an implementation detail in 3.6, a
  **guarantee since 3.7**.

**Cross-references.** Old string/`%`/`.format` mechanics and the `string` module → Vol XI. Deferred
annotations (PEP 563 / PEP 649) → Vol V. The type system in depth (mypy/pyright, protocols,
narrowing) → Vol XV. Dataclasses (which build on annotations) → Chapter 13. CPython dict internals
in full → Vol XII. f-string codegen and the PEG grammar → Vol X.

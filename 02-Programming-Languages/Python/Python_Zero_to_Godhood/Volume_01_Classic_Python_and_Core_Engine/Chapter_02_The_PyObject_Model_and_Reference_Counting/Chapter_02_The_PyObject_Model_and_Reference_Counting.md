# Chapter 2: The PyObject Model and Reference Counting (Python 1.x)

Chapter 1 established that a Python program is a graph of heap objects and that names are
bindings into it. This chapter dissects the node of that graph. **Every Python value is a
heap-allocated C struct that begins with the same header: a reference count and a type
pointer.** From those two fields flow polymorphism (the type pointer), automatic memory
management (the count), and the entire object protocol. We also confront the two facts that
most surprise engineers measuring Python in 2024: an integer costs 28 bytes, and `None`'s
reference count is over four billion and never changes. Both follow directly from the model.

## Section Index
- **2.1** The universal object header: `PyObject` and `PyVarObject`
- **2.2** The type object and the slot system
- **2.3** Reference counting: the lifecycle, and what `Py_INCREF`/`Py_DECREF` really do
- **2.4** Immortal objects (PEP 683) and what `getrefcount` now reports
- **2.5** What reference counting cannot do: cycles
- **2.6** Caching, constant deduplication, and interning — three different mechanisms
- **2.7** Performance and memory: the cost of universal boxing
- **2.8** Anti-patterns and the senior-engineer contrast
- **2.9** Summary and cross-references

---

## 2.1 The universal object header: `PyObject` and `PyVarObject`

**Why this exists.** For the interpreter to treat every value uniformly — store it in a
list, pass it to a function, garbage-collect it, print it — every value must expose the
same minimal interface. CPython achieves this by prefixing *every* object with a common
header. That header is the contract that makes "everything is an object" mechanically true.

```c
/* Illustrative; Include/object.h. PyObject is the base of every Python value. */
typedef struct _object {
    _PyObject_HEAD_EXTRA          /* debug-only doubly-linked list (Py_TRACE_REFS builds) */
    Py_ssize_t  ob_refcnt;        /* reference count */
    PyTypeObject *ob_type;        /* pointer to the type that defines this object's behavior */
} PyObject;

/* Variable-length objects (list, tuple, str, int...) add a count of their items. */
typedef struct {
    PyObject    ob_base;
    Py_ssize_t  ob_size;          /* number of items in the variable part */
} PyVarObject;
```

Two fields, and everything else is built on them: `ob_refcnt` drives memory management
(§2.3), and `ob_type` drives behavior (§2.2). A fixed-size object (like a `float`) embeds
`PyObject` directly; a variable-size object (like a `list` or an arbitrary-precision `int`)
embeds `PyVarObject` and stores its payload after the header.

**The senior-engineer contrast.** In C++ an object's layout is fixed by its static type at
compile time, and virtual dispatch goes through a per-class **vtable** pointer. CPython
inverts this:

| | C++ | CPython |
|---|---|---|
| What a value's size depends on | its static type, known at compile time | its runtime type; all *references* are one pointer wide |
| A "variable" holds | the object (by value) or a typed pointer | a `PyObject *` — always 8 bytes on 64-bit, regardless of what it points to |
| Dispatch | vtable pointer per object (for virtual methods only) | `ob_type` pointer per object; *all* operations dispatch through it |
| Identity of type | compile-time | a first-class runtime object (`type(x)` is itself a `PyObject`) |

```text
name x ──▶ ┌─────────────────────────────────────┐  heap
           │ (debug link fields, debug builds)    │
           │ ob_refcnt : 1                        │
           │ ob_type   : ───▶ PyTypeObject 'int'  │
           │ <payload: the integer's digits>      │
           └─────────────────────────────────────┘
```

Because all references are pointer-width, a `list` of a million integers is a million
pointers (8 MB) pointing at a million separate 28-byte integer objects scattered on the
heap — not a packed array of machine words. That single fact is why `array`, `numpy`, and
the buffer protocol (Vol IX) exist, and §2.7 quantifies the cost.

---

## 2.2 The type object and the slot system

**Why this exists.** `ob_type` points to a `PyTypeObject` — itself an object — that holds
the function pointers ("slots") the interpreter calls to make an object *do* things. When
you write `a + b`, the interpreter does not look for a method named `__add__` by string at
that moment; it reads `a`'s type's `tp_as_number->nb_add` slot, a direct C function
pointer. The dunder protocol you write in Python is the *source*; the slots are the
*compiled mechanism*.

```c
/* Illustrative; Include/cpython/object.h. PyTypeObject is itself a PyVarObject. */
struct _typeobject {
    PyObject_VAR_HEAD
    const char *tp_name;                /* "module.Class" for repr/error messages */
    Py_ssize_t  tp_basicsize, tp_itemsize;   /* allocation sizes */

    destructor  tp_dealloc;             /* called when refcount hits 0 (§2.3) */
    reprfunc    tp_repr;                /* __repr__ */
    hashfunc    tp_hash;               /* __hash__ */
    /* construction */
    newfunc     tp_new;                /* __new__  : allocate */
    initproc    tp_init;               /* __init__ : initialize */
    allocfunc   tp_alloc;              /* low-level memory allocation */
    /* grouped protocol tables (NULL if the type doesn't implement that protocol) */
    PyNumberMethods   *tp_as_number;   /* nb_add, nb_multiply, ... */
    PySequenceMethods *tp_as_sequence; /* sq_item, sq_length, ... */
    PyMappingMethods  *tp_as_mapping;  /* mp_subscript, mp_ass_subscript, ... */
};
```

**Dunder-to-slot mapping.** When a class is created, CPython wires each dunder method you
define into the corresponding slot:

| Python dunder | C slot |
|---|---|
| `__init__` | `tp_init` |
| `__new__` | `tp_new` |
| `__repr__` | `tp_repr` |
| `__hash__` | `tp_hash` |
| `__del__` | `tp_finalize` (not `tp_dealloc`) |
| `__add__` | `tp_as_number->nb_add` |
| `__getitem__` | `tp_as_mapping->mp_subscript` (or `tp_as_sequence->sq_item`) |
| `__len__` | `tp_as_sequence->sq_length` (`sq_length` / `mp_length`) |

```python
# Caption: defining __hash__ fills the tp_hash slot; the builtin hash() calls it.
class CustomObject:
    def __init__(self, val):
        self.val = val
    def __hash__(self):          # -> tp_hash
        return hash(self.val)

print("hash matches delegate:", hash(CustomObject("test")) == hash("test"))
```

Verified output (CPython 3.13.5):

```text
hash matches delegate: True
```

The grouping of numeric/sequence/mapping methods into sub-tables (`tp_as_number`, etc.) is
a space optimization: a type that is not a number leaves `tp_as_number` as `NULL` rather
than carrying dozens of unused pointers. The full slot system, the descriptor protocol, and
how `type` itself is built are the subject of Vol VIII; here we only need that **behavior
lives on the type, reached through `ob_type`.**

---

## 2.3 Reference counting: the lifecycle, and what `Py_INCREF`/`Py_DECREF` really do

**Why this exists.** CPython's primary memory manager is **reference counting**: each object
counts how many references point at it; when the count reaches zero the object is freed
*immediately and deterministically*. This is a deliberate contrast to the tracing
collectors of Java, Go, and the CLR, and it has concrete consequences (deterministic
finalization; no stop-the-world pause for acyclic garbage; but per-operation counting
overhead and a hard problem with cycles, §2.5).

Every reference gained calls `Py_INCREF`; every reference lost calls `Py_DECREF`. The
NULL-safe variants `Py_XINCREF`/`Py_XDECREF` guard against null pointers. When a decref
brings the count to zero, the **deallocation pipeline** runs:

1. The interpreter calls `obj->ob_type->tp_dealloc(obj)`.
2. The deallocator **decrefs every object this object referenced** — freeing a list decrefs
   each element; freeing a dict decrefs every key and value. This is a recursive,
   depth-first cascade.
3. The raw memory is returned to the allocator (PyMalloc — Vol VIII).

You can watch the count move from Python, with one caveat: passing the object to
`getrefcount` itself creates one transient reference, so subtract 1.

```python
# Caption: reference counting is observable; subtract 1 for getrefcount's own argument ref.
import sys

my_list = [100, 200]
print("getrefcount-1 at start:", sys.getrefcount(my_list) - 1)
alias = my_list
print("after one alias:", sys.getrefcount(my_list) - 1)
del alias
print("after del alias:", sys.getrefcount(my_list) - 1)
```

Verified output (CPython 3.13.5):

```text
getrefcount-1 at start: 1
after one alias: 2
after del alias: 1
```

`del alias` does **not** delete the list; it removes one *binding*. The object dies only
when the last reference goes away. This is the operational meaning of "names are bindings"
from Chapter 1.

> **Implementation detail, not language semantics.** Reference counting is a CPython
> mechanism. PyPy, GraalPy, and Jython use tracing collectors and do **not** refcount;
> code that relies on a `__del__` firing at a precise moment is relying on CPython, not on
> Python. Use `with`/context managers for deterministic resource release (Chapter 5).

---

## 2.4 Immortal objects (PEP 683) and what `getrefcount` now reports

**Why this exists.** A handful of objects — `None`, `True`, `False`, the small integers,
the empty `str`/`tuple`, interned identifiers, type objects — are referenced from
everywhere and live for the entire process. Counting and uncounting their references is pure
overhead, and in a **free-threaded** build (Vol VII) every such refcount write is a
contended atomic operation on a cache line shared by all threads. **PEP 683 (Python 3.12)**
made these objects **immortal**: their reference count is pinned to a sentinel value and
`Py_INCREF`/`Py_DECREF` skip them entirely.

```python
# Caption: immortal objects (3.12+) report a pinned, enormous refcount that never changes.
import sys
print("refcount(None):", sys.getrefcount(None))
print("as hex:", hex(sys.getrefcount(None)))
```

Verified output (CPython 3.13.5):

```text
refcount(None): 4294967295
as hex: 0xffffffff
```

That `0xffffffff` is the immortality sentinel on this build, not a real count. The practical
upshots: never write code that branches on the *magnitude* of a refcount; the modern
`Py_INCREF` macro is no longer a bare `ob_refcnt++` but first checks for the immortal
sentinel; and the per-object overhead this removes is exactly what makes the free-threaded
build's shared-object access viable (Vol VII, Ch 20).

---

## 2.5 What reference counting cannot do: cycles

Reference counting has one fatal blind spot: a **reference cycle** keeps its own counts
above zero even when nothing outside the cycle can reach it.

```python
# Caption: a self-referential structure that pure refcounting can never free.
a = {}
b = {}
a["b"] = b
b["a"] = a          # a -> b -> a : each holds the other; neither count reaches 0
del a, b            # bindings gone, but the two dicts still reference each other
```

After `del a, b`, the two dictionaries are unreachable yet each still has refcount 1 (from
the other). Pure reference counting would leak them forever. CPython's answer is a separate,
optional **cyclic garbage collector** (the `gc` module) that periodically finds and
reclaims such islands. That collector — its generations, thresholds, and the
`tp_traverse`/`tp_clear` protocol — is the subject of Chapter 3, where it enters the story
chronologically with Python 2.0. For now, hold the division of labor: **refcounting frees
acyclic garbage instantly; the cyclic collector cleans up the rest.**

---

## 2.6 Caching, constant deduplication, and interning — three different mechanisms

These three are routinely conflated, and the confusion produces the single most common
piece of Python misinformation: "`257 is 257` is `False`." On a real interpreter run as a
script, **it is `True`.** There are *three distinct mechanisms* at work, and separating
them is the whole point of this section.

### 2.6.1 The small-integer cache (a runtime singleton pool)

CPython preallocates the integers from **−5 to 256** as shared singletons
(`NSMALLNEGINTS`/`NSMALLPOSINTS`). *Any* expression producing a value in that range returns
the same object, so identity holds even across separate code objects.

### 2.6.2 Constant deduplication (a compile-time, per-code-object effect)

When the compiler builds a code object, it stores each distinct literal once in
`co_consts` and **deduplicates equal constants**. So two `257` literals *in the same
compilation unit* resolve to the same constant — hence the same object — even though 257 is
outside the small-int cache. This is not interning; it is the compiler not storing the same
constant twice.

### 2.6.3 String interning (a runtime global table)

Identifier-like string literals are added to a process-global **intern table**, so equal
identifier-like literals share one object. Strings built at runtime are *not* automatically
interned; `sys.intern()` forces it.

```python
# Caption: three mechanisms, demonstrated and distinguished.
import sys

a = 256
b = 256
print("256 cached (small-int pool):", a is b)

c = 257
d = 257
print("257 in one code object (constant dedup):", c is d)

g1 = {}; exec("x = 257", g1)
g2 = {}; exec("x = 257", g2)
print("257 across two compilation units:", g1["x"] is g2["x"])

s1 = "godhood"
s2 = "godhood"
print("identifier-like literal interned:", s1 is s2)

r2 = "".join(["god", "hood"])      # built at runtime
print("runtime-built string auto-interned:", r2 is s1)
print("sys.intern() forces sharing:", sys.intern(r2) is s1)

t1 = ()
t2 = tuple()
print("empty tuple singleton:", t1 is t2)
```

Verified output (CPython 3.13.5):

```text
256 cached (small-int pool): True
257 in one code object (constant dedup): True
257 across two compilation units: False
identifier-like literal interned: True
runtime-built string auto-interned: False
sys.intern() forces sharing: True
empty tuple singleton: True
```

The corrected mental model: `257 is 257` is `True` in a script (constant dedup) and `False`
only when the two `257`s come from different compilation units — which is exactly what the
interactive REPL does, compiling each line separately. That is where the folklore comes
from. The empty tuple and the singletons `None`/`True`/`False`/`Ellipsis`/`NotImplemented`
are statically allocated and unique by construction.

> **Anti-pattern.** Never use `is` to compare values (numbers, strings). `is` is identity;
> its results depend on caching, dedup, and interning, none of which are language
> guarantees for general values. Use `==`. Reserve `is` for genuine singletons (`is None`,
> `is True` when you mean the object, sentinels). Modern CPython will even emit a
> `SyntaxWarning` when it sees `is` applied directly to a literal.

---

## 2.7 Performance and memory: the cost of universal boxing

Universal boxing — every value a separate heap object behind a pointer — is what makes the
data model uniform, and it is also Python's dominant performance tax. The numbers are
concrete:

```python
# Caption: the memory cost of boxed objects (bytes), CPython 3.13.5, 64-bit.
import sys
for label, obj in [("int 0", 0), ("int 2**30", 2**30), ("int 2**100", 2**100),
                   ("list []", []), ("list [1,2,3]", [1, 2, 3]),
                   ("tuple ()", ()), ("str ''", ""), ("str 'a'", "a")]:
    print(f"{label:14s} {sys.getsizeof(obj):3d} bytes")
```

Verified output (CPython 3.13.5):

```text
int 0           28 bytes
int 2**30       32 bytes
int 2**100      40 bytes
list []         56 bytes
list [1,2,3]    88 bytes
tuple ()        40 bytes
str ''          41 bytes
str 'a'         42 bytes
```

Read these consequences off the numbers:

- **An integer is 28 bytes**, not 8. A Python `int` is a `PyVarObject` with a header plus
  arbitrary-precision digits; it grows with magnitude (`2**100` needs 40 bytes). A C `int64`
  is 8 bytes. A list of a million ints is ~8 MB of pointers *plus* ~28 MB of integer
  objects, versus 8 MB for a C array. This 4–5× factor, *plus* the loss of cache locality
  from chasing scattered pointers, is the entire motivation for `array.array`, `numpy`
  arrays, and the buffer protocol (Vol IX).
- **Refcount churn is real CPU cost.** Every bind, every argument pass, every list append
  touches a refcount. In the single-threaded GIL build these are plain increments; in the
  free-threaded build they become atomics — which is why immortalization (§2.4) and biased
  reference counting matter (Vol VII).
- **Identity caching saves allocations** for the hot, tiny objects (small ints, interned
  identifiers, singletons), which is why the model is viable at all despite per-object
  overhead.

---

## 2.8 Anti-patterns and the senior-engineer contrast

- **`is` for value comparison** (covered in §2.6): a latent bug that "works" until the value
  leaves the cached range or the REPL. Use `==`.
- **Relying on `__del__` for cleanup**: refcount-driven finalization is a CPython detail and
  is *not* guaranteed promptly (and never runs for cycle members until the cyclic collector
  acts). For files, sockets, locks — anything with an OS resource — use a context manager.
  This is the closest Python has to C++ RAII, and Chapter 5 builds it properly.
- **Assuming value semantics on assignment** (the C++ reflex): `b = a` aliases; it does not
  copy. Mutating through `b` is visible through `a`. Reach for `copy.copy`/`copy.deepcopy`
  when you need independence.
- **Treating `sys.getsizeof` as deep size**: it returns the object's own size, *not* the
  size of objects it references. `getsizeof([big_list])` is ~64 bytes regardless of contents.

---

## 2.9 Summary and cross-references

- Every value is a heap object beginning with a shared header: **`ob_refcnt`** (memory) and
  **`ob_type`** (behavior). Variable-length objects add `ob_size`.
- Behavior lives on the **type object** as C function-pointer **slots**; the dunder methods
  you write fill those slots.
- **Reference counting** frees acyclic garbage deterministically and immediately, at the
  cost of per-operation counting and an inability to reclaim **cycles** (→ Chapter 3).
- **PEP 683 immortal objects** (3.12+) pin the refcount of process-global objects;
  `getrefcount(None)` reports `0xffffffff`, not a real count.
- Identity surprises come from **three separate mechanisms** — the small-int cache,
  compile-time constant deduplication, and string interning — not one. `257 is 257` is
  `True` in a script. Compare values with `==`, never `is`.
- Universal boxing makes an `int` cost 28 bytes and destroys array locality, which is the
  root motivation for the numeric/zero-copy machinery in Vol IX.

**Cross-references.** The cyclic garbage collector → Chapter 3. Context managers and
deterministic cleanup → Chapter 5. The full type-slot and descriptor machinery → Vol VIII.
The free-threaded refcounting model (biased refcounting, atomics, immortalization) →
Vol VII, Ch 20. `array`, `memoryview`, and zero-copy numerics → Vol IX.

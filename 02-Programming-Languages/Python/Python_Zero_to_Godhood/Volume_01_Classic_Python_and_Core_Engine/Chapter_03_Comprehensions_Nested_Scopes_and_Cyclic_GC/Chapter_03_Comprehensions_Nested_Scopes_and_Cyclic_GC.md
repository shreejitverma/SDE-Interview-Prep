# Chapter 3: Comprehensions, Nested Scopes, and Cyclic GC (Python 2.0–2.1)

Python 2.0–2.1 added three features whose interactions define how names live and die in
modern Python: **list comprehensions**, **lexically nested scopes** (PEP 227), and the
**cyclic garbage collector**. Each looks like a convenience and is in fact a deep statement
about the execution model. Comprehensions force the question "what is a scope, mechanically?"
Nested scopes complete the LEGB lookup the compiler from Chapter 1 was already preparing for.
And the cyclic collector exists for exactly one reason: reference counting (Chapter 2) cannot
free a cycle. We treat all three at their modern depth, including the changes — PEP 709
comprehension inlining (3.12) and PEP 442 finalization order — that make the original 2.x
descriptions obsolete.

## Section Index
- **3.1** List comprehensions: the scope-leak bug, the fix, and PEP 709 inlining
- **3.2** Nested scopes and LEGB resolution (PEP 227)
- **3.3** Closures and cell objects, and the late-binding trap
- **3.4** The cyclic garbage collector
- **3.5** Performance and anti-patterns
- **3.6** Summary and cross-references

---

## 3.1 List comprehensions: the scope-leak bug, the fix, and PEP 709 inlining

**Why this exists.** A list comprehension is sugar for "build a list by iterating," but
*where the loop variable lives* turned out to be a subtle, evolving design decision that
touches frames, isolation, and performance.

**The original bug (Python 2.x).** Python 2.0 compiled a comprehension as an inline loop in
the enclosing function — `STORE_FAST` wrote the loop variable straight into the host's
locals. The loop variable **leaked**:

```python
# Python 2.x behaviour (historical — not how Python 3 works):
x = 999
data = [x for x in range(5)]
print(x)   # -> 4 in Python 2: the comprehension clobbered the outer x
```

**The fix (Python 3.0): isolation via a hidden function scope.** Python 3.0 gave each list,
set, and dict comprehension its own scope by compiling it as an implicit nested function with
its own frame. The loop variable became local to that hidden function and could no longer
leak:

```python
# Caption: in Python 3 the comprehension's loop variable is isolated.
x = 999
data = [x for x in range(5)]
print("x after comprehension:", x, "| data:", data)
```

Verified output (CPython 3.13.5):

```text
x after comprehension: 999 | data: [0, 1, 2, 3, 4]
```

**The modern reality (Python 3.12, PEP 709): isolation *without* a frame.** Creating and
tearing down a whole function frame per comprehension was pure overhead. **PEP 709** inlined
list/set/dict comprehensions back into the enclosing code object in Python 3.12 — but
preserved isolation by *saving and restoring* the loop variable around the comprehension
rather than by creating a scope. You can prove the frame is gone: an inlined comprehension
leaves **no nested code object** behind, whereas a generator expression (which must suspend,
so it genuinely needs its own frame) still does:

```python
# Caption: PEP 709 — list comprehensions are inlined (no nested code object);
# generator expressions still get their own code object.
def make_list(n):
    return [i * i for i in range(n)]

def make_gen(n):
    return (i * i for i in range(n))

list_nested = [c.co_name for c in make_list.__code__.co_consts if hasattr(c, "co_name")]
gen_nested = [c.co_name for c in make_gen.__code__.co_consts if hasattr(c, "co_name")]
print("nested code objects in make_list:", list_nested)
print("nested code objects in make_gen: ", gen_nested)
```

Verified output (CPython 3.13.5):

```text
nested code objects in make_list: []
nested code objects in make_gen:  ['<genexpr>']
```

The inlined bytecode reveals the save/restore mechanism — note `LOAD_FAST_AND_CLEAR` (stash
the outer `i` and blank it) on entry and the matching restore in an exception table so the
outer name survives even if the comprehension raises:

```text
# dis.dis of [i*i for i in range(n)] on CPython 3.13.5 (excerpt)
LOAD_GLOBAL              1 (range + NULL)
LOAD_FAST                0 (n)
CALL                     1
GET_ITER
LOAD_FAST_AND_CLEAR      1 (i)      # save outer i, clear the slot for isolation
SWAP                     2
BUILD_LIST               0
...
FOR_ITER  -> STORE_FAST_LOAD_FAST (i, i); LOAD_FAST i; BINARY_OP 5 (*); LIST_APPEND 2
...
SWAP 2; STORE_FAST 1 (i)           # restore outer i
RETURN_VALUE
```

**Old-vs-new, three eras.** 2.x: inline, leaks. 3.0–3.11: hidden function frame, isolated
but slower. 3.12+: inlined with save/restore, isolated *and* fast. The user-visible
semantics (isolation) has held since 3.0; only the mechanism changed. The lasting lesson:
**a comprehension's loop variable does not leak, but for a reason that has been re-engineered
twice.**

---

## 3.2 Nested scopes and LEGB resolution (PEP 227)

**Why this exists.** Before Python 2.1, name lookup was three-tier — **LGB** (Local, Global,
Built-in). A nested function could *not* see its enclosing function's locals, so this raised
`NameError`:

```python
# Pre-2.1 behaviour (historical):
def outer():
    x = 10
    def inner():
        return x        # NameError before PEP 227: x was neither local, global, nor builtin
    return inner
```

The workaround was to smuggle the value through a default argument: `def inner(x=x):`. **PEP
227** (optional in 2.1 via `from __future__ import nested_scopes`, standard in 2.2) inserted
the **Enclosing** scope, giving the modern **LEGB** order:

| Tier | Searches | Opcode |
|---|---|---|
| **L**ocal | the active frame's fast-locals array | `LOAD_FAST` |
| **E**nclosing | cells captured from outer functions | `LOAD_DEREF` |
| **G**lobal | the module `__dict__` | `LOAD_GLOBAL` |
| **B**uilt-in | the `builtins` namespace | `LOAD_GLOBAL` (falls through after globals) |

Crucially, **LEGB is resolved statically by the compiler** (Chapter 1's symbol-table pass),
not by searching dictionaries at runtime. The opcode chosen *is* the lookup decision: a local
is `LOAD_FAST` (array index), a free variable is `LOAD_DEREF` (cell dereference), a global is
`LOAD_GLOBAL`. Only module-top-level code and dynamic scopes fall back to the dictionary-based
`LOAD_NAME`.

```python
# Caption: the compiler bakes the scope decision into the opcode.
import dis

def fast_local_demo(x):
    y = x + 1
    return y

dis.dis(fast_local_demo)
```

Verified output (CPython 3.13.5):

```text
  1           RESUME                   0

  2           LOAD_FAST                0 (x)
              LOAD_CONST               1 (1)
              BINARY_OP                0 (+)
              STORE_FAST               1 (y)

  3           LOAD_FAST                1 (y)
              RETURN_VALUE
```

Both `x` and `y` are `LOAD_FAST`/`STORE_FAST` — array indexing, no dictionary involved.
(`BINARY_OP 0 (+)` is the unified 3.11 add from Chapter 1, not the pre-3.11 `BINARY_ADD`.)

---

## 3.3 Closures and cell objects, and the late-binding trap

**Why this exists.** When `inner` reads `outer`'s local `x`, that local must outlive `outer`'s
frame. Chapter 1 introduced the solution: the compiler promotes `x` to a **cell**
(`PyCellObject`) — a heap box shared by reference between the two functions. Here we make the
mechanism observable and expose the bug it causes.

```python
# Caption: closure cells are live, shared boxes — inspectable at runtime.
def outer_scope(multiplier):
    secret_value = 100
    def inner_scope(val):
        return val * multiplier + secret_value   # two free variables
    return inner_scope

fn = outer_scope(5)
print("closure cells:", [c.cell_contents for c in fn.__closure__])
print("inner co_freevars:", fn.__code__.co_freevars)
print("outer co_cellvars:", outer_scope.__code__.co_cellvars)
```

Verified output (CPython 3.13.5):

```text
closure cells: [5, 100]
inner co_freevars: ('multiplier', 'secret_value')
outer co_cellvars: ('multiplier', 'secret_value')
```

The compiler emits `LOAD_CLOSURE` + `MAKE_FUNCTION` in the enclosing function to bundle the
cells into the new function's `__closure__`, and `LOAD_DEREF` inside the closure to read
through a cell.

**The late-binding trap.** Because a cell holds a *reference to a live variable*, not a
snapshot, closures created in a loop all share the *same* cell and therefore all see the
loop variable's **final** value:

```python
# Caption: the classic late-binding closure bug, and the standard fix.
funcs = [lambda: i for i in range(3)]
print("late-binding (all see final i):", [f() for f in funcs])

funcs_fixed = [lambda i=i: i for i in range(3)]   # capture by default-arg value
print("default-arg capture fix:    ", [f() for f in funcs_fixed])
```

Verified output (CPython 3.13.5):

```text
late-binding (all see final i): [2, 2, 2]
default-arg capture fix:     [0, 1, 2]
```

This trips up every engineer at least once. The fix uses the fact that **default arguments
are evaluated once, at function-definition time** (Chapter 5): `i=i` snapshots the current
value into the parameter's default, decoupling it from the shared cell. The contrast with C++
is instructive: a C++ lambda forces you to *choose* capture-by-value (`[i]`) or
capture-by-reference (`[&i]`); Python only has capture-by-reference (the cell), and `i=i` is
how you simulate capture-by-value.

---

## 3.4 The cyclic garbage collector

**Why this exists.** Chapter 2 showed that reference counting cannot reclaim a **cycle**:
each object in the cycle keeps the others' counts above zero. Python 2.0 added a separate,
**generational, cycle-detecting collector** (the `gc` module) layered on top of reference
counting. Refcounting still does the bulk of the work and frees acyclic garbage instantly;
the cyclic collector runs periodically to find and break unreachable cycles.

**What it tracks.** Only **container** objects can form cycles (lists, dicts, sets, tuples,
instances of Python classes). Atomic objects (`int`, `str`, `float`) cannot reference other
objects and are never tracked. Tracked objects carry a `PyGC_Head` header that links them
into the collector's doubly-linked lists.

**The detection algorithm** (per collected generation):

1. **Snapshot.** Copy each tracked object's real refcount into a private `gc_refs` field.
2. **Subtract internal references.** Traverse every tracked object's references
   (`tp_traverse`); for each reference to another object *in the set*, decrement that
   target's `gc_refs`. After this pass, `gc_refs` counts only references from **outside** the
   set.
3. **Partition.** Objects with `gc_refs > 0` are reachable from outside (roots); they and
   everything they reach are **live**. Objects still at `gc_refs == 0` are unreachable
   garbage.
4. **Reclaim.** Break the garbage cycles (`tp_clear`) and free them.

**Generations and the weak generational hypothesis.** Objects are grouped into three
generations. New objects start in generation 0; survivors are promoted. The collector scans
generation 0 frequently and the older generations rarely, betting that *most objects die
young* — so most collections are cheap.

```python
# Caption: a cycle survives refcounting and is reclaimed only by gc.collect();
# objects with __del__ in a cycle ARE collected (PEP 442, Python 3.4+).
import gc, sys

class Node:
    def __init__(self, name):
        self.name = name
        self.ref = None
    def __del__(self):
        print(f"  __del__ {self.name}")

gc.collect()                       # clean slate
a = Node("A"); b = Node("B")
a.ref = b; b.ref = a               # build the cycle
print("refcount(A) in cycle:", sys.getrefcount(a) - 1)
del a, b                           # bindings gone; cycle keeps both alive
print("after del; running gc.collect() ...")
reclaimed = gc.collect()
print("objects reclaimed:", reclaimed)
print("default threshold:", gc.get_threshold())
```

Verified output (CPython 3.13.5):

```text
refcount(A) in cycle: 2
after del; running gc.collect() ...
  __del__ A
  __del__ B
objects reclaimed: 2
default threshold: (2000, 10, 10)
```

Two modern facts the original 2.x description predates:

- **Finalizers in cycles are now run (PEP 442, Python 3.4+).** Before 3.4, an object with a
  `__del__` method inside a cycle was deemed *uncollectable* and parked in `gc.garbage`
  forever. PEP 442 reworked finalization so cyclic objects are finalized then collected — as
  the output above shows, both `__del__`s fire.
- **The generation-0 threshold on this interpreter is 2000**, read via `gc.get_threshold()`.
  The threshold is a tunable triple `(t0, t1, t2)`: a collection of generation 0 is triggered
  when (allocations − deallocations) since the last collection exceeds `t0`; generation 1 is
  collected after `t1` collections of generation 0, and generation 2 after `t2` collections of
  generation 1. The values are build- and version-dependent and adjustable via
  `gc.set_threshold()`; do not hard-code the assumption that gen-0 fires at a particular
  count.

---

## 3.5 Performance and anti-patterns

**The collector has a cost, and you can control it.** Tracing is *O*(tracked objects in the
scanned generations). In allocation-heavy or latency-sensitive code (the trading loop of
Vol IX, request handlers, large batch loads), GC pauses are real:

- `gc.disable()` / `gc.enable()` — turn off automatic collection in a hot phase; collect
  explicitly at a safe point. Refcounting still reclaims all acyclic garbage while disabled,
  so this leaks only genuine cycles, briefly.
- `gc.freeze()` (3.7+) — move everything currently alive into a permanent generation that is
  never scanned. The canonical use is right after import/startup and before forking a server:
  it keeps long-lived objects out of every future scan and improves copy-on-write sharing
  across forked workers.
- **Avoid creating cycles you don't need.** Parent↔child back-references are the usual
  culprit; `weakref` (Vol VIII) breaks the cycle so plain refcounting can reclaim the object
  immediately, no collector required.

**Anti-patterns.**
- **The late-binding closure in a loop** (§3.3): use `i=i` capture or `functools.partial`.
- **Relying on `gc.garbage`** for finalizer-in-cycle objects: obsolete since PEP 442; design
  finalization to not assume it.
- **Leaking the loop variable** mentally: it does not leak from a comprehension, but a plain
  `for` loop's variable *does* persist after the loop — a real difference, not a bug.
- **Comprehension vs generator expression confusion**: a comprehension is eager and inlined
  (no frame); a generator expression is lazy and keeps its own frame. Reach for the generator
  when you won't consume all results or the sequence is large (Chapter 9 / Vol III).

---

## 3.6 Summary and cross-references

- **Comprehensions** isolate their loop variable (since 3.0), and as of **PEP 709 (3.12)**
  do so *inlined*, with save/restore, no nested frame. Generator expressions still own a
  frame.
- **PEP 227** added the **Enclosing** scope, completing **LEGB**, which the compiler resolves
  *statically* into `LOAD_FAST`/`LOAD_DEREF`/`LOAD_GLOBAL`/`LOAD_NAME`.
- **Closures** capture variables by **cell** (shared, live reference), which is why the
  **late-binding loop** trap exists; `i=i` simulates the capture-by-value Python lacks.
- The **cyclic garbage collector** exists solely to reclaim what refcounting cannot — cycles
  — using a generational copy-and-subtract algorithm; since **PEP 442** it finalizes cyclic
  objects rather than abandoning them.
- The collector is **tunable** (`disable`/`freeze`/`set_threshold`); know these levers for
  latency-sensitive code.

**Cross-references.** The compiler/symbol-table pass that assigns scopes → Chapter 1.
Reference counting and why cycles leak → Chapter 2. Default-argument evaluation timing →
Chapter 5. Generator expressions and lazy iteration → Chapter 9 and Vol III. `weakref`,
`__slots__`, and the allocator/GC internals in full → Vol VIII. GC tuning in a low-latency
context → Vol IX.

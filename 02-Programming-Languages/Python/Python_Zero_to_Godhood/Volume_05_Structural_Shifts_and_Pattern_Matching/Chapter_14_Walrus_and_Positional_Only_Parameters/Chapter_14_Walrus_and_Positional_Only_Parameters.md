# Chapter 14: The Walrus Operator and Positional-Only Parameters (Python 3.8)

Python 3.8 added two syntax features that look small and are quietly significant. The **walrus
operator** `:=` (PEP 572) broke Python's long-standing wall between statements and expressions,
letting you bind a name *inside* an expression. **Positional-only parameters** `/` (PEP 570) gave
function authors the same control over the positional/keyword boundary that C-implemented builtins
always had. Both are about precision: `:=` removes a redundant evaluation or a temporary line, and
`/` lets a library fix its calling contract so callers cannot couple to parameter *names*.

## Section Index
- **14.1** The assignment expression `:=` (PEP 572)
- **14.2** Positional-only parameters `/` (PEP 570)
- **14.3** Performance and anti-patterns
- **14.4** Summary and cross-references

---

## 14.1 The assignment expression `:=` (PEP 572)

**Why this exists.** Python deliberately separated *statements* (assignment `x = 1`, which yields
no value) from *expressions* (which do). That cleanliness forced two common patterns to be clumsy:
computing a value to *test* and then *reuse* required either a pre-loop assignment plus a `while
True`/`break`, or computing the value twice. The walrus operator adds an **assignment
expression** — it binds a name **and** evaluates to the bound value — without abolishing the
statement/expression distinction (it is a separate operator, not a redefinition of `=`).

At the AST level a normal assignment is an `Assign` *statement*; `x := 1` is a `NamedExpr`
*expression*. The bytecode shows the one extra step that makes it an expression — the value is
duplicated on the stack so the binding consumes one copy and the surrounding expression gets the
other:

```python
# Caption: := duplicates the value on the stack (COPY in 3.11+, DUP_TOP pre-3.11).
import dis
dis.dis(compile("(x := 1)", "<s>", "exec"))
```

Verified output (CPython 3.13.5):

```text
  1   LOAD_CONST    0 (1)
      COPY          1            # 3.11+ uses COPY; pre-3.11 used DUP_TOP
      STORE_NAME    0 (x)
      POP_TOP                    # (top-level expr statement discards the residual value)
      RETURN_CONST  1 (None)
```

`COPY 1` replaces the pre-3.11 `DUP_TOP`; the mechanism is identical — push, duplicate, store one
copy, leave the other for the enclosing expression to consume.

**The patterns it is for.** The walrus shines exactly where a value is both tested and used:

```python
# Caption: the two canonical uses — loop-until-sentinel, and filter-and-reuse.
import io

# (a) read-until-empty without a while True / break dance
stream = io.StringIO("aaa\nbbb\nccc\n")
lines = []
while (line := stream.readline()):
    lines.append(line.strip())
print("walrus while:", lines)

# (b) compute once in a comprehension filter, then reuse in the output
def parent_func(data):
    result = [y for x in data if (y := x * 2) > 2]
    return result, y            # y survives: it binds in the *surrounding* scope
print("result, y:", parent_func([1, 2, 3]))
```

Verified output (CPython 3.13.5):

```text
walrus while: ['aaa', 'bbb', 'ccc']
result, y: ([4, 6], 6)
```

**Scoping — the subtle part.** Inside a comprehension, a walrus target **binds in the enclosing
scope, not the comprehension's**. That is why `y` is still visible (and equals the last value, `6`)
after `parent_func`'s comprehension finishes — the comprehension computes `x*2` once per item, uses
it in both the filter and the output, and leaks the final `y` outward by design. Note this hoisting
survived **PEP 709** (3.12) comprehension inlining: the comprehension is now inlined (no nested code
object), yet the walrus still targets the surrounding scope:

```python
# Caption: PEP 709 — the comprehension is inlined; the walrus still binds outward.
nested = [c.co_name for c in parent_func.__code__.co_consts if hasattr(c, "co_name")]
print("nested code objects:", nested, "(empty => inlined)")
```

Verified output (CPython 3.13.5):

```text
nested code objects: [] (empty => inlined)
```

**Restrictions.** To keep this from becoming unreadable, the compiler forbids the dangerous cases.
You cannot rebind a comprehension's own iteration variable:

```python
# Caption: rebinding the comprehension loop variable is a SyntaxError.
try:
    compile("[i for i in range(3) if (i := 2)]", "<s>", "eval")
except SyntaxError as e:
    print("SyntaxError:", str(e).split(" (")[0])
```

Verified output (CPython 3.13.5):

```text
SyntaxError: assignment expression cannot rebind comprehension iteration variable 'i'
```

A walrus is also disallowed at the top level of an expression statement (`x := 1` must be
parenthesized) and inside a class-body comprehension (class namespaces aren't function frames, so
they can't carry the closure cell the hoisting needs). **When not to use it:** if `:=` makes a line
harder to scan, use a plain assignment statement — it exists to remove redundancy, not to win
golf.

---

## 14.2 Positional-only parameters `/` (PEP 570)

**Why this exists.** Before 3.8, pure-Python functions could not express what every C builtin
already did: "these parameters may be passed *only positionally*." That mattered for two reasons.
First, **API stability** — if a parameter can be passed by keyword, its *name* becomes part of your
public contract and you can never rename it without breaking callers. Second, **`**kwargs`
safety** — a function like `dict(**kwargs)` could not accept a key literally named `self` or the
parameter name without collision. The `/` marker fixes both: every parameter *before* `/` is
positional-only.

```python
# Caption: parameters before / are positional-only; after * are keyword-only.
def func(a, b, /, c, d, *, e, f):
    return (a, b, c, d, e, f)

co = func.__code__
print("co_posonlyargcount:", co.co_posonlyargcount,
      "| co_argcount:", co.co_argcount, "| co_kwonlyargcount:", co.co_kwonlyargcount)
print("positional call:", func(1, 2, 3, 4, e=5, f=6))

try:
    func(a=1, b=2, c=3, d=4, e=5, f=6)     # a, b are positional-only
except TypeError as e:
    print("TypeError:", e)
```

Verified output (CPython 3.13.5):

```text
co_posonlyargcount: 2 | co_argcount: 4 | co_kwonlyargcount: 2
positional call: (1, 2, 3, 4, 5, 6)
TypeError: func() got some positional-only arguments passed as keyword arguments: 'a, b'
```

The boundaries are encoded in the code object: `a, b` are positional-only
(`co_posonlyargcount == 2`); `c, d` are positional-or-keyword (so `co_argcount == 4` counts all
four positionals); `e, f` are keyword-only (`co_kwonlyargcount == 2`, after the `*`). Together `/`
and `*` give an author full control over how each parameter may be supplied.

**The performance angle.** When a function is called via the **vectorcall** protocol (PEP 590,
3.8), arguments arrive as a flat C array. For positional-only parameters the interpreter can copy
references straight into the frame's local slots **by index**, skipping the keyword-name
hash-matching that positional-or-keyword parameters require. For small, hot functions this trims a
measurable slice of call overhead — which is exactly why the CPython builtins use positional-only
parameters pervasively. (The full vectorcall calling convention is Vol VIII.)

**When to use it.** Mark a parameter positional-only when its name is an implementation detail you
want freedom to change, when you accept arbitrary `**kwargs` that might collide with a parameter
name, or when matching the signature style of a builtin. Don't over-apply it to ordinary
application code where keyword arguments aid readability.

---

## 14.3 Performance and anti-patterns

- **`:=` is for de-duplication, not density.** Use it to avoid evaluating something twice or to
  fold a sentinel-read loop; do not chain several walruses into one unreadable expression.
- **Mind the comprehension hoist.** A walrus in a comprehension leaks its target to the enclosing
  scope — useful for "keep the last/maximal value," surprising if you expected comprehension-local
  scoping. Name such targets clearly.
- **Positional-only for libraries and hot paths**, where name-stability and call speed matter; for
  ordinary code, keyword arguments are usually clearer.
- **`/` plus `*` is the full toolkit:** `def f(pos_only, /, normal, *, kw_only)`. Reach for the
  markers deliberately to encode the calling contract, not reflexively.

---

## 14.4 Summary and cross-references

- The **walrus operator** `:=` (PEP 572) is an **assignment expression**: it binds a name and
  evaluates to the value (bytecode duplicates via `COPY`). In comprehensions it **binds in the
  enclosing scope** (hoisting survives PEP 709 inlining); rebinding the loop variable or using it
  in a class-body comprehension is a `SyntaxError`.
- **Positional-only parameters** `/` (PEP 570) make pre-`/` parameters un-passable by keyword,
  protecting parameter *names* from becoming API, avoiding `**kwargs` collisions, and enabling
  index-based vectorcall argument copying. `co_posonlyargcount` records the count; misuse raises a
  precise `TypeError`.

**Cross-references.** Comprehension scope and PEP 709 inlining → Chapter 3. Closures and cells (the
hoisting mechanism) → Chapters 1 and 3. The vectorcall calling convention and argument binding
internals → Vol VIII. Keyword-only parameters and full signature design → Vol XV. PEG parser
(which made richer grammar like this practical) → Chapter 15.

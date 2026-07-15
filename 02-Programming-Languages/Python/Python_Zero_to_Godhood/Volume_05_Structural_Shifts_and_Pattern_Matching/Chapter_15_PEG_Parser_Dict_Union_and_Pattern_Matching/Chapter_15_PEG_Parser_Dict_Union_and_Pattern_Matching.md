# Chapter 15: The PEG Parser, Dict Union, and Pattern Matching (Python 3.9–3.10)

The 3.9–3.10 cycle delivered the most visible syntax addition since Python 3.0: **structural
pattern matching** (`match`/`case`, PEP 634). It became possible only because 3.9 replaced the
30-year-old LL(1) parser with a **PEG parser** (PEP 617) capable of the richer grammar. The same
window added the small but daily-useful **dict union operators** `|` and `|=` (PEP 584). This
chapter treats pattern matching as the deep topic — it is *destructuring plus matching*, not a C
`switch` — and is precise about the one feature that bites everyone: a bare name in a pattern
**captures**, it does not compare.

## Section Index
- **15.1** The PEG parser and the syntax it unlocked (PEP 617)
- **15.2** Dictionary union operators `|` and `|=` (PEP 584)
- **15.3** Structural pattern matching (PEP 634)
- **15.4** The capture-vs-value gotcha, performance, and anti-patterns
- **15.5** Summary and cross-references

---

## 15.1 The PEG parser and the syntax it unlocked (PEP 617)

Chapter 1 covered the LL(1)→PEG transition in depth; here is what it *bought* the language. The
**PEG** parser (PEP 617, Python 3.9) brings **ordered choice** (`e1 / e2` — first match wins,
deterministically) and **unlimited lookahead** with **packrat memoization** (caching each
(rule, position) result keeps parsing linear-time). Freed from one-token lookahead, the grammar
could express constructs the old parser could not — most visibly the **parenthesized context
managers** of 3.10:

```python
# Caption: parenthesized, multi-line context managers — only parseable with PEG lookahead.
with (
    open("a.txt") as a,
    open("b.txt") as b,
):
    ...
```

The old parser could not tell this apart from a parenthesized tuple without arbitrary lookahead.
The PEG rewrite also improved error messages (it knows what it expected at each position) and is
what made the `match` grammar (§15.3) feasible. Parser internals and the formal grammar are Vol X.

---

## 15.2 Dictionary union operators `|` and `|=` (PEP 584)

**Why this exists.** Merging dicts used to mean either `{**d1, **d2}` (terse but cryptic in larger
expressions) or `m = d1.copy(); m.update(d2)` (two statements, no inline use). **PEP 584** gave
`dict` the binary `|` (merge into a new dict) and `|=` (in-place update), mapping to the
`nb_or`/`nb_inplace_or` slots on `PyDict_Type`. Right-hand keys win on collision:

```python
# Caption: | merges (right wins), |= updates in place; non-mappings raise.
d1, d2 = {"a": 1, "b": 2}, {"b": 99, "c": 3}
print("d1 | d2 (right wins):", d1 | d2)

merged = d1.copy()
merged |= d2
print("|= update:", merged)

try:
    d1 | [1, 2]                  # right operand isn't a mapping
except TypeError as e:
    print("non-mapping:", str(e).split(":")[0])
```

Verified output (CPython 3.13.5):

```text
d1 | d2 (right wins): {'a': 1, 'b': 99, 'c': 3}
|= update: {'a': 1, 'b': 99, 'c': 3}
non-mapping: unsupported operand type(s) for |
```

Two semantics worth noting: `|` always returns a plain `dict` even if the operands are `dict`
subclasses, and `|=` accepts any mapping (or even an iterable of key/value pairs, like `update`) on
the right while `|` requires a mapping. Prefer `|`/`|=` for readability; `{**a, **b}` remains valid
and is marginally faster for merging many dicts at once.

---

## 15.3 Structural pattern matching (PEP 634)

**Why this exists — and what it is not.** `match`/`case` is **not** a C/Java `switch` (which only
compares a scalar against constants). It is **structural matching**: each `case` is a *pattern* that
simultaneously **tests the shape** of the subject and **binds names** from its components — closer
to Rust's `match` or ML-family destructuring. It shines when branching on the *structure* of data
(ASTs, JSON-ish messages, command tokens, geometric types).

```python
# Caption: the pattern taxonomy — literal, sequence, mapping, class, OR, guard, capture, wildcard.
def classify(x):
    match x:
        case 0:                              # literal (value) pattern
            return "zero"
        case [a]:                            # sequence pattern, length 1
            return f"one-list:{a}"
        case [a, b, *rest]:                  # sequence with star-capture
            return f"seq:{a},{b},rest={rest}"
        case {"type": t, "value": v}:        # mapping pattern (subset of keys)
            return f"map:{t}={v}"
        case str() as s if len(s) > 3:       # class pattern + 'as' binding + guard
            return f"long-str:{s}"
        case int() | float():                # OR pattern of class patterns
            return f"number:{x}"
        case _:                              # wildcard
            return "unknown"

for v in [0, [7], [1, 2, 3, 4], {"type": "pt", "value": 9}, "hello", 3.5, (1, 2)]:
    print(f"  {v!r:>22} -> {classify(v)}")
```

Verified output (CPython 3.13.5):

```text
                       0 -> zero
                     [7] -> one-list:7
            [1, 2, 3, 4] -> seq:1,2,rest=[3, 4]
  {'type': 'pt', 'value': 9} -> map:pt=9
                 'hello' -> long-str:hello
                     3.5 -> number:3.5
                  (1, 2) -> seq:1,2,rest=[]
```

Note `(1, 2)` matched the **sequence** pattern: sequence patterns match *any* `collections.abc.Sequence`
(lists, tuples, …) **except** `str`/`bytes`/`bytearray`, which are deliberately excluded so a string
is not destructured character-by-character. Mapping patterns match a *subset* of keys (extra keys are
fine).

**Class patterns** use `__match_args__` to map positional sub-patterns to attributes:

```python
# Caption: class patterns destructure by __match_args__ (positional) or by keyword.
class Point:
    __match_args__ = ("x", "y")
    def __init__(self, x, y):
        self.x, self.y = x, y

def locate(p):
    match p:
        case Point(0, 0):  return "origin"
        case Point(x, 0):  return f"x-axis@{x}"
        case Point(0, y):  return f"y-axis@{y}"
        case Point(x, y):  return f"point@{x},{y}"

print("  Point(0,0):", locate(Point(0, 0)))
print("  Point(5,0):", locate(Point(5, 0)))
print("  Point(2,3):", locate(Point(2, 3)))
```

Verified output (CPython 3.13.5):

```text
  Point(0,0): origin
  Point(5,0): x-axis@5
  Point(2,3): point@2,3
```

**What the compiler does.** A `match` does not compile to a linear `if/elif` chain; the compiler
builds a decision tree with dedicated opcodes — `MATCH_SEQUENCE`, `MATCH_MAPPING`, `MATCH_KEYS`,
`MATCH_CLASS`, plus `GET_LEN`/`UNPACK_SEQUENCE` — that test shape once and fail fast before binding:

```python
# Caption: real 3.13 bytecode for a sequence pattern.
import dis
def seqmatch(data):
    match data:
        case [1, y]:
            return y
        case _:
            return None
dis.dis(seqmatch)
```

Verified output (CPython 3.13.5, excerpt):

```text
 56   LOAD_FAST         0 (data)
      MATCH_SEQUENCE
      POP_JUMP_IF_FALSE  ... (to L1)
      GET_LEN
      LOAD_CONST        1 (2)
      COMPARE_OP        72 (==)        # length must be 2
      POP_JUMP_IF_FALSE  ... (to L1)
      UNPACK_SEQUENCE   2
      LOAD_CONST        2 (1)
      COMPARE_OP        88 (bool(==))  # first element must equal 1
      POP_JUMP_IF_FALSE  ... (to L1)
      STORE_FAST        1 (y)          # only now bind y
```

Bindings happen only on the success path — a failed sub-pattern cleans the stack and tries the next
case, so a half-matched pattern never leaks a partial binding.

---

## 15.4 The capture-vs-value gotcha, performance, and anti-patterns

**The one gotcha that bites everyone: a bare name captures.** In a pattern, an unqualified name is a
**capture pattern** — it matches *anything* and binds the name. It is **not** a comparison against a
variable of that name. So `case STATUS_OK:` does not test `subject == STATUS_OK`; it binds the
subject to `STATUS_OK`. To compare against a named constant, use a **dotted (value) pattern**:

```python
# Caption: bare name = capture (always matches); dotted name = value (compares equality).
import http

def capture(code):
    match code:
        case x:                       # bare name: captures, always matches
            return f"captured x={x}"
print("bare-name captures:", capture(404))

def check(code):
    match code:
        case http.HTTPStatus.OK:      # dotted: VALUE pattern, compares equality
            return "ok"
        case _:
            return "other"
print("dotted value 200:", check(200))
print("dotted value 404:", check(404))
```

Verified output (CPython 3.13.5):

```text
bare-name captures: captured x=404
dotted value 200: ok
dotted value 404: other
```

Python 3.13 adds a safety net: a bare-name capture *before* other cases makes them unreachable and
is now a compile-time `SyntaxError`:

```python
# Caption: 3.13 rejects a capture that shadows later cases.
try:
    compile("match c:\n case NAME:\n  pass\n case 1:\n  pass\n", "<s>", "exec")
except SyntaxError as e:
    print("SyntaxError:", str(e).split(" (")[0])
```

Verified output (CPython 3.13.5):

```text
SyntaxError: name capture 'NAME' makes remaining patterns unreachable
```

**Other anti-patterns and notes.**
- **Don't use `match` for plain equality** on a scalar against a few constants — an `if/elif` (or a
  dict dispatch) is clearer and no slower for that case. `match` earns its keep on *structure*.
- **Remember the str/bytes exclusion**: `case [x, y]:` will not match `"ab"`; match strings with
  `str()` patterns or guards.
- **Mapping patterns are subset matches**: `case {"id": i}:` matches any mapping containing `"id"`.
  Use `**rest` to capture the remainder, or guards to assert exactness.
- **`__match_args__` is the public destructuring contract** of your class — dataclasses set it
  automatically from their field order (Chapter 13).

---

## 15.5 Summary and cross-references

- The **PEG parser** (PEP 617, 3.9) — ordered choice + unlimited lookahead + packrat memoization —
  replaced LL(1) and unlocked richer syntax (parenthesized context managers; the `match` grammar).
- **`|`/`|=`** (PEP 584) merge/update dicts; `|` returns a plain `dict`, right keys win, non-mappings
  raise `TypeError`.
- **Structural pattern matching** (PEP 634) is **destructuring + matching**, compiled to a decision
  tree (`MATCH_SEQUENCE`/`MATCH_CLASS`/…). Patterns: literal, capture, wildcard `_`, sequence
  (excludes str/bytes), mapping (subset), class (via `__match_args__`), `|` OR, `as` binding, and
  `if` guards.
- **A bare name captures, it does not compare** — use a **dotted value pattern** for constants;
  3.13 errors on captures that make later cases unreachable.

**Cross-references.** LL(1)→PEG in depth → Chapter 1. `{**a, **b}` unpacking → Chapter 11. dict
internals and `update` semantics → Vol XII. `__match_args__` on dataclasses → Chapter 13. Enums in
`match` → Chapter 10. The formal grammar → Vol X. Builtin generics (PEP 585) and `X | Y` unions (PEP
604) → Chapter 16.

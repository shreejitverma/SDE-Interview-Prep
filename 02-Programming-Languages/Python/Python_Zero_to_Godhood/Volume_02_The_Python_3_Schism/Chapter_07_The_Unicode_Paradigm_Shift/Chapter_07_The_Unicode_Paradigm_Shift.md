# Chapter 7: The Unicode Paradigm Shift and Text vs. Bytes (Python 3.0)

Python 3.0 was a deliberate, compatibility-breaking reset, and its defining change was the
**hard separation of text from bytes**. In Python 2, one type (`str`) was both a byte string
and an ASCII text string, and the interpreter silently decoded between them — a design that
worked until a non-ASCII byte appeared, then failed at a distance, in production, on someone
else's data. Python 3 made `str` a sequence of **Unicode code points** and `bytes` a sequence
of **octets**, with **no implicit conversion** between them. This chapter covers that split and
the cluster of 3.0 changes that followed from taking correctness seriously: the `print`
function, true division, lazy iterators and views, and — reaching to 3.3 — the **PEP 393**
flexible string representation that made correct Unicode also memory-efficient.

## Section Index
- **7.1** The text/bytes split (PEP 3112) and the coercion ban
- **7.2** `bytes` and `bytearray`: the binary types
- **7.3** Unicode internals: from narrow/wide builds to PEP 393
- **7.4** Encoding, decoding, and error handlers (incl. `surrogateescape`)
- **7.5** `print()` as a function (PEP 3105)
- **7.6** Division unification (PEP 238) and the floor-vs-truncate contrast
- **7.7** Lazy iterators, dictionary views, and `memoryview`
- **7.8** Performance, anti-patterns, and summary

---

## 7.1 The text/bytes split (PEP 3112) and the coercion ban

**Why this exists.** In Python 2, `"café"` was a byte string whose meaning depended on the
source encoding, and `u"café"` was text; mixing them implicitly decoded the bytes with ASCII
and raised `UnicodeDecodeError` the moment a byte exceeded 127. The bug surfaced far from its
cause. Python 3 removes the ambiguity: **`str` is text (code points), `bytes` is binary
(octets), and the two never implicitly convert.** Any implicit mix is an immediate, local
`TypeError`:

```python
# Caption: Python 3 bans implicit text/bytes coercion — a local error, not a distant one.
try:
    b"data" + "string"
except TypeError as e:
    print("TypeError:", e)
```

Verified output (CPython 3.13.5):

```text
TypeError: can't concat str to bytes
```

(Note the message reads "can't concat str to bytes" — the `str` operand cannot be concatenated
to the `bytes` left operand.) The domains are disjoint all the way down to the C API:
`PyBytes_Concat` rejects `str`, and text/bytes comparisons never silently coerce.

**The senior-engineer contrast.** A C `char*` is bytes; "text" is a convention layered on top. A
Java `String` is text, but internally UTF-16, so a "character" (`char`) is a 16-bit code *unit*,
and code points above U+FFFF span two of them — `"😀".length() == 2` in Java. Python 3's `str`
is a sequence of **code points**: `len("😀") == 1`, indexing returns whole code points, and the
*encoding is not part of the object* — it exists only when you `encode()` to `bytes`. You decode
bytes into text at the boundary (input), work in `str`, and encode back to bytes at the boundary
(output). This "Unicode sandwich" is the discipline the whole language is built to enforce.

---

## 7.2 `bytes` and `bytearray`: the binary types

`bytes` is the **immutable** octet sequence; `bytearray` is its **mutable** sibling. Both
support the **buffer protocol** (Vol IX), letting C code and `memoryview` access their storage
without copying.

```c
/* Illustrative; Include/bytesobject.h — immutable, storage inline with the object. */
typedef struct {
    PyObject_VAR_HEAD
    Py_hash_t ob_shash;     /* cached hash; -1 if uncomputed (bytes are hashable) */
    char ob_sval[1];        /* the octets, allocated inline, NUL-terminated for C interop */
} PyBytesObject;
```

```c
/* Illustrative; Include/bytearrayobject.h — mutable, storage via a separate buffer. */
typedef struct {
    PyObject_VAR_HEAD
    Py_ssize_t ob_alloc;    /* capacity of the buffer */
    char *ob_bytes;         /* start of the allocation */
    char *ob_start;         /* start of the logical data (>= ob_bytes) */
    Py_ssize_t ob_exports;  /* count of active buffer exports (e.g. memoryviews) */
} PyByteArrayObject;
```

Two design details earn their keep. The split between `ob_bytes` and `ob_start` makes a
left-end deletion (`del ba[0]`) *O(1)* — just advance `ob_start` rather than shift the whole
buffer. And `ob_exports` is a safety interlock: while a `memoryview` is live (`ob_exports > 0`),
any operation that would reallocate the buffer raises `BufferError`, so a view can never end up
pointing at freed memory. `bytearray` grows with the same amortized-*O*(1) overallocation policy
as `list` (Chapter 5 / Vol XII).

---

## 7.3 Unicode internals: from narrow/wide builds to PEP 393

**The original problem.** Through Python 3.2, a CPython build stored *every* string in one fixed
width chosen at compile time: a **narrow build** used 2-byte `Py_UNICODE` (UCS-2), and a **wide
build** used 4-byte (UCS-4). Narrow builds mishandled astral characters (above U+FFFF) as
surrogate pairs, breaking `len()` and indexing; wide builds made `"hello"` cost 20 bytes of
payload. Neither was acceptable.

**PEP 393 (Python 3.3): flexible string representation.** CPython now picks the **narrowest
storage that fits the string's largest code point**, per string, at creation time — three
"kinds": 1 byte (Latin-1 range, including a pure-ASCII fast path), 2 bytes (BMP), or 4 bytes
(full Unicode). The result is correct indexing for all code points *and* compact memory, with no
build-time tradeoff. You can watch the kind change with the data:

```python
# Caption: PEP 393 chooses storage width per string from its largest code point.
import sys
for label, s in [("ASCII  'hello'", "hello"),
                 ("Latin-1 'hñ'", "hñ"),                # max U+00F1 -> 1 byte/char
                 ("BMP/UCS-2 'h你'", "h你"),            # max U+4F60 -> 2 bytes/char
                 ("Astral/UCS-4 'h😀'", "h😀")]:        # max U+1F600 -> 4 bytes/char
    print(f"  {label:20s} len={len(s)} getsizeof={sys.getsizeof(s)}")
print("  empty str:", sys.getsizeof(""))
```

Verified output (CPython 3.13.5):

```text
  ASCII  'hello'        len=5 getsizeof=46
  Latin-1 'hñ'          len=2 getsizeof=59
  BMP/UCS-2 'h你'        len=2 getsizeof=62
  Astral/UCS-4 'h😀'     len=2 getsizeof=68
  empty str: 41
```

Read the numbers: pure ASCII is the cheapest (compact header + 1 byte/char + a NUL); a single
non-ASCII code point promotes the *whole string* to a wider kind with a larger header (the
Latin-1/BMP/astral headers carry extra fields). The crucial guarantees: **`len()` and indexing
are always *O*(1) and always in code points** (no surrogate-pair surprises), and ASCII/Latin-1
text stays as cheap as a byte string. One emoji in a megabyte of ASCII, though, promotes the
entire string to 4 bytes/char — a real memory consideration for large text buffers.

---

## 7.4 Encoding, decoding, and error handlers (incl. `surrogateescape`)

`str.encode(encoding)` turns code points into `bytes`; `bytes.decode(encoding)` turns octets
back into a `str`. Both take an **error handler** governing malformed data: `strict` (default —
raise), `ignore` (drop), `replace` (insert U+FFFD or `?`), `backslashreplace` (escape), and
`surrogateescape`.

**`surrogateescape` (PEP 383)** is the one worth understanding, because it is how Python survives
the Unix filesystem. POSIX paths are arbitrary byte sequences, not guaranteed valid UTF-8. To
let such bytes round-trip through `str` losslessly, each undecodable byte `b` is mapped to a lone
surrogate code point `U+DC00 + b` on decode, and mapped back to `b` on encode:

```python
# Caption: surrogateescape round-trips arbitrary bytes through a str losslessly.
raw = b"bad_\xff_path.txt"
decoded = raw.decode("utf-8", "surrogateescape")
reencoded = decoded.encode("utf-8", "surrogateescape")
print("round-trips exactly:", reencoded == raw)
print("contains lone surrogate U+DCFF:", "\udcff" in decoded)
```

Verified output (CPython 3.13.5):

```text
round-trips exactly: True
contains lone surrogate U+DCFF: True
```

This is exactly how `os.fsencode`/`os.fsdecode` and `sys.argv` handle filenames. **The
discipline:** always pass `encoding=` explicitly to `open()`, `encode`, `decode`, and
subprocess boundaries. Relying on the locale default is the cause of "works on my machine,
`UnicodeDecodeError` in the container." (Python 3.15 makes UTF-8 the default for text files;
until then, be explicit. See Vol II, Ch 8 and Vol XIV.)

---

## 7.5 `print()` as a function (PEP 3105)

Python 2's `print` was a statement compiled to dedicated `PRINT_ITEM`/`PRINT_NEWLINE` opcodes —
its behavior fixed at compile time. Python 3 made it an ordinary builtin function, resolved by
normal name lookup and therefore overridable, composable, and keyword-configurable
(`sep`, `end`, `file`, `flush`):

```python
# Caption: print is a function call — note the modern CALL opcode, not a print statement.
import dis
dis.dis(compile('print("x")', "<s>", "exec"))
```

Verified output (CPython 3.13.5):

```text
  0           RESUME                   0
  1           LOAD_NAME                0 (print)
              PUSH_NULL
              LOAD_CONST               0 ('x')
              CALL                     1
              POP_TOP
              RETURN_CONST             1 (None)
```

Being a function, it accepts `print(*items, sep=", ", end="", file=sys.stderr, flush=True)`,
can be passed around, and can be replaced (`builtins.print = ...`) for logging shims. The
`PUSH_NULL` + `CALL` shape is the modern 3.11+ calling convention.

---

## 7.6 Division unification (PEP 238) and the floor-vs-truncate contrast

In Python 2, `/` was floor division for two integers but true division if either was a float —
`5/2 == 2` but `5.0/2 == 2.5` — a silent precision trap. Python 3 split the operators cleanly:
**`/` is always true division** (returns `float`), **`//` is floor division**.

```python
# Caption: / is true division; // floors toward negative infinity.
print("7/2 =", 7 / 2)
print("7//2 =", 7 // 2)
print("-7//2 =", -7 // 2)
```

Verified output (CPython 3.13.5):

```text
7/2 = 3.5
7//2 = 3
-7//2 = -4
```

**The senior-engineer contrast — and a real portability hazard.** C, C++, Java, and Go integer
division **truncates toward zero**: `-7 / 2 == -3`. Python's `//` **floors toward negative
infinity**: `-7 // 2 == -4`. Likewise `%` follows the divisor's sign in Python (`-7 % 2 == 1`)
but the dividend's sign in C (`-7 % 2 == -1`). Porting modular-arithmetic code between Python and
C without accounting for this produces off-by-one bugs that pass every positive-number test. When
you need truncation in Python, use `int(a / b)` or `math.trunc`.

---

## 7.7 Lazy iterators, dictionary views, and `memoryview`

Python 3 made the sequence-producing builtins **lazy**: `range`, `zip`, `map`, and `filter`
return iterators/iterable views instead of materializing lists, and `dict.keys/values/items`
return **live views** onto the dict.

**`range` is O(1) memory and O(1) random access** — it stores only `start`/`stop`/`step` and
computes `start + i*step` on demand, with arithmetic membership testing:

```python
# Caption: range is constant-memory with O(1) indexing and membership.
import sys
r = range(0, 10_000_000, 3)
print("getsizeof(range):", sys.getsizeof(r), "| len:", len(r))
print("9_999_999 in r:", 9_999_999 in r, "| r[1_000_000]:", r[1_000_000])
print("getsizeof(list(range(1000))):", sys.getsizeof(list(range(1000))))
```

Verified output (CPython 3.13.5):

```text
getsizeof(range): 48 | len: 3333334
9_999_999 in r: True | r[1_000_000]: 3000000
getsizeof(list(range(1000))): 8056
```

A `range` over ten million values costs 48 bytes; the equivalent list of just one thousand ints
already costs 8 KB (plus the int objects). **Dictionary views are live** — they reflect mutations
to the underlying dict rather than snapshotting:

```python
# Caption: dict views track the dict; they do not copy.
d = {"a": 1, "b": 2}
ks = d.keys()
d["c"] = 3
print("view reflects the insert:", list(ks))
```

Verified output (CPython 3.13.5):

```text
view reflects the insert: ['a', 'b', 'c']
```

**`memoryview`** extends the laziness to binary data: it exposes another object's buffer
(`bytes`, `bytearray`, `array`, NumPy arrays) **without copying**, so you can slice and even
mutate in place across the shared memory:

```python
# Caption: memoryview slices and edits a buffer with zero copy.
data = bytearray(b"system_payload_data")
view = memoryview(data)
sub = view[7:14]
print("slice:", sub.tobytes())
sub[0] = ord(b"P")
print("in-place edit visible in original:", bytes(data))
```

Verified output (CPython 3.13.5):

```text
slice: b'payload'
in-place edit visible in original: b'system_Payload_data'
```

This zero-copy capability is foundational for high-performance I/O and networking; the full
buffer protocol and `memoryview` mechanics are developed in Vol IX.

---

## 7.8 Performance, anti-patterns, and summary

**Anti-patterns.**
- **Omitting `encoding=`** on `open`/`encode`/`decode` — the top source of cross-platform
  `UnicodeDecodeError`. Always be explicit; use `surrogateescape` for filesystem paths.
- **Confusing `str` and `bytes`** at API boundaries — sockets, files in binary mode, and
  hashing all speak `bytes`; decode/encode at the edge, not in the middle.
- **`str` concatenation in a loop** — each `+=` builds a new immutable string (*O*(n²) overall).
  Accumulate in a `list` and `"".join(parts)` once.
- **Materializing lazy iterators needlessly** — `list(range(n))`, `list(d.keys())` to "use it
  twice." Iterate directly; reach for a list only when you need indexing or multiple passes.
- **Assuming C division semantics** — `//` floors toward −∞ and `%` follows the divisor's sign.

**Summary.**
- `str` is **code points**, `bytes`/`bytearray` are **octets**; there is **no implicit
  conversion** (PEP 3112). Decode at input, work in `str`, encode at output — the Unicode
  sandwich.
- **PEP 393** stores each string in the narrowest of three widths from its largest code point,
  giving correct *O*(1) indexing *and* compact memory.
- Error handlers (esp. **`surrogateescape`**, PEP 383) make arbitrary bytes round-trip through
  `str`; always specify the encoding.
- `print` is a **function** (PEP 3105); `/` is **true division** and `//` **floors toward −∞**
  (PEP 238), unlike C/Java truncation.
- `range`/`zip`/`map`/`filter` are **lazy**, dict `keys/values/items` are **live views**, and
  `memoryview` shares buffers **zero-copy**.

**Cross-references.** The layered text/binary I/O stack and `TextIOWrapper` → Chapter 6.
Stdlib consolidation and UTF-8 mode → Chapter 8. Iterators and generators in depth → Vol III.
The buffer protocol, `memoryview`, and zero-copy numerics → Vol IX. CPython string and container
internals in full → Vol XII. The complete codecs/encoding reference → Vol XI.

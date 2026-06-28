# Chapter 6: Low-Level File I/O and Exception Unwinding (Python 2.x → modern)

This chapter closes Volume I with the two runtime subsystems an engineer touches on every
program: how bytes move between a process and the operating system, and how control unwinds
when something fails. Both were re-architected away from their 2.x forms. File I/O moved from
thin C `stdio` wrappers to the **layered `io` stack** (PEP 3116) that talks to the kernel
directly. Exception handling moved from a per-frame **runtime block stack** to a static
**exception table** (Python 3.11) that makes a `try` block cost *nothing* when no exception is
raised. We teach the modern mechanisms and use the 2.x originals only to show what problem each
redesign solved.

## Section Index
- **6.1** The layered I/O architecture (PEP 3116)
- **6.2** Exception objects and per-thread exception state
- **6.3** Zero-cost exceptions: the exception table (Python 3.11)
- **6.4** Exception chaining: `__context__`, `__cause__`, and `raise from`
- **6.5** Performance and anti-patterns (EAFP vs LBYL)
- **6.6** Summary and cross-references

---

## 6.1 The layered I/O architecture (PEP 3116)

**Why this exists.** In Python 2, `open()` returned a `file` object that was a thin wrapper
around a C `stdio` `FILE *`. That inherited `stdio`'s buffering policy *and* its per-call
internal mutex — locks that are pure waste under the GIL, which already serializes interpreter
execution. **PEP 3116** replaced this with a composable three-layer stack (the `io` module)
that calls the kernel's `read(2)`/`write(2)` directly and lets each layer do one job:

```text
text   :  TextIOWrapper   — encode/decode str <-> bytes, universal newlines
            │
buffered:  BufferedReader / BufferedWriter — in-memory buffer, batches syscalls
            │
raw    :  FileIO          — a bare OS file descriptor; one syscall per read/write
```

The stack is observable: `open()` in text mode returns the top layer, and you can reach down
through it:

```python
# Caption: open() builds a three-object stack; each layer is reachable.
with open("demo.txt", "w", encoding="utf-8") as f:
    print("text layer:    ", type(f).__name__)
    print("buffered layer:", type(f.buffer).__name__)
    print("raw layer:     ", type(f.buffer.raw).__name__)
    print("OS descriptor: ", f.fileno())
    f.write("hello")
```

Verified output (CPython 3.13.5):

```text
text layer:     TextIOWrapper
buffered layer: BufferedWriter
raw layer:      FileIO
OS descriptor:  3
```

**What each layer buys you.**
- **`FileIO` (raw)** issues exactly one `read`/`write` syscall per call — unbuffered, slow if
  used directly for many small operations, but the foundation.
- **`BufferedReader`/`BufferedWriter`** hold an in-memory buffer (default ~8 KiB, exposed as
  `io.DEFAULT_BUFFER_SIZE`) so many small reads/writes collapse into few syscalls. Crossing the
  user/kernel boundary is the dominant cost in I/O; this layer is what makes line-by-line
  reading affordable.
- **`TextIOWrapper`** adds the `str`↔`bytes` codec and newline translation. Opening in binary
  mode (`"rb"`/`"wb"`) omits this layer and hands you `bytes` directly — the right choice for
  protocols, images, and anything non-textual. The full text/bytes/Unicode model is Vol II.

**The senior-engineer contrast.** A C programmer reaches for `fopen`/`fread` (`stdio`,
buffered) or `open`/`read` (raw syscalls) and chooses explicitly. Python's `io` stack gives you
*both* in one object: `f` is buffered text, `f.buffer` is buffered bytes, `f.buffer.raw` is the
unbuffered descriptor. You select the layer by reaching to it, and `open(..., buffering=0)`
gives a raw binary stream when you want to manage buffering yourself.

---

## 6.2 Exception objects and per-thread exception state

**Why this exists.** An exception must carry its type, a message/value, and the traceback of
where it propagated — and that state must be *per thread*, since two threads can be handling
different exceptions at once. CPython stores the active exception on the `PyThreadState`.

Two historical facts frame the modern design:

- **The string-exception era (pre-2.6).** You could once `raise "some error"` — a bare string,
  matched by identity. It made hierarchies and categorization impossible and was removed:
  **every exception must derive from `BaseException`.** The hierarchy matters — `except
  Exception` deliberately does *not* catch `KeyboardInterrupt`, `SystemExit`, or
  `GeneratorExit`, which subclass `BaseException` directly so that "catch all errors" does not
  also swallow shutdown signals.
- **State consolidation (3.11).** Python 2.x kept two separate triples on the thread state
  (`curexc_type/value/traceback` for the propagating exception, `exc_type/value/traceback` for
  the one being handled). Modern CPython stores a single exception **value** object — type and
  traceback are derivable from it (`exc.__traceback__`, `type(exc)`), so the triples collapsed
  to one pointer. `sys.exc_info()` still returns the familiar `(type, value, traceback)` tuple,
  reconstructed from that single object:

```python
# Caption: sys.exc_info() inside an except block, reconstructed from the active exception.
import sys
try:
    raise ValueError("v")
except ValueError:
    exc_type, exc_value, exc_tb = sys.exc_info()
    print("type:", exc_type.__name__, "| value:", exc_value)
```

Verified output (CPython 3.13.5):

```text
type: ValueError | value: v
```

Prefer `except SomeError as e:` and use `e` directly; `sys.exc_info()` is mainly for generic
logging/framework code that must work without naming the exception.

---

## 6.3 Zero-cost exceptions: the exception table (Python 3.11)

**Why this exists — the big change.** Through Python 3.10, each frame carried a runtime **block
stack**: entering a `try` executed a `SETUP_FINALLY`/`SETUP_EXCEPT` opcode that *pushed* a
`PyTryBlock` (handler address, stack level) onto that stack, and leaving the block popped it.
That meant every `try` cost real work on entry and exit **even when no exception was raised** —
the common case. Python 3.11 (the "Faster CPython" work) removed the block stack entirely and
replaced it with a static, per-code-object **exception table**: a side table mapping ranges of
bytecode offsets to their handler. On the non-exception path a `try` now executes **nothing
extra**; the cost is paid only when an exception is actually raised and the interpreter consults
the table to find the handler. This is what "zero-cost exceptions" means.

You can see it directly — there is no `SETUP_*` opcode, and an `ExceptionTable` appears at the
end of the disassembly:

```python
# Caption: a try/except compiles to an exception table, not block-stack setup.
import dis

def guarded(x):
    try:
        return 10 / x
    except ZeroDivisionError:
        return -1

dis.dis(guarded)
```

Verified output (CPython 3.13.5, excerpt):

```text
  L1:     LOAD_CONST    1 (10)
          LOAD_FAST     0 (x)
          BINARY_OP    11 (/)
  L2:     RETURN_VALUE
  --  L3: PUSH_EXC_INFO
          LOAD_GLOBAL   0 (ZeroDivisionError)
          CHECK_EXC_MATCH
          POP_JUMP_IF_FALSE  3 (to L5)
          POP_TOP
  L4:     POP_EXCEPT
          RETURN_CONST  2 (-1)
  L5:     RERAISE       0
  ...
ExceptionTable:
  L1 to L2 -> L3 [0]
  L3 to L4 -> L6 [1] lasti
  L5 to L6 -> L6 [1] lasti
```

The `try` body (`L1`–`L2`) carries no setup overhead. If `BINARY_OP /` raises, the interpreter
looks up the current offset in the `ExceptionTable`, finds the handler at `L3`, pushes the
exception info (`PUSH_EXC_INFO`), tests it (`CHECK_EXC_MATCH`), runs the handler, and clears the
exception state (`POP_EXCEPT`). If no handler in this frame matches, `RERAISE` propagates to the
caller, whose frame is searched the same way — building the traceback (`PyTracebackObject`)
frame by frame as it unwinds, until a handler catches it or the program exits via the top-level
traceback. The unwinding *semantics* are what the 2.x block-stack diagrams described; only the
*mechanism* (table lookup vs. runtime stack) changed, and with it the cost model.

---

## 6.4 Exception chaining: `__context__`, `__cause__`, and `raise from`

**Why this exists (PEP 3134).** When handling one error raises another, both are relevant: the
new one for the abstraction you present, the original for the root cause. Python links them
automatically.

- **Implicit chaining** — if an exception is raised *during* the handling of another, the new
  exception's `__context__` is set to the original. Tracebacks print "During handling of the
  above exception, another exception occurred."
- **Explicit chaining** — `raise New() from original` sets `__cause__` (and implies the link is
  deliberate), printing "The above exception was the direct cause."
- **Suppression** — `raise New() from None` clears the chain when the original is noise.

```python
# Caption: raise ... from ... sets __cause__; the original is also kept as __context__.
def chained():
    try:
        1 / 0
    except ZeroDivisionError as e:
        raise RuntimeError("wrapped") from e

try:
    chained()
except RuntimeError as e:
    print("__cause__:   ", type(e.__cause__).__name__)
    print("__context__: ", type(e.__context__).__name__)
```

Verified output (CPython 3.13.5):

```text
__cause__:    ZeroDivisionError
__context__:  ZeroDivisionError
```

This is the right way to wrap low-level errors in domain errors without losing the diagnostic
trail — wrap a `KeyError` from a config lookup in a `ConfigError`, `raise ConfigError(...) from
e`, and the traceback shows both. The multi-error generalization — **exception groups** and
`except*` (PEP 654) — belongs to concurrent code and is covered in Vol VI.

---

## 6.5 Performance and anti-patterns (EAFP vs LBYL)

**EAFP is now genuinely cheap.** Python idiom favors **EAFP** ("easier to ask forgiveness than
permission") — try the operation, handle the exception — over **LBYL** ("look before you leap")
with pre-checks. With zero-cost exceptions (§6.3), the `try` itself adds no overhead on the
success path, so EAFP is not merely idiomatic but efficient *when exceptions are rare*. The
caveat is unchanged: raising and catching is still expensive *per exception*, so EAFP loses
badly in a loop where the exceptional case is common — there, a cheap pre-check wins.

```python
# EAFP (preferred when the miss is rare):
try:
    value = config["timeout"]
except KeyError:
    value = DEFAULT_TIMEOUT
# LBYL (preferred when misses are common / the check is cheaper than the raise):
value = config["timeout"] if "timeout" in config else DEFAULT_TIMEOUT
```

**Anti-patterns.**
- **`except:` (bare) or `except BaseException`** catches `KeyboardInterrupt` and `SystemExit`,
  making programs unkillable and masking shutdown. Catch `Exception` at most, and prefer the
  narrowest type that fits.
- **Swallowing exceptions** (`except Exception: pass`) destroys diagnostics. At minimum log;
  better, re-raise or wrap with `raise ... from e`.
- **Not using a context manager for files.** `open()` without `with` leaks the descriptor until
  the GC happens to finalize it (and never deterministically — Chapter 2). Always `with
  open(...) as f:`.
- **Forgetting to flush/`fsync` for durability.** Buffering (§6.1) means a write is in user
  space until flushed; for crash-durable writes, `f.flush()` then `os.fsync(f.fileno())`.
- **Reading huge files with `.read()`.** Iterate the file object (line-buffered) or read in
  chunks; do not pull a multi-gigabyte file into one `str`.

---

## 6.6 Summary and cross-references

- File I/O is a **three-layer stack** (PEP 3116): `FileIO` (raw syscalls) → `BufferedReader/
  Writer` (batch syscalls) → `TextIOWrapper` (codec). Reach to `f.buffer`/`f.buffer.raw` to
  pick a layer; binary mode drops the text layer.
- Every exception derives from **`BaseException`**; `except Exception` intentionally spares
  `KeyboardInterrupt`/`SystemExit`. The thread-state exception triples collapsed to a single
  value object in 3.11; `sys.exc_info()` reconstructs the tuple.
- Python 3.11 replaced the runtime **block stack** with a static **exception table**, making
  `try` **zero-cost** on the success path; unwinding now means table lookup, building the
  traceback frame by frame.
- **Chaining** links errors: implicit `__context__`, explicit `__cause__` via `raise from`,
  suppressed via `from None`.
- **EAFP** is cheap on the happy path and idiomatic; reserve LBYL for hot loops where the
  exceptional case is common.

**Cross-references.** Deterministic resource cleanup with `with` → Chapter 5. Reference
counting vs. GC finalization of file objects → Chapter 2. The full text/bytes/Unicode and codec
model → Vol II. Exception groups and `except*` → Vol VI. The specializing interpreter behind
"Faster CPython" → Vol VI, Ch 17. The complete `io` and OS-interface reference → Vol XIV.

---

*Volume I complete. The next volume opens the Python 3 era with the Unicode paradigm shift
(Vol II, Ch 7).*

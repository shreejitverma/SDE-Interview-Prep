# Master Coverage Matrix — *Python Zero to Godhood*

Auditable map of the whole book. Updated as each volume completes. Status legend:
**DONE** (combined/authored, examples verified, sources archived) · **WIP** ·
**PLANNED** · **GAP** (must author).

Verification interpreter: **CPython 3.13.5** (`/opt/anaconda3/bin/python3`).

---

## Volume map (16 volumes; author's chronological-era + topical scheme, extended)

| Vol | Title | Theme | Status |
|---|---|---|---|
| I | Classic Python & the Core Engine | 1.0–2.7, the execution pipeline | **DONE** |
| II | The Python 3 Schism | 3.0–3.2: unicode/bytes, stdlib consolidation | **DONE** |
| III | Generators, Iterators & Async Inception | 3.3–3.5 | **DONE** |
| IV | Expressiveness & Ergonomics | 3.6–3.7 | **DONE** |
| V | Structural Shifts & Pattern Matching | 3.8–3.10 | **DONE** |
| VI | Performance Leap & Runtime Mechanics | 3.11–3.12 | PLANNED |
| VII | The GIL-less Future & JIT | 3.13–3.14+, free-threading, JIT | PLANNED |
| VIII | Runtime Internals & C Extensions | allocator/GC, C-API, descriptors/MRO model, ceval | PLANNED |
| IX | High-Performance & Low-Latency Concurrency | threads/mp/async, SIMD/GPU, structured concurrency, capstone | PLANNED |
| X | The Language Reference Formalisms | lexical/exec model, data model, import, runtime services | PLANNED |
| XI | Standard Library I — Text, Binary & Crypto | re, string, struct, hashlib | PLANNED |
| XII | Standard Library II — Data, Time, Numeric, Functional | datetime, enum, collections, functools | PLANNED |
| XIII | Standard Library III — Compression & Persistence | zlib/bz2/lzma, zip/tar, pickle/shelve | PLANNED |
| XIV | Standard Library IV — Networking & Internet | sockets, http, email, xml, ipaddress | PLANNED |
| XV | Standard Library V — System, Tooling & Packaging | os/subprocess, argparse, venv, packaging, **typing in depth** | PLANNED |
| XVI | Applied & Domain Python | scientific, web, data, quant, frontier | PLANNED |
| App | Appendices A–W | references, grammar, opcodes, glossaries | PLANNED |

---

## Volume I — Classic Python & the Core Engine

| Ch | Title | Seed source(s) | Mined legacy | Status | Examples (exec/gated) |
|---|---|---|---|---|---|
| 1 | Inception & the Executable Pipeline | `Chapter_01_Python_10_to_16…` | `Chapter_01_INCEPTION…` | **DONE** | 3/0 |
| 2 | The PyObject Model & Reference Counting | `Chapter_02_Python_1x…` | `Chapter_02_THE_PYOBJECT_CORE…` | **DONE** | 5/0 |
| 3 | Comprehensions, Nested Scopes & Cyclic GC | `Chapter_03_Python_20_to_21…` | `Chapter_03_SCOPES_NAMESPACES…` | **DONE** | 6/0 |
| 4 | Type–Class Unification, Descriptors & C3 MRO | `Chapter_04_Python_22_to_23…` | `Chapter_04_OBJECT-ORIENTED…` | **DONE** | 6/0 |
| 5 | Decorators, Context Managers & the 2.x Twilight | `Chapter_05_Python_24_to_27…` | — (see note) | **DONE** | 5/0 |
| 6 | Low-Level File I/O & Exception Unwinding | `Chapter_06_Python_2x…` | `Chapter_06_FILE_IO…` | **DONE** | 4/0 |

**Volume I wrap-up:** 6/6 chapters DONE; 30 examples executed live on CPython 3.13.5, 0 version-gated; every why-obligation covered; all sources archived. **Deferred mining:** the canonical Ch 5 source §5.4 (CPython container internals — list overallocation, dict/set/tuple layout, free lists) and the legacy `Chapter_05_UNDER_THE_HOOD_BUILT-IN_DATA_STRUCTURES.{md,tex}` (still in root) → to be mined for the Vol XII data-structures chapter. Anomalies: Track-C CAPS numbering is topic-misaligned (Ch5 CAPS = data structures, not decorators), handled by topic not number.

## Volume II — The Python 3 Schism

| Ch | Title | Seed source(s) | Mined legacy | Status | Examples (exec/gated) |
|---|---|---|---|---|---|
| 7 | The Unicode Paradigm Shift (text vs bytes, PEP 393) | `Chapter_07_Python_30…` | `Chapter_07_THE_PYTHON_30…`, `Chapter_08_ADVANCED_TEXT_VS_BYTES…` | **DONE** | 8/0 |
| 8 | Stdlib Consolidation & the New GIL (3.1–3.2) | `Chapter_08_Python_31_to_32…` | — | **DONE** | 6/0 |

**Volume II wrap-up:** 2/2 chapters DONE; 14 examples executed live on 3.13.5, 0 gated; sources archived. Corrections: `TypeError` text is "can't concat str to bytes" (draft reversed); real PEP 393 sizes replace draft guesses; spawn re-import gotcha demonstrated.

## Volume III — Generators, Iterators & Async Inception

| Ch | Title | Seed source(s) | Mined legacy | Status | Examples (exec/gated) |
|---|---|---|---|---|---|
| 9 | Iterators, Generators & `yield from` | `Chapter_09_Python_33…` | `Chapter_09_ITERATORS…` | **DONE** | 6/0 |
| 10 | Asyncio Inception, Pathlib & Enum (3.4) | `Chapter_10_Python_34…` | — | **DONE** | 5/0 |
| 11 | Native Async/Await & New Operators (3.5) | `Chapter_11_Python_35…` | `Chapter_11_NATIVE_ASYNCAWAIT…`, `Chapter_10_Python_35…` | **DONE** | 7/0 |

**Volume III wrap-up:** 3/3 DONE; 18 examples executed live on 3.13.5, 0 gated; sources archived. Corrections: PEP 393/buffer-protocol overlap deferred to Ch7/Vol IX (no dup); `@asyncio.coroutine` removed in 3.11 (gated as historical); modern `await` bytecode (`GET_AWAITABLE`/`SEND`/`END_SEND`) and `LIST_EXTEND` unpacking replace 3.5-era listings. Deferred: `Chapter_10_CONCURRENCY_MECHANICS` (GIL) → Vol VII.

## Volume IV — Expressive Modern Python

| Ch | Title | Seed source(s) | Mined legacy | Status | Examples (exec/gated) |
|---|---|---|---|---|---|
| 12 | F-Strings, Annotations & the Compact Dict (3.6) | `Chapter_12_Python_36…` | `Chapter_12_SYNTAX_ERGONOMICS…`, `Chapter_11_Python_36…` | **DONE** | 5/0 |
| 13 | Dataclasses, ContextVars & Dict Ordering (3.7) | `Chapter_13_Python_37…` | `Chapter_13_DATACLASSES…`, `Chapter_12_Python_37…` | **DONE** | 5/0 |

**Volume IV wrap-up:** 2/2 DONE; 10 examples executed live on 3.13.5, 0 gated; sources archived. Corrections: real 3.13 f-string bytecode (`CONVERT_VALUE`/`FORMAT_SIMPLE`/`FORMAT_WITH_SPEC`) + PEP 701 features replace `FORMAT_VALUE` flags; descriptor/`__slots__` overlap deferred to Ch4/Vol VIII (taught as applications, not re-derived).

## Volume V — Structural Shifts & Pattern Matching

| Ch | Title | Seed source(s) | Mined legacy | Status | Examples (exec/gated) |
|---|---|---|---|---|---|
| 14 | Walrus & Positional-Only Parameters (3.8) | `Chapter_14_Python_38…` | `Chapter_14_WALRUS…`, `Chapter_13_Python_38…` | **DONE** | 5/0 |
| 15 | PEG Parser, Dict Union & Pattern Matching (3.9–3.10) | `Chapter_15_Python_39_to_310…` | `Chapter_15_STRUCTURAL…`, `Chapter_14_Python_39_to_310…` | **DONE** | 6/0 |
| 16 | Typing Protocols & Structural Subtyping (3.8–3.10) | `Chapter_16_Python_38_to_310…` | `Chapter_16_STATIC_TYPING…`, `Chapter_15_Python_38_to_310…` | **DONE** | 6/0 |

**Volume V wrap-up:** 3/3 DONE; 17 examples executed live on 3.13.5, 0 gated; sources archived. Corrections: `:=` `COPY` bytecode + PEP 709 walrus-comprehension inlining; real `match` decision-tree bytecode; capture-vs-value gotcha + 3.13 unreachable-capture `SyntaxError`; PEG (Ch1) and full type-system (Vol XV) overlaps cross-referenced, not duplicated. Builtin generics (PEP 585) / `X|Y` (PEP 604) taught in Ch16.

*(Volumes VI–XVI matrices appended as each volume is executed.)*

---

## Known book-level GAPS to author (tracked)

- **Type system in depth** (Vol XV): variance, overloads, narrowing, `TypeIs`/`TypeGuard`,
  stubs, gradual typing, mypy vs pyright. *(current Track-B chapter is survey-level)*
- **Structured concurrency & the free-threaded memory model** (Vol IX). *(NEW)*
- **Buffer protocol / memoryview / zero-copy** as a first-class section (Vol IX).
- **3.14 verification pass** (Vol VII): t-strings (PEP 750), deferred annotations
  (PEP 649), subinterpreters stdlib (PEP 734), zstd — version-gated, source-verified.
- Heavy depth expansion of all thin Track-B stdlib + applied chapters.

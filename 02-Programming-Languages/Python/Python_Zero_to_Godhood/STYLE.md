# STYLE.md — The Canonical Style Guide for *Python Zero to Godhood*

This file is the single source of truth for voice, terminology, notation, and code
conventions across the entire book. Every chapter — combined or authored — conforms
to it. Update it deliberately, never ad hoc.

---

## 1. Audience & Voice

- **Reader:** a senior engineer. Assume fluency in at least one of C++, Java, Go, or
  Rust, comfort with operating-system and memory concepts, and working Python. This is
  a *mastery* guide, not a tutorial. Never explain what a `for` loop is; do explain what
  the interpreter *does* when it runs one.
- **Register:** authoritative, precise, senior-to-senior. No filler, no hype, no
  hand-holding, no padding, no motivational throat-clearing. Every sentence carries
  technical weight.
- **The "why" is mandatory.** Every major topic states the problem it solves and the
  runtime model beneath it ("What the interpreter actually does"). A feature explained
  without its rationale and its cost model is incomplete.
- **The senior-engineer contrast.** Where it illuminates the design, contrast Python's
  behaviour with the C++/Java/Go/Rust mental model (e.g. names-as-bindings vs.
  variables-as-typed-storage; refcounting vs. tracing GC; the GIL vs. a free-threaded
  runtime).

## 2. Canonical Terminology (use these exact terms book-wide)

| Use this | Never this |
|---|---|
| the **data model** | "magic methods" |
| **dunder** method (e.g. *the dunder protocol*) | "special method" used loosely |
| **reference counting** / **refcount** | "ref counting" inconsistently |
| **the GIL** (Global Interpreter Lock) | "global lock" |
| **free-threaded** build / runtime (PEP 703/779) | "no-GIL" as a noun |
| **CPython** when discussing the reference implementation | "Python" when the claim is implementation-specific |
| **bytecode**, **opcode**, **code object** (`PyCodeObject`) | "compiled code" loosely |
| **name binding** / a name is **bound** to an object | "variable assignment" when precision matters |
| **specializing adaptive interpreter** (3.11+) | "the JIT" (the JIT is PEP 744, separate) |

- Always distinguish **language semantics** (true of any conforming implementation) from
  **CPython implementation details** (refcounting, PyMalloc, the GIL). Label the latter
  explicitly.
- Spell PEPs as **PEP NNN** with the title on first mention: *PEP 617 (the PEG parser)*.
- Python versions: **3.x** (e.g. *Python 3.12*); a feature's introduction version is
  stated once, authoritatively, grounded in *What's New* / the PEP.

## 3. Structure of Every Chapter

1. `#` Chapter number + title (Markdown) / `\chapter{}` (LaTeX).
2. A 2–4 sentence introduction framing the chapter's **central model** and why it matters.
3. A numbered **table of contents / section index**.
4. Numbered sections and subsections: `N.1`, `N.2`, `N.2.1` (N = chapter number).
5. Each major concept carries an explicit **"Why this exists"** / **"What the interpreter
   actually does"** rationale.
6. A closing **Summary** (key takeaways) and, where relevant, a **cross-reference** block
   ("See Vol VII, Ch 20 for the free-threaded model").

### The depth bar (every major topic must include)
- **Motivation & model** — the problem and what the runtime actually does.
- **Senior-engineer contrast** — vs. C++/Java/Go where it illuminates.
- **Precise semantics & mechanics.**
- **Worked examples** — minimal → production-realistic, all runnable.
- **Performance & memory** — object overhead, allocations, refcount/GC, GIL/free-threading,
  big-O *and* constant factors where they matter.
- **Interactions, anti-patterns, and when NOT to use it.**
- **Old-vs-new** for version features — the pre-feature idiom and why it was inadequate.

## 4. Code Conventions

- **Style:** PEP 8; 4-space indent; `snake_case` functions/variables, `PascalCase` classes,
  `UPPER_SNAKE` constants. Type hints on non-trivial public signatures.
- **Imports:** standard library first, third-party second, local last; one module per line;
  no wildcard imports in examples except when *demonstrating* `import *`.
- **Every example is complete and runnable** unless explicitly a fragment. Prefer examples
  that `print()` an observable result so output can be shown and verified.
- **Captions:** every code block has a one-line caption describing what it demonstrates.
- **Verified output** is shown in a following block and labelled as produced by the stated
  interpreter. REPL transcripts use `pycon`.
- **Version gating:** if a construct requires a newer interpreter than the verification
  interpreter, label it `# Requires Python 3.X+` and mark the chapter note
  *"not executed here — requires 3.X; verified against PEP NNN / What's New."*
- **C source** (CPython internals) is shown with `c` fencing and labelled as illustrative /
  version-specific, with the file it derives from (e.g. `Objects/object.c`). When a struct
  has changed across versions, say so and point to current reality.

## 5. Verification Interpreter

- **Primary:** CPython **3.13.5** (`/opt/anaconda3/bin/python3`), `darwin`/arm64.
- Every non-gated example is executed here; its output is transcribed verbatim.
- Record any example that is version-gated and the authority it was checked against.

## 6. Formatting

### Markdown
- ATX headings (`##`, `###`, `####`). Fenced code: `python`, `pycon`, `c`, `text`, `bash`.
- Tables for comparative content (old-vs-new, model contrasts, cost models).
- **Bold** a key term on first introduction.
- A horizontal rule (`---`) before each major (`N.x`) section.

### LaTeX
- `\chapter{}`, `\section{}`, `\subsection{}`, `\subsubsection{}`.
- `\begin{lstlisting}[language=Python, caption={...}]` for code (set `language` per listing;
  use `language=C` for C, and a plain verbatim style for transcripts/diagrams).
- `\textbf{}` for key terms on first introduction; `itemize`/`enumerate` for lists.
- Emit only the chapter body, from `\chapter{}` onward. The preamble already exists — never
  emit `\documentclass`, `\usepackage`, or `\begin{document}`.

## 7. One Home Per Topic

A feature is taught in depth in **exactly one** canonical chapter; everywhere else
cross-references it. Canonical homes (authoritative — see the master coverage matrix):

- **Free-threading / GIL model** → Vol VII (3.13 free-threaded build).
- **Descriptors / MRO / metaclasses (the model)** → Vol VIII; Vol I Ch 4 keeps the
  *historical* 2.2–2.3 framing and cross-refs.
- **Memory allocator & GC** → Vol VIII; Vol I Ch 3 introduces cyclic GC historically.
- **The type system (mypy/pyright, variance, narrowing)** → Vol XV.
- **asyncio internals & structured concurrency** → Vol IX; Vol III introduces async/await.
- **The data model / dunder protocol** → Vol X.

## 8. Recurring Example Domains

Reuse a small set of domains so examples compound rather than scatter:
- **Trading / market microstructure** (order books, ticks, fixed-point prices) — the HPC
  through-line and the Vol IX capstone.
- **Text & encoding** (Unicode, bytes, protocols) — for the data/encoding chapters.
- **Geometry / vectors** (`Vec2`, `Point`) — for the data-model and operator chapters.
- **A small task scheduler** — for the concurrency/async chapters.

## 9. Cross-Reference Notation

- In prose: *"(see Vol VIII, Ch 24)"*. Within a volume: *"(see §4.3)"*.
- Markdown may link by anchor; LaTeX uses `\ref`/`\label` where the preamble supports it,
  otherwise plain prose references. Keep references stable to chapter *titles*, not raw
  numbers, since numbering may shift during restructuring.

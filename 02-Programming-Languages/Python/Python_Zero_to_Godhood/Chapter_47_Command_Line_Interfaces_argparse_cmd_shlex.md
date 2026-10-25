---
type: concept
track: [sde, quant-dev, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Command Line Interfaces (`argparse`, `cmd`, `shlex`)


Building robust CLI tools requires sophisticated argument parsing and command-loop management.

### 58.1 `argparse`: The Declarative CLI Engine

`argparse` (PEP 389, Python 3.2) is the recommended parser for new code and the successor to `optparse`; both `optparse` and the lower-level, C-style `getopt` remain supported and are not deprecated.
*   **Argument Actions**: `store`, `store_true`, `append`.
*   **Type Conversion**: It can automatically convert inputs to `int`, `Path`, or even open files directly using the `FileType` factory.
*   **Subcommands**: Supports Git-style subcommands (e.g., `git push`, `git pull`) by creating a separate parser for each subcommand and nesting them.

### 58.2 `cmd`: The Interactive Shell Framework

The `cmd` module provides a framework for building interactive line-oriented command interpreters (REPLs).
*   **The Event Loop**: `Cmd.cmdloop()` manages the reading of input and dispatching to methods named `do_X`.
*   **Tab Completion**: Integrates with the `readline` module (on Unix) to provide command and argument completion.

### 58.3 `shlex`: Shell Lexical Analysis

`shlex` splits strings following the rules of the POSIX shell.
*   **`quote()`**: Use this when building commands to be executed by a shell to prevent injection.
*   **Parsing**: It is a state-based lexical analyzer. It handles quotes, escapes, and comments in a way that resembles the Unix shell (most closely in POSIX mode, which `shlex.split()` uses), but it performs no parameter, command, or glob expansion and is not a full `/bin/sh` parser, making it useful for splitting command strings in process orchestration.

---

## See also in Developer Toolchain

- [[CLI-Design-and-Agent-Friendly-Interfaces-AXI]], [[Python-Entry-Points-Plugins-and-Namespace-Packages]]

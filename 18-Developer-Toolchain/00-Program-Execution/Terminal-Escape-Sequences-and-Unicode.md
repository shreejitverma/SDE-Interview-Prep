---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Terminal Escape Sequences and Unicode

> [!summary] Scope
> Owns user-space terminal protocol parsing, ECMA-48 escape codes, CSI, OSC, and DCS sequence structures.
> Explains alternate screen buffers, bracketed paste mode, the Kitty keyboard protocol, UTF-8 state decoders, wcwidth calculation, and grapheme cluster segmentation.
> Reaches Use through Debug, handing pty driver management to TTYs-PTYs-and-Line-Discipline.md and text filter utilities to Sed-and-Awk-Stream-Processing.md.

## Related

- Prerequisites: [[TTYs-PTYs-and-Line-Discipline]]
- Next: [[Virtual-Memory-Mmap-and-Process-Address-Space]]
- See also: [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Escape-Sequences-ECMA-48-and-xterm|Escape Sequences ECMA 48 and xterm]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Terminal-Escape-Injection|Terminal Escape Injection]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/terminfo-and-tput|terminfo and tput]]
- Lab: `code/00-Program-Execution/terminal_escape_unicode/`

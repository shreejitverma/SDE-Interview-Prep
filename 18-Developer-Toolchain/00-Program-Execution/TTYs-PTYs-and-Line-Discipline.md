---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# TTYs, PTYs, and Line Discipline

> [!summary] Scope
> Owns the kernel teletype subsystem, UNIX 98 pseudoterminal master/slave multiplexing (/dev/ptmx, /dev/pts/*), and N_TTY line discipline processing.
> Details termios c_iflag, c_oflag, c_cflag, and c_lflag manipulation for canonical versus raw mode, alongside window size propagation via TIOCSWINSZ, TIOCGWINSZ, and SIGWINCH.
> Reaches Explain through Rebuild, leaving ANSI terminal escape byte rendering to Terminal-Escape-Sequences-and-Unicode.md.

## Related

- Prerequisites: [[Signals-Process-Groups-Sessions-and-Job-Control]]
- Next: [[Terminal-Escape-Sequences-and-Unicode]]
- See also: [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/TTY-Line-Discipline|TTY Line Discipline]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Pseudo-Terminals|Pseudo Terminals]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Canonical-and-Raw-Mode|Canonical and Raw Mode]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/termios-and-stty|termios and stty]]
- Lab: `code/00-Program-Execution/pty_raw_mode/` (requires: linux)

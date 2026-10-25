---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Zsh, Ksh, Dash, and Other Shells

Production code runs under many shells: dash as /bin/sh on Debian and Ubuntu, BusyBox ash in Alpine, zsh on macOS, ksh on commercial Unix, PowerShell and MSYS2 on Windows.
These notes map where each shell runs, how each differs from POSIX and bash, and how to choose.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Shell Landscape What Runs Where](Shell-Landscape-What-Runs-Where.md)
2. [Zsh versus POSIX](Zsh-versus-POSIX.md)
3. [Zsh Expansion Flags](Zsh-Expansion-Flags.md)
4. [Zsh Glob Qualifiers](Zsh-Glob-Qualifiers.md)
5. [Zsh Line Editor Widgets](Zsh-Line-Editor-Widgets.md)
6. [Zsh Completion System](Zsh-Completion-System.md)
7. [Zsh Startup Files](Zsh-Startup-Files.md)
8. [Zsh Emulation Modes](Zsh-Emulation-Modes.md)
9. [Dash and Bashisms](Dash-and-Bashisms.md)
10. [Strict POSIX Shells: posh and yash](Strict-POSIX-Shells-posh-and-yash.md)
11. [ksh93 Features](ksh93-Features.md)
12. [mksh](mksh.md)
13. [BusyBox ash](BusyBox-ash.md)
14. [Fish](Fish.md)
15. [PowerShell Pipelines](PowerShell-Pipelines.md)
16. [Nushell](Nushell.md)
17. [Oils and YSH](Oils-and-YSH.md)
18. [Elvish](Elvish.md)
19. [Windows Shells: WSL, MSYS2, and Cygwin](Windows-Shells-WSL-MSYS2-and-Cygwin.md)
20. [Choosing a Shell](Choosing-a-Shell.md)

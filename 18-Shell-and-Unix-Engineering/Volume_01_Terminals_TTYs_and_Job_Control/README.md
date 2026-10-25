---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 01 - Terminals, TTYs, and Job Control

An interactive shell talks to a terminal device, not to a keyboard.
This volume follows a keystroke through the TTY line discipline, pseudo-terminals, escape sequences, and job control, so that Ctrl-C, Ctrl-Z, and "works in my terminal but not in cron" stop being mysteries.

## Prerequisites

- [Volume 00 - The Unix Process Model Under the Shell](../Volume_00_The_Unix_Process_Model/README.md)

## Chapters

1. [The TTY Subsystem and the Line Discipline](Chapter_01_TTY_Subsystem_and_Line_Discipline.md) - Canonical and raw modes, echo, special characters, signal-generating keys, and the exact point where Ctrl-C becomes SIGINT.
2. [termios and stty](Chapter_02_Termios_and_Stty.md) - Terminal attributes, saving and restoring modes, and reading single keys and passwords safely from a script.
3. [Pseudo-Terminals](Chapter_03_Pseudo_Terminals.md) - The pty master and slave pair, how ssh, tmux, script, and expect allocate them, and why programs behave differently without a terminal.
4. [Escape Sequences: ECMA-48, VT100, and xterm](Chapter_04_Escape_Sequences_ECMA_48_VT100_and_Xterm.md) - Control sequences for cursor movement, color, and modes, OSC sequences, terminfo and tput, and terminal escape injection.
5. [Job Control End to End](Chapter_05_Job_Control_End_to_End.md) - SIGTSTP, SIGTTIN, SIGTTOU, and SIGCONT, the foreground process group, and how fg, bg, jobs, and wait drive them.
6. [Line Editing: Readline and ZLE](Chapter_06_Line_Editing_Readline_and_ZLE.md) - Keymaps, inputrc, history expansion, completion hooks, and line editor widgets.
7. [Terminal Multiplexers and Emulators](Chapter_07_Terminal_Multiplexers_and_Emulators.md) - The tmux and screen client-server architecture, terminal emulator rendering, and TERM negotiation.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v01-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

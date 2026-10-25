---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Terminals and Job Control

An interactive shell talks to a terminal device, not a keyboard.
These notes follow a keystroke through the line discipline, pseudo-terminals, escape sequences, and job control.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [TTY Line Discipline](TTY-Line-Discipline.md)
2. [Canonical and Raw Mode](Canonical-and-Raw-Mode.md)
3. [termios and stty](termios-and-stty.md)
4. [Pseudo Terminals](Pseudo-Terminals.md)
5. [Escape Sequences: ECMA-48 and xterm](Escape-Sequences-ECMA-48-and-xterm.md)
6. [terminfo and tput](terminfo-and-tput.md)
7. [Job Control Signals](Job-Control-Signals.md)
8. [fg, bg, jobs, and the Foreground Process Group](fg-bg-jobs-and-the-Foreground-Process-Group.md)
9. [Readline and inputrc](Readline-and-inputrc.md)
10. [Terminal Multiplexers: tmux and screen](Terminal-Multiplexers-tmux-and-screen.md)
11. [Terminal Emulators and TERM](Terminal-Emulators-and-TERM.md)
12. [Terminal Escape Injection](Terminal-Escape-Injection.md)
13. [expect and Automating Interactive Programs](expect-and-Automating-Interactive-Programs.md)

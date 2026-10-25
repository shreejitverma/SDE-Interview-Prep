---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 05 - Zsh, Ksh, Dash, BusyBox, and the Shell Landscape

Production code runs under many shells: dash as `/bin/sh` on Debian and Ubuntu, BusyBox ash in Alpine containers, zsh as the macOS login shell, and ksh on commercial Unix.
This volume maps where each shell runs, how each one differs from POSIX and from bash, and how to choose.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)

## Chapters

1. [The Shell Landscape: What Runs Where](Chapter_01_The_Shell_Landscape_What_Runs_Where.md) - Which shell is `/bin/sh` and the login shell on each major operating system and container base image, and the compatibility level of each.
2. [Zsh as a Language](Chapter_02_Zsh_as_a_Language.md) - How zsh differs from POSIX sh, no field splitting of parameters by default, arrays indexed from 1, emulate, and setopt.
3. [Zsh Expansion Flags and Glob Qualifiers](Chapter_03_Zsh_Expansion_Flags_and_Glob_Qualifiers.md) - Parameter expansion flags, glob qualifiers, and extended globbing.
4. [The Zsh Line Editor and Completion System](Chapter_04_Zsh_Line_Editor_and_Completion_System.md) - ZLE widgets, the compsys completion system and writing completion functions, and zsh startup files.
5. [Dash and Minimal POSIX Shells](Chapter_05_Dash_and_Minimal_POSIX_Shells.md) - The design goals of dash, why Debian and Ubuntu use it as `/bin/sh`, posh and yash as strict references, and the bashisms that break.
6. [KornShell: ksh93 and mksh](Chapter_06_KornShell_ksh93_and_mksh.md) - ksh93 features, discipline functions and compound variables, and mksh.
7. [BusyBox ash in Containers and Embedded Systems](Chapter_07_BusyBox_ash_in_Containers_and_Embedded_Systems.md) - The BusyBox multi-call binary, the limits of its ash, and Alpine-based images.
8. [Fish and Interactive-First Shells](Chapter_08_Fish_and_Interactive_First_Shells.md) - The design of fish, its deliberate incompatibility with POSIX, and when it is the right choice.
9. [Structured-Data Shells: PowerShell, Nushell, Oils, and Elvish](Chapter_09_Structured_Data_Shells.md) - Object and table pipelines, what they fix about byte streams, and what they cost.
10. [Choosing a Shell](Chapter_10_Choosing_a_Shell.md) - Decision rules for scripts versus interactive use, portability targets, and the macOS bash 3.2 and zsh defaults.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v05-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

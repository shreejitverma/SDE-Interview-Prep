---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 19 - History, Papers, and Primary Sources

The shell's design choices make sense once you know the machines, people, and papers they came from.
This volume reads the primary sources, from the Thompson shell to modern shell language design.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)

## Chapters

1. [The Thompson and PWB Shells](Chapter_01_The_Thompson_and_PWB_Shells.md) - The first Unix shell, the PWB (Mashey) shell, and the arrival of pipes.
2. [The Bourne Shell](Chapter_02_The_Bourne_Shell.md) - Steve Bourne's design, its ALGOL 68 influenced source code, and its legacy.
3. [csh and the C Shell Critique](Chapter_03_Csh_and_the_C_Shell_Critique.md) - Bill Joy's C shell, its interactive innovations, and Tom Christiansen's "Csh Programming Considered Harmful".
4. [The KornShell and POSIX Standardization](Chapter_04_The_KornShell_and_POSIX_Standardization.md) - David Korn's shell, POSIX.2, and the choices the standard made.
5. [rc, es, and Plan 9](Chapter_05_Rc_Es_and_Plan_9.md) - Tom Duff's rc shell, what its design fixed, and its descendants.
6. [The Unix Philosophy Papers](Chapter_06_The_Unix_Philosophy_Papers.md) - "The UNIX Time-Sharing System" (1974), McIlroy on pipes, and "Program Design in the UNIX Environment".
7. [Modern Shell Language Design](Chapter_07_Modern_Shell_Language_Design.md) - The Oils project, structured shells, and lessons for language designers.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v19-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

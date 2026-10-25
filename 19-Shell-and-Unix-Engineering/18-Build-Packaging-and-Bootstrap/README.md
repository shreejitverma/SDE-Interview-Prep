---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Build, Packaging, and Bootstrap

Build systems, package managers, and installers are large shell programs that most engineers never read.
These notes cover Make recipes, configure scripts, Nix builders, package hooks, and installer design.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Make Recipe Execution](Make-Recipe-Execution.md)
2. [.ONESHELL and .SHELLFLAGS](ONESHELL-and-SHELLFLAGS.md)
3. [Autoconf and configure](Autoconf-and-configure.md)
4. [Portable sh for configure](Portable-sh-for-configure.md)
5. [Nix Builders and stdenv Phases](Nix-Builders-and-stdenv-Phases.md)
6. [writeShellApplication](writeShellApplication.md)
7. [Homebrew Formulas](Homebrew-Formulas.md)
8. [Debian Maintainer Scripts](Debian-Maintainer-Scripts.md)
9. [Installer Script Design](Installer-Script-Design.md)

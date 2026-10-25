---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 14 - Build Systems and Packaging

Build systems and package managers are large shell programs that most engineers never read.
This volume covers Make recipes, autoconf configure scripts, Nix builders, package hooks, and installer design.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)
- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)

## Chapters

1. [Make as a Shell Driver](Chapter_01_Make_as_a_Shell_Driver.md) - Recipe execution, one shell per line, `.ONESHELL`, `SHELL` and `.SHELLFLAGS`, and error propagation.
2. [Autoconf and Portable configure Scripts](Chapter_02_Autoconf_and_Portable_Configure_Scripts.md) - M4 generation, the constraints of portable sh, and reading a configure script.
3. [Nix Builders and stdenv](Chapter_03_Nix_Builders_and_Stdenv.md) - Derivation builders, setup.sh phases and hooks, and writeShellApplication.
4. [Homebrew, apt, and Package Hooks](Chapter_04_Package_Manager_Hooks.md) - Formula install blocks, Debian maintainer scripts, and their failure modes.
5. [Designing Installer Scripts](Chapter_05_Designing_Installer_Scripts.md) - Platform detection, idempotency, uninstall, and verification.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v14-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

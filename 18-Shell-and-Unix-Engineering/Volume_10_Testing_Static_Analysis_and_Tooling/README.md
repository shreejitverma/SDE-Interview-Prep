---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 10 - Testing, Static Analysis, and Tooling

Shell code is testable and statically analyzable, and the tooling is mature.
This volume covers ShellCheck and shfmt, the bats-core, ShellSpec, and shUnit2 test frameworks, mocking and golden tests, coverage, cross-shell CI matrices, and reviewing shell at staff level.
The labs in this curriculum use exactly this toolchain.

## Prerequisites

- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)

## Chapters

1. [ShellCheck in Depth](Chapter_01_ShellCheck_in_Depth.md) - How ShellCheck parses, its warning families and the bug each one prevents, directives, and configuration.
2. [shfmt and Style](Chapter_02_Shfmt_and_Style.md) - Formatting, dialect detection, EditorConfig integration, and simplification.
3. [Unit Testing with bats-core](Chapter_03_Unit_Testing_with_Bats_Core.md) - Test structure, run, helpers, setup and teardown, and the bats-support and bats-assert libraries.
4. [ShellSpec and shUnit2](Chapter_04_ShellSpec_and_ShUnit2.md) - Behavior-driven shell testing, cross-shell support, and choosing a framework.
5. [Mocking, Stubbing, and Golden Tests](Chapter_05_Mocking_Stubbing_and_Golden_Tests.md) - PATH-based command mocks, fake clocks and filesystems, and golden-output tests.
6. [Coverage for Shell](Chapter_06_Coverage_for_Shell.md) - kcov and bashcov mechanics, and what line coverage means for shell.
7. [CI Matrices Across Shells and Operating Systems](Chapter_07_CI_Matrices_Across_Shells_and_Operating_Systems.md) - A container per shell, macOS runners, and pinning tool versions.
8. [Reviewing Shell Code at Staff Level](Chapter_08_Reviewing_Shell_Code_at_Staff_Level.md) - A review checklist, ranking risk, and when to demand a rewrite in another language.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v10-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Testing and Static Analysis

Shell code is testable and statically analyzable, and the tooling is mature.
These notes cover ShellCheck, shfmt, bats-core, ShellSpec, shUnit2, mocking, coverage, cross-shell CI, and staff-level review.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [ShellCheck Internals](ShellCheck-Internals.md)
2. [ShellCheck Warning Families](ShellCheck-Warning-Families.md)
3. [shfmt](shfmt.md)
4. [bats-core](bats-core.md)
5. [bats Helpers and Assertions](bats-Helpers-and-Assertions.md)
6. [ShellSpec](ShellSpec.md)
7. [shUnit2](shUnit2.md)
8. [Mocking Commands via PATH](Mocking-Commands-via-PATH.md)
9. [Golden Output Tests](Golden-Output-Tests.md)
10. [Characterization Tests for Legacy Scripts](Characterization-Tests-for-Legacy-Scripts.md)
11. [Coverage with kcov](Coverage-with-kcov.md)
12. [Cross Shell CI Matrices](Cross-Shell-CI-Matrices.md)
13. [Reviewing Shell at Staff Level](Reviewing-Shell-at-Staff-Level.md)

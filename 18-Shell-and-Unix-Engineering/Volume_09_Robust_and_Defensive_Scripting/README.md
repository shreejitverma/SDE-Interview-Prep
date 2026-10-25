---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 09 - Robust and Defensive Scripting

Most shell scripts work on the happy path and fail silently on every other one.
This volume is the pattern catalog for scripts that fail loudly, clean up after themselves, can be rerun safely, and never leave a half-written result behind.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)
- [Volume 03 - Bash in Depth](../Volume_03_Bash_in_Depth/README.md)

## Chapters

1. [Unofficial Strict Mode and Its Failure Modes](Chapter_01_Unofficial_Strict_Mode_and_Its_Failure_Modes.md) - `set -euo pipefail`: what each option catches, what it misses, and better alternatives.
2. [Error Handling Patterns That Work](Chapter_02_Error_Handling_Patterns_That_Work.md) - Explicit checks, die functions, ERR traps, and propagating errors out of functions and subshells.
3. [Cleanup and Traps](Chapter_03_Cleanup_and_Traps.md) - Temporary resources, EXIT and signal traps, re-raising signals, and nested cleanup.
4. [Idempotency and Atomic Writes](Chapter_04_Idempotency_and_Atomic_Writes.md) - Safe reruns, mktemp in the target filesystem, rename for atomic replacement, and fsync.
5. [Locking and Mutual Exclusion](Chapter_05_Locking_and_Mutual_Exclusion.md) - flock, mkdir locks, lock files and PID reuse, and recovering stale locks.
6. [Retries, Backoff, and Timeouts](Chapter_06_Retries_Backoff_and_Timeouts.md) - Exponential backoff with jitter, bounded retries, and timeout wrappers.
7. [Argument Parsing and CLI Design](Chapter_07_Argument_Parsing_and_CLI_Design.md) - getopts, long options, the double-dash convention, help text, and exit code contracts.
8. [Configuration, Environment, and Logging](Chapter_08_Configuration_Environment_and_Logging.md) - Precedence rules, input validation, logging to stderr, and machine-readable output.
9. [Dry Run and Apply Design](Chapter_09_Dry_Run_and_Apply_Design.md) - Separating planning from execution, reviewable plans, and reversible operations.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v09-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

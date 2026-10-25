---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 21 - Capstones

Five projects that prove mastery end to end.
Each capstone has a specification, a test suite that defines done, and a review checklist.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)
- [Volume 03 - Bash in Depth](../Volume_03_Bash_in_Depth/README.md)
- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)
- [Volume 10 - Testing, Static Analysis, and Tooling](../Volume_10_Testing_Static_Analysis_and_Tooling/README.md)

## Chapters

1. [Capstone 1: A POSIX-Subset Shell in C](Chapter_01_Capstone_A_POSIX_Subset_Shell_in_C.md) - Pipelines, redirection, quoting, expansions, builtins, and job control, tested against dash as the oracle.
2. [Capstone 2: A Production CLI in Bash](Chapter_02_Capstone_A_Production_CLI_in_Bash.md) - Argument parsing, completions, a man page, a dry run, structured errors, a test suite, and a CI matrix.
3. [Capstone 3: Rescuing a Legacy Script](Chapter_03_Capstone_Rescuing_a_Legacy_Script.md) - Characterization tests first, then a safe refactor.
4. [Capstone 4: A Multi-Gigabyte Data Pipeline](Chapter_04_Capstone_A_Multi_Gigabyte_Data_Pipeline.md) - A benchmark of shell against Python and a compiled tool, and a written decision memo.
5. [Capstone 5: An Upstream Contribution](Chapter_05_Capstone_An_Upstream_Contribution.md) - A ShellCheck rule, a bats-core fix, or a well-formed bash bug report.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v21-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

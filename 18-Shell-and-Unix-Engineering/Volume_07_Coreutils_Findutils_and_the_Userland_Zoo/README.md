---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 07 - Coreutils, Findutils, and the Userland Zoo

A shell script is only as portable as the utilities it calls.
This volume compares the GNU, BSD and macOS, BusyBox, and uutils userlands, then goes deep on the utilities whose flags and edge cases cause most production bugs.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)

## Chapters

1. [Four Userlands: GNU, BSD and macOS, BusyBox, and uutils](Chapter_01_Four_Userlands_GNU_BSD_BusyBox_and_uutils.md) - History and design differences of the four userlands, and their current distribution adoption.
2. [Portability Traps Between Userlands](Chapter_02_Portability_Traps_Between_Userlands.md) - `sed -i`, date, stat, readlink, xargs, `grep -P`, mktemp, and other flags that diverge.
3. [find in Depth](Chapter_03_Find_in_Depth.md) - Expressions and operators, `-print0`, `-exec` with plus and semicolon, `-prune`, time and size tests, and traversal order.
4. [xargs and Argument Batching](Chapter_04_Xargs_and_Argument_Batching.md) - `-0`, `-n`, `-P`, and `-I`, batching under `ARG_MAX`, and safe filename handling.
5. [sort, uniq, and External Merge Sort](Chapter_05_Sort_Uniq_and_External_Merge_Sort.md) - Keys and field rules, stability, `-S` and temporary files, `--parallel`, and numeric, version, and human sorting.
6. [Files and Bytes: cp, mv, dd, tee, and install](Chapter_06_Files_and_Bytes_cp_mv_dd_tee_and_install.md) - Reflinks and copy_file_range, sparse files, rename semantics across filesystems, and atomic installs.
7. [stdio Buffering and stdbuf](Chapter_07_Stdio_Buffering_and_Stdbuf.md) - Full, line, and unbuffered modes, why pipelines appear to stall, and how stdbuf works.
8. [Process Utilities: timeout, flock, nohup, setsid, nice, and env](Chapter_08_Process_Wrapper_Utilities.md) - The mechanisms and portability of the utilities that wrap other commands.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v07-cNN-<slug>`, with a bats test that runs under every shell in the matrix.

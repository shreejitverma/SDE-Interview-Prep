---
type: playbook
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "https://bats-core.readthedocs.io/"
  - "https://www.shellcheck.net/wiki/"
  - "https://github.com/mvdan/sh"
---

# Shell Labs and the Shell Matrix

Every worked example in these notes is a lab in this folder, and every portability claim is proven by running that lab under many shells.
This note is the contract for writing a lab and the manual for running the gate and the matrix.

## Layout

```
labs/
  README.md          this note
  .shellcheckrc      ShellCheck options for every lab (found by searching upward from each script)
  .editorconfig      shfmt formatting for every lab file, suffixed or not (2-space indent, case indent, binary ops at line start)
  run-matrix.sh      runs every lab's bats tests once per shell, on the host or in the matrix images
  lib/lab.bash       shared bats helpers, listed under Helpers below
  lib/detach.c       the launcher that starts each matrix suite in a new session with a clean signal state
  matrix/            alpine.Dockerfile and debian.Dockerfile, the two container images of the matrix
  common/            shared lab tools, starting with showargs.sh
  NN-<note-stem>/    one lab per note, named after its folder number and lowercased note stem, for example 03-field-splitting-and-ifs
```

## The lab contract

1. A lab directory holds the scripts under test and at least one `.bats` file that tests them; `tools/check_shell_labs.py` fails a lab without a test.
2. Every shell file declares its dialect with a shebang, or with a `# shellcheck shell=bash` directive in its leading comment block when it is only sourced; the gate reads the shebang first, then the directive, then the suffix.
3. A file with a shebang is executable in git (`git update-index --chmod=+x FILE`).
4. ShellCheck and `shfmt -d` are clean; a `# shellcheck disable=SCnnnn` carries a reason on the same line.
5. Tests run the code under test through `run_lab` or `run_lab_c`, never by calling `sh` or `bash` directly, so the matrix can swap the shell.
6. When shells legitimately differ, the test asserts each family's actual behavior with `case $(lab_shell_family)`, rather than skipping a shell; the differences are the content.
7. The note shows output captured from a real run of the lab.
8. Labs are ASCII and never touch anything outside `BATS_TEST_TMPDIR`.

## Helpers in lib/lab.bash

`LAB_SHELL` names the shell under test as a command plus arguments, for example `dash`, `busybox ash`, or `zsh --emulate sh`; it defaults to `sh`.
`run-matrix.sh` also exports `LAB_EXPECT_FAMILY`, the family each shell must report, so `common/showargs.bats` fails when the matrix runs the wrong shell; it is empty for `sh`, `/bin/sh`, and `ksh`, whose family varies by host.

| Helper | Purpose |
|---|---|
| `run_lab SCRIPT [ARG...]` | Run a lab script under the shell under test, through bats `run`. |
| `run_lab_c CODE [NAME [ARG...]]` | Run inline code under the shell under test, like `sh -c`. |
| `lab_shell_family` | Print bash, zsh, zsh-sh, yash, busybox, posh, mksh, ksh93, or, when the shell sets no identifying variable, the name of the binary the command resolves to through symlinks (so Debian `sh` reports dash), detected by asking the shell itself. |
| `lab_shell_version` | Print the version the shell reports about itself, or `n/a` (dash has no version variable). |
| `lab_awk_flavor [CMD...]` | Print the implementation behind an awk command (default `awk`): gawk, mawk, onetrue, busybox, or unknown. The shell under test never changes which awk runs, so awk labs branch on this, not on the shell family. |
| `lab_awks` | List every awk command installed here (`awk`, `gawk`, `mawk`, `original-awk`, `nawk`, `busybox awk`), so one test can assert each implementation's behavior. |

`common/showargs.sh` prints the argument count and each argument in angle brackets, the standard probe for seeing what quoting, splitting, and globbing produced.

## Running

```sh
python3 tools/check_shell_labs.py                         # the gate: structure, ShellCheck, shfmt, zsh -n, bats (also in CI)
19-Shell-and-Unix-Engineering/labs/run-matrix.sh          # every lab under every shell on this machine
19-Shell-and-Unix-Engineering/labs/run-matrix.sh --list   # which shells this machine has, with versions
19-Shell-and-Unix-Engineering/labs/run-matrix.sh --containers           # the Alpine and Debian images
19-Shell-and-Unix-Engineering/labs/run-matrix.sh --containers common    # one lab only
```

Toolchain on macOS: `brew install shellcheck shfmt bats-core`, plus Docker or OrbStack for `--containers` (`orb start`).
The gate and CI pin ShellCheck 0.11.0, shfmt 3.14.1, and bats-core 1.14.0; use the same versions locally so warnings match.
The gate reads files through `git ls-files`, so stage a new lab (`git add`) before running it.
The gate and the matrix bound every test with `BATS_TEST_TIMEOUT` (default 60 seconds), so a lab that blocks fails instead of hanging; set the variable to override it, and `--containers` forwards it into the images.
Both also kill each shell's whole suite after `LAB_SUITE_TIMEOUT` (default 600 seconds; the matrix accepts only whole seconds), because a stray child that keeps bats's output open, such as a stopped process, outlives the per-test timeout; the suite then fails with `suite killed`, and `--containers` forwards this variable too.
Every suite runs as the leader of a new session, with no controlling terminal and stdin from `/dev/null`, in the gate, the matrix, the images, and CI alike.
It also starts from a clean signal state: `SIGINT` and `SIGQUIT` (in the matrix, every catchable signal) at their default dispositions and nothing blocked, whatever the caller ignored.
The matrix compiles `lib/detach.c` for this at the start of each run, on the host and inside each image, because a shell cannot do it: a non-interactive shell may not reset a signal ignored on entry, and it ignores `SIGINT` and `SIGQUIT` in its own background jobs.
So `set -m` is refused or disabled by most shells, `/dev/tty` cannot be opened, and a lab that needs a terminal must create a pseudo-terminal itself.
A failing matrix row prints the output of each failing test.
Every C probe, and `lib/detach.c`, is compiled with `${CC:-cc}`, so set `CC` to choose the compiler.

## The shell matrix

Shells observed by `run-matrix.sh` on 2026-10-25.
The script prints the live versions on every run; this table records one run so notes can cite it.

| Where | Shells |
|---|---|
| macOS host | `/bin/sh` and `/bin/bash` (bash 3.2.57), `bash` on PATH (5.3), zsh 5.9 native and with `--emulate sh`, dash, ksh93 (93u+ 2012-08-01), and the one true awk (version 20200816) as `awk` |
| `matrix/alpine.Dockerfile` (Alpine 3.22) | gcc for C probes, BusyBox ash 1.37.0 as `/bin/sh` and BusyBox awk as `awk`, bash 3.2.57, 4.4.23, 5.1.16, 5.2.37, and 5.3.20, dash 0.5.12, mksh R59, yash 2.58.1, zsh 5.9 |
| `matrix/debian.Dockerfile` (Debian 13 trixie) | gcc for C probes, dash 0.5.12 as `/bin/sh`, bash 5.2.37, posh 0.14.1, ksh93u+m 1.0.10, mksh R59, BusyBox ash 1.37.0, zsh 5.9, GNU coreutils 9.7, and three awks: mawk 1.3.4 (the default `awk`), gawk 5.2.1, and original-awk 2025-01-16 |

Container runs use an unprivileged `lab` user, so permission behavior matches a normal account.
A skipped test is counted and printed separately in every matrix row, never as a pass; with gcc in both images, a lab should not need to skip.
On a host where `sh` on PATH is not `/bin/sh` (a Nix or Homebrew setup), the matrix runs both, because they are different shells.

## Gotchas

- ShellCheck does not support zsh, so zsh files (a zsh shebang, even on a `.sh` file, or a `.zsh` suffix) get `shfmt` and `zsh -n` only; test their behavior in bats like any other lab.
- `zsh` and `zsh --emulate sh` are different shells for portability purposes: native zsh does not field split unquoted parameters.
- bats itself runs under bash; only the code passed to `run_lab` runs under the shell under test.

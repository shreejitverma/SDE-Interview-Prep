---
type: moc
track: [sde, distinguished, low-latency, quant-dev, ai-eng]
level:
status: draft
last_reviewed:
sources:
  - "SDE-Interview-Prep contributors, Developer Toolchain Verification Harness, SDE Interview Prep, 2026, https://github.com/shreejitverma/SDE-Interview-Prep"
---

# Developer Toolchain Code Labs and Verification Harness

This directory contains all runnable source code, test reproductions, rebuild implementations, and benchmark harnesses for the `18-Developer-Toolchain/` curriculum.
To ensure documentation integrity, every code excerpt, terminal transcript, and benchmark number presented in the knowledge notes is verified against real execution in this directory.

## Directory Layout

The directory structure mirrors the numbered pillar organization of the curriculum.
No Markdown files are permitted in `code/` other than this `README.md`:

```
code/
  Makefile                          # Master verification harness entry point
  _harness/                         # Verification harness
    verify.py                       # Discovers labs, checks requirements, runs, scrubs, and diffs
  00-Program-Execution/
    <lab_name>/
      run.sh                        # Lab execution entry point (run as `bash run.sh`)
      ...source files...            # C++, Python, Rust, or Shell source code
      expected/
        darwin-arm64.txt            # Golden output for macOS Apple Silicon
        linux-amd64.txt             # Golden output for Linux x86-64
        linux-aarch64.txt           # Golden output for Linux AArch64
  01-Command-Line/
  02-Version-Control/
  03-Debugging/
  04-Measurement-Methodology/
  05-Profiling/
  06-Build-Systems/
  07-Packaging-and-Release/
  08-Software-Supply-Chain/
  09-Engineering-Systems-at-Scale/
  11-Arena-and-Capstones/
```

## Platform Gating and Requires Tags

Different toolchain mechanisms require specific operating systems, CPU architectures, or execution privileges.
Each lab declares its environmental requirements in a `# requires:` comment within the first 10 lines of its `run.sh`:

- `# requires: linux`: Gated to native Linux environments. On macOS hosts the harness skips these labs; run `make verify` inside an OrbStack or Docker Linux environment to exercise them.
- `# requires: macos`: Gated to macOS hosts (Instruments, xctrace, codesign). On Linux hosts the harness skips these labs.
- `# requires: docker`: Requires an active Docker daemon to build OCI containers or test isolated build sandboxes.
- `# requires: pmu`: Requires access to hardware performance monitoring unit counters (e.g., instructions, cycles, cache misses), opted in with `TOOLCHAIN_LABS_PMU=1`. These cannot run inside virtualized hypervisors without PMU passthrough (an OrbStack VM exposes software events only).
- `# requires: nightly`: Requires a nightly Rust toolchain listed by `rustup toolchain list` (e.g., for Miri or `-Zsanitizer`).
- `# requires: network`: Requires active Internet access to communicate with upstream package registries (e.g., crates.io, PyPI, Conan Center), opted in with `TOOLCHAIN_LABS_NETWORK=1`.

If the current environment cannot satisfy a declared prerequisite, the harness prints a `SKIP` line naming the unmet requirement and never counts the lab as passed.

## Harness Commands

The master `Makefile` at the root of `code/` exposes the following standard targets:

### Verify All Compatible Labs
```bash
make verify
```
This command iterates over all labs across all pillars, evaluates `# requires:` tags against the host system, executes each eligible lab, scrubs heap addresses and temporary paths from stdout, and diffs it against the corresponding `expected/<platform>.txt` file.
Any output mismatch, failing lab, or missing expected file causes a non-zero exit code.

### Verify a Specific Lab
```bash
make verify LAB=capstone_mini_git
```
Runs the verification harness only for labs whose path contains the given text.
This allows rapid test-driven iteration during lab authoring and debugging.

### Record Golden Outputs
```bash
make record LAB=capstone_ptrace_debugger
```
Executes the specified lab and updates or creates its golden output file `expected/<platform>.txt` using the current platform key (e.g., `linux-aarch64`).
Always inspect the resulting diff carefully before committing.

### List Labs and Their Requirements
```bash
make list
```
Prints every lab directory with its `# requires:` line.
A note-to-lab crosscheck target is planned but not implemented yet.

## Authoring a New Lab

When contributing a new lab:
1. Create `code/<pillar>/<lab_name>/` using lowercase snake_case.
2. Provide a self-contained `run.sh` entry point (the harness runs it with `bash run.sh` and reads its stdout).
3. Add the `# requires:` comment within the first 10 lines of `run.sh` specifying platform dependencies.
4. Run `make record LAB=<pillar>/<lab_name>/` on the required platform to generate the initial golden output.
5. Reference the lab in the owning Markdown note using a standard relative link to the lab directory.

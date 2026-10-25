---
type: moc
track: [sde, distinguished, low-latency, quant-dev, ai-eng]
level:
status: draft
last_reviewed:
sources:
  - "Antigravity Engineering Team, Developer Toolchain Verification Harness, SDE Interview Prep, 2026, https://github.com/shreejitverma/SDE-Interview-Prep"
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
  _harness/                         # Core execution, diffing, and crosscheck scripts
    runner.py                       # Lab runner and platform dependency evaluator
    crosscheck.py                   # Validates note code excerpts against lab output
  00-program-execution/
    <lab_name>/
      Makefile or run.sh            # Lab execution entry point
      ...source files...            # C++, Python, Rust, or Shell source code
      expected/
        darwin-arm64.out            # Golden output for macOS Apple Silicon
        linux-x86_64.out            # Golden output for Linux x86-64
        linux-arm64.out             # Golden output for Linux AArch64
  01-command-line/
  02-version-control/
  03-debugging/
  04-measurement-methodology/
  05-profiling/
  06-build-systems/
  07-packaging-and-release/
  08-software-supply-chain/
  09-engineering-systems-at-scale/
  11-arena-and-capstones/
```

## Platform Gating and Requires Tags

Different toolchain mechanisms require specific operating systems, CPU architectures, or execution privileges.
Each lab declares its environmental requirements in the first lines of its primary script or `Makefile`:

- `# requires: linux`: Gated to native Linux environments. On macOS hosts, the harness automatically dispatches these labs inside an OrbStack or Docker container.
- `# requires: docker`: Requires an active Docker daemon to build OCI containers or test isolated build sandboxes.
- `# requires: pmu`: Requires access to hardware performance monitoring unit counters (e.g., instructions, cycles, cache misses). These cannot run inside virtualized hypervisors without PMU passthrough and require bare-metal Linux.
- `# requires: nightly`: Requires nightly language toolchains (e.g., Rust nightly for Miri or `-Zsanitizer`).
- `# requires: network`: Requires active Internet access to communicate with upstream package registries (e.g., crates.io, PyPI, Conan Center).

If the current environment cannot satisfy a declared prerequisite, the harness cleanly skips the lab and records the skip reason in the verification report rather than failing.

## Harness Commands

The master `Makefile` at the root of `code/` exposes the following standard targets:

### Verify All Compatible Labs
```bash
make verify
```
This command iterates over all labs across all pillars, evaluates `# requires:` tags against the host system, executes each eligible lab, and diffs stdout/stderr against the corresponding `expected/<target>.out` file.
Any output mismatch causes a non-zero exit code.

### Verify a Specific Lab
```bash
make verify LAB=capstone_mini_git
```
Runs the verification harness exclusively for the named lab directory.
This allows rapid test-driven iteration during lab authoring and debugging.

### Record Golden Outputs
```bash
make record LAB=capstone_ptrace_debugger
```
Executes the specified lab and updates or creates its golden output file under `expected/<target>.out` using the current platform target tuple (e.g., `linux-arm64`).
Always inspect the resulting diff carefully before committing.

### Crosscheck Notes Against Code
```bash
make crosscheck
```
Scans all Markdown notes in `18-Developer-Toolchain/` to ensure that every code block labeled with a lab link matches the verified source or output in `code/`.

## Authoring a New Lab

When contributing a new lab:
1. Create `code/<pillar>/<lab_name>/` using lowercase snake_case.
2. Provide a self-contained entry point (`Makefile` or executable `run.sh`).
3. Add the `# requires:` header comment specifying platform dependencies.
4. Run `make record LAB=<lab_name>` on the required platform to generate the initial golden output.
5. Reference the lab in the owning Markdown note using a standard relative link to the lab directory.

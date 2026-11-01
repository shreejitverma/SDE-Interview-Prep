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
    linux-labs.Dockerfile           # Pinned image for `# requires: linux` labs on a non-Linux host
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

- `# requires: linux`: Needs a Linux kernel.
  On a Linux host the lab runs natively; on any other host it runs in a container, as described in the section on container dispatch below.
- `# requires: macos`: Gated to macOS hosts (Instruments, xctrace, codesign).
  On Linux hosts the harness skips these labs.
- `# requires: docker`: Requires an active Docker daemon to build OCI containers or test isolated build sandboxes.
- `# requires: pmu`: Requires access to hardware performance monitoring unit counters (e.g., instructions, cycles, cache misses), opted in with `TOOLCHAIN_LABS_PMU=1`.
  These cannot run inside virtualized hypervisors without PMU passthrough (an OrbStack VM exposes software events only, and `perf_event_open` for cycles returned `ENOENT` even in a privileged OrbStack container on 2026-11-01).
- `# requires: nightly`: Requires a nightly Rust toolchain listed by `rustup toolchain list` (e.g., for Miri or `-Zsanitizer`).
- `# requires: network`: Requires active Internet access to communicate with upstream package registries (e.g., crates.io, PyPI, Conan Center), opted in with `TOOLCHAIN_LABS_NETWORK=1`.

If the current environment cannot satisfy a declared prerequisite, the harness prints a `SKIP` line naming the unmet requirement and never counts the lab as passed.

## Container Dispatch for Linux Labs

When a lab requires `linux`, the host is not Linux, and `docker info` succeeds, `make verify` and `make record` run the lab in a container and say so on its result line.

- **Image:** `_harness/linux-labs.Dockerfile`, tagged `toolchain-labs:<hash of the Dockerfile>` and built on first use, so editing the file forces a rebuild.
  The base is pinned by digest.
  The packages come from the live archive at build time, because Ubuntu's archive snapshots do not cover `ports.ubuntu.com`, where arm64 lives.
  A rebuild can therefore pull newer tools; `make verify` reports any change that matters, and `/etc/toolchain-labs-versions` in the image lists what was installed.
- **Isolation:** only the lab's directory is mounted, read-only, at `/mnt/lab`; the lab runs on a private copy at `/tmp/lab`, so it cannot dirty the tree.
  A dispatched lab therefore cannot reach its siblings through `../`.
- **Privileges:** the image's unprivileged `lab` user with `--cap-drop ALL`, `--security-opt no-new-privileges`, `--network none`, and `--ulimit core=-1`; never `--privileged`.
  Verified on OrbStack (as of 2026-11, kernel 7.2): this is enough for a kernel core and for GDB to run and dump its own child.
  A lab that needs a capability fails visibly until the harness grants it on purpose.
- **Expected file:** `expected/linux-<arch>.txt`, using the arch `uname -m` reports inside the container, not the host's.
  A daemon on another arch reports `MISS` until that file is recorded; `core_dump_triage` has only `linux-aarch64.txt`, so an amd64 daemon misses until `linux-amd64.txt` exists.
- **Limit:** `core_pattern` belongs to the VM's kernel, and a container cannot change it, so a Docker VM whose pattern pipes to a handler sends `core_dump_triage` down its GDB fallback and the `Core source:` line fails the diff.
  That failure is correct: the run did not exercise what the recording did.
  OrbStack's pattern is `core`.

| Situation | Result |
|---|---|
| `TOOLCHAIN_LABS_CONTAINER=0`, no `docker` on `PATH`, or `docker info` fails or hangs | `SKIP` naming the reason |
| Image build failure, docker error, non-zero exit in the container, or timeout | `FAIL` with stderr; a timed-out container is removed |
| `linux, pmu` | `SKIP`, even with `TOOLCHAIN_LABS_PMU=1`: the VM has no hardware counters |
| `linux, nightly` | `SKIP`: the image has no Rust toolchain |
| `linux, docker` | `SKIP`: the container gets no docker socket, which would hand it root on the VM |
| `linux, network` | `SKIP`, even with `TOOLCHAIN_LABS_NETWORK=1`: the container always runs with `--network none` |
| `macos`, alone or with `linux` | Never dispatches |

## Harness Commands

The master `Makefile` at the root of `code/` exposes the following standard targets:

### Verify All Compatible Labs
```bash
make verify
```
This command iterates over all labs across all pillars, evaluates `# requires:` tags against the host system, executes each eligible lab (in a container for a `linux` lab on a non-Linux host), scrubs heap addresses and temporary paths from stdout, and diffs it against the corresponding `expected/<platform>.txt` file.
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
Executes the specified lab and updates or creates its golden output file `expected/<platform>.txt` using the current platform key (e.g., `linux-aarch64`), which is the container's for a dispatched lab.
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
5. Reference the lab in the owning Markdown note with a `Lab:` code-span line under `## Related`, in the format of STYLE.md section 4.

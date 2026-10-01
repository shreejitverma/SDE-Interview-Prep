---
type: playbook
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: [https://lima-vm.io/docs/]
course: cs6210
lessons: []
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# AOS Lab Environment

Every AOS lab runs in one reproducible Linux VM, created with [Lima](https://lima-vm.io/docs/) on macOS.
On Apple M3 or later the VM gets nested KVM, so the virtualization labs can boot real guests inside it.

## Create the VM

```sh
brew install lima
cd 01-CS-Foundations/Operating-Systems/AOS/labs/setup
make vm          # limactl create --name=aos aos-lab.yaml && limactl start aos
make install     # installs every lab package inside the VM (about 10 minutes)
limactl stop aos && limactl start aos   # pick up the kvm and libvirt groups and the rtprio limit
make check-env   # reports what this machine can and cannot do
make shell       # a shell in the VM at the labs folder
```

The VM mounts the vault checkout and the treehouse worktree pool writable, so you edit on macOS and build in the VM.
Every lab folder has `make`, `make run`, and `make test`; run them inside the VM.

## What was verified (2026-10-01)

Host: Apple M3 Pro, 12 cores, 36 GB, macOS 27.2, Lima 2.2.0 with `vmType: vz` and `nestedVirtualization: true`.

Guest: Ubuntu 24.04.4 LTS, kernel 6.8.0-134-generic, 8 vCPUs, 12 GiB; gcc 13, clang 18.1.3, Python 3.12.3, QEMU 8.2.2, libvirt 10.0.0, perf 6.8.12, bpftrace 0.20.2, Open MPI 4.1.6, protoc 3.21.12.

| Capability | Status | Used by |
| --- | --- | --- |
| `/dev/kvm` (nested KVM) | works; a nested guest boots UEFI with `qemu-system-aarch64 -accel kvm -cpu host -M virt` | lab-03 |
| libvirt daemon | works after the relogin above | lab-03 |
| perf software events, tracepoints, kprobes, uprobes, bpftrace | works (`perf_event_paranoid = 1`) | lab-01, lab-02, lab-08 |
| perf hardware counters (cycles, cache misses, TLB misses, `perf c2c`) | **not available**: Apple's Virtualization.framework exposes no PMU to guests | labs measure with `clock_gettime` and the ARM virtual counter instead |
| `SCHED_FIFO` and `chrt` | works after relogin (rtprio limit 99); `SCHED_DEADLINE` needs `sudo` | lab-08, lab-22 |
| FUSE | works | lab-16 |
| Python venv `/opt/aos-venv` | grpcio, pytest, matplotlib, cryptography, fusepy | many labs |
| NUMA | the VM has one NUMA node; `numactl --hardware` shows it, and NUMA effects are explained rather than measured | lab-09 |

> [!note] Hardware counters
> If you need real cache and TLB counters, run the same lab on a bare-metal Linux x86 or ARM machine; every lab's Makefile works there unchanged.
> On the VM, compare relative timings, not absolute cycle counts.

## Files

- `aos-lab.yaml`: the Lima VM definition.
- `install-packages.sh`: the package list and the sysctl and limits changes; idempotent.
- `check-env.sh`: the capability report above.
- `Makefile`: `vm`, `install`, `check-env`, and `shell` targets.

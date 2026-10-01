#!/usr/bin/env bash
# Report which lab capabilities this machine has. Exit 1 only if a required tool is missing.
set -u
missing=0
need() { if command -v "$1" >/dev/null 2>&1; then printf 'ok      %s\n' "$1"; else printf 'MISSING %s\n' "$1"; missing=1; fi; }
have() { if eval "$2" >/dev/null 2>&1; then printf 'ok      %s\n' "$1"; else printf 'absent  %s (labs that need it will say so)\n' "$1"; fi; }
printf 'kernel  %s %s\n' "$(uname -r)" "$(uname -m)"
printf 'cpus    %s\n' "$(nproc)"
for t in gcc clang make python3 perf bpftrace numactl lstopo mpicc mpirun protoc fio iperf3 cyclictest stress-ng strace virsh qemu-system-aarch64; do need "$t"; done
have "/dev/kvm (nested KVM)" "test -e /dev/kvm"
have "libvirt daemon" "virsh -c qemu:///system list"
have "perf software events" "perf stat -e task-clock true"
have "perf hardware counters (absent under Apple vz)" "! perf stat -e cycles true 2>&1 | grep -q 'not supported'"
have "SCHED_FIFO (chrt, needs relogin after install)" "chrt -f 10 true"
have "fuse" "test -e /dev/fuse"
have "aos venv" "/opt/aos-venv/bin/python -c 'import grpc, pytest'"
exit $missing

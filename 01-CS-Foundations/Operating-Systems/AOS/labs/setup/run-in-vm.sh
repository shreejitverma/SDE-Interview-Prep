#!/usr/bin/env bash
# Run a lab's make target inside the Lima VM, from any checkout path.
#   labs/setup/run-in-vm.sh labs/lab-05-spinlocks test
#   labs/setup/run-in-vm.sh labs/lab-05-spinlocks capture   # runs `make run`, saves expected-output.txt here
# The lab folder is copied into the VM (/tmp/aoslab/<name>), so the checkout need not be mounted.
set -euo pipefail
VM="${AOS_VM:-aos}"
dir="${1:?usage: run-in-vm.sh <lab-dir> [make-target|capture]}"
target="${2:-test}"
name="$(basename "$(cd "$dir" && pwd)")"
remote="/tmp/aoslab/$name"
COPYFILE_DISABLE=1 tar -C "$dir" --no-xattrs --exclude='*.o' --exclude='build' -cf - . |
  limactl shell "$VM" bash -c "rm -rf '$remote' && mkdir -p '$remote' && tar -C '$remote' -xf -"
if [ "$target" = "capture" ]; then
  limactl shell "$VM" bash -lc "cd '$remote' && make >/dev/null && {
      echo \"# captured \$(date -u +%Y-%m-%dT%H:%MZ) on \$(uname -srm), \$(nproc) vCPUs, Apple M3 Pro host via Lima vz\";
      make -s run; }" > "$dir/expected-output.txt"
  echo "wrote $dir/expected-output.txt"
else
  limactl shell "$VM" bash -lc "cd '$remote' && make -s $target"
fi

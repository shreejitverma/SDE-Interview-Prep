#!/bin/bash
set -e

echo "=== IPC Ping-Pong Benchmarks ==="
./ipc_bench pipe 64
./ipc_bench unix 64
./ipc_bench shm 64
./ipc_bench pipe 4096
./ipc_bench unix 4096
./ipc_bench shm 4096

echo ""
echo "=== IPC Strace Profile (Pipe, 10000 iters of 64 bytes) ==="
strace -c -f ./ipc_bench pipe 64 2>&1 | grep -E "read|write|futex" || true

echo ""
echo "=== IPC Strace Profile (SHM, 10000 iters of 64 bytes) ==="
strace -c -f ./ipc_bench shm 64 2>&1 | grep -E "read|write|futex" || true

echo ""
echo "=== Sendfile vs Read/Write (100MB) ==="
./sendfile_bench rw
./sendfile_bench sendfile

echo ""
echo "=== Sendfile vs Read/Write Strace Profile ==="
echo "- read/write:"
strace -c -f ./sendfile_bench rw 2>&1 | grep -E "read|write|sendfile" || true
echo "- sendfile:"
strace -c -f ./sendfile_bench sendfile 2>&1 | grep -E "read|write|sendfile" || true

echo "PASS"

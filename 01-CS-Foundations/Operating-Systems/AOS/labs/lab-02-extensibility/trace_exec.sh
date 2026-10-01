#!/usr/bin/env bash

echo "Tracing execve() to demonstrate safe extensibility..."

# Run bpftrace and have it execute a simple command to trace its own exec
bpftrace -e 'tracepoint:syscalls:sys_enter_execve { printf("Process %s (PID %d) is executing %s\n", comm, pid, str(args->filename)); }' -c 'sleep 0.1' || true

exit 0

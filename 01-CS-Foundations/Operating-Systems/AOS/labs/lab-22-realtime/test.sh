#!/bin/bash
set -euo pipefail

echo "=== timers (SCHED_OTHER) ==="
./timers

echo "=== timers (SCHED_FIFO) ==="
./timers fifo

echo "=== timers (SCHED_DEADLINE) ==="
sudo ./timers deadline

echo "PASS"

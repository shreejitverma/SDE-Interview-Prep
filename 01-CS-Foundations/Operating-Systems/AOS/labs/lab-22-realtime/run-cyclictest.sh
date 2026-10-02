#!/bin/bash
set -euo pipefail

echo "=== System Baseline (Idle) ==="
echo "[Idle, SCHED_OTHER (cyclictest)]"
cyclictest --policy=other --threads=1 --interval=1000 --loops=1000 --quiet

echo ""
echo "[Idle, SCHED_FIFO (cyclictest)]"
sudo cyclictest --policy=fifo --priority=90 --threads=1 --interval=1000 --loops=1000 --quiet

echo ""
echo "=== System Under Load (stress-ng) ==="
echo "Starting stress-ng..."
stress-ng --cpu 4 --io 2 --vm 1 --vm-bytes 128M --timeout 10s --quiet &
STRESS_PID=$!
sleep 1

echo "[Load, SCHED_OTHER (cyclictest)]"
cyclictest --policy=other --threads=1 --interval=1000 --loops=1000 --quiet

echo ""
echo "[Load, SCHED_FIFO (cyclictest)]"
sudo cyclictest --policy=fifo --priority=90 --threads=1 --interval=1000 --loops=1000 --quiet

echo ""
echo "=== Custom timers (overshoot in ns) ==="
echo "[Load, timers SCHED_OTHER]"
./timers

echo ""
echo "[Load, timers SCHED_FIFO]"
sudo ./timers fifo

echo ""
echo "[Load, timers SCHED_DEADLINE]"
sudo ./timers deadline

echo ""
echo "Waiting for stress-ng to finish..."
wait $STRESS_PID || true
echo "Done."

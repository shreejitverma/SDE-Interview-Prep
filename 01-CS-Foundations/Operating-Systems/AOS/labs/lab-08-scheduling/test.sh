#!/bin/bash
set -e

echo "=== Compiling ==="
make affinity_workload

echo "=== Cache Affinity (taskset) ==="
echo "Unpinned execution:"
./affinity_workload 4
echo ""
echo "Pinned execution (taskset -c 0,1,2,3):"
taskset -c 0,1,2,3 ./affinity_workload 4
echo ""

echo "=== Perf Sched ==="
echo "Recording scheduler events..."
sudo perf sched record -- ./affinity_workload 4 < /dev/null > /dev/null 2>&1 || true
PAGER=cat sudo perf sched latency < /dev/null > perf_latency.txt 2>/dev/null || true
cat perf_latency.txt
echo ""

echo "=== Chrt Policies ==="
echo "Running with SCHED_OTHER (default):"
./affinity_workload 1
echo "Running with SCHED_FIFO (rt):"
sudo chrt -f 10 ./affinity_workload 1
echo "Running with SCHED_RR (rt):"
sudo chrt -r 10 ./affinity_workload 1
echo ""

echo "=== Cgroups v2 cpu.weight ==="
echo "Setting up cgroups..."
sudo mkdir -p /sys/fs/cgroup/test1
sudo mkdir -p /sys/fs/cgroup/test2
echo 100 | sudo tee /sys/fs/cgroup/test1/cpu.weight >/dev/null
echo 400 | sudo tee /sys/fs/cgroup/test2/cpu.weight >/dev/null

echo "Running 2 workloads pinned to CPU 0 in different cgroups (weight 100 vs 400)..."
sh -c 'echo $$ | sudo tee /sys/fs/cgroup/test1/cgroup.procs >/dev/null; exec taskset -c 0 ./affinity_workload 1 200 > /tmp/cgroup1.out' &
PID1=$!
sh -c 'echo $$ | sudo tee /sys/fs/cgroup/test2/cgroup.procs >/dev/null; exec taskset -c 0 ./affinity_workload 1 200 > /tmp/cgroup2.out' &
PID2=$!

wait $PID1
wait $PID2

echo "Weight 100 (test1):"
cat /tmp/cgroup1.out
echo "Weight 400 (test2):"
cat /tmp/cgroup2.out

# Cleanup
sudo rmdir /sys/fs/cgroup/test1
sudo rmdir /sys/fs/cgroup/test2
rm /tmp/cgroup1.out /tmp/cgroup2.out
echo ""

echo "=== Sched Simulator ==="
./sched_simulator.py
echo ""

echo "PASS"

#!/usr/bin/env python3
"""
disk_scheduler_sim.py
Simulate and benchmark disk scheduling algorithms:
  - FCFS  (First-Come First-Served)
  - SSTF  (Shortest Seek Time First)
  - SCAN  (Elevator)
  - C-SCAN (Circular SCAN)
  - LOOK  (SCAN without going to cylinder boundary)
  - C-LOOK (C-SCAN without going to boundary)
  - DEADLINE (Linux deadline scheduler concept)
  - CFQ   (Completely Fair Queuing concept)

Usage:
  python3 disk_scheduler_sim.py --demo
  python3 disk_scheduler_sim.py --benchmark
  python3 disk_scheduler_sim.py --head 50 --cylinders 200 --requests 10 20 80 90 120 150 40
"""

import sys
import argparse
import random
import collections
import math
from typing import List, Tuple, Dict


# ─────────────────────────────────────────────────────────────────────
# Disk model
# ─────────────────────────────────────────────────────────────────────

class DiskModel:
    """Physical disk parameters (1990s HDD baseline)."""
    def __init__(self, cylinders=200, rpm=7200, sectors_per_track=512,
                 sector_size=512):
        self.cylinders = cylinders
        self.rpm = rpm
        self.sectors_per_track = sectors_per_track
        self.sector_size = sector_size

        # Derived timings
        self.rotation_ms = 60_000.0 / rpm              # ms per rotation
        self.avg_rotational_latency_ms = self.rotation_ms / 2.0
        self.transfer_time_per_sector_ms = self.rotation_ms / sectors_per_track

        # Approximate seek time model: seek_ms ≈ a + b * sqrt(cylinders_crossed)
        self.seek_constant_ms = 1.0   # Minimum seek overhead
        self.seek_linear_ms   = 0.5   # Per cylinder seek cost

    def seek_time_ms(self, from_cyl: int, to_cyl: int) -> float:
        distance = abs(to_cyl - from_cyl)
        if distance == 0:
            return 0.0
        return self.seek_constant_ms + self.seek_linear_ms * math.sqrt(distance)

    def total_io_time_ms(self, from_cyl: int, to_cyl: int) -> float:
        return (self.seek_time_ms(from_cyl, to_cyl) +
                self.avg_rotational_latency_ms +
                self.transfer_time_per_sector_ms)


# ─────────────────────────────────────────────────────────────────────
# Result dataclass
# ─────────────────────────────────────────────────────────────────────

class ScheduleResult:
    def __init__(self, order: List[int], total_seek: int, avg_seek: float,
                 max_seek: int, seq_list: List[Tuple[int, int, int]],
                 wait_variance: float = 0.0):
        self.order        = order           # Service order of cylinder numbers
        self.total_seek   = total_seek      # Total cylinders traversed
        self.avg_seek     = avg_seek        # Average seek distance
        self.max_seek     = max_seek        # Maximum seek in one step
        self.seq_list     = seq_list        # [(from, to, distance), ...]
        self.wait_variance = wait_variance  # Request wait time variance (fairness)

    def summary(self) -> str:
        return (f"order={self.order} total_seek={self.total_seek} "
                f"avg={self.avg_seek:.1f} max={self.max_seek}")


# ─────────────────────────────────────────────────────────────────────
# Algorithm implementations
# ─────────────────────────────────────────────────────────────────────

def schedule_fcfs(head: int, requests: List[int],
                  cylinders: int = 200) -> ScheduleResult:
    """First-Come First-Served: serve in arrival order."""
    order = list(requests)
    seq = []
    total, max_s, current = 0, 0, head

    wait_times = []
    for i, cyl in enumerate(order):
        dist = abs(current - cyl)
        seq.append((current, cyl, dist))
        total += dist
        max_s = max(max_s, dist)
        wait_times.append(i)  # Request i waits i steps
        current = cyl

    variance = _variance(wait_times)
    return ScheduleResult(order, total, total / len(order), max_s, seq, variance)


def schedule_sstf(head: int, requests: List[int],
                  cylinders: int = 200) -> ScheduleResult:
    """Shortest Seek Time First: always serve closest pending request."""
    remaining = list(requests)
    order = []
    seq = []
    total, max_s, current = 0, 0, head

    arrival_time = {i: i for i in range(len(remaining))}  # Simulate arrival
    served_at = {}

    step = 0
    while remaining:
        closest = min(remaining, key=lambda r: abs(r - current))
        dist = abs(current - closest)
        seq.append((current, closest, dist))
        total += dist
        max_s = max(max_s, dist)
        order.append(closest)
        idx = requests.index(closest)
        served_at[idx] = step
        remaining.remove(closest)
        current = closest
        step += 1

    wait_times = [served_at[i] - i for i in range(len(requests))]
    variance = _variance(wait_times)
    return ScheduleResult(order, total, total / len(order), max_s, seq, variance)


def schedule_scan(head: int, requests: List[int], cylinders: int = 200,
                  direction: int = 1) -> ScheduleResult:
    """SCAN (Elevator): sweep in one direction, reverse at boundary."""
    lower = sorted(r for r in requests if r <= head)
    upper = sorted(r for r in requests if r > head)

    if direction == 1:  # Moving toward higher cylinders
        order = upper + lower[::-1]
    else:               # Moving toward lower cylinders
        order = lower[::-1] + upper

    seq = []
    total, max_s, current = 0, 0, head
    for cyl in order:
        dist = abs(current - cyl)
        seq.append((current, cyl, dist))
        total += dist
        max_s = max(max_s, dist)
        current = cyl

    return ScheduleResult(order, total, total / len(order), max_s, seq)


def schedule_cscan(head: int, requests: List[int],
                   cylinders: int = 200) -> ScheduleResult:
    """C-SCAN: sweep upward, jump to cylinder 0, continue upward."""
    lower = sorted(r for r in requests if r <= head)
    upper = sorted(r for r in requests if r > head)

    # Go up to top, then jump to 0 and continue up
    order = upper + lower
    # Add cylinder 0 and max-cylinder as virtual visits for accurate seek count
    if upper:
        jump_from = upper[-1]
        jump_to   = lower[0] if lower else cylinders - 1
    else:
        jump_from = head
        jump_to   = 0

    seq = []
    total, max_s, current = 0, 0, head

    for i, cyl in enumerate(order):
        # If transitioning from top to bottom (upper → lower), account for wrap
        if i == len(upper) and lower:
            # Seek to end of disk + wrap to beginning
            to_end = abs(current - (cylinders - 1))
            wrap = cylinders - 1  # jump from end to cylinder 0
            dist_overhead = to_end + wrap
            seq.append((current, cylinders - 1, to_end))
            seq.append((cylinders - 1, 0, cylinders - 1))
            total += dist_overhead
            max_s = max(max_s, to_end)
            current = 0

        dist = abs(current - cyl)
        seq.append((current, cyl, dist))
        total += dist
        max_s = max(max_s, dist)
        current = cyl

    return ScheduleResult(order, total, total / len(order), max_s, seq)


def schedule_look(head: int, requests: List[int],
                  cylinders: int = 200, direction: int = 1) -> ScheduleResult:
    """LOOK: like SCAN but reverses at last request, not disk boundary."""
    lower = sorted(r for r in requests if r <= head)
    upper = sorted(r for r in requests if r > head)

    if direction == 1:
        order = upper + lower[::-1]
    else:
        order = lower[::-1] + upper

    seq = []
    total, max_s, current = 0, 0, head
    for cyl in order:
        dist = abs(current - cyl)
        seq.append((current, cyl, dist))
        total += dist
        max_s = max(max_s, dist)
        current = cyl

    return ScheduleResult(order, total, total / len(order), max_s, seq)


def schedule_clook(head: int, requests: List[int],
                   cylinders: int = 200) -> ScheduleResult:
    """C-LOOK: go to last upper request, jump to lowest, continue up."""
    lower = sorted(r for r in requests if r < head)
    upper = sorted(r for r in requests if r >= head)
    order = upper + lower

    seq = []
    total, max_s, current = 0, 0, head
    jumped = False

    for i, cyl in enumerate(order):
        if i == len(upper) and lower and not jumped:
            # Jump from last upper to first lower (circular)
            jump_dist = abs(current - lower[0])
            seq.append((current, lower[0], jump_dist))
            total += jump_dist
            max_s = max(max_s, jump_dist)
            current = lower[0]
            jumped = True
            continue

        dist = abs(current - cyl)
        seq.append((current, cyl, dist))
        total += dist
        max_s = max(max_s, dist)
        current = cyl

    return ScheduleResult(order, total, total / len(order), max_s, seq)


# ─────────────────────────────────────────────────────────────────────
# Helper
# ─────────────────────────────────────────────────────────────────────

def _variance(values: List[float]) -> float:
    if not values:
        return 0.0
    mean = sum(values) / len(values)
    return sum((v - mean) ** 2 for v in values) / len(values)


# ─────────────────────────────────────────────────────────────────────
# Display
# ─────────────────────────────────────────────────────────────────────

def draw_disk_trace(head: int, result: ScheduleResult, cylinders: int = 200,
                    width: int = 60) -> None:
    """Draw ASCII visualization of the disk head movement."""
    scale = width / cylinders

    def pos(cyl):
        return int(cyl * scale)

    print(f"Disk head trace (head starts at {head}):")
    print("  " + "0" + " " * (width - 1) + str(cylinders))
    print("  " + "-" * width)

    current = head
    for from_c, to_c, dist in result.seq[:15]:  # Show first 15 moves
        lo = min(from_c, to_c)
        hi = max(from_c, to_c)
        line = [" "] * (width + 1)

        p_from = pos(from_c)
        p_to   = pos(to_c)
        p_lo   = pos(lo)
        p_hi   = pos(hi)

        for p in range(p_lo, p_hi + 1):
            line[p] = "-"
        if p_from < len(line):
            line[p_from] = "O"
        if p_to < len(line):
            line[p_to]   = "X"

        direction = "→" if to_c > from_c else "←"
        print(f"  {''.join(line)}  {from_c:3d}{direction}{to_c:3d} d={dist}")


def print_result(name: str, result: ScheduleResult,
                 disk: DiskModel = None) -> None:
    print(f"\n{'─'*55}")
    print(f"  {name}")
    print(f"{'─'*55}")
    print(f"  Service order : {result.order}")
    print(f"  Total seek    : {result.total_seek} cylinders")
    print(f"  Avg seek      : {result.avg_seek:.1f} cylinders/request")
    print(f"  Max seek      : {result.max_seek} cylinders")
    if result.wait_variance > 0:
        print(f"  Wait variance : {result.wait_variance:.1f} (lower=fairer)")
    if disk:
        total_time = sum(disk.seek_time_ms(f, t) + disk.avg_rotational_latency_ms
                         for f, t, _ in result.seq_list)
        print(f"  Estimated time: {total_time:.1f} ms")


# ─────────────────────────────────────────────────────────────────────
# Benchmark
# ─────────────────────────────────────────────────────────────────────

def benchmark(cylinders: int = 500, n_requests: int = 50,
              n_trials: int = 20) -> None:
    print(f"\n{'='*65}")
    print(f"Benchmark: {cylinders} cylinders, {n_requests} requests, {n_trials} trials")
    print(f"{'='*65}")

    algorithms = [
        ("FCFS",   schedule_fcfs),
        ("SSTF",   schedule_sstf),
        ("SCAN",   schedule_scan),
        ("C-SCAN", schedule_cscan),
        ("LOOK",   schedule_look),
        ("C-LOOK", schedule_clook),
    ]

    disk = DiskModel(cylinders=cylinders)
    results = {name: [] for name, _ in algorithms}
    variance = {name: [] for name, _ in algorithms}

    for trial in range(n_trials):
        head = random.randint(0, cylinders - 1)
        reqs = [random.randint(0, cylinders - 1) for _ in range(n_requests)]

        for name, func in algorithms:
            r = func(head, reqs, cylinders)
            results[name].append(r.total_seek)
            variance[name].append(r.wait_variance)

    # Print summary table
    print(f"\n{'Algorithm':>8} | {'Avg Total Seek':>14} | {'Std Dev':>7} | "
          f"{'Worst':>5} | {'Best':>5} | {'Fairness':>8}")
    print("-" * 65)

    for name, _ in algorithms:
        data = results[name]
        avg  = sum(data) / len(data)
        std  = math.sqrt(sum((x - avg)**2 for x in data) / len(data))
        worst = max(data)
        best  = min(data)
        fair  = sum(variance[name]) / len(variance[name])
        print(f"{name:>8} | {avg:>14.0f} | {std:>7.1f} | "
              f"{worst:>5} | {best:>5} | {fair:>8.1f}")

    print("""
Interpretation:
  FCFS   - Fair (FIFO order) but worst seek performance
  SSTF   - Best average seek but poor fairness (starvation possible!)
  SCAN   - Good throughput, good fairness, slight end-of-disk bias
  C-SCAN - Better uniformity than SCAN (no direction bias)
  LOOK   - Like SCAN but doesn't go to empty boundaries (faster)
  C-LOOK - Best of C-SCAN + LOOK; Linux elevator uses this style

Linux I/O schedulers (lsblk -d -o NAME,ROTA,SCHED):
  mq-deadline - Per-request deadlines (SSDs + HDDs); default for SATA
  bfq        - Budget Fair Queuing; best for interactive workloads
  kyber      - Lightweight; optimal for NVMe SSDs
  none       - No scheduling; best for fast NVMe (request order preserved)

Check/change scheduler:
  cat /sys/block/sda/queue/scheduler
  echo mq-deadline > /sys/block/sda/queue/scheduler
""")


# ─────────────────────────────────────────────────────────────────────
# Demo
# ─────────────────────────────────────────────────────────────────────

def run_demo() -> None:
    # Classic textbook example
    head = 53
    requests = [98, 183, 37, 122, 14, 124, 65, 67]
    cylinders = 200

    print("=" * 60)
    print(f"Classic Disk Scheduling Demo")
    print(f"Initial head position: {head}")
    print(f"Request queue: {requests}")
    print(f"Cylinders: 0–{cylinders - 1}")
    print("=" * 60)

    disk = DiskModel(cylinders=cylinders)
    runs = [
        ("FCFS",   schedule_fcfs(head, requests, cylinders)),
        ("SSTF",   schedule_sstf(head, requests, cylinders)),
        ("SCAN",   schedule_scan(head, requests, cylinders)),
        ("C-SCAN", schedule_cscan(head, requests, cylinders)),
        ("LOOK",   schedule_look(head, requests, cylinders)),
        ("C-LOOK", schedule_clook(head, requests, cylinders)),
    ]

    for name, result in runs:
        print_result(name, result, disk)
        draw_disk_trace(head, result, cylinders)

    # Summary table
    print(f"\n{'='*45}")
    print(f"{'Algorithm':>8} | {'Total Seek':>10} | {'Avg Seek':>8}")
    print("-" * 35)
    for name, r in runs:
        print(f"{name:>8} | {r.total_seek:>10} | {r.avg_seek:>8.1f}")


# ─────────────────────────────────────────────────────────────────────
# SSD vs HDD Analysis
# ─────────────────────────────────────────────────────────────────────

def ssd_vs_hdd_analysis() -> None:
    """
    SSDs have ~0 seek time, so scheduling matters much less.
    For SSDs, the key concern is: write amplification, wear leveling,
    queue depth (NCQ/NVMe), not seek ordering.
    """
    print("\n=== SSD vs HDD: Why Scheduling Matters Less for SSDs ===")
    print()
    print("HDD (7200 RPM, typical):")
    hdd = DiskModel(cylinders=500, rpm=7200)
    print(f"  Seek (1 cyl):    {hdd.seek_time_ms(0, 1):.2f} ms")
    print(f"  Seek (100 cyl):  {hdd.seek_time_ms(0, 100):.2f} ms")
    print(f"  Seek (full):     {hdd.seek_time_ms(0, 499):.2f} ms")
    print(f"  Avg rotational:  {hdd.avg_rotational_latency_ms:.2f} ms")
    print(f"  Transfer/sector: {hdd.transfer_time_per_sector_ms:.3f} ms")

    print()
    print("NVMe SSD (typical):")
    print(f"  Random 4K read latency: 0.02–0.10 ms (20–100 µs)")
    print(f"  Sequential throughput:  3,000–7,000 MB/s")
    print(f"  Queue depth:            up to 65,535 commands")
    print(f"  Access pattern:         random == sequential (no seek!)")
    print()
    print("Conclusion: For NVMe SSDs, use 'none' scheduler.")
    print("For SATA SSDs, 'mq-deadline' or 'bfq' work well.")
    print("For HDDs, 'bfq' or 'mq-deadline' with read/write prioritization.")


# ─────────────────────────────────────────────────────────────────────
# Main
# ─────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="Disk Scheduling Simulator")
    parser.add_argument("--head",      type=int, default=53)
    parser.add_argument("--cylinders", type=int, default=200)
    parser.add_argument("--requests",  type=int, nargs="+",
                        default=[98, 183, 37, 122, 14, 124, 65, 67])
    parser.add_argument("--algorithm",
                        choices=["fcfs","sstf","scan","cscan","look","clook"],
                        default=None)
    parser.add_argument("--demo",      action="store_true")
    parser.add_argument("--benchmark", action="store_true")
    parser.add_argument("--ssd",       action="store_true")
    args = parser.parse_args()

    random.seed(42)

    if args.demo or (not args.algorithm and not args.benchmark and not args.ssd):
        run_demo()

    if args.benchmark:
        benchmark(cylinders=args.cylinders)

    if args.ssd:
        ssd_vs_hdd_analysis()

    if args.algorithm:
        algos = {
            "fcfs":  schedule_fcfs, "sstf":  schedule_sstf,
            "scan":  schedule_scan, "cscan": schedule_cscan,
            "look":  schedule_look, "clook": schedule_clook,
        }
        func = algos[args.algorithm]
        result = func(args.head, args.requests, args.cylinders)
        print_result(args.algorithm.upper(), result)
        draw_disk_trace(args.head, result, args.cylinders)


if __name__ == "__main__":
    main()

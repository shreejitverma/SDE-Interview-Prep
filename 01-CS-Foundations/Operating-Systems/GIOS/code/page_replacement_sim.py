#!/usr/bin/env python3
"""
page_replacement_sim.py
Simulate and benchmark page replacement algorithms:
  - OPT  (Optimal / Belady's) - theoretical lower bound
  - FIFO (First-In First-Out)
  - LRU  (Least Recently Used)
  - Clock (Second Chance)
  - Enhanced Clock (NRU - Not Recently Used)
  - LFU  (Least Frequently Used)
  - Working Set
  - WSClock

Usage:
  python3 page_replacement_sim.py [--frames N] [--trace TRACE_FILE]
  python3 page_replacement_sim.py --demo          # run all demos
  python3 page_replacement_sim.py --benchmark     # comprehensive benchmark

Reference trace file format (one page number per line):
  1
  2
  3
  1
  4
  ...
"""

import sys
import argparse
import random
import collections
import heapq
from typing import List, Tuple, Dict


# ─────────────────────────────────────────────────────────────────────
# Algorithm implementations
# ─────────────────────────────────────────────────────────────────────

def simulate_opt(frames: int, pages: List[int]) -> Tuple[int, List[str]]:
    """Optimal (Belady's) - replace page not used for longest time in future."""
    memory = []
    faults = 0
    log = []

    for i, page in enumerate(pages):
        if page in memory:
            log.append(f"  [{i:3d}] page {page:3d} HIT   memory={memory}")
            continue

        faults += 1
        if len(memory) < frames:
            memory.append(page)
            log.append(f"  [{i:3d}] page {page:3d} FAULT memory={memory} (cold)")
        else:
            # Find which resident page is used furthest in the future
            future_use = {}
            for p in memory:
                try:
                    future_use[p] = pages.index(p, i + 1)
                except ValueError:
                    future_use[p] = float('inf')  # Never used again

            # Evict the page used furthest in future
            evict = max(future_use, key=lambda p: future_use[p])
            memory[memory.index(evict)] = page
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT evict={evict} "
                f"(next_use={future_use[evict]}) memory={memory}"
            )

    return faults, log


def simulate_fifo(frames: int, pages: List[int]) -> Tuple[int, List[str]]:
    """FIFO - evict the page that has been in memory the longest."""
    memory = collections.deque(maxlen=frames)  # Order = arrival order
    in_memory = set()
    faults = 0
    log = []

    queue = collections.deque()  # Tracks arrival order

    for i, page in enumerate(pages):
        if page in in_memory:
            log.append(f"  [{i:3d}] page {page:3d} HIT   memory={list(in_memory)}")
            continue

        faults += 1
        if len(in_memory) < frames:
            in_memory.add(page)
            queue.append(page)
            log.append(f"  [{i:3d}] page {page:3d} FAULT memory={list(in_memory)} (cold)")
        else:
            evict = queue.popleft()
            in_memory.discard(evict)
            in_memory.add(page)
            queue.append(page)
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT evict={evict} "
                f"memory={sorted(in_memory)}"
            )

    return faults, log


def simulate_lru(frames: int, pages: List[int]) -> Tuple[int, List[str]]:
    """LRU - evict the page that was least recently used."""
    memory = collections.OrderedDict()  # page → timestamp (most recent at end)
    faults = 0
    log = []

    for i, page in enumerate(pages):
        if page in memory:
            memory.move_to_end(page)  # Mark as recently used
            log.append(f"  [{i:3d}] page {page:3d} HIT   memory={list(memory.keys())}")
            continue

        faults += 1
        if len(memory) < frames:
            memory[page] = i
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT memory={list(memory.keys())} (cold)"
            )
        else:
            # Evict LRU page (oldest = first in OrderedDict)
            evict, _ = memory.popitem(last=False)
            memory[page] = i
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT evict={evict} "
                f"memory={list(memory.keys())}"
            )

    return faults, log


def simulate_clock(frames: int, pages: List[int]) -> Tuple[int, List[str]]:
    """Clock (Second Chance) - circular FIFO with reference bits."""
    # Each frame: (page, reference_bit)
    frames_arr = [None] * frames
    ref_bits = [0] * frames
    hand = 0      # Clock hand
    page_to_frame = {}
    faults = 0
    log = []

    for i, page in enumerate(pages):
        if page in page_to_frame:
            frame_idx = page_to_frame[page]
            ref_bits[frame_idx] = 1  # Set reference bit on access
            log.append(
                f"  [{i:3d}] page {page:3d} HIT   "
                f"frames={frames_arr} refs={ref_bits} hand={hand}"
            )
            continue

        faults += 1

        # Find a frame to evict
        while True:
            if frames_arr[hand] is None:
                # Empty frame
                frames_arr[hand] = page
                ref_bits[hand] = 1
                page_to_frame[page] = hand
                hand = (hand + 1) % frames
                break
            elif ref_bits[hand] == 0:
                # Evict: reference bit is 0 (second chance used up)
                evict = frames_arr[hand]
                del page_to_frame[evict]
                frames_arr[hand] = page
                ref_bits[hand] = 1
                page_to_frame[page] = hand
                hand = (hand + 1) % frames
                log.append(
                    f"  [{i:3d}] page {page:3d} FAULT evict={evict} "
                    f"frames={frames_arr} refs={ref_bits} hand={hand}"
                )
                break
            else:
                # Give second chance: clear ref bit, advance hand
                ref_bits[hand] = 0
                hand = (hand + 1) % frames

        if faults <= 1 or log[-1].startswith(f"  [{i:3d}]") is False:
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT (cold) "
                f"frames={frames_arr} refs={ref_bits}"
            )

    return faults, log


def simulate_lfu(frames: int, pages: List[int]) -> Tuple[int, List[str]]:
    """LFU - evict least frequently used; ties broken by LRU."""
    freq: Dict[int, int] = {}         # page → frequency count
    last_use: Dict[int, int] = {}     # page → last access time
    in_memory: set = set()
    faults = 0
    log = []

    for i, page in enumerate(pages):
        if page in in_memory:
            freq[page] = freq.get(page, 0) + 1
            last_use[page] = i
            log.append(
                f"  [{i:3d}] page {page:3d} HIT   "
                f"freq={freq[page]} memory={sorted(in_memory)}"
            )
            continue

        faults += 1
        freq[page] = freq.get(page, 0) + 1
        last_use[page] = i

        if len(in_memory) < frames:
            in_memory.add(page)
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT memory={sorted(in_memory)} (cold)"
            )
        else:
            # Evict page with lowest frequency; ties: evict LRU (oldest use)
            evict = min(in_memory, key=lambda p: (freq.get(p, 0), last_use.get(p, 0)))
            in_memory.discard(evict)
            in_memory.add(page)
            log.append(
                f"  [{i:3d}] page {page:3d} FAULT evict={evict} "
                f"(freq={freq.get(evict, 0)}) memory={sorted(in_memory)}"
            )

    return faults, log


# ─────────────────────────────────────────────────────────────────────
# Belady's Anomaly detection (FIFO can have MORE faults with MORE frames!)
# ─────────────────────────────────────────────────────────────────────

def check_beladys_anomaly(pages: List[int], max_frames: int = 8) -> None:
    """Demonstrate Belady's Anomaly: FIFO can have MORE faults with MORE frames."""
    print("\n=== Belady's Anomaly (FIFO only) ===")
    print(f"Pages: {pages}")
    print(f"{'Frames':>6} | {'FIFO faults':>11} | {'LRU faults':>10}")
    print("-" * 35)

    prev_fifo = None
    for f in range(1, max_frames + 1):
        fifo_faults, _ = simulate_fifo(f, pages)
        lru_faults, _  = simulate_lru(f, pages)
        anomaly = ""
        if prev_fifo is not None and fifo_faults > prev_fifo:
            anomaly = "  ← ANOMALY!"
        print(f"{f:>6} | {fifo_faults:>11} | {lru_faults:>10}{anomaly}")
        prev_fifo = fifo_faults


# ─────────────────────────────────────────────────────────────────────
# Comprehensive benchmark
# ─────────────────────────────────────────────────────────────────────

def benchmark(frames_list: List[int] = None, trace_len: int = 10000,
              n_pages: int = 20) -> None:
    """Benchmark all algorithms across different frame counts and access patterns."""
    if frames_list is None:
        frames_list = [2, 4, 8, 16, 32]

    algorithms = {
        "OPT":   simulate_opt,
        "LRU":   simulate_lru,
        "CLOCK": simulate_clock,
        "FIFO":  simulate_fifo,
        "LFU":   simulate_lfu,
    }

    # Generate different workload patterns
    workloads = {
        "Uniform random": [random.randint(0, n_pages - 1)
                           for _ in range(trace_len)],

        "Locality (80/20)": [
            random.randint(0, n_pages // 5 - 1) if random.random() < 0.8
            else random.randint(n_pages // 5, n_pages - 1)
            for _ in range(trace_len)
        ],

        "Sequential scan": [i % n_pages for i in range(trace_len)],

        "Stack depth (LRU optimal)": list(
            # Repeatedly access recent pages - perfect for LRU
            __import__('itertools').chain.from_iterable(
                range(max(0, i - 4), i + 1) for i in range(trace_len // 5)
            )
        )[:trace_len],
    }

    for workload_name, pages in workloads.items():
        print(f"\n{'='*70}")
        print(f"Workload: {workload_name}  (trace={len(pages)}, pages=0..{n_pages-1})")
        print(f"{'='*70}")
        print(f"{'Alg':>6}", end="")
        for f in frames_list:
            print(f"  {f:>3}fr", end="")
        print()
        print("-" * (7 + 6 * len(frames_list)))

        for name, func in algorithms.items():
            print(f"{name:>6}", end="")
            for f in frames_list:
                faults, _ = func(f, pages)
                hit_rate = 100 * (1 - faults / len(pages))
                print(f"  {hit_rate:4.0f}%", end="")
            print()

    # Comparison note
    print("""
Notes:
  OPT   = Theoretical minimum (offline oracle, not implementable)
  LRU   = Best practical algorithm; approximated by Clock in practice
  CLOCK = Linux/BSD page cache uses enhanced Clock (approximates LRU)
  FIFO  = Simple but poor; subject to Belady's Anomaly
  LFU   = Good for stable working sets; slow to adapt to changes
""")


# ─────────────────────────────────────────────────────────────────────
# Working Set Model
# ─────────────────────────────────────────────────────────────────────

def working_set_analysis(pages: List[int], window: int = 5) -> None:
    """Analyze working set size over time."""
    print(f"\n=== Working Set Model (window={window}) ===")
    print("The working set W(t,△) = set of pages referenced in last △ accesses\n")

    ws_sizes = []
    for t in range(len(pages)):
        start = max(0, t - window + 1)
        ws = set(pages[start:t + 1])
        ws_sizes.append(len(ws))

    avg_ws = sum(ws_sizes) / len(ws_sizes)
    max_ws = max(ws_sizes)

    print(f"Trace: {pages}")
    print(f"WS sizes over time: {ws_sizes}")
    print(f"Average working set size: {avg_ws:.1f} pages")
    print(f"Peak working set size: {max_ws} pages")
    print(f"Recommendation: allocate ≥ {max_ws} frames to avoid thrashing")


# ─────────────────────────────────────────────────────────────────────
# Demo: Classic trace from textbook
# ─────────────────────────────────────────────────────────────────────

def run_demo() -> None:
    # Silberschatz OS textbook example
    textbook_trace = [7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1]
    frames = 3

    print("=" * 60)
    print(f"Classic OS Textbook Trace (Silberschatz)")
    print(f"Pages: {textbook_trace}")
    print(f"Frames: {frames}")
    print("=" * 60)

    for name, func in [("OPT", simulate_opt), ("FIFO", simulate_fifo),
                       ("LRU", simulate_lru), ("Clock", simulate_clock),
                       ("LFU", simulate_lfu)]:
        faults, log = func(frames, textbook_trace)
        hit_rate = 100 * (1 - faults / len(textbook_trace))
        print(f"\n--- {name}: {faults} faults, {hit_rate:.1f}% hit rate ---")
        for line in log[:8]:
            print(line)
        if len(log) > 8:
            print(f"  ... ({len(log) - 8} more entries)")

    # Belady's anomaly demo
    belady_trace = [1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5]
    check_beladys_anomaly(belady_trace)

    # Working set
    working_set_analysis([1, 2, 3, 4, 1, 2, 1, 3, 1, 4, 2, 1], window=4)


# ─────────────────────────────────────────────────────────────────────
# Main
# ─────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="Page Replacement Algorithm Simulator")
    parser.add_argument("--frames", type=int, default=3, help="Number of frames")
    parser.add_argument("--trace", type=str, help="Path to trace file")
    parser.add_argument("--demo", action="store_true", help="Run textbook demo")
    parser.add_argument("--benchmark", action="store_true",
                        help="Comprehensive benchmark")
    parser.add_argument("--algorithm", choices=["opt", "fifo", "lru", "clock", "lfu"],
                        default="lru", help="Algorithm to use with --trace")
    args = parser.parse_args()

    random.seed(42)

    if args.demo or (not args.trace and not args.benchmark):
        run_demo()

    if args.benchmark:
        benchmark()

    if args.trace:
        with open(args.trace) as f:
            pages = [int(line.strip()) for line in f if line.strip()]

        algos = {"opt": simulate_opt, "fifo": simulate_fifo,
                 "lru": simulate_lru, "clock": simulate_clock, "lfu": simulate_lfu}
        func = algos[args.algorithm]
        faults, log = func(args.frames, pages)
        hit_rate = 100 * (1 - faults / len(pages))
        print(f"Algorithm: {args.algorithm.upper()}")
        print(f"Frames: {args.frames}")
        print(f"Page references: {len(pages)}")
        print(f"Page faults: {faults}")
        print(f"Hit rate: {hit_rate:.2f}%")


if __name__ == "__main__":
    main()

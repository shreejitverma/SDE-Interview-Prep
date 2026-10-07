---
id: design-task-scheduler
title: "Low-Level Design: Distributed and In-Memory Task Scheduler"
tags:
  - lld
  - interview-problem
  - task-scheduler
  - concurrency
  - cron
level: advanced
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources: []
---

# Low-Level Design: Distributed and In-Memory Task Scheduler

## 1. Problem Statement and Requirements

Design an in-memory and distributed **Task Scheduler** (comparable to Java's Quartz, ScheduledThreadPoolExecutor, or Linux cron) capable of scheduling millions of one-time and recurring delayed tasks with microsecond timing accuracy.

### 1.1 Functional Requirements
1. **One-Time Delayed Tasks**: `schedule(task, delay_ms)` executes once after the specified delay.
2. **Recurring Tasks**:
   - `schedule_at_fixed_rate(task, initial_delay, period_ms)`: Schedules executions at uniform time intervals, irrespective of task execution duration.
   - `schedule_with_fixed_delay(task, initial_delay, delay_ms)`: Schedules subsequent executions with a fixed delay *after* the previous execution finishes.
3. **Task Cancellation**: `cancel(task_id)` aborts unexecuted tasks immediately.
4. **Misfire Policies**: Deterministic handling when a task is delayed past its scheduled execution window due to thread pool saturation (e.g., Run Once Immediately vs Discard Missed Runs).

### 1.2 Non-Functional & Concurrency Requirements
1. **Low Scheduling Latency**: Polling or waking up for due tasks must not waste CPU cycles in busy-wait loops.
2. **Thread Safety**: Dynamic submissions, cancellations, and concurrent worker executions must be strictly synchronized.

```mermaid
flowchart TD
    Client["Client App"] -->|schedule(task, run_at)| Scheduler["TaskScheduler Engine"]
    Scheduler --> TaskQueue["Min-Heap Priority Queue (Sorted by run_at)"]
    Scheduler --> LeaderThread["Scheduler Coordinator Thread"]
    LeaderThread -->|Condition.wait(due_time - now)| SleepState["Sleep until Earliest Due Task"]
    LeaderThread -->|Due Tasks Popped| WorkerPool["Worker ThreadPool Executor"]
    WorkerPool --> ExecTask["Execute Task Callback"]
    ExecTask --> RecurringCheck{"Is Recurring?"}
    RecurringCheck -- Yes --> Reschedule["Compute next_run_at and re-insert into Heap"]
    RecurringCheck -- No --> Terminate["Task Complete"]
```

---

## 2. Core Scheduling Architectures: Min-Heap vs Hashed Wheel Timer

### 2.1 Min-Heap Priority Queue ($O(\log N)$)
Tasks are stored in a binary min-heap sorted by their `next_run_at` timestamp:
- **Peek Earliest**: $O(1)$ inspects the top element.
- **Insert / Reschedule**: $O(\log N)$ to insert a new task.
- **Cancellation**: $O(\log N)$ with lazy marking.
Ideal when task counts are moderate ($10^3$ to $10^5$) and delays vary widely across seconds, hours, and days.

### 2.2 Hashed Wheel Timer (Netty TimerWheel) ($O(1)$)
Invented by George Varghese and Anthony Lauck:
- A circular buffer of $N$ buckets (ticks), each representing a discrete time slot (e.g., 100ms per tick).
- A pointer moves forward by one tick every interval.
- Tasks are hashed into `tick = (run_at / tick_duration) % wheel_size`.
- Adding or removing a task is $O(1)$.
Ideal for high-throughput networking systems (such as Netty or gRPC) managing millions of short-lived I/O connection timeouts.

---

## 3. Fixed Rate vs Fixed Delay Semantics

```
Timeline: Task takes 3 seconds to run. Period = 5 seconds.

Fixed Rate (Starts every 5s):
|-- Run 3s --|-- 2s Idle --|-- Run 3s --|-- 2s Idle --|
0s                       5s                       10s

Fixed Delay (5s gap between end of previous and start of next):
|-- Run 3s --|----- 5s Delay -----|-- Run 3s --|
0s           3s                   8s           11s
```

---

## 4. Complete Production-Grade Simulation in Python

The following script implements a concurrent **Task Scheduler** using a thread-safe **Min-Heap Priority Queue**, condition variable sleep synchronization, and worker thread pool execution.

```python
"""
In-Memory Task Scheduler Production Simulation.
Demonstrates:
1. Min-Heap Priority Queue sorted by scheduled execution timestamp.
2. Condition variable wait-timeout synchronization eliminating busy-waiting.
3. Fixed-rate and one-time execution semantics with cancel tokens.
4. Worker thread pool execution preventing long tasks from blocking scheduler.
"""

from abc import ABC, abstractmethod
import heapq
import threading
import time
from typing import Callable, Dict, List, Optional
import uuid


class ScheduledTask:
    """Task metadata envelope stored in min-heap."""
    def __init__(self, task_id: str, action: Callable[[], None], run_at: float, period_sec: Optional[float] = None):
        self.task_id = task_id
        self.action = action
        self.run_at = run_at
        self.period_sec = period_sec
        self.is_cancelled = False

    def __lt__(self, other: 'ScheduledTask') -> bool:
        # Min-heap sorts by lowest run_at timestamp
        return self.run_at < other.run_at


class TaskScheduler:
    """Thread-safe task scheduler engine."""
    def __init__(self, worker_threads: int = 4):
        self._heap: List[ScheduledTask] = []
        self._tasks: Dict[str, ScheduledTask] = {}
        self._lock = threading.Lock()
        self._condition = threading.Condition(self._lock)
        self._running = True

        # Coordinator thread monitoring due tasks
        self._coordinator = threading.Thread(target=self._coordinator_loop, name="SchedulerCoordinator", daemon=True)
        self._coordinator.start()

        # ThreadPool for task execution
        self._work_queue: List[Callable] = []
        self._worker_pool = [
            threading.Thread(target=self._worker_loop, name=f"SchedulerWorker-{i}", daemon=True)
            for i in range(worker_threads)
        ]
        self._worker_queue: 'queue.Queue' = __import__('queue').Queue()
        for w in self._worker_pool:
            w.start()

    def schedule(self, action: Callable[[], None], delay_sec: float) -> str:
        return self._schedule_internal(action, delay_sec, period_sec=None)

    def schedule_at_fixed_rate(self, action: Callable[[], None], initial_delay_sec: float, period_sec: float) -> str:
        return self._schedule_internal(action, initial_delay_sec, period_sec=period_sec)

    def _schedule_internal(self, action: Callable[[], None], delay_sec: float, period_sec: Optional[float]) -> str:
        task_id = str(uuid.uuid4())
        run_at = time.time() + delay_sec
        task = ScheduledTask(task_id, action, run_at, period_sec)

        with self._condition:
            self._tasks[task_id] = task
            heapq.heappush(self._heap, task)
            # Wake coordinator in case new task is due earlier than current head
            self._condition.notify_all()

        return task_id

    def cancel(self, task_id: str) -> bool:
        with self._condition:
            task = self._tasks.get(task_id)
            if task and not task.is_cancelled:
                task.is_cancelled = True
                del self._tasks[task_id]
                return True
            return False

    def _coordinator_loop(self) -> None:
        while self._running:
            with self._condition:
                now = time.time()
                while self._heap and self._heap[0].is_cancelled:
                    heapq.heappop(self._heap)

                if not self._heap:
                    self._condition.wait(timeout=1.0)
                    continue

                earliest_task = self._heap[0]
                if earliest_task.run_at <= now:
                    # Pop task and dispatch to worker pool
                    task = heapq.heappop(self._heap)
                    if not task.is_cancelled:
                        self._worker_queue.put(task)

                        # Handle recurring reschedule
                        if task.period_sec is not None:
                            task.run_at = now + task.period_sec
                            heapq.heappush(self._heap, task)
                else:
                    # Sleep until earliest task is due
                    wait_time = earliest_task.run_at - now
                    self._condition.wait(timeout=max(0.001, wait_time))

    def _worker_loop(self) -> None:
        while self._running:
            try:
                task: ScheduledTask = self._worker_queue.get(timeout=0.1)
            except Exception:
                continue

            if not task.is_cancelled:
                try:
                    task.action()
                except Exception as ex:
                    print(f"Task {task.task_id} execution failed: {ex}")

            self._worker_queue.task_done()

    def shutdown(self) -> None:
        self._running = False
        with self._condition:
            self._condition.notify_all()
        self._coordinator.join()


# =====================================================================
# VERIFICATION SUITE
# =====================================================================

def main():
    print("Executing Task Scheduler Verification Suite...")

    scheduler = TaskScheduler(worker_threads=2)
    execution_record: List[str] = []
    lock = threading.Lock()

    def record_action(tag: str):
        with lock:
            execution_record.append(tag)

    # 1. Schedule one-time task
    scheduler.schedule(lambda: record_action("ONCE_100MS"), delay_sec=0.1)

    # 2. Schedule recurring fixed-rate task
    recurring_id = scheduler.schedule_at_fixed_rate(
        lambda: record_action("RECURRING_50MS"), initial_delay_sec=0.05, period_sec=0.05
    )

    # 3. Schedule task to be cancelled
    cancelled_id = scheduler.schedule(lambda: record_action("SHOULD_NOT_RUN"), delay_sec=0.15)
    cancelled = scheduler.cancel(cancelled_id)
    assert cancelled is True

    # Sleep to allow tasks to trigger
    time.sleep(0.22)

    # Cancel recurring task
    scheduler.cancel(recurring_id)

    scheduler.shutdown()

    # Verifications
    with lock:
        assert "ONCE_100MS" in execution_record
        assert "SHOULD_NOT_RUN" not in execution_record
        recurring_count = execution_record.count("RECURRING_50MS")
        assert recurring_count >= 3  # Should have fired ~3-4 times in 200ms
        print(f"Task Scheduler Executions Verified: Recurring fired {recurring_count} times, One-time executed, Cancelled preserved.")

    print("All Task Scheduler validations completed successfully.")


if __name__ == "__main__":
    main()
```

---

## 5. Active Recall Interview Questions

<details>
<summary>1. What is the difference between `scheduleAtFixedRate` and `scheduleWithFixedDelay`?</summary>
`scheduleAtFixedRate` schedules executions at fixed intervals relative to the initial start time (e.g., $T, T+P, T+2P$), regardless of how long each execution takes.
If a task runs longer than $P$, subsequent executions may run back-to-back.
`scheduleWithFixedDelay` enforces that the delay period $P$ begins only *after* the previous execution finishes, ensuring a minimum rest gap between runs.
</details>

<details>
<summary>2. How does a condition variable prevent busy-waiting in a Min-Heap task scheduler?</summary>
Instead of polling the heap in a tight loop (`while True: check()`), the coordinator thread peeks at the earliest task's `run_at` timestamp.
If `run_at > now`, it invokes `condition.wait(timeout = run_at - now)`.
The operating system puts the thread to sleep until either the timeout expires or another thread schedules an earlier task and calls `condition.notify()`.
</details>

<details>
<summary>3. What is a Hashed Wheel Timer, and why is it O(1) compared to a Min-Heap's O(log N)?</summary>
A Hashed Wheel Timer represents time as a circular array of buckets (ticks).
Tasks are hashed into a bucket based on `(run_at / tick_duration) % wheel_size`.
Adding a task is an $O(1)$ array insertion, and advancing ticks is $O(1)$, avoiding the $O(\log N)$ tree balancing required by binary min-heaps.
</details>

<details>
<summary>4. What is a task misfire, and what are common misfire handling policies?</summary>
A misfire occurs when a task is scheduled for time $T$, but due to thread pool saturation, machine pause, or server restart, it does not begin execution until $T + \Delta$.
Common misfire policies include:
1. **Fire Once Immediately**: Executes one catch-up run now and aligns subsequent runs with the schedule.
2. **Discard All Missed**: Skips all missed executions and waits for the next future scheduled time.
3. **Execute All Missed Runs**: Sequentially runs every missed invocation (risks cascading overload).
</details>

<details>
<summary>5. Why should a Task Scheduler coordinator thread delegate execution to a worker thread pool?</summary>
If the coordinator thread executed the task callback inline, a slow or blocking task would prevent the coordinator from evaluating subsequent due tasks in the heap, causing all other scheduled tasks to misfire.
Delegating execution to a worker pool keeps the scheduler coordinator free to evaluate timing invariants.
</details>

<details>
<summary>6. How is task cancellation implemented in a Min-Heap without expensive O(N) heap deletions?</summary>
Through Lazy Cancellation (tombstoning).
`cancel(task_id)` sets an `is_cancelled = true` boolean flag on the task object in $O(1)$.
When the task reaches the top of the min-heap, the coordinator pops and discards it without executing, amortizing deletion cost to $O(\log N)$.
</details>

<details>
<summary>7. How does a distributed task scheduler (e.g., Quartz Cluster or Temporal) prevent duplicate task execution across multiple servers?</summary>
Using distributed database locks or leader election.
Nodes attempt to acquire an exclusive row lock (`SELECT ... FOR UPDATE` in Quartz) or a distributed lock lease in ZooKeeper/Redis on the scheduled task record.
Only the node that successfully acquires the lease executes the task and advances `next_fire_time`.
</details>

<details>
<summary>8. In Java's `ScheduledThreadPoolExecutor`, what happens if a task throws an unhandled RuntimeException?</summary>
If a recurring task throws an unhandled exception, `ScheduledThreadPoolExecutor` silently suppresses subsequent executions of that task without logging or warning, permanently terminating the recurrence.
Production code must always wrap task bodies in a top-level `try-catch(Throwable)` block.
</details>

<details>
<summary>9. What is clock drift, and how does it affect task schedulers relying on monotonic time vs wall-clock time?</summary>
Wall-clock time (`gettimeofday()`, NTP updates, leap seconds) can jump forward or backward, potentially triggering premature executions or massive delays.
Schedulers must use **Monotonic Clocks** (`CLOCK_MONOTONIC`, `System.nanoTime()`) for interval and delay calculations, as monotonic time is guaranteed never to jump backwards.
</details>

<details>
<summary>10. Under what condition is a cron parser required versus a simple fixed-rate delay?</summary>
When task executions are bound to calendar boundaries (e.g., "every Monday at 9:00 AM" or "the last business day of the month"), which vary with daylight saving time, month lengths, and leap years.
Cron parsers evaluate schedule strings against calendar time rather than uniform fixed millisecond intervals.
</details>

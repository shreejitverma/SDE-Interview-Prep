---
title: LeetCode - 0621 - Task Scheduler
tags:
  - leetcode
  - problem
  - greedy
  - math
  - hash-table
difficulty: medium
source: leetcode
problem_number: "0621"
topics:
  - Greedy
  - Math
  - Hash Table
  - Counting
---

# LeetCode 0621: Task Scheduler

## Problem Breakdown

You are given an array of CPU `tasks`, each represented by a character from `'A'` to `'Z'`.
Each task takes one unit of CPU time to execute.
The CPU must wait for at least `n` units of cooldown time between two executions of the identical task.
During any unit of time, the CPU can either run an available task or remain idle.
Return the minimum number of units of time required to complete all tasks.

### Mathematical Structure of Cooldown Slots

- The tasks with the maximum frequency (`max_freq`) dictate the overall scheduling framework.
- If task `A` occurs `max_freq` times, there must be at least `max_freq - 1` gaps separating these executions.
- Each gap must have length at least `n` (giving frame chunk size `n + 1`).
- Therefore, there are `(max_freq - 1)` chunks, each requiring `(n + 1)` slots.
- The final execution of the highest-frequency tasks occurs in the concluding chunk without trailing idle time.
- If there are `max_count` distinct tasks that share the maximum frequency `max_freq`, the final chunk requires `max_count` slots.
- Thus, the minimum theoretical frame size is:

$$\text{slots} = (\text{max\_freq} - 1) \times (n + 1) + \text{max\_count}$$

- If there are sufficiently many other tasks to fill all idle slots completely without exceeding the frame, the required time is simply the total number of tasks `tasks.length`.
- The final result is:

$$\text{answer} = \max(\text{tasks.length}, (\text{max\_freq} - 1) \times (n + 1) + \text{max\_count})$$

## Optimal Approaches

### Greedy Frequency Formula

```
Tasks: [A, A, A, B, B, B], n = 2
Frequencies: A: 3, B: 3
max_freq = 3, max_count = 2 (both A and B)

Frame Construction:
Chunk 1: [A, B, idle] -> length 3 (n + 1)
Chunk 2: [A, B, idle] -> length 3 (n + 1)
Chunk 3: [A, B]       -> length 2 (max_count)

Total slots = (3 - 1) * (2 + 1) + 2 = 2 * 3 + 2 = 8
```

1. Count the occurrences of each task in an integer frequency table of size 26.
2. Find `max_freq`, the highest frequency value among all characters.
3. Count how many characters have frequency equal to `max_freq` (`max_count`).
4. Calculate the required frame length: `(max_freq - 1) * (n + 1) + max_count`.
5. Return the maximum of `tasks.length` and the calculated frame length.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n)$ | One linear pass over the `tasks` array, followed by a fixed scan of 26 letters. |
| **Space Complexity** | $O(1)$ | Fixed 26-element array for uppercase English alphabet task frequencies. |

## Common Traps & Edge Cases

- **Zero Cooldown ($n = 0$)**: When $n = 0$, no idle time is required; the answer is always `tasks.length`.
- **Multiple Tasks with Maximum Frequency**: Forgetting to count how many tasks share `max_freq` results in underestimating the length of the final chunk.
- **Empty Idle Slots Not Needed**: When there are more than enough distinct low-frequency tasks, no CPU idle intervals occur. Taking $\max(\text{tasks.length}, \dots)$ prevents returning an underestimate.

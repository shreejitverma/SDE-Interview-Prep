---
title: LeetCode - 0045 - Jump Game II
tags:
  - leetcode
  - problem
  - greedy
  - array
  - dynamic-programming
difficulty: medium
source: leetcode
problem_number: "0045"
topics:
  - Greedy
  - Array
  - Dynamic Programming
---

# LeetCode 0045: Jump Game II

## Problem Breakdown

You are given a 0-indexed array of integers `nums` of length `n`.
You are initially positioned at `nums[0]`.
Each element `nums[i]` represents the maximum length of a forward jump from index `i`.
Return the minimum number of jumps to reach index `n - 1`.
The test cases are generated such that you can always reach `n - 1`.

### Key Observations

- At any jump step, you have a current range of reachable indices defined by `[current_start, current_end]`.
- From this window, the next jump can reach any index up to `farthest = max(i + nums[i])` for all `i` within the current window.
- This creates an implicit breadth-first search (BFS) level structure over the array indices.
- Instead of explicitly generating queue allocations for BFS levels, we can track the frontier using two boundary pointers.
- When the iterator `i` reaches `current_end`, we must consume one jump, advancing `current_end` to `farthest`.

## Optimal Approaches

### Greedy Implicit BFS

```
Index:    0   1   2   3   4
Nums:    [2,  3,  1,  1,  4]
          ^
Step 0: current_end = 0, farthest = 2
        At i = 0 (current_end): jump 1 -> current_end = 2, range [1, 2]

Step 1: i = 1 -> farthest = max(2, 1 + 3) = 4
        i = 2 (current_end): jump 2 -> current_end = 4 >= n - 1 (target reached)
Total Jumps = 2
```

1. Initialize `jumps = 0`, `current_end = 0`, and `farthest = 0`.
2. Iterate `i` from `0` to `n - 2`. We stop at `n - 2` because reaching `n - 1` does not require another jump if we already arrived.
3. Update `farthest = max(farthest, i + nums[i])`.
4. If `i == current_end`, increment `jumps` and update `current_end = farthest`.
5. If `current_end >= n - 1` at any point, early exit since the destination is reachable.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n)$ | Single forward linear scan over the array up to $n - 1$. |
| **Space Complexity** | $O(1)$ | Constant extra space with three integer registers. |

## Common Traps & Edge Cases

- **Length of Array is 1**: When `nums.length == 1`, we are already at the destination, so the answer is strictly `0`.
- **Iterating past $n - 2$**: If the loop runs to $n - 1$, and `n - 1 == current_end`, an unnecessary extra jump would be counted.
- **Negative or Zero Values**: The problem states you can always reach the target, but zeros can stall progress if not handled greedily.

---
title: LeetCode - 0704 - Binary Search
tags:
  - leetcode
  - problem
  - binary-search
  - array
difficulty: easy
source: leetcode
problem_number: "0704"
topics:
  - Binary Search
  - Array
---

# LeetCode 0704: Binary Search

## Problem Breakdown

Given an array of integers `nums` which is sorted in ascending order, and an integer `target`, write a function to search `target` in `nums`.
If `target` exists, then return its index.
Otherwise, return `-1`.
You must write an algorithm with $O(\log n)$ runtime complexity.

### Halving Invariant

- In any sorted array, comparing the median element with `target` eliminates half of the remaining search space.
- If `nums[mid] == target`, the element is located at index `mid`.
- If `nums[mid] < target`, the target cannot reside in the range $[left, mid]$ because all elements before or at `mid` are strictly smaller than `target`. Hence, the new search boundary becomes $[mid + 1, right]$.
- If `nums[mid] > target`, the target cannot reside in the range $[mid, right]$ because all elements after or at `mid` are strictly larger than `target`. Hence, the new search boundary becomes $[left, mid - 1]$.
- Repeating this comparison until $left > right$ guarantees either finding `target` or proving its absence in at most $\lfloor \log_2 n \rfloor + 1$ iterations.

## Optimal Approaches

### Iterative Binary Search

```
nums = [-1, 0, 3, 5, 9, 12], target = 9
Indices: 0, 1, 2, 3, 4, 5

Iteration 1: left = 0, right = 5
mid = 0 + (5 - 0) / 2 = 2
nums[2] = 3 < 9 -> left = mid + 1 = 3

Iteration 2: left = 3, right = 5
mid = 3 + (5 - 3) / 2 = 4
nums[4] = 9 == 9 -> Target found at index 4

Result = 4
```

1. Initialize `left = 0` and `right = nums.length - 1`.
2. While `left <= right`:
   - Compute `mid = left + (right - left) / 2` to prevent potential integer overflow.
   - If `nums[mid] == target`, return `mid`.
   - If `nums[mid] < target`, update `left = mid + 1`.
   - Otherwise, update `right = mid - 1`.
3. If loop terminates without a match, return `-1`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(\log n)$ | Search space halves upon each comparison step. |
| **Space Complexity** | $O(1)$ | Constant extra space with two boundary indices and mid pointer. |

## Common Traps & Edge Cases

- **Integer Overflow**: Using `(left + right) / 2` can trigger signed 32-bit integer overflow if `left + right > 2^31 - 1`; using `left + (right - left) / 2` is universally safe across languages.
- **Loop Condition (`<=` vs `<`)**: Searching a closed interval $[left, right]$ requires `left <= right` so that single-element search spaces (where `left == right`) are inspected.
- **Target Not Present**: When target is smaller than the minimum element or larger than the maximum element, boundary updates contract safely to terminate the loop.

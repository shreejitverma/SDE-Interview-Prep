---
title: LeetCode - 0287 - Find the Duplicate Number
tags:
  - leetcode
  - problem
  - two-pointers
  - binary-search
  - bit-manipulation
  - array
difficulty: medium
source: leetcode
problem_number: "0287"
topics:
  - Two Pointers
  - Binary Search
  - Bit Manipulation
  - Array
---

# LeetCode 0287: Find the Duplicate Number

## Problem Breakdown

Given an array of integers `nums` containing `n + 1` integers where each integer is in the range `[1, n]` inclusive.
There is only one repeated number in `nums`, return this repeated number.
You must solve the problem without modifying the array `nums` and uses only constant extra space $O(1)$.

### Graph Duality and Pigeonhole Principle

- By Dirichlet's Pigeonhole Principle, placing $n + 1$ integers into $n$ distinct pigeonholes guaranteed that at least one integer appears multiple times.
- If we interpret the array as a directed functional graph where an edge exists from index $i$ to index $\text{nums}[i]$ ($i \to \text{nums}[i]$):
  - Because all values $\text{nums}[i] \in [1, n]$, index `0` has an out-degree of 1 and an in-degree of 0 (no element can equal 0).
  - The duplicate number has multiple incoming directed edges (in-degree $\ge 2$).
  - Walking this graph from index `0` forms a linear path leading into a directed cycle whose entry node is precisely the duplicate value.
- This is mathematically isomorphic to finding the entry point of a cycle in a linked list (LeetCode 142).

## Optimal Approaches

### Floyd's Tortoise and Hare Cycle Detection

```
nums = [1, 3, 4, 2, 2]
Indices: 0, 1, 2, 3, 4

Graph edges:
0 -> 1 -> 3 -> 2 -> 4 -> 2 (cycle between 2 and 4)

Phase 1:
slow = nums[0] = 1, fast = nums[nums[0]] = nums[1] = 3
Step 1: slow = nums[1] = 3, fast = nums[nums[3]] = nums[2] = 4
Step 2: slow = nums[3] = 2, fast = nums[nums[4]] = nums[2] = 4
Step 3: slow = nums[2] = 4, fast = nums[nums[4]] = nums[2] = 4 (Collision at node 4)

Phase 2:
fast = 0
Step 1: slow = nums[4] = 2, fast = nums[0] = 1
Step 2: slow = nums[2] = 4, fast = nums[1] = 3
Step 3: slow = nums[4] = 2, fast = nums[3] = 2 (Collision at node 2)

Result = 2 (cycle entrance)
```

1. **Phase 1 (Cycle Detection)**:
   - Initialize `slow = nums[0]` and `fast = nums[nums[0]]`.
   - Advance `slow = nums[slow]` by 1 step and `fast = nums[nums[fast]]` by 2 steps until they intersect.
2. **Phase 2 (Cycle Entrance)**:
   - Reset `fast = 0`.
   - Advance both `slow = nums[slow]` and `fast = nums[fast]` by 1 step until they meet.
   - The meeting index is the duplicate number.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n)$ | Fast pointer traverses the cycle loop in at most $2n$ steps. |
| **Space Complexity** | $O(1)$ | Strictly two integer pointers, satisfying the zero-modification constraint. |

## Common Traps & Edge Cases

- **Modifying the Array**: Marking visited numbers via negative values violates the strict read-only requirement.
- **Starting from Index Zero**: Because array values are $\ge 1$, starting at index 0 guarantees we never begin inside a zero self-loop.
- **Multiple Duplicate Frequencies**: The repeated number can occur 2, 3, or more times; Floyd's algorithm correctly routes to the entrance regardless of in-degree multiplicity.

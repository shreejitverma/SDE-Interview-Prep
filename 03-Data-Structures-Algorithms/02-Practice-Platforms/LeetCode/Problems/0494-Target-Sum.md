---
title: LeetCode - 0494 - Target Sum
tags:
  - leetcode
  - problem
  - dynamic-programming
  - knapsack
  - array
difficulty: medium
source: leetcode
problem_number: "0494"
topics:
  - Dynamic Programming
  - Knapsack
  - Array
---

# LeetCode 0494: Target Sum

## Problem Breakdown

You are given an integer array `nums` and an integer `target`.
You want to build an evaluation expression out of `nums` by adding one of the symbols `'+'` and `'-'` before each integer in `nums` and then concatenating all the integers.
Return the number of different expressions that you can build, which evaluate to `target`.

### Mathematical Reduction to 0/1 Knapsack

Let $P$ denote the subset of numbers assigned a positive sign `+`.
Let $N$ denote the subset of numbers assigned a negative sign `-`.
From the problem definition:

$$\sum P - \sum N = \text{target}$$

Since every number in `nums` must be assigned either a positive or negative sign:

$$\sum P + \sum N = \sum \text{nums}$$

Adding these two equations together:

$$2 \cdot \sum P = \text{target} + \sum \text{nums} \implies \sum P = \frac{\text{target} + \sum \text{nums}}{2}$$

This reduces the problem to finding the number of subsets of `nums` whose sum equals:

$$S = \frac{\text{target} + \sum \text{nums}}{2}$$

### Necessary Constraints

1. If $\sum \text{nums} < |\text{target}|$, no combination can ever reach $\text{target}$.
2. If $(\text{target} + \sum \text{nums})$ is odd, $\sum P$ cannot be an integer, so the answer is strictly `0`.

## Optimal Approaches

### 1D Dynamic Programming (Subset Sum)

```
nums = [1, 1, 1, 1, 1], target = 3
total_sum = 5
subset_sum = (3 + 5) / 2 = 4

dp array size = 5 (indices 0 to 4)
Initial: dp[0] = 1, all others 0

Processing each 1:
After num 1: dp[1]=1, dp[0]=1
After num 2: dp[2]=1, dp[1]=2, dp[0]=1
After num 3: dp[3]=1, dp[2]=3, dp[1]=3, dp[0]=1
After num 4: dp[4]=1, dp[3]=4, dp[2]=6, dp[1]=4, dp[0]=1
After num 5: dp[4]=5, dp[3]=10, dp[2]=10, dp[1]=5, dp[0]=1
Answer = dp[4] = 5
```

1. Initialize `dp[0] = 1` and `dp[s] = 0` for all $s > 0$.
2. For each number `num` in `nums`:
   - Traverse $s$ backwards from `subset_sum` down to `num`.
   - Update: `dp[s] = dp[s] + dp[s - num]`.
   - Backwards traversal guarantees each item is used at most once (0/1 Knapsack semantics).
3. Return `dp[subset_sum]`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n \cdot S)$ | Where $S = (\sum \text{nums} + \text{target}) / 2 \le 1000$. |
| **Space Complexity** | $O(S)$ | Space-optimized rolling 1D DP table of size $S + 1$. |

## Common Traps & Edge Cases

- **Negative Target Values**: When `target < 0`, `(total_sum + target)` can still be positive and valid, but verify that `total_sum >= abs(target)`.
- **Zeros in Array**: Elements equal to `0` can either receive a `+` or `-` sign, each doubling the number of valid subset expressions ($2^{\text{zeros}}$). The backwards DP loop naturally handles `0` correctly because `dp[s] += dp[s - 0]`.
- **Odd Sum Parity**: If `(total_sum + target) % 2 != 0`, immediately return `0`.

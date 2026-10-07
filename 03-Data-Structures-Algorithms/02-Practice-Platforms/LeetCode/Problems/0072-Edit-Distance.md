---
title: LeetCode - 0072 - Edit Distance
tags:
  - leetcode
  - problem
  - dynamic-programming
  - string
difficulty: hard
source: leetcode
problem_number: "0072"
topics:
  - Dynamic Programming
  - String
---

# LeetCode 0072: Edit Distance

## Problem Breakdown

Given two strings `word1` and `word2`, return the minimum number of operations required to convert `word1` to `word2`.
You have the following three operations permitted on a word:
1. Insert a character.
2. Delete a character.
3. Replace a character.

### Recurrence Formulation

Let `dp[i][j]` represent the minimum number of edit operations required to convert the prefix `word1[0..i)` to `word2[0..j)`.

- **Base Cases**:
  - `dp[0][j] = j`: Converting an empty string to a prefix of length $j$ requires $j$ insertions.
  - `dp[i][0] = i`: Converting a prefix of length $i$ to an empty string requires $i$ deletions.
- **Inductive Step**:
  - If characters match (`word1[i - 1] == word2[j - 1]`):
    $$\text{dp}[i][j] = \text{dp}[i - 1][j - 1]$$
  - If characters differ (`word1[i - 1] != word2[j - 1]`):
    $$\text{dp}[i][j] = 1 + \min(\text{dp}[i - 1][j], \text{dp}[i][j - 1], \text{dp}[i - 1][j - 1])$$
    Where:
    - $\text{dp}[i - 1][j]$ corresponds to **deleting** `word1[i - 1]`.
    - $\text{dp}[i][j - 1]$ corresponds to **inserting** `word2[j - 1]`.
    - $\text{dp}[i - 1][j - 1]$ corresponds to **replacing** `word1[i - 1]` with `word2[j - 1]`.

## Optimal Approaches

### Space-Optimized 1D Dynamic Programming

Because computing row $i$ only depends on the current row and the immediate previous row, we can reduce storage to a single 1D vector of length $\min(m, n) + 1$.
A scalar variable `prev_diag` stores the value corresponding to $\text{dp}[i - 1][j - 1]$.

```
word1 = "horse", word2 = "ros"

DP Matrix:
      ""   r   o   s
""    0    1   2   3
h     1    1   2   3
o     2    2   1   2
r     3    2   2   2
s     4    3   3   2
e     5    4   4   3

Minimum operations = 3
1. horse -> rorse (replace 'h' with 'r')
2. rorse -> rose  (delete 'r')
3. rose  -> ros   (delete 'e')
```

1. If $m < n$, swap words so $n \le m$, ensuring the DP array size is strictly $O(\min(m, n))$.
2. Initialize array `dp[j] = j` for $0 \le j \le n$.
3. For each character $i$ from 1 to $m$:
   - Set `prev_diag = dp[0]` and update `dp[0] = i`.
   - For each character $j$ from 1 to $n$:
     - Store current `dp[j]` in a temporary variable.
     - If `word1[i - 1] == word2[j - 1]`, set `dp[j] = prev_diag`.
     - Otherwise, set `dp[j] = 1 + min({dp[j], dp[j - 1], prev_diag})`.
     - Update `prev_diag = temp`.
4. Return `dp[n]`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(m \cdot n)$ | Nested loops examining all prefix pairs once with constant transition cost. |
| **Space Complexity** | $O(\min(m, n))$ | Rolling single 1D buffer sized to the shorter string. |

## Common Traps & Edge Cases

- **Empty Strings**: If either string is empty, the answer is trivially the length of the non-empty string.
- **Identical Strings**: When strings are identical, zero operations are needed; the diagonal matches skip all increments.
- **Memory Overhead**: Storing the full $M \times N$ matrix is unnecessary and incurs high cache-miss latency for large texts.

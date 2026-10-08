---
title: LeetCode - 0010 - Regular Expression Matching
tags:
  - leetcode
  - problem
  - dynamic-programming
  - string
  - recursion
difficulty: hard
source: leetcode
problem_number: "0010"
topics:
  - Dynamic Programming
  - String
  - Recursion
---

# LeetCode 0010: Regular Expression Matching

## Problem Breakdown

Given an input string `s` and a pattern `p`, implement regular expression matching with support for `'.'` and `'*'` where:
- `'.'` Matches any single character.
- `'*'` Matches zero or more of the preceding element.
The matching should cover the entire input string (not partial).

### State Transition Invariants

Let `dp[i][j]` be a boolean denoting whether the prefix `s[0..i)` matches the pattern prefix `p[0..j)`.

1. **Base Case**:
   - `dp[0][0] = true`: Empty string matches empty pattern.
   - For an empty string `s` ($i = 0$), a pattern token like `a*` can match zero characters:
     $$\text{dp}[0][j] = \text{dp}[0][j - 2] \quad \text{if } p[j - 1] == \text{'*'}$$
2. **Standard Character or Dot Match** ($p[j - 1] \ne \text{'*'}$):
   - If $p[j - 1] == \text{'.'}$ or $p[j - 1] == s[i - 1]$:
     $$\text{dp}[i][j] = \text{dp}[i - 1][j - 1]$$
3. **Star Wildcard Transition** ($p[j - 1] == \text{'*'}$):
   - **Zero Occurrences**: Ignore the preceding character and the star:
     $$\text{dp}[i][j] = \text{dp}[i][j - 2]$$
   - **One or More Occurrences**: If $p[j - 2]$ matches $s[i - 1]$ (or $p[j - 2] == \text{'.'}`):
     $$\text{dp}[i][j] = \text{dp}[i][j] \lor \text{dp}[i - 1][j]$$

## Optimal Approaches

### 2D Dynamic Programming Matrix

```
s = "aab", p = "c*a*b"

Matrix evaluation:
       ""   c   *   a   *   b
""     T    F   T   F   T   F
a      F    F   F   T   T   F
a      F    F   F   F   T   F
b      F    F   F   F   F   T

Result: dp[3][5] = True
Explanation: c* matches "" (0 c's), a* matches "aa", b matches "b".
```

1. Allocate a boolean 2D table `dp` of size `(m + 1) x (n + 1)` initialized to `false`.
2. Set `dp[0][0] = true`.
3. Pre-populate row 0 for star patterns: if `p[j - 1] == '*'`, set `dp[0][j] = dp[0][j - 2]`.
4. Iterate `i` from 1 to $m$ and `j` from 1 to $n$:
   - If `p[j - 1] == '*'`:
     - Test zero matches: `dp[i][j] = dp[i][j - 2]`.
     - Test single/multiple matches: if `p[j - 2] == '.' || p[j - 2] == s[i - 1]`, evaluate `dp[i][j] = dp[i][j] || dp[i - 1][j]`.
   - Else if `p[j - 1] == '.' || p[j - 1] == s[i - 1]`:
     - `dp[i][j] = dp[i - 1][j - 1]`.
5. Return `dp[m][n]`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(m \cdot n)$ | Each cell `(i, j)` is evaluated once with constant number of state checks. |
| **Space Complexity** | $O(m \cdot n)$ | Can be reduced to $O(n)$ with two rolling rows since row $i$ depends only on $i - 1$. |

## Common Traps & Edge Cases

- **Patterns Starting with Wildcard Quantifiers**: Valid inputs guarantee `'*'` is always preceded by a valid char, so $j \ge 2$ when encountering `'*'`.
- **Preceding Token Matching Dot**: When pattern is `.*`, it can absorb any character in `s` repeatedly; `dp[i - 1][j]` handles arbitrary repetition.
- **Empty String Matches**: Empty string `s = ""` can match complex patterns such as `"a*b*c*"`.

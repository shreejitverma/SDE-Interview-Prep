---
title: LeetCode - 0131 - Palindrome Partitioning
tags:
  - leetcode
  - problem
  - backtracking
  - dynamic-programming
  - string
difficulty: medium
source: leetcode
problem_number: "0131"
topics:
  - Backtracking
  - Dynamic Programming
  - String
---

# LeetCode 0131: Palindrome Partitioning

## Problem Breakdown

Given a string `s`, partition `s` such that every substring of the partition is a palindrome.
Return all possible palindrome partitioning of `s`.

### Backtracking Search Space Analysis

- A string of length $n$ contains $n - 1$ potential cut points between adjacent characters.
- Each cut point can either be selected or bypassed, giving $2^{n - 1}$ total possible partitions.
- For a partition to be valid, every segment `s[start..end]` produced by the cuts must satisfy the palindrome property ($s[i] == s[\text{end} - (i - \text{start})]$).
- A depth-first backtracking search generates partitions incrementally:
  - From index `start`, explore all possible ending boundaries `end` ($start \le end < n$).
  - If the prefix `s[start..end]` is a palindrome, append it to the current path and recursively solve for the suffix starting at `end + 1`.
  - When the search reaches `start == n`, the active path constitutes a complete, valid palindrome partition.

## Optimal Approaches

### Recursive Backtracking with Two-Pointer Validation

```
s = "aab"

Path exploration:
start = 0:
  end = 0: "a" is palindrome -> current = ["a"]
    start = 1:
      end = 1: "a" is palindrome -> current = ["a", "a"]
        start = 2:
          end = 2: "b" is palindrome -> current = ["a", "a", "b"]
            start = 3 == n -> Add ["a", "a", "b"] to result
      end = 2: "ab" not palindrome
  end = 1: "aa" is palindrome -> current = ["aa"]
    start = 2:
      end = 2: "b" is palindrome -> current = ["aa", "b"]
        start = 3 == n -> Add ["aa", "b"] to result
  end = 2: "aab" not palindrome

Result: [["a", "a", "b"], ["aa", "b"]]
```

1. Maintain `result` list and `current` path buffer.
2. In `backtrack(start)`:
   - If `start == s.length()`, clone `current` into `result` and return.
   - For `end` from `start` to `s.length() - 1`:
     - Test whether `s[start..end]` is a palindrome using two pointers.
     - If true, push substring to `current`, call `backtrack(end + 1)`, and pop back.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(n \cdot 2^n)$ | There are $2^{n - 1}$ partitions; verifying palindromes and copying strings takes $O(n)$ time per leaf. |
| **Space Complexity** | $O(n)$ | Call stack depth bounded by string length $n$, excluding the returned results. |

## Common Traps & Edge Cases

- **Substring Boundary Indices**: Ensure end indices match language substring slice semantics (e.g. $[start, end + 1)$ vs $[start, end]$).
- **Single Character Strings**: Strings with length 1 (e.g. `"a"`) trivially have one valid partition `[["a"]]`.
- **Precomputed Palindrome DP**: For longer strings, precomputing an $N \times N$ boolean DP table reduces palindrome checks to $O(1)$, though backtracking dominates overall asymptotic bounds.

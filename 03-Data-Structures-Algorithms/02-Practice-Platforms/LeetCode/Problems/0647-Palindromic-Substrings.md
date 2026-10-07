---
id: 0647-palindromic-substrings
title: "LeetCode 647: Palindromic Substrings (Manacher's Algorithm & Center Expansion Deep Dive)"
tags:
  - dsa
  - leetcode
  - string
  - dynamic-programming
  - two-pointers
  - manachers-algorithm
level: medium
type: problem-breakdown
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/palindromic-substrings/"
---

# LeetCode 647: Palindromic Substrings (Manacher's Algorithm & Center Expansion Deep Dive)

## 1. Problem Statement and Architectural Overview

Given a string `s`, return the number of palindromic substrings in it.
A string is a palindrome when it reads the same backward as forward.
A substring is a contiguous sequence of characters within the string.

### Critical Constraints
- $1 \le \text{s.length} \le 1000$
- `s` consists of lowercase English letters.

---

## 2. Mathematical Formalism and Invariant Proofs

Let $S = s_0 s_1 \dots s_{n-1}$ be a string of length $n$.
A substring $S[i \dots j]$ is palindromic if $S[i+k] = S[j-k]$ for all $0 \le k \le \lfloor (j - i) / 2 \rfloor$.
There are $\binom{n+1}{2} = \frac{n(n+1)}{2}$ contiguous substrings in $S$.
Each palindromic substring is uniquely determined by its symmetry center.
A palindrome of odd length $2k + 1$ has center at integer index $i$.
A palindrome of even length $2k$ has center between indices $i$ and $i + 1$.
Thus, there exist exactly $2n - 1$ possible palindrome centers in $S$.

### Manacher's Transformation and Palindrome Radius Invariant
Define the transformed string $T = \ \text{"\textasciicircum\#"} \cdot s_0 \cdot \text{"\#"} \cdot s_1 \dots \text{"\#"} \cdot s_{n-1} \cdot \text{"\#\$"}$.
By inserting sentinel characters `#`, every palindrome in $S$ maps to an odd-length palindrome in $T$.
Let $P[i]$ denote the radius of the longest palindrome centered at index $i$ in $T$, such that $T[i - P[i] \dots i + P[i]]$ is a palindrome.
Notice that in the original string $S$, a palindrome radius of $P[i]$ in $T$ corresponds to exactly:
$$K_i = \left\lfloor \frac{P[i] + 1}{2} \right\rfloor$$
distinct palindromic substrings centered at $i$.
The total number of palindromic substrings in $S$ is therefore:
$$\text{Total} = \sum_{i=1}^{|T|-2} \left\lfloor \frac{P[i] + 1}{2} \right\rfloor$$

### Manacher's Linear Time Invariant Proof
Maintain the current rightmost palindrome boundary $R$ and its corresponding center $C$.
For any index $i < R$, let $i' = 2C - i$ be the mirror of $i$ with respect to center $C$.
Because $T[C - (R - C) \dots C + (R - C)]$ is a palindrome, the substring centered at $i$ is initially identical to the substring centered at $i'$.
Hence:
$$P[i] \ge \min(R - i, P[i'])$$
We only expand characters when $i + P[i] \ge R$.
Whenever an expansion succeeds, $R$ increases strictly.
Because $R$ advances monotonically from $1$ to $|T|$ and never decreases, the total number of character comparisons is bounded by $2|T| = O(N)$.
Therefore, Manacher's algorithm runs in strictly $O(N)$ worst-case time.

---

## 3. Four-Tier Solution Architecture

### Tier 1: Optimal Manacher's Algorithm
- **Core Concept**: Preprocess string with boundary and delimiter sentinels.
- Exploit mirror reflection $P[i] = \min(R - i, P[2C - i])$ to skip redundant character comparisons.
- Accumulate $(P[i] + 1) / 2$ for each center.
- **Time Complexity**: $O(N)$ strictly linear time.
- **Space Complexity**: $O(N)$ auxiliary space for the transformed string and radius array.

### Tier 2: Space-Optimized Center Expansion (Two Pointers)
- **Core Concept**: Iterate through all $2n - 1$ centers.
- For odd centers, expand outwards from $(i, i)$.
- For even centers, expand outwards from $(i, i + 1)$.
- Increment total counter on every matching character match until mismatch or boundary hit.
- **Time Complexity**: $O(N^2)$ worst case (e.g. all characters identical, $s = \text{"aaaa"}$).
- **Space Complexity**: $O(1)$ auxiliary space without any dynamic heap allocation.

### Tier 3: 2D Dynamic Programming Tabulation
- **Core Concept**: Maintain boolean table $\text{dp}[i][j]$ indicating whether $S[i \dots j]$ is a palindrome.
- Recurrence: $\text{dp}[i][j] = (S[i] == S[j]) \land (j - i \le 2 \lor \text{dp}[i+1][j-1])$.
- Count `true` entries in the upper triangular matrix.
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(N^2)$ (can be reduced to $O(N)$ via sliding rows).

### Tier 4: Naive Cubic Verification
- **Core Concept**: Enumerate all $O(N^2)$ substring pairs $(i, j)$.
- For each substring, run a two-pointer palindrome check taking $O(j - i) = O(N)$ time.
- **Time Complexity**: $O(N^3)$ operations ($10^9$ operations for $N = 1000$, resulting in Time Limit Exceeded).
- **Space Complexity**: $O(1)$ auxiliary space.

---

## 4. Hardware, Memory, and Cache Systems Considerations

1. **Center Expansion Cache Line Locality**: The two-pointer expansion reads memory symmetrically outwards from the center.
For small strings ($N \le 1000$), the entire string resides in L1 cache, allowing center expansion to achieve sub-millisecond execution times despite $O(N^2)$ theoretical complexity.
2. **Sentinel Characters in Manacher's**: Using `^` and `$` sentinels at string ends completely eliminates boundary condition checks inside the inner expansion loop, saving branch misprediction stalls.
3. **Branch Elimination**: The inner expansion loop in Tier 1 executes branch-free character equality checks until mismatch.

---

## 5. Edge-Case Boundary Defense Matrix

| Edge Case Dimension | Input Scenario | Expected Behavior | Failure Mode Without Defense |
|:---|:---|:---|:---|
| Single Character | `s = "a"` | Return `1` | Out-of-bounds array access on center expansion |
| All Identical Characters | `s = "aaaa"` | Return $n(n+1)/2 = 10$ | Infinite loop or incorrect count on repeated matches |
| No Multi-Character Palindromes | `s = "abcde"` | Return $n = 5$ | Off-by-one undercount omitting single character centers |
| Two Identical Characters | `s = "aa"` | Return `3` ("a", "a", "aa") | Omitting even-length center expansion |
| Alternating Characters | `s = "aba"` | Return `4` ("a", "b", "a", "aba") | Missing outer expansion after single character match |

---

## 6. Comprehensive 10 Frequently Asked Questions (FAQ)

### 1. Why does Manacher's algorithm run in $O(N)$ time?
Because the right boundary $R$ moves strictly to the right.
Every character comparison that successfully expands a palindrome increases $R$.
Since $R$ cannot exceed $|T|$, the total number of successful expansions across the entire algorithm is at most $|T|$.

### 2. Why insert '#' sentinels between characters?
Sentinels transform both even-length and odd-length palindromes into odd-length palindromes, allowing a single unified expansion logic.

### 3. Why are '^' and '$' used as boundary sentinels?
Using distinct sentinels `^` at the beginning and `$` at the end guarantees that the while loop `T[i + 1 + P[i]] == T[i - 1 - P[i]]` terminates automatically without checking index bounds.

### 4. How does $(P[i] + 1) / 2$ derive the palindrome count?
In the transformed string $T$, a palindrome radius of $P[i]$ spans $P[i]$ original characters and delimiters.
Integer division $(P[i] + 1) / 2$ exactly counts how many original string palindromes share that center.

### 5. When is the $O(N^2)$ center expansion preferred over Manacher's?
Center expansion is simpler to implement, requires $O(1)$ auxiliary memory, and runs faster in practice for small strings ($N \le 1000$) due to lower constant factors and zero string reallocation overhead.

### 6. Can Manacher's algorithm be modified to find the longest palindromic substring?
Yes, tracking the maximum value of $P[i]$ and its center $C$ directly yields LeetCode 5 (Longest Palindromic Substring).

### 7. How does the 2D DP solution traverse the matrix?
The DP solution must evaluate substrings in increasing order of length (or backwards on row index $i$ and forwards on column index $j$) so subproblems $\text{dp}[i+1][j-1]$ are available.

### 8. What is the maximum number of palindromic substrings for a string of length $N$?
The maximum occurs when all characters are identical (e.g. "aaaa"), yielding $\frac{N(N+1)}{2}$ palindromic substrings.

### 9. Can rolling hash (Rabin-Karp) be used here?
Yes, computing polynomial forward and reverse rolling hashes allows verifying palindromes in $O(1)$ after $O(N)$ preprocessing, but hash collisions can introduce false positives unless multi-modulus hashing is used.

### 10. How does this problem relate to LeetCode 5?
LeetCode 5 asks for the single longest palindromic substring.
LeetCode 647 asks for the total count of all valid palindromic substrings.
Both share the exact same core algorithmic foundations (Manacher and Center Expansion).

---

## 7. Related Problem Cross-References

- [[0005-Longest-Palindromic-Substring|LeetCode 5: Longest Palindromic Substring]]
- [[0125-Valid-Palindrome|LeetCode 125: Valid Palindrome]]

---

## 8. Standalone Implementation Links

- [C++ Implementation](../C++/palindromic-substrings.cpp)
- [Python Implementation](../Python/palindromic-substrings.py)
- [Java Implementation](../Java/palindromic-substrings.java)
- [TypeScript Implementation](../TypeScript/palindromic-substrings.ts)
- [Go Implementation](../Golang/palindromic-substrings.go)
- [Rust Implementation](../Rust/palindromic-substrings.rs)

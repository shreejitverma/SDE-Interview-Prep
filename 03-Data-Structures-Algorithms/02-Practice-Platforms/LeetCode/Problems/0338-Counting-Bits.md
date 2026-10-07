---
id: 0338-counting-bits
title: "LeetCode 338: Counting Bits (Dynamic Programming & Bit Manipulation Deep Dive)"
tags:
  - dsa
  - leetcode
  - dynamic-programming
  - bit-manipulation
level: easy
type: problem-breakdown
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/counting-bits/"
---

# LeetCode 338: Counting Bits (Dynamic Programming & Bit Manipulation Deep Dive)

## 1. Problem Statement and Architectural Overview

Given an integer $n$, return an array `ans` of length $n + 1$ such that for each $i$ ($0 \le i \le n$), `ans[i]` is the number of $1$'s in the binary representation of $i$.

### Critical Constraints
- $0 \le n \le 10^5$
- Follow-up: An $O(n)$ time solution running in a single pass without relying on built-in popcount library functions.

---

## 2. Mathematical Formalism and Invariant Proofs

Let $\text{popcount}(x)$ denote the Hamming weight of an integer $x \ge 0$, defined as the number of set bits in its binary expansion:
$$\text{popcount}(x) = \sum_{k=0}^{31} \left( \lfloor x \cdot 2^{-k} \rfloor \pmod 2 \right)$$

### Inductive Invariant via Right-Shift (Least Significant Bit Decomposition)
Any non-negative integer $i$ can be expressed uniquely as:
$$i = 2 \cdot \lfloor i / 2 \rfloor + (i \bmod 2) = (i \gg 1) \ll 1 + (i \ \& \ 1)$$
Notice that shifting $i$ right by 1 bit removes exactly the least significant bit ($i \ \& \ 1$) and preserves all other set bits.
By linearity of bit summation:
$$\text{popcount}(i) = \text{popcount}(i \gg 1) + (i \ \& \ 1)$$
Because $i \gg 1 < i$ for all $i \ge 1$, $\text{popcount}(i \gg 1)$ is already evaluated and stored in the DP table.
Thus, by mathematical induction on $i$, every entry $i \in [1, n]$ is computed in exact $O(1)$ constant time from base case $\text{ans}[0] = 0$.

### Alternative Inductive Invariant via Brian Kernighan's Property
Consider the bitwise operation $i \ \& \ (i - 1)$.
Subtracting 1 from $i$ inverts all bits up to and including the lowest set bit.
Therefore, taking the bitwise AND of $i$ and $i - 1$ clears precisely the lowest set bit of $i$, leaving all higher set bits unchanged.
Hence:
$$\text{popcount}(i) = \text{popcount}(i \ \& \ (i - 1)) + 1$$
Since $i \ \& \ (i - 1) < i$, this recurrence provides an equally rigorous, branch-free $O(1)$ DP step.

---

## 3. Four-Tier Solution Architecture

### Tier 1: Optimal Dynamic Programming via Least Significant Bit
- **Core Concept**: Compute table sequentially from $1$ to $n$ using $\text{ans}[i] = \text{ans}[i \gg 1] + (i \ \& \ 1)$.
- Accesses previous entries at index $i / 2$, which resides safely in L1 cache due to recent writes.
- **Time Complexity**: $O(N)$ single pass with exactly one shift, one AND, and one addition per element.
- **Space Complexity**: $O(1)$ auxiliary space excluding the allocated output array of size $N + 1$.

### Tier 2: Dynamic Programming via Lowest Set Bit (Kernighan Transition)
- **Core Concept**: Compute table using $\text{ans}[i] = \text{ans}[i \ \& \ (i - 1)] + 1$.
- Bypasses parity checks entirely, relying on clearing the lowest bit.
- **Time Complexity**: $O(N)$ single pass.
- **Space Complexity**: $O(1)$ auxiliary space.

### Tier 3: Dynamic Programming via Most Significant Bit (Range Doubling)
- **Core Concept**: Maintain the highest power of 2 seen so far ($\text{offset}$).
- When $i == \text{offset} \times 2$, update $\text{offset} = i$.
- Then $\text{ans}[i] = \text{ans}[i - \text{offset}] + 1$.
- Mirrors the pattern of blocks $[2^k, 2^{k+1}-1]$ repeating $[0, 2^k - 1]$ shifted by 1.
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(1)$ auxiliary space.

### Tier 4: Naive Exhaustive Popcount per Integer
- **Core Concept**: For each integer $i \in [0, n]$, count set bits using a loop shifting through 32 bits or Brian Kernighan's loop.
- Performs $\Theta(N \log N)$ or $\Theta(32N)$ operations with zero subproblem reuse.
- **Time Complexity**: $O(N \log N)$ operations.
- **Space Complexity**: $O(1)$ auxiliary space.

---

## 4. Hardware, Memory, and Cache Systems Considerations

1. **Sequential Memory Writes**: The DP array is written strictly sequentially from $i = 0$ to $n$, which aligns perfectly with modern CPU store buffers and cache line allocation policies.
2. **Cache Locality of $i \gg 1$**: When computing element $i$, the dependent element $i / 2$ was computed earlier in the recent half of the loop.
For moderate $n \le 10^5$, the entire array occupies $400 \text{ KB}$, fitting comfortably within L2 cache ($512 \text{ KB}$ to $1 \text{ MB}$).
3. **Pipelining and Superscalar Execution**: The Tier 1 expression `(i & 1) + ans[i >> 1]` contains independent arithmetic and shift sub-expressions that execute concurrently on modern superscalar ALUs.

---

## 5. Edge-Case Boundary Defense Matrix

| Edge Case Dimension | Input Scenario | Expected Behavior | Failure Mode Without Defense |
|:---|:---|:---|:---|
| Zero Input | $n = 0$ | Return `[0]` | Array out-of-bounds allocating size 0 instead of $n + 1$ |
| Smallest Non-Zero | $n = 1$ | Return `[0, 1]` | Off-by-one loop boundary |
| Power of Two | $n = 16$ | Correct transition at $16$ (`ans[16] = 1`) | Stale offset in range doubling |
| Large Input | $n = 10^5$ | Linear completion in $< 2 \text{ ms}$ | Time limit exceeded if using naive 32-bit loop |

---

## 6. Comprehensive 10 Frequently Asked Questions (FAQ)

### 1. What is the fundamental recurrence for counting bits?
The most idiomatic recurrence is $\text{ans}[i] = \text{ans}[i \gg 1] + (i \ \& \ 1)$, which reduces the problem to an already solved prefix plus the parity bit.

### 2. How does $i \ \& \ (i - 1)$ compare to $i \gg 1$?
Both run in $O(1)$ time per step.
$i \gg 1$ leverages sequential right-shifting, while $i \ \& \ (i - 1)$ removes the lowest set bit.
Both yield identical output.

### 3. Why is the time complexity strictly $O(N)$?
Every integer from $1$ to $n$ performs exactly three constant-time primitive operations: one shift, one bitwise AND, and one table lookup addition.

### 4. What is the base case?
The base case is $\text{ans}[0] = 0$, representing zero set bits for zero.

### 5. Why do we allocate $n + 1$ elements?
Because the index range includes $0$ up through $n$ inclusive, requiring $n + 1$ elements.

### 6. Can this be parallelized across multiple threads?
Yes, using the formula $\text{popcount}(i) = \text{popcount}(i \gg 1) + (i \ \& \ 1)$ within blocks, or computing SIMD vector popcount instructions (`VPOPCNTDQ` on AVX-512).

### 7. Does this require 64-bit integer handling?
No, the input is bounded by $10^5$, which fits comfortably inside a standard 32-bit signed or unsigned integer.

### 8. How does this relate to LeetCode 191 (Number of 1 Bits)?
LeetCode 191 calculates the popcount of a single 32-bit integer.
LeetCode 338 calculates the popcounts of all integers from $0$ to $n$ simultaneously using DP to avoid recomputation.

### 9. Can we use range doubling?
Yes, every power-of-two interval $[2^k, 2^{k+1}-1]$ is a mirror of $[0, 2^k - 1]$ with each value incremented by 1 (the leading bit).

### 10. Which solution has the smallest instruction count?
The Kernighan transition $\text{ans}[i] = \text{ans}[i \ \& \ (i - 1)] + 1$ often compiles to fewer ALU cycles because it performs one bitwise operation and one addition.

---

## 7. Related Problem Cross-References

- [[0190-Reverse-Bits|LeetCode 190: Reverse Bits]]
- [[0191-Number-of-1-Bits|LeetCode 191: Number of 1 Bits]]
- [[0268-Missing-Number|LeetCode 268: Missing Number]]
- [[0078-Subsets|LeetCode 78: Subsets]]

---

## 8. Standalone Implementation Links

- [C++ Implementation](../C++/counting-bits.cpp)
- [Python Implementation](../Python/counting-bits.py)
- [Java Implementation](../Java/counting-bits.java)
- [TypeScript Implementation](../TypeScript/counting-bits.ts)
- [Go Implementation](../Golang/counting-bits.go)
- [Rust Implementation](../Rust/counting-bits.rs)

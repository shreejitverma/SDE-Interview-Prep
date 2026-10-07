---
id: 0268-missing-number
title: "LeetCode 268: Missing Number (Bit Manipulation & Math Deep Dive)"
tags:
  - dsa
  - leetcode
  - bit-manipulation
  - math
  - array
level: easy
type: problem-breakdown
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/missing-number/"
---

# LeetCode 268: Missing Number (Bit Manipulation & Math Deep Dive)

## 1. Problem Statement and Architectural Overview

Given an array `nums` containing $n$ distinct numbers in the range $[0, n]$, return the only number in the range that is missing from the array.

### Critical Constraints
- $n == \text{nums.length}$
- $1 \le n \le 10^4$
- $0 \le \text{nums}[i] \le n$
- All the numbers of `nums` are unique.

---

## 2. Mathematical Formalism and Invariant Proofs

Let $S = \{0, 1, 2, \dots, n\}$ be the complete set of integers in the closed range $[0, n]$.
Let $A = \{\text{nums}[0], \text{nums}[1], \dots, \text{nums}[n-1]\}$ be the input multiset where $|A| = n$ and $A \subset S$.
Because all elements in $A$ are distinct, there exists a unique element $m \in S \setminus A$.

### Bitwise Invariant Proof via XOR Group Properties
Consider the binary operation $\oplus$ (bitwise exclusive OR).
Recall the fundamental abelian group properties of $(\mathbb{Z}, \oplus)$:
1. Commutativity: $a \oplus b = b \oplus a$
2. Associativity: $(a \oplus b) \oplus c = a \oplus (b \oplus c)$
3. Identity element: $a \oplus 0 = a$
4. Self-inverse property: $a \oplus a = 0$

Define the compound XOR reduction over both the complete index range $[0, n]$ and the array values $A$:
$$X = \left( \bigoplus_{i=0}^n i \right) \oplus \left( \bigoplus_{j=0}^{n-1} \text{nums}[j] \right)$$
Rearranging terms by commutativity and pairing identical elements:
$$X = m \oplus \left( \bigoplus_{x \in A} (x \oplus x) \right) = m \oplus \left( \bigoplus_{x \in A} 0 \right) = m \oplus 0 = m$$
Hence, reducing all indices and elements under bitwise XOR isolates the missing value $m$ in exactly $O(N)$ operations with zero auxiliary storage and absolute immunity to arithmetic overflow.

---

## 3. Four-Tier Solution Architecture

### Tier 1: Optimal Single-Pass Bitwise XOR
- **Core Concept**: Seed an accumulator with $n$.
- Loop $i$ from $0$ to $n-1$, accumulating $\text{acc} = \text{acc} \oplus i \oplus \text{nums}[i]$.
- Every integer present in both the index domain and the value domain cancels out into $0$.
- The unmatched missing number survives as the final value.
- **Time Complexity**: $O(N)$ single sequential pass.
- **Space Complexity**: $O(1)$ auxiliary space utilizing a single CPU integer register.

### Tier 2: Closed-Form Gaussian Summation
- **Core Concept**: Compute the expected arithmetic series sum:
$$\Sigma_{\text{expected}} = \frac{n(n + 1)}{2}$$
- Subtract each array element from the expected sum:
$$m = \Sigma_{\text{expected}} - \sum_{i=0}^{n-1} \text{nums}[i]$$
- **Time Complexity**: $O(N)$ single pass.
- **Space Complexity**: $O(1)$ auxiliary space.
- Note: When $n$ is very large (e.g. $n > 65535$ in 32-bit signed environments), $n(n+1)$ can overflow if not promoted to 64-bit integers.

### Tier 3: In-Place Cyclic Sort
- **Core Concept**: Place each number $\text{nums}[i]$ at index $\text{nums}[i]$ via cyclic swaps while $\text{nums}[i] < n$ and $\text{nums}[i] \ne \text{nums}[\text{nums}[i]]$.
- After stabilization, the first index $i$ where $\text{nums}[i] \ne i$ is the missing number.
- If all indices $0 \le i < n$ match, the missing number is $n$.
- **Time Complexity**: $O(N)$ since each element is swapped to its final position at most once.
- **Space Complexity**: $O(1)$ auxiliary space (mutates input array).

### Tier 4: Hash Set / Sorting Baseline
- **Core Concept**: Insert all elements into an unordered hash set.
- Iterate $i$ from $0$ to $n$ and query presence in $O(1)$.
- Alternatively, sort the array in $O(N \log N)$ and search for the index mismatch.
- **Time Complexity**: $O(N)$ with hash set, $O(N \log N)$ with sorting.
- **Space Complexity**: $O(N)$ auxiliary heap memory for the hash set.

---

## 4. Hardware, Memory, and Cache Systems Considerations

1. **Instruction-Level Parallelism (ILP)**: The XOR accumulation loop can be vectorized using SIMD registers (AVX2/NEON) to XOR 8 or 16 integers per cycle.
2. **Branchless Execution**: The Tier 1 XOR solution executes completely branch-free, achieving a 100% predictable instruction pipeline with zero branch misprediction penalties.
3. **Cache Friendliness**: The array is read in strict sequential order, allowing the CPU hardware prefetcher to load subsequent cache lines with zero memory stall cycles.

---

## 5. Edge-Case Boundary Defense Matrix

| Edge Case Dimension | Input Scenario | Expected Behavior | Failure Mode Without Defense |
|:---|:---|:---|:---|
| Missing Smallest Value | `nums = [1, 2, 3]`, $n = 3$ | Return `0` | Off-by-one index loop omitting $0$ check |
| Missing Largest Value | `nums = [0, 1, 2]`, $n = 3$ | Return `3` | Failure to initialize accumulator with $n$ |
| Minimal Input Size | `nums = [0]`, $n = 1$ | Return `1` | Out-of-bounds loop terminating prematurely |
| Minimal Input Missing Zero | `nums = [1]`, $n = 1$ | Return `0` | Returning default zero incorrectly or crashing |
| Large $n$ Arithmetic Overflow | $n = 10^5$ | Gauss sum overflows 32-bit integer | Integer overflow wrapping into negative numbers |

---

## 6. Comprehensive 10 Frequently Asked Questions (FAQ)

### 1. Why is bitwise XOR preferred over Gaussian summation?
Gaussian summation requires computing $n(n + 1) / 2$, which can overflow standard 32-bit signed integers when $n \ge 65,536$.
Bitwise XOR never overflows because the result at every bit position is bounded between 0 and 1.

### 2. Can XOR handle negative numbers or non-contiguous sets?
No, this specific algebraic cancellation invariant relies on elements being a permutation of $[0, n] \setminus \{m\}$.
If values outside $[0, n]$ are introduced, the XOR pairings no longer collapse to zero.

### 3. How does cyclic sort compare to XOR?
Cyclic sort runs in $O(N)$ time and $O(1)$ space, but it mutates the input array and incurs cache misses during pointer swaps.
XOR is read-only, cache-friendly, and branch-free.

### 4. What is the CPU cost of the XOR operation?
XOR is a single-cycle primitive instruction with latency and throughput of 1 cycle on almost all modern CPU architectures.

### 5. Why initialize the accumulator to $n$?
The array has length $n$, so indices run from $0$ to $n-1$.
Initializing the accumulator to $n$ incorporates the final number in the range $[0, n]$ into the XOR reduction.

### 6. Can the Gauss sum avoid overflow without 64-bit integers?
Yes, by accumulating differences incrementally: $\text{diff} = \sum_{i=0}^{n-1} (i - \text{nums}[i]) + n$.
This prevents the sum from growing larger than $n$.

### 7. How does this problem relate to LeetCode 136 (Single Number)?
Both problems leverage the exact same self-inverse property of XOR ($x \oplus x = 0$) to isolate an element appearing an odd number of times.

### 8. What happens if multiple numbers are missing?
If two numbers are missing, the XOR sum will equal $a \oplus b$.
Separating $a$ and $b$ requires partitioning the numbers using a differentiating bit, as in LeetCode 260 (Single Number III).

### 9. Can binary search be applied here?
Binary search can be applied only if the array is already sorted, yielding $O(\log N)$ time.
Sorting an unsorted array takes $O(N \log N)$, which is strictly worse than $O(N)$ XOR.

### 10. Does compiler auto-vectorization optimize the XOR loop?
Modern C++ and Rust compilers (GCC, Clang, rustc) automatically unroll and vectorize the sequential XOR fold using 128-bit or 256-bit SIMD registers.

---

## 7. Related Problem Cross-References

- [[0190-Reverse-Bits|LeetCode 190: Reverse Bits]]
- [[0191-Number-of-1-Bits|LeetCode 191: Number of 1 Bits]]
- [[0078-Subsets|LeetCode 78: Subsets]]

---

## 8. Standalone Implementation Links

- [C++ Implementation](../C++/missing-number.cpp)
- [Python Implementation](../Python/missing-number.py)
- [Java Implementation](../Java/missing-number.java)
- [TypeScript Implementation](../TypeScript/missing-number.ts)
- [Go Implementation](../Golang/missing-number.go)
- [Rust Implementation](../Rust/missing-number.rs)

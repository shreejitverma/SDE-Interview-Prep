---
id: leetcode-0191-number-of-1-bits
title: "LeetCode 0191: Number of 1 Bits"
tags:
  - dsa
  - leetcode
  - bit-manipulation
  - divide-and-conquer
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/number-of-1-bits/"
---

# LeetCode 0191: Number of 1 Bits

## 1. Problem Formalization and Constraints

Given a positive integer `n`, write a function that returns the number of set bits it has (also known as the Hamming weight).

### Constraints
- $1 \le n \le 2^{31} - 1$ (or 32-bit unsigned integer up to $2^{32} - 1$)

### Follow-up
- If this function is called many times, how would you optimize it?

### Examples
- **Example 1**:
  - Input: `n = 11` (binary representation `00000000000000000000000000001011`)
  - Output: `3`
  - Explanation: The input binary string has a total of three set bits.
- **Example 2**:
  - Input: `n = 128` (binary representation `00000000000000000000000010000000`)
  - Output: `1`
  - Explanation: The input binary string has a total of one set bit.
- **Example 3**:
  - Input: `n = 2147483645` (binary representation `01111111111111111111111111111101`)
  - Output: `30`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Brian Kernighan's Bit Trick | $O(k)$ | $O(1)$ | Clears the lowest set bit using `n &= n - 1` on each step; loop iterates exactly $k$ times where $k$ is the number of 1-bits ($k \le 32$). |
| **Tier 2 (Divide & Conquer)** | Parallel Popcount Mask Additions | $O(1)$ (5 additions) | $O(1)$ | Adds adjacent 1-bit, 2-bit, 4-bit, 8-bit, 16-bit counts in parallel using bitmasks in exactly 5 instructions with zero branches. |
| **Tier 3 (Bit-by-Bit Shift)** | 32-Iteration Linear Bit Shift | $O(32) = O(1)$ | $O(1)$ | Checks least significant bit via `count += n & 1`, shifts right logically `n >>>= 1` exactly 32 times; constant time but always takes 32 iterations. |
| **Tier 4 (Lookup Table)** | 8-Bit Byte Lookup Table (4 Lookups) | $O(1)$ (4 accesses) | $O(1)$ (256-entry array) | Decomposes 32-bit integer into 4 bytes; looks up precomputed popcounts from a 256-byte cache; ideal for amortized batch streams. |

*Notation*: $k$ is the Hamming weight (number of set 1-bits), $0 \le k \le 32$.

---

## 3. Tier 1: Most Optimal Solution (Brian Kernighan's Bit Trick)

### 3.1 Algorithmic Mechanics and Invariant Proof

The algorithm operates on the fundamental bitwise identity:
Subtracting 1 from a non-zero integer $n$ flips all bits from the lowest set bit downward:
$$n = (\dots 100\dots0)_2 \implies n - 1 = (\dots 011\dots1)_2$$
Performing a bitwise AND between $n$ and $n - 1$ clears the least significant set bit while leaving all higher bits unchanged:
$$n \ \& \ (n - 1) = (\dots 000\dots0)_2$$

**Algorithm**:
1. Initialize `count = 0`.
2. While `n != 0`:
   - Set `n = n & (n - 1)`.
   - Increment `count += 1`.
3. Return `count`.

**Invariant Proof**:
Let $k(n)$ denote the number of 1-bits in the binary representation of $n$.
Base case: If $n = 0$, $k(n) = 0$, the loop does not execute, and `count = 0` is returned.
Inductive step: Assume $n > 0$.
The binary representation of $n$ can be written as $n = a \cdot 2^{m+1} + 2^m$ where $2^m$ is the least significant set bit and $a \ge 0$.
Then $n - 1 = a \cdot 2^{m+1} + (2^m - 1) = a \cdot 2^{m+1} + \sum_{j=0}^{m-1} 2^j$.
Taking the bitwise AND:
$$n \ \& \ (n - 1) = \left(a \cdot 2^{m+1} + 2^m\right) \ \& \ \left(a \cdot 2^{m+1} + \sum_{j=0}^{m-1} 2^j\right) = a \cdot 2^{m+1}$$
The bit at position $m$ is set to 0, while all other bits remain identical.
Thus, $k(n \ \& \ (n - 1)) = k(n) - 1$.
Each iteration decreases the population count by exactly 1.
Because $n$ initially has $k$ set bits, the loop terminates after precisely $k$ iterations with $n = 0$ and `count == k`.
Correctness is mathematically proven.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(k)$ where $k \le 32$ is the number of set bits. In the best case ($n = 0$ or powers of two), takes 0 or 1 iterations; in the worst case (all ones), takes 32 iterations.
- **Auxiliary Space Complexity**: $O(1)$. Operates in-place using a single counter register.

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(k) where k is the number of 1-bits
// Space: O(1)

#include <cstdint>

class Solution {
public:
    int hammingWeight(uint32_t n) {
        int count = 0;
        while (n != 0) {
            n &= (n - 1);
            ++count;
        }
        return count;
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(k) where k is the number of 1-bits
# Space: O(1)

class Solution:
    def hammingWeight(self, n: int) -> int:
        count = 0
        while n:
            n &= n - 1
            count += 1
        return count
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(k) where k is the number of 1-bits
// Space: O(1)

class Solution {
    // treat n as an unsigned value
    public int hammingWeight(int n) {
        int count = 0;
        while (n != 0) {
            n &= (n - 1);
            count++;
        }
        return count;
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(k) where k is the number of 1-bits
// Space: O(1)

function hammingWeight(n: number): number {
    let count = 0;
    while (n !== 0) {
        n = (n & (n - 1)) >>> 0;
        count++;
    }
    return count;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(k) where k is the number of 1-bits
// Space: O(1)

package main

func hammingWeight(num uint32) int {
	count := 0
	for num != 0 {
		num &= (num - 1)
		count++
	}
	return count
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(k) where k is the number of 1-bits
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn hamming_weight(mut n: u32) -> i32 {
        let mut count = 0;
        while n != 0 {
            n &= n - 1;
            count += 1;
        }
        count
    }
}
```

---

## 4. Tier 2: Divide and Conquer Parallel Popcount

### 4.1 Mechanical Description
Sums the bits in parallel hierarchical tree steps using fixed bitmasks:
```cpp
n = (n & 0x55555555) + ((n >> 1) & 0x55555555); // sum 2-bit fields
n = (n & 0x33333333) + ((n >> 2) & 0x33333333); // sum 4-bit fields
n = (n & 0x0F0F0F0F) + ((n >> 4) & 0x0F0F0F0F); // sum 8-bit fields
n = (n & 0x00FF00FF) + ((n >> 8) & 0x00FF00FF); // sum 16-bit fields
n = (n & 0x0000FFFF) + ((n >> 16) & 0x0000FFFF); // sum 32-bit fields
```

### 4.2 Trade-offs
- Exactly 5 arithmetic steps with zero loops and zero branch mispredictions.
- More verbose code than Brian Kernighan's loop.

---

## 5. Tier 3: 32-Iteration Linear Bit Shift

### 5.1 Mechanical Description
Iterate a loop 32 times:
Accumulate `count += (n & 1)` and shift `n >>>= 1`.

### 5.2 Trade-offs
- Simple to reason about.
- Always runs all 32 iterations even if $n$ only has a single set bit (e.g. $n = 1$).

---

## 6. Tier 4: Byte-by-Byte Lookup Table (Amortized Stream Optimization)

### 6.1 Mechanical Description
Precompute a lookup table `table[256]` of 8-bit popcounts.
Decompose $n$ into four 8-bit bytes and sum their table entries:
$$\text{count} = \text{table}[n \ \& \ \text{0xFF}] + \text{table}[(n \gg 8) \ \& \ \text{0xFF}] + \text{table}[(n \gg 16) \ \& \ \text{0xFF}] + \text{table}[(n \gg 24) \ \& \ \text{0xFF}]$$

### 6.2 Trade-offs
- Extremely fast for repeated calls over massive data streams.
- Requires allocating a 256-byte static cache table.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Hardware Population Count Instruction**: Modern x86 processors provide the `POPCNT` instruction; GCC's `__builtin_popcount` and Rust's `u32::count_ones()` directly compile to this hardware assembly instruction.
2. **Bit Clearing Efficiency**: Brian Kernighan's trick `n & (n - 1)` compiles to a single x86 instruction (`BLSR` from BMI1).
3. **Branch Elimination**: In JavaScript/TypeScript, applying unsigned right shift `>>> 0` ensures 32-bit unsigned numeric semantics without floating-point conversion.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single set bit (power of two) | `n = 1024` | Returns `1` | Loops exactly once |
| All 32 bits set | `n = 0xFFFFFFFF` | Returns `32` | Loops 32 times, clearing each bit |
| Zero value | `n = 0` | Returns `0` | While loop condition `n != 0` skips |
| High bit set (negative signed value) | $n = 2^{31}$ | Returns `1` | Treated as unsigned 32-bit; clears bit 31 cleanly |
| Alternating bit pattern | `n = 0x55555555` | Returns `16` | Accurately counts 16 set bits |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What does the expression `n & (n - 1)` do?
It clears the lowest (least significant) 1-bit in $n$, leaving all other bits completely unchanged.

### 2. How can we check if a number is a power of two?
A positive integer $n$ is a power of two if and only if it has exactly one set bit: `n > 0 && (n & (n - 1)) == 0`.

### 3. What is Hamming weight?
The Hamming weight of a string or integer is the number of symbols that are different from zero (for binary numbers, the count of 1-bits).

### 4. How does Rust provide hardware popcount?
Rust's `u32::count_ones()` compiles directly to the machine's hardware popcount instruction.

### 5. Why is Brian Kernighan's algorithm preferred over checking all 32 bits?
Because its runtime is proportional to the number of set bits ($k$) rather than the bit width (32). For sparse numbers, it finishes in just 1 to 3 iterations.

### 6. What is the maximum number of set bits in a 32-bit integer?
32 bits.

### 7. Does this algorithm work for 64-bit integers?
Yes, using `uint64_t` or `u64`; `n &= n - 1` operates identically regardless of integer bit width.

### 8. How does the lookup table approach address the follow-up question?
If called billions of times, 4 table lookups in an L1-cached array of size 256 avoid branches and minimize CPU cycles.

### 9. Why does TypeScript need `>>> 0`?
In JavaScript/TypeScript, bitwise operations produce signed 32-bit integers; applying `>>> 0` coerces negative results into unsigned 32-bit values.

### 10. How does this relate to LeetCode 338 (Counting Bits)?
LeetCode 338 requires calculating the Hamming weight for all numbers from $0$ to $N$. Dynamic programming uses $P[i] = P[i \ \& \ (i - 1)] + 1$ to compute all counts in $O(N)$ time.

---

## 10. Related Problems and Systematic Progression Links

- [[0078-Subsets]]: Bitmask generation of subsets.
- [[0190-Reverse-Bits]]: Bitwise reversal of 32-bit unsigned integers.
- LeetCode 231 (Power of Two): Testing if an integer is a power of two via bit manipulation.
- LeetCode 338 (Counting Bits): DP generation of bit count array for $0 \dots N$.
- LeetCode 461 (Hamming Distance): XOR followed by bit count computation.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/number-of-1-bits.cpp)
- [Python Implementation](../Python/number-of-1-bits.py)
- [Java Implementation](../Java/number-of-1-bits.java)
- [TypeScript Implementation](../TypeScript/number-of-1-bits.ts)
- [Go Implementation](../Golang/number-of-1-bits.go)
- [Rust Implementation](../Rust/number-of-1-bits.rs)

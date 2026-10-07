---
id: leetcode-0190-reverse-bits
title: "LeetCode 0190: Reverse Bits"
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
  - "https://leetcode.com/problems/reverse-bits/"
---

# LeetCode 0190: Reverse Bits

## 1. Problem Formalization and Constraints

Reverse bits of a given 32 bits unsigned integer.

### Constraints
- The input must be a binary string of length 32.

### Follow-up
- If this function is called many times, how would you optimize it?

### Examples
- **Example 1**:
  - Input: `n = 00000010100101000001111010011100` (43261596 in base 10)
  - Output: `964176192` (`00111001011110000010100101000000` in base 2)
  - Explanation: The input binary string has been reversed.
- **Example 2**:
  - Input: `n = 11111111111111111111111111111101` (4294967293 in base 10)
  - Output: `3221225471` (`10111111111111111111111111111111` in base 2)

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Divide-and-Conquer Bitwise Masking | $O(1)$ (5 operations) | $O(1)$ | Swaps adjacent 16-bit, 8-bit, 4-bit, 2-bit, and 1-bit chunks in $\log_2(32) = 5$ parallel bitwise operations without loops. |
| **Tier 2 (Bit-by-Bit Shift)** | 32-Iteration Bit Shift Loop | $O(1)$ (32 iterations) | $O(1)$ | Extracts the least significant bit with `n & 1`, shifts into result `(result << 1) | bit`, and shifts `n >>> 1`; universally portable. |
| **Tier 3 (Byte Lookup Table)** | Byte-by-Byte Table Memoization | $O(1)$ (4 table lookups) | $O(1)$ (256-byte cache) | Precomputes reverse table for all $2^8 = 256$ byte values; decomposes 32-bit integer into 4 bytes; ideal for high-throughput calls. |
| **Tier 4 (Brute Force)** | Binary String Conversion | $O(1)$ (string allocation) | $O(1)$ (string heap) | Converts integer to 32-character binary string, reverses string, and parses back to integer; high memory and garbage collection overhead. |

---

## 3. Tier 1: Most Optimal Solution (Divide-and-Conquer Bitwise Masking)

### 3.1 Algorithmic Mechanics and Invariant Proof

Reversing 32 bits can be broken down using divide-and-conquer:
1. Swap the upper 16 bits with the lower 16 bits:
   $$n = (n \gg 16) \mid (n \ll 16)$$
2. Swap adjacent 8-bit blocks (bytes) using mask `0x00FF00FF`:
   $$n = ((n \ \& \ \text{0xFF00FF00}) \gg 8) \mid ((n \ \& \ \text{0x00FF00FF}) \ll 8)$$
3. Swap adjacent 4-bit blocks (nibbles) using mask `0x0F0F0F0F`:
   $$n = ((n \ \& \ \text{0xF0F0F0F0}) \gg 4) \mid ((n \ \& \ \text{0x0F0F0F0F}) \ll 4)$$
4. Swap adjacent 2-bit pairs using mask `0x33333333`:
   $$n = ((n \ \& \ \text{0xCCCCCCCC}) \gg 2) \mid ((n \ \& \ \text{0x33333333}) \ll 2)$$
5. Swap adjacent 1-bit pairs using mask `0x55555555`:
   $$n = ((n \ \& \ \text{0xAAAAAAAA}) \gg 1) \mid ((n \ \& \ \text{0x55555555}) \ll 1)$$

**Invariant Proof**:
Let the 32 bits be indexed from 0 to 31.
Reversing the bits requires mapping index $i$ to index $31 - i$.
In binary representation, $31 - i = \sim i \pmod{32}$.
Negating a 5-bit binary number flips each of its 5 constituent bits independently:
$$\text{bit } 4 \ (\text{weight } 16), \quad \text{bit } 3 \ (\text{weight } 8), \quad \text{bit } 2 \ (\text{weight } 4), \quad \text{bit } 1 \ (\text{weight } 2), \quad \text{bit } 0 \ (\text{weight } 1)$$
Step 1 flips bit 4 of all indices (swapping halves of size 16).
Step 2 flips bit 3 of all indices (swapping quarters of size 8).
Step 3 flips bit 2 of all indices (swapping eighths of size 4).
Step 4 flips bit 1 of all indices (swapping pairs of size 2).
Step 5 flips bit 0 of all indices (swapping individual bits).
Because each bit of the index is inverted exactly once, every bit $i$ is moved to $31 - i$.
The entire operation requires exactly 5 constant-time parallel instructions with zero branching and zero loops.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(1)$. Exactly 5 parallel shift-mask-or operations, executing in under 5 CPU instruction cycles.
- **Auxiliary Space Complexity**: $O(1)$. Zero additional heap or stack memory.

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1) (5 bitwise operations)
// Space: O(1)

#include <cstdint>

class Solution {
public:
    uint32_t reverseBits(uint32_t n) {
        n = (n >> 16) | (n << 16);
        n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        return n;
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(1)
# Space: O(1)

class Solution:
    def reverseBits(self, n: int) -> int:
        n = ((n >> 16) | (n << 16)) & 0xffffffff
        n = (((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8)) & 0xffffffff
        n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4)) & 0xffffffff
        n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2)) & 0xffffffff
        n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1)) & 0xffffffff
        return n
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

class Solution {
    // treat n as an unsigned value
    public int reverseBits(int n) {
        n = (n >>> 16) | (n << 16);
        n = ((n & 0xff00ff00) >>> 8) | ((n & 0x00ff00ff) << 8);
        n = ((n & 0xf0f0f0f0) >>> 4) | ((n & 0x0f0f0f0f) << 4);
        n = ((n & 0xcccccccc) >>> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xaaaaaaaa) >>> 1) | ((n & 0x55555555) << 1);
        return n;
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

function reverseBits(n: number): number {
    let result = 0;
    for (let i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1);
        n >>>= 1;
    }
    return result >>> 0;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

package main

func reverseBits(num uint32) uint32 {
	num = (num >> 16) | (num << 16)
	num = ((num & 0xff00ff00) >> 8) | ((num & 0x00ff00ff) << 8)
	num = ((num & 0xf0f0f0f0) >> 4) | ((num & 0x0f0f0f0f) << 4)
	num = ((num & 0xcccccccc) >> 2) | ((num & 0x33333333) << 2)
	num = ((num & 0xaaaaaaaa) >> 1) | ((num & 0x55555555) << 1)
	return num
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn reverse_bits(mut x: u32) -> u32 {
        let mut result = 0;
        for _ in 0..32 {
            result = (result << 1) | (x & 1);
            x >>= 1;
        }
        result
    }
}
```

---

## 4. Tier 2: 32-Iteration Bit Shift Loop

### 4.1 Mechanical Description
Initialize `result = 0`.
Loop exactly 32 times:
- Shift `result` left by 1 bit: `result <<= 1`.
- Append the lowest bit of `n`: `result |= (n & 1)`.
- Shift `n` right logically by 1 bit: `n >>>= 1`.
Return `result`.

### 4.2 Trade-offs
- Universal across all architectures and simple to understand.
- Requires 32 loop iterations and branch checks, executing slower than the 5-step divide-and-conquer approach.

---

## 5. Tier 3: Byte-by-Byte Table Memoization (Follow-Up Optimization)

### 5.1 Mechanical Description
Precompute a lookup table `table[256]` of 8-bit reversed values.
Decompose the 32-bit integer into 4 bytes:
$$\text{byte}_0 = n \ \& \ \text{0xFF}, \quad \text{byte}_1 = (n \gg 8) \ \& \ \text{0xFF}, \quad \text{byte}_2 = (n \gg 16) \ \& \ \text{0xFF}, \quad \text{byte}_3 = (n \gg 24) \ \& \ \text{0xFF}$$
Reconstruct the reversed 32-bit integer:
$$\text{reversed} = (\text{table}[\text{byte}_0] \ll 24) \mid (\text{table}[\text{byte}_1] \ll 16) \mid (\text{table}[\text{byte}_2] \ll 8) \mid \text{table}[\text{byte}_3]$$

### 5.2 Trade-offs
- Precomputation table consumes only 256 bytes of memory, easily fitting in the L1 data cache.
- Ideal when reversing billions of integers in high-throughput network or graphics applications.

---

## 6. Tier 4: Binary String Conversion (Brute Force Baseline)

### 6.1 Mechanical Description
Format the integer as a 32-character binary string, pad with leading zeros, reverse the string, and parse as an unsigned 32-bit integer.

### 6.2 Trade-offs
- Highly inefficient due to string allocations, ASCII character conversions, and garbage collector overhead.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Instruction-Level Parallelism**: Modern superscalar processors execute the independent shift and mask operations of Tier 1 across multiple ALU execution ports in parallel.
2. **Hardware Intrinsic Support**: Many CPU architectures provide native bit reversal instructions (such as ARM `RBIT`); Rust's `u32::reverse_bits()` maps directly to this hardware instruction when compiled with target CPU extensions.
3. **Logical vs Arithmetic Shifts**: In Java and JavaScript, bitwise operators operate on signed 32-bit integers; using unsigned right shift `>>>` is required to prevent sign extension.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| All zeros | `n = 0` | Returns `0` | All bit swaps yield 0 |
| All ones | `n = 0xFFFFFFFF` | Returns `0xFFFFFFFF` | All bit swaps yield 0xFFFFFFFF |
| Single high bit set | `n = 1 << 31` | Returns `1` | Bit moves from position 31 to position 0 |
| Alternating bit pattern | `n = 0xAAAAAAAA` | Returns `0x55555555` | Even/odd bit swap reverses pattern |
| Sign bit overflow in JS | Large values with bit 31 set | Interpreted as unsigned 32-bit | `result >>> 0` coerces to unsigned |

---

## 9. Frequently Asked Questions (FAQ)

### 1. How does `n >>> 1` differ from `n >> 1` in Java and JavaScript?
`>>` is an arithmetic right shift that preserves the sign bit by shifting in 1s if the number is negative. `>>>` is a logical right shift that always shifts in 0s, which is required for unsigned numbers.

### 2. Why does the divide-and-conquer mask approach work in 5 steps?
Because a 32-bit integer has $2^5 = 32$ bits, each step swaps power-of-two subdivisions ($16, 8, 4, 2, 1$).

### 3. How does the lookup table follow-up improve performance?
Instead of 32 iterations or 5 multi-step bit operations, 4 byte lookups in an L1-cached 256-element array produce the answer in 4 memory loads.

### 4. What is the value of `0x55555555` in binary?
`0x5` is `0101`, so `0x55555555` is an alternating bitmask that selects all odd-indexed bits.

### 5. What is the value of `0xAAAAAAAA` in binary?
`0xA` is `1010`, selecting all even-indexed bits.

### 6. Can Rust's standard library reverse bits directly?
Yes, `u32::reverse_bits()` is a built-in standard method that compiles down to hardware assembly.

### 7. Why must Python apply `& 0xffffffff` after bit operations?
Python integers have arbitrary precision and do not overflow at 32 bits; bitwise left-shifts can grow beyond 32 bits without manual truncation.

### 8. Does reversing bits alter endianness?
No. Reversing bits reverses the bit order within the number. Byte swapping (`htonl` / `ntohl`) changes endianness without reversing individual bits within each byte.

### 9. What is the difference between reversing bits and reversing bytes?
Byte reversal swaps 8-bit groups (e.g. byte 0 with byte 3). Bit reversal reverses each of the 32 individual bits.

### 10. Can this algorithm be extended to 64-bit integers?
Yes, simply prepend one more step swapping halves of size 32 ($2^6 = 64$ bits takes 6 steps).

---

## 10. Related Problems and Systematic Progression Links

- [[0078-Subsets]]: Bitmask generation of power sets.
- [[0198-House-Robber]]: Linear state optimization.
- LeetCode 7 (Reverse Integer): Decimal base-10 digit reversal with overflow detection.
- LeetCode 191 (Number of 1 Bits): Hamming weight bit counting.
- LeetCode 338 (Counting Bits): Dynamic programming bit popcount array construction.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/reverse-bits.cpp)
- [Python Implementation](../Python/reverse-bits.py)
- [Java Implementation](../Java/reverse-bits.java)
- [TypeScript Implementation](../TypeScript/reverse-bits.ts)
- [Go Implementation](../Golang/reverse-bits.go)
- [Rust Implementation](../Rust/reverse-bits.rs)

---
id: leetcode-0091-decode-ways
title: "LeetCode 0091: Decode Ways"
tags:
  - dsa
  - leetcode
  - string
  - dynamic-programming
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/decode-ways/"
---

# LeetCode 0091: Decode Ways

## 1. Problem Formalization and Constraints

A message containing letters from A-Z can be encoded into numbers using the following mapping:
'A' -> "1", 'B' -> "2", ..., 'Z' -> "26".
To decode an encoded message, all the digits must be grouped then mapped back into letters using the reverse of the mapping above.
There may be multiple ways to decode because digits can be taken individually or in pairs.
For example, "11106" can be mapped into:
- "AAJF" with the grouping (1, 1, 10, 6)
- "KJF" with the grouping (11, 10, 6)
Note that the grouping (1, 11, 06) is invalid because "06" cannot be mapped into 'F' since "6" is different from "06".
Given a string `s` containing only digits, return the number of ways to decode it.
The test cases are generated so that the answer fits in a 32-bit integer.

### Constraints
- $1 \le \text{s.length} \le 100$
- `s` consists of digits and may contain leading zero(s).

### Examples
- **Example 1**:
  - Input: `s = "12"`
  - Output: `2`
  - Explanation: "12" could be decoded as "AB" (1 2) or "L" (12).
- **Example 2**:
  - Input: `s = "226"`
  - Output: `3`
  - Explanation: "226" could be decoded as "BZ" (2 26), "VF" (22 6), or "BBF" (2 2 6).
- **Example 3**:
  - Input: `s = "06"`
  - Output: `0`
  - Explanation: "06" cannot be mapped to "F" because of the leading zero ("6" is different from "06").

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Constant-Space Rolling DP | $O(N)$ | $O(1)$ auxiliary | Tracks `prev1` (1 step back) and `prev2` (2 steps back) with strict validity checks. |
| **Tier 2 (1D DP Array)** | Full 1D Tabulation Array | $O(N)$ | $O(N)$ auxiliary | Array `dp[i]` represents ways to decode prefix `s[0..i-1]`. |
| **Tier 3 (Memoized DFS)** | Top-Down Recursion with Memoization | $O(N)$ | $O(N)$ auxiliary | Explores branching choices recursively, caching answers by index. |
| **Tier 4 (Brute Force)** | Pure Unmemoized Recursion | $O(2^N)$ | $O(N)$ auxiliary | Recursive tree with branching factor up to 2 without caching, causing TLE. |

---

## 3. Tier 1: Most Optimal Solution (Constant-Space Rolling DP)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $s[0 \dots N-1]$ be the input string.
If $s[0] == \text{'0'}$, the string cannot be decoded; return 0 immediately.
Define:
- `prev2`: number of ways to decode the prefix ending at index $i - 2$. Initialized to 1 (representing the empty string base case).
- `prev1`: number of ways to decode the prefix ending at index $i - 1$. Initialized to 1 (representing $s[0]$).

For each index $i \in [1, N-1]$:
1. Initialize `current = 0`.
2. Single-digit transition: If $s[i] \ne \text{'0'}$, then $s[i]$ maps to a valid character ('1'-'9'). Add `prev1` to `current`.
3. Two-digit transition: Consider the two-digit number formed by $s[i-1]$ and $s[i]$. If $10 \le \text{two\_digit} \le 26$, add `prev2` to `current`.
4. Update rolling variables: `prev2 = prev1`, `prev1 = current`.
5. If at any point both transitions are 0, `current` becomes 0 (and future decodings may also collapse).
Return `prev1`.

**Invariant Proof**:
Let $D(k)$ denote the number of valid decodings for the prefix $s[0 \dots k]$.
A prefix $s[0 \dots k]$ can only be formed by:
- Appending a single valid digit $s[k] \in ['1', '9']$ to a valid decoding of $s[0 \dots k-1]$.
- Appending a valid two-digit sequence $s[k-1 \dots k] \in ["10", "26"]$ to a valid decoding of $s[0 \dots k-2]$.
Since any valid decoding must end in either a 1-character token or a 2-character token, and no token can have length $> 2$ or length $< 1$, these two cases partition all valid decodings of $s[0 \dots k]$.
By induction, `current` computes $D(k) = [s[k] \ne '0'] \cdot D(k-1) + [10 \le s[k-1..k] \le 26] \cdot D(k-2)$.
Because $D(k)$ only references $D(k-1)$ and $D(k-2)$, two scalar registers are sufficient, proving correctness in $O(1)$ space.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Performs a single pass of $N - 1$ iterations with constant time arithmetic and string character inspections.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space utilizing two integer state variables.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>

class Solution {
public:
    int numDecodings(std::string s) {
        if (s.empty() || s[0] == '0') {
            return 0;
        }

        int prev2 = 1;
        int prev1 = 1;

        for (size_t i = 1; i < s.length(); ++i) {
            int current = 0;

            if (s[i] != '0') {
                current += prev1;
            }

            int two_digit = (s[i - 1] - '0') * 10 + (s[i] - '0');
            if (two_digit >= 10 && two_digit <= 26) {
                current += prev2;
            }

            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }
};
```

#### Python 3
```python
class Solution:
    def numDecodings(self, s: str) -> int:
        if not s or s[0] == "0":
            return 0

        prev2, prev1 = 1, 1

        for i in range(1, len(s)):
            current = 0

            if s[i] != "0":
                current += prev1

            two_digit = int(s[i - 1 : i + 1])
            if 10 <= two_digit <= 26:
                current += prev2

            prev2, prev1 = prev1, current

        return prev1
```

#### Java 21
```java
class Solution {
    public int numDecodings(String s) {
        if (s == null || s.isEmpty() || s.charAt(0) == '0') {
            return 0;
        }

        int prev2 = 1;
        int prev1 = 1;

        for (int i = 1; i < s.length(); i++) {
            int current = 0;

            if (s.charAt(i) != '0') {
                current += prev1;
            }

            int twoDigit = (s.charAt(i - 1) - '0') * 10 + (s.charAt(i) - '0');
            if (twoDigit >= 10 && twoDigit <= 26) {
                current += prev2;
            }

            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }
}
```

#### TypeScript
```typescript
function numDecodings(s: string): number {
    if (!s || s[0] === '0') {
        return 0;
    }

    let prev2 = 1;
    let prev1 = 1;

    for (let i = 1; i < s.length; i++) {
        let current = 0;

        if (s[i] !== '0') {
            current += prev1;
        }

        const twoDigit = Number(s.substring(i - 1, i + 1));
        if (twoDigit >= 10 && twoDigit <= 26) {
            current += prev2;
        }

        prev2 = prev1;
        prev1 = current;
    }

    return prev1;
}
```

#### Go
```go
package main

func numDecodings(s string) int {
	if len(s) == 0 || s[0] == '0' {
		return 0
	}

	prev2 := 1
	prev1 := 1

	for i := 1; i < len(s); i++ {
		current := 0

		if s[i] != '0' {
			current += prev1
		}

		twoDigit := int(s[i-1]-'0')*10 + int(s[i]-'0')
		if twoDigit >= 10 && twoDigit <= 26 {
			current += prev2
		}

		prev2 = prev1
		prev1 = current
	}

	return prev1
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn num_decodings(s: String) -> i32 {
        let bytes = s.as_bytes();
        if bytes.is_empty() || bytes[0] == b'0' {
            return 0;
        }

        let mut prev2 = 1i32;
        let mut prev1 = 1i32;

        for i in 1..bytes.len() {
            let mut current = 0i32;

            if bytes[i] != b'0' {
                current += prev1;
            }

            let two_digit = (bytes[i - 1] - b'0') * 10 + (bytes[i] - b'0');
            if (10..=26).contains(&two_digit) {
                current += prev2;
            }

            prev2 = prev1;
            prev1 = current;
        }

        prev1
    }
}
```

---

## 4. Tier 2: 1D Dynamic Programming Tabulation Array

### 4.1 Mechanical Description
Create an array `dp` of size $N + 1$.
`dp[0] = 1` and `dp[1] = 1` (if $s[0] \ne '0'$).
For $i$ from 2 to $N$:
If $s[i-1] \ne '0'$, `dp[i] += dp[i-1]`.
If $10 \le \text{int}(s[i-2..i-1]) \le 26$, `dp[i] += dp[i-2]`.
Return `dp[N]`.

```python
def numDecodingsDP(s: str) -> int:
    if not s or s[0] == "0":
        return 0
    n = len(s)
    dp = [0] * (n + 1)
    dp[0] = 1
    dp[1] = 1
    for i in range(2, n + 1):
        one = int(s[i - 1 : i])
        two = int(s[i - 2 : i])
        if 1 <= one <= 9:
            dp[i] += dp[i - 1]
        if 10 <= two <= 26:
            dp[i] += dp[i - 2]
    return dp[n]
```

### 4.2 Trade-offs
- Provides a clean index alignment where `dp[i]` represents prefix length $i$.
- Requires $O(N)$ heap allocation instead of $O(1)$ scalar variables.

---

## 5. Tier 3: Top-Down Recursion with Memoization

### 5.1 Mechanical Description
Define `dfs(index)` returning ways to decode suffix $s[\text{index}\dots N-1]$.
Base cases:
If $\text{index} == N$, return 1.
If $s[\text{index}] == '0'$, return 0.
Branch into single digit decode `dfs(index + 1)` and two digit decode `dfs(index + 2)` if within 10 to 26.
Store answers in `memo[index]`.

### 5.2 Trade-offs
- Intuitive translation from the problem definition.
- Consumes $O(N)$ recursion call stack frames and requires memoization map lookups.

---

## 6. Tier 4: Unmemoized Recursion (Brute Force Baseline)

### 6.1 Mechanical Description
Evaluate the recursion without caching results.
Fibonacci-style overlapping subproblems cause the recursion tree to explode exponentially ($O(2^N)$), timing out on inputs of length $\ge 35$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Byte Inspection**: In C++ and Rust, inspecting characters via byte indexing (`bytes[i] - b'0'`) avoids substring slicing and dynamic memory allocation.
2. **Short-Circuit Evaluation**: Using arithmetic comparisons rather than string conversion minimizes CPU cycles and memory allocations.
3. **Register Storage**: The two rolling variables fit directly into CPU registers (`RDX`, `RCX`), producing zero memory roundtrips.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Leading zero | `s = "06"` | Returns `0` | Immediate check `if (s[0] == '0') return 0;`. |
| Trailing lone zero | `s = "100"` | Returns `0` | At index 2, '0' cannot be single, and "00" is invalid; returns 0. |
| Valid embedded zeroes | `s = "10"` or `s = "20"` | Returns `1` | Single digit branch adds 0; two-digit branch adds `prev2`. |
| Invalid two-digit zero | `s = "30"` or `s = "40"` | Returns `0` | Neither single digit nor two-digit matches; `current` stays 0. |
| Maximum valid encoding | `s = "26"` | Returns `2` | Decodes as "B F" or "Z". |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does '0' require special handling?
There is no letter mapped to '0'. '0' can only exist as the second digit of "10" or "20".

### 2. Why is `prev2` initialized to 1 instead of 0?
An empty prefix represents one valid state before characters are consumed, ensuring valid two-digit prefixes like "12" count both options correctly.

### 3. What happens when consecutive zeroes like "100" appear?
At the second zero, single-digit is 0, and "00" is outside $[10, 26]$. Thus `current = 0`, causing future counts to become 0.

### 4. How does this compare to Climbing Stairs (LeetCode 70)?
Climbing Stairs always allows taking 1 or 2 steps unconditionally. Decode Ways is Climbing Stairs with conditional step validity.

### 5. Why is substring parsing slower than byte arithmetic?
`s.substring()` allocates a new string object on the heap in many languages, whereas `(s[i-1]-'0')*10 + (s[i]-'0')` performs two arithmetic operations in registers.

### 6. Can the answer exceed standard integer limits?
The problem constraints guarantee the answer fits within a standard 32-bit signed integer.

### 7. How does Decode Ways II (LeetCode 639) complicate this?
LeetCode 639 introduces wildcard character '*' which can represent any digit 1-9, expanding the transition conditions significantly.

### 8. Does the direction of iteration (left-to-right vs right-to-left) matter?
Both work symmetrically. Left-to-right aligns naturally with prefix-based stream processing.

### 9. Why does Rust use `as_bytes()`?
Rust strings are UTF-8 validated. `as_bytes()` allows $O(1)$ random indexing since the input is guaranteed to consist only of ASCII digits.

### 10. Could leading zeroes exist in the middle of a string?
Yes, in sequences like "106", the '0' is part of "10", leaving "6" to be processed as a valid single digit.

---

## 10. Related Problems and Systematic Progression Links

- [[0070-Climbing-Stairs]]: Fundamental 1D Fibonacci dynamic programming.
- [[0139-Word-Break]]: Partitioning strings using a dictionary.
- LeetCode 639 (Decode Ways II): Decoding strings with wildcard character '*'.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/decode-ways.cpp)
- [Python Implementation](../Python/decode-ways.py)
- [Java Implementation](../Java/decode-ways.java)
- [TypeScript Implementation](../TypeScript/decode-ways.ts)
- [Go Implementation](../Golang/decode-ways.go)
- [Rust Implementation](../Rust/decode-ways.rs)

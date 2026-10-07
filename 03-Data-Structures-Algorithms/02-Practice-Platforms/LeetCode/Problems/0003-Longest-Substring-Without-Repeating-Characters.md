---
id: leetcode-0003-longest-substring-without-repeating-characters
title: "LeetCode 0003: Longest Substring Without Repeating Characters"
tags:
  - dsa
  - leetcode
  - string
  - sliding-window
  - hash-table
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/longest-substring-without-repeating-characters/"
---

# LeetCode 0003: Longest Substring Without Repeating Characters

## 1. Problem Formalization and Constraints

Given a string `s`, find the length of the longest substring without repeating characters.
A substring is a contiguous non-empty sequence of characters within a string.

### Constraints
- $0 \le \text{s.length} \le 5 \times 10^4$
- `s` consists of English letters, digits, symbols, and spaces.

### Examples
- **Example 1**:
  - Input: `s = "abcabcbb"`
  - Output: `3`
  - Explanation: The answer is `"abc"`, with the length of 3.
- **Example 2**:
  - Input: `s = "bbbbb"`
  - Output: `1`
  - Explanation: The answer is `"b"`, with the length of 1.
- **Example 3**:
  - Input: `s = "pwwkew"`
  - Output: `3`
  - Explanation: The answer is `"wke"`, with the length of 3.
    Notice that the answer must be a substring; `"pwke"` is a subsequence and not a substring.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Sliding Window with Last Seen Index Lookup | $O(N)$ | $O(\min(N, \Sigma))$ | Jumps the left window boundary directly past the conflicting character's previous index. |
| **Tier 2 (Space-Optimized Alternative)** | Fixed 128/256-Element ASCII Direct Lookup Array | $O(N)$ | $O(1)$ fixed | Replaces dynamic hash tables with a fixed-size stack/static buffer. |
| **Tier 3 (Time-Optimized Alternative)** | Sliding Window with Hash Set Incremental Eviction | $O(N)$ | $O(\min(N, \Sigma))$ | Evicts characters one by one from the left boundary until the collision is resolved. |
| **Tier 4 (Brute Force)** | Exhaustive Substring Uniqueness Validation | $O(N^2)$ | $O(\Sigma)$ | Enumerates all substrings and validates character set uniqueness. |

---

## 3. Tier 1: Most Optimal Solution (Sliding Window with Index Jump)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let `s[0 ... N-1]` be the input string.
We maintain a sliding window $[L, R]$ where all characters within the window are strictly distinct.
We record the most recent index where each character appeared in a hash map `lastSeen`.

As the right boundary $R$ advances from $0$ to $N - 1$:
1. If $s[R]$ was previously observed at index $j$ within the active window ($j \ge L$):
   Advance the left boundary directly to $j + 1$:
   $$L = \max(L, \text{lastSeen}[s[R]] + 1)$$
2. Record or update $\text{lastSeen}[s[R]] = R$.
3. Update $\text{maxLen} = \max(\text{maxLen}, R - L + 1)$.

**Inductive Invariant**:
At every step $R$, the substring $s[L \dots R]$ contains no duplicate characters, and $L$ is the minimal valid left boundary for window ending at $R$.
Because $L$ only moves forward and each character is processed once, the runtime is strictly $O(N)$ with at most $N$ lookups.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly $N$ steps with $O(1)$ table operations per step.
- **Space Complexity**: $O(\min(N, \Sigma))$ where $\Sigma$ is the alphabet size (at most 128 for ASCII or 256 for extended ASCII).

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    int lengthOfLongestSubstring(const std::string& s) {
        std::vector<int> lastSeen(256, -1);
        int left = 0;
        int maxLen = 0;
        for (int right = 0; right < static_cast<int>(s.size()); ++right) {
            unsigned char c = s[right];
            if (lastSeen[c] >= left) {
                left = lastSeen[c] + 1;
            }
            lastSeen[c] = right;
            maxLen = std::max(maxLen, right - left + 1);
        }
        return maxLen;
    }
};
```

#### Python 3.12
```python
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        last_seen = {}
        left = 0
        max_len = 0
        for right, c in enumerate(s):
            if c in last_seen and last_seen[c] >= left:
                left = last_seen[c] + 1
            last_seen[c] = right
            max_len = max(max_len, right - left + 1)
        return max_len
```

#### Java 21
```java
import java.util.HashMap;
import java.util.Map;

class Solution {
    public int lengthOfLongestSubstring(String s) {
        Map<Character, Integer> lookup = new HashMap<>();
        int left = 0;
        int maxLen = 0;
        for (int right = 0; right < s.length(); right++) {
            char c = s.charAt(right);
            if (lookup.containsKey(c)) {
                left = Math.max(left, lookup.get(c) + 1);
            }
            lookup.put(c, right);
            maxLen = Math.max(maxLen, right - left + 1);
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function lengthOfLongestSubstring(s: string): number {
    const lookup = new Map<string, number>();
    let left = 0;
    let maxLen = 0;
    for (let right = 0; right < s.length; right++) {
        const c = s[right];
        if (lookup.has(c)) {
            left = Math.max(left, lookup.get(c)! + 1);
        }
        lookup.set(c, right);
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

#### Go
```go
package main

func lengthOfLongestSubstring(s string) int {
    lastSeen := make(map[byte]int)
    left := 0
    maxLen := 0
    for right := 0; right < len(s); right++ {
        c := s[right]
        if prevIdx, exists := lastSeen[c]; exists && prevIdx >= left {
            left = prevIdx + 1
        }
        lastSeen[c] = right
        if right-left+1 > maxLen {
            maxLen = right - left + 1
        }
    }
    return maxLen
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let mut lookup = HashMap::new();
        let mut left = 0;
        let mut max_len = 0;
        for (right, c) in s.chars().enumerate() {
            if let Some(&prev_idx) = lookup.get(&c) {
                left = left.max(prev_idx + 1);
            }
            lookup.insert(c, right);
            max_len = max_len.max(right - left + 1);
        }
        max_len as i32
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Fixed 128-Element ASCII Array)

### 4.1 Algorithmic Mechanics

Because input characters fall within standard ASCII limits, a fixed array of 128 or 256 integers eliminates all dynamic heap hash table allocations.
We initialize the array with $-1$.
Accessing `lastSeen[c]` executes in a single memory load with zero hashing overhead.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear scan.
- **Space Complexity**: $O(1)$ fixed 128/256-word static array.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <array>
#include <algorithm>

class Solution {
public:
    int lengthOfLongestSubstring(const std::string& s) {
        std::array<int, 128> lastSeen;
        lastSeen.fill(-1);
        int left = 0, maxLen = 0;
        for (int right = 0; right < static_cast<int>(s.size()); ++right) {
            unsigned char c = s[right];
            if (lastSeen[c] >= left) {
                left = lastSeen[c] + 1;
            }
            lastSeen[c] = right;
            maxLen = std::max(maxLen, right - left + 1);
        }
        return maxLen;
    }
};
```

#### Python 3.12
```python
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        last_seen = [-1] * 128
        left = 0
        max_len = 0
        for right, c in enumerate(s):
            code = ord(c)
            if code < 128 and last_seen[code] >= left:
                left = last_seen[code] + 1
            if code < 128:
                last_seen[code] = right
            max_len = max(max_len, right - left + 1)
        return max_len
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int lengthOfLongestSubstring(String s) {
        int[] lastSeen = new int[128];
        Arrays.fill(lastSeen, -1);
        int left = 0;
        int maxLen = 0;
        for (int right = 0; right < s.length(); right++) {
            char c = s.charAt(right);
            if (lastSeen[c] >= left) {
                left = lastSeen[c] + 1;
            }
            lastSeen[c] = right;
            maxLen = Math.max(maxLen, right - left + 1);
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function lengthOfLongestSubstring(s: string): number {
    const lastSeen = new Int32Array(128).fill(-1);
    let left = 0;
    let maxLen = 0;
    for (let right = 0; right < s.length; right++) {
        const c = s.charCodeAt(right);
        if (c < 128 && lastSeen[c] >= left) {
            left = lastSeen[c] + 1;
        }
        if (c < 128) {
            lastSeen[c] = right;
        }
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

#### Go
```go
package main

func lengthOfLongestSubstring(s string) int {
    var lastSeen [128]int
    for i := range lastSeen {
        lastSeen[i] = -1
    }
    left := 0
    maxLen := 0
    for right := 0; right < len(s); right++ {
        c := s[right]
        if c < 128 && lastSeen[c] >= left {
            left = lastSeen[c] + 1
        }
        if c < 128 {
            lastSeen[c] = right
        }
        if right-left+1 > maxLen {
            maxLen = right - left + 1
        }
    }
    return maxLen
}
```

#### Rust
```rust
impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let mut last_seen = [-1i32; 128];
        let mut left = 0;
        let mut max_len = 0;
        for (right, c) in s.bytes().enumerate() {
            let idx = c as usize;
            if idx < 128 && last_seen[idx] >= left {
                left = last_seen[idx] + 1;
            }
            if idx < 128 {
                last_seen[idx] = right as i32;
            }
            max_len = max_len.max(right as i32 - left + 1);
        }
        max_len
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Sliding Window with Hash Set Eviction)

### 5.1 Algorithmic Mechanics

Instead of jumping directly to the collision index, we use a hash set containing all active window elements.
When $s[R]$ is already in the set, an inner while loop incrementally removes $s[L]$ from the set and advances $L = L + 1$ until $s[R]$ is no longer present.
Then $s[R]$ is added to the set.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(2N) = O(N)$. Both $L$ and $R$ traverse the string at most once.
- **Space Complexity**: $O(\min(N, \Sigma))$ auxiliary set storage.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <unordered_set>
#include <algorithm>

class Solution {
public:
    int lengthOfLongestSubstring(const std::string& s) {
        std::unordered_set<char> window;
        int left = 0, maxLen = 0;
        for (int right = 0; right < static_cast<int>(s.size()); ++right) {
            while (window.count(s[right])) {
                window.erase(s[left++]);
            }
            window.insert(s[right]);
            maxLen = std::max(maxLen, right - left + 1);
        }
        return maxLen;
    }
};
```

#### Python 3.12
```python
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        window = set()
        left = 0
        max_len = 0
        for right, c in enumerate(s):
            while c in window:
                window.remove(s[left])
                left += 1
            window.add(c)
            max_len = max(max_len, right - left + 1)
        return max_len
```

#### Java 21
```java
import java.util.HashSet;
import java.util.Set;

class Solution {
    public int lengthOfLongestSubstring(String s) {
        Set<Character> window = new HashSet<>();
        int left = 0;
        int maxLen = 0;
        for (int right = 0; right < s.length(); right++) {
            char c = s.charAt(right);
            while (window.contains(c)) {
                window.remove(s.charAt(left++));
            }
            window.add(c);
            maxLen = Math.max(maxLen, right - left + 1);
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function lengthOfLongestSubstring(s: string): number {
    const window = new Set<string>();
    let left = 0;
    let maxLen = 0;
    for (let right = 0; right < s.length; right++) {
        const c = s[right];
        while (window.has(c)) {
            window.delete(s[left++]);
        }
        window.add(c);
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

#### Go
```go
package main

func lengthOfLongestSubstring(s string) int {
    window := make(map[byte]bool)
    left := 0
    maxLen := 0
    for right := 0; right < len(s); right++ {
        for window[s[right]] {
            delete(window, s[left])
            left++
        }
        window[s[right]] = true
        if right-left+1 > maxLen {
            maxLen = right - left + 1
        }
    }
    return maxLen
}
```

#### Rust
```rust
use std::collections::HashSet;

impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let mut window = HashSet::new();
        let bytes = s.as_bytes();
        let mut left = 0;
        let mut max_len = 0;
        for (right, &b) in bytes.iter().enumerate() {
            while window.contains(&b) {
                window.remove(&bytes[left]);
                left += 1;
            }
            window.insert(b);
            max_len = max_len.max(right - left + 1);
        }
        max_len as i32
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Exhaustive Substring Enumeration)

### 6.1 Algorithmic Mechanics

We test every pair of start and end indices $(i, j)$ with $0 \le i \le j < N$.
For each substring, a nested loop or set verifies whether all characters in $s[i \dots j]$ are unique.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$ checks using incremental character set accumulation.
- **Space Complexity**: $O(\Sigma)$ set storage.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    int lengthOfLongestSubstring(const std::string& s) {
        int n = s.size();
        int maxLen = 0;
        for (int i = 0; i < n; ++i) {
            std::vector<bool> seen(256, false);
            for (int j = i; j < n; ++j) {
                unsigned char c = s[j];
                if (seen[c]) break;
                seen[c] = true;
                maxLen = std::max(maxLen, j - i + 1);
            }
        }
        return maxLen;
    }
};
```

#### Python 3.12
```python
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        n = len(s)
        max_len = 0
        for i in range(n):
            seen = set()
            for j in range(i, n):
                if s[j] in seen:
                    break
                seen.add(s[j])
                max_len = max(max_len, j - i + 1)
        return max_len
```

#### Java 21
```java
class Solution {
    public int lengthOfLongestSubstring(String s) {
        int n = s.length();
        int maxLen = 0;
        for (int i = 0; i < n; i++) {
            boolean[] seen = new boolean[128];
            for (int j = i; j < n; j++) {
                char c = s.charAt(j);
                if (seen[c]) break;
                seen[c] = true;
                maxLen = Math.max(maxLen, j - i + 1);
            }
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function lengthOfLongestSubstring(s: string): number {
    const n = s.length;
    let maxLen = 0;
    for (let i = 0; i < n; i++) {
        const seen = new Uint8Array(128);
        for (let j = i; j < n; j++) {
            const code = s.charCodeAt(j);
            if (code < 128 && seen[code]) break;
            if (code < 128) seen[code] = 1;
            maxLen = Math.max(maxLen, j - i + 1);
        }
    }
    return maxLen;
}
```

#### Go
```go
package main

func lengthOfLongestSubstring(s string) int {
    n := len(s)
    maxLen := 0
    for i := 0; i < n; i++ {
        seen := make([]bool, 128)
        for j := i; j < n; j++ {
            c := s[j]
            if c < 128 && seen[c] {
                break
            }
            if c < 128 {
                seen[c] = true
            }
            if j-i+1 > maxLen {
                maxLen = j - i + 1
            }
        }
    }
    return maxLen
}
```

#### Rust
```rust
impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let bytes = s.as_bytes();
        let n = bytes.len();
        let mut max_len = 0;
        for i in 0..n {
            let mut seen = [false; 128];
            for j in i..n {
                let b = bytes[j] as usize;
                if b < 128 && seen[b] {
                    break;
                }
                if b < 128 {
                    seen[b] = true;
                }
                max_len = max_len.max((j - i + 1) as i32);
            }
        }
        max_len
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is `left = Math.max(left, lookup.get(c) + 1)` necessary instead of `left = lookup.get(c) + 1`?</summary>
If character $c$ was seen before the current left boundary ($lastSeen[c] < left$), unconditionally assigning `lookup.get(c) + 1` would move the left boundary backward.
Moving backward would re-admit characters that have already been excluded, violating the window's uniqueness invariant.
The `Math.max` guard ensures the window boundary only monotonically moves forward.
</details>

<details>
<summary>2. Why does the direct index jump in Tier 1 outperform the incremental set eviction in Tier 3?</summary>
In Tier 3, resolving a conflict with a character near the front of a long window requires the left pointer to increment through and delete many intermediate characters.
In Tier 1, a single array or map lookup jumps the left pointer directly past the collision in $O(1)$ operations, eliminating all intermediate deletions.
</details>

<details>
<summary>3. What is the cache impact of using a fixed 128-byte array vs a hash table?</summary>
A 128-byte array occupies exactly two 64-byte CPU cache lines and fits easily inside the L1 data cache.
Hash tables involve dynamic heap memory allocation, pointer dereferencing, and potential hash collision chains, which frequently miss cache.
The direct array implementation runs $3\times$ to $5\times$ faster in microbenchmarks.
</details>

<details>
<summary>4. How does Unicode / UTF-8 multibyte encoding affect this problem?</summary>
If the string contains non-ASCII UTF-8 multibyte runes (such as emojis or accented characters), a simple 128-byte array can overflow.
In languages like Go and Rust, one must decode runes using `s.chars()` or UTF-8 decoders and utilize a dynamic hash map or expanded 65,536-entry array.
</details>

<details>
<summary>5. How does this sliding window pattern generalize to At Most $K$ Distinct Characters?</summary>
Instead of ensuring every character is unique, we maintain a frequency map of characters in the window.
When the number of keys in the frequency map exceeds $K$, we contract the left boundary until `map.size() <= K`.
</details>

<details>
<summary>6. How can SIMD instructions be used for substring uniqueness validation?</summary>
For short chunks (up to 16 or 32 bytes), SIMD conflict detection instructions (`_mm_conflict_epi32` in AVX-512) can test all pairwise duplicates across vector lanes simultaneously in a single CPU cycle.
</details>

<details>
<summary>7. What is the difference between substring and subsequence in competitive programming?</summary>
A substring is strictly contiguous ($\text{s}[i \dots j]$).
A subsequence can be formed by deleting zero or more characters from anywhere in the string without changing the relative order of remaining characters.
Substrings are solved via sliding windows; subsequences typically require dynamic programming.
</details>

<details>
<summary>8. In Rust, why is `s.as_bytes()` preferred over `s.chars()` when characters are ASCII?</summary>
`s.as_bytes()` produces a slice of raw bytes `&[u8]` with zero UTF-8 validation and direct indexability.
`s.chars()` creates an iterator that decodes variable-length UTF-8 code points, which adds instruction overhead if characters are known to be ASCII.
</details>

<details>
<summary>9. What is the upper bound on the maximum return value?</summary>
The maximum return value is bounded by $\min(N, \Sigma)$.
For standard ASCII characters, $\Sigma = 128$.
Even if $N = 50000$, the answer can never exceed 128 if the alphabet is ASCII.
</details>

<details>
<summary>10. What are the key unit test edge cases for this problem?</summary>
1. Empty string `""` (returns 0).
2. Single-character string `"a"` (returns 1).
3. All identical characters `"aaaa"` (returns 1).
4. All distinct characters `"abcdef"` (returns length of string).
5. Repeats at boundaries: `"abba"` (tests the `max(left, prev + 1)` backward jump guard).
6. Strings containing spaces and symbols: `"a b!c a"`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/longest-substring-without-repeating-characters.cpp)
- [Python Implementation](../Python/longest-substring-without-repeating-characters.py)
- [Java Implementation](../Java/longest-substring-without-repeating-characters.java)
- [TypeScript Implementation](../TypeScript/longest-substring-without-repeating-characters.ts)
- [Go Implementation](../Golang/longest-substring-without-repeating-characters.go)
- [Rust Implementation](../Rust/longest-substring-without-repeating-characters.rs)

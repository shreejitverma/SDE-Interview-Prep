---
id: leetcode-0424-longest-repeating-character-replacement
title: "LeetCode 0424: Longest Repeating Character Replacement"
tags:
  - dsa
  - leetcode
  - sliding-window
  - string
  - hash-table
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/longest-repeating-character-replacement/"
---

# LeetCode 0424: Longest Repeating Character Replacement

## 1. Problem Formalization and Constraints

You are given a string `s` and an integer `k`.
You can choose any character of the string and change it to any other uppercase English character.
You can perform this operation at most `k` times.
Return the length of the longest substring containing the same letter you can get after performing the above operations.

### Constraints
- $1 \le \text{s.length} \le 10^5$
- `s` consists of only uppercase English letters.
- $0 \le k \le \text{s.length}$

### Examples
- **Example 1**:
  - Input: `s = "ABAB", k = 2`
  - Output: `4`
  - Explanation: Replace the two 'A's with two 'B's or vice versa.
- **Example 2**:
  - Input: `s = "AABABBA", k = 1`
  - Output: `4`
  - Explanation: Replace the one 'B' in the middle with 'A' to form "AAAABBA", where substring "AAAA" has length 4.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Non-Shrinking Sliding Window | $O(N)$ | $O(1)$ | Window size only increases or shifts forward; avoids recomputing max frequency after left increment. |
| **Tier 2 (Space-Optimized)** | Standard Shrinking Sliding Window | $O(N)$ | $O(1)$ | Explicitly shrinks left boundary when replacements exceed $k$; uses fixed 26-element integer table. |
| **Tier 3 (Time-Optimized Alternative)** | Sliding Window with Full Frequency Scan | $O(26 \times N)$ | $O(1)$ | Re-evaluates max frequency across all 26 character counters on every window adjustment. |
| **Tier 4 (Brute Force)** | Exhaustive Substring Enumeration | $O(N^2)$ | $O(1)$ | Iterates over all possible substrings $(i, j)$ and tests feasibility against replacement limit $k$. |

---

## 3. Tier 1: Most Optimal Solution (Non-Shrinking Sliding Window)

### 3.1 Algorithmic Mechanics and Invariant Proof

A substring is valid if the number of characters that need to be replaced is at most $k$:
$$\text{Window Length} - \text{Max Frequency Character Count} \le k$$
As the pointer `right` moves forward from $0$ to $N - 1$:
1. Increment the frequency of `s[right]` and update `max_count = max(max_count, count[s[right]])`.
2. Check if the window is invalid: `(right - left + 1) - max_count > k`.
3. If invalid, decrement `count[s[left]]` and increment `left` by 1.
Notice that we do not shrink the window further.
Because we only care about finding a window strictly larger than the current maximum, the window size never shrinks.
It either expands by 1 (when valid) or shifts forward by 1 (when invalid).
At the end of iteration, the maximum valid window size is simply `N - left`.

**Invariant Proof**:
`max_count` only increases when a character within the current window achieves a frequency strictly greater than all previous observations.
If a smaller valid window appears later, it cannot beat our historical maximum window size.
Therefore, not decrementing `max_count` when shifting `left` preserves the invariant that the window size equals the maximum valid length discovered.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each character is visited at most twice (once by `right`, once by `left`).
- **Space Complexity**: $O(1)$. Fixed table of 26 uppercase English letters.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    int characterReplacement(std::string s, int k) {
        int count[26] = {0};
        int max_count = 0;
        int left = 0;

        for (int right = 0; right < static_cast<int>(s.length()); ++right) {
            max_count = std::max(max_count, ++count[s[right] - 'A']);
            if ((right - left + 1) - max_count > k) {
                --count[s[left] - 'A'];
                ++left;
            }
        }
        return static_cast<int>(s.length()) - left;
    }
};
```

#### Python 3.12
```python
class Solution:
    def characterReplacement(self, s: str, k: int) -> int:
        count = [0] * 26
        max_count = 0
        left = 0

        for right, ch in enumerate(s):
            idx = ord(ch) - 65
            count[idx] += 1
            max_count = max(max_count, count[idx])
            if (right - left + 1) - max_count > k:
                count[ord(s[left]) - 65] -= 1
                left += 1

        return len(s) - left
```

#### Java 21
```java
class Solution {
    public int characterReplacement(String s, int k) {
        int[] count = new int[26];
        int maxCount = 0;
        int left = 0;
        int n = s.length();

        for (int right = 0; right < n; right++) {
            int idx = s.charAt(right) - 'A';
            maxCount = Math.max(maxCount, ++count[idx]);
            if ((right - left + 1) - maxCount > k) {
                count[s.charAt(left) - 'A']--;
                left++;
            }
        }
        return n - left;
    }
}
```

#### TypeScript
```typescript
function characterReplacement(s: string, k: number): number {
    const count = new Int32Array(26);
    let maxCount = 0;
    let left = 0;
    const n = s.length;

    for (let right = 0; right < n; right++) {
        const idx = s.charCodeAt(right) - 65;
        count[idx]++;
        maxCount = Math.max(maxCount, count[idx]);
        if ((right - left + 1) - maxCount > k) {
            count[s.charCodeAt(left) - 65]--;
            left++;
        }
    }
    return n - left;
}
```

#### Go
```go
package main

func characterReplacement(s: string, k: int) int {
    count := [26]int{}
    maxCount := 0
    left := 0
    n := len(s)

    for right := 0; right < n; right++ {
        idx := s[right] - 'A'
        count[idx]++
        if count[idx] > maxCount {
            maxCount = count[idx]
        }
        if (right - left + 1) - maxCount > k {
            count[s[left]-'A']--
            left++
        }
    }
    return n - left
}
```

#### Rust
```rust
impl Solution {
    pub fn character_replacement(s: String, k: i32) -> i32 {
        let bytes = s.as_bytes();
        let mut count = [0i32; 26];
        let mut max_count = 0;
        let mut left = 0;
        let n = bytes.len();

        for right in 0..n {
            let idx = (bytes[right] - b'A') as usize;
            count[idx] += 1;
            max_count = max_count.max(count[idx]);
            if (right - left + 1) as i32 - max_count > k {
                count[(bytes[left] - b'A') as usize] -= 1;
                left += 1;
            }
        }
        (n - left) as i32
    }
}
```

---

## 4. Tier 2: Space-Optimized Solution (Standard Shrinking Sliding Window)

### 4.1 Algorithmic Mechanics and Invariant Proof

In this variation, whenever the current window becomes invalid, a `while` loop advances `left` until the condition `(right - left + 1) - max_count <= k` is restored.
At each step, we update `max_len = max(max_len, right - left + 1)`.
This makes the window strictly conform to the valid state at the end of each iteration.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$. `right` and `left` advance at most $N$ times.
- **Space Complexity**: $O(1)$. Fixed 26-element array.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    int characterReplacement(std::string s, int k) {
        int count[26] = {0};
        int max_count = 0;
        int left = 0;
        int max_len = 0;

        for (int right = 0; right < static_cast<int>(s.length()); ++right) {
            max_count = std::max(max_count, ++count[s[right] - 'A']);
            while ((right - left + 1) - max_count > k) {
                --count[s[left] - 'A'];
                ++left;
            }
            max_len = std::max(max_len, right - left + 1);
        }
        return max_len;
    }
};
```

#### Python 3.12
```python
class Solution:
    def characterReplacement(self, s: str, k: int) -> int:
        count = [0] * 26
        max_count = 0
        left = 0
        max_len = 0

        for right, ch in enumerate(s):
            idx = ord(ch) - 65
            count[idx] += 1
            max_count = max(max_count, count[idx])
            while (right - left + 1) - max_count > k:
                count[ord(s[left]) - 65] -= 1
                left += 1
            max_len = max(max_len, right - left + 1)

        return max_len
```

#### Java 21
```java
class Solution {
    public int characterReplacement(String s, int k) {
        int[] count = new int[26];
        int maxCount = 0;
        int left = 0;
        int maxLen = 0;

        for (int right = 0; right < s.length(); right++) {
            int idx = s.charAt(right) - 'A';
            maxCount = Math.max(maxCount, ++count[idx]);
            while ((right - left + 1) - maxCount > k) {
                count[s.charAt(left) - 'A']--;
                left++;
            }
            maxLen = Math.max(maxLen, right - left + 1);
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function characterReplacement(s: string, k: number): number {
    const count = new Int32Array(26);
    let maxCount = 0;
    let left = 0;
    let maxLen = 0;

    for (let right = 0; right < s.length; right++) {
        const idx = s.charCodeAt(right) - 65;
        count[idx]++;
        maxCount = Math.max(maxCount, count[idx]);
        while ((right - left + 1) - maxCount > k) {
            count[s.charCodeAt(left) - 65]--;
            left++;
        }
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

#### Go
```go
package main

func characterReplacement(s: string, k: int) int {
    count := [26]int{}
    maxCount := 0
    left := 0
    maxLen := 0

    for right := 0; right < len(s); right++ {
        idx := s[right] - 'A'
        count[idx]++
        if count[idx] > maxCount {
            maxCount = count[idx]
        }
        for (right - left + 1) - maxCount > k {
            count[s[left]-'A']--
            left++
        }
        if (right - left + 1) > maxLen {
            maxLen = right - left + 1
        }
    }
    return maxLen
}
```

#### Rust
```rust
impl Solution {
    pub fn character_replacement(s: String, k: i32) -> i32 {
        let bytes = s.as_bytes();
        let mut count = [0i32; 26];
        let mut max_count = 0;
        let mut left = 0;
        let mut max_len = 0;

        for right in 0..bytes.len() {
            let idx = (bytes[right] - b'A') as usize;
            count[idx] += 1;
            max_count = max_count.max(count[idx]);
            while (right - left + 1) as i32 - max_count > k {
                count[(bytes[left] - b'A') as usize] -= 1;
                left += 1;
            }
            max_len = max_len.max((right - left + 1) as i32);
        }
        max_len
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Sliding Window with Full Frequency Scan)

### 5.1 Algorithmic Mechanics and Invariant Proof

Instead of relying on the cached historical `max_count`, this variant recomputes the true maximum frequency across all 26 counters whenever `left` is incremented.
This ensures `max_count` always reflects the exact character frequency of the current active window.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(26 \times N)$. On each window shrink, a loop scans the 26 alphabet counters.
- **Space Complexity**: $O(1)$. Fixed 26-element array.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    int characterReplacement(std::string s, int k) {
        int count[26] = {0};
        int left = 0;
        int max_len = 0;

        for (int right = 0; right < static_cast<int>(s.length()); ++right) {
            ++count[s[right] - 'A'];
            int max_count = 0;
            for (int c = 0; c < 26; ++c) {
                max_count = std::max(max_count, count[c]);
            }
            while ((right - left + 1) - max_count > k) {
                --count[s[left] - 'A'];
                ++left;
                max_count = 0;
                for (int c = 0; c < 26; ++c) {
                    max_count = std::max(max_count, count[c]);
                }
            }
            max_len = std::max(max_len, right - left + 1);
        }
        return max_len;
    }
};
```

#### Python 3.12
```python
class Solution:
    def characterReplacement(self, s: str, k: int) -> int:
        count = [0] * 26
        left = 0
        max_len = 0

        for right, ch in enumerate(s):
            count[ord(ch) - 65] += 1
            max_count = max(count)
            while (right - left + 1) - max_count > k:
                count[ord(s[left]) - 65] -= 1
                left += 1
                max_count = max(count)
            max_len = max(max_len, right - left + 1)

        return max_len
```

#### Java 21
```java
class Solution {
    public int characterReplacement(String s, int k) {
        int[] count = new int[26];
        int left = 0;
        int maxLen = 0;

        for (int right = 0; right < s.length(); right++) {
            count[s.charAt(right) - 'A']++;
            int maxCount = getMax(count);
            while ((right - left + 1) - maxCount > k) {
                count[s.charAt(left) - 'A']--;
                left++;
                maxCount = getMax(count);
            }
            maxLen = Math.max(maxLen, right - left + 1);
        }
        return maxLen;
    }

    private int getMax(int[] count) {
        int m = 0;
        for (int c : count) {
            if (c > m) m = c;
        }
        return m;
    }
}
```

#### TypeScript
```typescript
function characterReplacement(s: string, k: number): number {
    const count = new Int32Array(26);
    let left = 0;
    let maxLen = 0;

    for (let right = 0; right < s.length; right++) {
        count[s.charCodeAt(right) - 65]++;
        let maxCount = 0;
        for (let c = 0; c < 26; c++) {
            if (count[c] > maxCount) maxCount = count[c];
        }
        while ((right - left + 1) - maxCount > k) {
            count[s.charCodeAt(left) - 65]--;
            left++;
            maxCount = 0;
            for (let c = 0; c < 26; c++) {
                if (count[c] > maxCount) maxCount = count[c];
            }
        }
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

#### Go
```go
package main

func characterReplacement(s: string, k: int) int {
    count := [26]int{}
    left := 0
    maxLen := 0

    getMax := func() int {
        m := 0
        for _, v := range count {
            if v > m {
                m = v
            }
        }
        return m
    }

    for right := 0; right < len(s); right++ {
        count[s[right]-'A']++
        maxCount := getMax()
        for (right - left + 1) - maxCount > k {
            count[s[left]-'A']--
            left++
            maxCount = getMax()
        }
        if (right - left + 1) > maxLen {
            maxLen = right - left + 1
        }
    }
    return maxLen
}
```

#### Rust
```rust
impl Solution {
    pub fn character_replacement(s: String, k: i32) -> i32 {
        let bytes = s.as_bytes();
        let mut count = [0i32; 26];
        let mut left = 0;
        let mut max_len = 0;

        let get_max = |c: &[i32; 26]| -> i32 {
            *c.iter().max().unwrap_or(&0)
        };

        for right in 0..bytes.len() {
            count[(bytes[right] - b'A') as usize] += 1;
            let mut max_count = get_max(&count);
            while (right - left + 1) as i32 - max_count > k {
                count[(bytes[left] - b'A') as usize] -= 1;
                left += 1;
                max_count = get_max(&count);
            }
            max_len = max_len.max((right - left + 1) as i32);
        }
        max_len
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Exhaustive Substring Enumeration)

### 6.1 Algorithmic Mechanics and Invariant Proof

The brute force algorithm considers all possible starting indices $i$ and ending indices $j$.
For each substring `s[i .. j]`, it counts the frequencies of all characters, determines the majority character count, and tests whether `(j - i + 1) - max_freq <= k`.
It tracks the maximum length across all valid substrings.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Generating all $O(N^2)$ substrings and incrementally tracking character counts.
- **Space Complexity**: $O(1)$. Fixed 26-element array for each start index.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    int characterReplacement(std::string s, int k) {
        int n = static_cast<int>(s.length());
        int max_len = 0;

        for (int i = 0; i < n; ++i) {
            int count[26] = {0};
            int max_freq = 0;
            for (int j = i; j < n; ++j) {
                max_freq = std::max(max_freq, ++count[s[j] - 'A']);
                int len = j - i + 1;
                if (len - max_freq <= k) {
                    max_len = std::max(max_len, len);
                }
            }
        }
        return max_len;
    }
};
```

#### Python 3.12
```python
class Solution:
    def characterReplacement(self, s: str, k: int) -> int:
        n = len(s)
        max_len = 0

        for i in range(n):
            count = [0] * 26
            max_freq = 0
            for j in range(i, n):
                idx = ord(s[j]) - 65
                count[idx] += 1
                max_freq = max(max_freq, count[idx])
                length = j - i + 1
                if length - max_freq <= k:
                    max_len = max(max_len, length)

        return max_len
```

#### Java 21
```java
class Solution {
    public int characterReplacement(String s, int k) {
        int n = s.length();
        int maxLen = 0;

        for (int i = 0; i < n; i++) {
            int[] count = new int[26];
            int maxFreq = 0;
            for (int j = i; j < n; j++) {
                int idx = s.charAt(j) - 'A';
                count[idx]++;
                maxFreq = Math.max(maxFreq, count[idx]);
                int len = j - i + 1;
                if (len - maxFreq <= k) {
                    maxLen = Math.max(maxLen, len);
                }
            }
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function characterReplacement(s: string, k: number): number {
    const n = s.length;
    let maxLen = 0;

    for (let i = 0; i < n; i++) {
        const count = new Int32Array(26);
        let maxFreq = 0;
        for (let j = i; j < n; j++) {
            const idx = s.charCodeAt(j) - 65;
            count[idx]++;
            if (count[idx] > maxFreq) maxFreq = count[idx];
            const len = j - i + 1;
            if (len - maxFreq <= k) {
                if (len > maxLen) maxLen = len;
            }
        }
    }
    return maxLen;
}
```

#### Go
```go
package main

func characterReplacement(s: string, k: int) int {
    n := len(s)
    maxLen := 0

    for i := 0; i < n; i++ {
        count := [26]int{}
        maxFreq := 0
        for j := i; j < n; j++ {
            idx := s[j] - 'A'
            count[idx]++
            if count[idx] > maxFreq {
                maxFreq = count[idx]
            }
            length := j - i + 1
            if length - maxFreq <= k {
                if length > maxLen {
                    maxLen = length
                }
            }
        }
    }
    return maxLen
}
```

#### Rust
```rust
impl Solution {
    pub fn character_replacement(s: String, k: i32) -> i32 {
        let bytes = s.as_bytes();
        let n = bytes.len();
        let mut max_len = 0;

        for i in 0..n {
            let mut count = [0i32; 26];
            let mut max_freq = 0;
            for j in i..n {
                let idx = (bytes[j] - b'A') as usize;
                count[idx] += 1;
                max_freq = max_freq.max(count[idx]);
                let len = (j - i + 1) as i32;
                if len - max_freq <= k {
                    max_len = max_len.max(len);
                }
            }
        }
        max_len
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is it mathematically safe not to decrement `max_count` when shifting `left` in the optimal solution?</summary>
We only care about finding a window strictly larger than the current maximum window size.
To achieve a larger window, the newly encountered character must produce a `max_count` strictly greater than our current historical `max_count`.
Any smaller or stale `max_count` cannot produce a larger valid window, so not updating `max_count` downward never produces false positives.
</details>

<details>
<summary>2. What is the condition that determines if a substring can be made homogeneous with at most $k$ operations?</summary>
The condition is $(\text{length} - \text{frequency of most frequent character}) \le k$.
This formula computes the count of all characters that are not the majority character, which is the exact number of replacements required.
</details>

<details>
<summary>3. What happens if $k \ge \text{s.length}$?</summary>
If $k$ is greater than or equal to the string length, every character can be converted to match any single character.
The answer is immediately `s.length`.
</details>

<details>
<summary>4. What is the time complexity difference between the non-shrinking window and the recomputing window?</summary>
The non-shrinking window runs in strict $O(N)$ because each step takes $O(1)$ operations without scanning counters.
Recomputing the maximum frequency scans all 26 counters upon every window shrink, running in $O(26 \times N)$ operations.
</details>

<details>
<summary>5. Can this algorithm be extended to arbitrary Unicode strings?</summary>
Yes, by replacing the fixed 26-element array with a hash table (`std::unordered_map` or `HashMap`).
The space complexity increases to $O(\min(N, \Sigma))$ where $\Sigma$ is the alphabet size.
</details>

<details>
<summary>6. Why does the non-shrinking window return `s.length() - left` directly at the end?</summary>
Because the window never shrinks, once a window of size $W$ is established, `right - left + 1` remains at least $W$.
At the final iteration, `right = n - 1`, making the final window size $(n - 1) - left + 1 = n - left$.
</details>

<details>
<summary>7. What edge case occurs if the input string consists of only a single distinct character (e.g. `"AAAA"`)?</summary>
`max_count` equals the window length at every step.
`length - max_count` remains 0 throughout the loop, so `left` is never incremented and the function returns $N$.
</details>

<details>
<summary>8. How does compiler auto-vectorization benefit the fixed 26-element array scan?</summary>
Because 26 is a small fixed constant that fits inside a single 256-bit SIMD register (e.g. AVX2), the compiler can vectorize max reduction into a few SIMD instructions.
</details>

<details>
<summary>9. What is the behavior when $k = 0$?</summary>
When $k = 0$, no characters can be replaced.
The problem simplifies to finding the longest contiguous substring consisting of identical characters.
</details>

<details>
<summary>10. Why is dynamic programming typically not used for this problem?</summary>
Sliding window solves the problem in $O(N)$ time and $O(1)$ auxiliary space.
A dynamic programming formulation would require 2D or 3D state tracking $(i, j, k)$, requiring at least $O(N \times k)$ space and time, which is strictly inferior.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/longest-repeating-character-replacement.cpp)
- [Python Implementation](../Python/longest-repeating-character-replacement.py)
- [Java Implementation](../Java/longest-repeating-character-replacement.java)
- [TypeScript Implementation](../TypeScript/longest-repeating-character-replacement.ts)
- [Go Implementation](../Golang/longest-repeating-character-replacement.go)
- [Rust Implementation](../Rust/longest-repeating-character-replacement.rs)

---
id: leetcode-0242-valid-anagram
title: "LeetCode 0242: Valid Anagram"
tags:
  - dsa
  - leetcode
  - string
  - hash-table
  - sorting
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/valid-anagram/"
---

# LeetCode 0242: Valid Anagram

## 1. Problem Formalization and Constraints

Given two strings `s` and `t`, return `true` if `t` is an anagram of `s`, and `false` otherwise.
An anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

### Constraints
- $1 \le \text{s.length}, \text{t.length} \le 5 \times 10^4$
- `s` and `t` consist of lowercase English letters.

### Follow-up
What if the inputs contain Unicode characters? How would you adapt your solution to such a case?

### Examples
- **Example 1**:
  - Input: `s = "anagram", t = "nagaram"`
  - Output: `true`
- **Example 2**:
  - Input: `s = "rat", t = "car"`
  - Output: `false`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Fixed 26-Element Frequency Balance Vector | $O(N)$ | $O(1)$ (26 words) | Increments on `s`, decrements on `t` simultaneously; verifies zero-sum balance. |
| **Tier 2 (Space-Optimized Alternative)** | In-Place In-Order Character Sorting | $O(N \log N)$ | $O(1)$ extra | Sorts character buffers in-place, reducing equivalence to direct string comparison. |
| **Tier 3 (Time-Optimized Alternative)** | Universal Dynamic Hash Map Frequency Counter | $O(N)$ | $O(U)$ | Handles arbitrary Unicode / multibyte codepoints via hash tables. |
| **Tier 4 (Brute Force)** | Linear Search with Character Deletion | $O(N^2)$ | $O(N)$ | Searches for each character of `s` in `t` and splices it out. |

---

## 3. Tier 1: Most Optimal Solution (Fixed Frequency Balance Vector)

### 3.1 Algorithmic Mechanics and Invariant Proof

1. **Length Invariant**:
   If $\text{length}(s) \ne \text{length}(t)$, return `false` immediately because an anagram must contain the exact same number of letters.
2. **Frequency Balancing**:
   We initialize an array `counts` of size 26 with zeros.
   In a single loop from $0$ to $N - 1$:
   - Increment `counts[s[i] - 'a']++`.
   - Decrement `counts[t[i] - 'a']--`.
3. **Equivalence Validation**:
   If every element in `counts` is exactly $0$, $s$ and $t$ have identical character multisets, proving they are anagrams.

**Multiset Preservation Invariant**:
Two strings of equal length over alphabet $\Sigma$ are anagrams if and only if their Parikh vectors $(\#_c(s))_{c \in \Sigma}$ and $(\#_c(t))_{c \in \Sigma}$ are identical.
The difference vector $\Delta = \#_c(s) - \#_c(t)$ equals zero for all $c \in \Sigma$ if and only if $s$ is an anagram of $t$.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly $N$ loop iterations followed by 26 constant operations.
- **Space Complexity**: $O(1)$. Auxiliary memory is fixed at exactly 26 integer words.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>

class Solution {
public:
    bool isAnagram(const std::string& s, const std::string& t) {
        if (s.size() != t.size()) return false;
        int counts[26] = {0};
        for (size_t i = 0; i < s.size(); ++i) {
            ++counts[s[i] - 'a'];
            --counts[t[i] - 'a'];
        }
        for (int c : counts) {
            if (c != 0) return false;
        }
        return true;
    }
};
```

#### Python 3.12
```python
class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        if len(s) != len(t):
            return False
        counts = [0] * 26
        for sc, tc in zip(s, t):
            counts[ord(sc) - 97] += 1
            counts[ord(tc) - 97] -= 1
        return all(c == 0 for c in counts)
```

#### Java 21
```java
class Solution {
    public boolean isAnagram(String s, String t) {
        if (s.length() != t.length()) {
            return false;
        }
        int[] counts = new int[26];
        for (int i = 0; i < s.length(); i++) {
            counts[s.charAt(i) - 'a']++;
            counts[t.charAt(i) - 'a']--;
        }
        for (int c : counts) {
            if (c != 0) {
                return false;
            }
        }
        return true;
    }
}
```

#### TypeScript
```typescript
function isAnagram(s: string, t: string): boolean {
    if (s.length !== t.length) {
        return false;
    }
    const counts = new Int32Array(26);
    for (let i = 0; i < s.length; i++) {
        counts[s.charCodeAt(i) - 97]++;
        counts[t.charCodeAt(i) - 97]--;
    }
    for (let i = 0; i < 26; i++) {
        if (counts[i] !== 0) {
            return false;
        }
    }
    return true;
}
```

#### Go
```go
package main

func isAnagram(s string, t string) bool {
    if len(s) != len(t) {
        return false
    }
    var counts [26]int
    for i := 0; i < len(s); i++ {
        counts[s[i]-'a']++
        counts[t[i]-'a']--
    }
    for _, c := range counts {
        if c != 0 {
            return false
        }
    }
    return true
}
```

#### Rust
```rust
impl Solution {
    pub fn is_anagram(s: String, t: String) -> bool {
        if s.len() != t.len() {
            return false;
        }
        let mut counts = [0i32; 26];
        for (sb, tb) in s.bytes().zip(t.bytes()) {
            counts[(sb - b'a') as usize] += 1;
            counts[(tb - b'a') as usize] -= 1;
        }
        counts.iter().all(|&c| c == 0)
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (In-Place Character Sorting)

### 4.1 Algorithmic Mechanics

We sort the character sequences of both strings alphabetically.
If $s$ and $t$ are anagrams, their sorted representations are identical ($s_{\text{sorted}} == t_{\text{sorted}}$).
In languages where string buffers can be sorted in-place, this requires zero auxiliary memory.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$ sorting time.
- **Space Complexity**: $O(1)$ extra space if in-place, or $O(N)$ for immutable string copies.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <algorithm>

class Solution {
public:
    bool isAnagram(std::string s, std::string t) {
        if (s.size() != t.size()) return false;
        std::sort(s.begin(), s.end());
        std::sort(t.begin(), t.end());
        return s == t;
    }
};
```

#### Python 3.12
```python
class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        return len(s) == len(t) and sorted(s) == sorted(t)
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public boolean isAnagram(String s, String t) {
        if (s.length() != t.length()) return false;
        char[] sArr = s.toCharArray();
        char[] tArr = t.toCharArray();
        Arrays.sort(sArr);
        Arrays.sort(tArr);
        return Arrays.equals(sArr, tArr);
    }
}
```

#### TypeScript
```typescript
function isAnagram(s: string, t: string): boolean {
    if (s.length !== t.length) return false;
    return s.split("").sort().join("") === t.split("").sort().join("");
}
```

#### Go
```go
package main

import "sort"

func isAnagram(s string, t string) bool {
    if len(s) != len(t) {
        return false
    }
    sBytes := []byte(s)
    tBytes := []byte(t)
    sort.Slice(sBytes, func(i, j int) bool { return sBytes[i] < sBytes[j] })
    sort.Slice(tBytes, func(i, j int) bool { return tBytes[i] < tBytes[j] })
    return string(sBytes) == string(tBytes)
}
```

#### Rust
```rust
impl Solution {
    pub fn is_anagram(s: String, t: String) -> bool {
        if s.len() != t.len() {
            return false;
        }
        let mut s_bytes = s.into_bytes();
        let mut t_bytes = t.into_bytes();
        s_bytes.sort_unstable();
        t_bytes.sort_unstable();
        s_bytes == t_bytes
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Universal Hash Map for Unicode)

### 5.1 Algorithmic Mechanics

To support the follow-up question where inputs contain arbitrary Unicode codepoints (such as Chinese, Cyrillic, or emoji glyphs), a fixed 26-element array is insufficient.
We use a dynamic hash table `std::unordered_map<char32_t, int>` or `collections.Counter`.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ average time.
- **Space Complexity**: $O(U)$ where $U \le N$ is the number of distinct Unicode codepoints.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <unordered_map>

class Solution {
public:
    bool isAnagram(const std::string& s, const std::string& t) {
        if (s.size() != t.size()) return false;
        std::unordered_map<char, int> counts;
        for (char c : s) ++counts[c];
        for (char c : t) {
            auto it = counts.find(c);
            if (it == counts.end() || --it->second < 0) {
                return false;
            }
        }
        return true;
    }
};
```

#### Python 3.12
```python
from collections import Counter

class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        return len(s) == len(t) and Counter(s) == Counter(t)
```

#### Java 21
```java
import java.util.HashMap;
import java.util.Map;

class Solution {
    public boolean isAnagram(String s, String t) {
        if (s.length() != t.length()) return false;
        Map<Integer, Integer> counts = new HashMap<>();
        s.codePoints().forEach(cp -> counts.put(cp, counts.getOrDefault(cp, 0) + 1));
        for (int cp : (Iterable<Integer>) t.codePoints()::iterator) {
            int count = counts.getOrDefault(cp, 0);
            if (count == 0) return false;
            counts.put(cp, count - 1);
        }
        return true;
    }
}
```

#### TypeScript
```typescript
function isAnagram(s: string, t: string): boolean {
    if (s.length !== t.length) return false;
    const counts = new Map<string, number>();
    for (const c of s) {
        counts.set(c, (counts.get(c) ?? 0) + 1);
    }
    for (const c of t) {
        const count = counts.get(c) ?? 0;
        if (count === 0) return false;
        counts.set(c, count - 1);
    }
    return true;
}
```

#### Go
```go
package main

func isAnagram(s string, t string) bool {
    if len(s) != len(t) {
        return false
    }
    counts := make(map[rune]int)
    for _, r := range s {
        counts[r]++
    }
    for _, r := range t {
        counts[r]--
        if counts[r] < 0 {
            return false
        }
    }
    return true
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn is_anagram(s: String, t: String) -> bool {
        if s.chars().count() != t.chars().count() {
            return false;
        }
        let mut counts = HashMap::new();
        for c in s.chars() {
            *counts.entry(c).or_insert(0) += 1;
        }
        for c in t.chars() {
            let entry = counts.entry(c).or_insert(0);
            *entry -= 1;
            if *entry < 0 {
                return false;
            }
        }
        true
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Linear Search and Character Deletion)

### 6.1 Algorithmic Mechanics

For every character $c$ in $s$, find the first occurrence of $c$ in $t$ and delete it.
If $c$ is not found in $t$, return `false`.
If all characters are successfully matched and deleted, return `true`.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$ due to linear searches and string splices.
- **Space Complexity**: $O(N)$ for mutable copies of $t$.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>

class Solution {
public:
    bool isAnagram(const std::string& s, std::string t) {
        if (s.size() != t.size()) return false;
        for (char c : s) {
            size_t pos = t.find(c);
            if (pos == std::string::npos) return false;
            t.erase(pos, 1);
        }
        return t.empty();
    }
};
```

#### Python 3.12
```python
class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        if len(s) != len(t):
            return False
        t_list = list(t)
        for c in s:
            if c not in t_list:
                return False
            t_list.remove(c)
        return len(t_list) == 0
```

#### Java 21
```java
class Solution {
    public boolean isAnagram(String s, String t) {
        if (s.length() != t.length()) return false;
        StringBuilder sb = new StringBuilder(t);
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            int idx = sb.indexOf(String.valueOf(c));
            if (idx == -1) return false;
            sb.deleteCharAt(idx);
        }
        return sb.length() == 0;
    }
}
```

#### TypeScript
```typescript
function isAnagram(s: string, t: string): boolean {
    if (s.length !== t.length) return false;
    let tStr = t;
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        const idx = tStr.indexOf(c);
        if (idx === -1) return false;
        tStr = tStr.slice(0, idx) + tStr.slice(idx + 1);
    }
    return tStr.length === 0;
}
```

#### Go
```go
package main

import "strings"

func isAnagram(s string, t string) bool {
    if len(s) != len(t) {
        return false
    }
    for _, r := range s {
        idx := strings.IndexRune(t, r)
        if idx == -1 {
            return false
        }
        t = t[:idx] + t[idx+len(string(r)):]
    }
    return len(t) == 0
}
```

#### Rust
```rust
impl Solution {
    pub fn is_anagram(s: String, mut t: String) -> bool {
        if s.len() != t.len() {
            return false;
        }
        for c in s.chars() {
            if let Some(idx) = t.find(c) {
                t.remove(idx);
            } else {
                return false;
            }
        }
        t.is_empty()
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is the 26-element array solution faster than `std::unordered_map`?</summary>
The 26-element integer array occupies only $26 \times 4 = 104$ bytes, fitting completely within two 64-byte L1 CPU cache lines.
Array index calculation `c - 'a'` executes in a single register subtraction instruction.
`std::unordered_map` involves dynamic bucket hashing, pointer indirection, and heap overhead, running $\approx 10\times$ slower.
</details>

<details>
<summary>2. How should the solution adapt when the input contains arbitrary Unicode?</summary>
In Unicode, characters can be multibyte UTF-8 codepoints or grapheme clusters.
A hash map mapping Unicode scalar values (`char32_t` in C++, `rune` in Go, codepoint integers in Java) to frequency counts handles arbitrary alphabets without buffer overflow.
Additionally, Unicode normalization (e.g., NFC vs NFD forms) may be required if accented characters can be represented either as precomposed or combined codepoints.
</details>

<details>
<summary>3. Why is early exit on `s.length != t.length` essential?</summary>
Comparing lengths takes $O(1)$ time.
If lengths differ, the strings cannot be anagrams.
Short-circuiting here eliminates all subsequent character iterations and hashing allocations.
</details>

<details>
<summary>4. Can Valid Anagram be solved using bitwise XOR?</summary>
No. While XOR detects single missing elements in arrays, XOR is order-independent and insensitive to frequency parities.
For example, `"aa"` and `"bb"` have `('a'^'a') == ('b'^'b') == 0`, but they are not anagrams.
XOR fails to count character frequencies.
</details>

<details>
<summary>5. How does SIMD vectorization accelerate character frequency counting?</summary>
Using AVX-512 vector instructions, 64 characters can be loaded into a single `__m512i` vector register.
Histogram tallying can be performed using vector scatter/gather or parallel binning instructions to count frequencies across hundreds of characters per microsecond.
</details>

<details>
<summary>6. How does this problem relate to Group Anagrams (LeetCode 49)?</summary>
In LeetCode 49, multiple strings must be grouped into anagram buckets.
Each string's sorted representation or its serialized 26-element frequency signature (e.g., `"#1#0#0...#2"`) is used as a hash map key to collect matching words.
</details>

<details>
<summary>7. What prevents integer underflow during subtraction in Tier 1?</summary>
Because both strings are guaranteed to have identical lengths, if any character count drops below zero during subtraction, another character count must remain above zero.
Checking `count[c] < 0` during the second pass enables immediate early-exit return without a separate final validation loop.
</details>

<details>
<summary>8. In Rust, why is `s.bytes().zip(t.bytes())` zero-cost?</summary>
Because `s.len() == t.len()` is verified beforehand, the Rust compiler proves that neither slice will read out of bounds.
The iterator zip is compiled into a single loop over contiguous memory with bounds checks completely eliminated by LLVM.
</details>

<details>
<summary>9. What is the difference between an anagram and an anagrammatic palindrome?</summary>
An anagram is any rearrangement of letters.
An anagrammatic palindrome is an anagram that is also a palindrome, which requires that at most one character in the frequency distribution has an odd count.
</details>

<details>
<summary>10. What are the key unit test edge cases for Valid Anagram?</summary>
1. Different length strings: `"a"` and `"ab"` (returns false).
2. Identical single characters: `"a"` and `"a"` (returns true).
3. Equal lengths, disjoint letters: `"ab"` and `"cd"` (returns false).
4. Equal lengths, partial overlaps: `"aacc"` and `"ccac"` (returns false).
5. Valid anagrams with repeating letters: `"anagram"` and `"nagaram"`.
6. Large strings ($N = 50000$).
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/valid-anagram.cpp)
- [Python Implementation](../Python/valid-anagram.py)
- [Java Implementation](../Java/valid-anagram.java)
- [TypeScript Implementation](../TypeScript/valid-anagram.ts)
- [Go Implementation](../Golang/valid-anagram.go)
- [Rust Implementation](../Rust/valid-anagram.rs)

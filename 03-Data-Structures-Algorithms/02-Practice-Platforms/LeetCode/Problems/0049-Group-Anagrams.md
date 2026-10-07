---
id: leetcode-0049-group-anagrams
title: "LeetCode 0049: Group Anagrams"
tags:
  - dsa
  - leetcode
  - hash-table
  - string
  - sorting
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/group-anagrams/"
---

# LeetCode 0049: Group Anagrams

## 1. Problem Formalization and Constraints

Given an array of strings `strs`, group the anagrams together.
You can return the answer in any order.
An anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

### Constraints
- $1 \le \text{strs.length} \le 10^4$
- $0 \le \text{strs}[i]\text{.length} \le 100$
- `strs[i]` consists of lowercase English letters.

### Examples
- **Example 1**:
  - Input: `strs = ["eat","tea","tan","ate","nat","bat"]`
  - Output: `[["bat"],["nat","tan"],["ate","eat","tea"]]`
- **Example 2**:
  - Input: `strs = [""]`
  - Output: `[[""]]`
- **Example 3**:
  - Input: `strs = ["a"]`
  - Output: `[["a"]]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Character Count Array / Tuple Hashing | $O(N \times K)$ | $O(N \times K)$ | Uses a 26-element character frequency array as the hash map key; avoids comparison sorts entirely. |
| **Tier 2 (Space-Optimized / Sorted Key)** | Canonical Sorted String Key | $O(N \times K \log K)$ | $O(N \times K)$ | Sorts each string alphabetically to form the canonical bucket key; simple and cache-friendly. |
| **Tier 3 (Time-Optimized Alternative)** | Prime Product Hashing | $O(N \times K)$ | $O(N \times K)$ | Maps letters to primes and hashes by multiplication; requires arbitrary-precision integers to prevent overflow. |
| **Tier 4 (Brute Force)** | Pairwise Anagram Check with Visited Array | $O(N^2 \times K)$ | $O(N)$ | Compares every string pairwise using frequency checks; quadratic latency leads to timeouts on large inputs. |

---

## 3. Tier 1: Most Optimal Solution (Character Count Array / Tuple Hashing)

### 3.1 Algorithmic Mechanics and Invariant Proof

Two strings are anagrams if and only if they contain identical character frequencies for all 26 lowercase English letters.
Instead of sorting each string in $O(K \log K)$ time, we construct a 26-element integer count tuple:
$$\text{key} = (\text{count}[0], \text{count}[1], \dots, \text{count}[25])$$
In languages with hashable fixed-size arrays (like Go and Rust), the array `[26]int` or `[u8; 26]` can directly serve as the hash table key.
In other languages, we serialize the counts into a delimited string (e.g., `"#1#0#0...#2"`).

**Invariant Proof**:
The Fundamental Theorem of Arithmetic and bijection of character multiset representations guarantee that:
$$\text{Count}(s_1) = \text{Count}(s_2) \iff s_1 \text{ is an anagram of } s_2$$
Thus, every word maps to its unique canonical equivalence class in $O(K)$ time.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \times K)$, where $N$ is `strs.length` and $K$ is the maximum length of a string in `strs`. Constructing the count array takes $O(K)$ time per string.
- **Space Complexity**: $O(N \times K)$ to store the grouped strings inside the hash map.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

class Solution {
public:
    std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
        std::unordered_map<std::string, std::vector<std::string>> groups;

        for (const std::string& s : strs) {
            std::array<int, 26> count = {0};
            for (char c : s) {
                ++count[c - 'a'];
            }
            std::string key = "";
            for (int i = 0; i < 26; ++i) {
                key += '#';
                key += std::to_string(count[i]);
            }
            groups[key].push_back(s);
        }

        std::vector<std::vector<std::string>> result;
        result.reserve(groups.size());
        for (auto& pair : groups) {
            result.push_back(std::move(pair.second));
        }
        return result;
    }
};
```

#### Python 3.12
```python
from collections import defaultdict

class Solution:
    def groupAnagrams(self, strs: list[str]) -> list[list[str]]:
        groups: defaultdict[tuple[int, ...], list[str]] = defaultdict(list)

        for s in strs:
            count = [0] * 26
            for ch in s:
                count[ord(ch) - 97] += 1
            groups[tuple(count)].append(s)

        return list(groups.values())
```

#### Java 21
```java
import java.util.*;

class Solution {
    public List<List<String>> groupAnagrams(String[] strs) {
        Map<String, List<String>> map = new HashMap<>();

        for (String s : strs) {
            int[] count = new int[26];
            for (char c : s.toCharArray()) {
                count[c - 'a']++;
            }
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < 26; i++) {
                sb.append('#').append(count[i]);
            }
            String key = sb.toString();

            map.computeIfAbsent(key, k -> new ArrayList<>()).add(s);
        }

        return new ArrayList<>(map.values());
    }
}
```

#### TypeScript
```typescript
function groupAnagrams(strs: string[]): string[][] {
    const map = new Map<string, string[]>();

    for (const s of strs) {
        const count = new Array(26).fill(0);
        for (let i = 0; i < s.length; i++) {
            count[s.charCodeAt(i) - 97]++;
        }
        const key = count.join("#");

        const group = map.get(key);
        if (group) {
            group.push(s);
        } else {
            map.set(key, [s]);
        }
    }

    return Array.from(map.values());
}
```

#### Go
```go
package main

func groupAnagrams(strs []string) [][]string {
    groups := make(map[[26]int][]string)

    for _, s := range strs {
        var count [26]int
        for i := 0; i < len(s); i++ {
            count[s[i]-'a']++
        }
        groups[count] = append(groups[count], s)
    }

    result := make([][]string, 0, len(groups))
    for _, group := range groups {
        result = append(result, group)
    }

    return result
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn group_anagrams(strs: Vec<String>) -> Vec<Vec<String>> {
        let mut map: HashMap<[u8; 26], Vec<String>> = HashMap::new();

        for s in strs {
            let mut count = [0u8; 26];
            for &b in s.as_bytes() {
                count[(b - b'a') as usize] += 1;
            }
            map.entry(count).or_default().push(s);
        }

        map.into_values().collect()
    }
}
```

---

## 4. Tier 2: Space-Optimized Solution (Canonical Sorted String Key)

### 4.1 Algorithmic Mechanics and Invariant Proof

Each string `s` is sorted alphabetically:
$$\text{key} = \text{sort}(s)$$
Because two anagrams contain identical multiset frequencies, their lexicographically sorted permutations are identical.
We insert each original string into the hash map indexed by this sorted key.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \times K \log K)$, where sorting each string of length $K$ takes $O(K \log K)$ time.
- **Space Complexity**: $O(N \times K)$ to store keys and grouped strings.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

class Solution {
public:
    std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
        std::unordered_map<std::string, std::vector<std::string>> groups;

        for (const std::string& s : strs) {
            std::string key = s;
            std::sort(key.begin(), key.end());
            groups[key].push_back(s);
        }

        std::vector<std::vector<std::string>> result;
        result.reserve(groups.size());
        for (auto& pair : groups) {
            result.push_back(std::move(pair.second));
        }
        return result;
    }
};
```

#### Python 3.12
```python
from collections import defaultdict

class Solution:
    def groupAnagrams(self, strs: list[str]) -> list[list[str]]:
        groups: defaultdict[str, list[str]] = defaultdict(list)

        for s in strs:
            key = "".join(sorted(s))
            groups[key].append(s)

        return list(groups.values())
```

#### Java 21
```java
import java.util.*;

class Solution {
    public List<List<String>> groupAnagrams(String[] strs) {
        Map<String, List<String>> map = new HashMap<>();

        for (String s : strs) {
            char[] chars = s.toCharArray();
            Arrays.sort(chars);
            String key = new String(chars);

            map.computeIfAbsent(key, k -> new ArrayList<>()).add(s);
        }

        return new ArrayList<>(map.values());
    }
}
```

#### TypeScript
```typescript
function groupAnagrams(strs: string[]): string[][] {
    const map = new Map<string, string[]>();

    for (const s of strs) {
        const key = s.split("").sort().join("");
        const group = map.get(key);
        if (group) {
            group.push(s);
        } else {
            map.set(key, [s]);
        }
    }

    return Array.from(map.values());
}
```

#### Go
```go
package main

import "sort"

func groupAnagrams(strs []string) [][]string {
    groups := make(map[string][]string)

    for _, s := range strs {
        bytes := []byte(s)
        sort.Slice(bytes, func(i, j int) bool {
            return bytes[i] < bytes[j]
        })
        key := string(bytes)
        groups[key] = append(groups[key], s)
    }

    result := make([][]string, 0, len(groups))
    for _, group := range groups {
        result = append(result, group)
    }

    return result
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn group_anagrams(strs: Vec<String>) -> Vec<Vec<String>> {
        let mut map: HashMap<Vec<u8>, Vec<String>> = HashMap::new();

        for s in strs {
            let mut key = s.as_bytes().to_vec();
            key.sort_unstable();
            map.entry(key).or_default().push(s);
        }

        map.into_values().collect()
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Prime Product Hashing)

### 5.1 Algorithmic Mechanics and Invariant Proof

By the Fundamental Theorem of Arithmetic, every positive integer has a unique prime factorization.
Assign the first 26 prime numbers to the letters `'a'` through `'z'`:
$$P = \{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101\}$$
For any string $s$, define its hash as:
$$\text{hash}(s) = \prod_{c \in s} P[c - \text{'a'}]$$
Two strings have the same prime product hash if and only if their character multisets are identical.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N \times K)$. Computing the product takes $O(K)$ time per string.
- **Space Complexity**: $O(N \times K)$ for hash table storage.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <unordered_map>

class Solution {
public:
    std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
        const int primes[26] = {
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
            47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101
        };
        const unsigned long long MOD = 1000000007;

        // Pair hash and length to avoid rare modulo collisions
        std::unordered_map<std::string, std::vector<std::string>> groups;

        for (const std::string& s : strs) {
            unsigned long long hash = 1;
            for (char c : s) {
                hash = (hash * primes[c - 'a']) % MOD;
            }
            std::string key = std::to_string(hash) + ":" + std::to_string(s.length());
            groups[key].push_back(s);
        }

        std::vector<std::vector<std::string>> result;
        for (auto& p : groups) {
            result.push_back(std::move(p.second));
        }
        return result;
    }
};
```

#### Python 3.12
```python
from collections import defaultdict

class Solution:
    def groupAnagrams(self, strs: list[str]) -> list[list[str]]:
        primes = [
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
            47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101
        ]
        groups: defaultdict[int, list[str]] = defaultdict(list)

        for s in strs:
            prod = 1
            for ch in s:
                prod *= primes[ord(ch) - 97]
            groups[prod].append(s)

        return list(groups.values())
```

#### Java 21
```java
import java.math.BigInteger;
import java.util.*;

class Solution {
    public List<List<String>> groupAnagrams(String[] strs) {
        int[] primes = {
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
            47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101
        };
        Map<BigInteger, List<String>> map = new HashMap<>();

        for (String s : strs) {
            BigInteger prod = BigInteger.ONE;
            for (char c : s.toCharArray()) {
                prod = prod.multiply(BigInteger.valueOf(primes[c - 'a']));
            }
            map.computeIfAbsent(prod, k -> new ArrayList<>()).add(s);
        }

        return new ArrayList<>(map.values());
    }
}
```

#### TypeScript
```typescript
function groupAnagrams(strs: string[]): string[][] {
    const primes = [
        2n, 3n, 5n, 7n, 11n, 13n, 17n, 19n, 23n, 29n, 3n, 37n, 41n, 43n,
        47n, 53n, 59n, 61n, 67n, 71n, 73n, 79n, 83n, 89n, 97n, 101n
    ];
    const map = new Map<string, string[]>();

    for (const s of strs) {
        let prod = 1n;
        for (let i = 0; i < s.length; i++) {
            prod *= primes[s.charCodeAt(i) - 97];
        }
        const key = prod.toString();
        const group = map.get(key);
        if (group) {
            group.push(s);
        } else {
            map.set(key, [s]);
        }
    }

    return Array.from(map.values());
}
```

#### Go
```go
package main

import (
    "math/big"
)

func groupAnagrams(strs []string) [][]string {
    primes := []int64{
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
        47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
    }
    groups := make(map[string][]string)

    for _, s := range strs {
        prod := big.NewInt(1)
        for i := 0; i < len(s); i++ {
            prod.Mul(prod, big.NewInt(primes[s[i]-'a']))
        }
        key := prod.String()
        groups[key] = append(groups[key], s)
    }

    result := make([][]string, 0, len(groups))
    for _, group := range groups {
        result = append(result, group)
    }

    return result
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn group_anagrams(strs: Vec<String>) -> Vec<Vec<String>> {
        let primes: [u128; 26] = [
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
            47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
        ];
        let mut map: HashMap<[u8; 26], Vec<String>> = HashMap::new();

        for s in strs {
            let mut count = [0u8; 26];
            for &b in s.as_bytes() {
                count[(b - b'a') as usize] += 1;
            }
            map.entry(count).or_default().push(s);
        }

        map.into_values().collect()
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Pairwise Anagram Check with Visited Array)

### 6.1 Algorithmic Mechanics and Invariant Proof

Maintain a boolean `visited` array of size $N$.
For each unvisited index $i$:
1. Start a new anagram group initialized with `strs[i]`.
2. Compare `strs[i]` pairwise with all subsequent unvisited words `strs[j]` ($j > i$) using character frequency verification.
3. If `strs[j]` is an anagram of `strs[i]`, append `strs[j]` to the group and mark `visited[j] = true`.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2 \times K)$. Compares each pair of words in $O(K)$ time across $O(N^2)$ candidate pairs.
- **Space Complexity**: $O(N)$ for the boolean visited tracker.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <array>

class Solution {
public:
    std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
        int n = static_cast<int>(strs.size());
        std::vector<bool> visited(n, false);
        std::vector<std::vector<std::string>> result;

        auto isAnagram = [](const std::string& a, const std::string& b) {
            if (a.length() != b.length()) return false;
            std::array<int, 26> count = {0};
            for (char c : a) ++count[c - 'a'];
            for (char c : b) {
                if (--count[c - 'a'] < 0) return false;
            }
            return true;
        };

        for (int i = 0; i < n; ++i) {
            if (visited[i]) continue;
            visited[i] = true;
            std::vector<std::string> group = {strs[i]};

            for (int j = i + 1; j < n; ++j) {
                if (!visited[j] && isAnagram(strs[i], strs[j])) {
                    visited[j] = true;
                    group.push_back(strs[j]);
                }
            }
            result.push_back(std::move(group));
        }
        return result;
    }
};
```

#### Python 3.12
```python
class Solution:
    def groupAnagrams(self, strs: list[str]) -> list[list[str]]:
        n = len(strs)
        visited = [False] * n
        result: list[list[str]] = []

        def is_anagram(a: str, b: str) -> bool:
            if len(a) != len(b):
                return False
            cnt = [0] * 26
            for ch in a:
                cnt[ord(ch) - 97] += 1
            for ch in b:
                cnt[ord(ch) - 97] -= 1
                if cnt[ord(ch) - 97] < 0:
                    return False
            return True

        for i in range(n):
            if visited[i]:
                continue
            visited[i] = True
            group = [strs[i]]
            for j in range(i + 1, n):
                if not visited[j] and is_anagram(strs[i], strs[j]):
                    visited[j] = True
                    group.append(strs[j])
            result.append(group)

        return result
```

#### Java 21
```java
import java.util.*;

class Solution {
    public List<List<String>> groupAnagrams(String[] strs) {
        int n = strs.length;
        boolean[] visited = new boolean[n];
        List<List<String>> result = new ArrayList<>();

        for (int i = 0; i < n; i++) {
            if (visited[i]) continue;
            visited[i] = true;
            List<String> group = new ArrayList<>();
            group.add(strs[i]);

            for (int j = i + 1; j < n; j++) {
                if (!visited[j] && isAnagram(strs[i], strs[j])) {
                    visited[j] = true;
                    group.add(strs[j]);
                }
            }
            result.add(group);
        }
        return result;
    }

    private boolean isAnagram(String a, String b) {
        if (a.length() != b.length()) return false;
        int[] count = new int[26];
        for (char c : a.toCharArray()) count[c - 'a']++;
        for (char c : b.toCharArray()) {
            if (--count[c - 'a'] < 0) return false;
        }
        return true;
    }
}
```

#### TypeScript
```typescript
function groupAnagrams(strs: string[]): string[][] {
    const n = strs.length;
    const visited = new Uint8Array(n);
    const result: string[][] = [];

    function isAnagram(a: string, b: string): boolean {
        if (a.length !== b.length) return false;
        const count = new Int32Array(26);
        for (let i = 0; i < a.length; i++) count[a.charCodeAt(i) - 97]++;
        for (let i = 0; i < b.length; i++) {
            const idx = b.charCodeAt(i) - 97;
            if (--count[idx] < 0) return false;
        }
        return true;
    }

    for (let i = 0; i < n; i++) {
        if (visited[i]) continue;
        visited[i] = 1;
        const group = [strs[i]];

        for (let j = i + 1; j < n; j++) {
            if (!visited[j] && isAnagram(strs[i], strs[j])) {
                visited[j] = 1;
                group.push(strs[j]);
            }
        }
        result.push(group);
    }

    return result;
}
```

#### Go
```go
package main

func groupAnagrams(strs []string) [][]string {
    n := len(strs)
    visited := make([]bool, n)
    var result [][]string

    isAnagram := func(a, b string) bool {
        if len(a) != len(b) {
            return false
        }
        var count [26]int
        for i := 0; i < len(a); i++ {
            count[a[i]-'a']++
        }
        for i := 0; i < len(b); i++ {
            count[b[i]-'a']--
            if count[b[i]-'a'] < 0 {
                return false
            }
        }
        return true
    }

    for i := 0; i < n; i++ {
        if visited[i] {
            continue
        }
        visited[i] = true
        group := []string{strs[i]}

        for j := i + 1; j < n; j++ {
            if !visited[j] && isAnagram(strs[i], strs[j]) {
                visited[j] = true
                group = append(group, strs[j])
            }
        }
        result = append(result, group)
    }

    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn group_anagrams(strs: Vec<String>) -> Vec<Vec<String>> {
        let n = strs.len();
        let mut visited = vec![false; n];
        let mut result = Vec::new();

        let is_anagram = |a: &str, b: &str| -> bool {
            if a.len() != b.len() {
                return false;
            }
            let mut count = [0i32; 26];
            for &c in a.as_bytes() {
                count[(c - b'a') as usize] += 1;
            }
            for &c in b.as_bytes() {
                let idx = (c - b'a') as usize;
                count[idx] -= 1;
                if count[idx] < 0 {
                    return false;
                }
            }
            true
        };

        for i in 0..n {
            if visited[i] {
                continue;
            }
            visited[i] = true;
            let mut group = vec![strs[i].clone()];

            for j in (i + 1)..n {
                if !visited[j] && is_anagram(&strs[i], &strs[j]) {
                    visited[j] = true;
                    group.push(strs[j].clone());
                }
            }
            result.push(group);
        }

        result
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is the 26-element count tuple asymptotically faster than sorting strings?</summary>
Counting characters runs in $O(K)$ linear time relative to the string length.
Sorting runs in $O(K \log K)$ comparison time.
For large string lengths $K$, the count array eliminates the logarithmic sorting multiplier.
</details>

<details>
<summary>2. Why can array types like `[26]int` in Go and `[u8; 26]` in Rust directly serve as map keys?</summary>
In Go and Rust, fixed-size arrays have defined value equality and hashing semantics built directly into the compiler.
In Java and Python, raw arrays cannot serve as map keys because their equality is based on object reference identity, requiring tuples or formatted string serialization.
</details>

<details>
<summary>3. What is the danger of Prime Product Hashing in fixed-width integers?</summary>
A string of length 100 consisting of letters like 'z' (prime 101) produces $101^{100}$, which exceeds 64-bit and 128-bit integer capacities.
Using standard fixed integer types causes silent integer overflow and hash collisions unless arbitrary-precision integers (BigInteger) are used.
</details>

<details>
<summary>4. What happens when the input contains empty strings `""`?</summary>
An empty string produces a count array of all zeros.
All empty strings map to the same key and group together correctly.
</details>

<details>
<summary>5. How does delimiter insertion prevent key collision in serialized strings?</summary>
Without delimiters, `"ab"` (count: 1 of a, 1 of b) and `"k"` (count: 11 of something) could produce ambiguous representations like `11000...`.
Delimiters like `#` ensure each frequency integer is parsed as a distinct field.
</details>

<details>
<summary>6. How does compiler vectorization optimize character counting?</summary>
When processing ASCII strings, SIMD instructions can tally character counts across 16 or 32 bytes simultaneously using vector shuffle instructions.
</details>

<details>
<summary>7. What is the time complexity when all $N$ strings are distinct and have length $K$?</summary>
The hash map performs $N$ insertions with unique keys, still completing in $O(N \times K)$ time.
</details>

<details>
<summary>8. How does `computeIfAbsent` in Java 21 improve performance?</summary>
It eliminates redundant double lookups (`containsKey` followed by `get` or `put`), executing lookup and initialization in a single hash probe.
</details>

<details>
<summary>9. Why does the brute force approach time out on LeetCode?</summary>
For $N = 10^4$, $N^2 = 10^8$ operations.
Pairwise comparisons at $10^8$ steps exceed the typical 2-second time limit for online judges.
</details>

<details>
<summary>10. What is the maximum possible value in any entry of the 26-count array given $K \le 100$?</summary>
The maximum count is 100.
Because $100 < 256$, each count fits inside an 8-bit unsigned integer (`u8`), allowing compact memory usage in Rust and C++.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/group-anagrams.cpp)
- [Python Implementation](../Python/group-anagrams.py)
- [Java Implementation](../Java/group-anagrams.java)
- [TypeScript Implementation](../TypeScript/group-anagrams.ts)
- [Go Implementation](../Golang/group-anagrams.go)
- [Rust Implementation](../Rust/group-anagrams.rs)

---
id: leetcode-0005-longest-palindromic-substring
title: "LeetCode 0005: Longest Palindromic Substring"
tags:
  - dsa
  - leetcode
  - string
  - dynamic-programming
  - manachers-algorithm
  - two-pointers
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/longest-palindromic-substring/"
---

# LeetCode 0005: Longest Palindromic Substring

## 1. Problem Formalization and Constraints

Given a string `s`, return the longest palindromic substring in `s`.
A string is called a palindrome if it reads the same backward as forward.

### Constraints
- $1 \le \text{s.length} \le 1000$
- `s` consists of only digits and English letters.

### Examples
- **Example 1**:
  - Input: `s = "babad"`
  - Output: `"bab"`
  - Explanation: `"aba"` is also a valid answer.
- **Example 2**:
  - Input: `s = "cbbd"`
  - Output: `"bb"`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Manacher's Algorithm | $O(N)$ | $O(N)$ | Interleaves delimiters and uses palindrome symmetry around center to achieve strict linear time. |
| **Tier 2 (Space-Optimized)** | Expand Around Centers | $O(N^2)$ | $O(1)$ | Tests $2N - 1$ centers with two pointers; achieves optimal zero auxiliary heap space. |
| **Tier 3 (Time-Optimized Alternative)** | 2D Dynamic Programming Tabulation | $O(N^2)$ | $O(N^2)$ | Table lookup `dp[i][j]` tracks palindromic subproblems; high quadratic memory overhead. |
| **Tier 4 (Brute Force)** | Exhaustive Substring Check | $O(N^3)$ | $O(1)$ | Generates all $O(N^2)$ substrings and validates each with a two-pointer linear pass. |

---

## 3. Tier 1: Most Optimal Solution (Manacher's Algorithm)

### 3.1 Algorithmic Mechanics and Invariant Proof

Manacher's Algorithm solves the problem in strict linear time by transforming the string to handle both even and odd length palindromes uniformly and exploiting previously computed palindrome boundaries.
1. Preprocess string `s` by inserting special delimiter `#` between every character, flanked by start and end guards `^` and `$`.
For example, `"aba"` becomes `"^#a#b#a#$"`.
This ensures all palindromes have an odd length in the transformed string `T`.
2. Maintain array `P[i]` denoting the radius of the longest palindrome centered at index `i`.
3. Maintain the rightmost boundary `R` of any identified palindrome and its corresponding center `C`.
4. When evaluating index `i`:
   - If $i < R$, the mirror index is $i' = 2C - i$.
   We initialize $P[i] = \min(R - i, P[i'])$.
   - Expand beyond the current radius while $T[i + 1 + P[i]] == T[i - 1 - P[i]]$.
   - If $i + P[i] > R$, update $C = i$ and $R = i + P[i]$.
5. Find the maximum value in `P` and extract the original substring.

**Invariant Proof**:
The right boundary `R` only advances forward, monotonically increasing from $0$ to $|T|$.
Each character comparison either extends `R` or fails and terminates the expansion loop for that center.
Therefore, the total number of character comparisons across the entire algorithm is bounded by $2|T|$, guaranteeing $O(N)$ time complexity.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Amortized linear time because the right boundary `R` advances at most $2N + 3$ times.
- **Space Complexity**: $O(N)$. Auxiliary string `T` and radius array `P` require $2N + 3$ elements.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <algorithm>

class Solution {
public:
    std::string longestPalindrome(std::string s) {
        if (s.empty()) return "";
        std::string t = "^";
        for (char c : s) {
            t += '#';
            t += c;
        }
        t += "#$";

        int n = static_cast<int>(t.length());
        std::vector<int> p(n, 0);
        int c = 0, r = 0;

        for (int i = 1; i < n - 1; ++i) {
            int i_mirror = 2 * c - i;
            if (r > i) {
                p[i] = std::min(r - i, p[i_mirror]);
            }
            while (t[i + 1 + p[i]] == t[i - 1 - p[i]]) {
                ++p[i];
            }
            if (i + p[i] > r) {
                c = i;
                r = i + p[i];
            }
        }

        int max_len = 0;
        int center_idx = 0;
        for (int i = 1; i < n - 1; ++i) {
            if (p[i] > max_len) {
                max_len = p[i];
                center_idx = i;
            }
        }

        int start = (center_idx - max_len) / 2;
        return s.substr(start, max_len);
    }
};
```

#### Python 3.12
```python
class Solution:
    def longestPalindrome(self, s: str) -> str:
        if not s:
            return ""
        t = "^#" + "#".join(s) + "#$"
        n = len(t)
        p = [0] * n
        c = 0
        r = 0

        for i in range(1, n - 1):
            i_mirror = 2 * c - i
            if r > i:
                p[i] = min(r - i, p[i_mirror])
            while t[i + 1 + p[i]] == t[i - 1 - p[i]]:
                p[i] += 1
            if i + p[i] > r:
                c = i
                r = i + p[i]

        max_len = 0
        center_idx = 0
        for i in range(1, n - 1):
            if p[i] > max_len:
                max_len = p[i]
                center_idx = i

        start = (center_idx - max_len) // 2
        return s[start : start + max_len]
```

#### Java 21
```java
class Solution {
    public String longestPalindrome(String s) {
        if (s == null || s.length() == 0) return "";
        StringBuilder sb = new StringBuilder("^");
        for (int i = 0; i < s.length(); i++) {
            sb.append("#").append(s.charAt(i));
        }
        sb.append("#$");
        String t = sb.toString();

        int n = t.length();
        int[] p = new int[n];
        int c = 0, r = 0;

        for (int i = 1; i < n - 1; i++) {
            int iMirror = 2 * c - i;
            if (r > i) {
                p[i] = Math.min(r - i, p[iMirror]);
            }
            while (t.charAt(i + 1 + p[i]) == t.charAt(i - 1 - p[i])) {
                p[i]++;
            }
            if (i + p[i] > r) {
                c = i;
                r = i + p[i];
            }
        }

        int maxLen = 0;
        int centerIndex = 0;
        for (int i = 1; i < n - 1; i++) {
            if (p[i] > maxLen) {
                maxLen = p[i];
                centerIndex = i;
            }
        }

        int start = (centerIndex - maxLen) / 2;
        return s.substring(start, start + maxLen);
    }
}
```

#### TypeScript
```typescript
function longestPalindrome(s: string): string {
    if (s.length <= 1) return s;

    let t = "^";
    for (let i = 0; i < s.length; i++) {
        t += "#" + s[i];
    }
    t += "#$";

    const n = t.length;
    const p = new Int32Array(n);
    let c = 0;
    let r = 0;

    for (let i = 1; i < n - 1; i++) {
        const iMirror = 2 * c - i;
        if (r > i) {
            p[i] = Math.min(r - i, p[iMirror]);
        }
        while (t[i + 1 + p[i]] === t[i - 1 - p[i]]) {
            p[i]++;
        }
        if (i + p[i] > r) {
            c = i;
            r = i + p[i];
        }
    }

    let maxLen = 0;
    let centerIndex = 0;
    for (let i = 1; i < n - 1; i++) {
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIndex = i;
        }
    }

    const start = Math.floor((centerIndex - maxLen) / 2);
    return s.substring(start, start + maxLen);
}
```

#### Go
```go
package main

func longestPalindrome(s string) string {
    if len(s) <= 1 {
        return s
    }

    t := make([]byte, 0, 2*len(s)+3)
    t = append(t, '^')
    for i := 0; i < len(s); i++ {
        t = append(t, '#', s[i])
    }
    t = append(t, '#', '$')

    n := len(t)
    p := make([]int, n)
    c, r := 0, 0

    for i := 1; i < n-1; i++ {
        iMirror := 2*c - i
        if r > i {
            if r-i < p[iMirror] {
                p[i] = r - i
            } else {
                p[i] = p[iMirror]
            }
        }
        for t[i+1+p[i]] == t[i-1-p[i]] {
            p[i]++
        }
        if i+p[i] > r {
            c = i
            r = i + p[i]
        }
    }

    maxLen := 0
    centerIndex := 0
    for i := 1; i < n-1; i++ {
        if p[i] > maxLen {
            maxLen = p[i]
            centerIndex = i
        }
    }

    start := (centerIndex - maxLen) / 2
    return s[start : start+maxLen]
}
```

#### Rust
```rust
impl Solution {
    pub fn longest_palindrome(s: String) -> String {
        if s.len() <= 1 {
            return s;
        }

        let mut t: Vec<u8> = Vec::with_capacity(2 * s.len() + 3);
        t.push(b'^');
        for &b in s.as_bytes() {
            t.push(b'#');
            t.push(b);
        }
        t.push(b'#');
        t.push(b'$');

        let n = t.len();
        let mut p = vec![0usize; n];
        let mut c = 0usize;
        let mut r = 0usize;

        for i in 1..n - 1 {
            let i_mirror = if 2 * c >= i { 2 * c - i } else { 0 };
            if r > i {
                p[i] = (r - i).min(p[i_mirror]);
            }
            while t[i + 1 + p[i]] == t[i - 1 - p[i]] {
                p[i] += 1;
            }
            if i + p[i] > r {
                c = i;
                r = i + p[i];
            }
        }

        let mut max_len = 0usize;
        let mut center_index = 0usize;
        for i in 1..n - 1 {
            if p[i] > max_len {
                max_len = p[i];
                center_index = i;
            }
        }

        let start = (center_index - max_len) / 2;
        s[start..start + max_len].to_string()
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized Solution (Expand Around Centers)

### 4.1 Algorithmic Mechanics and Invariant Proof

Every palindrome has a center:
- An odd-length palindrome centers on a single character (e.g. `"aba"` centers on `b`).
- An even-length palindrome centers between two characters (e.g. `"abba"` centers between `b` and `b`).
In a string of length $N$, there are $2N - 1$ potential centers ($N$ character centers and $N - 1$ between-character centers).
For each center, expand outward with two pointers `left` and `right` while `s[left] == s[right]`.
Track the maximum length and starting index encountered.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. There are $2N - 1$ centers, and each center can expand at most $O(N)$ characters.
- **Space Complexity**: $O(1)$. Requires only integer pointers; zero auxiliary heap memory.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <algorithm>

class Solution {
public:
    std::string longestPalindrome(std::string s) {
        if (s.empty()) return "";
        int start = 0, max_len = 1;

        auto expand = [&](int l, int r) {
            while (l >= 0 && r < static_cast<int>(s.length()) && s[l] == s[r]) {
                --l;
                ++r;
            }
            int len = r - l - 1;
            if (len > max_len) {
                max_len = len;
                start = l + 1;
            }
        };

        for (int i = 0; i < static_cast<int>(s.length()); ++i) {
            expand(i, i);
            expand(i, i + 1);
        }

        return s.substr(start, max_len);
    }
};
```

#### Python 3.12
```python
class Solution:
    def longestPalindrome(self, s: str) -> str:
        if len(s) <= 1:
            return s
        start = 0
        max_len = 1

        def expand(l: int, r: int) -> None:
            nonlocal start, max_len
            while l >= 0 and r < len(s) and s[l] == s[r]:
                l -= 1
                r += 1
            cur_len = r - l - 1
            if cur_len > max_len:
                max_len = cur_len
                start = l + 1

        for i in range(len(s)):
            expand(i, i)
            expand(i, i + 1)

        return s[start : start + max_len]
```

#### Java 21
```java
class Solution {
    public String longestPalindrome(String s) {
        if (s == null || s.length() < 1) return "";
        int start = 0, end = 0;

        for (int i = 0; i < s.length(); i++) {
            int len1 = expandAroundCenter(s, i, i);
            int len2 = expandAroundCenter(s, i, i + 1);
            int len = Math.max(len1, len2);
            if (len > end - start) {
                start = i - (len - 1) / 2;
                end = i + len / 2;
            }
        }
        return s.substring(start, end + 1);
    }

    private int expandAroundCenter(String s, int left, int right) {
        while (left >= 0 && right < s.length() && s.charAt(left) == s.charAt(right)) {
            left--;
            right++;
        }
        return right - left - 1;
    }
}
```

#### TypeScript
```typescript
function longestPalindrome(s: string): string {
    if (s.length <= 1) return s;
    let start = 0;
    let maxLen = 1;

    function expand(l: number, r: number): void {
        while (l >= 0 && r < s.length && s[l] === s[r]) {
            l--;
            r++;
        }
        const len = r - l - 1;
        if (len > maxLen) {
            maxLen = len;
            start = l + 1;
        }
    }

    for (let i = 0; i < s.length; i++) {
        expand(i, i);
        expand(i, i + 1);
    }

    return s.substring(start, start + maxLen);
}
```

#### Go
```go
package main

func longestPalindrome(s string) string {
    if len(s) <= 1 {
        return s
    }
    start := 0
    maxLen := 1

    expand := func(l, r int) {
        for l >= 0 && r < len(s) && s[l] == s[r] {
            l--
            r++
        }
        length := r - l - 1
        if length > maxLen {
            maxLen = length
            start = l + 1
        }
    }

    for i := 0; i < len(s); i++ {
        expand(i, i)
        expand(i, i+1)
    }

    return s[start : start+maxLen]
}
```

#### Rust
```rust
impl Solution {
    pub fn longest_palindrome(s: String) -> String {
        let bytes = s.as_bytes();
        let n = bytes.len();
        if n <= 1 {
            return s;
        }

        let mut start = 0;
        let mut max_len = 1;

        let mut expand = |mut l: i32, mut r: i32| {
            while l >= 0 && (r as usize) < n && bytes[l as usize] == bytes[r as usize] {
                l -= 1;
                r += 1;
            }
            let len = (r - l - 1) as usize;
            if len > max_len {
                max_len = len;
                start = (l + 1) as usize;
            }
        };

        for i in 0..n {
            expand(i as i32, i as i32);
            expand(i as i32, (i + 1) as i32);
        }

        s[start..start + max_len].to_string()
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (2D Dynamic Programming Tabulation)

### 5.1 Algorithmic Mechanics and Invariant Proof

Define boolean subproblem $dp[i][j]$ as true if the substring $s[i \dots j]$ is a palindrome.
The recurrence relation is:
$$dp[i][j] = (s[i] == s[j]) \land (j - i \le 2 \lor dp[i + 1][j - 1])$$
Base cases:
- Any single character substring is a palindrome ($j - i = 0$).
- Two identical adjacent characters form a palindrome ($j - i = 1$).
We iterate by increasing substring length from $1$ to $N$.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Fills an $N \times N$ upper-triangular matrix.
- **Space Complexity**: $O(N^2)$. Requires an $N \times N$ boolean matrix.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>

class Solution {
public:
    std::string longestPalindrome(std::string s) {
        int n = static_cast<int>(s.length());
        if (n <= 1) return s;

        std::vector<std::vector<bool>> dp(n, std::vector<bool>(n, false));
        int start = 0, max_len = 1;

        for (int i = 0; i < n; ++i) dp[i][i] = true;

        for (int len = 2; len <= n; ++len) {
            for (int i = 0; i <= n - len; ++i) {
                int j = i + len - 1;
                if (s[i] == s[j]) {
                    if (len == 2 || dp[i + 1][j - 1]) {
                        dp[i][j] = true;
                        if (len > max_len) {
                            max_len = len;
                            start = i;
                        }
                    }
                }
            }
        }
        return s.substr(start, max_len);
    }
};
```

#### Python 3.12
```python
class Solution:
    def longestPalindrome(self, s: str) -> str:
        n = len(s)
        if n <= 1:
            return s

        dp = [[False] * n for _ in range(n)]
        start = 0
        max_len = 1

        for i in range(n):
            dp[i][i] = True

        for length in range(2, n + 1):
            for i in range(n - length + 1):
                j = i + length - 1
                if s[i] == s[j]:
                    if length == 2 or dp[i + 1][j - 1]:
                        dp[i][j] = True
                        if length > max_len:
                            max_len = length
                            start = i

        return s[start : start + max_len]
```

#### Java 21
```java
class Solution {
    public String longestPalindrome(String s) {
        int n = s.length();
        if (n <= 1) return s;

        boolean[][] dp = new boolean[n][n];
        int start = 0, maxLen = 1;

        for (int i = 0; i < n; i++) dp[i][i] = true;

        for (int len = 2; len <= n; len++) {
            for (int i = 0; i <= n - len; i++) {
                int j = i + len - 1;
                if (s.charAt(i) == s.charAt(j)) {
                    if (len == 2 || dp[i + 1][j - 1]) {
                        dp[i][j] = true;
                        if (len > maxLen) {
                            maxLen = len;
                            start = i;
                        }
                    }
                }
            }
        }
        return s.substring(start, start + maxLen);
    }
}
```

#### TypeScript
```typescript
function longestPalindrome(s: string): string {
    const n = s.length;
    if (n <= 1) return s;

    const dp: boolean[][] = Array.from({ length: n }, () => new Array(n).fill(false));
    let start = 0;
    let maxLen = 1;

    for (let i = 0; i < n; i++) dp[i][i] = true;

    for (let len = 2; len <= n; len++) {
        for (let i = 0; i <= n - len; i++) {
            const j = i + len - 1;
            if (s[i] === s[j]) {
                if (len === 2 || dp[i + 1][j - 1]) {
                    dp[i][j] = true;
                    if (len > maxLen) {
                        maxLen = len;
                        start = i;
                    }
                }
            }
        }
    }
    return s.substring(start, start + maxLen);
}
```

#### Go
```go
package main

func longestPalindrome(s string) string {
    n := len(s)
    if n <= 1 {
        return s
    }

    dp := make([][]bool, n)
    for i := range dp {
        dp[i] = make([]bool, n)
        dp[i][i] = true
    }

    start := 0
    maxLen := 1

    for length := 2; length <= n; length++ {
        for i := 0; i <= n-length; i++ {
            j := i + length - 1
            if s[i] == s[j] {
                if length == 2 || dp[i+1][j-1] {
                    dp[i][j] = true
                    if length > maxLen {
                        maxLen = length
                        start = i
                    }
                }
            }
        }
    }

    return s[start : start+maxLen]
}
```

#### Rust
```rust
impl Solution {
    pub fn longest_palindrome(s: String) -> String {
        let bytes = s.as_bytes();
        let n = bytes.len();
        if n <= 1 {
            return s;
        }

        let mut dp = vec![vec![false; n]; n];
        let mut start = 0;
        let mut max_len = 1;

        for i in 0..n {
            dp[i][i] = true;
        }

        for len in 2..=n {
            for i in 0..=(n - len) {
                let j = i + len - 1;
                if bytes[i] == bytes[j] {
                    if len == 2 || dp[i + 1][j - 1] {
                        dp[i][j] = true;
                        if len > max_len {
                            max_len = len;
                            start = i;
                        }
                    }
                }
            }
        }

        s[start..start + max_len].to_string()
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Exhaustive Substring Check)

### 6.1 Algorithmic Mechanics and Invariant Proof

Generate all possible substrings $s[i \dots j]$ with $0 \le i \le j < N$.
For each substring, verify whether it is a palindrome by comparing symmetric characters from both ends inward.
Maintain the longest valid substring found.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^3)$. There are $\frac{N(N + 1)}{2}$ substrings, and checking each takes $O(N)$ time.
- **Space Complexity**: $O(1)$. No auxiliary heap allocations.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>

class Solution {
public:
    std::string longestPalindrome(std::string s) {
        int n = static_cast<int>(s.length());
        std::string best = "";

        for (int i = 0; i < n; ++i) {
            for (int j = i; j < n; ++j) {
                if (j - i + 1 > static_cast<int>(best.length()) && isPalindrome(s, i, j)) {
                    best = s.substr(i, j - i + 1);
                }
            }
        }
        return best;
    }

private:
    bool isPalindrome(const std::string& s, int l, int r) {
        while (l < r) {
            if (s[l++] != s[r--]) return false;
        }
        return true;
    }
};
```

#### Python 3.12
```python
class Solution:
    def longestPalindrome(self, s: str) -> str:
        best = ""
        n = len(s)

        for i in range(n):
            for j in range(i, n):
                sub = s[i : j + 1]
                if len(sub) > len(best) and sub == sub[::-1]:
                    best = sub

        return best
```

#### Java 21
```java
class Solution {
    public String longestPalindrome(String s) {
        int n = s.length();
        String best = "";

        for (int i = 0; i < n; i++) {
            for (int j = i; j < n; j++) {
                if (j - i + 1 > best.length() && isPalindrome(s, i, j)) {
                    best = s.substring(i, j + 1);
                }
            }
        }
        return best;
    }

    private boolean isPalindrome(String s, int l, int r) {
        while (l < r) {
            if (s.charAt(l++) != s.charAt(r--)) return false;
        }
        return true;
    }
}
```

#### TypeScript
```typescript
function longestPalindrome(s: string): string {
    const n = s.length;
    let best = "";

    function isPalindrome(l: number, r: number): boolean {
        while (l < r) {
            if (s[l++] !== s[r--]) return false;
        }
        return true;
    }

    for (let i = 0; i < n; i++) {
        for (let j = i; j < n; j++) {
            if (j - i + 1 > best.length && isPalindrome(i, j)) {
                best = s.substring(i, j + 1);
            }
        }
    }
    return best;
}
```

#### Go
```go
package main

func longestPalindrome(s string) string {
    n := len(s)
    best := ""

    isPalindrome := func(l, r int) bool {
        for l < r {
            if s[l] != s[r] {
                return false
            }
            l++
            r--
        }
        return true
    }

    for i := 0; i < n; i++ {
        for j := i; j < n; j++ {
            if j-i+1 > len(best) && isPalindrome(i, j) {
                best = s[i : j+1]
            }
        }
    }

    return best
}
```

#### Rust
```rust
impl Solution {
    pub fn longest_palindrome(s: String) -> String {
        let bytes = s.as_bytes();
        let n = bytes.len();
        let mut best = "";

        let is_palindrome = |mut l: usize, mut r: usize| -> bool {
            while l < r {
                if bytes[l] != bytes[r] {
                    return false;
                }
                l += 1;
                r -= 1;
            }
            true
        };

        for i in 0..n {
            for j in i..n {
                if j - i + 1 > best.len() && is_palindrome(i, j) {
                    best = &s[i..=j];
                }
            }
        }

        best.to_string()
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why does Manacher's Algorithm interleave `#` characters between original letters?</summary>
In a standard string, palindromes can have either odd length (centered on a character) or even length (centered between two characters).
By inserting delimiters, every palindrome in the transformed string has an odd length and centers on an explicit delimiter or character, unifying the logic.
</details>

<details>
<summary>2. What is the role of boundary guard characters `^` and `$` in Manacher's Algorithm?</summary>
Guards prevent out-of-bounds memory accesses when expanding palindrome radii without requiring manual boundary check conditions inside the expansion loop.
</details>

<details>
<summary>3. Why is the time complexity of Manacher's Algorithm strictly $O(N)$ and not $O(N^2)$?</summary>
Because the rightmost boundary $R$ strictly moves forward.
Character comparisons that match advance $R$, and comparisons that do not match break the while loop immediately.
The total number of successful matches across all iterations cannot exceed the length of the string.
</details>

<details>
<summary>4. What is the space-time trade-off between Expand Around Centers and Manacher's Algorithm?</summary>
Expand Around Centers requires $O(1)$ auxiliary space and $O(N^2)$ time.
Manacher's Algorithm requires $O(N)$ auxiliary space but achieves optimal $O(N)$ linear time.
For $N \le 1000$, Expand Around Centers is often preferred in practice due to simplicity and minimal memory footprint.
</details>

<details>
<summary>5. How does the 2D DP solution fail when scaling to strings of length $10^5$?</summary>
An $N \times N$ matrix requires $(10^5)^2 = 10^{10}$ bytes (approximately 10 GB) of memory, causing an immediate Out-Of-Memory error.
Furthermore, $10^{10}$ operations will exceed standard 1-second CPU execution limits.
</details>

<details>
<summary>6. How do you recover the original substring coordinates from the transformed string radius in Manacher's?</summary>
If the maximum palindrome center is `center_idx` with radius `max_len`, the starting index in original string `s` is `(center_idx - max_len) / 2`, and its length is exactly `max_len`.
</details>

<details>
<summary>7. What is the behavior of all four solutions when the input string contains all identical characters (e.g. `"aaaaa"`)?</summary>
The entire string is a palindrome.
Manacher's algorithm expands $R$ directly to the end guard in one pass.
Expand Around Centers expands to the boundaries for the middle center.
The DP table becomes upper-triangular all true.
All return the complete input string.
</details>

<details>
<summary>8. Can rolling hash (Rabin-Karp) and binary search be used to solve this problem?</summary>
Yes. Compute prefix forward and reverse polynomial rolling hashes.
Binary search on the palindrome length (separately for odd and even lengths) and use $O(1)$ hash queries to check for matches, yielding an $O(N \log N)$ solution.
</details>

<details>
<summary>9. Why does `P[i] = min(R - i, P[i_mirror])` hold true?</summary>
Because index $i$ and its mirror $i'$ are symmetric about center $C$.
Within the bounds of $[2C - R, R]$, the substring centered at $i$ is identical to the substring centered at $i'$, so its palindrome radius is at least $P[i']$, clamped by the distance to the known boundary $R - i$.
</details>

<details>
<summary>10. What happens if the input string consists of distinct characters only (e.g. `"abcdef"`)?</summary>
Every single character is a palindrome of length 1.
The algorithms will identify $P[i] = 1$ for each character and return the first character `s[0]`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/longest-palindromic-substring.cpp)
- [Python Implementation](../Python/longest-palindromic-substring.py)
- [Java Implementation](../Java/longest-palindromic-substring.java)
- [TypeScript Implementation](../TypeScript/longest-palindromic-substring.ts)
- [Go Implementation](../Golang/longest-palindromic-substring.go)
- [Rust Implementation](../Rust/longest-palindromic-substring.rs)

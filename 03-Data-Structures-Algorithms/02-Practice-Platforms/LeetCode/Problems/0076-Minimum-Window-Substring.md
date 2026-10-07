---
id: leetcode-0076-minimum-window-substring
title: "LeetCode 0076: Minimum Window Substring"
tags:
  - dsa
  - leetcode
  - sliding-window
  - hash-table
  - string
  - two-pointers
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/minimum-window-substring/"
---

# LeetCode 0076: Minimum Window Substring

## 1. Problem Formalization and Constraints

Given two strings `s` and `t` of lengths `m` and `n` respectively, return the minimum window substring of `s` such that every character in `t` (including duplicates) is included in the window.
If there is no such substring, return the empty string `""`.
The testcases will be generated such that the answer is unique.

### Constraints
- $m == \text{s.length}$
- $n == \text{t.length}$
- $1 \le m, n \le 10^5$
- `s` and `t` consist of uppercase and lowercase English letters.

### Examples
- **Example 1**:
  - Input: `s = "ADOBECODEBANC", t = "ABC"`
  - Output: `"BANC"`
  - Explanation: The minimum window substring `"BANC"` includes 'A', 'B', and 'C' from string `t`.
- **Example 2**:
  - Input: `s = "a", t = "a"`
  - Output: `"a"`
- **Example 3**:
  - Input: `s = "a", t = "aa"`
  - Output: `""`
  - Explanation: Both 'a's from `t` must be included in the window, so no valid substring exists.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Sliding Window with Scalar Match Counter | $O(M + N)$ | $O(1)$ | 128-element ASCII array with scalar `remain` counter avoids repeated frequency comparisons. |
| **Tier 2 (Space-Optimized / Filtered)** | Filtered-S Sliding Window | $O(M + N)$ | $O(M)$ | Filters $S$ to retain only indices belonging to $T$; ideal when $|T| \ll |S|$. |
| **Tier 3 (Time-Optimized Alternative)** | Binary Search on Window Length | $O(M \log M)$ | $O(1)$ | Binary searches candidate window size $L$; checks validity with fixed-size rolling window. |
| **Tier 4 (Brute Force)** | Exhaustive Substring Evaluation | $O(M^2 \times |\Sigma|)$ | $O(1)$ | Checks all $O(M^2)$ substrings and compares character frequencies against $T$. |

---

## 3. Tier 1: Most Optimal Solution (Sliding Window with Scalar Match Counter)

### 3.1 Algorithmic Mechanics and Invariant Proof

Instead of comparing all 128 ASCII frequencies between the window and string $t$ at every step, maintain a scalar counter `remain = len(t)` representing the number of required character occurrences still missing from the active window.
1. Populate frequency table `count` using characters in `t`.
2. Expand pointer `right` from $0$ to $M - 1$:
   - If `count[s[right]] > 0`, decrement `remain--`.
   - Decrement `count[s[right]]--`.
   (Negative values signify surplus occurrences inside the window).
3. Whenever `remain == 0`, a valid window is found:
   - Record the window if its length `right - left + 1` is smaller than the current minimum.
   - Shrink the window from the left: increment `count[s[left]]++`.
   - If `count[s[left]] > 0`, the removed character was essential, so increment `remain++`.
   - Advance `left++`.

**Invariant Proof**:
The scalar `remain` drops to $0$ if and only if every character in $t$ is covered with sufficient frequency by the window $s[left \dots right]$.
Because `left` and `right` advance monotonically from $0$ to $M$, each pointer executes at most $M$ steps, guaranteeing strict $O(M + N)$ runtime.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(M + N)$. Linear pass to tally $T$ ($N$ steps) and at most $2M$ pointer increments over $S$.
- **Space Complexity**: $O(1)$ auxiliary space. Uses a fixed 128-element integer array.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <climits>

class Solution {
public:
    std::string minWindow(std::string s, std::string t) {
        if (s.length() < t.length()) return "";

        int count[128] = {0};
        for (char c : t) ++count[static_cast<unsigned char>(c)];

        int remain = static_cast<int>(t.length());
        int left = 0;
        int min_start = 0;
        int min_len = INT_MAX;

        for (int right = 0; right < static_cast<int>(s.length()); ++right) {
            unsigned char r_char = s[right];
            if (count[r_char] > 0) {
                --remain;
            }
            --count[r_char];

            while (remain == 0) {
                if (right - left + 1 < min_len) {
                    min_len = right - left + 1;
                    min_start = left;
                }

                unsigned char l_char = s[left];
                ++count[l_char];
                if (count[l_char] > 0) {
                    ++remain;
                }
                ++left;
            }
        }
        return min_len == INT_MAX ? "" : s.substr(min_start, min_len);
    }
};
```

#### Python 3.12
```python
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        if len(s) < len(t):
            return ""

        count = [0] * 128
        for ch in t:
            count[ord(ch)] += 1

        remain = len(t)
        left = 0
        min_start = 0
        min_len = float("inf")

        for right, ch in enumerate(s):
            r_ord = ord(ch)
            if count[r_ord] > 0:
                remain -= 1
            count[r_ord] -= 1

            while remain == 0:
                cur_len = right - left + 1
                if cur_len < min_len:
                    min_len = cur_len
                    min_start = left

                l_ord = ord(s[left])
                count[l_ord] += 1
                if count[l_ord] > 0:
                    remain += 1
                left += 1

        return "" if min_len == float("inf") else s[min_start : min_start + min_len]
```

#### Java 21
```java
class Solution {
    public String minWindow(String s, String t) {
        if (s == null || t == null || s.length() < t.length()) {
            return "";
        }

        int[] count = new int[128];
        for (char c : t.toCharArray()) {
            count[c]++;
        }

        int remain = t.length();
        int left = 0;
        int minStart = 0;
        int minLen = Integer.MAX_VALUE;

        for (int right = 0; right < s.length(); right++) {
            char rChar = s.charAt(right);
            if (count[rChar] > 0) {
                remain--;
            }
            count[rChar]--;

            while (remain == 0) {
                if (right - left + 1 < minLen) {
                    minLen = right - left + 1;
                    minStart = left;
                }

                char lChar = s.charAt(left);
                count[lChar]++;
                if (count[lChar] > 0) {
                    remain++;
                }
                left++;
            }
        }

        return minLen == Integer.MAX_VALUE ? "" : s.substring(minStart, minStart + minLen);
    }
}
```

#### TypeScript
```typescript
function minWindow(s: string, t: string): string {
    if (s.length < t.length) return "";

    const count = new Int32Array(128);
    for (let i = 0; i < t.length; i++) {
        count[t.charCodeAt(i)]++;
    }

    let remain = t.length;
    let left = 0;
    let minStart = 0;
    let minLen = Infinity;

    for (let right = 0; right < s.length; right++) {
        const rightChar = s.charCodeAt(right);
        if (count[rightChar] > 0) {
            remain--;
        }
        count[rightChar]--;

        while (remain === 0) {
            if (right - left + 1 < minLen) {
                minLen = right - left + 1;
                minStart = left;
            }

            const leftChar = s.charCodeAt(left);
            count[leftChar]++;
            if (count[leftChar] > 0) {
                remain++;
            }
            left++;
        }
    }

    return minLen === Infinity ? "" : s.substring(minStart, minStart + minLen);
}
```

#### Go
```go
package main

import "math"

func minWindow(s string, t string) string {
    if len(s) < len(t) {
        return ""
    }

    var count [128]int
    for i := 0; i < len(t); i++ {
        count[t[i]]++
    }

    remain := len(t)
    left := 0
    minStart := 0
    minLen := math.MaxInt32

    for right := 0; right < len(s); right++ {
        if count[s[right]] > 0 {
            remain--
        }
        count[s[right]]--

        for remain == 0 {
            if right-left+1 < minLen {
                minLen = right - left + 1
                minStart = left
            }

            count[s[left]]++
            if count[s[left]] > 0 {
                remain++
            }
            left++
        }
    }

    if minLen == math.MaxInt32 {
        return ""
    }
    return s[minStart : minStart+minLen]
}
```

#### Rust
```rust
impl Solution {
    pub fn min_window(s: String, t: String) -> String {
        if s.len() < t.len() {
            return String::new();
        }

        let s_bytes = s.as_bytes();
        let t_bytes = t.as_bytes();

        let mut count = [0i32; 128];
        for &b in t_bytes {
            count[b as usize] += 1;
        }

        let mut remain = t_bytes.len() as i32;
        let mut left = 0usize;
        let mut min_start = 0usize;
        let mut min_len = usize::MAX;

        for right in 0..s_bytes.len() {
            let r_char = s_bytes[right] as usize;
            if count[r_char] > 0 {
                remain -= 1;
            }
            count[r_char] -= 1;

            while remain == 0 {
                let win_len = right - left + 1;
                if win_len < min_len {
                    min_len = win_len;
                    min_start = left;
                }

                let l_char = s_bytes[left] as usize;
                count[l_char] += 1;
                if count[l_char] > 0 {
                    remain += 1;
                }
                left += 1;
            }
        }

        if min_len == usize::MAX {
            String::new()
        } else {
            s[min_start..min_start + min_len].to_string()
        }
    }
}
```

---

## 4. Tier 2: Space-Optimized / Filtered Solution (Filtered-S Sliding Window)

### 4.1 Algorithmic Mechanics and Invariant Proof

When $S$ is very long and contains sparse occurrences of characters in $T$, iterating over every character in $S$ causes needless pointer steps.
Create a list of pairs `filtered_s = [(index, char)]` for only those characters in $S$ that appear in $T$.
Run the sliding window exclusively over `filtered_s`, computing window lengths using the original indices stored in the pairs.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(M + N)$. Filters $S$ in $O(M)$ time and traverses `filtered_s` of size $K \le M$ in $O(K)$ steps.
- **Space Complexity**: $O(M)$ to store the filtered pairs.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>
#include <climits>

class Solution {
public:
    std::string minWindow(std::string s, std::string t) {
        if (s.length() < t.length()) return "";

        int count[128] = {0};
        for (char c : t) ++count[static_cast<unsigned char>(c)];

        std::vector<std::pair<int, char>> filtered;
        for (int i = 0; i < static_cast<int>(s.length()); ++i) {
            if (count[static_cast<unsigned char>(s[i])] > 0) {
                filtered.emplace_back(i, s[i]);
            }
        }

        int remain = static_cast<int>(t.length());
        int left = 0;
        int min_start = 0;
        int min_len = INT_MAX;

        for (int right = 0; right < static_cast<int>(filtered.size()); ++right) {
            char r_char = filtered[right].second;
            if (count[static_cast<unsigned char>(r_char)] > 0) --remain;
            --count[static_cast<unsigned char>(r_char)];

            while (remain == 0) {
                int start = filtered[left].first;
                int end = filtered[right].first;
                if (end - start + 1 < min_len) {
                    min_len = end - start + 1;
                    min_start = start;
                }

                char l_char = filtered[left].second;
                ++count[static_cast<unsigned char>(l_char)];
                if (count[static_cast<unsigned char>(l_char)] > 0) ++remain;
                ++left;
            }
        }
        return min_len == INT_MAX ? "" : s.substr(min_start, min_len);
    }
};
```

#### Python 3.12
```python
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        if len(s) < len(t):
            return ""

        count = [0] * 128
        for ch in t:
            count[ord(ch)] += 1

        filtered = [(i, ch) for i, ch in enumerate(s) if count[ord(ch)] > 0]

        remain = len(t)
        left = 0
        min_start = 0
        min_len = float("inf")

        for right, (r_idx, r_ch) in enumerate(filtered):
            if count[ord(r_ch)] > 0:
                remain -= 1
            count[ord(r_ch)] -= 1

            while remain == 0:
                l_idx, l_ch = filtered[left]
                cur_len = r_idx - l_idx + 1
                if cur_len < min_len:
                    min_len = cur_len
                    min_start = l_idx

                count[ord(l_ch)] += 1
                if count[ord(l_ch)] > 0:
                    remain += 1
                left += 1

        return "" if min_len == float("inf") else s[min_start : min_start + min_len]
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

class Solution {
    public String minWindow(String s, String t) {
        if (s.length() < t.length()) return "";

        int[] count = new int[128];
        for (char c : t.toCharArray()) count[c]++;

        List<int[]> filtered = new ArrayList<>();
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if (count[c] > 0) filtered.add(new int[]{i, c});
        }

        int remain = t.length();
        int left = 0;
        int minStart = 0;
        int minLen = Integer.MAX_VALUE;

        for (int right = 0; right < filtered.size(); right++) {
            char rChar = (char) filtered.get(right)[1];
            if (count[rChar] > 0) remain--;
            count[rChar]--;

            while (remain == 0) {
                int start = filtered.get(left)[0];
                int end = filtered.get(right)[0];
                if (end - start + 1 < minLen) {
                    minLen = end - start + 1;
                    minStart = start;
                }

                char lChar = (char) filtered.get(left)[1];
                count[lChar]++;
                if (count[lChar] > 0) remain++;
                left++;
            }
        }

        return minLen == Integer.MAX_VALUE ? "" : s.substring(minStart, minStart + minLen);
    }
}
```

#### TypeScript
```typescript
function minWindow(s: string, t: string): string {
    if (s.length < t.length) return "";

    const count = new Int32Array(128);
    for (let i = 0; i < t.length; i++) count[t.charCodeAt(i)]++;

    const filtered: [number, number][] = [];
    for (let i = 0; i < s.length; i++) {
        const code = s.charCodeAt(i);
        if (count[code] > 0) filtered.push([i, code]);
    }

    let remain = t.length;
    let left = 0;
    let minStart = 0;
    let minLen = Infinity;

    for (let right = 0; right < filtered.length; right++) {
        const [, rCode] = filtered[right];
        if (count[rCode] > 0) remain--;
        count[rCode]--;

        while (remain === 0) {
            const [start] = filtered[left];
            const [end] = filtered[right];
            if (end - start + 1 < minLen) {
                minLen = end - start + 1;
                minStart = start;
            }

            const [, lCode] = filtered[left];
            count[lCode]++;
            if (count[lCode] > 0) remain++;
            left++;
        }
    }

    return minLen === Infinity ? "" : s.substring(minStart, minStart + minLen);
}
```

#### Go
```go
package main

import "math"

func minWindow(s string, t string) string {
    if len(s) < len(t) {
        return ""
    }

    var count [128]int
    for i := 0; i < len(t); i++ {
        count[t[i]]++
    }

    type entry struct {
        idx int
        ch  byte
    }
    var filtered []entry
    for i := 0; i < len(s); i++ {
        if count[s[i]] > 0 {
            filtered = append(filtered, entry{idx: i, ch: s[i]})
        }
    }

    remain := len(t)
    left := 0
    minStart := 0
    minLen := math.MaxInt32

    for right := 0; right < len(filtered); right++ {
        rChar := filtered[right].ch
        if count[rChar] > 0 {
            remain--
        }
        count[rChar]--

        for remain == 0 {
            start := filtered[left].idx
            end := filtered[right].idx
            if end-start+1 < minLen {
                minLen = end - start + 1
                minStart = start
            }

            lChar := filtered[left].ch
            count[lChar]++
            if count[lChar] > 0 {
                remain++
            }
            left++
        }
    }

    if minLen == math.MaxInt32 {
        return ""
    }
    return s[minStart : minStart+minLen]
}
```

#### Rust
```rust
impl Solution {
    pub fn min_window(s: String, t: String) -> String {
        if s.len() < t.len() {
            return String::new();
        }

        let mut count = [0i32; 128];
        for &b in t.as_bytes() {
            count[b as usize] += 1;
        }

        let filtered: Vec<(usize, u8)> = s
            .as_bytes()
            .iter()
            .enumerate()
            .filter(|(_, &b)| count[b as usize] > 0)
            .map(|(i, &b)| (i, b))
            .collect();

        let mut remain = t.len() as i32;
        let mut left = 0usize;
        let mut min_start = 0usize;
        let mut min_len = usize::MAX;

        for right in 0..filtered.len() {
            let (_, r_ch) = filtered[right];
            if count[r_ch as usize] > 0 {
                remain -= 1;
            }
            count[r_ch as usize] -= 1;

            while remain == 0 {
                let (start, _) = filtered[left];
                let (end, _) = filtered[right];
                let cur_len = end - start + 1;
                if cur_len < min_len {
                    min_len = cur_len;
                    min_start = start;
                }

                let (_, l_ch) = filtered[left];
                count[l_ch as usize] += 1;
                if count[l_ch as usize] > 0 {
                    remain += 1;
                }
                left += 1;
            }
        }

        if min_len == usize::MAX {
            String::new()
        } else {
            s[min_start..min_start + min_len].to_string()
        }
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Binary Search on Window Length)

### 5.1 Algorithmic Mechanics and Invariant Proof

Binary search over the window length $L \in [|T|, |S|]$.
For a fixed length $L$, test if there exists any valid window of size $L$ in $S$ using a fixed-size sliding window:
- Slide a window of length $L$ across $S$ from index $0$ to $M - L$.
- If a valid window exists for size $L$, search smaller lengths ($high = mid - 1$).
- Otherwise, search larger lengths ($low = mid + 1$).

### 5.2 Complexity Analysis
- **Time Complexity**: $O(M \log M)$. The binary search executes $O(\log M)$ iterations, and each rolling window check runs in $O(M)$ time.
- **Space Complexity**: $O(1)$ auxiliary space for ASCII counter arrays.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>

class Solution {
public:
    std::string minWindow(std::string s, std::string t) {
        if (s.length() < t.length()) return "";

        int t_count[128] = {0};
        int unique_t = 0;
        for (char c : t) {
            if (++t_count[static_cast<unsigned char>(c)] == 1) ++unique_t;
        }

        auto isValid = [&](int len, int& best_start) {
            int win_count[128] = {0};
            int matches = 0;

            for (int i = 0; i < len; ++i) {
                unsigned char c = s[i];
                if (++win_count[c] == t_count[c] && t_count[c] > 0) ++matches;
            }
            if (matches == unique_t) {
                best_start = 0;
                return true;
            }

            for (int i = len; i < static_cast<int>(s.length()); ++i) {
                unsigned char in_c = s[i];
                if (++win_count[in_c] == t_count[in_c] && t_count[in_c] > 0) ++matches;

                unsigned char out_c = s[i - len];
                if (win_count[out_c] == t_count[out_c] && t_count[out_c] > 0) --matches;
                --win_count[out_c];

                if (matches == unique_t) {
                    best_start = i - len + 1;
                    return true;
                }
            }
            return false;
        };

        int low = static_cast<int>(t.length());
        int high = static_cast<int>(s.length());
        int best_len = -1, best_start = -1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            int start = -1;
            if (isValid(mid, start)) {
                best_len = mid;
                best_start = start;
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        return best_len == -1 ? "" : s.substr(best_start, best_len);
    }
};
```

#### Python 3.12
```python
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        if len(s) < len(t):
            return ""

        t_count = [0] * 128
        unique_t = 0
        for ch in t:
            t_count[ord(ch)] += 1
            if t_count[ord(ch)] == 1:
                unique_t += 1

        def is_valid(length: int) -> int:
            win_count = [0] * 128
            matches = 0
            for i in range(length):
                c = ord(s[i])
                win_count[c] += 1
                if t_count[c] > 0 and win_count[c] == t_count[c]:
                    matches += 1
            if matches == unique_t:
                return 0

            for i in range(length, len(s)):
                in_c = ord(s[i])
                win_count[in_c] += 1
                if t_count[in_c] > 0 and win_count[in_c] == t_count[in_c]:
                    matches += 1

                out_c = ord(s[i - length])
                if t_count[out_c] > 0 and win_count[out_c] == t_count[out_c]:
                    matches -= 1
                win_count[out_c] -= 1

                if matches == unique_t:
                    return i - length + 1
            return -1

        low = len(t)
        high = len(s)
        best_len = -1
        best_start = -1

        while low <= high:
            mid = (low + high) // 2
            start = is_valid(mid)
            if start != -1:
                best_len = mid
                best_start = start
                high = mid - 1
            else:
                low = mid + 1

        return "" if best_len == -1 else s[best_start : best_start + best_len]
```

#### Java 21
```java
class Solution {
    public String minWindow(String s, String t) {
        if (s.length() < t.length()) return "";

        int[] tCount = new int[128];
        int uniqueT = 0;
        for (char c : t.toCharArray()) {
            if (++tCount[c] == 1) uniqueT++;
        }

        int low = t.length(), high = s.length();
        int bestLen = -1, bestStart = -1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            int start = check(s, mid, tCount, uniqueT);
            if (start != -1) {
                bestLen = mid;
                bestStart = start;
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        return bestLen == -1 ? "" : s.substring(bestStart, bestStart + bestLen);
    }

    private int check(String s, int len, int[] tCount, int uniqueT) {
        int[] winCount = new int[128];
        int matches = 0;

        for (int i = 0; i < len; i++) {
            char c = s.charAt(i);
            if (++winCount[c] == tCount[c] && tCount[c] > 0) matches++;
        }
        if (matches == uniqueT) return 0;

        for (int i = len; i < s.length(); i++) {
            char inC = s.charAt(i);
            if (++winCount[inC] == tCount[inC] && tCount[inC] > 0) matches++;

            char outC = s.charAt(i - len);
            if (winCount[outC] == tCount[outC] && tCount[outC] > 0) matches--;
            winCount[outC]--;

            if (matches == uniqueT) return i - len + 1;
        }
        return -1;
    }
}
```

#### TypeScript
```typescript
function minWindow(s: string, t: string): string {
    if (s.length < t.length) return "";

    const tCount = new Int32Array(128);
    let uniqueT = 0;
    for (let i = 0; i < t.length; i++) {
        const c = t.charCodeAt(i);
        if (++tCount[c] === 1) uniqueT++;
    }

    function check(len: number): number {
        const winCount = new Int32Array(128);
        let matches = 0;

        for (let i = 0; i < len; i++) {
            const c = s.charCodeAt(i);
            if (++winCount[c] === tCount[c] && tCount[c] > 0) matches++;
        }
        if (matches === uniqueT) return 0;

        for (let i = len; i < s.length; i++) {
            const inC = s.charCodeAt(i);
            if (++winCount[inC] === tCount[inC] && tCount[inC] > 0) matches++;

            const outC = s.charCodeAt(i - len);
            if (winCount[outC] === tCount[outC] && tCount[outC] > 0) matches--;
            winCount[outC]--;

            if (matches === uniqueT) return i - len + 1;
        }
        return -1;
    }

    let low = t.length, high = s.length;
    let bestLen = -1, bestStart = -1;

    while (low <= high) {
        const mid = (low + high) >> 1;
        const start = check(mid);
        if (start !== -1) {
            bestLen = mid;
            bestStart = start;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    return bestLen === -1 ? "" : s.substring(bestStart, bestStart + bestLen);
}
```

#### Go
```go
package main

func minWindow(s string, t string) string {
    if len(s) < len(t) {
        return ""
    }

    var tCount [128]int
    uniqueT := 0
    for i := 0; i < len(t); i++ {
        tCount[t[i]]++
        if tCount[t[i]] == 1 {
            uniqueT++
        }
    }

    check := func(length int) int {
        var winCount [128]int
        matches := 0

        for i := 0; i < length; i++ {
            c := s[i]
            winCount[c]++
            if tCount[c] > 0 && winCount[c] == tCount[c] {
                matches++
            }
        }
        if matches == uniqueT {
            return 0
        }

        for i := length; i < len(s); i++ {
            inC := s[i]
            winCount[inC]++
            if tCount[inC] > 0 && winCount[inC] == tCount[inC] {
                matches++
            }

            outC := s[i-length]
            if tCount[outC] > 0 && winCount[outC] == tCount[outC] {
                matches--
            }
            winCount[outC]--

            if matches == uniqueT {
                return i - length + 1
            }
        }
        return -1
    }

    low, high := len(t), len(s)
    bestLen, bestStart := -1, -1

    for low <= high {
        mid := (low + high) / 2
        start := check(mid)
        if start != -1 {
            bestLen = mid
            bestStart = start
            high = mid - 1
        } else {
            low = mid + 1
        }
    }

    if bestLen == -1 {
        return ""
    }
    return s[bestStart : bestStart+bestLen]
}
```

#### Rust
```rust
impl Solution {
    pub fn min_window(s: String, t: String) -> String {
        if s.len() < t.len() {
            return String::new();
        }

        let s_bytes = s.as_bytes();
        let t_bytes = t.as_bytes();

        let mut t_count = [0i32; 128];
        let mut unique_t = 0;
        for &b in t_bytes {
            t_count[b as usize] += 1;
            if t_count[b as usize] == 1 {
                unique_t += 1;
            }
        }

        let check = |len: usize| -> Option<usize> {
            let mut win_count = [0i32; 128];
            let mut matches = 0;

            for i in 0..len {
                let c = s_bytes[i] as usize;
                win_count[c] += 1;
                if t_count[c] > 0 && win_count[c] == t_count[c] {
                    matches += 1;
                }
            }
            if matches == unique_t {
                return Some(0);
            }

            for i in len..s_bytes.len() {
                let in_c = s_bytes[i] as usize;
                win_count[in_c] += 1;
                if t_count[in_c] > 0 && win_count[in_c] == t_count[in_c] {
                    matches += 1;
                }

                let out_c = s_bytes[i - len] as usize;
                if t_count[out_c] > 0 && win_count[out_c] == t_count[out_c] {
                    matches -= 1;
                }
                win_count[out_c] -= 1;

                if matches == unique_t {
                    return Some(i - len + 1);
                }
            }
            None
        };

        let mut low = t_bytes.len();
        let mut high = s_bytes.len();
        let mut best_len = None;
        let mut best_start = 0;

        while low <= high {
            let mid = low + (high - low) / 2;
            if let Some(start) = check(mid) {
                best_len = Some(mid);
                best_start = start;
                if mid == 0 {
                    break;
                }
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        match best_len {
            Some(len) => s[best_start..best_start + len].to_string(),
            None => String::new(),
        }
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Exhaustive Substring Evaluation)

### 6.1 Algorithmic Mechanics and Invariant Proof

Examine every possible substring $s[i \dots j]$ with $j - i + 1 \ge |T|$.
For each substring, count character occurrences and verify whether each character in $T$ has equal or greater representation in the substring.
Return the shortest valid substring.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(M^2 \times |\Sigma|)$. Evaluates $O(M^2)$ substrings, spending $O(128)$ per evaluation.
- **Space Complexity**: $O(1)$ auxiliary space for character tables.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <vector>

class Solution {
public:
    std::string minWindow(std::string s, std::string t) {
        int m = static_cast<int>(s.length());
        int n = static_cast<int>(t.length());
        if (m < n) return "";

        int t_cnt[128] = {0};
        for (char c : t) ++t_cnt[static_cast<unsigned char>(c)];

        std::string best = "";

        for (int i = 0; i < m; ++i) {
            int sub_cnt[128] = {0};
            for (int j = i; j < m; ++j) {
                ++sub_cnt[static_cast<unsigned char>(s[j])];
                if (j - i + 1 < n) continue;

                bool valid = true;
                for (int k = 0; k < 128; ++k) {
                    if (sub_cnt[k] < t_cnt[k]) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    if (best.empty() || (j - i + 1) < static_cast<int>(best.length())) {
                        best = s.substr(i, j - i + 1);
                    }
                    break; // Smaller window with same start cannot be found further right
                }
            }
        }
        return best;
    }
};
```

#### Python 3.12
```python
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        m, n = len(s), len(t)
        if m < n:
            return ""

        t_cnt = [0] * 128
        for ch in t:
            t_cnt[ord(ch)] += 1

        best = ""

        for i in range(m):
            sub_cnt = [0] * 128
            for j in range(i, m):
                sub_cnt[ord(s[j])] += 1
                if j - i + 1 < n:
                    continue

                valid = True
                for k in range(128):
                    if sub_cnt[k] < t_cnt[k]:
                        valid = False
                        break
                if valid:
                    if not best or (j - i + 1) < len(best):
                        best = s[i : j + 1]
                    break

        return best
```

#### Java 21
```java
class Solution {
    public String minWindow(String s, String t) {
        int m = s.length(), n = t.length();
        if (m < n) return "";

        int[] tCnt = new int[128];
        for (char c : t.toCharArray()) tCnt[c]++;

        String best = "";

        for (int i = 0; i < m; i++) {
            int[] subCnt = new int[128];
            for (int j = i; j < m; j++) {
                subCnt[s.charAt(j)]++;
                if (j - i + 1 < n) continue;

                boolean valid = true;
                for (int k = 0; k < 128; k++) {
                    if (subCnt[k] < tCnt[k]) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    if (best.isEmpty() || (j - i + 1) < best.length()) {
                        best = s.substring(i, j + 1);
                    }
                    break;
                }
            }
        }
        return best;
    }
}
```

#### TypeScript
```typescript
function minWindow(s: string, t: string): string {
    const m = s.length, n = t.length;
    if (m < n) return "";

    const tCnt = new Int32Array(128);
    for (let i = 0; i < n; i++) tCnt[t.charCodeAt(i)]++;

    let best = "";

    for (let i = 0; i < m; i++) {
        const subCnt = new Int32Array(128);
        for (let j = i; j < m; j++) {
            subCnt[s.charCodeAt(j)]++;
            if (j - i + 1 < n) continue;

            let valid = true;
            for (let k = 0; k < 128; k++) {
                if (subCnt[k] < tCnt[k]) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                if (best === "" || (j - i + 1) < best.length) {
                    best = s.substring(i, j + 1);
                }
                break;
            }
        }
    }
    return best;
}
```

#### Go
```go
package main

func minWindow(s string, t string) string {
    m, n := len(s), len(t)
    if m < n {
        return ""
    }

    var tCnt [128]int
    for i := 0; i < n; i++ {
        tCnt[t[i]]++
    }

    best := ""

    for i := 0; i < m; i++ {
        var subCnt [128]int
        for j := i; j < m; j++ {
            subCnt[s[j]]++
            if j-i+1 < n {
                continue
            }

            valid := true
            for k := 0; k < 128; k++ {
                if subCnt[k] < tCnt[k] {
                    valid = false
                    break
                }
            }
            if valid {
                if best == "" || (j-i+1) < len(best) {
                    best = s[i : j+1]
                }
                break
            }
        }
    }
    return best
}
```

#### Rust
```rust
impl Solution {
    pub fn min_window(s: String, t: String) -> String {
        let m = s.len();
        let n = t.len();
        if m < n {
            return String::new();
        }

        let s_bytes = s.as_bytes();
        let mut t_cnt = [0i32; 128];
        for &b in t.as_bytes() {
            t_cnt[b as usize] += 1;
        }

        let mut best: Option<&str> = None;

        for i in 0..m {
            let mut sub_cnt = [0i32; 128];
            for j in i..m {
                sub_cnt[s_bytes[j] as usize] += 1;
                if j - i + 1 < n {
                    continue;
                }

                let mut valid = true;
                for k in 0..128 {
                    if sub_cnt[k] < t_cnt[k] {
                        valid = false;
                        break;
                    }
                }
                if valid {
                    let candidate = &s[i..=j];
                    if best.map_or(true, |b| candidate.len() < b.len()) {
                        best = Some(candidate);
                    }
                    break;
                }
            }
        }

        best.unwrap_or("").to_string()
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why can frequency array values drop below zero in the optimal sliding window?</summary>
Negative values indicate duplicate or surplus occurrences of a character beyond what is strictly required by $T$.
When shrinking `left`, a surplus character can be evicted without invalidating the window until its count reaches positive values again.
</details>

<details>
<summary>2. What occurs when string $S$ is shorter than string $T$ ($|S| < |T|$)?</summary>
It is impossible for any substring of $S$ to contain all characters of $T$.
The initial guard condition `if (s.length() < t.length()) return ""` returns immediately in $O(1)$ time.
</details>

<details>
<summary>3. Why is the 128-element fixed array preferable to `std::unordered_map`?</summary>
128 integers fit inside 512 bytes, residing comfortably in the CPU L1 data cache.
A hash map incurs dynamic heap allocation, hash hashing overhead, and pointer indirection, resulting in a significantly slower constant factor.
</details>

<details>
<summary>4. When does the Filtered-S approach significantly outperform the raw sliding window?</summary>
When $|S| = 10^5$ but $|T| = 3$ and only 50 characters in $S$ match characters in $T$.
Filtering skips $99.95\%$ of unnecessary pointer increments.
</details>

<details>
<summary>5. How does the scalar counter `remain` maintain consistency?</summary>
`remain` counts only the positive transitions: it decrements when `count[r] > 0` before decrementing, and increments when `count[l] > 0` after incrementing.
This cleanly ignores surplus characters.
</details>

<details>
<summary>6. What happens when $S = \text{"a"}$ and $T = \text{"aa"}$?</summary>
`remain` starts at 2.
Encountering the single `'a'` decrements `remain` to 1.
The loop terminates without `remain` ever reaching 0, returning `""`.
</details>

<details>
<summary>7. Why does the binary search approach work even though window validity is monotonic?</summary>
If there is a valid window of size $L$, any window of size $L' > L$ containing that window is also valid.
This monotonic property enables binary search over length $L$.
</details>

<details>
<summary>8. How do you return the minimal substring when multiple valid windows have identical length?</summary>
The problem constraints guarantee that the test cases generate a unique minimum answer.
If multiple identical-length windows existed, returning the first encountered window is standard.
</details>

<details>
<summary>9. What is the worst-case time complexity of the brute force method?</summary>
$O(M^2 \times |\Sigma|)$, which for $M = 10^5$ results in $10^{10} \times 128 \approx 10^{12}$ operations, guaranteeing Time Limit Exceeded.
</details>

<details>
<summary>10. What is the ASCII range needed to cover both uppercase and lowercase English letters?</summary>
`'A'` is 65 and `'z'` is 122.
An array size of 128 safely covers all ASCII indices from 0 to 127 without out-of-bounds access.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/minimum-window-substring.cpp)
- [Python Implementation](../Python/minimum-window-substring.py)
- [Java Implementation](../Java/minimum-window-substring.java)
- [TypeScript Implementation](../TypeScript/minimum-window-substring.ts)
- [Go Implementation](../Golang/minimum-window-substring.go)
- [Rust Implementation](../Rust/minimum-window-substring.rs)

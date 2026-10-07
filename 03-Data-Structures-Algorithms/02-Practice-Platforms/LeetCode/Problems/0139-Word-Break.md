---
id: leetcode-0139-word-break
title: "LeetCode 0139: Word Break"
tags:
  - dsa
  - leetcode
  - dynamic-programming
  - string
  - trie
  - hash-table
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/word-break/"
---

# LeetCode 0139: Word Break

## 1. Problem Formalization and Constraints

Given a string `s` and a dictionary of strings `wordDict`, return `true` if `s` can be segmented into a space-separated sequence of one or more dictionary words.
The same word in the dictionary may be reused multiple times in the segmentation.

### Constraints
- $1 \le \text{s.length} \le 300$
- $1 \le \text{wordDict.length} \le 1000$
- $1 \le \text{wordDict}[i]\text{.length} \le 20$
- `s` and `wordDict}[i]` consist of only lowercase English letters.
- All the strings of `wordDict` are unique.

### Examples
- **Example 1**:
  - Input: `s = "leetcode", wordDict = ["leet","code"]`
  - Output: `true`
  - Explanation: Return true because "leetcode" can be segmented as "leet code".
- **Example 2**:
  - Input: `s = "applepenapple", wordDict = ["apple","pen"]`
  - Output: `true`
  - Explanation: Return true because "applepenapple" can be segmented as "apple pen apple". Note that dictionary words may be reused.
- **Example 3**:
  - Input: `s = "catsandog", wordDict = ["cats","dog","sand","and","cat"]`
  - Output: `false`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | DP with Max-Length Window Pruning | $O(N \times L_{\max})$ | $O(N + M)$ | `dp[i]` indicates whether prefix `s[0..i-1]` can be segmented; scans backward at most $L_{\max}$ steps. |
| **Tier 2 (Trie DP)** | Trie Automaton Traversal with DP | $O(N \times L_{\max})$ | $O(\Sigma \cdot M)$ | Encodes `wordDict` into a Prefix Trie; steps forward from each valid `dp[i] == true` index. |
| **Tier 3 (BFS Graph)** | Breadth-First Search on String Indices | $O(N^2)$ | $O(N)$ | Models indices $0 \dots N$ as directed graph nodes; enqueues reachable partition boundaries. |
| **Tier 4 (Brute Force)** | Unmemoized Recursive Backtracking | $O(2^N)$ | $O(N)$ | Branches recursively on every matching dictionary prefix without caching repeated subproblems. |

---

## 3. Tier 1: Most Optimal Solution (DP with Max-Length Window Pruning)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $N = |s|$ and let $L_{\max} = \max_{w \in \text{wordDict}} |w|$.
Define a boolean table `dp` of size $N + 1$, where:
$$\text{dp}[i] = \text{true} \iff s[0 \dots i-1] \text{ can be validly segmented into dictionary words.}$$

Base Case:
$\text{dp}[0] = \text{true}$, since an empty prefix requires zero dictionary words.

Transition:
For each prefix length $i$ from $1$ to $N$:
$$\text{dp}[i] = \bigvee_{j = \max(0, i - L_{\max})}^{i - 1} (\text{dp}[j] \land (s[j \dots i - 1] \in \text{wordDict}))$$

**Optimization**:
Because no word in the dictionary has length greater than $L_{\max}$, the inner loop only needs to check starting split points $j \ge i - L_{\max}$.
Scanning $j$ backward from $i - 1$ down to $\max(0, i - L_{\max})$ allows early break the moment $\text{dp}[i]$ becomes `true`.

**Invariant Proof**:
Assume $\text{dp}[k]$ is correctly computed for all $k < i$.
If $\text{dp}[i]$ is set to `true`, there exists some split index $j < i$ such that $s[0 \dots j-1]$ is segmentable (by inductive hypothesis $\text{dp}[j] = \text{true}$) and $s[j \dots i-1]$ is an exact dictionary word.
Their concatenation forms $s[0 \dots i-1]$.
Conversely, if $s[0 \dots i-1]$ is segmentable, its final constituent word must start at some valid index $j \ge i - L_{\max}$ with $\text{dp}[j] = \text{true}$.
The inner loop tests all such candidate splits, ensuring no valid transition is missed.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \times L_{\max})$. The outer loop runs $N$ times. For each $i$, the inner loop executes at most $L_{\max}$ substring extractions and hash set lookups. With constraints $N \le 300$ and $L_{\max} \le 20$, total operations are bounded by $300 \times 20 = 6000$ operations.
- **Auxiliary Space Complexity**: $O(N + M)$, where $N$ is the DP table length and $M$ is the cumulative length of all strings stored in the hash set.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <string>
#include <unordered_set>
#include <string_view>
#include <algorithm>

class Solution {
public:
    bool wordBreak(std::string s, std::vector<std::string>& wordDict) {
        std::unordered_set<std::string_view> dict;
        size_t maxLen = 0;
        for (const auto& w : wordDict) {
            dict.insert(w);
            maxLen = std::max(maxLen, w.size());
        }

        size_t n = s.size();
        std::vector<bool> dp(n + 1, false);
        dp[0] = true;

        std::string_view sv(s);

        for (size_t i = 1; i <= n; ++i) {
            size_t start = (i > maxLen) ? (i - maxLen) : 0;
            for (size_t j = i; j > start; --j) {
                if (dp[j - 1] && dict.count(sv.substr(j - 1, i - (j - 1)))) {
                    dp[i] = true;
                    break;
                }
            }
        }

        return dp[n];
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def wordBreak(self, s: str, wordDict: List[str]) -> bool:
        word_set = set(wordDict)
        max_len = max(len(w) for w in wordDict) if wordDict else 0

        n = len(s)
        dp = [False] * (n + 1)
        dp[0] = True

        for i in range(1, n + 1):
            start = max(0, i - max_len)
            for j in range(i - 1, start - 1, -1):
                if dp[j] and s[j:i] in word_set:
                    dp[i] = True
                    break

        return dp[n]
```

#### Java 21
```java
import java.util.HashSet;
import java.util.List;
import java.util.Set;

class Solution {
    public boolean wordBreak(String s, List<String> wordDict) {
        Set<String> dict = new HashSet<>(wordDict);
        int maxLen = 0;
        for (String w : wordDict) {
            maxLen = Math.max(maxLen, w.length());
        }

        int n = s.length();
        boolean[] dp = new boolean[n + 1];
        dp[0] = true;

        for (int i = 1; i <= n; i++) {
            int start = Math.max(0, i - maxLen);
            for (int j = i - 1; j >= start; j--) {
                if (dp[j] && dict.contains(s.substring(j, i))) {
                    dp[i] = true;
                    break;
                }
            }
        }

        return dp[n];
    }
}
```

#### TypeScript 5
```typescript
function wordBreak(s: string, wordDict: string[]): boolean {
    const dict = new Set<string>(wordDict);
    let maxLen = 0;
    for (const w of wordDict) {
        maxLen = Math.max(maxLen, w.length);
    }

    const n = s.length;
    const dp = new Array<boolean>(n + 1).fill(false);
    dp[0] = true;

    for (let i = 1; i <= n; i++) {
        const start = Math.max(0, i - maxLen);
        for (let j = i - 1; j >= start; j--) {
            if (dp[j] && dict.has(s.substring(j, i))) {
                dp[i] = true;
                break;
            }
        }
    }

    return dp[n];
}
```

#### Go 1.22
```go
package main

func wordBreak(s string, wordDict []string) bool {
	dict := make(map[string]bool, len(wordDict))
	maxLen := 0
	for _, w := range wordDict {
		dict[w] = true
		if len(w) > maxLen {
			maxLen = len(w)
		}
	}

	n := len(s)
	dp := make([]bool, n+1)
	dp[0] = true

	for i := 1; i <= n; i++ {
		start := i - maxLen
		if start < 0 {
			start = 0
		}
		for j := i - 1; j >= start; j-- {
			if dp[j] && dict[s[j:i]] {
				dp[i] = true
				break
			}
		}
	}

	return dp[n]
}
```

#### Rust 2021
```rust
use std::collections::HashSet;

impl Solution {
    pub fn word_break(s: String, word_dict: Vec<String>) -> bool {
        let max_len = word_dict.iter().map(|w| w.len()).max().unwrap_or(0);
        let dict: HashSet<&str> = word_dict.iter().map(|w| w.as_str()).collect();

        let n = s.len();
        let mut dp = vec![false; n + 1];
        dp[0] = true;

        for i in 1..=n {
            let start = i.saturating_sub(max_len);
            for j in (start..i).rev() {
                if dp[j] && dict.contains(&s[j..i]) {
                    dp[i] = true;
                    break;
                }
            }
        }

        dp[n]
    }
}
```

---

## 4. Tier 2: Space/Time Optimized Trie DP

### 4.1 Algorithmic Mechanics
Instead of string hashing and repeated substring allocations, insert all words into a Prefix Trie.
For every index $i$ where $\text{dp}[i] == \text{true}$:
Follow character transitions in the Trie for characters $s[i], s[i + 1], \dots, s[k - 1]$.
Whenever a Trie node marks a terminal word, set $\text{dp}[k] = \text{true}$.
If a character mismatch occurs in the Trie, branch exploration halts immediately in $O(1)$ operations without computing hash values.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \times L_{\max} + \text{TrieBuildTime})$.
- **Space Complexity**: $O(\Sigma \times M + N)$ for Trie nodes and the boolean array.

### 4.3 Implementation (C++20)
```cpp
#include <vector>
#include <string>

struct TrieNode {
    bool isWord = false;
    TrieNode* children[26] = {nullptr};
};

class Solution {
public:
    bool wordBreak(std::string s, std::vector<std::string>& wordDict) {
        TrieNode* root = new TrieNode();
        for (const auto& w : wordDict) {
            TrieNode* curr = root;
            for (char ch : w) {
                int idx = ch - 'a';
                if (!curr->children[idx]) {
                    curr->children[idx] = new TrieNode();
                }
                curr = curr->children[idx];
            }
            curr->isWord = true;
        }

        int n = static_cast<int>(s.size());
        std::vector<bool> dp(n + 1, false);
        dp[0] = true;

        for (int i = 0; i < n; ++i) {
            if (!dp[i]) continue;

            TrieNode* curr = root;
            for (int j = i; j < n; ++j) {
                int idx = s[j] - 'a';
                if (!curr->children[idx]) break;
                curr = curr->children[idx];
                if (curr->isWord) {
                    dp[j + 1] = true;
                }
            }
        }

        return dp[n];
    }
};
```

---

## 5. Tier 3: Breadth-First Search on String Indices

### 5.1 Algorithmic Mechanics
Model string segmentation as a directed graph path from vertex $0$ to vertex $N$:
- An edge exists from $u$ to $v$ if $s[u \dots v - 1] \in \text{wordDict}$.
- Enqueue index $0$.
- Maintain an array `visited` to avoid re-enqueuing already verified indices.
- Pop index `curr`: for every dictionary word matching prefix at `curr`, enqueue `curr + word.length()`.
- Return `true` if index $N$ is dequeued.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(N)$ auxiliary queue and visited array memory.

### 5.3 Implementation (C++20)
```cpp
#include <vector>
#include <string>
#include <unordered_set>
#include <queue>

class Solution {
public:
    bool wordBreak(std::string s, std::vector<std::string>& wordDict) {
        std::unordered_set<std::string> dict(wordDict.begin(), wordDict.end());
        int n = static_cast<int>(s.size());

        std::queue<int> q;
        std::vector<bool> visited(n, false);
        q.push(0);

        while (!q.empty()) {
            int start = q.front();
            q.pop();

            if (start == n) return true;
            if (visited[start]) continue;
            visited[start] = true;

            for (int end = start + 1; end <= n; ++end) {
                if (dict.count(s.substr(start, end - start))) {
                    q.push(end);
                }
            }
        }

        return false;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Recursive Backtracking Without Memoization)

### 6.1 Algorithmic Mechanics
Tests all prefixes of `s` against `wordDict`.
When a prefix matches, recurses on the suffix.
On adversarial inputs like $s = \text{"aaaaaab"}$ with $\text{wordDict} = [\text{"a"}, \text{"aa"}, \text{"aaa"}]$, this explores all $2^N$ split combinations, leading to time limit exceeded.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(2^N)$.
- **Space Complexity**: $O(N)$ recursion stack frames.

### 6.3 Implementation (Python 3)
```python
from typing import List

class Solution:
    def wordBreak(self, s: str, wordDict: List[str]) -> bool:
        word_set = set(wordDict)

        def can_break(start: int) -> bool:
            if start == len(s):
                return True
            for end in range(start + 1, len(s) + 1):
                if s[start:end] in word_set and can_break(end):
                    return True
            return False

        return can_break(0)
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why does bounding the inner loop by $L_{\max}$ improve complexity from $O(N^2)$ to $O(N \cdot L_{\max})$?</summary>
In the worst case without $L_{\max}$, every index $i$ checks all $j \in [0, i - 1]$, requiring $N^2 / 2$ checks.
Since no word exceeds length $L_{\max} = 20$, substrings longer than $20$ cannot exist in the dictionary, eliminating redundant checks.
</details>

<details>
<summary>2. Why is `dp[0]` initialized to `true`?</summary>
`dp[0] = true` acts as the inductive base case.
When a prefix $s[0 \dots i-1]$ matches a dictionary word directly, `dp[0] && dict.contains(s[0..i-1])` resolves to `true`.
</details>

<details>
<summary>3. What is the advantage of using `std::string_view` over `std::string::substr` in C++?</summary>
`std::string::substr` allocates and copies characters into a new heap buffer.
`std::string_view::substr` performs an $O(1)$ pointer and length slice without allocating heap memory.
</details>

<details>
<summary>4. How does the Trie-based DP approach eliminate string hashing overhead?</summary>
Instead of computing rolling hashes or full string hashes for every substring, the Trie navigates one character transition at a time, rejecting invalid branches immediately upon hitting a null child.
</details>

<details>
<summary>5. What happens when `wordDict` contains characters not present in `s`?</summary>
Extraneous dictionary words are stored in the hash set or Trie but never traversed during matching.
Pre-filtering dictionary words to only those containing valid alphabet characters provides marginal speedups.
</details>

<details>
<summary>6. Can this algorithm return the actual segmented sentences?</summary>
LeetCode 140 (Word Break II) requires returning all segmentations.
That requires backtracking combined with memoization (or DFS over the DP transition DAG) to construct sentences.
</details>

<details>
<summary>7. What occurs when `s` cannot be segmented because the last character has no matching word?</summary>
All `dp[j]` matching tests for $i = N$ will evaluate to `false`, leaving `dp[N] == false`, which correctly returns `false`.
</details>

<details>
<summary>8. Why is the visited array essential in the BFS implementation (Tier 3)?</summary>
Multiple prefixes can end at the exact same split index $k$.
Without the `visited` array, index $k$ would be enqueued multiple times, triggering exponential state explosion identical to unmemoized recursion.
</details>

<details>
<summary>9. What is the impact of character case sensitivity?</summary>
The problem constraints guarantee lowercase English letters only, permitting small fixed-size arrays of size 26 for Trie nodes.
</details>

<details>
<summary>10. How does memory layout impact DP table access speed?</summary>
A flat vector of booleans (`vector<bool>` or byte array) sits contiguously in the L1 data cache.
Checking `dp[j]` is an $O(1)$ cache hit.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/word-break.cpp)
- [Python Implementation](../Python/word-break.py)
- [Java Implementation](../Java/word-break.java)
- [TypeScript Implementation](../TypeScript/word-break.ts)
- [Go Implementation](../Golang/word-break.go)
- [Rust Implementation](../Rust/word-break.rs)

---
id: leetcode-0079-word-search
title: "LeetCode 0079: Word Search"
tags:
  - dsa
  - leetcode
  - backtracking
  - matrix
  - dfs
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/word-search/"
---

# LeetCode 0079: Word Search

## 1. Problem Formalization and Constraints

Given an $m \times n$ grid of characters `board` and a string `word`, return `true` if `word` exists in the grid.
The word can be constructed from letters of sequentially adjacent cells, where adjacent cells are horizontally or vertically neighboring.
The same letter cell may not be used more than once in a single word construction.

### Constraints
- $m == \text{board.length}$
- $n == \text{board}[i]\text{.length}$
- $1 \le m, n \le 6$
- $1 \le \text{word.length} \le 15$
- `board` and `word` consist of only lowercase and uppercase English letters.

### Examples
- **Example 1**:
  - Input: `board = [["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], word = "ABCCED"`
  - Output: `true`
- **Example 2**:
  - Input: `board = [["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], word = "SEE"`
  - Output: `true`
- **Example 3**:
  - Input: `board = [["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], word = "ABCB"`
  - Output: `false`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Frequency-Pruned In-Place Backtracking | $O(M \times N \times 3^L)$ | $O(L)$ | Character frequency validation, directional endpoint reversal, and in-place cell masking. |
| **Tier 2 (Space-Optimized)** | Standard In-Place DFS Backtracking | $O(M \times N \times 4^L)$ | $O(L)$ | Mutates board cell to a sentinel character during exploration and restores on return. |
| **Tier 3 (Time-Optimized Alternative)** | Explicit Visited Matrix DFS | $O(M \times N \times 4^L)$ | $O(M \times N + L)$ | Avoids modifying input board by tracking an explicit 2D boolean array. |
| **Tier 4 (Brute Force)** | Unpruned Recursive Path Generator | $O(M \times N \times 4^L)$ | $O(L)$ | Explores without pre-flight size or character frequency checks. |

---

## 3. Tier 1: Most Optimal Solution (Frequency-Pruned In-Place Backtracking)

### 3.1 Algorithmic Mechanics and Invariant Proof

The search begins by verifying two necessary conditions:
1. Grid capacity: $M \times N \ge L$, where $L = \text{word.length}$.
2. Character frequencies: The grid must contain at least as many occurrences of each character as required by `word`.
If any required character frequency is deficient, return `false` immediately without searching.

Next, check the frequency of the first letter `word[0]` versus the last letter `word[L - 1]`.
If the starting character appears more frequently in the grid than the terminal character, reverse `word`.
Reversing the search string reduces the branching factor at the root of the search tree, drastically pruning false recursive paths early.

During depth-first traversal from coordinate $(r, c)$ at character index `idx`:
- Base case: If `idx == L`, the entire word has been matched; return `true`.
- Boundary and match check: If $(r, c)$ is out of bounds or `board[r][c] != target[idx]`, return `false`.
- Masking: Temporarily overwrite `board[r][c]` with a non-alphabetic sentinel `'#'`.
- Branching: Recurse into all 4 cardinal neighbors with index `idx + 1`.
- Backtracking: Restore `board[r][c]` to its original character before returning.

**Invariant Proof**:
At depth $k$, all characters `target[0..k-1]` have been uniquely matched to distinct coordinates on the grid.
Overwriting `board[r][c] = '#'` guarantees that the current path cannot re-enter $(r, c)$, maintaining the simple path invariant.
The restoration step ensures complete isolation across sibling branches.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(M \times N \times 3^L)$ in the theoretical worst case. After the first step, each step branches in at most 3 directions because the previous cell is blocked. With frequency and endpoint pruning, average runtime is practically $O(M \times N + L)$.
- **Auxiliary Space Complexity**: $O(L)$, bounded by the call stack depth which never exceeds the length of `word`.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <string>
#include <algorithm>

class Solution {
public:
    bool exist(std::vector<std::vector<char>>& board, std::string word) {
        int m = static_cast<int>(board.size());
        int n = static_cast<int>(board[0].size());
        int len = static_cast<int>(word.size());
        if (m * n < len) return false;

        std::vector<int> freq(128, 0);
        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                freq[board[r][c]]++;
            }
        }

        for (char ch : word) {
            if (--freq[ch] < 0) return false;
        }

        // Endpoint frequency pruning
        std::string target = word;
        if (freq[word.front()] > freq[word.back()]) {
            std::reverse(target.begin(), target.end());
        }

        auto dfs = [&](auto self, int r, int c, int idx) -> bool {
            if (idx == len) return true;
            if (r < 0 || r >= m || c < 0 || c >= n || board[r][c] != target[idx]) {
                return false;
            }

            char temp = board[r][c];
            board[r][c] = '#';

            bool found = self(self, r + 1, c, idx + 1)
                      || self(self, r - 1, c, idx + 1)
                      || self(self, r, c + 1, idx + 1)
                      || self(self, r, c - 1, idx + 1);

            board[r][c] = temp;
            return found;
        };

        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (board[r][c] == target[0] && dfs(dfs, r, c, 0)) {
                    return true;
                }
            }
        }

        return false;
    }
};
```

#### Python 3
```python
from collections import Counter
from typing import List

class Solution:
    def exist(self, board: List[List[str]], word: str) -> bool:
        m, n = len(board), len(board[0])
        if m * n < len(word):
            return False

        board_counts = Counter(ch for row in board for ch in row)
        word_counts = Counter(word)
        for ch, count in word_counts.items():
            if board_counts[ch] < count:
                return False

        target = word
        if board_counts[word[0]] > board_counts[word[-1]]:
            target = word[::-1]

        def dfs(r: int, c: int, idx: int) -> bool:
            if idx == len(target):
                return True
            if r < 0 or r >= m or c < 0 or c >= n or board[r][c] != target[idx]:
                return False

            temp = board[r][c]
            board[r][c] = "#"

            found = (
                dfs(r + 1, c, idx + 1)
                or dfs(r - 1, c, idx + 1)
                or dfs(r, c + 1, idx + 1)
                or dfs(r, c - 1, idx + 1)
            )

            board[r][c] = temp
            return found

        for r in range(m):
            for c in range(n):
                if board[r][c] == target[0] and dfs(r, c, 0):
                    return True

        return False
```

#### Java 21
```java
class Solution {
    public boolean exist(char[][] board, String word) {
        int m = board.length;
        int n = board[0].length;
        int len = word.length();
        if (m * n < len) return false;

        int[] boardFreq = new int[128];
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                boardFreq[board[r][c]]++;
            }
        }
        for (int i = 0; i < len; i++) {
            if (--boardFreq[word.charAt(i)] < 0) return false;
        }

        String target = word;
        if (boardFreq[word.charAt(0)] > boardFreq[word.charAt(len - 1)]) {
            target = new StringBuilder(word).reverse().toString();
        }

        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (board[r][c] == target.charAt(0)) {
                    if (dfs(board, r, c, target, 0)) return true;
                }
            }
        }
        return false;
    }

    private boolean dfs(char[][] board, int r, int c, String word, int idx) {
        if (idx == word.length()) return true;
        if (r < 0 || r >= board.length || c < 0 || c >= board[0].length || board[r][c] != word.charAt(idx)) {
            return false;
        }

        char temp = board[r][c];
        board[r][c] = '#';

        boolean found = dfs(board, r + 1, c, word, idx + 1)
                     || dfs(board, r - 1, c, word, idx + 1)
                     || dfs(board, r, c + 1, word, idx + 1)
                     || dfs(board, r, c - 1, word, idx + 1);

        board[r][c] = temp;
        return found;
    }
}
```

#### TypeScript 5
```typescript
function exist(board: string[][], word: string): boolean {
    const m = board.length;
    const n = board[0].length;
    const len = word.length;
    if (m * n < len) return false;

    const boardFreq: Map<string, number> = new Map();
    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            boardFreq.set(board[r][c], (boardFreq.get(board[r][c]) || 0) + 1);
        }
    }

    for (const ch of word) {
        const count = boardFreq.get(ch) || 0;
        if (count === 0) return false;
        boardFreq.set(ch, count - 1);
    }

    let target = word;
    if ((boardFreq.get(word[0]) || 0) > (boardFreq.get(word[len - 1]) || 0)) {
        target = word.split('').reverse().join('');
    }

    function dfs(r: number, c: number, idx: number): boolean {
        if (idx === target.length) return true;
        if (r < 0 || r >= m || c < 0 || c >= n || board[r][c] !== target[idx]) {
            return false;
        }

        const temp = board[r][c];
        board[r][c] = '#';

        const found = dfs(r + 1, c, idx + 1)
                   || dfs(r - 1, c, idx + 1)
                   || dfs(r, c + 1, idx + 1)
                   || dfs(r, c - 1, idx + 1);

        board[r][c] = temp;
        return found;
    }

    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (board[r][c] === target[0] && dfs(r, c, 0)) {
                return true;
            }
        }
    }

    return false;
}
```

#### Go 1.22
```go
package main

func exist(board [][]byte, word string) bool {
	m, n, wordLen := len(board), len(board[0]), len(word)
	if m*n < wordLen {
		return false
	}

	boardFreq := make([]int, 128)
	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			boardFreq[board[r][c]]++
		}
	}

	for i := 0; i < wordLen; i++ {
		boardFreq[word[i]]--
		if boardFreq[word[i]] < 0 {
			return false
		}
	}

	target := word
	if boardFreq[word[0]] > boardFreq[word[wordLen-1]] {
		runes := []byte(word)
		for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
			runes[i], runes[j] = runes[j], runes[i]
		}
		target = string(runes)
	}

	var dfs func(r, c, idx int) bool
	dfs = func(r, c, idx int) bool {
		if idx == len(target) {
			return true
		}
		if r < 0 || r >= m || c < 0 || c >= n || board[r][c] != target[idx] {
			return false
		}

		temp := board[r][c]
		board[r][c] = '#'

		found := dfs(r+1, c, idx+1) ||
			dfs(r-1, c, idx+1) ||
			dfs(r, c+1, idx+1) ||
			dfs(r, c-1, idx+1)

		board[r][c] = temp
		return found
	}

	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			if board[r][c] == target[0] && dfs(r, c, 0) {
				return true
			}
		}
	}

	return false
}
```

#### Rust 2021
```rust
impl Solution {
    pub fn exist(mut board: Vec<Vec<char>>, word: String) -> bool {
        let m = board.len();
        let n = board[0].len();
        let word_len = word.len();
        if m * n < word_len {
            return false;
        }

        let mut board_freq = [0i32; 128];
        for r in 0..m {
            for c in 0..n {
                board_freq[board[r][c] as usize] += 1;
            }
        }

        let word_bytes = word.as_bytes();
        for &b in word_bytes {
            board_freq[b as usize] -= 1;
            if board_freq[b as usize] < 0 {
                return false;
            }
        }

        let target: Vec<char> = if board_freq[word_bytes[0] as usize]
            > board_freq[word_bytes[word_len - 1] as usize]
        {
            word.chars().rev().collect()
        } else {
            word.chars().collect()
        };

        fn dfs(
            board: &mut Vec<Vec<char>>,
            r: usize,
            c: usize,
            target: &[char],
            idx: usize,
            m: usize,
            n: usize,
        ) -> bool {
            if idx == target.len() {
                return true;
            }
            if board[r][c] != target[idx] {
                return false;
            }
            if idx + 1 == target.len() {
                return true;
            }

            let temp = board[r][c];
            board[r][c] = '#';

            let mut found = false;
            let dirs = [(0isize, 1isize), (0, -1), (1, 0), (-1, 0)];
            for (dr, dc) in dirs {
                let nr = r as isize + dr;
                let nc = c as isize + dc;
                if nr >= 0 && nr < m as isize && nc >= 0 && nc < n as isize {
                    if dfs(board, nr as usize, nc as usize, target, idx + 1, m, n) {
                        found = true;
                        break;
                    }
                }
            }

            board[r][c] = temp;
            found
        }

        for r in 0..m {
            for c in 0..n {
                if board[r][c] == target[0] && dfs(&mut board, r, c, &target, 0, m, n) {
                    return true;
                }
            }
        }

        false
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized Solution (In-Place DFS Backtracking)

### 4.1 Algorithmic Mechanics
Tier 2 omits pre-frequency counting and starts directly by iterating through every cell $(r, c)$.
When `board[r][c] == word[0]`, depth-first search is launched immediately.
The auxiliary space is strictly bounded by the maximum recursion call stack, requiring $O(L)$ auxiliary memory and $O(1)$ heap memory.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(M \times N \times 4^L)$.
- **Space Complexity**: $O(L)$ recursion stack space.

### 4.3 Implementation (C++20)
```cpp
#include <vector>
#include <string>

class Solution {
public:
    bool exist(std::vector<std::vector<char>>& board, std::string word) {
        int m = static_cast<int>(board.size());
        int n = static_cast<int>(board[0].size());

        auto dfs = [&](auto self, int r, int c, int idx) -> bool {
            if (idx == static_cast<int>(word.size())) return true;
            if (r < 0 || r >= m || c < 0 || c >= n || board[r][c] != word[idx]) {
                return false;
            }

            char temp = board[r][c];
            board[r][c] = '#';

            bool found = self(self, r + 1, c, idx + 1)
                      || self(self, r - 1, c, idx + 1)
                      || self(self, r, c + 1, idx + 1)
                      || self(self, r, c - 1, idx + 1);

            board[r][c] = temp;
            return found;
        };

        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (board[r][c] == word[0] && dfs(dfs, r, c, 0)) {
                    return true;
                }
            }
        }

        return false;
    }
};
```

---

## 5. Tier 3: Time-Optimized Alternative (Explicit Visited Grid DFS)

### 5.1 Algorithmic Mechanics
In environments where input mutation is forbidden (such as concurrent readers on shared memory or read-only buffers), an auxiliary $M \times N$ boolean array `visited` is allocated.
Before traversing a neighbor, check `!visited[nr][nc]`.
Set `visited[r][c] = true` before recursive calls and reset `visited[r][c] = false` upon backtrack.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(M \times N \times 4^L)$.
- **Space Complexity**: $O(M \times N + L)$ auxiliary space for the boolean matrix and recursion stack.

### 5.3 Implementation (C++20)
```cpp
#include <vector>
#include <string>

class Solution {
public:
    bool exist(std::vector<std::vector<char>>& board, std::string word) {
        int m = static_cast<int>(board.size());
        int n = static_cast<int>(board[0].size());
        std::vector<std::vector<bool>> visited(m, std::vector<bool>(n, false));

        auto dfs = [&](auto self, int r, int c, int idx) -> bool {
            if (idx == static_cast<int>(word.size())) return true;
            if (r < 0 || r >= m || c < 0 || c >= n || visited[r][c] || board[r][c] != word[idx]) {
                return false;
            }

            visited[r][c] = true;
            bool found = self(self, r + 1, c, idx + 1)
                      || self(self, r - 1, c, idx + 1)
                      || self(self, r, c + 1, idx + 1)
                      || self(self, r, c - 1, idx + 1);
            visited[r][c] = false;

            return found;
        };

        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (board[r][c] == word[0] && dfs(dfs, r, c, 0)) {
                    return true;
                }
            }
        }

        return false;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Recursive Path Search Without Pruning)

### 6.1 Algorithmic Mechanics
The baseline brute-force approach initiates a search from each matching cell, blindly descending through all paths without prior sanity checks on string length or character availability.
On inputs with massive repeating character grids (such as a $30 \times 30$ board of `'A'` searching for $15$ `'A'` followed by a `'B'`), this strategy suffers exponential time consumption.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(M \times N \times 4^L)$.
- **Space Complexity**: $O(L)$ stack frames.

### 6.3 Implementation (Python 3)
```python
from typing import List

class Solution:
    def exist(self, board: List[List[str]], word: str) -> bool:
        m, n = len(board), len(board[0])

        def dfs(r: int, c: int, idx: int) -> bool:
            if idx == len(word):
                return True
            if r < 0 or r >= m or c < 0 or c >= n or board[r][c] != word[idx]:
                return False

            orig = board[r][c]
            board[r][c] = "#"
            found = (
                dfs(r + 1, c, idx + 1)
                or dfs(r - 1, c, idx + 1)
                or dfs(r, c + 1, idx + 1)
                or dfs(r, c - 1, idx + 1)
            )
            board[r][c] = orig
            return found

        for r in range(m):
            for c in range(n):
                if dfs(r, c, 0):
                    return True
        return False
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is the worst-case branching factor 3 instead of 4 after the initial step?</summary>
After stepping into coordinate $(r, c)$ from an incoming neighbor $(r', c')$, that predecessor cell is masked with `'#'`.
Consequently, at most 3 remaining adjacent neighbors are eligible for exploration.
</details>

<details>
<summary>2. Why does word reversal accelerate the search when the start character is more frequent than the end character?</summary>
If the board contains 100 occurrences of `word[0]` but only 2 of `word[L - 1]`, searching forward initiates 100 search trees that fail deeply.
Searching backward initiates only 2 search trees, pruning non-viable exploration by $98\%$.
</details>

<details>
<summary>3. What happens if word length exceeds total grid cells ($L > M \times N$)?</summary>
Because cells cannot be reused within a single path, the simple path length cannot exceed $M \times N$.
The preliminary guard check immediately returns `false` in $O(1)$ time.
</details>

<details>
<summary>4. Why is Breadth-First Search (BFS) typically unsuitable for this problem?</summary>
BFS requires tracking the set of visited cells along every frontier path independently.
This leads to exponential state replication in the queue, causing severe $O(4^L \times L)$ memory overhead.
DFS requires only $O(L)$ space due to depth-first backtracking.
</details>

<details>
<summary>5. How does bitmasking compare with character replacement for tracking visited cells?</summary>
When $M \times N \le 64$, an unsigned 64-bit integer bitmask can track visited coordinates in $O(1)$ space and register bitwise operations.
However, in-place character mutation `board[r][c] = '#'` is universally scalable regardless of grid dimensions.
</details>

<details>
<summary>6. What is the impact of character case sensitivity?</summary>
The problem specifies both lowercase and uppercase English letters.
The frequency counter array must support at least 128 elements to index all ASCII values without collision.
</details>

<details>
<summary>7. Why does short-circuit evaluation in logical OR expressions matter in DFS?</summary>
The expression `dfs(up) || dfs(down) || dfs(left) || dfs(right)` evaluates left-to-right.
The moment any branch returns `true`, subsequent branches are immediately aborted, skipping expensive traversal.
</details>

<details>
<summary>8. How does Word Search I differ fundamentally from Word Search II (LeetCode 212)?</summary>
Word Search I checks a single target word using DFS backtracking.
Word Search II queries a dictionary of thousands of words simultaneously, requiring a Prefix Trie integrated with grid DFS.
</details>

<details>
<summary>9. What is the behavior when the target word has length 1?</summary>
The search requires only finding any cell $(r, c)$ where `board[r][c] == word[0]`.
The recursive step finishes immediately at `idx + 1 == 1`.
</details>

<details>
<summary>10. What ensures thread safety when mutative in-place backtracking is utilized?</summary>
In-place mutation modifies shared board state, which is not thread-safe.
If multiple threads search the board concurrently, either lock synchronization or Tier 3 visited grids must be used.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/word-search.cpp)
- [Python Implementation](../Python/word-search.py)
- [Java Implementation](../Java/word-search.java)
- [TypeScript Implementation](../TypeScript/word-search.ts)
- [Go Implementation](../Golang/word-search.go)
- [Rust Implementation](../Rust/word-search.rs)

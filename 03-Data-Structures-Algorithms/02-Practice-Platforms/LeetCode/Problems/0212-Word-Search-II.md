---
id: 0212-word-search-ii
title: "LeetCode 212: Word Search II (Trie & Backtracking Deep Dive)"
tags:
  - dsa
  - leetcode
  - trie
  - backtracking
  - depth-first-search
  - matrix
level: hard
type: problem-breakdown
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/word-search-ii/"
---

# LeetCode 212: Word Search II (Trie & Backtracking Deep Dive)

## 1. Problem Statement and Architectural Overview

Given an $m \times n$ grid of characters `board` and an array of strings `words`, return all words on the board.
Each word must be constructed from letters of sequentially adjacent cells, where adjacent cells are horizontally or vertically neighboring.
The same letter cell cannot be used more than once in a single word path.

### Critical Constraints
- $m == \text{board.length}$
- $n == \text{board}[i]\text{.length}$
- $1 \le m, n \le 12$
- $\text{board}[i][j]$ is a lowercase English letter.
- $1 \le \text{words.length} \le 3 \times 10^4$
- $1 \le \text{words}[i]\text{.length} \le 10$
- $\text{words}[i]$ consists of lowercase English letters.
- All strings in `words` are unique.

---

## 2. Mathematical Formalism and Invariant Proofs

Let $\Sigma = \{a, b, \dots, z\}$ denote the alphabet of size 26.
Let $W = \{w_1, w_2, \dots, w_k\}$ denote the dictionary of words to search.
Let $T$ denote the prefix Trie built over $W$.
Each node $u \in T$ corresponds to a unique prefix $p(u)$.
Let $G = (V, E)$ represent the grid graph where $V = \{(r, c) \mid 0 \le r < m, 0 \le c < n\}$ and edges connect orthogonal neighbors.

### Inductive Invariant of Simultaneous Graph-Trie Traversal
Let $P_t = (v_1, v_2, \dots, v_t)$ be a simple path of length $t$ on $G$ visited by the current DFS branch.
Let $S(P_t) = \text{board}[v_1] \cdot \text{board}[v_2] \dots \text{board}[v_t]$ be the concatenated word prefix.
The search maintains the invariant:
$$\exists u \in T \quad \text{such that} \quad p(u) = S(P_t)$$
If no such node $u$ exists in $T$, then for all extensions $P'$ of $P_t$, $S(P') \notin \text{Prefixes}(W)$.
Hence, the entire subtree of paths rooted at $P_t$ can be safely pruned immediately.

### Trie Subtree Pruning Invariant
For each node $u \in T$, maintain $\text{wordCount}(u)$, the number of unvisited dictionary words contained in the subtree rooted at $u$.
When a terminal word at node $u$ is collected:
1. Clear the terminal word marker to prevent duplicate outputs.
2. Decrement $\text{wordCount}(u)$.
3. If $\text{wordCount}(u) = 0$, unlink node $u$ from its parent in $T$.
By induction, once an unlinked branch reaches $\text{wordCount} = 0$, subsequent DFS traversals visiting the parent will skip exploration entirely, reducing future branch expansions from $O(3^L)$ to $O(1)$.

---

## 3. Four-Tier Solution Architecture

### Tier 1: Optimal Prefix Trie with In-Place Grid Backtracking and Leaf Pruning
- **Core Concept**: Build a 26-ary Trie storing words directly at their terminal nodes.
- Traverse the grid once for every cell $(r, c)$.
- If the root has a child matching $\text{board}[r][c]$, enter DFS.
- In-place mutate $\text{board}[r][c] = \text{'\#'}$ during exploration to enforce single-use semantics without allocating a dynamic `visited` set.
- Proactively prune depleted Trie nodes backwards to eliminate dead branches.
- **Time Complexity**: $O(M \cdot N \cdot 4 \cdot 3^{L - 1})$ worst case without pruning, but bounded by $\min(M \cdot N \cdot 4 \cdot 3^{L - 1}, \sum |w_i|)$ with active pruning.
- **Space Complexity**: $O(\sum |w_i|)$ for the Trie structure and $O(L)$ for the maximum recursion call stack.

### Tier 2: Space-Optimized Trie Node Allocation (Flat Array Pool)
- **Core Concept**: Avoid individual dynamic allocations per Trie node by using a contiguous flat vector buffer acting as an arena allocator.
- Each node stores an integer offset pointing to child nodes.
- Preserves CPU L1/L2 data cache spatial locality and avoids heap fragmentation from thousands of small pointer allocations.
- **Time Complexity**: $O(M \cdot N \cdot 3^{L - 1})$.
- **Space Complexity**: $O(\sum |w_i|)$ contiguous arena memory.

### Tier 3: Classic Prefix Trie with External HashSet Deduplication
- **Core Concept**: Insert words into a standard Trie.
- Run DFS without leaf node pruning.
- Store results in an external hash set to discard duplicate word hits when multiple paths spell the same word.
- **Time Complexity**: $O(M \cdot N \cdot 4 \cdot 3^{L - 1})$.
- **Space Complexity**: $O(\sum |w_i| + K)$ where $K$ is the number of found words.

### Tier 4: Naive Word Search (Repeated DFS per Word)
- **Core Concept**: Iterate through each word in $W$, and for each word execute standard LeetCode 79 `exist(board, word)`.
- Re-scans the $M \times N$ matrix up to $|W| = 30,000$ times.
- Extremely redundant as common prefixes like "apple" and "application" are completely re-searched from scratch.
- **Time Complexity**: $O(K \cdot M \cdot N \cdot 4 \cdot 3^{L - 1})$ which exceeds the time limit ($30000 \times 144 \times 3^9 \approx 8.5 \times 10^{10}$ operations).
- **Space Complexity**: $O(L)$ recursion stack.

---

## 4. Hardware, Memory, and Cache Systems Considerations

1. **Spatial Cache Locality in Board Representation**: The board has size at most $12 \times 12 = 144$ bytes.
The entire matrix easily fits in a single 64-byte L1 cache line or two adjacent lines.
2. **In-Place Mutation Overhead**: Overwriting $\text{board}[r][c]$ with `#'` and reverting avoids allocating a 144-element boolean array per cell or passing an auxiliary array, keeping register pressure low.
3. **Branch Prediction and Early Exit**: The proactive Trie pruning mechanism ensures that words found in the first quadrant of the board delete their nodes, making later DFS queries on later cells fail at depth 1.

---

## 5. Edge-Case Boundary Defense Matrix

| Edge Case Dimension | Input Scenario | Expected Behavior | Failure Mode Without Defense |
|:---|:---|:---|:---|
| Single Cell Board | `board = [["a"]]`, `words = ["a", "b"]` | Return `["a"]` | Out of bounds array indexing on neighbor loops |
| Multiple Paths for Same Word | Board has several paths spelling "cat" | Return one instance of "cat" | Duplicate words returned without `curr.word = null` |
| Overlapping Prefixes | `words = ["app", "apple"]` | Return both `app` and `apple` | Early exit at prefix node truncates deeper search |
| Missing Common Letter | Board has no 'z', words start with 'z' | Instant $O(1)$ rejection at root | Searching board uselessly without root child check |
| Long Cyclic Paths | Path attempts to revisit cell | Backtracking sentinel `#` rejects | Infinite cycles or incorrect character reuse |

---

## 6. Comprehensive 10 Frequently Asked Questions (FAQ)

### 1. Why is Trie preferred over searching each word individually?
Searching each word individually takes $O(K \cdot M \cdot N \cdot 3^L)$ time, which times out for $K = 30,000$.
A Trie combines identical prefixes into shared paths, reducing the search space to a single concurrent traversal.

### 2. How does leaf node pruning improve run time?
Once all words under a Trie subtree are discovered, pruning that subtree removes it from consideration.
Subsequent cell searches encountering that letter fail immediately in $O(1)$ time rather than exploring thousands of empty branches.

### 3. What is the maximum recursion depth?
The maximum word length is 10, so the maximum recursion depth is bounded by 10.
This eliminates any possibility of stack overflow errors.

### 4. Why mutate the board in place with '#' instead of a visited matrix?
Mutating the board in place incurs zero memory allocation and maintains cache locality.
A separate visited matrix requires clearing or reallocating overhead across recursive calls.

### 5. Why store the entire string at the terminal Trie node?
Storing the string directly at the terminal node allows $O(1)$ extraction when reached, avoiding the need to reconstruct the string through string concatenation during recursion.

### 6. Can a single board cell be visited multiple times in different words?
Yes, a cell can be part of many different words across separate DFS branches.
It cannot be reused only within the exact same word path.

### 7. Does the order of returned words matter?
No, the problem allows returning the matching words in any arbitrary order.

### 8. How do we prevent duplicate entries in the result?
When a terminal word is encountered, we immediately set `node.word = null` after appending it to the result list.

### 9. What is the branch factor of the grid DFS?
From any cell, there are 4 directions, but the cell from which we just came is marked visited, so there are at most 3 valid branching options at each step.

### 10. How does this problem relate to Word Search I?
Word Search I searches for a single target word, solvable with simple DFS.
Word Search II searches for thousands of target words simultaneously, requiring a Prefix Trie for multi-target pruning.

---

## 7. Related Problem Cross-References

- [[0079-Word-Search|LeetCode 79: Word Search]]
- [[0208-Implement-Trie-Prefix-Tree|LeetCode 208: Implement Trie (Prefix Tree)]]
- [[0211-Design-Add-and-Search-Words-Data-Structure|LeetCode 211: Design Add and Search Words Data Structure]]

---

## 8. Standalone Implementation Links

- [C++ Implementation](../C++/word-search-ii.cpp)
- [Python Implementation](../Python/word-search-ii.py)
- [Java Implementation](../Java/word-search-ii.java)
- [TypeScript Implementation](../TypeScript/word-search-ii.ts)
- [Go Implementation](../Golang/word-search-ii.go)
- [Rust Implementation](../Rust/word-search-ii.rs)

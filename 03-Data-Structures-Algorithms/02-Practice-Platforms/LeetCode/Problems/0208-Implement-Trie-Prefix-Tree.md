---
id: leetcode-0208-implement-trie-prefix-tree
title: "LeetCode 0208: Implement Trie (Prefix Tree)"
tags:
  - dsa
  - leetcode
  - design
  - trie
  - string
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/implement-trie-prefix-tree/"
---

# LeetCode 0208: Implement Trie (Prefix Tree)

## 1. Problem Formalization and Constraints

A trie (pronounced as "try") or prefix tree is a tree data structure used to efficiently store and retrieve keys in a dataset of strings.
There are various applications of this data structure, such as autocomplete and spellchecker.
Implement the `Trie` class:
- `Trie()`: Initializes the trie object.
- `void insert(String word)`: Inserts the string `word` into the trie.
- `boolean search(String word)`: Returns `true` if the string `word` is in the trie (i.e., was inserted before), and `false` otherwise.
- `boolean startsWith(String prefix)`: Returns `true` if there is a previously inserted string `word` that has the prefix `prefix`, and `false` otherwise.

### Constraints
- $1 \le \text{word.length}, \text{prefix.length} \le 2000$
- `word` and `prefix` consist only of lowercase English letters.
- At most $3 \times 10^4$ calls in total will be made to `insert`, `search`, and `startsWith`.

### Examples
- **Example 1**:
  - Input:
    - `["Trie", "insert", "search", "search", "startsWith", "insert", "search"]`
    - `[[], ["apple"], ["apple"], ["app"], ["app"], ["app"], ["app"]]`
  - Output:
    - `[null, null, true, false, true, null, true]`
  - Explanation:
    - `Trie trie = new Trie();`
    - `trie.insert("apple");`
    - `trie.search("apple"); // return True`
    - `trie.search("app"); // return False`
    - `trie.startsWith("app"); // return True`
    - `trie.insert("app");`
    - `trie.search("app"); // return True`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Fixed Array Trie (Alphabet Array of Size 26) | $O(L)$ per op | $O(N \cdot L \cdot \Sigma)$ total | Uses direct array indexing `children[ch - 'a']`; yields $O(1)$ child branch transitions and zero hash collisions. |
| **Tier 2 (Hash Map Trie)** | Hash Map Child Pointers | $O(L)$ per op | $O(N \cdot L)$ total | Uses hash tables at each node; more memory efficient when alphabet size $\Sigma$ is large or unicode. |
| **Tier 3 (Binary Search Trie / Ternary Search Tree)** | Ternary Search Tree (TST) | $O(L \log \Sigma)$ per op | $O(N \cdot L)$ total | Stores 3 pointers per node (left, equal, right); balances memory and branch speed. |
| **Tier 4 (Brute Force Set)** | Hash Set / Array String Search | $O(L)$ insert, $O(K \cdot L)$ startsWith | $O(N \cdot L)$ total | Stores words in a hash set; `startsWith` scans all $K$ stored strings checking prefixes. |

*Notation*: $L$ is the length of the string, $N$ is the number of inserted strings, and $\Sigma = 26$ is the alphabet size.

---

## 3. Tier 1: Most Optimal Solution (Fixed Array Trie)

### 3.1 Algorithmic Mechanics and Invariant Proof

Each node represents a state in the prefix language automaton:
- `children`: Fixed array of 26 pointers, where index $k$ represents character $\text{'a'} + k$.
- `is_end`: Boolean flag indicating whether a complete word terminates at this node.

Operations:
1. `insert(word)`: Start at `root`. For each character `ch`:
   - Compute index $i = ch - \text{'a'}$.
   - If `children[i]` is null, instantiate a new node.
   - Advance `curr = children[i]`.
   - After traversing all characters, set `curr.is_end = true`.
2. `search(word)`: Traverse the prefix. If any character pointer is null, return `false`. At the terminal node, return `curr.is_end`.
3. `startsWith(prefix)`: Traverse the prefix. If all characters exist, return `true`; if any pointer is null, return `false`.

**Invariant Proof**:
Let $T$ be the trie rooted at `root`.
Every path of length $k$ from `root` to a node $u$ defines a unique string $s = c_1 c_2 \dots c_k$ over $\Sigma$.
Because child transitions are uniquely determined by character index, each string maps to a unique node in $T$.
`is_end` is set to true if and only if $s$ was explicitly added via `insert(s)`.
Thus:
- `search(word)` returns true $\iff$ node $u$ corresponding to `word` exists and has `is_end == true`.
- `startsWith(prefix)` returns true $\iff$ node $v$ corresponding to `prefix` exists in $T$, which implies at least one string having prefix $v$ was inserted into the trie.
Both conditions are exact, proving complete correctness.

### 3.2 Complexity Analysis
- **Time Complexity**:
  - `insert(word)`: $O(L)$ where $L$ is `word.length`.
  - `search(word)`: $O(L)$.
  - `startsWith(prefix)`: $O(L)$.
  - Every step takes $O(1)$ constant time array indexing.
- **Auxiliary Space Complexity**: $O(N \cdot L \cdot \Sigma)$ worst-case total heap memory across all nodes. In practice, common prefixes share nodes, dramatically reducing actual memory consumption.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <string>
#include <array>
#include <memory>

class Trie {
private:
    struct TrieNode {
        std::array<std::unique_ptr<TrieNode>, 26> children{};
        bool is_end = false;
    };

    std::unique_ptr<TrieNode> root;

    const TrieNode* findPrefix(const std::string& prefix) const {
        const TrieNode* curr = root.get();
        for (char ch : prefix) {
            int idx = ch - 'a';
            if (!curr->children[idx]) {
                return nullptr;
            }
            curr = curr->children[idx].get();
        }
        return curr;
    }

public:
    Trie() : root(std::make_unique<TrieNode>()) {}

    void insert(const std::string& word) {
        TrieNode* curr = root.get();
        for (char ch : word) {
            int idx = ch - 'a';
            if (!curr->children[idx]) {
                curr->children[idx] = std::make_unique<TrieNode>();
            }
            curr = curr->children[idx].get();
        }
        curr->is_end = true;
    }

    bool search(const std::string& word) const {
        const TrieNode* node = findPrefix(word);
        return node != nullptr && node->is_end;
    }

    bool startsWith(const std::string& prefix) const {
        return findPrefix(prefix) != nullptr;
    }
};
```

#### Python 3
```python
class TrieNode:
    def __init__(self) -> None:
        self.children: dict[str, TrieNode] = {}
        self.is_end: bool = False


class Trie:
    def __init__(self) -> None:
        self.root = TrieNode()

    def insert(self, word: str) -> None:
        curr = self.root
        for ch in word:
            if ch not in curr.children:
                curr.children[ch] = TrieNode()
            curr = curr.children[ch]
        curr.is_end = True

    def _find_prefix(self, prefix: str) -> TrieNode | None:
        curr = self.root
        for ch in prefix:
            if ch not in curr.children:
                return None
            curr = curr.children[ch]
        return curr

    def search(self, word: str) -> bool:
        node = self._find_prefix(word)
        return node is not None and node.is_end

    def startsWith(self, prefix: str) -> bool:
        return self._find_prefix(prefix) is not None
```

#### Java 21
```java
class Trie {
    private static class TrieNode {
        TrieNode[] children = new TrieNode[26];
        boolean isEnd = false;
    }

    private final TrieNode root;

    public Trie() {
        this.root = new TrieNode();
    }

    public void insert(String word) {
        TrieNode curr = root;
        for (int i = 0; i < word.length(); i++) {
            int idx = word.charAt(i) - 'a';
            if (curr.children[idx] == null) {
                curr.children[idx] = new TrieNode();
            }
            curr = curr.children[idx];
        }
        curr.isEnd = true;
    }

    private TrieNode findPrefix(String prefix) {
        TrieNode curr = root;
        for (int i = 0; i < prefix.length(); i++) {
            int idx = prefix.charAt(i) - 'a';
            if (curr.children[idx] == null) {
                return null;
            }
            curr = curr.children[idx];
        }
        return curr;
    }

    public boolean search(String word) {
        TrieNode node = findPrefix(word);
        return node != null && node.isEnd;
    }

    public boolean startsWith(String prefix) {
        return findPrefix(prefix) != null;
    }
}
```

#### TypeScript
```typescript
class TrieNode {
    children: (TrieNode | null)[];
    isEnd: boolean;

    constructor() {
        this.children = new Array(26).fill(null);
        this.isEnd = false;
    }
}

class Trie {
    private root: TrieNode;

    constructor() {
        this.root = new TrieNode();
    }

    insert(word: string): void {
        let curr = this.root;
        for (let i = 0; i < word.length; i++) {
            const idx = word.charCodeAt(i) - 97;
            if (!curr.children[idx]) {
                curr.children[idx] = new TrieNode();
            }
            curr = curr.children[idx]!;
        }
        curr.isEnd = true;
    }

    private findPrefix(prefix: string): TrieNode | null {
        let curr = this.root;
        for (let i = 0; i < prefix.length; i++) {
            const idx = prefix.charCodeAt(i) - 97;
            if (!curr.children[idx]) {
                return null;
            }
            curr = curr.children[idx]!;
        }
        return curr;
    }

    search(word: string): boolean {
        const node = this.findPrefix(word);
        return node !== null && node.isEnd;
    }

    startsWith(prefix: string): boolean {
        return this.findPrefix(prefix) !== null;
    }
}
```

#### Go
```go
package main

type TrieNode struct {
	children [26]*TrieNode
	isEnd    bool
}

type Trie struct {
	root *TrieNode
}

func Constructor() Trie {
	return Trie{root: &TrieNode{}}
}

func (t *Trie) Insert(word string) {
	curr := t.root
	for i := 0; i < len(word); i++ {
		idx := word[i] - 'a'
		if curr.children[idx] == nil {
			curr.children[idx] = &TrieNode{}
		}
		curr = curr.children[idx]
	}
	curr.isEnd = true
}

func (t *Trie) findPrefix(prefix string) *TrieNode {
	curr := t.root
	for i := 0; i < len(prefix); i++ {
		idx := prefix[i] - 'a'
		if curr.children[idx] == nil {
			return nil
		}
		curr = curr.children[idx]
	}
	return curr
}

func (t *Trie) Search(word string) bool {
	node := t.findPrefix(word)
	return node != nil && node.isEnd
}

func (t *Trie) StartsWith(prefix string) bool {
	return t.findPrefix(prefix) != nil
}
```

#### Rust
```rust
#[derive(Default)]
pub struct TrieNode {
    children: [Option<Box<TrieNode>>; 26],
    is_end: bool,
}

#[derive(Default)]
pub struct Trie {
    root: TrieNode,
}

impl Trie {
    pub fn new() -> Self {
        Default::default()
    }

    pub fn insert(&mut self, word: String) {
        let mut curr = &mut self.root;
        for &byte in word.as_bytes() {
            let idx = (byte - b'a') as usize;
            curr = curr.children[idx].get_or_insert_with(|| Box::new(TrieNode::default()));
        }
        curr.is_end = true;
    }

    fn find_prefix(&self, prefix: &str) -> Option<&TrieNode> {
        let mut curr = &self.root;
        for &byte in prefix.as_bytes() {
            let idx = (byte - b'a') as usize;
            match curr.children[idx].as_ref() {
                Some(next) => curr = next,
                None => return None,
            }
        }
        Some(curr)
    }

    pub fn search(&self, word: String) -> bool {
        self.find_prefix(&word).map_or(false, |node| node.is_end)
    }

    pub fn starts_with(&self, prefix: String) -> bool {
        self.find_prefix(&prefix).is_some()
    }
}
```

---

## 4. Tier 2: Hash Map Child Pointers

### 4.1 Mechanical Description
Instead of allocating a fixed array of 26 pointers per node, each node holds a dynamic hash map from `char` to `TrieNode*`.
This is useful for Unicode characters or sparse character sets.

### 4.2 Trade-offs
- Saves memory when nodes have very few children.
- Incurs hash calculation and pointer indirection overhead on every character transition.

---

## 5. Tier 3: Ternary Search Tree (TST)

### 5.1 Mechanical Description
Each node stores a character and three pointers: `left` (smaller char), `equal` (matches char, advance to next string char), and `right` (larger char).
Combines the memory efficiency of a binary search tree with the prefix structure of a trie.

### 5.2 Trade-offs
- Takes $O(L \log \Sigma)$ time per operation.
- Requires significantly fewer pointers per node (3 vs 26), offering high space savings for large character sets.

---

## 6. Tier 4: Hash Set String Search (Brute Force Baseline)

### 6.1 Mechanical Description
Store inserted strings inside a hash set.
`search(word)` takes $O(L)$ hash set lookup time.
However, `startsWith(prefix)` must scan through all stored strings, checking if `s.startswith(prefix)`, taking $O(K \cdot L)$ time where $K$ is total stored words.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Fixed Array Size**: An array of 26 64-bit pointers takes $26 \times 8 = 208$ bytes, fitting across approximately 3 CPU cache lines (64 bytes each).
2. **RAII Cleanups**: Using smart pointers (`std::unique_ptr` in C++, `Box` in Rust) guarantees automatic recursive deallocation when the trie is destroyed, preventing memory leaks.
3. **Branch Elimination**: Computing `ch - 'a'` replaces dictionary lookups with simple register subtraction and pointer offset arithmetic.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single character word | `word = "a"` | Inserts and retrieves cleanly | Root child index 0 marked as `is_end`. |
| Substring prefix query | `insert("apple")`, `search("app")` | Returns `false` | Node for "app" exists, but `is_end` is `false`. |
| Exact prefix match | `insert("apple")`, `startsWith("app")` | Returns `true` | Node exists; `startsWith` does not require `is_end`. |
| Non-existent prefix | `search("banana")` | Returns `false` | Traversal terminates on null child pointer at index 'b'. |
| Repeated insertions | `insert("app")`, `insert("app")` | Idempotent operation | Sets `is_end = true` multiple times safely. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What does the word 'Trie' originate from?
The name 'Trie' comes from the middle syllable of 're**trie**val', coined by Edward Fredkin.

### 2. Why is a Trie faster than a Hash Table for prefix queries?
A hash table cannot answer prefix queries without checking all keys or hashing all possible prefixes of every key. A Trie naturally preserves prefixes along its path.

### 3. How can memory consumption be reduced for long unique paths?
Radix trees (or Patricia tries) compress chains of single-child nodes into edge strings, reducing node counts significantly.

### 4. What is the time complexity difference between `search` and `startsWith`?
Both take $O(L)$ time. The only difference is that `search` requires checking `node.is_end == true`, while `startsWith` only requires `node != null`.

### 5. Why are lowercase English letters specifically constrained?
Lower-case English letters have alphabet size $\Sigma = 26$, enabling a compact fixed array of 26 pointers indexed via `ch - 'a'`.

### 6. How does Rust prevent memory leaks in cyclic pointer graphs?
Tries are strict directed trees without back-pointers; using `Box<TrieNode>` represents unique hierarchical ownership, deallocating cleanly on drop.

### 7. What happens if an inserted string is an empty string?
While the constraints require length $\ge 1$, an empty string would mark `root.is_end = true`.

### 8. How does Trie deletion work?
Deletion traverses the word, marks `is_end = false`, and optionally prunes leaf nodes upwards that no longer have other children or active endpoints.

### 9. Can a Trie be used for finding the longest common prefix?
Yes, by traversing down the root as long as each node has exactly one child and `is_end == false`.

### 10. How does this compare with LeetCode 211 (Design Add and Search Words Data Structure)?
LeetCode 211 adds wildcard search ('.'), which requires backtracking across all 26 children when a wildcard character is encountered.

---

## 10. Related Problems and Systematic Progression Links

- [[0079-Word-Search]]: Matrix word search backtracking.
- [[0139-Word-Break]]: Dictionary lookup string partitioning.
- LeetCode 211 (Design Add and Search Words Data Structure): Trie with wildcard pattern matching.
- LeetCode 212 (Word Search II): Backtracking multi-word search on a 2D board with Trie.
- LeetCode 677 (Map Sum Pairs): Prefix sum queries on trie values.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/implement-trie-prefix-tree.cpp)
- [Python Implementation](../Python/implement-trie-prefix-tree.py)
- [Java Implementation](../Java/implement-trie-prefix-tree.java)
- [TypeScript Implementation](../TypeScript/implement-trie-prefix-tree.ts)
- [Go Implementation](../Golang/implement-trie-prefix-tree.go)
- [Rust Implementation](../Rust/implement-trie-prefix-tree.rs)

---
id: leetcode-0211-design-add-and-search-words-data-structure
title: "LeetCode 0211: Design Add and Search Words Data Structure"
tags:
  - dsa
  - leetcode
  - design
  - trie
  - string
  - backtracking
  - depth-first-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/design-add-and-search-words-data-structure/"
---

# LeetCode 0211: Design Add and Search Words Data Structure

## 1. Problem Formalization and Constraints

Design a data structure that supports adding new words and finding if a string matches any previously added string.
Implement the `WordDictionary` class:
- `WordDictionary()`: Initializes the object.
- `void addWord(word)`: Adds `word` to the data structure, it can be matched later.
- `bool search(word)`: Returns `true` if there is any string in the data structure that matches `word` or `false` otherwise.
  `word` may contain dots `'.'` where dots can be matched with any letter.

### Constraints
- $1 \le \text{word.length} \le 25$
- `word` in `addWord` consists of lowercase English letters.
- `word` in `search` consists of `'.'` or lowercase English letters.
- There will be at most 2 dots in `word` for search queries.
- At most $10^4$ calls in total will be made to `addWord` and `search`.

### Examples
- **Example 1**:
  - Input:
    - `["WordDictionary","addWord","addWord","addWord","search","search","search","search"]`
    - `[[],["bad"],["dad"],["mad"],["pad"],["bad"],[".ad"],["b.."]]`
  - Output:
    - `[null,null,null,null,false,true,true,true]`
  - Explanation:
    - `WordDictionary wordDictionary = new WordDictionary();`
    - `wordDictionary.addWord("bad");`
    - `wordDictionary.addWord("dad");`
    - `wordDictionary.addWord("mad");`
    - `wordDictionary.search("pad"); // return False`
    - `wordDictionary.search("bad"); // return True`
    - `wordDictionary.search(".ad"); // return True`
    - `wordDictionary.search("b.."); // return True`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Fixed Array Trie with Recursive DFS | $O(L)$ add, $O(26^D \cdot L)$ search | $O(N \cdot L \cdot \Sigma)$ total | Uses direct array indexing for characters; branches across all non-null children upon encountering `'.'`. |
| **Tier 2 (Hash Map Trie)** | Hash Map Child Pointers with DFS | $O(L)$ add, $O(\Sigma^D \cdot L)$ search | $O(N \cdot L)$ total | Dynamic hash map per node; saves space on sparse branching at the cost of map lookup overhead. |
| **Tier 3 (Length Buckets)** | Group Words by Length in Hash Map | $O(1)$ add, $O(M \cdot L)$ search | $O(N \cdot L)$ total | Words grouped by length; `search` scans only words with length equal to target string; degrades if many words share the same length. |
| **Tier 4 (Brute Force)** | Flat Array / List Scan | $O(1)$ add, $O(N \cdot L)$ search | $O(N \cdot L)$ total | Appends words to an array; scans every stored word sequentially on search; highly inefficient at scale. |

*Notation*: $L$ is word length, $D$ is the count of wildcard dots (here $D \le 2$), $N$ is total inserted words, $M$ is words of length $L$, and $\Sigma = 26$.

---

## 3. Tier 1: Most Optimal Solution (Fixed Array Trie with Recursive DFS)

### 3.1 Algorithmic Mechanics and Invariant Proof

The trie consists of nodes containing a boolean terminal flag `is_end` and an array of 26 pointers `children`:
1. `addWord(word)`:
   Traverse the trie matching each character.
   If a child node does not exist at index `ch - 'a'`, instantiate it.
   After all characters are processed, mark the terminating node with `is_end = true`.
2. `search(word)`:
   Initiate recursive traversal `dfs(index, curr)` starting at `root` and index 0:
   - If `index == word.length()`, return `curr.is_end`.
   - If `word[index]` is a lowercase letter `c`, calculate `idx = c - 'a'`.
     If `curr.children[idx]` is non-null, recurse on `(index + 1, curr.children[idx])`; otherwise return `false`.
   - If `word[index] == '.'`, iterate through all 26 possible children.
     For every non-null child, recursively execute `dfs(index + 1, child)`.
     If any recursive branch returns `true`, immediately return `true`.
   - If all options are exhausted without a match, return `false`.

**Invariant Proof**:
Let $T$ denote the trie of all words added so far.
For any query string $Q$ of length $L$, $Q$ matches an added word $W = c_1 c_2 \dots c_L$ if and only if for all $1 \le i \le L$, either $Q[i] = W[i]$ or $Q[i] = \text{'.'}$.
If $Q[i]$ is an exact character, exactly one deterministic branch exists in $T$.
If $Q[i] = \text{'.'}$, branching over all non-null children tests all valid character assignments for $Q[i]$.
Because DFS visits all possible paths matching the prefix pattern up to depth $L$ and checks `curr.is_end`, it returns `true` if and only if at least one path spelling a valid added word exists in $T$.
Pruning on null pointers guarantees that fruitless search branches terminate immediately.
Therefore, the search procedure is sound and complete.

### 3.2 Complexity Analysis
- **Time Complexity**:
  - `addWord(word)`: $O(L)$ where $L$ is the length of `word`.
  - `search(word)`: If no dots are present, $O(L)$. If $D$ dots are present, the worst-case number of visited branches is $O(26^D \cdot L)$. Since the problem constraints specify at most 2 dots, $26^2 = 676$, making search practically instant ($O(L)$ with a small constant factor).
- **Auxiliary Space Complexity**:
  - $O(N \cdot L \cdot \Sigma)$ heap space in the worst case for storing the trie nodes.
  - The recursion stack for `search` consumes $O(L)$ space, bounded by the maximum word length ($L \le 25$).

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

#include <string>
#include <vector>
#include <memory>
#include <array>

using namespace std;

class WordDictionary {
public:
    struct TrieNode {
        bool isEnd = false;
        array<unique_ptr<TrieNode>, 26> children{};
    };

    WordDictionary() : root_(make_unique<TrieNode>()) {}

    void addWord(const string& word) {
        auto* curr = root_.get();
        for (char ch : word) {
            int idx = ch - 'a';
            if (!curr->children[idx]) {
                curr->children[idx] = make_unique<TrieNode>();
            }
            curr = curr->children[idx].get();
        }
        curr->isEnd = true;
    }

    bool search(const string& word) const {
        return searchHelper(word, 0, root_.get());
    }

private:
    bool searchHelper(const string& word, int index, const TrieNode* curr) const {
        if (!curr) return false;
        if (index == static_cast<int>(word.length())) {
            return curr->isEnd;
        }

        char ch = word[index];
        if (ch == '.') {
            for (const auto& child : curr->children) {
                if (child && searchHelper(word, index + 1, child.get())) {
                    return true;
                }
            }
            return false;
        } else {
            int idx = ch - 'a';
            return curr->children[idx] && searchHelper(word, index + 1, curr->children[idx].get());
        }
    }

    unique_ptr<TrieNode> root_;
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
# Space: O(N * L) total heap memory across all nodes

class TrieNode:
    def __init__(self):
        self.children = {}
        self.is_end = False

class WordDictionary:
    def __init__(self):
        self.root = TrieNode()

    def addWord(self, word: str) -> None:
        curr = self.root
        for ch in word:
            if ch not in curr.children:
                curr.children[ch] = TrieNode()
            curr = curr.children[ch]
        curr.is_end = True

    def search(self, word: str) -> bool:
        def dfs(index: int, curr: TrieNode) -> bool:
            if index == len(word):
                return curr.is_end

            ch = word[index]
            if ch == '.':
                for child in curr.children.values():
                    if dfs(index + 1, child):
                        return True
                return False
            else:
                if ch not in curr.children:
                    return False
                return dfs(index + 1, curr.children[ch])

        return dfs(0, self.root)
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

class WordDictionary {
    private static class TrieNode {
        boolean isEnd = false;
        TrieNode[] children = new TrieNode[26];
    }

    private final TrieNode root;

    public WordDictionary() {
        root = new TrieNode();
    }

    public void addWord(String word) {
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

    public boolean search(String word) {
        return searchInNode(word, 0, root);
    }

    private boolean searchInNode(String word, int index, TrieNode curr) {
        if (curr == null) {
            return false;
        }
        if (index == word.length()) {
            return curr.isEnd;
        }

        char ch = word.charAt(index);
        if (ch == '.') {
            for (TrieNode child : curr.children) {
                if (child != null && searchInNode(word, index + 1, child)) {
                    return true;
                }
            }
            return false;
        } else {
            int idx = ch - 'a';
            return searchInNode(word, index + 1, curr.children[idx]);
        }
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

class TrieNode {
    isEnd: boolean = false;
    children: (TrieNode | null)[] = new Array(26).fill(null);
}

class WordDictionary {
    private root: TrieNode;

    constructor() {
        this.root = new TrieNode();
    }

    addWord(word: string): void {
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

    search(word: string): boolean {
        return this.dfs(word, 0, this.root);
    }

    private dfs(word: string, index: number, curr: TrieNode): boolean {
        if (index === word.length) {
            return curr.isEnd;
        }

        const ch = word[index];
        if (ch === '.') {
            for (let i = 0; i < 26; i++) {
                const child = curr.children[i];
                if (child && this.dfs(word, index + 1, child)) {
                    return true;
                }
            }
            return false;
        } else {
            const idx = ch.charCodeAt(0) - 97;
            const child = curr.children[idx];
            return child ? this.dfs(word, index + 1, child) : false;
        }
    }
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

package main

type WordDictionary struct {
	isEnd    bool
	children [26]*WordDictionary
}

func Constructor() WordDictionary {
	return WordDictionary{}
}

func (this *WordDictionary) AddWord(word string) {
	curr := this
	for i := 0; i < len(word); i++ {
		idx := word[i] - 'a'
		if curr.children[idx] == nil {
			curr.children[idx] = &WordDictionary{}
		}
		curr = curr.children[idx]
	}
	curr.isEnd = true
}

func (this *WordDictionary) Search(word string) bool {
	return this.searchHelper(word, 0)
}

func (this *WordDictionary) searchHelper(word string, index int) bool {
	if index == len(word) {
		return this.isEnd
	}

	ch := word[index]
	if ch == '.' {
		for i := 0; i < 26; i++ {
			if this.children[i] != nil && this.children[i].searchHelper(word, index+1) {
				return true
			}
		}
		return false
	}

	idx := ch - 'a'
	if this.children[idx] == nil {
		return false
	}
	return this.children[idx].searchHelper(word, index+1)
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

pub struct WordDictionary {
    is_end: bool,
    children: [Option<Box<WordDictionary>>; 26],
}

impl WordDictionary {
    pub fn new() -> Self {
        const INIT: Option<Box<WordDictionary>> = None;
        WordDictionary {
            is_end: false,
            children: [INIT; 26],
        }
    }

    pub fn add_word(&mut self, word: String) {
        let mut curr = self;
        for b in word.bytes() {
            let idx = (b - b'a') as usize;
            curr = curr.children[idx].get_or_insert_with(|| Box::new(WordDictionary::new()));
        }
        curr.is_end = true;
    }

    pub fn search(&self, word: String) -> bool {
        self.search_helper(word.as_bytes(), 0)
    }

    fn search_helper(&self, bytes: &[u8], index: usize) -> bool {
        if index == bytes.len() {
            return self.is_end;
        }

        let b = bytes[index];
        if b == b'.' {
            for child in &self.children {
                if let Some(next_node) = child {
                    if next_node.search_helper(bytes, index + 1) {
                        return true;
                    }
                }
            }
            false
        } else {
            let idx = (b - b'a') as usize;
            if let Some(next_node) = &self.children[idx] {
                next_node.search_helper(bytes, index + 1)
            } else {
                false
            }
        }
    }
}
```

---

## 4. Tier 2: Hash Map Child Pointers with DFS

### 4.1 Mechanical Description
Instead of allocating 26-element pointer arrays per node, each `TrieNode` maintains a hash table from character to child node reference (`unordered_map<char, TrieNode*>` in C++, `dict` in Python).
Adding words inserts keys dynamically.
During search, a regular character checks for key containment in the hash table.
A dot character `'.'` iterates through all existing entries in the node's map.

### 4.2 Trade-offs
- Saves memory when branching factor is sparse.
- Adds hash lookup overhead and indirect memory dereferencing on every step.

---

## 5. Tier 3: Length-Bucket Hash Table with Wildcard Scanning

### 5.1 Mechanical Description
Maintain a hash table `buckets: Map[Integer, List[String]]` mapping word length to lists of inserted words.
`addWord(word)` appends `word` to `buckets[len(word)]`.
`search(word)` retrieves all words in `buckets[len(word)]` and compares each word character by character against `word`.
If all non-dot characters match, return `true`.

### 5.2 Trade-offs
- Very fast when word lengths are distributed uniformly.
- Degrades to $O(M \cdot L)$ where $M$ is the number of words of length $L$; fails if thousands of words have identical length.

---

## 6. Tier 4: Flat Array / List Scan (Brute Force Baseline)

### 6.1 Mechanical Description
Store all words in an array or list.
Upon each search call, perform a linear scan over all $N$ words, checking length equality and character-by-character compatibility.

### 6.2 Trade-offs
- Trivial to implement.
- Unacceptable $O(N \cdot L)$ time per query for large dictionaries.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Fixed Array Node Sizing**: An array of 26 pointers fits within $26 \times 8 = 208$ bytes, which spans 3 to 4 contiguous 64-byte cache lines.
2. **Wildcard Branching Factor**: With at most 2 dots per query, the maximum fanout is bounded by $26^2 = 676$ leaves, making recursive call stack overhead negligible.
3. **RAII Destructors in C++**: `std::unique_ptr` guarantees that deep Trie hierarchies are deleted cleanly when the dictionary goes out of scope without manual memory management.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| All dots query | `search("...")` | Matches any word of length 3 | Verifies depth 3 node with `is_end == true` |
| Trailing dot | `search("ba.")` | Matches "bad", "bar", etc. | Final recursive level checks `child.is_end` |
| Leading dot | `search(".ad")` | Matches "bad", "dad", etc. | Root fanout branches across all existing children |
| No match on exact word | `search("xyz")` | Returns `false` | Terminates on first null pointer |
| Non-terminal prefix match | `addWord("apple")`, `search("app")` | Returns `false` | Returns `curr.is_end` which is false at prefix node |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why is a Trie better than regex matching for this problem?
A Trie shares common prefixes among words, allowing all words with a shared prefix to be tested in a single step instead of testing each word against a regex engine.

### 2. What is the maximum number of recursive calls for a query?
Since the constraints guarantee at most 2 dots and length $\le 25$, the branching factor is at most $26^2 \times 25 \approx 16900$ steps in the theoretical worst case.

### 3. Can iterative BFS be used instead of recursive DFS for search?
Yes, using a queue of `(node, index)` pairs. However, DFS requires less auxiliary memory ($O(L)$ stack vs exponential queue width).

### 4. Why should root node not be marked `isEnd = true`?
Because an empty string was not inserted into the dictionary.

### 5. What if the word length exceeds 25?
For long strings with many wildcards, DFS can blow up exponentially; Aho-Corasick or nondeterministic finite automata (NFA) would be required.

### 6. How does word length bucketing compare in practice?
In Python on LeetCode test suites, length bucketing often runs faster than an object-heavy Trie due to Python dictionary and string comparison optimizations.

### 7. How does Rust handle recursive deallocation for `WordDictionary`?
When `WordDictionary` drops, `Option<Box<WordDictionary>>` automatically deallocates recursively via its standard `Drop` implementation.

### 8. Does `addWord` support duplicate words?
Yes, inserting an existing word simply sets `curr.is_end = true` idempotently without creating duplicate nodes.

### 9. What character set is supported?
Only lowercase English letters `'a'` through `'z'` are supported by the 26-element array.

### 10. Can words contain uppercase characters or digits?
If uppercase characters or digits were allowed, the alphabet size $\Sigma$ would increase (e.g., 62 for alphanumeric), making hash map nodes more attractive.

---

## 10. Related Problems and Systematic Progression Links

- [[0079-Word-Search]]: 2D matrix character grid search with backtracking.
- [[0139-Word-Break]]: Dynamic programming string partitioning with dictionary.
- [[0208-Implement-Trie-Prefix-Tree]]: Standard prefix tree implementation without wildcards.
- LeetCode 212 (Word Search II): Trie-guided multi-word backtracking on 2D board.
- LeetCode 677 (Map Sum Pairs): Prefix sum calculation with Trie.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/add-and-search-word-data-structure-design.cpp)
- [Python Implementation](../Python/add-and-search-word-data-structure-design.py)
- [Java Implementation](../Java/add-and-search-word-data-structure-design.java)
- [TypeScript Implementation](../TypeScript/add-and-search-word-data-structure-design.ts)
- [Go Implementation](../Golang/add-and-search-word-data-structure-design.go)
- [Rust Implementation](../Rust/add-and-search-word-data-structure-design.rs)

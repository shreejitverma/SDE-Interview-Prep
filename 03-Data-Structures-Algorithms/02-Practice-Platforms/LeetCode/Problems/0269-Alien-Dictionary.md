---
id: 0269-alien-dictionary
title: "LeetCode 269: Alien Dictionary (Topological Sort & Graph Cycle Detection Deep Dive)"
tags:
  - dsa
  - leetcode
  - graph
  - topological-sort
  - breadth-first-search
  - depth-first-search
level: hard
type: problem-breakdown
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/alien-dictionary/"
---

# LeetCode 269: Alien Dictionary (Topological Sort & Graph Cycle Detection Deep Dive)

## 1. Problem Statement and Architectural Overview

There is a new alien language that uses the English alphabet.
The order among letters is unknown to you.
You are given a list of strings `words` from the alien language's dictionary, where the strings are sorted lexicographically according to the rules of this new language.
Return a string of the unique letters in the new alien language sorted in lexicographically increasing order by the new language's rules.
If there is no valid solution, return the empty string `""`.
If there are multiple valid solutions, return any one of them.

### Critical Constraints
- $1 \le \text{words.length} \le 100$
- $1 \le \text{words}[i]\text{.length} \le 100$
- $\text{words}[i]$ consists of only lowercase English letters.

---

## 2. Mathematical Formalism and Invariant Proofs

Let $\Sigma$ denote the alphabet of lowercase English letters, $|\Sigma| \le 26$.
Let $V \subseteq \Sigma$ be the set of characters appearing anywhere in `words`.
Let $G = (V, E)$ be a directed precedence graph.

### Extraction of Directed Precedence Edges
Consider two consecutive words in the dictionary $W_A$ and $W_B$.
Let $k$ be the smallest index such that $W_A[k] \ne W_B[k]$.
If such an index $k$ exists, lexicographical sorting guarantees that $W_A[k]$ must precede $W_B[k]$ in the alien alphabet.
We construct the directed edge $(W_A[k], W_B[k]) \in E$.
If no such index $k$ exists and $|W_A| > |W_B|$, then $W_B$ is a proper prefix of $W_A$.
By definition of standard lexicographical order, a proper prefix must appear before its extension (e.g. "app" before "apple").
If $|W_A| > |W_B|$ with $W_B$ as a prefix, the input sequence violates the fundamental definition of lexicographical order, and no valid total ordering can exist ($G$ is inherently unsatisfiable, return `""`).

### Topological Ordering Invariant (DAG Property)
A valid alphabet ordering corresponds to a linear extension of the partial order defined by $G$.
By the Szpilrajn extension theorem, every strict partial order has a linear extension if and only if $G$ is a Directed Acyclic Graph (DAG).
If $G$ contains a directed cycle $C = (v_1, v_2, \dots, v_k, v_1)$, then:
$$v_1 \prec v_2 \prec \dots \prec v_k \prec v_1 \implies v_1 \prec v_1$$
This is a direct contradiction of the irreflexivity of strict total orders.
Therefore, a valid linear ordering exists if and only if Kahn's BFS algorithm consumes all $|V|$ unique vertices.

---

## 3. Four-Tier Solution Architecture

### Tier 1: Optimal Kahn's BFS with Static 26-Array Graph
- **Core Concept**: Represent adjacency lists and in-degrees using fixed-size arrays indexed by `ch - 'a'`.
- Scan all words to collect unique active characters $V$.
- Compare adjacent words to build directed edges $(u, v)$ and increment $\text{inDegree}[v]$.
- Detect invalid prefix inversions immediately during edge extraction.
- Push all vertices with $\text{inDegree} = 0$ into a FIFO queue.
- Dequeue vertex $u$, append to result string, decrement in-degrees of successors $v \in \text{Adj}[u]$, and push when $\text{inDegree}[v] = 0$.
- If $\text{result.length} < |V|$, return `""` (cycle detected).
- **Time Complexity**: $O(C)$ where $C = \sum |w_i|$ is the total character volume in `words`.
Graph traversal runs in $O(|V| + |E|) = O(26 + 26^2) = O(1)$.
- **Space Complexity**: $O(1)$ auxiliary space bounded by the English alphabet size 26.

### Tier 2: Post-Order DFS with Three-Color Cycle Detection
- **Core Concept**: Recursively traverse each component using three visit states:
  - White (0): Unvisited.
  - Gray (1): Currently exploring in recursion call stack (cycle if re-encountered).
  - Black (2): Fully processed and backtracked.
- Append vertices to the output list upon exit (reverse post-order).
- Return reversed list as the topological sort.
- **Time Complexity**: $O(C)$.
- **Space Complexity**: $O(1)$ stack and state arrays.

### Tier 3: Dynamic Map-Based BFS Graph
- **Core Concept**: Build graph using dynamic hash maps: `Map<Character, Set<Character>>`.
- Useful for generalized Unicode alphabets of arbitrary size.
- Incurs hash calculation and pointer indirection overhead per character edge.
- **Time Complexity**: $O(C \cdot \text{hash\_cost})$.
- **Space Complexity**: $O(|V| + |E|)$ heap allocated memory.

### Tier 4: Naive Exhaustive Permutation Search
- **Core Concept**: Generate all $|V|!$ permutations of unique characters.
- For each permutation, verify whether every word in `words` satisfies the proposed alphabet ordering.
- **Time Complexity**: $O(|V|! \cdot C)$ where $|V|! \le 26! \approx 4 \times 10^{26}$, which is computationally infeasible.
- **Space Complexity**: $O(|V|)$ permutation buffer.

---

## 4. Hardware, Memory, and Cache Systems Considerations

1. **Fixed-Size Contiguous Arrays**: Because $|\Sigma| = 26$, using `int inDegree[26]` and `bool present[26]` eliminates dynamic heap allocations and guarantees that all graph metadata resides in L1 data cache.
2. **Branch Prediction on Prefix Check**: The adjacent word prefix validation runs before graph construction, allowing immediate early termination on malformed input.
3. **Queue Locality**: Storing integer offsets $0 \dots 25$ in the queue allows the BFS loop to execute in a tiny register footprint.

---

## 5. Edge-Case Boundary Defense Matrix

| Edge Case Dimension | Input Scenario | Expected Behavior | Failure Mode Without Defense |
|:---|:---|:---|:---|
| Invalid Prefix Inversion | `words = ["abc", "ab"]` | Return `""` | Algorithmic false positive ordering produced |
| Direct 2-Cycle | `words = ["z", "x", "z"]` | Return `""` | Returning partial string missing cyclic characters |
| Multiple Disconnected Components | `words = ["zy", "zx"]` | Return valid order like `"zyx"` | Skipping unlinked components or stalling BFS |
| Single Character Input | `words = ["z"]` | Return `"z"` | Outputting empty string due to zero edges |
| Identical Words | `words = ["abc", "abc"]` | Return `"abc"` in any order | Adding self-loops or false cycle detection |

---

## 6. Comprehensive 10 Frequently Asked Questions (FAQ)

### 1. Why do we only extract edges from the first differing character between adjacent words?
In lexicographical ordering, characters after the first differing character provide no precedence information.
For example, knowing "abz" comes before "aca" tells us 'b' comes before 'c', but gives no information about 'z' versus 'a'.

### 2. Why must we compare only adjacent words instead of all pairs?
Lexicographical order is transitive ($W_1 \le W_2 \le W_3$).
If all adjacent pairs satisfy the ordering, all non-adjacent pairs are automatically satisfied.

### 3. How do we distinguish between an invalid dictionary and multiple valid orderings?
An invalid dictionary contains a cycle or an invalid prefix inversion, in which case the topological order is incomplete.
Multiple valid orderings occur when vertices have equal rank in the DAG; returning any valid topological order is acceptable.

### 4. What causes the invalid prefix case `["abc", "ab"]`?
In standard lexicographical sorting, "ab" is a prefix of "abc" and must strictly appear first.
If "abc" appears before "ab", no alphabet ordering can make the dictionary sorted.

### 5. Why is Kahn's algorithm preferred over DFS for this problem?
Kahn's BFS detects cycles naturally by comparing the count of visited vertices against $|V|$.
DFS requires an extra three-color state tracking mechanism and post-traversal array reversal.

### 6. Can there be disconnected components in the graph?
Yes, letters that never appear in comparative positions form independent components.
Kahn's algorithm handles disconnected components seamlessly by enqueueing all initial zero-in-degree nodes simultaneously.

### 7. What is the maximum number of unique characters?
Since all characters are lowercase English letters, $|V| \le 26$.

### 8. Does the presence of duplicate directed edges affect correctness?
Duplicate edges increment the in-degree count multiple times if not deduplicated.
In our implementation, deduplicating or using adjacency lists safely decrements in-degree once per distinct edge pair.

### 9. What is the auxiliary space complexity?
$O(1)$ space, because the number of vertices is bounded by 26 and edges are bounded by $26 \times 26 = 676$.

### 10. How does this compare to Course Schedule?
Course Schedule (LeetCode 207) tests cycle detection on arbitrary integer graphs.
Alien Dictionary additionally requires parsing the graph edges from sorted text strings and handling lexical prefix rules.

---

## 7. Related Problem Cross-References

- [[0207-Course-Schedule|LeetCode 207: Course Schedule]]
- [[0133-Clone-Graph|LeetCode 133: Clone Graph]]
- [[0200-Number-of-Islands|LeetCode 200: Number of Islands]]

---

## 8. Standalone Implementation Links

- [C++ Implementation](../C++/alien-dictionary.cpp)
- [Python Implementation](../Python/alien-dictionary.py)
- [Java Implementation](../Java/alien-dictionary.java)
- [TypeScript Implementation](../TypeScript/alien-dictionary.ts)
- [Go Implementation](../Golang/alien-dictionary.go)
- [Rust Implementation](../Rust/alien-dictionary.rs)

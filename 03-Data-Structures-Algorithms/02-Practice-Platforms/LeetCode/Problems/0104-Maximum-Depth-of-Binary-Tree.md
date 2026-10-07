---
id: leetcode-0104-maximum-depth-of-binary-tree
title: "LeetCode 0104: Maximum Depth of Binary Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - dfs
  - recursion
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/maximum-depth-of-binary-tree/"
---

# LeetCode 0104: Maximum Depth of Binary Tree

## 1. Problem Formalization and Constraints

Given the root of a binary tree, return its maximum depth.
A binary tree's maximum depth is the number of nodes along the longest path from the root node down to the farthest leaf node.

### Constraints
- The number of nodes in the tree is in the range $[0, 10^4]$.
- $-100 \le \text{Node.val} \le 100$

### Examples
- **Example 1**:
  - Input: `root = [3,9,20,null,null,15,7]`
  - Output: `3`
- **Example 2**:
  - Input: `root = [1,null,2]`
  - Output: `2`
- **Example 3**:
  - Input: `root = []`
  - Output: `0`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Post-Order Recursive DFS | $O(N)$ | $O(H)$ | Bottom-up recurrence $\text{depth} = 1 + \max(\text{left}, \text{right})$; minimal overhead. |
| **Tier 2 (Space-Optimized Alternative)** | Level-Order Queue BFS | $O(N)$ | $O(W)$ | Counts outer loop iterations while draining horizontal layers batch by batch. |
| **Tier 3 (Stack DFS)** | Iterative DFS with Depth Pairs | $O(N)$ | $O(H)$ | Simulates call stack explicitly using `(node, current_depth)` pairs. |
| **Tier 4 (Brute Force)** | Path Enumeration to All Leaves | $O(N \times H)$ | $O(N \times H)$ | Generates all root-to-leaf paths as explicit node lists and measures max list length. |

---

## 3. Tier 1: Most Optimal Solution (Post-Order Recursive DFS)

### 3.1 Algorithmic Mechanics and Invariant Proof

The maximum depth of a binary tree rooted at `node` is inductively defined by the recurrence:
$$\text{depth}(\text{node}) = \begin{cases} 0 & \text{if node is null} \\ 1 + \max(\text{depth}(\text{node.left}), \text{depth}(\text{node.right})) & \text{otherwise} \end{cases}$$

**Invariant Proof**:
Base case: For an empty tree (`root == nullptr`), there are 0 nodes on any path; returning 0 is correct.
Inductive step: Assume the induction hypothesis holds for all trees with fewer than $N$ nodes.
For a tree of $N$ nodes, both `node.left` and `node.right` are subtrees with strictly fewer than $N$ nodes.
By the induction hypothesis, recursive calls accurately return the maximum node counts from `node.left` and `node.right` to their respective deepest leaves.
Any path from `node` to a leaf must traverse through either `node.left` or `node.right`.
Taking $1 + \max(\text{left}, \text{right})$ accounts for the edge to the deepest child plus `node` itself, proving exact correctness.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Every node is visited once during the bottom-up traversal.
- **Auxiliary Space Complexity**: $O(H)$, where $H$ is the tree height. For a balanced tree $H = O(\log N)$; for a skewed tree $H = O(N)$ call frames.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <algorithm>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    int maxDepth(TreeNode* root) {
        if (!root) return 0;
        return 1 + std::max(maxDepth(root->left), maxDepth(root->right));
    }
};
```

#### Python 3
```python
from typing import Optional

class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

class Solution:
    def maxDepth(self, root: Optional[TreeNode]) -> int:
        if not root:
            return 0
        return 1 + max(self.maxDepth(root.left), self.maxDepth(root.right))
```

#### Java 21
```java
class Solution {
    public int maxDepth(TreeNode root) {
        if (root == null) return 0;
        return 1 + Math.max(maxDepth(root.left), maxDepth(root.right));
    }
}
```

#### TypeScript 5
```typescript
function maxDepth(root: TreeNode | null): number {
    if (!root) return 0;
    return 1 + Math.max(maxDepth(root.left), maxDepth(root.right));
}
```

#### Go 1.22
```go
package main

func maxDepth(root *TreeNode) int {
	if root == nil {
		return 0
	}
	leftDepth := maxDepth(root.Left)
	rightDepth := maxDepth(root.Right)
	if leftDepth > rightDepth {
		return 1 + leftDepth
	}
	return 1 + rightDepth
}
```

#### Rust 2021
```rust
use std::rc::Rc;
use std::cell::RefCell;
use std::cmp;

impl Solution {
    pub fn max_depth(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        match root {
            None => 0,
            Some(node) => {
                let n = node.borrow();
                1 + cmp::max(
                    Self::max_depth(n.left.clone()),
                    Self::max_depth(n.right.clone()),
                )
            }
        }
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Level-Order Queue BFS)

### 4.1 Algorithmic Mechanics
For wide shallow trees, or when recursion depth risks call stack exhaustion on deeply nested single-branch trees, an iterative BFS traversal is ideal:
- Enqueue `root`.
- Initialize `depth = 0`.
- While `queue` is not empty, increment `depth++`.
- Snapshot `levelSize = queue.size()` and pop that many nodes, inserting non-null children.
- When the queue is drained, `depth` holds the exact number of horizontal tiers.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(W)$ where $W$ is the tree's maximum width.

### 4.3 Implementation (C++20)
```cpp
#include <queue>

class Solution {
public:
    int maxDepth(TreeNode* root) {
        if (!root) return 0;

        std::queue<TreeNode*> q;
        q.push(root);
        int depth = 0;

        while (!q.empty()) {
            size_t sz = q.size();
            ++depth;
            for (size_t i = 0; i < sz; ++i) {
                TreeNode* node = q.front();
                q.pop();
                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }
        }

        return depth;
    }
};
```

---

## 5. Tier 3: Stack-Based Alternative (Iterative DFS with Node-Depth Pairs)

### 5.1 Algorithmic Mechanics
Simulates call frames on heap memory using an explicit stack of pairs `(TreeNode*, int current_depth)`:
- Push `(root, 1)` onto the stack.
- While the stack is not empty, pop `(node, d)`.
- Update `max_d = std::max(max_d, d)`.
- If `node->right` exists, push `(node->right, d + 1)`.
- If `node->left` exists, push `(node->left, d + 1)`.
This guarantees $O(H)$ memory usage while preventing operating system thread stack overflow.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(H)$ auxiliary stack frames on the heap.

### 5.3 Implementation (C++20)
```cpp
#include <stack>
#include <utility>
#include <algorithm>

class Solution {
public:
    int maxDepth(TreeNode* root) {
        if (!root) return 0;

        std::stack<std::pair<TreeNode*, int>> st;
        st.push({root, 1});
        int maxD = 0;

        while (!st.empty()) {
            auto [node, d] = st.top();
            st.pop();
            maxD = std::max(maxD, d);

            if (node->right) st.push({node->right, d + 1});
            if (node->left) st.push({node->left, d + 1});
        }

        return maxD;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Path Enumeration to All Leaves)

### 6.1 Algorithmic Mechanics
Recursively enumerates all distinct root-to-leaf paths as materialized arrays.
Once all paths are generated, computes $\max_{\text{path} \in \text{paths}} |\text{path}|$.
This wastes substantial memory storing duplicated prefix node references across multiple branches.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N \times H)$.
- **Space Complexity**: $O(N \times H)$ memory for path storage.

### 6.3 Implementation (Python 3)
```python
from typing import Optional, List

class Solution:
    def maxDepth(self, root: Optional[TreeNode]) -> int:
        if not root:
            return 0

        paths: List[List[int]] = []

        def find_paths(node: Optional[TreeNode], current_path: List[int]) -> None:
            if not node:
                return
            new_path = current_path + [node.val]
            if not node.left and not node.right:
                paths.append(new_path)
                return
            find_paths(node.left, new_path)
            find_paths(node.right, new_path)

        find_paths(root, [])
        return max(len(p) for p in paths)
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. What is the difference between node depth and node height?</summary>
Depth is the distance (number of edges or nodes) from the root down to the given node.
Height is the distance from the given node down to its farthest leaf.
The maximum depth of the tree equals the height of the root node.
</details>

<details>
<summary>2. How does the maximum call stack depth compare between balanced and skewed trees?</summary>
In a balanced tree, maximum recursion depth is $\lfloor \log_2 N \rfloor + 1$.
In a skewed tree (e.g., each node has only a right child), maximum recursion depth is $N$.
</details>

<details>
<summary>3. Why is tail call optimization (TCO) generally inapplicable to standard recursive binary tree depth?</summary>
Because each invocation requires two non-tail recursive calls (`depth(left)` and `depth(right)`) followed by a combining operation `1 + max(...)`, the compiler cannot reuse the activation frame.
</details>

<details>
<summary>4. What happens when the tree has 0 nodes (`root == nullptr`)?</summary>
The base case triggers immediately, returning 0 in $O(1)$ operations without executing loops or allocations.
</details>

<details>
<summary>5. How does Maximum Depth differ from Minimum Depth (LeetCode 111)?</summary>
In Minimum Depth, if a node has only one child, you cannot take $\min(\text{depth}(\text{left}), \text{depth}(\text{right}))$ because the null child would return 0, which is not a valid leaf.
Maximum Depth safely uses $\max$ across both subtrees unconditionally.
</details>

<details>
<summary>6. Can Morris Traversal calculate tree depth in $O(1)$ space?</summary>
Yes, by tracking current depth while descending and ascending along threaded pointers, updating the global maximum depth at leaf discoveries.
</details>

<details>
<summary>7. Why does iterative BFS use less memory than recursive DFS on degenerate trees?</summary>
On a degenerate skewed tree of size $N$, BFS holds at most 1 element in its queue ($O(1)$ auxiliary space), whereas recursive DFS pushes $N$ stack frames ($O(N)$ space).
</details>

<details>
<summary>8. How does `std::max` behave when both subtrees have equal depth?</summary>
It returns the common depth value, and adding 1 accounts for the current ancestor node.
</details>

<details>
<summary>9. What is the impact of deeply nested trees on stack overflow limits?</summary>
Many production environments allocate only 1MB to 8MB for thread stack size.
A skewed tree with $10^5$ nodes may trigger a `StackOverflowError` in recursive implementations, necessitating iterative BFS or iterative heap-based DFS.
</details>

<details>
<summary>10. How does Maximum Depth relate to Binary Tree Diameter (LeetCode 543)?</summary>
The diameter through any node is $\text{depth}(\text{left}) + \text{depth}(\text{right})$.
Maximum Depth computes the components needed for diameter calculation in a single post-order pass.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/maximum-depth-of-binary-tree.cpp)
- [Python Implementation](../Python/maximum-depth-of-binary-tree.py)
- [Java Implementation](../Java/maximum-depth-of-binary-tree.java)
- [TypeScript Implementation](../TypeScript/maximum-depth-of-binary-tree.ts)
- [Go Implementation](../Golang/maximum-depth-of-binary-tree.go)
- [Rust Implementation](../Rust/maximum-depth-of-binary-tree.rs)

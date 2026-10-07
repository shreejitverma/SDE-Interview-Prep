---
id: leetcode-0100-same-tree
title: "LeetCode 0100: Same Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - depth-first-search
  - breadth-first-search
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/same-tree/"
---

# LeetCode 0100: Same Tree

## 1. Problem Formalization and Constraints

Given the roots of two binary trees `p` and `q`, write a function to check if they are the same or not.
Two binary trees are considered the same if they are structurally identical, and the nodes have the same value.

### Constraints
- The number of nodes in both trees is in the range $[0, 100]$.
- $-10^4 \le \text{Node.val} \le 10^4$

### Examples
- **Example 1**:
  - Input: `p = [1,2,3]`, `q = [1,2,3]`
  - Output: `true`
- **Example 2**:
  - Input: `p = [1,2]`, `q = [1,null,2]`
  - Output: `false`
- **Example 3**:
  - Input: `p = [1,2,1]`, `q = [1,1,2]`
  - Output: `false`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Recursive Pre-Order DFS | $O(N)$ | $O(H)$ | Recursively evaluates node value equivalence and structural identity; short-circuits on first mismatch; minimal call-stack overhead. |
| **Tier 2 (Iterative BFS)** | Level-Order Queue with Node Pairs | $O(N)$ | $O(W)$ | Enqueues corresponding pairs `(p_curr, q_curr)` in a BFS queue; prevents deep recursive call stacks; space bounded by maximum tree width $W$. |
| **Tier 3 (Iterative DFS)** | Explicit Stack with Node Pairs | $O(N)$ | $O(H)$ | Simulates recursion stack explicitly; pairs of nodes popped and compared; avoids function call overhead. |
| **Tier 4 (Tree Serialization)** | Canonical Pre-Order String Serialization | $O(N)$ | $O(N)$ | Serializes both trees into strings including null markers (e.g. `1,2,#,#,3,#,#`); string equality test; incurs string concatenation allocations. |

*Notation*: $N$ is the number of nodes in the smaller tree, $H$ is the tree height ($O(\log N)$ to $O(N)$), and $W$ is the maximum tree width.

---

## 3. Tier 1: Most Optimal Solution (Recursive Pre-Order DFS)

### 3.1 Algorithmic Mechanics and Invariant Proof

The recursive function `isSameTree(p, q)` verifies two conditions:
1. **Base Cases (Structural Comparison)**:
   - If both `p == null` and `q == null`, both trees are empty and identical. Return `true`.
   - If exactly one of `p` or `q` is `null`, their structures differ. Return `false`.
2. **Value and Subtree Comparison**:
   - If `p.val != q.val`, node values differ. Return `false`.
   - If `p.val == q.val`, recursively check whether both left subtrees and right subtrees are identical:
     $$\text{isSameTree}(p.\text{left}, q.\text{left}) \land \text{isSameTree}(p.\text{right}, q.\text{right})$$

**Invariant Proof**:
Let $T_p$ and $T_q$ be two binary trees.
Two trees are identical $\iff$ either both are empty, or their root values match and their respective left subtrees and right subtrees are identical.
Base case: If both trees have size 0, both are `null`, and the algorithm returns `true`.
If one has size 0 and the other has size $> 0$, the algorithm returns `false`.
Inductive step: Assume the algorithm correctly determines tree equivalence for all trees of size $< K$.
For trees of size $K$, the algorithm checks root equality $p.\text{val} == q.\text{val}$.
Because the left and right subtrees strictly have size $< K$, the inductive hypothesis ensures that both recursive calls return `true` if and only if the subtrees are identical.
By logical conjunction, `isSameTree(p, q)` returns `true` if and only if both tree structures and all node values are identical.
The induction is complete.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ where $N$ is the number of nodes in the smaller tree. The algorithm terminates as soon as a mismatch is detected, and visits each node at most once.
- **Auxiliary Space Complexity**: $O(H)$ where $H$ is the tree height, consumed by the recursion call stack ($O(\log N)$ for balanced trees, $O(N)$ for completely degenerate trees).

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (!p && !q) {
            return true;
        }
        if (!p || !q || p->val != q->val) {
            return false;
        }
        return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N)
# Space: O(H)

# Definition for a binary tree node.
# class TreeNode:
#     def __init__(self, val=0, left=None, right=None):
#         self.val = val
#         self.left = left
#         self.right = right

class Solution:
    def isSameTree(self, p: Optional[TreeNode], q: Optional[TreeNode]) -> bool:
        if not p and not q:
            return True
        if not p or not q or p.val != q.val:
            return False
        return self.isSameTree(p.left, q.left) and self.isSameTree(p.right, q.right)
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

/*
// Definition for a binary tree node.
public class TreeNode {
    int val;
    TreeNode left;
    TreeNode right;
    TreeNode() {}
    TreeNode(int val) { this.val = val; }
    TreeNode(int val, TreeNode left, TreeNode right) {
        this.val = val;
        this.left = left;
        this.right = right;
    }
}
*/

class Solution {
    public boolean isSameTree(TreeNode p, TreeNode q) {
        if (p == null && q == null) {
            return true;
        }
        if (p == null || q == null || p.val != q.val) {
            return false;
        }
        return isSameTree(p.left, q.left) && isSameTree(p.right, q.right);
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

/**
 * Definition for a binary tree node.
 * class TreeNode {
 *     val: number
 *     left: TreeNode | null
 *     right: TreeNode | null
 *     constructor(val?: number, left?: TreeNode | null, right?: TreeNode | null) {
 *         this.val = (val===undefined ? 0 : val)
 *         this.left = (left===undefined ? null : left)
 *         this.right = (right===undefined ? null : right)
 *     }
 * }
 */

function isSameTree(p: TreeNode | null, q: TreeNode | null): boolean {
    if (!p && !q) {
        return true;
    }
    if (!p || !q || p.val !== q.val) {
        return false;
    }
    return isSameTree(p.left, q.left) && isSameTree(p.right, q.right);
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

package main

/**
 * Definition for a binary tree node.
 * type TreeNode struct {
 *     Val int
 *     Left *TreeNode
 *     Right *TreeNode
 * }
 */

func isSameTree(p *TreeNode, q *TreeNode) bool {
	if p == nil && q == nil {
		return true
	}
	if p == nil || q == nil || p.Val != q.Val {
		return false
	}
	return isSameTree(p.Left, q.Left) && isSameTree(p.Right, q.Right)
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

use std::cell::RefCell;
use std::rc::Rc;

#[derive(Debug, PartialEq, Eq)]
pub struct TreeNode {
    pub val: i32,
    pub left: Option<Rc<RefCell<TreeNode>>>,
    pub right: Option<Rc<RefCell<TreeNode>>>,
}

pub struct Solution;

impl Solution {
    pub fn is_same_tree(
        p: Option<Rc<RefCell<TreeNode>>>,
        q: Option<Rc<RefCell<TreeNode>>>,
    ) -> bool {
        match (p, q) {
            (None, None) => true,
            (Some(p_node), Some(q_node)) => {
                let p_borrow = p_node.borrow();
                let q_borrow = q_node.borrow();
                p_borrow.val == q_borrow.val
                    && Self::is_same_tree(p_borrow.left.clone(), q_borrow.left.clone())
                    && Self::is_same_tree(p_borrow.right.clone(), q_borrow.right.clone())
            }
            _ => false,
        }
    }
}
```

---

## 4. Tier 2: Iterative BFS with Synchronized Queue

### 4.1 Mechanical Description
Use a single queue holding pairs of nodes `(p, q)`.
Enqueue `(rootP, rootQ)`.
While queue is non-empty, pop `(nodeP, nodeQ)`:
- If both are null, continue.
- If one is null or `nodeP.val != nodeQ.val`, return `false`.
- Enqueue `(nodeP.left, nodeQ.left)` and `(nodeP.right, nodeQ.right)`.
If queue is exhausted without mismatch, return `true`.

### 4.2 Trade-offs
- Completely avoids recursive stack overflow.
- Allocates heap memory for queue elements.

---

## 5. Tier 3: Iterative DFS with Pair Stack

### 5.1 Mechanical Description
Maintain an explicit stack of node pairs.
Push `(rootP, rootQ)`.
While stack is not empty, pop pair and compare values; push corresponding child pairs onto the stack.

### 5.2 Trade-offs
- Retains $O(H)$ memory bound without system call stack limitations.
- Slightly more verbose than recursive formulation.

---

## 6. Tier 4: Canonical Tree Serialization Comparison (Brute Force Baseline)

### 6.1 Mechanical Description
Perform pre-order traversal on both trees, serializing null nodes explicitly as `'#'` separated by commas.
Compare the two resulting strings for equality.

### 6.2 Trade-offs
- Visually simple and verifiable.
- Requires building full strings of size $O(N)$, consuming unnecessary memory and preventing early-exit optimizations on first mismatch.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Short-Circuit Evaluation**: Using `&&` in the boolean return statement short-circuits execution as soon as `left` subtrees mismatch, avoiding traversal of right subtrees.
2. **Cache Performance**: Traversal matches the pointer layout of the node structs in memory, yielding high L1 cache hit rates.
3. **Rust Pattern Matching**: Using `match (p, q)` leverages Rust's compiler optimizations to generate efficient jump tables for option handling.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Both trees empty | `p = null, q = null` | Returns `true` | Base case `!p && !q` triggers |
| One tree empty | `p = [1], q = null` | Returns `false` | `!p || !q` catches asymmetry |
| Mirrored structure | `p = [1, 2, null], q = [1, null, 2]` | Returns `false` | Left vs right structural mismatch |
| Same structure, different values | `p = [1, 2], q = [1, 3]` | Returns `false` | `p.val != q.val` catches value inequality |
| Large balanced trees | $N = 100$ | Returns `true` in $O(N)$ | Runs in $< 1$ millisecond |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What is the difference between Same Tree (100) and Symmetric Tree (101)?
Same Tree compares two distinct trees for exact structural equivalence. Symmetric Tree compares a single tree against its own reflection (left child compared to right child).

### 2. Can we compare trees using in-order traversal alone?
No. Different tree structures can produce identical in-order traversals (for example `[1, 2]` vs `[2, 1]`). Structural comparison requires both structure and values.

### 3. Does post-order traversal work as well as pre-order?
Yes, any traversal that checks corresponding nodes in both trees works. Pre-order is preferred because it checks root values first, enabling early exit.

### 4. What happens when both trees are null?
The base check `if (!p && !q) return true;` returns true, representing empty trees.

### 5. How many nodes can the trees have?
The constraints state $0 \le N \le 100$, which makes execution virtually instantaneous.

### 6. Can node values be negative?
Yes, node values range from $-10^4$ to $10^4$. Value comparison handles negative values correctly.

### 7. How does Rust compare `Option<Rc<RefCell<TreeNode>>>`?
Rust's `match (p, q)` checks whether both options are `Some` or `None`, and borrows the inner nodes for value comparison.

### 8. What is the maximum height of the tree?
In the worst case (skewed tree), $H = N = 100$.

### 9. Why is tree serialization considered suboptimal?
Because serialization consumes $O(N)$ extra memory for strings and traverses both entire trees even when their root values differ.

### 10. Does this algorithm work for general N-ary trees?
Yes. Compare root values and then iterate over all corresponding children in parallel: `isSameTree(p.children[i], q.children[i])`.

---

## 10. Related Problems and Systematic Progression Links

- [[0104-Maximum-Depth-of-Binary-Tree]]: Binary tree recursive depth evaluation.
- [[0226-Invert-Binary-Tree]]: Binary tree structural inversion.
- [[0236-Lowest-Common-Ancestor-of-a-Binary-Tree]]: Binary tree post-order ancestor resolution.
- LeetCode 101 (Symmetric Tree): Checking if a tree is a mirror of itself.
- LeetCode 572 (Subtree of Another Tree): Testing if one tree is a subtree of another using tree matching.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/same-tree.cpp)
- [Python Implementation](../Python/same-tree.py)
- [Java Implementation](../Java/same-tree.java)
- [TypeScript Implementation](../TypeScript/same-tree.ts)
- [Go Implementation](../Golang/same-tree.go)
- [Rust Implementation](../Rust/same-tree.rs)

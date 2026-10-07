---
id: leetcode-0236-lowest-common-ancestor-of-a-binary-tree
title: "LeetCode 0236: Lowest Common Ancestor of a Binary Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - depth-first-search
  - recursion
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree/"
---

# LeetCode 0236: Lowest Common Ancestor of a Binary Tree

## 1. Problem Formalization and Constraints

Given a binary tree, find the lowest common ancestor (LCA) of two given nodes `p` and `q`.
According to the definition of LCA on Wikipedia: "The lowest common ancestor is defined between two nodes `p` and `q` as the lowest node in `T` that has both `p` and `q` as descendants (where we allow a node to be a descendant of itself)."

### Constraints
- The number of nodes in the tree is in the range $[2, 10^5]$.
- $-10^9 \le \text{Node.val} \le 10^9$
- All `Node.val` are unique.
- `p != q`
- `p` and `q` will exist in the tree.

### Examples
- **Example 1**:
  - Input: `root = [3,5,1,6,2,0,8,null,null,7,4]`, `p = 5`, `q = 1`
  - Output: `3`
  - Explanation: The LCA of nodes `5` and `1` is `3`.
- **Example 2**:
  - Input: `root = [3,5,1,6,2,0,8,null,null,7,4]`, `p = 5`, `q = 4`
  - Output: `5`
  - Explanation: The LCA of nodes `5` and `4` is `5`, since a node can be a descendant of itself according to the LCA definition.
- **Example 3**:
  - Input: `root = [1,2]`, `p = 1`, `q = 2`
  - Output: `1`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Post-Order Recursive Traversal (Bottom-Up) | $O(N)$ | $O(H)$ | Bubbles up matches from leaves to root; returns node immediately when both left and right return non-null; optimal $O(1)$ memory overhead beyond recursion stack. |
| **Tier 2 (Parent Pointers)** | Iterative Parent Hash Map & Ancestor Set | $O(N)$ | $O(N)$ | BFS/DFS records parent pointers in a hash table; builds ancestor path for `p`, then walks `q` upwards until intersection; uses $O(N)$ auxiliary table space. |
| **Tier 3 (Path Tracing)** | Root-to-Node Path Finding | $O(N)$ | $O(H)$ | Discovers linear node paths from `root` to `p` and `root` to `q`; finds the last matching node; requires path storage and separate traversals. |
| **Tier 4 (Brute Force)** | Top-Down Subtree Search | $O(N^2)$ | $O(H)$ | Tests if `root` contains both `p` and `q`; recurses on left or right child until the split point; redundant subtree scans lead to quadratic runtime. |

*Notation*: $N$ is total tree nodes, and $H$ is the tree height ($O(\log N)$ best-case, $O(N)$ worst-case).

---

## 3. Tier 1: Most Optimal Solution (Post-Order Recursive Traversal)

### 3.1 Algorithmic Mechanics and Invariant Proof

The bottom-up post-order DFS operates with the following recurrence relation:
1. **Base Cases**:
   - If `root` is `null`, return `null`.
   - If `root == p` or `root == q`, return `root`.
2. **Recursive Decomposition**:
   - Recurse on the left subtree: `left = lowestCommonAncestor(root.left, p, q)`.
   - Recurse on the right subtree: `right = lowestCommonAncestor(root.right, p, q)`.
3. **Bottom-Up Synthesis**:
   - If both `left != null` and `right != null`, `p` and `q` lie in separate subtrees of `root`. Hence, `root` is their Lowest Common Ancestor. Return `root`.
   - If only one child subtree returns non-null (`left != null` or `right != null`), return the non-null result upwards.
   - If both return `null`, return `null`.

**Invariant Proof**:
Let $T_u$ denote the subtree rooted at $u$.
Assume both $p$ and $q$ exist in the tree.
Case 1: $p$ is an ancestor of $q$.
When DFS visits $p$, it encounters `root == p` and immediately returns $p$ without searching deeper into $T_p$.
Because $q \in T_p$, the other branch of the tree containing no targets returns `null`.
At every ancestor of $p$, only the branch containing $p$ returns non-null, bubbling $p$ all the way to the top.
Thus $p$ is correctly returned as the LCA.
Case 2: Neither is an ancestor of the other.
Then there exists a unique lowest node $w$ such that $p \in T_{w.\text{left}}$ and $q \in T_{w.\text{right}}$ (or vice versa).
For this node $w$, both recursive calls return non-null pointers ($p$ and $q$ respectively).
Node $w$ detects `left != null && right != null` and returns $w$.
For every proper ancestor $a$ of $w$, the entire pair $\{p, q\}$ resides exclusively within one child branch of $a$, so that branch returns $w$ while the other branch returns `null`.
Node $a$ therefore propagates $w$ upwards without modification.
The algorithm is therefore correct and exact for all inputs.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ where $N$ is the number of nodes in the binary tree. In the worst case, every node is visited once.
- **Auxiliary Space Complexity**: $O(H)$ auxiliary space where $H$ is tree height, consumed by the recursion call stack. For a balanced tree, $H = O(\log N)$; for a skewed tree, $H = O(N)$.

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
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if (!root || root == p || root == q) {
            return root;
        }

        TreeNode* left = lowestCommonAncestor(root->left, p, q);
        TreeNode* right = lowestCommonAncestor(root->right, p, q);

        if (left && right) {
            return root;
        }

        return left ? left : right;
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
#     def __init__(self, x):
#         self.val = x
#         self.left = None
#         self.right = None

class Solution:
    def lowestCommonAncestor(self, root: 'TreeNode', p: 'TreeNode', q: 'TreeNode') -> 'TreeNode':
        if not root or root == p or root == q:
            return root

        left = self.lowestCommonAncestor(root.left, p, q)
        right = self.lowestCommonAncestor(root.right, p, q)

        if left and right:
            return root

        return left if left else right
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
    TreeNode(int x) { val = x; }
}
*/

class Solution {
    public TreeNode lowestCommonAncestor(TreeNode root, TreeNode p, TreeNode q) {
        if (root == null || root == p || root == q) {
            return root;
        }

        TreeNode left = lowestCommonAncestor(root.left, p, q);
        TreeNode right = lowestCommonAncestor(root.right, p, q);

        if (left != null && right != null) {
            return root;
        }

        return left != null ? left : right;
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

function lowestCommonAncestor(
    root: TreeNode | null,
    p: TreeNode | null,
    q: TreeNode | null
): TreeNode | null {
    if (!root || root === p || root === q) {
        return root;
    }

    const left = lowestCommonAncestor(root.left, p, q);
    const right = lowestCommonAncestor(root.right, p, q);

    if (left && right) {
        return root;
    }

    return left !== null ? left : right;
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

func lowestCommonAncestor(root, p, q *TreeNode) *TreeNode {
	if root == nil || root == p || root == q {
		return root
	}

	left := lowestCommonAncestor(root.Left, p, q)
	right := lowestCommonAncestor(root.Right, p, q)

	if left != nil && right != nil {
		return root
	}

	if left != nil {
		return left
	}
	return right
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

impl TreeNode {
    #[inline]
    pub fn new(val: i32) -> Self {
        TreeNode {
            val,
            left: None,
            right: None,
        }
    }
}

pub struct Solution;

impl Solution {
    pub fn lowest_common_ancestor(
        root: Option<Rc<RefCell<TreeNode>>>,
        p: Option<Rc<RefCell<TreeNode>>>,
        q: Option<Rc<RefCell<TreeNode>>>,
    ) -> Option<Rc<RefCell<TreeNode>>> {
        let p_node = p.as_ref()?;
        let q_node = q.as_ref()?;
        Self::helper(&root, p_node, q_node)
    }

    fn helper(
        root: &Option<Rc<RefCell<TreeNode>>>,
        p: &Rc<RefCell<TreeNode>>,
        q: &Rc<RefCell<TreeNode>>,
    ) -> Option<Rc<RefCell<TreeNode>>> {
        let curr = root.as_ref()?;
        if Rc::ptr_eq(curr, p) || Rc::ptr_eq(curr, q) {
            return Some(Rc::clone(curr));
        }

        let left = Self::helper(&curr.borrow().left, p, q);
        let right = Self::helper(&curr.borrow().right, p, q);

        match (left, right) {
            (Some(_), Some(_)) => Some(Rc::clone(curr)),
            (Some(l), None) => Some(l),
            (None, Some(r)) => Some(r),
            (None, None) => None,
        }
    }
}
```

---

## 4. Tier 2: Iterative Parent Pointer Hash Map & Ancestor Set

### 4.1 Mechanical Description
Use a queue or stack to traverse the tree (BFS or DFS) until both `p` and `q` are visited.
Maintain a hash table `parent: Map[TreeNode, TreeNode]` recording the parent pointer of every discovered node.
Once both nodes are in `parent`, traverse upwards from `p` to `root`, adding all ancestors to a hash set `ancestors`.
Next, traverse upwards from `q` using the parent map; the first node encountered in `ancestors` is the Lowest Common Ancestor.

### 4.2 Trade-offs
- Avoids deep recursion call stacks.
- Consumes $O(N)$ extra memory for the parent pointer map and ancestor set.

---

## 5. Tier 3: Root-to-Node Path Finding

### 5.1 Mechanical Description
Perform two DFS traversals from `root` to identify the path to `p` (`pathP`) and the path to `q` (`pathQ`).
Each path is a sequence of nodes starting with `root`.
Compare the two paths sequentially until the elements diverge: `pathP[i] != pathQ[i]`.
The element immediately preceding the divergence point (`pathP[i-1]`) is the LCA.

### 5.2 Trade-offs
- Conceptually intuitive and clean to debug.
- Requires two separate passes and additional list allocations for storing paths.

---

## 6. Tier 4: Top-Down Subtree Search (Brute Force Baseline)

### 6.1 Mechanical Description
Write a helper function `contains(node, target)` that checks whether `target` exists in the subtree of `node`.
Starting at `root`:
- If both `p` and `q` are contained in `root.left`, recurse on `root.left`.
- If both are contained in `root.right`, recurse on `root.right`.
- Otherwise, `root` is the split point, so return `root`.

### 6.2 Trade-offs
- Simple top-down approach.
- Incurs $O(N^2)$ quadratic worst-case runtime because nodes at lower levels are scanned repeatedly.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Stack Frame Optimization**: Modern compilers can optimize recursive leaf-checks and branch returns into register operations, minimizing cache pressure.
2. **Short-Circuiting Evaluation**: When `root == p` or `root == q`, child traversal is skipped entirely, saving work when target nodes reside near the tree root.
3. **Deep Tree Stack Safety**: For trees of depth $10^5$, an iterative approach or heap-allocated explicit stack is required to avoid platform call-stack limits.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Node is ancestor of other | `p = 5`, `q = 4` (where 4 is inside 5's subtree) | Returns `5` | Short-circuits at `root == p`, returning `p` |
| Root is one of the nodes | `root = p` | Returns `root` | `root == p` base check returns root directly |
| Balanced two-node tree | `root = 1`, `root.left = 2` | Returns `1` | `left` returns 2, `right` returns null; returns 1 |
| Deep skewed tree | $N = 10^5$ line graph | Returns correct ancestor | Tail recursion or increased stack limit |
| Target nodes in opposite subtrees | `p` in left tree, `q` in right tree | Returns `root` | `left && right` branch triggers |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What is the fundamental difference between LeetCode 235 and LeetCode 236?
LeetCode 235 assumes a Binary Search Tree (BST) where value comparisons determine whether to move left or right in $O(H)$ time without searching both sides. LeetCode 236 is a general binary tree requiring DFS traversal of both subtrees.

### 2. What happens if one of the nodes does not exist in the tree?
Under the current constraints, both nodes are guaranteed to exist. If either could be absent (as in LeetCode 1644), an additional count flag must verify both were visited before confirming the LCA.

### 3. Why does the algorithm not traverse deeper when `root == p` is met?
Because if `q` is in `p`'s subtree, `p` is already the LCA. If `q` is elsewhere, `q` will be found in a parallel subtree and bubble up to meet `p` at their true ancestor.

### 4. Can node values be duplicates?
The problem constraints guarantee unique node values. If duplicates existed, pointer/reference equality would still resolve the correct nodes.

### 5. How does Rust handle pointer equality for tree nodes?
Rust uses `Rc::ptr_eq(&node1, &node2)` to test reference equality without borrowing or copying data.

### 6. Can this be solved with Tarjan's off-line lowest common ancestors algorithm?
Yes, Tarjan's algorithm uses disjoint-set union (Union-Find) to answer multiple LCA queries in $O(\alpha(N))$ time per query offline.

### 7. Does this solution work for multi-way (N-ary) trees?
Yes, simply loop over all children of `root`; if two distinct children return non-null, `root` is the LCA.

### 8. What is the minimum number of nodes in the input?
The constraints specify at least 2 nodes.

### 9. Why is the time complexity $O(N)$ and not $O(H)$?
Because in an arbitrary binary tree without order invariants, we cannot predict which branch contains the target nodes and may need to visit all nodes.

### 10. Can the Lowest Common Ancestor be computed using binary lifting?
Yes, binary lifting precomputes $2^k$-th ancestors in $O(N \log N)$ preprocessing and answers any LCA query in $O(\log N)$ time, commonly used in tree query systems.

---

## 10. Related Problems and Systematic Progression Links

- [[0104-Maximum-Depth-of-Binary-Tree]]: Basic binary tree recursive traversal.
- [[0124-Binary-Tree-Maximum-Path-Sum]]: Bottom-up subtree post-order synthesis.
- [[0226-Invert-Binary-Tree]]: Structural manipulation of binary trees.
- [[0235-Lowest-Common-Ancestor-of-a-Binary-Search-Tree]]: Binary search tree specialized LCA in $O(H)$ time.
- LeetCode 1644 (Lowest Common Ancestor of a Binary Tree II): LCA when nodes may not exist in the tree.
- LeetCode 1650 (Lowest Common Ancestor of a Binary Tree III): LCA when nodes have direct parent pointers.
- LeetCode 1676 (Lowest Common Ancestor of a Binary Tree IV): LCA of multiple nodes.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/lowest-common-ancestor-of-a-binary-tree.cpp)
- [Python Implementation](../Python/lowest-common-ancestor-of-a-binary-tree.py)
- [Java Implementation](../Java/lowest-common-ancestor-of-a-binary-tree.java)
- [TypeScript Implementation](../TypeScript/lowest-common-ancestor-of-a-binary-tree.ts)
- [Go Implementation](../Golang/lowest-common-ancestor-of-a-binary-tree.go)
- [Rust Implementation](../Rust/lowest-common-ancestor-of-a-binary-tree.rs)

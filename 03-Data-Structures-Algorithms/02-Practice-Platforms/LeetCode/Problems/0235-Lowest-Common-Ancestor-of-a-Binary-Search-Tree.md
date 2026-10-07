---
id: leetcode-0235-lowest-common-ancestor-of-a-binary-search-tree
title: "LeetCode 0235: Lowest Common Ancestor of a Binary Search Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-search-tree
  - binary-tree
  - dfs
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-search-tree/"
---

# LeetCode 0235: Lowest Common Ancestor of a Binary Search Tree

## 1. Problem Formalization and Constraints

Given a binary search tree (BST), find the lowest common ancestor (LCA) node of two given nodes in the BST.
According to the definition of LCA on Wikipedia: "The lowest common ancestor is defined between two nodes $p$ and $q$ as the lowest node in $T$ that has both $p$ and $q$ as descendants (where we allow a node to be a descendant of itself)."

### Constraints
- The number of nodes in the tree is in the range $[2, 10^5]$.
- $-10^9 \le \text{Node.val} \le 10^9$
- All `Node.val` are unique.
- $p \ne q$
- $p$ and $q$ will exist in the BST.

### Examples
- **Example 1**:
  - Input: `root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 8`
  - Output: `6`
  - Explanation: The LCA of nodes 2 and 8 is 6.
- **Example 2**:
  - Input: `root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 4`
  - Output: `2`
  - Explanation: The LCA of nodes 2 and 4 is 2, since a node can be a descendant of itself according to the LCA definition.
- **Example 3**:
  - Input: `root = [2,1], p = 2, q = 1`
  - Output: `2`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative Value Range Splitting | $O(H)$ | $O(1)$ auxiliary | Walks down the tree following BST value invariants; terminates at the first node where paths to $p$ and $q$ diverge. |
| **Tier 2 (Recursive)** | Recursive BST Splitting | $O(H)$ | $O(H)$ auxiliary | Recursive formulation of value splitting; uses $O(H)$ stack frames. |
| **Tier 3 (Generic Tree LCA)** | Post-Order Subtree Flagging | $O(N)$ | $O(H)$ auxiliary | Treats tree as a general binary tree without utilizing BST properties; visits unnecessary subtrees. |
| **Tier 4 (Path Intersection)** | Ancestor Path Root-to-Node Recording | $O(H)$ | $O(H)$ auxiliary | Traces root-to-$p$ and root-to-$q$ paths into lists; finds last common element. |

*Notation*: $H$ is the tree height ($O(\log N)$ average/balanced, $O(N)$ worst-case).

---

## 3. Tier 1: Most Optimal Solution (Iterative Value Range Splitting)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let `small = min(p.val, q.val)` and `large = max(p.val, q.val)`.
Starting at `curr = root`:
1. If `curr.val > large`: Both $p$ and $q$ must reside in the left subtree. Advance `curr = curr.left`.
2. If `curr.val < small`: Both $p$ and $q$ must reside in the right subtree. Advance `curr = curr.right`.
3. If `small <= curr.val <= large`: The search paths for $p$ and $q$ split at `curr` (or one of them is equal to `curr`).
   Therefore, `curr` is the lowest common ancestor. Return `curr`.

**Invariant Proof**:
Let $T$ be a valid BST and $u$ be the lowest common ancestor of $p$ and $q$.
Every node $v \in T$ defines a partition of values:
- Values in $L(v)$ are strictly less than $v.\text{val}$.
- Values in $R(v)$ are strictly greater than $v.\text{val}$.

Case 1: If $v.\text{val} > \max(p.\text{val}, q.\text{val})$, both $p$ and $q$ are strictly smaller than $v.\text{val}$. They must both belong to $L(v)$. Any common ancestor must lie in $L(v)$.
Case 2: If $v.\text{val} < \min(p.\text{val}, q.\text{val})$, both $p$ and $q$ are strictly greater than $v.\text{val}$. They must both belong to $R(v)$. Any common ancestor must lie in $R(v)$.
Case 3: If $\min(p.\text{val}, q.\text{val}) \le v.\text{val} \le \max(p.\text{val}, q.\text{val})$:
- If $p.\text{val} < v.\text{val} < q.\text{val}$, $p$ is in $L(v)$ and $q$ is in $R(v)$. No child of $v$ contains both $p$ and $q$. Thus $v$ is the lowest common ancestor.
- If $v.\text{val} == p.\text{val}$ (or $q.\text{val}$), $p$ is an ancestor of $q$ (or vice versa), and by definition a node can be a descendant of itself. Thus $v$ is the LCA.

Because each step strictly decreases the distance to the LCA and terminates at the exact divergence node, the algorithm is proven correct in $O(H)$ time and $O(1)$ space.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(H)$. At each step, moves down one level in the tree. At most $H$ nodes are visited, which is $O(\log N)$ for balanced trees and $O(N)$ for degenerate trees.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space requiring only scalar pointer updates.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <algorithm>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        int small = std::min(p->val, q->val);
        int large = std::max(p->val, q->val);

        while (root != nullptr) {
            if (root->val > large) {
                root = root->left;
            } else if (root->val < small) {
                root = root->right;
            } else {
                return root;
            }
        }

        return nullptr;
    }
};
```

#### Python 3
```python
class TreeNode:
    def __init__(self, x: int):
        self.val = x
        self.left: TreeNode | None = None
        self.right: TreeNode | None = None


class Solution:
    def lowestCommonAncestor(
        self, root: TreeNode, p: TreeNode, q: TreeNode
    ) -> TreeNode:
        small = min(p.val, q.val)
        large = max(p.val, q.val)

        curr: TreeNode | None = root
        while curr:
            if curr.val > large:
                curr = curr.left
            elif curr.val < small:
                curr = curr.right
            else:
                return curr

        return root
```

#### Java 21
```java
class TreeNode {
    int val;
    TreeNode left;
    TreeNode right;
    TreeNode(int x) { val = x; }
}

class Solution {
    public TreeNode lowestCommonAncestor(TreeNode root, TreeNode p, TreeNode q) {
        int small = Math.min(p.val, q.val);
        int large = Math.max(p.val, q.val);

        TreeNode curr = root;
        while (curr != null) {
            if (curr.val > large) {
                curr = curr.left;
            } else if (curr.val < small) {
                curr = curr.right;
            } else {
                return curr;
            }
        }

        return null;
    }
}
```

#### TypeScript
```typescript
class TreeNode {
    val: number;
    left: TreeNode | null;
    right: TreeNode | null;
    constructor(val?: number, left?: TreeNode | null, right?: TreeNode | null) {
        this.val = val === undefined ? 0 : val;
        this.left = left === undefined ? null : left;
        this.right = right === undefined ? null : right;
    }
}

function lowestCommonAncestor(root: TreeNode | null, p: TreeNode | null, q: TreeNode | null): TreeNode | null {
    if (!root || !p || !q) {
        return null;
    }

    const small = Math.min(p.val, q.val);
    const large = Math.max(p.val, q.val);

    let curr: TreeNode | null = root;
    while (curr !== null) {
        if (curr.val > large) {
            curr = curr.left;
        } else if (curr.val < small) {
            curr = curr.right;
        } else {
            return curr;
        }
    }

    return null;
}
```

#### Go
```go
package main

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func lowestCommonAncestor(root, p, q *TreeNode) *TreeNode {
	small := p.Val
	large := q.Val
	if small > large {
		small, large = large, small
	}

	curr := root
	for curr != nil {
		if curr.Val > large {
			curr = curr.Left
		} else if curr.Val < small {
			curr = curr.Right
		} else {
			return curr
		}
	}

	return nil
}
```

#### Rust
```rust
use std::rc::Rc;
use std::cell::RefCell;

#[derive(Debug, PartialEq, Eq)]
pub struct TreeNode {
    pub val: i32,
    pub left: Option<Rc<RefCell<TreeNode>>>,
    pub right: Option<Rc<RefCell<TreeNode>>>,
}

pub struct Solution;

impl Solution {
    pub fn lowest_common_ancestor(
        root: Option<Rc<RefCell<TreeNode>>>,
        p: Option<Rc<RefCell<TreeNode>>>,
        q: Option<Rc<RefCell<TreeNode>>>,
    ) -> Option<Rc<RefCell<TreeNode>>> {
        let p_val = p.as_ref()?.borrow().val;
        let q_val = q.as_ref()?.borrow().val;
        let small = p_val.min(q_val);
        let large = p_val.max(q_val);

        let mut curr = root;
        while let Some(node) = curr {
            let val = node.borrow().val;
            if val > large {
                curr = node.borrow().left.clone();
            } else if val < small {
                curr = node.borrow().right.clone();
            } else {
                return Some(node);
            }
        }

        None
    }
}
```

---

## 4. Tier 2: Recursive BST Splitting

### 4.1 Mechanical Description
Formulate the tree traversal recursively:
```python
def lowestCommonAncestorRec(root: TreeNode, p: TreeNode, q: TreeNode) -> TreeNode:
    if root.val > p.val and root.val > q.val:
        return lowestCommonAncestorRec(root.left, p, q)
    if root.val < p.val and root.val < q.val:
        return lowestCommonAncestorRec(root.right, p, q)
    return root
```

### 4.2 Trade-offs
- Elegant tail-recursive code.
- Consumes $O(H)$ stack frames on the call stack, which can reach $10^5$ frames on degenerate trees, risking stack overflow in Python.

---

## 5. Tier 3: General Binary Tree LCA (Post-Order Traversal)

### 5.1 Mechanical Description
Ignore BST value ordering and apply the general binary tree LCA algorithm (LeetCode 236):
Recursively check left and right subtrees.
If both return non-null, `root` is the LCA.
If only one returns non-null, return that non-null node.

### 5.2 Trade-offs
- Works on any arbitrary binary tree.
- Takes $O(N)$ time instead of $O(H)$ because it does not know which branch to prune.

---

## 6. Tier 4: Root-to-Node Path Intersection

### 6.1 Mechanical Description
Record the path of nodes from `root` down to $p$ in list $P_1$, and from `root` down to $q$ in list $P_2$.
Iterate simultaneously along $P_1$ and $P_2$; the last identical node is the LCA.

### 6.2 Complexity & Deficiencies
- Takes $O(H)$ time but requires $O(H)$ extra list memory.
- Requires two separate downward traversals instead of a single search pass.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Register Traversal**: Pointer updates occur entirely within CPU registers (`RAX`, `RBX`), needing zero auxiliary heap or stack allocations.
2. **Branch Prediction**: During descent through deep trees, branches follow either left or right paths with high branch predictor accuracy.
3. **Pointers vs Values**: Comparing primitive integers `val` avoids pointer dereference chains until the split point is reached.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| $p$ is ancestor of $q$ | $p$ is root, $q$ is leaf | Returns $p$ | `curr.val == p.val`, hits condition $\text{small} \le \text{curr.val} \le \text{large}$ and returns $p$. |
| $q$ is ancestor of $p$ | $q$ is root, $p$ is leaf | Returns $q$ | Symmetrically returns $q$. |
| Nodes in different subtrees | $p$ in left subtree, $q$ in right | Returns root | Root satisfies condition immediately, returning root. |
| Two-node tree | `root = [2, 1], p = 2, q = 1` | Returns `2` | Correctly identifies root as ancestor of child. |
| Negative values | Values between $-10^9$ and $10^9$ | Operates correctly | Integer comparisons preserve sign and order. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does the BST property make LCA much faster than general binary trees?
In a BST, values guide the search deterministically down a single path ($O(H)$). In a general tree, both subtrees must be searched ($O(N)$).

### 2. What happens if $p$ or $q$ does not exist in the BST?
The problem constraints guarantee both nodes exist. If one were absent, the algorithm could return a false ancestor; a validation pass would be required.

### 3. Does the relative order of $p$ and $q$ matter?
No. Pre-computing `small = min(p.val, q.val)` and `large = max(p.val, q.val)` normalizes the comparison.

### 4. What is the difference between this problem and LeetCode 236?
LeetCode 236 is for general binary trees without sorted value invariants, requiring post-order DFS in $O(N)$ time.

### 5. Why is the iterative solution strictly superior to the recursive solution?
The iterative solution uses $O(1)$ space and cannot trigger call stack overflow errors even on degenerate linked-list trees with depth $10^5$.

### 6. Can the split condition evaluate true at non-LCA nodes?
No. The split condition means one target is in the left subtree (or equal to root) and the other is in the right subtree (or equal to root). A node cannot have descendants on both sides without being their LCA.

### 7. How does this compare with Lowest Common Ancestor in a DAG?
LCA in a DAG is more complex because multiple lowest common ancestors can exist, requiring topological ordering and reachability sets.

### 8. What is the time complexity on a completely degenerate tree?
$O(N)$ time, as the height equals the number of nodes $N$.

### 9. Why does Go use pointer equality?
Tree nodes in Go are represented by pointers `*TreeNode`; returning `curr` returns the identical node instance.

### 10. How does Rust handle tree node cloning?
In Rust, `Rc::clone` increments the reference counter to yield another reference to the shared LCA node without deep cloning the tree.

---

## 10. Related Problems and Systematic Progression Links

- [[0098-Validate-Binary-Search-Tree]]: Validating BST ordering properties.
- [[0226-Invert-Binary-Tree]]: Inverting binary trees.
- [[0230-Kth-Smallest-Element-in-a-BST]]: Traversal order properties in BST.
- LeetCode 236 (Lowest Common Ancestor of a Binary Tree): LCA in general binary trees.
- LeetCode 1644 (Lowest Common Ancestor of a Binary Tree II): LCA when nodes may not exist in tree.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/lowest-common-ancestor-of-a-binary-search-tree.cpp)
- [Python Implementation](../Python/lowest-common-ancestor-of-a-binary-search-tree.py)
- [Java Implementation](../Java/lowest-common-ancestor-of-a-binary-search-tree.java)
- [TypeScript Implementation](../TypeScript/lowest-common-ancestor-of-a-binary-search-tree.ts)
- [Go Implementation](../Golang/lowest-common-ancestor-of-a-binary-search-tree.go)
- [Rust Implementation](../Rust/lowest-common-ancestor-of-a-binary-search-tree.rs)

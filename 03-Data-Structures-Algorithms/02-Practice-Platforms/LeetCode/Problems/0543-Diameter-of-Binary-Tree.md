---
id: leetcode-0543-diameter-of-binary-tree
title: "LeetCode 0543: Diameter of Binary Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - depth-first-search
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/diameter-of-binary-tree/"
---

# LeetCode 0543: Diameter of Binary Tree

## 1. Problem Formalization and Constraints

Given the `root` of a binary tree, return the length of the diameter of the tree.
The diameter of a binary tree is the length of the longest path between any two nodes in a tree.
This path may or may not pass through the `root`.
The length of a path between two nodes is represented by the number of edges between them.

### Constraints
- The number of nodes in the tree is in the range $[1, 10^4]$.
- $-100 \le \text{Node.val} \le 100$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Single-Pass Post-Order DFS | $O(N)$ | $O(H)$ | Bottom-up subtree depth recursion; combines left and right heights at every node. |
| **Tier 2 (Iterative DFS)** | Post-Order Stack Traversal | $O(N)$ | $O(H)$ | Eliminates recursive call stack frames using explicit node state tracking. |
| **Tier 3 (Double DFS)** | Top-Down Depth Recomputation | $O(N^2)$ | $O(H)$ | Evaluates `depth(left) + depth(right)` at each node independently; redundant visits. |
| **Tier 4 (All-Pairs BFS)** | Graph Conversion & All-Pairs Shortest Path | $O(N^2)$ | $O(N)$ | Converts tree to undirected adjacency graph and computes maximum BFS path. |

---

## 3. Tier 1: Most Optimal Solution (Single-Pass Post-Order DFS)

### 3.1 Algorithmic Mechanics and Invariant Proof

Any path in a binary tree has a unique highest node (its Lowest Common Ancestor).
For a fixed highest node $u$, the longest simple path passing through $u$ consists of:
- The longest path descending into its left subtree: length $D_{\text{left}}$
- The longest path descending into its right subtree: length $D_{\text{right}}$
The edge count of this path is exactly $D_{\text{left}} + D_{\text{right}}$.
By performing a bottom-up post-order traversal:
1. Recursively compute the maximum subtree depths $D_{\text{left}}$ and $D_{\text{right}}$.
2. Maintain the global maximum diameter $\text{max\_diameter} = \max(\text{max\_diameter}, D_{\text{left}} + D_{\text{right}})$.
3. Return the maximum depth extended by 1 to the parent: $1 + \max(D_{\text{left}}, D_{\text{right}})$.
Every node is visited exactly once, yielding strict $O(N)$ runtime.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    int diameterOfBinaryTree(TreeNode* root) {
        int max_diameter = 0;
        maxDepth(root, max_diameter);
        return max_diameter;
    }

private:
    int maxDepth(TreeNode* node, int& max_diameter) {
        if (!node) return 0;
        const int left = maxDepth(node->left, max_diameter);
        const int right = maxDepth(node->right, max_diameter);
        max_diameter = std::max(max_diameter, left + right);
        return 1 + std::max(left, right);
    }
};
```

#### Python
```python
class Solution:
    def diameterOfBinaryTree(self, root: Optional[TreeNode]) -> int:
        max_diameter = 0

        def max_depth(node: Optional[TreeNode]) -> int:
            nonlocal max_diameter
            if not node:
                return 0
            left = max_depth(node.left)
            right = max_depth(node.right)
            max_diameter = max(max_diameter, left + right)
            return 1 + max(left, right)

        max_depth(root)
        return max_diameter
```

#### Java
```java
class Solution {
    private int maxDiameter = 0;

    public int diameterOfBinaryTree(TreeNode root) {
        maxDiameter = 0;
        maxDepth(root);
        return maxDiameter;
    }

    private int maxDepth(TreeNode node) {
        if (node == null) return 0;
        int left = maxDepth(node.left);
        int right = maxDepth(node.right);
        maxDiameter = Math.max(maxDiameter, left + right);
        return 1 + Math.max(left, right);
    }
}
```

#### TypeScript
```typescript
function diameterOfBinaryTree(root: TreeNode | null): number {
    let maxDiameter = 0;

    function maxDepth(node: TreeNode | null): number {
        if (node === null) return 0;
        const left = maxDepth(node.left);
        const right = maxDepth(node.right);
        maxDiameter = Math.max(maxDiameter, left + right);
        return 1 + Math.max(left, right);
    }

    maxDepth(root);
    return maxDiameter;
}
```

#### Golang
```go
package leetcode

func diameterOfBinaryTree(root *TreeNode) int {
	maxDiameter := 0

	var maxDepth func(node *TreeNode) int
	maxDepth = func(node *TreeNode) int {
		if node == nil {
			return 0
		}
		left := maxDepth(node.Left)
		right := maxDepth(node.Right)
		if left+right > maxDiameter {
			maxDiameter = left + right
		}
		if left > right {
			return 1 + left
		}
		return 1 + right
	}

	maxDepth(root)
	return maxDiameter
}
```

#### Rust
```rust
use std::rc::Rc;
use std::cell::RefCell;

pub struct Solution;

impl Solution {
    pub fn diameter_of_binary_tree(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        let mut max_diameter = 0;
        Self::max_depth(&root, &mut max_diameter);
        max_diameter
    }

    fn max_depth(node: &Option<Rc<RefCell<TreeNode>>>, max_diameter: &mut i32) -> i32 {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            let left = Self::max_depth(&n_borrow.left, max_diameter);
            let right = Self::max_depth(&n_borrow.right, max_diameter);
            *max_diameter = (*max_diameter).max(left + right);
            1 + left.max(right)
        } else {
            0
        }
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N)$ as each node is visited once in post-order sequence.
- **Space Complexity**: $O(H)$ auxiliary space on the recursion call stack, where $H \le N$ is the tree height ($O(\log N)$ on balanced trees, $O(N)$ on degenerate trees).
- **Branch Predictor Friendly**: Post-order recursion processes left then right subtrees deterministically, avoiding branching mispredictions.

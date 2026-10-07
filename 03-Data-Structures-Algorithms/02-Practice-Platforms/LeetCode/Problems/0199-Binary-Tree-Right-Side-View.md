---
id: leetcode-0199-binary-tree-right-side-view
title: "LeetCode 0199: Binary Tree Right Side View"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - breadth-first-search
  - depth-first-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/binary-tree-right-side-view/"
---

# LeetCode 0199: Binary Tree Right Side View

## 1. Problem Formalization and Constraints

Given the `root` of a binary tree, imagine yourself standing on the right side of it.
Return the values of the nodes you can see ordered from top to bottom.

### Constraints
- The number of nodes in the tree is in the range $[0, 100]$.
- $-100 \le \text{Node.val} \le 100$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal DFS)** | Right-Child First Pre-Order DFS | $O(N)$ | $O(H)$ | Visits right subtrees first; first node seen at depth $d$ is rightmost visible node. |
| **Tier 2 (Optimal BFS)** | Level-Order Queue Traversal | $O(N)$ | $O(W)$ | Processes levels sequentially; takes the terminal element of each level queue. |
| **Tier 3 (Full Tree BFS)** | Level Array Collection | $O(N)$ | $O(N)$ | Stores entire levels in 2D arrays before extracting right boundaries; extra heap allocations. |
| **Tier 4 (Brute Force)** | Recursive Path Depth Mapping | $O(N \log N)$ | $O(N)$ | Collects $(x, y)$ coordinates with sorting by ordinate and abscissa; redundant overhead. |

---

## 3. Tier 1: Most Optimal Solution (Right-Child First Pre-Order DFS)

### 3.1 Algorithmic Mechanics and Invariant Proof

By visiting nodes in the sequence: `Root -> Right Subtree -> Left Subtree`:
1. For any depth $d \ge 0$, the rightmost node at depth $d$ is visited strictly before any node positioned to its left at the same depth.
2. The running size of `result` tracks the maximum depth discovered so far.
3. When reaching node $u$ at depth $d$, if $d == \text{result.size()}$, then node $u$ is guaranteed to be the first (and therefore rightmost) node visited at depth $d$.
4. We record $u\text{.val}$ into `result`. Subsequent visits to other nodes at depth $d$ will observe $d < \text{result.size()}$ and be skipped.
This guarantees strict $O(N)$ runtime while using only $O(H)$ stack space, minimizing heap allocations.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    std::vector<int> rightSideView(TreeNode* root) {
        std::vector<int> result;
        dfs(root, 0, result);
        return result;
    }

private:
    void dfs(TreeNode* node, size_t depth, std::vector<int>& result) {
        if (!node) return;
        if (depth == result.size()) {
            result.push_back(node->val);
        }
        dfs(node->right, depth + 1, result);
        dfs(node->left, depth + 1, result);
    }
};
```

#### Python
```python
class Solution:
    def rightSideView(self, root: Optional[TreeNode]) -> list[int]:
        result = []

        def dfs(node: Optional[TreeNode], depth: int) -> None:
            if not node:
                return
            if depth == len(result):
                result.append(node.val)
            dfs(node.right, depth + 1)
            dfs(node.left, depth + 1)

        dfs(root, 0)
        return result
```

#### Java
```java
class Solution {
    public List<Integer> rightSideView(TreeNode root) {
        List<Integer> result = new ArrayList<>();
        dfs(root, 0, result);
        return result;
    }

    private void dfs(TreeNode node, int depth, List<Integer> result) {
        if (node == null) return;
        if (depth == result.size()) {
            result.add(node.val);
        }
        dfs(node.right, depth + 1, result);
        dfs(node.left, depth + 1, result);
    }
}
```

#### TypeScript
```typescript
function rightSideView(root: TreeNode | null): number[] {
    const result: number[] = [];

    function dfs(node: TreeNode | null, depth: number): void {
        if (!node) return;
        if (depth === result.length) {
            result.push(node.val);
        }
        dfs(node.right, depth + 1);
        dfs(node.left, depth + 1);
    }

    dfs(root, 0);
    return result;
}
```

#### Golang
```go
package main

func rightSideView(root *TreeNode) []int {
	result := make([]int, 0)

	var dfs func(node *TreeNode, depth int)
	dfs = func(node *TreeNode, depth int) {
		if node == nil {
			return
		}
		if depth == len(result) {
			result = append(result, node.Val)
		}
		dfs(node.Right, depth+1)
		dfs(node.Left, depth+1)
	}

	dfs(root, 0)
	return result
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn right_side_view(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        let mut result = Vec::new();
        Self::dfs(&root, 0, &mut result);
        result
    }

    fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, depth: usize, result: &mut Vec<i32>) {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            if depth == result.len() {
                result.push(n_borrow.val);
            }
            Self::dfs(&n_borrow.right, depth + 1, result);
            Self::dfs(&n_borrow.left, depth + 1, result);
        }
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N)$ as each node in the tree is visited at most once.
- **Space Complexity**: $O(H)$ auxiliary call stack memory, where $H \le N$ is the tree height.
- **Optimal Memory Footprint**: Avoids heap allocation for a BFS queue; requires only a flat array of size $H$ for the visible elements.

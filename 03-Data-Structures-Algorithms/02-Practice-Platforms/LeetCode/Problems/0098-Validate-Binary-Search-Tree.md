---
id: leetcode-0098-validate-binary-search-tree
title: "LeetCode 0098: Validate Binary Search Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-search-tree
  - dfs
  - recursion
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/validate-binary-search-tree/"
---

# LeetCode 0098: Validate Binary Search Tree

## 1. Problem Formalization and Constraints

Given the root of a binary tree, determine if it is a valid binary search tree (BST).
A valid BST is defined as follows:
- The left subtree of a node contains only nodes with keys strictly less than the node's key.
- The right subtree of a node contains only nodes with keys strictly greater than the node's key.
- Both the left and right subtrees must also be binary search trees.

### Constraints
- The number of nodes in the tree is in the range $[1, 10^4]$.
- $-2^{31} \le \text{Node.val} \le 2^{31} - 1$

### Examples
- **Example 1**:
  - Input: `root = [2,1,3]`
  - Output: `true`
- **Example 2**:
  - Input: `root = [5,1,4,null,null,3,6]`
  - Output: `false`
  - Explanation: The root node's value is 5 but its right child's value is 4.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Bounded Range DFS Validation | $O(N)$ | $O(H)$ | Propagates open interval $(low, high)$ down the tree, checking root validity in $O(1)$ per node. |
| **Tier 2 (Space-Optimized Alternative)** | Iterative In-Order with Previous Pointer | $O(N)$ | $O(H)$ | Simulates recursion stack; validates strictly increasing sequence $prev < curr$ without whole-tree bounds. |
| **Tier 3 (Time/Space Boundary Optimal)** | Morris In-Order Threaded Traversal | $O(N)$ | $O(1)$ | Temporarily threads right pointers of in-order predecessors; restores tree structure during pass. |
| **Tier 4 (Brute Force)** | In-Order Array Materialization | $O(N)$ | $O(N)$ | Collects all node values into a dynamically sized array and verifies strict monotonicity afterward. |

---

## 3. Tier 1: Most Optimal Solution (Bounded Range DFS Validation)

### 3.1 Algorithmic Mechanics and Invariant Proof

A common fallacy in BST validation is verifying only local parent-child invariants: `node.left.val < node.val` and `node.right.val > node.val`.
This local check fails to detect cross-ancestor violations (for example, a right-child's left descendant whose value is smaller than the top root).

To preserve the global BST property, each node must fall within an open interval $(low, high)$:
1. At the root, $low = -\infty$ and $high = +\infty$.
2. When descending into the left subtree, the upper bound tightens: $(low, \text{node.val})$.
3. When descending into the right subtree, the lower bound tightens: $(\text{node.val}, high)$.
4. If at any node, $\text{node.val} \le low$ or $\text{node.val} \ge high$, the tree is invalid.

**Invariant Proof**:
By induction, every node in `node.left` satisfies $\text{val} < \text{node.val}$, and since it also inherits $low$, it satisfies $low < \text{val} < \text{node.val}$.
Similarly, every node in `node.right` satisfies $\text{node.val} < \text{val} < high$.
Transitivity guarantees that every node in the entire subtree respects all ancestor partition constraints.

Because 32-bit node values can equal $-2^{31}$ or $2^{31} - 1$, bounds should be represented either as 64-bit integers (`long long` / `int64`) or via nullable pointers/optional references to avoid integer overflow bugs.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each node is visited at most once, and interval checks execute in $O(1)$ time. Early termination halts traversal as soon as the first invalid node is encountered.
- **Auxiliary Space Complexity**: $O(H)$, where $H$ is the tree height. For a balanced tree $H = O(\log N)$; in the degenerate skewed case $H = O(N)$ stack frames.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <optional>
#include <cstdint>

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
    bool isValidBST(TreeNode* root) {
        return validate(root, std::nullopt, std::nullopt);
    }

private:
    bool validate(TreeNode* node, std::optional<int64_t> low, std::optional<int64_t> high) {
        if (!node) return true;

        int64_t val = node->val;
        if (low.has_value() && val <= low.value()) return false;
        if (high.has_value() && val >= high.value()) return false;

        return validate(node->left, low, val) && validate(node->right, val, high);
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
    def isValidBST(self, root: Optional[TreeNode]) -> bool:
        def validate(node: Optional[TreeNode], low: float, high: float) -> bool:
            if not node:
                return True
            if node.val <= low or node.val >= high:
                return False
            return validate(node.left, low, node.val) and validate(node.right, node.val, high)

        return validate(root, float("-inf"), float("inf"))
```

#### Java 21
```java
class Solution {
    public boolean isValidBST(TreeNode root) {
        return validate(root, null, null);
    }

    private boolean validate(TreeNode node, Long low, Long high) {
        if (node == null) return true;
        long val = node.val;
        if (low != null && val <= low) return false;
        if (high != null && val >= high) return false;
        return validate(node.left, low, val) && validate(node.right, val, high);
    }
}
```

#### TypeScript 5
```typescript
function isValidBST(root: TreeNode | null): boolean {
    function validate(node: TreeNode | null, low: number | null, high: number | null): boolean {
        if (!node) return true;
        if (low !== null && node.val <= low) return false;
        if (high !== null && node.val >= high) return false;
        return validate(node.left, low, node.val) && validate(node.right, node.val, high);
    }

    return validate(root, null, null);
}
```

#### Go 1.22
```go
package main

import "math"

func isValidBST(root *TreeNode) bool {
	return validate(root, math.MinInt64, math.MaxInt64)
}

func validate(node *TreeNode, low, high int64) bool {
	if node == nil {
		return true
	}
	val := int64(node.Val)
	if val <= low || val >= high {
		return false
	}
	return validate(node.Left, low, val) && validate(node.Right, val, high)
}
```

#### Rust 2021
```rust
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_valid_bst(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        Self::validate(&root, None, None)
    }

    fn validate(
        node: &Option<Rc<RefCell<TreeNode>>>,
        low: Option<i64>,
        high: Option<i64>,
    ) -> bool {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            let val = n_borrow.val as i64;
            if let Some(l) = low {
                if val <= l {
                    return false;
                }
            }
            if let Some(h) = high {
                if val >= h {
                    return false;
                }
            }
            Self::validate(&n_borrow.left, low, Some(val))
                && Self::validate(&n_borrow.right, Some(val), high)
        } else {
            true
        }
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized Alternative (Iterative In-Order Traversal with Previous Pointer)

### 4.1 Algorithmic Mechanics
An in-order traversal of a valid BST visits keys in strictly ascending numerical order.
By simulating recursion with an explicit stack, we traverse to the leftmost leaf, process the node by checking if `curr.val <= prev.val`, update `prev = curr.val`, and shift to `curr.right`.
Early exit triggers the moment any non-increasing element is encountered.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Every node is pushed and popped from the stack at most once.
- **Space Complexity**: $O(H)$ auxiliary space for the traversal stack.

### 4.3 Implementation (C++20)
```cpp
#include <stack>
#include <optional>
#include <cstdint>

class Solution {
public:
    bool isValidBST(TreeNode* root) {
        std::stack<TreeNode*> st;
        TreeNode* curr = root;
        std::optional<int64_t> prev = std::nullopt;

        while (curr != nullptr || !st.empty()) {
            while (curr != nullptr) {
                st.push(curr);
                curr = curr->left;
            }

            curr = st.top();
            st.pop();

            if (prev.has_value() && curr->val <= prev.value()) {
                return false;
            }
            prev = curr->val;

            curr = curr->right;
        }

        return true;
    }
};
```

---

## 5. Tier 3: Time/Space Boundary Optimal (Morris In-Order Threaded Traversal)

### 5.1 Algorithmic Mechanics
Morris Traversal achieves an in-order tree walk in $O(1)$ auxiliary space without using recursion or an auxiliary stack.
For the current node `curr`:
1. If `curr->left` is null, compare `curr->val` with `prev`, update `prev`, and set `curr = curr->right`.
2. Otherwise, find the in-order predecessor (the rightmost node in `curr->left`):
   - If `pre->right` is null, create a temporary thread `pre->right = curr`, and move `curr = curr->left`.
   - If `pre->right == curr`, the left subtree is fully processed: remove the thread `pre->right = nullptr`, check monotonicity `curr->val <= prev`, update `prev`, and set `curr = curr->right`.

Note that upon detecting an invalid element during Morris traversal, one must continue traversal until all modified threads are dismantled, or restore them carefully, to preserve tree integrity.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each edge is traversed at most three times.
- **Space Complexity**: $O(1)$ auxiliary space.

### 5.3 Implementation (C++20)
```cpp
#include <optional>
#include <cstdint>

class Solution {
public:
    bool isValidBST(TreeNode* root) {
        TreeNode* curr = root;
        std::optional<int64_t> prev = std::nullopt;
        bool valid = true;

        while (curr != nullptr) {
            if (curr->left == nullptr) {
                if (prev.has_value() && curr->val <= prev.value()) {
                    valid = false;
                }
                prev = curr->val;
                curr = curr->right;
            } else {
                TreeNode* pre = curr->left;
                while (pre->right != nullptr && pre->right != curr) {
                    pre = pre->right;
                }

                if (pre->right == nullptr) {
                    pre->right = curr; // Establish thread
                    curr = curr->left;
                } else {
                    pre->right = nullptr; // Sever thread
                    if (prev.has_value() && curr->val <= prev.value()) {
                        valid = false;
                    }
                    prev = curr->val;
                    curr = curr->right;
                }
            }
        }

        return valid;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (In-Order Array Materialization)

### 6.1 Algorithmic Mechanics
The simplest baseline performs a recursive in-order traversal, dumping all node values into an array `vals`.
Once complete, iterate through `vals[0..len-2]` and verify whether `vals[i] < vals[i + 1]` holds for all indices.
This consumes $O(N)$ extra heap memory regardless of tree height and cannot early-exit on initial violations.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(N)$ auxiliary memory for the materialized vector plus $O(H)$ recursion stack.

### 6.3 Implementation (Python 3)
```python
from typing import Optional, List

class Solution:
    def isValidBST(self, root: Optional[TreeNode]) -> bool:
        vals: List[int] = []

        def inorder(node: Optional[TreeNode]) -> None:
            if not node:
                return
            inorder(node.left)
            vals.append(node.val)
            inorder(node.right)

        inorder(root)

        for i in range(len(vals) - 1):
            if vals[i] >= vals[i + 1]:
                return False
        return True
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why does checking only `node.left.val < node.val` and `node.right.val > node.val` fail?</summary>
Binary search trees enforce a global property: all nodes in the left subtree must be less than the ancestor.
A node could satisfy the local property with its parent while violating the boundary of a grandparent higher up in the hierarchy.
</details>

<details>
<summary>2. How do boundary values $-2^{31}$ and $2^{31} - 1$ cause bugs if 32-bit signed integers are used?</summary>
If the initial range is initialized with `INT_MIN` and `INT_MAX`, a valid root containing `INT_MIN` triggers `node.val <= low` (or `node.val < low`), leading to incorrect falsification or integer overflow upon arithmetic adjustment.
</details>

<details>
<summary>3. Are duplicate keys permitted in a valid LeetCode BST?</summary>
No. The BST definition explicitly requires left subtree values to be strictly less than ($<$) and right subtree values to be strictly greater than ($>$) the node's key.
</details>

<details>
<summary>4. What is the advantage of using Optional or Nullable references over 64-bit infinity?</summary>
Optional bounds avoid assumptions about the underlying primitive integer width, making the pattern portable to arbitrary-precision integers or generic types implementing `Comparable<T>`.
</details>

<details>
<summary>5. What is the time complexity difference between early-exit DFS and full in-order materialization?</summary>
If the left child of the root violates the BST condition, early-exit DFS terminates in $O(1)$ operations.
In-order materialization traverses all $N$ nodes before detecting the violation.
</details>

<details>
<summary>6. Why must Morris Traversal dismantle all temporary threads even if an invalid key is discovered?</summary>
If Morris Traversal terminates immediately upon discovering an error, the tree structure remains corrupted with circular references, causing memory leaks or infinite loops in client applications.
</details>

<details>
<summary>7. How does tree balance affect stack space usage?</summary>
For a balanced tree (AVL or Red-Black), recursion stack space is $O(\log N)$.
For a completely degenerate linked-list tree, stack space scales linearly to $O(N)$.
</details>

<details>
<summary>8. Can Post-Order traversal be used to validate a BST?</summary>
Yes. In post-order validation, each subtree returns its minimum value, maximum value, and a boolean status to its parent.
The parent verifies $\max(\text{left}) < \text{node.val} < \min(\text{right})$.
</details>

<details>
<summary>9. What is the minimum number of nodes in the input tree according to constraints?</summary>
The constraint states $1 \le N \le 10^4$.
A single isolated root node is inherently a valid BST.
</details>

<details>
<summary>10. How does BST validation relate to recovering a BST (LeetCode 99)?</summary>
Recover Binary Search Tree uses the in-order traversal sequence from Tier 2 to locate the exactly two swapped nodes where $prev.val > curr.val$ occurs.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/validate-binary-search-tree.cpp)
- [Python Implementation](../Python/validate-binary-search-tree.py)
- [Java Implementation](../Java/validate-binary-search-tree.java)
- [TypeScript Implementation](../TypeScript/validate-binary-search-tree.ts)
- [Go Implementation](../Golang/validate-binary-search-tree.go)
- [Rust Implementation](../Rust/validate-binary-search-tree.rs)

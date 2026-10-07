---
id: leetcode-0124-binary-tree-maximum-path-sum
title: "LeetCode 0124: Binary Tree Maximum Path Sum"
tags:
  - dsa
  - leetcode
  - tree
  - depth-first-search
  - dynamic-programming
  - recursion
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/binary-tree-maximum-path-sum/"
---

# LeetCode 0124: Binary Tree Maximum Path Sum

## 1. Problem Formalization and Constraints

A path in a binary tree is a sequence of nodes where each pair of adjacent nodes in the sequence has an edge connecting them.
A node can only appear in the sequence at most once.
The path does not necessarily need to pass through the root of the tree.
The path sum of a path is the sum of the node values along the path.
Given the root of a binary tree, return the maximum path sum of any non-empty path.

### Constraints
- The number of nodes in the tree is in the range $[1, 3 \times 10^4]$.
- $-1000 \le \text{Node.val} \le 1000$

### Examples
- **Example 1**:
  - Input: `root = [1,2,3]`
  - Output: `6`
  - Explanation: The optimal path is $2 \to 1 \to 3$ with a path sum of $2 + 1 + 3 = 6$.
- **Example 2**:
  - Input: `root = [-10,9,20,null,null,15,7]`
  - Output: `42`
  - Explanation: The optimal path is $15 \to 20 \to 7$ with a path sum of $15 + 20 + 7 = 42$.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Post-Order DFS with Global Maximum Accumulation | $O(N)$ | $O(H)$ | Computes max single-branch gain bottom-up while continuously updating apex turnaround path sum. |
| **Tier 2 (Iterative)** | Explicit Call-Frame Emulation Stack | $O(N)$ | $O(H)$ | Simulates post-order traversal using an explicit stack; eliminates risk of call-stack overflow on deep skewed trees. |
| **Tier 3 (Subtree Hash)** | Memoized Subtree Branch Map with Tree Traversal | $O(N)$ | $O(N)$ | Stores maximum downwards branch sum in an auxiliary hash map before computing apex values; costs extra heap allocations. |
| **Tier 4 (Brute Force)** | All-Pairs Path Enumeration via Lowest Common Ancestor | $O(N^2)$ | $O(N)$ | Traverses every node as candidate apex and computes maximum downward branches independently via repeated DFS traversals. |

---

## 3. Tier 1: Most Optimal Solution (Post-Order DFS with Global Maximum Accumulation)

### 3.1 Algorithmic Mechanics and Invariant Proof

Every path in a binary tree has a unique highest node, known as the apex or turn-around node of the path.
If node $u$ serves as the apex:
- The path enters $u$ from at most one node in its left subtree.
- The path exits $u$ to at most one node in its right subtree.
- Both branches are optional because negative branch sums should be discarded (clamped to 0).

At each node $u$, define the single-branch gain $G(u)$ as the maximum sum achievable along a downward path originating at $u$:
$$G(u) = u.\text{val} + \max(0, \max(G(u.\text{left}), G(u.\text{right})))$$

Simultaneously, the maximum path sum that has node $u$ as its apex is:
$$\text{ApexPath}(u) = u.\text{val} + \max(0, G(u.\text{left})) + \max(0, G(u.\text{right}))$$

We maintain a global running maximum initialized to $-\infty$.
During a post-order traversal:
1. Recursively compute the branch gain of the left child, clamped at 0.
2. Recursively compute the branch gain of the right child, clamped at 0.
3. Evaluate $\text{ApexPath}(u)$ and update the global maximum.
4. Return $G(u)$ to the caller.

**Invariant Proof**:
Base Case: For an empty node (null pointer), the branch gain is strictly 0.
Inductive Step: Assume for all descendants of node $u$, their branch gains are computed correctly.
A path passing through $u$ can branch into both the left and right subtrees, but a path extending to $u$'s parent can select at most one child branch.
By taking $\max(0, G(u.\text{left}))$ and $\max(0, G(u.\text{right}))$, any branch contributing negative net value is omitted.
Thus, every valid non-empty path is considered at its unique highest node, guaranteeing global optimality.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$, where $N$ is the number of nodes in the binary tree. Every node is visited exactly once in post-order.
- **Auxiliary Space Complexity**: $O(H)$, where $H$ is the height of the tree. The maximum space consumed corresponds to the recursion stack depth, which is $O(\log N)$ for balanced trees and $O(N)$ in the worst-case degenerate linked list tree.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

#include <algorithm>
#include <climits>

class Solution {
public:
    int maxPathSum(TreeNode* root) {
        int max_sum = INT_MIN;
        maxGain(root, max_sum);
        return max_sum;
    }

private:
    int maxGain(TreeNode* node, int& max_sum) {
        if (!node) {
            return 0;
        }

        int left_gain = std::max(0, maxGain(node->left, max_sum));
        int right_gain = std::max(0, maxGain(node->right, max_sum));

        int current_path = node->val + left_gain + right_gain;
        max_sum = std::max(max_sum, current_path);

        return node->val + std::max(left_gain, right_gain);
    }
};
```

#### Python 3
```python
from typing import Optional

class TreeNode:
    def __init__(self, val: int = 0, left: Optional['TreeNode'] = None, right: Optional['TreeNode'] = None):
        self.val = val
        self.left = left
        self.right = right

class Solution:
    def maxPathSum(self, root: Optional[TreeNode]) -> int:
        max_sum = float('-inf')

        def max_gain(node: Optional[TreeNode]) -> int:
            nonlocal max_sum
            if not node:
                return 0

            left_gain = max(0, max_gain(node.left))
            right_gain = max(0, max_gain(node.right))

            current_path = node.val + left_gain + right_gain
            max_sum = max(max_sum, current_path)

            return node.val + max(left_gain, right_gain)

        max_gain(root)
        return int(max_sum)
```

#### Java 21
```java
class TreeNode {
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

public class Solution {
    private int maxSum = Integer.MIN_VALUE;

    public int maxPathSum(TreeNode root) {
        maxSum = Integer.MIN_VALUE;
        maxGain(root);
        return maxSum;
    }

    private int maxGain(TreeNode node) {
        if (node == null) {
            return 0;
        }

        int leftGain = Math.max(0, maxGain(node.left));
        int rightGain = Math.max(0, maxGain(node.right));

        int currentPath = node.val + leftGain + rightGain;
        maxSum = Math.max(maxSum, currentPath);

        return node.val + Math.max(leftGain, rightGain);
    }
}
```

#### TypeScript 5
```typescript
class TreeNode {
    val: number;
    left: TreeNode | null;
    right: TreeNode | null;
    constructor(val?: number, left?: TreeNode | null, right?: TreeNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.left = (left === undefined ? null : left);
        this.right = (right === undefined ? null : right);
    }
}

function maxPathSum(root: TreeNode | null): number {
    let maxSum = -Infinity;

    function maxGain(node: TreeNode | null): number {
        if (!node) {
            return 0;
        }

        const leftGain = Math.max(0, maxGain(node.left));
        const rightGain = Math.max(0, maxGain(node.right));

        const currentPath = node.val + leftGain + rightGain;
        if (currentPath > maxSum) {
            maxSum = currentPath;
        }

        return node.val + Math.max(leftGain, rightGain);
    }

    maxGain(root);
    return maxSum;
}
```

#### Go 1.22
```go
package main

import "math"

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func maxPathSum(root *TreeNode) int {
	maxSum := math.MinInt32

	var maxGain func(node *TreeNode) int
	maxGain = func(node *TreeNode) int {
		if node == nil {
			return 0
		}

		leftGain := maxGain(node.Left)
		if leftGain < 0 {
			leftGain = 0
		}

		rightGain := maxGain(node.Right)
		if rightGain < 0 {
			rightGain = 0
		}

		currentPath := node.Val + leftGain + rightGain
		if currentPath > maxSum {
			maxSum = currentPath
		}

		if leftGain > rightGain {
			return node.Val + leftGain
		}
		return node.Val + rightGain
	}

	maxGain(root)
	return maxSum
}
```

#### Rust 1.75
```rust
use std::rc::Rc;
use std::cell::RefCell;
use std::cmp;

#[derive(Debug, PartialEq, Eq)]
pub struct TreeNode {
    pub val: i32,
    pub left: Option<Rc<RefCell<TreeNode>>>,
    pub right: Option<Rc<RefCell<TreeNode>>>,
}

pub struct Solution;

impl Solution {
    pub fn max_path_sum(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        let mut max_sum = i32::MIN;
        Self::max_gain(&root, &mut max_sum);
        max_sum
    }

    fn max_gain(node: &Option<Rc<RefCell<TreeNode>>>, max_sum: &mut i32) -> i32 {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            let left_gain = cmp::max(0, Self::max_gain(&n_borrow.left, max_sum));
            let right_gain = cmp::max(0, Self::max_gain(&n_borrow.right, max_sum));

            let current_path = n_borrow.val + left_gain + right_gain;
            *max_sum = cmp::max(*max_sum, current_path);

            n_borrow.val + cmp::max(left_gain, right_gain)
        } else {
            0
        }
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized / Iterative Post-Order Traversal

### 4.1 Implementation Mechanism

In mission-critical runtime environments, unbounded recursion creates a stack overflow hazard on deeply skewed trees where $H \approx N$.
We can emulate the call stack explicitly using a two-state transition:
- State 1 (Discover): Traverse down to the leaves, pushing nodes onto an explicit stack.
- State 2 (Process): When both child subtrees have returned their gains, compute the apex path sum and pass the branch gain upwards.

```cpp
#include <algorithm>
#include <climits>
#include <stack>
#include <unordered_map>

class SolutionIterative {
public:
    int maxPathSum(TreeNode* root) {
        if (!root) return 0;

        int max_sum = INT_MIN;
        std::unordered_map<TreeNode*, int> gains;
        std::stack<TreeNode*> s;
        TreeNode* curr = root;
        TreeNode* last_visited = nullptr;

        while (curr || !s.empty()) {
            if (curr) {
                s.push(curr);
                curr = curr->left;
            } else {
                TreeNode* peek_node = s.top();
                if (peek_node->right && last_visited != peek_node->right) {
                    curr = peek_node->right;
                } else {
                    s.pop();
                    int left_gain = std::max(0, gains[peek_node->left]);
                    int right_gain = std::max(0, gains[peek_node->right]);
                    max_sum = std::max(max_sum, peek_node->val + left_gain + right_gain);
                    gains[peek_node] = peek_node->val + std::max(left_gain, right_gain);
                    last_visited = peek_node;
                }
            }
        }

        return max_sum;
    }
};
```

### 4.2 Trade-offs
- Eliminates thread call-stack limitations.
- Requires an auxiliary hash table or pointer tags to store return values from child nodes.

---

## 5. Tier 3: Time-Complexity Alternative (Two-Pass Memoized Subtree Decomposition)

### 5.1 Algorithmic Structure
Pass 1 traverses the tree bottom-up to populate a table of single-branch downward gains for each node.
Pass 2 visits each node, pulls the precalculated branch sums of its children, and computes the apex turnaround value.

```python
class SolutionTwoPass:
    def maxPathSum(self, root: Optional[TreeNode]) -> int:
        if not root:
            return 0

        branch_gain = {}

        def compute_branches(node: Optional[TreeNode]) -> int:
            if not node:
                return 0
            left = max(0, compute_branches(node.left))
            right = max(0, compute_branches(node.right))
            gain = node.val + max(left, right)
            branch_gain[node] = gain
            return gain

        compute_branches(root)

        max_sum = float('-inf')
        def evaluate_apex(node: Optional[TreeNode]) -> None:
            nonlocal max_sum
            if not node:
                return
            left = max(0, branch_gain.get(node.left, 0))
            right = max(0, branch_gain.get(node.right, 0))
            max_sum = max(max_sum, node.val + left + right)
            evaluate_apex(node.left)
            evaluate_apex(node.right)

        evaluate_apex(root)
        return int(max_sum)
```

### 5.2 Trade-offs
- Clearly separates branch calculation from apex evaluation.
- Incurs $2 \times$ traversal passes and heap allocations for the hash map.

---

## 6. Tier 4: Brute Force Baseline (All-Pairs LCA Path Enumeration)

### 6.1 Mechanical Description
Enumerate every pair of nodes $(u, v)$ in the binary tree.
Find their Lowest Common Ancestor (LCA).
Traverse the unique simple path from $u$ up to $\text{LCA}(u, v)$ and down to $v$, summing the node values.
Track the maximum sum observed across all $O(N^2)$ pairs.

### 6.2 Complexity
- **Time Complexity**: $O(N^2)$ to $O(N^3)$ depending on whether LCA and path summation are memoized.
- **Space Complexity**: $O(N)$ auxiliary memory for path buffers and ancestor tables.
- **Verdict**: Unusable for $N = 3 \times 10^4$, timing out on any platform.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Pointer Dereferencing Overhead**: Binary tree nodes allocated dynamically via `new` or `malloc` are scattered across the process heap.
2. Traversal exhibits low spatial locality compared to sequential array buffers.
3. **Recursion Stack Frame Sizing**: Each stack frame in C++ stores the return address, saved frame pointer, node pointer, and local references (approx. 32 to 48 bytes per level).
4. For tree depth $H = 10^4$, stack memory consumption remains around 500 KB, which fits within typical 8 MB default thread stack limits.
5. In embedded or constrained systems with 64 KB thread stacks, the iterative approach of Tier 2 is mandatory.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| All Negative Node Values | `[-3, -2, -5]` | Returns highest single node: `-2` | Initialize `max_sum` to `INT_MIN` instead of 0. |
| Single Node Tree | `[42]` or `[-10]` | Returns the lone node value | Base condition covers single node naturally. |
| Skewed Linear Tree (Degenerate) | Linked list of $3 \times 10^4$ nodes | Operates without stack overflow | Deep recursion limits guarded. |
| Zero Value Nodes | Nodes with value 0 | Traverses correctly without skipping | Clamping logic uses $\ge 0$. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why must the return value of `maxGain` be clamped to 0?
If a child subtree yields a negative maximum branch sum, including it would decrease the total path sum. Clamping to 0 models the option to omit that branch.

### 2. Can the optimal path consist of a single node?
Yes. If every node in the tree has negative values, the maximum path sum is the single least-negative node in the entire tree.

### 3. Why cannot `maxGain` return `node.val + left_gain + right_gain` to its parent?
A path cannot fork. If a path passed through the parent and both left and right children of a node, that node would have degree three in the path, violating the simple path definition.

### 4. What is the difference between this problem and Maximum Subarray (Kadane's algorithm)?
Kadane's algorithm operates on a 1D sequential array where paths are continuous intervals. Here, paths are tree segments that can bend once at their apex node.

### 5. How are integer overflow errors avoided?
Node values range from $-1000$ to $1000$ and $N \le 3 \times 10^4$. The maximum possible path sum is $3 \times 10^7$, well within standard 32-bit signed integer limits ($[-2.14 \times 10^9, 2.14 \times 10^9]$).

### 6. Can Morris Traversal achieve $O(1)$ space for this problem?
Morris traversal modifies tree pointers to avoid recursion stacks. However, computing bottom-up values requires post-order traversal with reverse child traversals, which adds significant complexity without asymptotic time improvement.

### 7. Does this solution handle non-binary trees (general trees)?
Yes, for an $m$-ary tree, the apex path sum at node $u$ would take $u.\text{val}$ plus the sum of the two largest non-negative child branch gains.

### 8. What is the time complexity if tree node values can be modified in-place?
The time complexity remains $O(N)$ because every node must still be visited. In-place modification allows storing branch gains in existing node fields.

### 9. Why does Rust require `Rc<RefCell<TreeNode>>`?
Rust's strict ownership model forbids multiple mutable references. `Rc<RefCell<TreeNode>>` enables shared ownership and interior mutability during tree traversal.

### 10. How does compiler optimization affect the recursive DFS?
With `-O3`, modern C++ compilers inline small helper functions and optimize register allocation for `left_gain` and `right_gain`, resulting in fast CPU pipeline execution.

---

## 10. Related Problems and Systematic Progression Links

- [[0104-Maximum-Depth-of-Binary-Tree]]: Foundational post-order tree height traversal.
- [[0105-Construct-Binary-Tree-from-Preorder-and-Inorder-Traversal]]: Tree reconstruction and recursive structural analysis.
- [[0226-Invert-Binary-Tree]]: Recursive structural tree transformation.
- LeetCode 112 (Path Sum): Target path sum verification from root to leaf.
- LeetCode 543 (Diameter of Binary Tree): Maximum distance between any two nodes in a binary tree.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/binary-tree-maximum-path-sum.cpp)
- [Python Implementation](../Python/binary-tree-maximum-path-sum.py)
- [Java Implementation](../Java/binary-tree-maximum-path-sum.java)
- [TypeScript Implementation](../TypeScript/binary-tree-maximum-path-sum.ts)
- [Go Implementation](../Golang/binary-tree-maximum-path-sum.go)
- [Rust Implementation](../Rust/binary-tree-maximum-path-sum.rs)

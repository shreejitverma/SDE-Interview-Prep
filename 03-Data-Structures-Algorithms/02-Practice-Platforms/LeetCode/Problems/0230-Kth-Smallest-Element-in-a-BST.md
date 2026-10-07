---
id: leetcode-0230-kth-smallest-element-in-a-bst
title: "LeetCode 0230: Kth Smallest Element in a BST"
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
  - "https://leetcode.com/problems/kth-smallest-element-in-a-bst/"
---

# LeetCode 0230: Kth Smallest Element in a BST

## 1. Problem Formalization and Constraints

Given the `root` of a binary search tree, and an integer `k`, return the $k$-th smallest value (1-indexed) of all the values of the nodes in the tree.

### Constraints
- The number of nodes in the tree is $n$.
- $1 \le k \le n \le 10^4$
- $0 \le \text{Node.val} \le 10^4$

### Follow-up
If the BST is modified often (i.e., we can do insert and delete operations) and you need to find the $k$-th smallest frequently, how would you optimize?

### Examples
- **Example 1**:
  - Input: `root = [3,1,4,null,2], k = 1`
  - Output: `1`
- **Example 2**:
  - Input: `root = [5,3,6,2,4,null,null,1], k = 3`
  - Output: `3`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative In-Order Traversal with Early Stopping | $O(H + k)$ | $O(H)$ auxiliary | Traverses tree in-order using an explicit stack; terminates immediately upon popping the $k$-th element. |
| **Tier 2 (Morris Traversal)** | Threaded In-Order Traversal | $O(H + k)$ | $O(1)$ auxiliary | Modifies right child pointers temporarily to establish threads; achieves $O(1)$ space without a call stack. |
| **Tier 3 (Recursive In-Order)** | Full In-Order Traversal into Array | $O(N)$ | $O(N)$ auxiliary | Flattens tree into a sorted dynamic array; accesses index $k - 1$. |
| **Tier 4 (Augmented BST)** | Order Statistic Tree (Follow-up) | $O(H)$ per query | $O(1)$ query space | Maintains `subtree_size` on each node; navigates left/right child counts in $O(\log N)$ steps. |

*Notation*: $N$ is total node count, $H$ is tree height ($O(\log N)$ balanced, $O(N)$ degenerate), and $k$ is the target rank.

---

## 3. Tier 1: Most Optimal Solution (Iterative In-Order Traversal with Early Stopping)

### 3.1 Algorithmic Mechanics and Invariant Proof

By the definition of a Binary Search Tree (BST), an in-order traversal (Left $\to$ Root $\to$ Right) visits all nodes in strictly increasing numerical order.
Instead of visiting all $N$ nodes, we simulate in-order traversal using an explicit stack and stop as soon as $k$ nodes have been visited:
1. Initialize `curr = root` and an empty stack `stack`.
2. Push all left ancestors of `curr` onto `stack` until `curr` is null.
3. Pop the top node from `stack`. This node is the next smallest element in the BST.
4. Decrement `k`. If `k == 0`, return `curr.val`.
5. Set `curr = curr.right` and repeat from step 2.

**Invariant Proof**:
Let $T$ be a valid BST.
For any node $u \in T$, all nodes in its left subtree $L(u)$ satisfy $\text{val}(v) < \text{val}(u)$, and all nodes in its right subtree $R(u)$ satisfy $\text{val}(w) > \text{val}(u)$.
In-order traversal processes all elements in $L(u)$ before $u$, and all elements in $R(u)$ after $u$.
By induction on tree height, the $j$-th node popped from the stack is the $j$-th smallest element in $T$.
When $k$ reaches 0, exactly $k$ elements have been popped in ascending order.
The node popped at step $k$ is therefore guaranteed to be the $k$-th smallest element of the BST.
Early termination preserves this invariant while avoiding traversing the remaining $N - k$ nodes.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(H + k)$. Reaching the leftmost leaf takes $O(H)$ steps. Popping and traversing to the $k$-th element visits at most $k$ nodes. For balanced BSTs ($H = \log N$), this executes in $O(\log N + k)$ operations.
- **Auxiliary Space Complexity**: $O(H)$ auxiliary space representing the maximum number of nodes held on the traversal stack at any time.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

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
    int kthSmallest(TreeNode* root, int k) {
        std::vector<TreeNode*> stack;
        TreeNode* curr = root;

        while (curr != nullptr || !stack.empty()) {
            while (curr != nullptr) {
                stack.push_back(curr);
                curr = curr->left;
            }

            curr = stack.back();
            stack.pop_back();

            if (--k == 0) {
                return curr->val;
            }

            curr = curr->right;
        }

        return -1;
    }
};
```

#### Python 3
```python
from typing import Optional


class TreeNode:
    def __init__(
        self,
        val: int = 0,
        left: Optional["TreeNode"] = None,
        right: Optional["TreeNode"] = None,
    ):
        self.val = val
        self.left = left
        self.right = right


class Solution:
    def kthSmallest(self, root: Optional[TreeNode], k: int) -> int:
        stack: list[TreeNode] = []
        curr = root

        while curr or stack:
            while curr:
                stack.append(curr)
                curr = curr.left

            curr = stack.pop()
            k -= 1
            if k == 0:
                return curr.val

            curr = curr.right

        return -1
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Deque;

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

class Solution {
    public int kthSmallest(TreeNode root, int k) {
        Deque<TreeNode> stack = new ArrayDeque<>();
        TreeNode curr = root;

        while (curr != null || !stack.isEmpty()) {
            while (curr != null) {
                stack.push(curr);
                curr = curr.left;
            }

            curr = stack.pop();
            if (--k == 0) {
                return curr.val;
            }

            curr = curr.right;
        }

        return -1;
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

function kthSmallest(root: TreeNode | null, k: number): number {
    const stack: TreeNode[] = [];
    let curr = root;

    while (curr !== null || stack.length > 0) {
        while (curr !== null) {
            stack.push(curr);
            curr = curr.left;
        }

        curr = stack.pop()!;
        if (--k === 0) {
            return curr.val;
        }

        curr = curr.right;
    }

    return -1;
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

func kthSmallest(root *TreeNode, k int) int {
	var stack []*TreeNode
	curr := root

	for curr != nil || len(stack) > 0 {
		for curr != nil {
			stack = append(stack, curr)
			curr = curr.Left
		}

		curr = stack[len(stack)-1]
		stack = stack[:len(stack)-1]

		k--
		if k == 0 {
			return curr.Val
		}

		curr = curr.Right
	}

	return -1
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
    pub fn kth_smallest(root: Option<Rc<RefCell<TreeNode>>>, mut k: i32) -> i32 {
        let mut stack = Vec::new();
        let mut curr = root;

        while curr.is_some() || !stack.is_empty() {
            while let Some(node) = curr {
                curr = node.borrow().left.clone();
                stack.push(node);
            }

            if let Some(node) = stack.pop() {
                k -= 1;
                if k == 0 {
                    return node.borrow().val;
                }
                curr = node.borrow().right.clone();
            }
        }

        -1
    }
}
```

---

## 4. Tier 2: Morris In-Order Traversal

### 4.1 Mechanical Description
Traverse without a stack by creating temporary threads:
At node `curr`:
If `curr.left` is null, process `curr`, decrement `k`, and move to `curr.right`.
Else, find the in-order predecessor `pred` (rightmost node in left subtree).
If `pred.right` is null, set `pred.right = curr` and move to `curr.left`.
If `pred.right == curr`, revert `pred.right = null`, process `curr`, decrement `k`, and move to `curr.right`.

### 4.2 Trade-offs
- Operates in strict $O(1)$ auxiliary space.
- Modifies tree pointers during traversal; must restore pointers even after finding $k$ if tree mutation is prohibited.

---

## 5. Tier 3: Full In-Order Traversal Array

### 5.1 Mechanical Description
Perform recursive in-order DFS across the entire tree, appending every visited value to a dynamic list.
Return `list[k - 1]`.

### 5.2 Trade-offs
- Extremely simple to implement.
- Always visits all $N$ nodes regardless of $k$, consuming $O(N)$ auxiliary heap memory.

---

## 6. Tier 4: Order Statistic Tree (Follow-Up Design)

### 6.1 Mechanical Description
Augment each node structure with `count`: the total number of nodes in its subtree:
$$\text{count}(u) = 1 + \text{count}(u.\text{left}) + \text{count}(u.\text{right})$$
To find the $k$-th smallest element:
Let $L = \text{count}(u.\text{left})$.
If $k \le L$, search in $u.\text{left}$.
If $k == L + 1$, return $u.\text{val}$.
If $k > L + 1$, search in $u.\text{right}$ with new target $k - L - 1$.

### 6.2 Trade-offs
- Answering queries takes $O(H)$ time without any traversal.
- Insertions and deletions must maintain subtree counts along the ancestor path in $O(H)$ time.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Stack Pre-Allocation**: Reserving capacity for the vector stack (`stack.reserve(64)`) accommodates trees of depth up to 64, completely preventing vector heap reallocations.
2. **Early Loop Exit**: For $k = 1$, the loop terminates after pushing and popping the leftmost leaf ($O(H)$ operations), completely skipping $N - 1$ nodes.
3. **Cache Line Sharing**: Visiting pointers in depth-first order maintains temporal locality for ancestor nodes stored on the stack.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Smallest element ($k = 1$) | `k = 1` | Returns leftmost leaf | Pops first node from leftmost path and immediately exits. |
| Largest element ($k = N$) | `k = N` | Returns rightmost element | Traverses full tree and pops last node. |
| Left-skewed tree | Chain of left children | Returns $k$-th smallest node | Stack depth reaches $N$; processes in exact ascending order. |
| Right-skewed tree | Chain of right children | Returns $k$-th smallest node | Stack depth remains 1; moves right $k$ times. |
| Single node tree | `root = [1], k = 1` | Returns `1` | Traversal terminates on first pop. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does in-order traversal of a BST produce sorted order?
Because BST invariants mandate that left subtree values are strictly smaller than root, and right subtree values are strictly larger. Visiting Left $\to$ Root $\to$ Right naturally yields ascending order.

### 2. Can this algorithm be adapted to find the $k$-th largest element?
Yes. Reverse the in-order traversal: visit Right $\to$ Root $\to$ Left, decrementing $k$ at each node.

### 3. What is the time complexity if $k = 1$?
The time complexity is $O(H)$ to walk down the leftmost spine of the tree.

### 4. What is the space complexity in a balanced BST?
In a balanced BST, tree height $H = \log_2 N$. For $N = 10^4$, $\log_2(10000) \approx 14$ stack frames, using negligible memory.

### 5. Why does Morris Traversal need pointer restoration?
Morris traversal temporarily links predecessor nodes back to ancestors. If not restored, the tree contains cycles that break subsequent traversals.

### 6. How does the follow-up handle frequent inserts and deletes?
By maintaining subtree sizes within self-balancing BST nodes (like AVL or Red-Black trees), queries, insertions, and deletions all execute in $O(\log N)$ time.

### 7. Why is iterative traversal better than recursive DFS here?
Iterative traversal allows clean `return` on the $k$-th element without unwinding recursive call frames or setting global state flags.

### 8. Does the tree contain duplicate values?
Standard BST definitions and the problem constraints guarantee unique keys.

### 9. Why does Go use a slice as a stack?
Go slices with `append` and `slice[:len-1]` provide an amortized $O(1)$ LIFO stack with minimal allocation overhead.

### 10. How does Rust's `RefCell` affect performance?
`RefCell::borrow` performs dynamic runtime borrow checks. In production systems without shared tree ownership, raw pointers or arena allocators provide higher throughput.

---

## 10. Related Problems and Systematic Progression Links

- [[0098-Validate-Binary-Search-Tree]]: Validating BST ordering invariants.
- [[0102-Binary-Tree-Level-Order-Traversal]]: Breadth-first level ordering.
- [[0104-Maximum-Depth-of-Binary-Tree]]: Measuring tree height.
- [[0226-Invert-Binary-Tree]]: Recursive tree structural inversion.
- [[0235-Lowest-Common-Ancestor-of-a-Binary-Search-Tree]]: Utilizing BST properties for ancestor discovery.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/kth-smallest-element-in-a-bst.cpp)
- [Python Implementation](../Python/kth-smallest-element-in-a-bst.py)
- [Java Implementation](../Java/kth-smallest-element-in-a-bst.java)
- [TypeScript Implementation](../TypeScript/kth-smallest-element-in-a-bst.ts)
- [Go Implementation](../Golang/kth-smallest-element-in-a-bst.go)
- [Rust Implementation](../Rust/kth-smallest-element-in-a-bst.rs)

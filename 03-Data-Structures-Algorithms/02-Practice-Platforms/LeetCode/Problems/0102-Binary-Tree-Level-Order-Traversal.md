---
id: leetcode-0102-binary-tree-level-order-traversal
title: "LeetCode 0102: Binary Tree Level Order Traversal"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - bfs
  - queue
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/binary-tree-level-order-traversal/"
---

# LeetCode 0102: Binary Tree Level Order Traversal

## 1. Problem Formalization and Constraints

Given the root of a binary tree, return the level order traversal of its nodes' values (that is, from left to right, level by level).

### Constraints
- The number of nodes in the tree is in the range $[0, 2000]$.
- $-1000 \le \text{Node.val} \le 1000$

### Examples
- **Example 1**:
  - Input: `root = [3,9,20,null,null,15,7]`
  - Output: `[[3],[9,20],[15,7]]`
- **Example 2**:
  - Input: `root = [1]`
  - Output: `[[1]]`
- **Example 3**:
  - Input: `root = []`
  - Output: `[]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative BFS with Batch Level Sizing | $O(N)$ | $O(W)$ | Samples `queue.size()` before processing each tier; preserves cache locality and eliminates delimiters. |
| **Tier 2 (Space-Optimized Alternative)** | Pre-Order DFS with Depth Indexing | $O(N)$ | $O(H)$ | Recursively passes integer depth `d`; appends values to `result[d]` directly on stack frames. |
| **Tier 3 (Alternative BFS)** | Two-Queue Level Ping-Pong Buffer | $O(N)$ | $O(W)$ | Maintains separate `current` and `next` level queues, swapping pointers at tier boundaries. |
| **Tier 4 (Brute Force)** | Sentinel Null Delimiter BFS | $O(N)$ | $O(W)$ | Inserts `nullptr` into a single FIFO queue to signal level demarcations. |

---

## 3. Tier 1: Most Optimal Solution (Iterative BFS with Batch Level Sizing)

### 3.1 Algorithmic Mechanics and Invariant Proof

To collect nodes tier by tier, a FIFO queue is initialized with `root`.
At the start of each iteration:
1. Snapshot the integer size of the queue: `levelSize = queue.size()`.
2. Allocate a collection of capacity `levelSize` to store values belonging to the current horizontal horizon.
3. Dequeue exactly `levelSize` elements in a tight loop.
4. For each dequeued node, append its value to the current level array, and enqueue non-null left and right child pointers.
5. Append the completed level array to the master list `result`.

**Invariant Proof**:
Let $Q_k$ denote the queue contents after $k$ outer loop iterations.
By induction on depth $d$, at the start of loop iteration $d$:
- The queue contains all nodes at depth $d$ from left to right, and no nodes of any other depth.
- Taking `levelSize = |Q_d|` ensures exactly the nodes of depth $d$ are popped.
- Enqueuing children during this phase places all nodes of depth $d + 1$ into $Q_{d+1}$ in strict left-to-right order.
Thus, level isolation is maintained without auxiliary delimiter tags.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each node is inserted into the queue exactly once and popped exactly once.
- **Auxiliary Space Complexity**: $O(W)$, where $W$ is the maximum width (maximum number of nodes at any level) of the binary tree. For a full binary tree, $W \le \lceil N / 2 \rceil = O(N)$.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <queue>

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
    std::vector<std::vector<int>> levelOrder(TreeNode* root) {
        std::vector<std::vector<int>> result;
        if (!root) return result;

        std::queue<TreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            size_t levelSize = q.size();
            std::vector<int> currentLevel;
            currentLevel.reserve(levelSize);

            for (size_t i = 0; i < levelSize; ++i) {
                TreeNode* node = q.front();
                q.pop();
                currentLevel.push_back(node->val);

                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }

            result.push_back(std::move(currentLevel));
        }

        return result;
    }
};
```

#### Python 3
```python
from collections import deque
from typing import Optional, List

class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

class Solution:
    def levelOrder(self, root: Optional[TreeNode]) -> List[List[int]]:
        result: List[List[int]] = []
        if not root:
            return result

        queue = deque([root])

        while queue:
            level_size = len(queue)
            current_level: List[int] = []

            for _ in range(level_size):
                node = queue.popleft()
                current_level.append(node.val)

                if node.left:
                    queue.append(node.left)
                if node.right:
                    queue.append(node.right)

            result.append(current_level)

        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

class Solution {
    public List<List<Integer>> levelOrder(TreeNode root) {
        List<List<Integer>> result = new ArrayList<>();
        if (root == null) return result;

        Queue<TreeNode> queue = new LinkedList<>();
        queue.offer(root);

        while (!queue.isEmpty()) {
            int levelSize = queue.size();
            List<Integer> currentLevel = new ArrayList<>(levelSize);

            for (int i = 0; i < levelSize; i++) {
                TreeNode node = queue.poll();
                currentLevel.add(node.val);

                if (node.left != null) queue.offer(node.left);
                if (node.right != null) queue.offer(node.right);
            }

            result.add(currentLevel);
        }

        return result;
    }
}
```

#### TypeScript 5
```typescript
function levelOrder(root: TreeNode | null): number[][] {
    const result: number[][] = [];
    if (!root) return result;

    const queue: TreeNode[] = [root];

    while (queue.length > 0) {
        const levelSize = queue.length;
        const currentLevel: number[] = [];

        for (let i = 0; i < levelSize; i++) {
            const node = queue.shift()!;
            currentLevel.push(node.val);

            if (node.left) queue.push(node.left);
            if (node.right) queue.push(node.right);
        }

        result.push(currentLevel);
    }

    return result;
}
```

#### Go 1.22
```go
package main

func levelOrder(root *TreeNode) [][]int {
	var result [][]int
	if root == nil {
		return result
	}

	queue := []*TreeNode{root}

	for len(queue) > 0 {
		levelSize := len(queue)
		currentLevel := make([]int, levelSize)

		for i := 0; i < levelSize; i++ {
			node := queue[i]
			currentLevel[i] = node.Val

			if node.Left != nil {
				queue = append(queue, node.Left)
			}
			if node.Right != nil {
				queue = append(queue, node.Right)
			}
		}

		result = append(result, currentLevel)
		queue = queue[levelSize:]
	}

	return result
}
```

#### Rust 2021
```rust
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::VecDeque;

impl Solution {
    pub fn level_order(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<Vec<i32>> {
        let mut result = Vec::new();
        let Some(root_node) = root else {
            return result;
        };

        let mut queue = VecDeque::new();
        queue.push_back(root_node);

        while !queue.is_empty() {
            let level_size = queue.len();
            let mut current_level = Vec::with_capacity(level_size);

            for _ in 0..level_size {
                if let Some(node) = queue.pop_front() {
                    let n = node.borrow();
                    current_level.push(n.val);

                    if let Some(left) = n.left.clone() {
                        queue.push_back(left);
                    }
                    if let Some(right) = n.right.clone() {
                        queue.push_back(right);
                    }
                }
            }

            result.push(current_level);
        }

        result
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Pre-Order DFS with Depth Indexing)

### 4.1 Algorithmic Mechanics
Although level order traversal is conceptually breadth-first, a recursive depth-first search can populate the result matrix by carrying the recursion depth `level`:
- If `level == result.size()`, allocate a new inner list `result.push_back({})`.
- Append `node->val` to `result[level]`.
- Recurse into `dfs(node->left, level + 1)` followed by `dfs(node->right, level + 1)`.

Because left children are visited before right children at any given depth, elements at level `d` are inserted in left-to-right order.
On deep, narrow trees (such as degenerate linked lists), this consumes $O(1)$ breadth space instead of full tier allocations.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(H)$ auxiliary call stack space. In balanced trees $H = O(\log N)$.

### 4.3 Implementation (C++20)
```cpp
#include <vector>

class Solution {
public:
    std::vector<std::vector<int>> levelOrder(TreeNode* root) {
        std::vector<std::vector<int>> result;
        dfs(root, 0, result);
        return result;
    }

private:
    void dfs(TreeNode* node, size_t level, std::vector<std::vector<int>>& result) {
        if (!node) return;

        if (level == result.size()) {
            result.emplace_back();
        }
        result[level].push_back(node->val);

        dfs(node->left, level + 1, result);
        dfs(node->right, level + 1, result);
    }
};
```

---

## 5. Tier 3: Memory-Bounded Alternative (Two-Queue Level Ping-Pong)

### 5.1 Algorithmic Mechanics
Instead of querying dynamic queue size, allocate two explicit vectors or queues: `currLevel` and `nextLevel`.
Iterate over `currLevel`, appending values to the row buffer and collecting children into `nextLevel`.
At the completion of the level, append the row buffer to `result` and swap `currLevel = std::move(nextLevel)`.
This pattern decouples reads and writes, facilitating parallel child extraction or lock-free concurrent traversals.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(W)$ auxiliary memory.

### 5.3 Implementation (C++20)
```cpp
#include <vector>

class Solution {
public:
    std::vector<std::vector<int>> levelOrder(TreeNode* root) {
        std::vector<std::vector<int>> result;
        if (!root) return result;

        std::vector<TreeNode*> curr = {root};
        while (!curr.empty()) {
            std::vector<TreeNode*> next;
            std::vector<int> values;
            values.reserve(curr.size());

            for (TreeNode* node : curr) {
                values.push_back(node->val);
                if (node->left) next.push_back(node->left);
                if (node->right) next.push_back(node->right);
            }

            result.push_back(std::move(values));
            curr = std::move(next);
        }

        return result;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Sentinel Null Delimiter BFS)

### 6.1 Algorithmic Mechanics
The classical sentinel approach injects a null pointer (`nullptr`) into the queue as an explicit demarcation marker:
- Push `root` followed by `nullptr`.
- When popping a regular node, add value to `currentLevel` and enqueue its children.
- When popping `nullptr`, the current level is complete: push `currentLevel` to `result` and, if the queue is not empty, push a fresh `nullptr`.
This technique is error-prone due to infinite loops if sentinel insertion conditions fail.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(W)$ queue space.

### 6.3 Implementation (Python 3)
```python
from collections import deque
from typing import Optional, List

class Solution:
    def levelOrder(self, root: Optional[TreeNode]) -> List[List[int]]:
        if not root:
            return []

        queue = deque([root, None])
        result = []
        current_level = []

        while queue:
            node = queue.popleft()
            if node is not None:
                current_level.append(node.val)
                if node.left:
                    queue.append(node.left)
                if node.right:
                    queue.append(node.right)
            else:
                result.append(current_level)
                current_level = []
                if queue:
                    queue.append(None)

        return result
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is sampling `queue.size()` preferred over sentinel null nodes in BFS?</summary>
Sampling the integer size avoids allocating and inserting dummy sentinel objects, avoids branching on null values inside the processing loop, and prevents accidental infinite loops if queue exhaustion checks are mishandled.
</details>

<details>
<summary>2. How does the maximum queue size compare between a degenerate tree and a complete binary tree?</summary>
In a degenerate tree (linked list shape), the maximum queue size is 1 ($O(1)$ space).
In a complete binary tree, the last level contains $\lceil N / 2 \rceil$ nodes, requiring $O(N)$ queue space.
</details>

<details>
<summary>3. Why does DFS Pre-Order traversal produce identical level-order arrays?</summary>
DFS visits left subtrees before right subtrees.
Because nodes at depth $d$ on the left side of the tree are visited before nodes at depth $d$ on the right side, elements appended to `result[d]` maintain strict left-to-right ordering.
</details>

<details>
<summary>4. What is the impact of passing `root = nullptr`?</summary>
The initial guard condition `if (!root) return result;` immediately detects an empty tree and returns an empty list `[]` in $O(1)$ time.
</details>

<details>
<summary>5. How does memory pre-allocation with `reserve()` improve performance in C++?</summary>
Since `levelSize` is known prior to dequeuing elements of that level, `currentLevel.reserve(levelSize)` prevents multiple amortized reallocations and vector copies as elements are added.
</details>

<details>
<summary>6. How can this pattern be adapted for Zigzag Level Order Traversal (LeetCode 103)?</summary>
Maintain a boolean flag `leftToRight` inverted at each level.
If false, reverse `currentLevel` before adding to `result`, or use a double-ended queue (`deque`) to push elements in alternate directions.
</details>

<details>
<summary>7. What is the worst-case space consumption of DFS vs BFS?</summary>
DFS space is $O(H)$ (worst-case $O(N)$ for a linked list, best-case $O(\log N)$ for a balanced tree).
BFS space is $O(W)$ (worst-case $O(N)$ for a balanced full tree, best-case $O(1)$ for a linked list).
</details>

<details>
<summary>8. In Go, why is slice reslicing `queue = queue[levelSize:]` efficient?</summary>
Reslicing shifts the view of the slice pointer without reallocating underlying memory buffers.
</details>

<details>
<summary>9. Why must `level == result.size()` trigger a new vector allocation in DFS?</summary>
Because levels are indexed $0, 1, 2, \dots$, arriving at depth $d$ for the first time implies `result` currently has size $d$.
Adding a new empty vector expands the result array to accommodate depth $d$.
</details>

<details>
<summary>10. Is BFS thread-safe if nodes are distributed across worker threads?</summary>
The Two-Queue Ping-Pong approach (Tier 3) is easiest to parallelize: worker threads can process nodes of `currLevel` in parallel, safely accumulating child nodes into thread-local buffers that merge into `nextLevel`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/binary-tree-level-order-traversal.cpp)
- [Python Implementation](../Python/binary-tree-level-order-traversal.py)
- [Java Implementation](../Java/binary-tree-level-order-traversal.java)
- [TypeScript Implementation](../TypeScript/binary-tree-level-order-traversal.ts)
- [Go Implementation](../Golang/binary-tree-level-order-traversal.go)
- [Rust Implementation](../Rust/binary-tree-level-order-traversal.rs)

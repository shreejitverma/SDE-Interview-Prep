---
id: leetcode-0226-invert-binary-tree
title: "LeetCode 0226: Invert Binary Tree"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - dfs
  - bfs
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/invert-binary-tree/"
---

# LeetCode 0226: Invert Binary Tree

## 1. Problem Formalization and Constraints

Given the `root` of a binary tree, invert the tree, and return its root.
Inverting a binary tree reflects the tree across its vertical axis such that for every node, its left and right subtrees are recursively swapped.

### Constraints
- The number of nodes in the tree is in the range $[0, 100]$.
- $-100 \le \text{Node.val} \le 100$

### Examples
- **Example 1**:
  - Input: `root = [4,2,7,1,3,6,9]`
  - Output: `[4,7,2,9,6,3,1]`
- **Example 2**:
  - Input: `root = [2,1,3]`
  - Output: `[2,3,1]`
- **Example 3**:
  - Input: `root = []`
  - Output: `[]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Recursive DFS Subtree Pointer Swap | $O(N)$ | $O(H)$ stack | Swaps `root.left` and `root.right` pointers recursively; optimal elegant recursion. |
| **Tier 2 (Space-Optimized Alternative)** | Iterative Breadth-First Search (Queue Traversal) | $O(N)$ | $O(W)$ | Level-by-level pointer swap avoiding deep recursion call stacks; $W \le N/2$. |
| **Tier 3 (Time-Optimized Alternative)** | Iterative LIFO Stack (Explicit DFS Traversal) | $O(N)$ | $O(H)$ | Simulates recursion stack explicitly on the heap to prevent thread stack exhaustion. |
| **Tier 4 (Brute Force)** | Complete Tree Deep Copy with Mirrored Allocation | $O(N)$ | $O(N)$ heap | Allocates a brand new binary tree cloning nodes in mirrored order instead of in-place rewiring. |

---

## 3. Tier 1: Most Optimal Solution (Recursive DFS Pointer Swap)

### 3.1 Algorithmic Mechanics and Invariant Proof

For any binary tree node `root`:
1. **Base Case**: If `root == null`, return `null`.
2. **Recursive Inversion**:
   Invert the left child: `invertedLeft = invertTree(root.left)`.
   Invert the right child: `invertedRight = invertTree(root.right)`.
3. **Pointer Swap**:
   Assign `root.left = invertedRight`.
   Assign `root.right = invertedLeft`.
4. Return `root`.

**Structural Mirror Invariant**:
By mathematical induction on tree height $H$:
- Base case $H = 0$: Null trees and single-node trees are their own vertical mirror image.
- Inductive hypothesis: Assume all subtrees of height $< H$ are correctly inverted.
- Inductive step: For a node at height $H$, swapping its already inverted left and right subtrees reflects the entire tree across the vertical axis.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Every node is visited exactly once.
- **Space Complexity**: $O(H)$ where $H$ is tree height ($O(\log N)$ for balanced trees, $O(N)$ for skewed degenerate trees).

### 3.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;
        TreeNode* temp = root->left;
        root->left = invertTree(root->right);
        root->right = invertTree(temp);
        return root;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if not root:
            return None
        root.left, root.right = self.invertTree(root.right), self.invertTree(root.left)
        return root
```

#### Java 21
```java
class Solution {
    public TreeNode invertTree(TreeNode root) {
        if (root == null) return null;
        TreeNode temp = root.left;
        root.left = invertTree(root.right);
        root.right = invertTree(temp);
        return root;
    }
}
```

#### TypeScript
```typescript
function invertTree(root: TreeNode | null): TreeNode | null {
    if (root === null) return null;
    const temp = root.left;
    root.left = invertTree(root.right);
    root.right = invertTree(temp);
    return root;
}
```

#### Go
```go
package main

func invertTree(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    root.Left, root.Right = invertTree(root.Right), invertTree(root.Left)
    return root
}
```

#### Rust
```rust
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn invert_tree(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        if let Some(node) = root.clone() {
            let mut n = node.borrow_mut();
            let left = n.left.take();
            let right = n.right.take();
            n.left = Self::invert_tree(right);
            n.right = Self::invert_tree(left);
        }
        root
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Iterative BFS Queue)

### 4.1 Algorithmic Mechanics

We initialize a FIFO queue containing `root`.
While the queue is non-empty:
1. Dequeue `curr = queue.pop()`.
2. Swap `curr.left` and `curr.right`.
3. If `curr.left != null`, push it into the queue.
4. If `curr.right != null`, push it into the queue.

This mirrors the tree level by level without recursive call frames.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ node visits.
- **Space Complexity**: $O(W) \le O(N/2) = O(N)$ queue storage.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <queue>

class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;
        std::queue<TreeNode*> q;
        q.push(root);
        while (!q.empty()) {
            TreeNode* curr = q.front();
            q.pop();
            std::swap(curr->left, curr->right);
            if (curr->left) q.push(curr->left);
            if (curr->right) q.push(curr->right);
        }
        return root;
    }
};
```

#### Python 3.12
```python
from collections import deque
from typing import Optional

class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if not root:
            return None
        q = deque([root])
        while q:
            curr = q.popleft()
            curr.left, curr.right = curr.right, curr.left
            if curr.left:
                q.append(curr.left)
            if curr.right:
                q.append(curr.right)
        return root
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Queue;

class Solution {
    public TreeNode invertTree(TreeNode root) {
        if (root == null) return null;
        Queue<TreeNode> q = new ArrayDeque<>();
        q.offer(root);
        while (!q.isEmpty()) {
            TreeNode curr = q.poll();
            TreeNode temp = curr.left;
            curr.left = curr.right;
            curr.right = temp;
            if (curr.left != null) q.offer(curr.left);
            if (curr.right != null) q.offer(curr.right);
        }
        return root;
    }
}
```

#### TypeScript
```typescript
function invertTree(root: TreeNode | null): TreeNode | null {
    if (!root) return null;
    const q: TreeNode[] = [root];
    let head = 0;
    while (head < q.length) {
        const curr = q[head++];
        const temp = curr.left;
        curr.left = curr.right;
        curr.right = temp;
        if (curr.left) q.push(curr.left);
        if (curr.right) q.push(curr.right);
    }
    return root;
}
```

#### Go
```go
package main

func invertTree(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    q := []*TreeNode{root}
    for len(q) > 0 {
        curr := q[0]
        q = q[1:]
        curr.Left, curr.Right = curr.Right, curr.Left
        if curr.Left != nil {
            q = append(q, curr.Left)
        }
        if curr.Right != nil {
            q = append(q, curr.Right)
        }
    }
    return root
}
```

#### Rust
```rust
use std::collections::VecDeque;
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn invert_tree(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        let root = root?;
        let mut q = VecDeque::new();
        q.push_back(root.clone());
        while let Some(node) = q.pop_front() {
            let mut n = node.borrow_mut();
            let left = n.left.clone();
            let right = n.right.clone();
            n.left = right;
            n.right = left;
            if let Some(ref l) = n.left {
                q.push_back(l.clone());
            }
            if let Some(ref r) = n.right {
                q.push_back(r.clone());
            }
        }
        Some(root)
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Iterative Explicit Stack DFS)

### 5.1 Algorithmic Mechanics

We use an explicit heap-allocated stack `std::vector<TreeNode*>` or `Vec<Rc<RefCell<TreeNode>>>`.
We push `root` and loop until the stack is empty, swapping children and pushing non-null subtrees.
This protects against execution stack overflow while preserving preorder DFS traversal order.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ operations.
- **Space Complexity**: $O(H)$ auxiliary stack frames.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;
        std::vector<TreeNode*> stack = {root};
        while (!stack.empty()) {
            TreeNode* curr = stack.back();
            stack.pop_back();
            std::swap(curr->left, curr->right);
            if (curr->left) stack.push_back(curr->left);
            if (curr->right) stack.push_back(curr->right);
        }
        return root;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if not root:
            return None
        stack = [root]
        while stack:
            curr = stack.pop()
            curr.left, curr.right = curr.right, curr.left
            if curr.left:
                stack.append(curr.left)
            if curr.right:
                stack.append(curr.right)
        return root
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Deque;

class Solution {
    public TreeNode invertTree(TreeNode root) {
        if (root == null) return null;
        Deque<TreeNode> stack = new ArrayDeque<>();
        stack.push(root);
        while (!stack.isEmpty()) {
            TreeNode curr = stack.pop();
            TreeNode temp = curr.left;
            curr.left = curr.right;
            curr.right = temp;
            if (curr.left != null) stack.push(curr.left);
            if (curr.right != null) stack.push(curr.right);
        }
        return root;
    }
}
```

#### TypeScript
```typescript
function invertTree(root: TreeNode | null): TreeNode | null {
    if (!root) return null;
    const stack: TreeNode[] = [root];
    while (stack.length > 0) {
        const curr = stack.pop()!;
        const temp = curr.left;
        curr.left = curr.right;
        curr.right = temp;
        if (curr.left) stack.push(curr.left);
        if (curr.right) stack.push(curr.right);
    }
    return root;
}
```

#### Go
```go
package main

func invertTree(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    stack := []*TreeNode{root}
    for len(stack) > 0 {
        curr := stack[len(stack)-1]
        stack = stack[:len(stack)-1]
        curr.Left, curr.Right = curr.Right, curr.Left
        if curr.Left != nil {
            stack = append(stack, curr.Left)
        }
        if curr.Right != nil {
            stack = append(stack, curr.Right)
        }
    }
    return root
}
```

#### Rust
```rust
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn invert_tree(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        let root = root?;
        let mut stack = vec![root.clone()];
        while let Some(node) = stack.pop() {
            let mut n = node.borrow_mut();
            let left = n.left.clone();
            let right = n.right.clone();
            n.left = right;
            n.right = left;
            if let Some(ref l) = n.left {
                stack.push(l.clone());
            }
            if let Some(ref r) = n.right {
                stack.push(r.clone());
            }
        }
        Some(root)
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Tree Cloning with Mirrored Child Allocation)

### 6.1 Algorithmic Mechanics

We construct a completely new binary tree without mutating any existing nodes:
`newRoot = new TreeNode(root.val)`.
`newRoot.left = cloneInverted(root.right)`.
`newRoot.right = cloneInverted(root.left)`.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$ allocations.
- **Space Complexity**: $O(N)$ new heap nodes allocated.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (!root) return nullptr;
        TreeNode* newRoot = new TreeNode(root->val);
        newRoot->left = invertTree(root->right);
        newRoot->right = invertTree(root->left);
        return newRoot;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if not root:
            return None
        new_root = TreeNode(root.val)
        new_root.left = self.invertTree(root.right)
        new_root.right = self.invertTree(root.left)
        return new_root
```

#### Java 21
```java
class Solution {
    public TreeNode invertTree(TreeNode root) {
        if (root == null) return null;
        TreeNode newRoot = new TreeNode(root.val);
        newRoot.left = invertTree(root.right);
        newRoot.right = invertTree(root.left);
        return newRoot;
    }
}
```

#### TypeScript
```typescript
function invertTree(root: TreeNode | null): TreeNode | null {
    if (!root) return null;
    const newRoot = new TreeNode(root.val);
    newRoot.left = invertTree(root.right);
    newRoot.right = invertTree(root.left);
    return newRoot;
}
```

#### Go
```go
package main

func invertTree(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    return &TreeNode{
        Val:   root.Val,
        Left:  invertTree(root.Right),
        Right: invertTree(root.Left),
    }
}
```

#### Rust
```rust
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn invert_tree(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        let node = root?;
        let n = node.borrow();
        let new_left = Self::invert_tree(n.right.clone());
        let new_right = Self::invert_tree(n.left.clone());
        Some(Rc::new(RefCell::new(TreeNode {
            val: n.val,
            left: new_left,
            right: new_right,
        })))
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. What happens if you invert the left child, then write `root.right = invertTree(root.left)` without a temporary variable?</summary>
In languages without tuple assignment (like C++ or Java), `root.left = invertTree(root.right)` overwrites the original left pointer.
Subsequently calling `invertTree(root.left)` would re-invert the newly assigned right subtree, leaving the original left subtree completely unvisited and orphaned in memory.
Saving `temp = root.left` prior to assignment is essential.
</details>

<details>
<summary>2. Why does Python's `root.left, root.right = f(root.right), f(root.left)` work safely without a temp variable?</summary>
Python evaluates the entire right-hand side expressions first into an anonymous tuple `(f(root.right), f(root.left))` before unpacking and binding to the left-hand side targets.
This guarantees the original references are intact during evaluation.
</details>

<details>
<summary>3. Why did Max Howell's infamous tweet make this problem legendary?</summary>
Max Howell (creator of Homebrew) famously tweeted in 2015: "Google: 90% of our engineers use the software you wrote (Homebrew), but you can't invert a binary tree on a whiteboard so fuck off."
The tweet highlighted the disparity between pragmatic systems engineering contributions and canonical algorithmic whiteboard interviews.
</details>

<details>
<summary>4. How does Rust's `Rc<RefCell<T>>` enable tree mutation safely?</summary>
Rust's standard ownership rules disallow shared mutable references (`&mut`).
`Rc<RefCell<T>>` provides reference counting (`Rc`) for shared ownership combined with interior mutability (`RefCell`), enforcing borrow checking dynamically at runtime.
Calling `.borrow_mut()` allows safe mutation of the child pointers.
</details>

<details>
<summary>5. What is the relationship between tree inversion and symmetrical tree validation (LeetCode 101)?</summary>
A tree is symmetric if and only if it is structurally and value-wise identical to its own inversion.
Testing if `isSymmetric(root)` is equivalent to testing whether `isSameTree(root.left, invertTree(root.right))`.
</details>

<details>
<summary>6. How does inverting a Binary Search Tree (BST) affect its BST ordering invariant?</summary>
A standard BST satisfies $\text{left} < \text{node} < \text{right}$.
Inverting a BST transforms it into a reverse-ordered BST where $\text{left} > \text{node} > \text{right}$.
Its inorder traversal changes from monotonically ascending to monotonically descending.
</details>

<details>
<summary>7. What is the cache locality behavior of pointer-based tree inversion?</summary>
Traversing pointer-linked tree nodes scatters memory references across the heap.
Unless nodes were allocated contiguously using an arena or pool allocator, pointer dereferencing suffers from cold cache line fetches.
</details>

<details>
<summary>8. Can tree inversion be performed using Morris Traversal in $O(1)$ auxiliary space?</summary>
Morris traversal creates temporary threaded back-pointers to simulate a call stack.
However, modifying child pointers while simultaneously creating threaded links risks severing threads prematurely.
Post-order threaded reversal is theoretically possible but adds high implementation complexity.
</details>

<details>
<summary>9. What ensures that this algorithm halts on cyclical graphs?</summary>
Binary trees are by definition directed acyclic graphs where every node except the root has an in-degree of 1.
If an input structure contains an improper cycle, both recursive and queue-based traversals loop infinitely until memory exhaustion.
</details>

<details>
<summary>10. What are the key unit test edge cases for Invert Binary Tree?</summary>
1. Empty tree: `root = null` (returns `null`).
2. Single node: `[1]` (returns `[1]`).
3. Fully left-skewed tree: `[1, 2, null, 3]` -> becomes fully right-skewed `[1, null, 2, null, 3]`.
4. Fully right-skewed tree -> becomes fully left-skewed.
5. Perfect complete binary tree: `[4, 2, 7, 1, 3, 6, 9]`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/invert-binary-tree.cpp)
- [Python Implementation](../Python/invert-binary-tree.py)
- [Java Implementation](../Java/invert-binary-tree.java)
- [TypeScript Implementation](../TypeScript/invert-binary-tree.ts)
- [Go Implementation](../Golang/invert-binary-tree.go)
- [Rust Implementation](../Rust/invert-binary-tree.rs)

---
id: leetcode-0105-construct-binary-tree-from-preorder-and-inorder-traversal
title: "LeetCode 0105: Construct Binary Tree from Preorder and Inorder Traversal"
tags:
  - dsa
  - leetcode
  - tree
  - binary-tree
  - divide-and-conquer
  - hash-table
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/"
---

# LeetCode 0105: Construct Binary Tree from Preorder and Inorder Traversal

## 1. Problem Formalization and Constraints

Given two integer arrays `preorder` and `inorder` where `preorder` is the preorder traversal of a binary tree and `inorder` is the inorder traversal of the same tree, construct and return the binary tree.
All values in `preorder` and `inorder` are guaranteed to be unique.

### Constraints
- $1 \le \text{preorder.length} \le 3000$
- $\text{inorder.length} == \text{preorder.length}$
- $-3000 \le \text{preorder}[i], \text{inorder}[i] \le 3000$
- `preorder` and `inorder` consist of unique values.
- Each value of `inorder` also appears in `preorder`.
- `preorder` is guaranteed to be the preorder traversal of the tree.
- `inorder` is guaranteed to be the inorder traversal of the tree.

### Examples
- **Example 1**:
  - Input: `preorder = [3,9,20,15,7], inorder = [9,3,15,20,7]`
  - Output: `[3,9,20,null,null,15,7]`
- **Example 2**:
  - Input: `preorder = [-1], inorder = [-1]`
  - Output: `[-1]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Hash-Indexed Divide and Conquer | $O(N)$ | $O(N)$ | Maps values to inorder indices; divides ranges in $O(1)$ lookup time per node. |
| **Tier 2 (Space-Optimized DFS)** | In-Order Stop-Bound Recursion | $O(N)$ | $O(H)$ | Eliminates hash map by tracking inorder consumption boundary with a sentinel stop value. |
| **Tier 3 (Iterative)** | Explicit Stack Simulation | $O(N)$ | $O(H)$ | Pushes nodes while preorder descends left; pops along inorder matches to attach right children. |
| **Tier 4 (Brute Force)** | Linear Search & Subarray Slicing | $O(N^2)$ | $O(N^2)$ | Scans inorder array linearly for root; copies array slices at every recursive frame. |

---

## 3. Tier 1: Most Optimal Solution (Hash-Indexed Divide and Conquer)

### 3.1 Algorithmic Mechanics and Invariant Proof

Preorder traversal strictly follows `[Root, ...LeftSubtree..., ...RightSubtree...]`.
Inorder traversal strictly follows `[...LeftSubtree..., Root, ...RightSubtree...]`.

Algorithmic procedure:
1. Build a hash map `inMap` mapping each value `inorder[i]` to its index `i`.
2. Maintain a global or referenced index `preIndex` initialized to 0, representing the next root candidate in `preorder`.
3. Define recursive function `build(inStart, inEnd)`:
   - Base case: If `inStart > inEnd`, the current range is empty; return `nullptr`.
   - Read root value: `rootVal = preorder[preIndex++]`.
   - Allocate root node: `root = new TreeNode(rootVal)`.
   - Look up root's position in inorder array: `mid = inMap[rootVal]`.
   - All elements in `inorder[inStart .. mid - 1]` belong exclusively to the left subtree.
   - All elements in `inorder[mid + 1 .. inEnd]` belong exclusively to the right subtree.
   - Recurse: `root->left = build(inStart, mid - 1)`.
   - Recurse: `root->right = build(mid + 1, inEnd)`.
   - Return `root`.

**Invariant Proof**:
Because preorder visits the root first followed immediately by the left subtree, incrementing `preIndex` sequentially matches the preorder sequence if and only if `root->left` is resolved completely before `root->right`.
Uniqueness of node values guarantees a bijective mapping between keys and inorder positions `mid`.
Subarray partitions are strictly non-overlapping, and every element is placed into exactly one node, proving structural exactness.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Building the hash map takes $O(N)$ time. Constructing $N$ nodes performs $O(1)$ operations per node, yielding linear time.
- **Auxiliary Space Complexity**: $O(N)$. The hash table stores $N$ entries, and the call stack consumes $O(H)$ memory, where $H \le N$.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_map>

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
    TreeNode* buildTree(std::vector<int>& preorder, std::vector<int>& inorder) {
        std::unordered_map<int, int> inMap;
        for (int i = 0; i < static_cast<int>(inorder.size()); ++i) {
            inMap[inorder[i]] = i;
        }

        int preIndex = 0;
        return build(preorder, 0, static_cast<int>(inorder.size()) - 1, preIndex, inMap);
    }

private:
    TreeNode* build(const std::vector<int>& preorder, int inStart, int inEnd,
                    int& preIndex, const std::unordered_map<int, int>& inMap) {
        if (inStart > inEnd) return nullptr;

        int rootVal = preorder[preIndex++];
        TreeNode* root = new TreeNode(rootVal);
        int mid = inMap.at(rootVal);

        root->left = build(preorder, inStart, mid - 1, preIndex, inMap);
        root->right = build(preorder, mid + 1, inEnd, preIndex, inMap);

        return root;
    }
};
```

#### Python 3
```python
from typing import List, Optional

class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

class Solution:
    def buildTree(self, preorder: List[int], inorder: List[int]) -> Optional[TreeNode]:
        in_map = {val: idx for idx, val in enumerate(inorder)}
        pre_iter = iter(preorder)

        def build(in_start: int, in_end: int) -> Optional[TreeNode]:
            if in_start > in_end:
                return None

            root_val = next(pre_iter)
            root = TreeNode(root_val)
            mid = in_map[root_val]

            root.left = build(in_start, mid - 1)
            root.right = build(mid + 1, in_end)

            return root

        return build(0, len(inorder) - 1)
```

#### Java 21
```java
import java.util.HashMap;
import java.util.Map;

class Solution {
    private int preIndex = 0;
    private final Map<Integer, Integer> inMap = new HashMap<>();

    public TreeNode buildTree(int[] preorder, int[] inorder) {
        for (int i = 0; i < inorder.length; i++) {
            inMap.put(inorder[i], i);
        }
        return build(preorder, 0, inorder.length - 1);
    }

    private TreeNode build(int[] preorder, int inStart, int inEnd) {
        if (inStart > inEnd) return null;

        int rootVal = preorder[preIndex++];
        TreeNode root = new TreeNode(rootVal);
        int mid = inMap.get(rootVal);

        root.left = build(preorder, inStart, mid - 1);
        root.right = build(preorder, mid + 1, inEnd);

        return root;
    }
}
```

#### TypeScript 5
```typescript
function buildTree(preorder: number[], inorder: number[]): TreeNode | null {
    const inMap: Map<number, number> = new Map();
    for (let i = 0; i < inorder.length; i++) {
        inMap.set(inorder[i], i);
    }

    let preIndex = 0;

    function build(inStart: number, inEnd: number): TreeNode | null {
        if (inStart > inEnd) return null;

        const rootVal = preorder[preIndex++];
        const root = new TreeNode(rootVal);
        const mid = inMap.get(rootVal)!;

        root.left = build(inStart, mid - 1);
        root.right = build(mid + 1, inEnd);

        return root;
    }

    return build(0, inorder.length - 1);
}
```

#### Go 1.22
```go
package main

func buildTree(preorder []int, inorder []int) *TreeNode {
	inMap := make(map[int]int, len(inorder))
	for i, v := range inorder {
		inMap[v] = i
	}

	preIndex := 0

	var build func(inStart, inEnd int) *TreeNode
	build = func(inStart, inEnd int) *TreeNode {
		if inStart > inEnd {
			return nil
		}

		rootVal := preorder[preIndex]
		preIndex++
		root := &TreeNode{Val: rootVal}
		mid := inMap[rootVal]

		root.Left = build(inStart, mid-1)
		root.Right = build(mid+1, inEnd)

		return root
	}

	return build(0, len(inorder)-1)
}
```

#### Rust 2021
```rust
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::HashMap;

impl Solution {
    pub fn build_tree(preorder: Vec<i32>, inorder: Vec<i32>) -> Option<Rc<RefCell<TreeNode>>> {
        let mut in_map = HashMap::with_capacity(inorder.len());
        for (i, &v) in inorder.iter().enumerate() {
            in_map.insert(v, i);
        }

        let mut pre_index = 0;

        fn build(
            preorder: &[i32],
            in_start: isize,
            in_end: isize,
            pre_index: &mut usize,
            in_map: &HashMap<i32, usize>,
        ) -> Option<Rc<RefCell<TreeNode>>> {
            if in_start > in_end {
                return None;
            }

            let root_val = preorder[*pre_index];
            *pre_index += 1;

            let mid = in_map[&root_val] as isize;

            let left = build(preorder, in_start, mid - 1, pre_index, in_map);
            let right = build(preorder, mid + 1, in_end, pre_index, in_map);

            Some(Rc::new(RefCell::new(TreeNode {
                val: root_val,
                left,
                right,
            })))
        }

        build(&preorder, 0, inorder.len() as isize - 1, &mut pre_index, &in_map)
    }
}
```

---

## 4. Tier 2: Space-Optimized DFS (In-Order Stop-Bound Recursion)

### 4.1 Algorithmic Mechanics
We can avoid the $O(N)$ memory of the hash map by using an inorder sentinel stop value.
Notice that the left subtree stops descending when the next inorder element matches the parent node's value.
By passing a `stop` integer (initialized to an out-of-band sentinel or maximum bound):
- If `preIndex < preorder.size()` and `inorder[inIndex] != stop`:
  - Allocate `root = new TreeNode(preorder[preIndex++])`.
  - Recurse into left subtree with stop value equal to `root->val`.
  - Increment `inIndex++` (as the current root has now been visited in inorder).
  - Recurse into right subtree with the original `stop` value.
This consumes strictly $O(H)$ auxiliary space.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Each element in preorder and inorder is inspected once.
- **Space Complexity**: $O(H)$ auxiliary space for call stack frames.

### 4.3 Implementation (C++20)
```cpp
#include <vector>
#include <climits>

class Solution {
public:
    TreeNode* buildTree(std::vector<int>& preorder, std::vector<int>& inorder) {
        int preIdx = 0;
        int inIdx = 0;
        return build(preorder, inorder, preIdx, inIdx, LONG_MAX);
    }

private:
    TreeNode* build(const std::vector<int>& preorder, const std::vector<int>& inorder,
                    int& preIdx, int& inIdx, long stop) {
        if (preIdx == static_cast<int>(preorder.size())) return nullptr;
        if (inorder[inIdx] == stop) {
            inIdx++;
            return nullptr;
        }

        int val = preorder[preIdx++];
        TreeNode* root = new TreeNode(val);

        root->left = build(preorder, inorder, preIdx, inIdx, val);
        root->right = build(preorder, inorder, preIdx, inIdx, stop);

        return root;
    }
};
```

---

## 5. Tier 3: Iterative Stack Construction

### 5.1 Algorithmic Mechanics
An iterative approach maintains a stack of active ancestors:
- Push the root node `preorder[0]` onto the stack.
- Iterate through `i = 1 .. N - 1`:
  - If `stack.top()->val != inorder[inIdx]`, `preorder[i]` must be the left child of `stack.top()`.
  - Otherwise, we have reached the bottom of a left spine: pop until `stack.top()->val != inorder[inIdx]`.
  - The last popped node is the parent of `preorder[i]`, attaching it as its right child.
  - Push the newly created node onto the stack.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(H)$ stack frames.

### 5.3 Implementation (C++20)
```cpp
#include <vector>
#include <stack>

class Solution {
public:
    TreeNode* buildTree(std::vector<int>& preorder, std::vector<int>& inorder) {
        if (preorder.empty()) return nullptr;

        TreeNode* root = new TreeNode(preorder[0]);
        std::stack<TreeNode*> st;
        st.push(root);

        size_t inIdx = 0;
        for (size_t i = 1; i < preorder.size(); ++i) {
            TreeNode* curr = st.top();
            TreeNode* next = new TreeNode(preorder[i]);

            if (curr->val != inorder[inIdx]) {
                curr->left = next;
            } else {
                while (!st.empty() && st.top()->val == inorder[inIdx]) {
                    curr = st.top();
                    st.pop();
                    ++inIdx;
                }
                curr->right = next;
            }
            st.push(next);
        }

        return root;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Linear Scan & Subarray Slicing)

### 6.1 Algorithmic Mechanics
Without an auxiliary hash map or sentinel parameter, find the index of `rootVal` in `inorder` using linear scan `std::find` in $O(K)$ time for subarray size $K$.
Then allocate copies of the sliced subarrays for left and right partitions.
On skewed trees, this degrades to $O(N^2)$ time and $O(N^2)$ memory copying.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(N^2)$ due to recursive slice copies.

### 6.3 Implementation (Python 3)
```python
from typing import List, Optional

class Solution:
    def buildTree(self, preorder: List[int], inorder: List[int]) -> Optional[TreeNode]:
        if not preorder or not inorder:
            return None

        root_val = preorder[0]
        root = TreeNode(root_val)
        mid = inorder.index(root_val)

        root.left = self.buildTree(preorder[1:1 + mid], inorder[:mid])
        root.right = self.buildTree(preorder[1 + mid:], inorder[mid + 1:])

        return root
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is the uniqueness of node values critical for this algorithm?</summary>
Without unique keys, looking up `rootVal` in `inorder` produces ambiguous indices, rendering deterministic partitioning into left and right subtrees impossible.
</details>

<details>
<summary>2. Why must the left subtree be constructed before the right subtree in Tier 1?</summary>
Because `preorder` traverses `[Root, Left, Right]`, `preIndex` increments through left subtree nodes first.
If the right subtree were evaluated first, `preorder[preIndex]` would misassign right subtree candidates to the left subtree.
</details>

<details>
<summary>3. Can a binary tree be uniquely reconstructed from Preorder and Postorder traversals?</summary>
No, not for a general binary tree.
When a node has only one child, preorder and postorder cannot differentiate whether the child is a left child or a right child (unless it is a full binary tree).
</details>

<details>
<summary>4. What determines the number of elements in the left subtree?</summary>
The left subtree contains exactly $\text{mid} - \text{inStart}$ elements, where $\text{mid}$ is the index of `rootVal` in `inorder`.
</details>

<details>
<summary>5. How does Tier 2 eliminate the $O(N)$ hash map space?</summary>
Tier 2 observes that the inorder sequence encounters the root node immediately after completing all nodes in its left subtree.
By using `rootVal` as the stop boundary, recursion terminates naturally without pre-computing index lookups.
</details>

<details>
<summary>6. What happens if the input has a single node?</summary>
`inStart == inEnd == 0`, allocating the root node and triggering base cases `inStart > inEnd` for both children, correctly yielding a leaf node.
</details>

<details>
<summary>7. Why does slicing arrays in Python degrade performance to $O(N^2)$?</summary>
Slicing `arr[a:b]` creates a new list copy of length $b - a$.
Summing slice lengths over an unbalanced tree yields $\sum_{k=1}^N k = O(N^2)$ time and space overhead.
</details>

<details>
<summary>8. How does this problem relate to LeetCode 106 (Construct from Inorder and Postorder)?</summary>
In LeetCode 106, the root is at `postorder[postIndex--]`, which traverses root then right subtree then left subtree.
Consequently, the right subtree must be constructed before the left subtree.
</details>

<details>
<summary>9. What is the maximum recursion depth on a skewed tree?</summary>
$N$ stack frames, which may require increasing stack size or using the Tier 3 iterative approach.
</details>

<details>
<summary>10. How does memory alignment impact cache performance during tree construction?</summary>
Nodes allocated dynamically on the heap with `new` may be scattered across memory pages.
An arena allocator or flat array storage (`vector<TreeNode>`) significantly improves cache locality.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/construct-binary-tree-from-preorder-and-inorder-traversal.cpp)
- [Python Implementation](../Python/construct-binary-tree-from-preorder-and-inorder-traversal.py)
- [Java Implementation](../Java/construct-binary-tree-from-preorder-and-inorder-traversal.java)
- [TypeScript Implementation](../TypeScript/construct-binary-tree-from-preorder-and-inorder-traversal.ts)
- [Go Implementation](../Golang/construct-binary-tree-from-preorder-and-inorder-traversal.go)
- [Rust Implementation](../Rust/construct-binary-tree-from-preorder-and-inorder-traversal.rs)

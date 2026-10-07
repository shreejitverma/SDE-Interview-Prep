---
id: leetcode-0138-copy-list-with-random-pointer
title: "LeetCode 0138: Copy List with Random Pointer"
tags:
  - dsa
  - leetcode
  - linked-list
  - hash-table
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/copy-list-with-random-pointer/"
---

# LeetCode 0138: Copy List with Random Pointer

## 1. Problem Formalization and Constraints

A linked list of length $n$ is given such that each node contains an additional random pointer, which could point to any node in the list, or `null`.
Construct a deep copy of the list.
The deep copy should consist of exactly $n$ brand new nodes, where each new node has its value set to the value of its corresponding original node.
Both the `next` and `random` pointer of the new nodes should point to new nodes in the copied list such that the pointers in the original list and copied list represent the same list state.
None of the pointers in the new list should point to nodes in the original list.

For example, if there are two nodes `X` and `Y` in the original list, where `X.random --> Y`, then for the corresponding two nodes `x` and `y` in the copied list, `x.random --> y`.
Return the head of the copied linked list.

The linked list is represented in the input/output as a list of $n$ nodes.
Each node is represented as a pair of `[val, random_index]` where:
- `val`: an integer representing `Node.val`
- `random_index`: the index of the node (0-indexed) that the `random` pointer points to, or `null` if it does not point to any node.

Your code will only be given the `head` of the original linked list.

### Constraints
- $0 \le n \le 1000$
- $-10^4 \le \text{Node.val} \le 10^4$
- `Node.random` is `null` or is pointing to some node in the linked list.

### Examples
- **Example 1**:
  - Input: `head = [[7,null],[13,0],[11,4],[10,2],[1,0]]`
  - Output: `[[7,null],[13,0],[11,4],[10,2],[1,0]]`
- **Example 2**:
  - Input: `head = [[1,1],[2,1]]`
  - Output: `[[1,1],[2,1]]`
- **Example 3**:
  - Input: `head = [[3,null],[3,0],[3,null]]`
  - Output: `[[3,null],[3,0],[3,null]]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | In-Place Node Interleaving | $O(N)$ | $O(1)$ | Interleaves cloned nodes directly after originals (`A -> A' -> B -> B'`); allows $O(1)$ random resolution before unweaving. |
| **Tier 2 (Hash Map)** | Hash Table Mapping | $O(N)$ | $O(N)$ | Maps each original node address to its cloned counterpart in a hash table; simple two-pass approach but incurs $O(N)$ hash overhead. |
| **Tier 3 (DFS / Memoization)** | Recursive Graph Traversal | $O(N)$ | $O(N)$ | Treats the list as a directed graph of out-degree 2; clones nodes recursively while caching visited nodes; risks recursion limit. |
| **Tier 4 (Brute Force)** | Index-Based Linear Search | $O(N^2)$ | $O(1)$ | Clones the linear backbone, then searches the entire list from the start to find the index of each target random node. |

---

## 3. Tier 1: Most Optimal Solution (In-Place Node Interleaving)

### 3.1 Algorithmic Mechanics and Invariant Proof

The interleave algorithm proceeds in three linear passes:
1. **Pass 1: Node Duplication**:
   For each original node `curr`, construct a new clone node `copy` with identical `val`.
   Insert `copy` immediately after `curr`: `curr -> copy -> curr.next`.
2. **Pass 2: Random Pointer Wiring**:
   For each original node `curr`, its clone is positioned at `curr.next`.
   If `curr.random` is non-null, the clone of `curr.random` is located at `curr.random.next`.
   Therefore: `curr.next.random = curr.random.next`.
3. **Pass 3: List Decoupling (Unweaving)**:
   Restore the original list's `next` pointers while simultaneously extracting the cloned nodes into an independent deep-copied list.

**Invariant Proof**:
Let the original list be $L = \langle u_1, u_2, \dots, u_n \rangle$.
After Pass 1, the list is transformed into $L' = \langle u_1, v_1, u_2, v_2, \dots, u_n, v_n \rangle$ where $v_i$ is the clone of $u_i$.
Because $u_i.\text{next} = v_i$, any arbitrary node reference $u_k$ can access its clone via $u_k.\text{next}$.
In Pass 2, whenever $u_i.\text{random} = u_j$, setting $v_i.\text{random} = u_i.\text{random}.\text{next}$ sets $v_i.\text{random} = v_j$.
Because all nodes exist prior to Pass 2, all forward and backward random references resolve correctly without ambiguity or cycle complications.
Pass 3 cleanly unweaves $L'$ back into $L$ and the independent clone list $C = \langle v_1, v_2, \dots, v_n \rangle$.
Thus, complete deep copy correctness is guaranteed in $O(1)$ auxiliary memory.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ where $N$ is the number of nodes in the linked list. Each of the three passes touches every node exactly once.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space. Memory is allocated strictly for the $N$ newly requested nodes that constitute the deep copy result.

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;

    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};
*/

class Solution {
public:
    Node* copyRandomList(Node* head) {
        if (!head) {
            return nullptr;
        }

        // 1. Interleave duplicate nodes
        for (auto* curr = head; curr; curr = curr->next->next) {
            auto* copy = new Node(curr->val);
            copy->next = curr->next;
            curr->next = copy;
        }

        // 2. Assign random pointers
        for (auto* curr = head; curr; curr = curr->next->next) {
            if (curr->random) {
                curr->next->random = curr->random->next;
            }
        }

        // 3. Separate original and cloned lists
        Node dummy(0);
        for (auto* curr = head, *copy_curr = &dummy;
             curr;
             copy_curr = copy_curr->next, curr = curr->next) {
            copy_curr->next = curr->next;
            curr->next = curr->next->next;
        }

        return dummy.next;
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N)
# Space: O(1)

"""
# Definition for a Node.
class Node:
    def __init__(self, x: int, next: 'Node' = None, random: 'Node' = None):
        self.val = int(x)
        self.next = next
        self.random = random
"""

class Solution:
    def copyRandomList(self, head: 'Optional[Node]') -> 'Optional[Node]':
        if not head:
            return None

        # 1. Interleave cloned nodes
        curr = head
        while curr:
            copy = Node(curr.val)
            copy.next = curr.next
            curr.next = copy
            curr = copy.next

        # 2. Assign random pointers
        curr = head
        while curr:
            if curr.random:
                curr.next.random = curr.random.next
            curr = curr.next.next

        # 3. Separate lists
        dummy = Node(0)
        copy_curr = dummy
        curr = head
        while curr:
            copy_curr.next = curr.next
            curr.next = curr.next.next
            copy_curr = copy_curr.next
            curr = curr.next

        return dummy.next
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

/*
// Definition for a Node.
class Node {
    int val;
    Node next;
    Node random;

    public Node(int val) {
        this.val = val;
        this.next = null;
        this.random = null;
    }
}
*/

class Solution {
    public Node copyRandomList(Node head) {
        if (head == null) {
            return null;
        }

        // 1. Create duplicate nodes interleaved in the list
        Node curr = head;
        while (curr != null) {
            Node copy = new Node(curr.val);
            copy.next = curr.next;
            curr.next = copy;
            curr = copy.next;
        }

        // 2. Assign random pointers for cloned nodes
        curr = head;
        while (curr != null) {
            if (curr.random != null) {
                curr.next.random = curr.random.next;
            }
            curr = curr.next.next;
        }

        // 3. Separate original and cloned lists
        Node originalCurr = head;
        Node copyHead = head.next;
        Node copyCurr = copyHead;

        while (originalCurr != null) {
            originalCurr.next = originalCurr.next.next;
            if (copyCurr.next != null) {
                copyCurr.next = copyCurr.next.next;
            }
            originalCurr = originalCurr.next;
            copyCurr = copyCurr.next;
        }

        return copyHead;
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
// Space: O(1)

/**
 * Definition for _Node.
 * class _Node {
 *     val: number
 *     next: _Node | null
 *     random: _Node | null
 *     constructor(val?: number, next?: _Node, random?: _Node) {
 *         this.val = (val===undefined ? 0 : val)
 *         this.next = (next===undefined ? null : next)
 *         this.random = (random===undefined ? null : random)
 *     }
 * }
 */

function copyRandomList(head: _Node | null): _Node | null {
    if (!head) return null;

    // 1. Interleave cloned nodes
    let curr: _Node | null = head;
    while (curr) {
        const copy = new _Node(curr.val, curr.next, null);
        curr.next = copy;
        curr = copy.next;
    }

    // 2. Assign random pointers
    curr = head;
    while (curr) {
        if (curr.random) {
            curr.next!.random = curr.random.next;
        }
        curr = curr.next!.next;
    }

    // 3. Separate the lists
    let orig: _Node | null = head;
    const copyHead = head.next;
    let copy: _Node | null = copyHead;

    while (orig) {
        orig.next = orig.next ? orig.next.next : null;
        if (copy && copy.next) {
            copy.next = copy.next.next;
        }
        orig = orig.next;
        copy = copy ? copy.next : null;
    }

    return copyHead;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

/**
 * Definition for a Node.
 * type Node struct {
 *     Val int
 *     Next *Node
 *     Random *Node
 * }
 */

func copyRandomList(head *Node) *Node {
	if head == nil {
		return nil
	}

	// 1. Interleave cloned nodes
	curr := head
	for curr != nil {
		copyNode := &Node{
			Val:  curr.Val,
			Next: curr.Next,
		}
		curr.Next = copyNode
		curr = copyNode.Next
	}

	// 2. Assign random pointers
	curr = head
	for curr != nil {
		if curr.Random != nil {
			curr.Next.Random = curr.Random.Next
		}
		curr = curr.Next.Next
	}

	// 3. Separate original and copied lists
	orig := head
	copyHead := head.Next
	copyCurr := copyHead

	for orig != nil {
		orig.Next = orig.Next.Next
		if copyCurr.Next != nil {
			copyCurr.Next = copyCurr.Next.Next
		}
		orig = orig.Next
		copyCurr = copyCurr.Next
	}

	return copyHead
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

pub struct Node {
    pub val: i32,
    pub next: Option<Rc<RefCell<Node>>>,
    pub random: Option<Rc<RefCell<Node>>>,
}

impl Node {
    pub fn new(val: i32) -> Rc<RefCell<Self>> {
        Rc::new(RefCell::new(Node {
            val,
            next: None,
            random: None,
        }))
    }
}

pub struct Solution;

impl Solution {
    pub fn copy_random_list(head: Option<Rc<RefCell<Node>>>) -> Option<Rc<RefCell<Node>>> {
        let head_ref = head.as_ref()?;
        let mut visited: HashMap<usize, Rc<RefCell<Node>>> = HashMap::new();

        let mut curr = Some(Rc::clone(head_ref));
        while let Some(node) = curr {
            let addr = Rc::as_ptr(&node) as usize;
            let val = node.borrow().val;
            visited.insert(addr, Node::new(val));
            curr = node.borrow().next.clone();
        }

        curr = Some(Rc::clone(head_ref));
        while let Some(node) = curr {
            let addr = Rc::as_ptr(&node) as usize;
            let clone_node = visited.get(&addr).unwrap();

            if let Some(next_node) = &node.borrow().next {
                let next_addr = Rc::as_ptr(next_node) as usize;
                clone_node.borrow_mut().next = visited.get(&next_addr).cloned();
            }

            if let Some(random_node) = &node.borrow().random {
                let random_addr = Rc::as_ptr(random_node) as usize;
                clone_node.borrow_mut().random = visited.get(&random_addr).cloned();
            }

            curr = node.borrow().next.clone();
        }

        let head_addr = Rc::as_ptr(head_ref) as usize;
        visited.get(&head_addr).cloned()
    }
}
```

---

## 4. Tier 2: Hash Map Mapping Solution

### 4.1 Mechanical Description
Maintain a hash table `visited: Map[OldNode, NewNode]` mapping original node memory addresses to newly instantiated clone nodes.
In the first pass, iterate through the list, allocate a clone for every node, and record the mapping in the table.
In the second pass, iterate through the list again and wire the pointers:
- `visited[curr].next = visited.get(curr.next, null)`
- `visited[curr].random = visited.get(curr.random, null)`

### 4.2 Trade-offs
- Straightforward to implement and reason about.
- Allocates $O(N)$ extra hash table memory, increasing heap allocations and degrading cache locality.

---

## 5. Tier 3: Recursive Graph Traversal with Memoization (DFS)

### 5.1 Mechanical Description
View the linked list as a general directed graph where each node has up to two directed outgoing edges: `next` and `random`.
Perform depth-first search starting from `head`.
When visiting node `u`:
- If `u` is `null`, return `null`.
- If `u` is already in the memoization table, return `memo[u]`.
- Otherwise, instantiate `v = Node(u.val)`, record `memo[u] = v`, and recursively populate:
  - `v.next = dfs(u.next)`
  - `v.random = dfs(u.random)`

### 5.2 Trade-offs
- Elegant graph formulation.
- Uses $O(N)$ stack frames; risks call stack overflow for deep lists ($N > 10^4$).

---

## 6. Tier 4: Index-Based Linear Search (Brute Force Baseline)

### 6.1 Mechanical Description
First, clone the singly linked list using only `next` pointers.
For each original node `curr`, determine the 0-indexed position $k$ of `curr.random` by linearly traversing the original list from `head`.
Once the index $k$ is determined, traverse the cloned list from `copyHead` for $k$ steps to locate the target node, and set `copyCurr.random = targetCopy`.

### 6.2 Trade-offs
- Avoids auxiliary memory without mutating the original list structure.
- Quadratic time complexity $O(N^2)$ due to repeated linear scans.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Interleaving Cache Locality**: Interleaving cloned nodes creates adjacent memory allocations during creation, improving cache hit rates during the second pass.
2. **Pointer Unweaving Invariant**: Care must be taken during the third pass to fully restore original `curr.next` pointers; failure to restore corrupts the input data structure.
3. **Reference Counting and Cycles**: In languages with automatic reference counting (such as Swift or Rust's `Rc`), cyclic random pointers create circular references that leak unless broken or handled via weak pointers.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Empty list | `head = null` | Returns `null` | Early exit `if (!head) return nullptr;` |
| Single node, self-referencing random | `head = [[1, 0]]` | Cloned node has `random` pointing to itself | `curr.next.random = curr.random.next` resolves `copy.random = copy` cleanly |
| Single node, null random | `head = [[1, null]]` | Cloned node has `random = null` | Guard `if (curr.random)` preserves default `null` |
| Multiple identical values | `values = [3, 3, 3]` | Clones map by identity, not value | Address/pointer-based interleaving avoids value collision |
| Random pointing forward | Node 0 points to Node 4 | Resolves forward link correctly | All nodes are created in Pass 1 before Pass 2 wires links |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does the interleaving approach not require auxiliary space?
Because the original list itself acts as the hash table, using `curr.next` to store the pointer to the cloned node.

### 2. Can the input list be modified during execution?
Yes, temporarily modifying the list during interleaving is permissible as long as Pass 3 completely restores the original pointers.

### 3. What happens if `random` points to a node that has not been created yet?
In the two-pass hash map and three-pass interleaving approaches, all nodes are created before any random pointers are assigned, eliminating ordering issues.

### 4. How does this problem relate to general graph cloning?
A linked list with random pointers is a directed graph where each vertex has out-degree at most 2. Graph cloning algorithms apply directly.

### 5. Why is the hash map approach preferred in production systems?
The hash map approach does not mutate the source list, making it thread-safe for concurrent readers.

### 6. How do we test that a copy is a deep copy?
Verify that all cloned node values and structure match, while `clone != orig` and no pointer in `clone` references any node in `orig`.

### 7. Can we solve this problem in a single pass?
Yes, with a hash table that creates nodes on demand whenever a previously unseen `next` or `random` node is referenced.

### 8. What is the impact of list lengths up to $10^5$ on the recursive DFS approach?
Python and Java standard call stacks would encounter a `StackOverflowError` without increasing the recursion limit.

### 9. Why does Rust require `Rc<RefCell<Node>>` for this data structure?
Safe Rust enforces single ownership. Random pointers and shared links violate the borrowing model without interior mutability and reference counting.

### 10. Does unweaving modify the returned cloned list?
No, unweaving separates the two lists into two distinct, valid chains.

---

## 10. Related Problems and Systematic Progression Links

- [[0021-Merge-Two-Sorted-Lists]]: Linear linked list pointer manipulation.
- [[0133-Clone-Graph]]: Deep cloning of a general undirected graph.
- [[0141-Linked-List-Cycle]]: Fast and slow pointer cycle detection.
- [[0143-Reorder-List]]: List splitting, reversing, and interleaving.
- [[0206-Reverse-Linked-List]]: Fundamental linked list pointer reversal.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/copy-list-with-random-pointer.cpp)
- [Python Implementation](../Python/copy-list-with-random-pointer.py)
- [Java Implementation](../Java/copy-list-with-random-pointer.java)
- [TypeScript Implementation](../TypeScript/copy-list-with-random-pointer.ts)
- [Go Implementation](../Golang/copy-list-with-random-pointer.go)
- [Rust Implementation](../Rust/copy-list-with-random-pointer.rs)

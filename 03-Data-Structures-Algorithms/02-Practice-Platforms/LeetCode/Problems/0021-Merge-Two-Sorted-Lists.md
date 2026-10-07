---
id: leetcode-0021-merge-two-sorted-lists
title: "LeetCode 0021: Merge Two Sorted Lists"
tags:
  - dsa
  - leetcode
  - linked-list
  - recursion
  - two-pointers
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/merge-two-sorted-lists/"
---

# LeetCode 0021: Merge Two Sorted Lists

## 1. Problem Formalization and Constraints

You are given the heads of two sorted linked lists `list1` and `list2`.
Merge the two lists into one sorted list.
The list should be made by splicing together the nodes of the first two lists.
Return the head of the merged linked list.

### Constraints
- The number of nodes in both lists is in the range $[0, 50]$.
- $-100 \le \text{Node.val} \le 100$
- Both `list1` and `list2` are sorted in non-decreasing order.

### Examples
- **Example 1**:
  - Input: `list1 = [1,2,4], list2 = [1,3,4]`
  - Output: `[1,1,2,3,4,4]`
- **Example 2**:
  - Input: `list1 = [], list2 = []`
  - Output: `[]`
- **Example 3**:
  - Input: `list1 = [], list2 = [0]`
  - Output: `[0]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative Two Pointers with Sentinel Dummy Head | $O(N + M)$ | $O(1)$ | Splices node pointers in-place using a sentinel node to avoid boundary branching. |
| **Tier 2 (Space-Optimized Alternative)** | Pointer-to-Pointer (Indirect Pointer) Traversal | $O(N + M)$ | $O(1)$ | Uses Linus Torvalds' indirect pointer pattern (`ListNode**`) with zero sentinel nodes. |
| **Tier 3 (Time-Optimized Alternative)** | Structural Recursive Splicing | $O(N + M)$ | $O(N + M)$ | Elegantly expresses merge recurrence; incurs stack frame overhead proportional to depth. |
| **Tier 4 (Brute Force)** | Value Extraction, External Sorting, and Reallocation | $O(K \log K)$ | $O(K)$ | Dumps all elements to an array, sorts, and constructs a new linked list ($K = N + M$). |

---

## 3. Tier 1: Most Optimal Solution (Iterative Dummy Node Splice)

### 3.1 Algorithmic Mechanics and Invariant Proof

We allocate a stack sentinel node `dummy` and maintain a tail pointer `current = &dummy`.
While both `list1` and `list2` are non-null:
1. Compare `list1.val` and `list2.val`.
2. Attach the smaller node to `current.next`.
3. Advance the corresponding list pointer by one node.
4. Advance `current = current.next`.
When either list becomes null, attach the remaining non-null list directly:
`current.next = (list1 != null) ? list1 : list2`.
Return `dummy.next`.

**Inductive Invariant**:
At every step, the linked list starting at `dummy.next` and ending at `current` forms a strictly sorted sequence containing the smallest $k$ nodes from the union of both inputs.
Because node pointers are rewired without allocating new nodes, auxiliary heap space is strictly $O(1)$.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N + M)$. Exactly one comparison per spliced node until one list exhausts.
- **Space Complexity**: $O(1)$. Only pointer scalars are maintained on the stack.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy(0);
        ListNode* current = &dummy;
        while (list1 && list2) {
            if (list1->val <= list2->val) {
                current->next = list1;
                list1 = list1->next;
            } else {
                current->next = list2;
                list2 = list2->next;
            }
            current = current->next;
        }
        current->next = list1 ? list1 : list2;
        return dummy.next;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        dummy = ListNode(0)
        current = dummy
        while list1 and list2:
            if list1.val <= list2.val:
                current.next = list1
                list1 = list1.next
            else:
                current.next = list2
                list2 = list2.next
            current = current.next
        current.next = list1 if list1 else list2
        return dummy.next
```

#### Java 21
```java
class Solution {
    public ListNode mergeTwoLists(ListNode list1, ListNode list2) {
        ListNode dummy = new ListNode(0);
        ListNode current = dummy;
        while (list1 != null && list2 != null) {
            if (list1.val <= list2.val) {
                current.next = list1;
                list1 = list1.next;
            } else {
                current.next = list2;
                list2 = list2.next;
            }
            current = current.next;
        }
        current.next = (list1 != null) ? list1 : list2;
        return dummy.next;
    }
}
```

#### TypeScript
```typescript
function mergeTwoLists(list1: ListNode | null, list2: ListNode | null): ListNode | null {
    const dummy = new ListNode(0);
    let current = dummy;
    while (list1 !== null && list2 !== null) {
        if (list1.val <= list2.val) {
            current.next = list1;
            list1 = list1.next;
        } else {
            current.next = list2;
            list2 = list2.next;
        }
        current = current.next;
    }
    current.next = list1 !== null ? list1 : list2;
    return dummy.next;
}
```

#### Go
```go
package main

func mergeTwoLists(list1 *ListNode, list2 *ListNode) *ListNode {
    dummy := &ListNode{}
    current := dummy
    for list1 != nil && list2 != nil {
        if list1.Val <= list2.Val {
            current.Next = list1
            list1 = list1.Next
        } else {
            current.Next = list2
            list2 = list2.Next
        }
        current = current.Next
    }
    if list1 != nil {
        current.Next = list1
    } else {
        current.Next = list2
    }
    return dummy.Next
}
```

#### Rust
```rust
impl Solution {
    pub fn merge_two_lists(
        mut list1: Option<Box<ListNode>>,
        mut list2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut dummy = Box::new(ListNode::new(0));
        let mut current = &mut dummy;

        while list1.is_some() && list2.is_some() {
            if list1.as_ref().unwrap().val <= list2.as_ref().unwrap().val {
                let mut next = list1.take().unwrap();
                list1 = next.next.take();
                current.next = Some(next);
            } else {
                let mut next = list2.take().unwrap();
                list2 = next.next.take();
                current.next = Some(next);
            }
            current = current.next.as_mut().unwrap();
        }

        current.next = if list1.is_some() { list1 } else { list2 };
        dummy.next
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Indirect Pointer / Double Pointer)

### 4.1 Algorithmic Mechanics

In systems languages like C and C++, the "Linus Torvalds indirect pointer" pattern uses a pointer to a pointer (`ListNode** pp = &head`).
We dereference `*pp` to link directly into the pointer variable that points to the head, avoiding creating or stack-allocating a dummy sentinel node.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N + M)$ comparisons.
- **Space Complexity**: $O(1)$ auxiliary memory; zero dummy node objects.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode* head = nullptr;
        ListNode** pp = &head;
        while (list1 && list2) {
            if (list1->val <= list2->val) {
                *pp = list1;
                list1 = list1->next;
            } else {
                *pp = list2;
                list2 = list2->next;
            }
            pp = &((*pp)->next);
        }
        *pp = list1 ? list1 : list2;
        return head;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        if not list1: return list2
        if not list2: return list1
        if list1.val > list2.val:
            list1, list2 = list2, list1
        head = list1
        while list1.next and list2:
            if list1.next.val <= list2.val:
                list1 = list1.next
            else:
                next_l1 = list1.next
                list1.next = list2
                list2 = next_l1
                list1 = list1.next
        if list2:
            list1.next = list2
        return head
```

#### Java 21
```java
class Solution {
    public ListNode mergeTwoLists(ListNode list1, ListNode list2) {
        if (list1 == null) return list2;
        if (list2 == null) return list1;
        if (list1.val > list2.val) {
            ListNode temp = list1;
            list1 = list2;
            list2 = temp;
        }
        ListNode head = list1;
        while (list1.next != null && list2 != null) {
            if (list1.next.val <= list2.val) {
                list1 = list1.next;
            } else {
                ListNode next1 = list1.next;
                list1.next = list2;
                list2 = next1;
                list1 = list1.next;
            }
        }
        if (list2 != null) {
            list1.next = list2;
        }
        return head;
    }
}
```

#### TypeScript
```typescript
function mergeTwoLists(list1: ListNode | null, list2: ListNode | null): ListNode | null {
    if (!list1) return list2;
    if (!list2) return list1;
    if (list1.val > list2.val) {
        const temp = list1;
        list1 = list2;
        list2 = temp;
    }
    const head = list1;
    while (list1.next && list2) {
        if (list1.next.val <= list2.val) {
            list1 = list1.next;
        } else {
            const next1 = list1.next;
            list1.next = list2;
            list2 = next1;
            list1 = list1.next;
        }
    }
    if (list2) {
        list1.next = list2;
    }
    return head;
}
```

#### Go
```go
package main

func mergeTwoLists(list1 *ListNode, list2 *ListNode) *ListNode {
    if list1 == nil {
        return list2
    }
    if list2 == nil {
        return list1
    }
    if list1.Val > list2.Val {
        list1, list2 = list2, list1
    }
    head := list1
    for list1.Next != nil && list2 != nil {
        if list1.Next.Val <= list2.Val {
            list1 = list1.Next
        } else {
            next1 := list1.Next
            list1.Next = list2
            list2 = next1
            list1 = list1.Next
        }
    }
    if list2 != nil {
        list1.Next = list2
    }
    return head
}
```

#### Rust
```rust
impl Solution {
    pub fn merge_two_lists(
        mut list1: Option<Box<ListNode>>,
        mut list2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut head = None;
        let mut current = &mut head;

        while list1.is_some() && list2.is_some() {
            if list1.as_ref().unwrap().val <= list2.as_ref().unwrap().val {
                let mut node = list1.take().unwrap();
                list1 = node.next.take();
                *current = Some(node);
            } else {
                let mut node = list2.take().unwrap();
                list2 = node.next.take();
                *current = Some(node);
            }
            current = &mut current.as_mut().unwrap().next;
        }

        *current = if list1.is_some() { list1 } else { list2 };
        head
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Recursive Splicing)

### 5.1 Algorithmic Mechanics

We define the recurrence relation:
$$\text{merge}(L_1, L_2) = \begin{cases} L_2 & \text{if } L_1 = \emptyset \\ L_1 & \text{if } L_2 = \emptyset \\ L_1 \oplus \text{merge}(L_1.\text{next}, L_2) & \text{if } L_1.\text{val} \le L_2.\text{val} \\ L_2 \oplus \text{merge}(L_1, L_2.\text{next}) & \text{otherwise} \end{cases}$$

This structure directly reflects the mathematical definition of merging sorted streams.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N + M)$ recursive steps.
- **Space Complexity**: $O(N + M)$ stack frames.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        if (!list1) return list2;
        if (!list2) return list1;
        if (list1->val <= list2->val) {
            list1->next = mergeTwoLists(list1->next, list2);
            return list1;
        } else {
            list2->next = mergeTwoLists(list1, list2->next);
            return list2;
        }
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        if not list1:
            return list2
        if not list2:
            return list1
        if list1.val <= list2.val:
            list1.next = self.mergeTwoLists(list1.next, list2)
            return list1
        else:
            list2.next = self.mergeTwoLists(list1, list2.next)
            return list2
```

#### Java 21
```java
class Solution {
    public ListNode mergeTwoLists(ListNode list1, ListNode list2) {
        if (list1 == null) return list2;
        if (list2 == null) return list1;
        if (list1.val <= list2.val) {
            list1.next = mergeTwoLists(list1.next, list2);
            return list1;
        } else {
            list2.next = mergeTwoLists(list1, list2.next);
            return list2;
        }
    }
}
```

#### TypeScript
```typescript
function mergeTwoLists(list1: ListNode | null, list2: ListNode | null): ListNode | null {
    if (!list1) return list2;
    if (!list2) return list1;
    if (list1.val <= list2.val) {
        list1.next = mergeTwoLists(list1.next, list2);
        return list1;
    } else {
        list2.next = mergeTwoLists(list1, list2.next);
        return list2;
    }
}
```

#### Go
```go
package main

func mergeTwoLists(list1 *ListNode, list2 *ListNode) *ListNode {
    if list1 == nil {
        return list2
    }
    if list2 == nil {
        return list1
    }
    if list1.Val <= list2.Val {
        list1.Next = mergeTwoLists(list1.Next, list2)
        return list1
    }
    list2.Next = mergeTwoLists(list1, list2.Next)
    return list2
}
```

#### Rust
```rust
impl Solution {
    pub fn merge_two_lists(
        list1: Option<Box<ListNode>>,
        list2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        match (list1, list2) {
            (None, l2) => l2,
            (l1, None) => l1,
            (Some(mut n1), Some(mut n2)) => {
                if n1.val <= n2.val {
                    n1.next = Self::merge_two_lists(n1.next, Some(n2));
                    Some(n1)
                } else {
                    n2.next = Self::merge_two_lists(Some(n1), n2.next);
                    Some(n2)
                }
            }
        }
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Value Extraction and Re-sorting)

### 6.1 Algorithmic Mechanics

We traverse both linked lists and collect all scalar integer values into a dynamic array.
We sort the array using standard quicksort/mergesort.
Finally, we construct a brand new linked list from the sorted values.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(K \log K)$ where $K = N + M$.
- **Space Complexity**: $O(K)$ auxiliary memory allocating a new vector and nodes.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        std::vector<int> vals;
        while (list1) {
            vals.push_back(list1->val);
            list1 = list1->next;
        }
        while (list2) {
            vals.push_back(list2->val);
            list2 = list2->next;
        }
        std::sort(vals.begin(), vals.end());
        ListNode dummy(0);
        ListNode* curr = &dummy;
        for (int v : vals) {
            curr->next = new ListNode(v);
            curr = curr->next;
        }
        return dummy.next;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        vals = []
        while list1:
            vals.append(list1.val)
            list1 = list1.next
        while list2:
            vals.append(list2.val)
            list2 = list2.next
        vals.sort()
        dummy = ListNode(0)
        curr = dummy
        for v in vals:
            curr.next = ListNode(v)
            curr = curr.next
        return dummy.next
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class Solution {
    public ListNode mergeTwoLists(ListNode list1, ListNode list2) {
        List<Integer> vals = new ArrayList<>();
        while (list1 != null) {
            vals.add(list1.val);
            list1 = list1.next;
        }
        while (list2 != null) {
            vals.add(list2.val);
            list2 = list2.next;
        }
        Collections.sort(vals);
        ListNode dummy = new ListNode(0);
        ListNode curr = dummy;
        for (int v : vals) {
            curr.next = new ListNode(v);
            curr = curr.next;
        }
        return dummy.next;
    }
}
```

#### TypeScript
```typescript
function mergeTwoLists(list1: ListNode | null, list2: ListNode | null): ListNode | null {
    const vals: number[] = [];
    while (list1 !== null) {
        vals.push(list1.val);
        list1 = list1.next;
    }
    while (list2 !== null) {
        vals.push(list2.val);
        list2 = list2.next;
    }
    vals.sort((a, b) => a - b);
    const dummy = new ListNode(0);
    let curr = dummy;
    for (const v of vals) {
        curr.next = new ListNode(v);
        curr = curr.next;
    }
    return dummy.next;
}
```

#### Go
```go
package main

import "sort"

func mergeTwoLists(list1 *ListNode, list2 *ListNode) *ListNode {
    var vals []int
    for list1 != nil {
        vals = append(vals, list1.Val)
        list1 = list1.Next
    }
    for list2 != nil {
        vals = append(vals, list2.Val)
        list2 = list2.Next
    }
    sort.Ints(vals)
    dummy := &ListNode{}
    curr := dummy
    for _, v := range vals {
        curr.Next = &ListNode{Val: v}
        curr = curr.Next
    }
    return dummy.Next
}
```

#### Rust
```rust
impl Solution {
    pub fn merge_two_lists(
        mut list1: Option<Box<ListNode>>,
        mut list2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut vals = Vec::new();
        while let Some(node) = list1 {
            vals.push(node.val);
            list1 = node.next;
        }
        while let Some(node) = list2 {
            vals.push(node.val);
            list2 = node.next;
        }
        vals.sort_unstable();
        let mut head = None;
        let mut curr = &mut head;
        for v in vals {
            *curr = Some(Box::new(ListNode::new(v)));
            curr = &mut curr.as_mut().unwrap().next;
        }
        head
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is the dummy node allocated on the stack rather than the heap in C++?</summary>
Declaring `ListNode dummy(0);` allocates the 16 bytes directly on the CPU stack frame.
This avoids a call to the dynamic heap allocator (`new ListNode`), avoids a potential memory leak, and guarantees automatic deallocation upon function exit.
</details>

<details>
<summary>2. Why does attaching `current.next = (list1 != null) ? list1 : list2` operate in $O(1)$ time?</summary>
In a linked list, each node already holds a pointer to its successor.
Once one list is exhausted, the remaining list is already sorted.
A single pointer assignment links the entire remaining chain in $O(1)$ without needing to traverse the rest of the nodes.
</details>

<details>
<summary>3. Why is the recursive solution suboptimal for production systems with long lists?</summary>
Each recursive call allocates a new stack frame storing return address, arguments, and local variables.
For lists with $10^5$ nodes, recursion depth reaches $10^5$, exceeding default thread stack limits ($1\text{MB}$ to $8\text{MB}$) and triggering a fatal stack overflow crash.
</details>

<details>
<summary>4. How does Rust's ownership model handle moving nodes between lists?</summary>
In Rust, `Option<Box<ListNode>>` owns the node heap memory.
Using `.take()` extracts the node from the option, replacing it with `None`.
Rewiring `.next` transfers exclusive ownership without cloning data or violating borrow checker invariants.
</details>

<details>
<summary>5. How does this algorithm form the merge step of Merge Sort on linked lists?</summary>
Top-down Merge Sort on linked lists divides a list into two halves using the slow and fast pointer technique (Tortoise and Hare), recursively sorts both halves, and invokes `mergeTwoLists` to combine them in $O(N)$ time.
</details>

<details>
<summary>6. What is the cache behavior of linked list merging versus array merging?</summary>
Array merging benefits from contiguous memory streams with predictable sequential cache prefetches.
Linked list merging chases heap pointers; if nodes were allocated at disparate heap addresses, each pointer dereference risks an L1/L2/L3 cache miss.
</details>

<details>
<summary>7. What prevents cycles from forming during the merge?</summary>
The algorithm strictly advances forward monotonically (`list1 = list1.next` or `list2 = list2.next`).
No pointer is ever assigned to an ancestor or previously processed node, guaranteeing acyclicity.
</details>

<details>
<summary>8. How can this algorithm extend to $K$ sorted lists (LeetCode 23)?</summary>
We can merge $K$ sorted lists using:
1. Divide and conquer: pair up lists and merge in $\lceil\log_2 K\rceil$ rounds.
2. Min-Heap (priority queue): maintain the head of each of the $K$ lists, extracting the minimum in $O(\log K)$ time per node.
</details>

<details>
<summary>9. Why is `<` vs `<=` relevant when comparing `list1.val <= list2.val`?</summary>
Using `<=` ensures stability: elements from `list1` precede identical elements from `list2`.
While value ordering is preserved either way for primitive integers, stability matters when nodes carry auxiliary satellite payload data.
</details>

<details>
<summary>10. What are the key unit test edge cases for merging two sorted lists?</summary>
1. Both lists empty: `[], []` -> `[]`.
2. One list empty: `[], [0]` -> `[0]`.
3. Disjoint ranges: `[1, 2, 3], [4, 5, 6]` -> `[1, 2, 3, 4, 5, 6]`.
4. Identical elements: `[1, 1, 1], [1, 1, 1]` -> `[1, 1, 1, 1, 1, 1]`.
5. Interleaved single-element lists: `[1], [2]` -> `[1, 2]`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/merge-two-sorted-lists.cpp)
- [Python Implementation](../Python/merge-two-sorted-lists.py)
- [Java Implementation](../Java/merge-two-sorted-lists.java)
- [TypeScript Implementation](../TypeScript/merge-two-sorted-lists.ts)
- [Go Implementation](../Golang/merge-two-sorted-lists.go)
- [Rust Implementation](../Rust/merge-two-sorted-lists.rs)

---
id: leetcode-0019-remove-nth-node-from-end-of-list
title: "LeetCode 0019: Remove Nth Node From End of List"
tags:
  - dsa
  - leetcode
  - linked-list
  - two-pointers
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/remove-nth-node-from-end-of-list/"
---

# LeetCode 0019: Remove Nth Node From End of List

## 1. Problem Formalization and Constraints

Given the head of a linked list, remove the $n$-th node from the end of the list and return its head.

### Constraints
- The number of nodes in the list is $sz$.
- $1 \le sz \le 30$
- $0 \le \text{Node.val} \le 100$
- $1 \le n \le sz$

### Examples
- **Example 1**:
  - Input: `head = [1,2,3,4,5], n = 2`
  - Output: `[1,2,3,5]`
- **Example 2**:
  - Input: `head = [1], n = 1`
  - Output: `[]`
- **Example 3**:
  - Input: `head = [1,2], n = 1`
  - Output: `[1]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | One-Pass Two Pointers with Dummy Node | $O(N)$ | $O(1)$ | Fast pointer creates a gap of $n$ nodes; slow pointer lands directly on predecessor in a single pass. |
| **Tier 2 (Space-Optimized)** | Two-Pass Length Calculation | $O(N)$ | $O(1)$ | First pass counts length $L$; second pass traverses $L - n$ nodes to perform unlinking. |
| **Tier 3 (Time-Optimized Alternative)** | Stack-Based Predecessor Lookup | $O(N)$ | $O(N)$ | Pushes all node pointers onto a stack; pops $n$ elements to immediately access the target predecessor. |
| **Tier 4 (Brute Force)** | Array Buffer Rebuilding | $O(N)$ | $O(N)$ | Collects all nodes into an indexable array; removes $(L - n)$-th element and re-links pointers. |

---

## 3. Tier 1: Most Optimal Solution (One-Pass Two Pointers with Dummy Node)

### 3.1 Algorithmic Mechanics and Invariant Proof

To delete a node in a singly linked list without extra space, we must position a pointer at its immediate predecessor.
When deleting the head node ($n = sz$), there is no natural predecessor in the list.
We introduce a dummy sentinel node pointing to `head` (`dummy.next = head`), ensuring uniform handling for every node including the head.

1. Initialize two pointers `fast` and `slow` pointing to `dummy`.
2. Advance `fast` by $n$ steps forward.
The distance between `fast` and `slow` is now exactly $n$ nodes.
3. Advance both `fast` and `slow` synchronously one step at a time until `fast.next == nullptr`.
4. When `fast` reaches the final node of the list, `slow` is located precisely at the node preceding the $n$-th node from the end.
5. Unlink the target node: `slow.next = slow.next.next`.
6. Return `dummy.next`.

**Invariant Proof**:
Let the length of the list be $L$.
The $n$-th node from the end is at 0-indexed position $L - n$ from `head` (or $L - n + 1$ from `dummy`).
Its predecessor is at distance $L - n$ from `dummy`.
Because `fast` starts $n$ steps ahead of `slow`, when `fast` travels from position $n$ to the end at position $L$, it executes $(L - n)$ steps.
Since `slow` moves simultaneously, `slow` travels exactly $(L - n)$ steps from `dummy`, positioning it precisely at the predecessor node.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly one pass through the list containing $N$ nodes.
- **Space Complexity**: $O(1)$. Only requires two pointer references and one sentinel node.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode dummy(0, head);
        ListNode* fast = &dummy;
        ListNode* slow = &dummy;

        for (int i = 0; i < n; ++i) {
            fast = fast->next;
        }

        while (fast->next != nullptr) {
            fast = fast->next;
            slow = slow->next;
        }

        ListNode* to_delete = slow->next;
        slow->next = slow->next->next;
        delete to_delete;

        return dummy.next;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class ListNode:
    def __init__(self, val: int = 0, next: Optional['ListNode'] = None):
        self.val = val
        self.next = next

class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        dummy = ListNode(0, head)
        fast = dummy
        slow = dummy

        for _ in range(n):
            if fast.next:
                fast = fast.next

        while fast.next:
            fast = fast.next
            slow = slow.next

        slow.next = slow.next.next
        return dummy.next
```

#### Java 21
```java
class ListNode {
    int val;
    ListNode next;
    ListNode() {}
    ListNode(int val) { this.val = val; }
    ListNode(int val, ListNode next) { this.val = val; this.next = next; }
}

class Solution {
    public ListNode removeNthFromEnd(ListNode head, int n) {
        ListNode dummy = new ListNode(0, head);
        ListNode fast = dummy;
        ListNode slow = dummy;

        for (int i = 0; i < n; i++) {
            fast = fast.next;
        }

        while (fast.next != null) {
            fast = fast.next;
            slow = slow.next;
        }

        slow.next = slow.next.next;
        return dummy.next;
    }
}
```

#### TypeScript
```typescript
class ListNode {
    val: number;
    next: ListNode | null;
    constructor(val?: number, next?: ListNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.next = (next === undefined ? null : next);
    }
}

function removeNthFromEnd(head: ListNode | null, n: number): ListNode | null {
    const dummy = new ListNode(0, head);
    let fast: ListNode | null = dummy;
    let slow: ListNode | null = dummy;

    for (let i = 0; i < n; i++) {
        if (fast) fast = fast.next;
    }

    while (fast && fast.next) {
        fast = fast.next;
        slow = slow!.next;
    }

    if (slow && slow.next) {
        slow.next = slow.next.next;
    }

    return dummy.next;
}
```

#### Go
```go
package main

type ListNode struct {
    Val  int
    Next *ListNode
}

func removeNthFromEnd(head *ListNode, n int) *ListNode {
    dummy := &ListNode{Val: 0, Next: head}
    fast := dummy
    slow := dummy

    for i := 0; i < n; i++ {
        fast = fast.Next
    }

    for fast.Next != nil {
        fast = fast.Next
        slow = slow.Next
    }

    slow.Next = slow.Next.Next
    return dummy.Next
}
```

#### Rust
```rust
#[derive(PartialEq, Eq, Clone, Debug)]
pub struct ListNode {
    pub val: i32,
    pub next: Option<Box<ListNode>>,
}

impl Solution {
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let mut dummy = Some(Box::new(ListNode { val: 0, next: head }));
        let mut len = 0;
        {
            let mut curr = dummy.as_ref().unwrap().next.as_ref();
            while let Some(node) = curr {
                len += 1;
                curr = node.next.as_ref();
            }
        }

        let mut curr = dummy.as_mut();
        for _ in 0..(len - n) {
            curr = curr.unwrap().next.as_mut();
        }

        let next = curr.as_mut().unwrap().next.as_mut().unwrap().next.take();
        curr.unwrap().next = next;

        dummy.unwrap().next
    }
}
```

---

## 4. Tier 2: Space-Optimized Solution (Two-Pass Length Calculation)

### 4.1 Algorithmic Mechanics and Invariant Proof

1. Perform an initial traversal from `head` to compute the total length $L$ of the linked list.
2. The index of the node to remove from `head` is $L - n$.
3. In the second pass, create a dummy node pointing to `head` and advance a pointer by $L - n$ steps.
4. The pointer lands on the node immediately preceding the target node.
5. Skip the target node: `curr.next = curr.next.next`.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Two linear passes over the list ($L$ steps in pass 1, $L - n$ steps in pass 2).
- **Space Complexity**: $O(1)$. Uses only integer counters and pointer references.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        int length = 0;
        ListNode* curr = head;
        while (curr != nullptr) {
            ++length;
            curr = curr->next;
        }

        ListNode dummy(0, head);
        curr = &dummy;
        for (int i = 0; i < length - n; ++i) {
            curr = curr->next;
        }

        ListNode* to_delete = curr->next;
        curr->next = curr->next->next;
        delete to_delete;

        return dummy.next;
    }
};
```

#### Python 3.12
```python
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        length = 0
        curr = head
        while curr:
            length += 1
            curr = curr.next

        dummy = ListNode(0, head)
        curr = dummy
        for _ in range(length - n):
            curr = curr.next

        curr.next = curr.next.next
        return dummy.next
```

#### Java 21
```java
class Solution {
    public ListNode removeNthFromEnd(ListNode head, int n) {
        int length = 0;
        ListNode curr = head;
        while (curr != null) {
            length++;
            curr = curr.next;
        }

        ListNode dummy = new ListNode(0, head);
        curr = dummy;
        for (int i = 0; i < length - n; i++) {
            curr = curr.next;
        }

        curr.next = curr.next.next;
        return dummy.next;
    }
}
```

#### TypeScript
```typescript
function removeNthFromEnd(head: ListNode | null, n: number): ListNode | null {
    let length = 0;
    let curr = head;
    while (curr) {
        length++;
        curr = curr.next;
    }

    const dummy = new ListNode(0, head);
    curr = dummy;
    for (let i = 0; i < length - n; i++) {
        if (curr) curr = curr.next;
    }

    if (curr && curr.next) {
        curr.next = curr.next.next;
    }

    return dummy.next;
}
```

#### Go
```go
package main

func removeNthFromEnd(head *ListNode, n int) *ListNode {
    length := 0
    curr := head
    for curr != nil {
        length++
        curr = curr.Next
    }

    dummy := &ListNode{Val: 0, Next: head}
    curr = dummy
    for i := 0; i < length-n; i++ {
        curr = curr.Next
    }

    curr.Next = curr.Next.Next
    return dummy.Next
}
```

#### Rust
```rust
impl Solution {
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let mut dummy = Some(Box::new(ListNode { val: 0, next: head }));
        let mut len = 0;
        {
            let mut ptr = dummy.as_ref().unwrap().next.as_ref();
            while let Some(node) = ptr {
                len += 1;
                ptr = node.next.as_ref();
            }
        }

        let mut ptr = dummy.as_mut();
        for _ in 0..(len - n) {
            ptr = ptr.unwrap().next.as_mut();
        }

        let next = ptr.as_mut().unwrap().next.as_mut().unwrap().next.take();
        ptr.unwrap().next = next;

        dummy.unwrap().next
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Stack-Based Traversal)

### 5.1 Algorithmic Mechanics and Invariant Proof

A stack naturally reverses the order of elements (LIFO).
1. Traverse the linked list and push every node pointer onto a stack, starting with a dummy sentinel node.
2. Pop $n$ nodes from the stack.
3. The node now at the top of the stack is the predecessor of the node to remove.
4. Update `top.next = top.next.next`.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Single traversal to populate the stack, followed by $n$ pop operations.
- **Space Complexity**: $O(N)$. Auxiliary stack holds all $N + 1$ node references.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <stack>

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode dummy(0, head);
        std::stack<ListNode*> st;
        ListNode* curr = &dummy;

        while (curr != nullptr) {
            st.push(curr);
            curr = curr->next;
        }

        for (int i = 0; i < n; ++i) {
            st.pop();
        }

        ListNode* prev = st.top();
        ListNode* to_delete = prev->next;
        prev->next = prev->next->next;
        delete to_delete;

        return dummy.next;
    }
};
```

#### Python 3.12
```python
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        dummy = ListNode(0, head)
        stack: list[ListNode] = []
        curr: Optional[ListNode] = dummy

        while curr:
            stack.append(curr)
            curr = curr.next

        for _ in range(n):
            stack.pop()

        prev = stack[-1]
        prev.next = prev.next.next
        return dummy.next
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Deque;

class Solution {
    public ListNode removeNthFromEnd(ListNode head, int n) {
        ListNode dummy = new ListNode(0, head);
        Deque<ListNode> stack = new ArrayDeque<>();
        ListNode curr = dummy;

        while (curr != null) {
            stack.push(curr);
            curr = curr.next;
        }

        for (int i = 0; i < n; i++) {
            stack.pop();
        }

        ListNode prev = stack.peek();
        prev.next = prev.next.next;
        return dummy.next;
    }
}
```

#### TypeScript
```typescript
function removeNthFromEnd(head: ListNode | null, n: number): ListNode | null {
    const dummy = new ListNode(0, head);
    const stack: ListNode[] = [];
    let curr: ListNode | null = dummy;

    while (curr) {
        stack.push(curr);
        curr = curr.next;
    }

    for (let i = 0; i < n; i++) {
        stack.pop();
    }

    const prev = stack[stack.length - 1];
    if (prev && prev.next) {
        prev.next = prev.next.next;
    }

    return dummy.next;
}
```

#### Go
```go
package main

func removeNthFromEnd(head *ListNode, n int) *ListNode {
    dummy := &ListNode{Val: 0, Next: head}
    var stack []*ListNode
    curr := dummy

    for curr != nil {
        stack = append(stack, curr)
        curr = curr.Next
    }

    for i := 0; i < n; i++ {
        stack = stack[:len(stack)-1]
    }

    prev := stack[len(stack)-1]
    prev.Next = prev.Next.Next
    return dummy.Next
}
```

#### Rust
```rust
impl Solution {
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let mut vals = Vec::new();
        let mut curr = head.as_ref();
        while let Some(node) = curr {
            vals.push(node.val);
            curr = node.next.as_ref();
        }

        let target_idx = vals.len() - n as usize;
        vals.remove(target_idx);

        let mut dummy = Box::new(ListNode { val: 0, next: None });
        let mut tail = &mut dummy;
        for v in vals {
            tail.next = Some(Box::new(ListNode { val: v, next: None }));
            tail = tail.next.as_mut().unwrap();
        }

        dummy.next
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Array Buffer Rebuilding)

### 6.1 Algorithmic Mechanics and Invariant Proof

1. Collect all node values into an array buffer.
2. Remove the value at index $\text{len} - n$.
3. Construct a brand-new linked list from the updated values.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Traverses the list once, removes an element from the buffer in $O(N)$ time, and builds a new list in $O(N)$ time.
- **Space Complexity**: $O(N)$. Stores $N$ values in an auxiliary array and allocates new nodes.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        std::vector<int> vals;
        ListNode* curr = head;
        while (curr != nullptr) {
            vals.push_back(curr->val);
            curr = curr->next;
        }

        vals.erase(vals.end() - n);

        ListNode dummy(0);
        ListNode* tail = &dummy;
        for (int v : vals) {
            tail->next = new ListNode(v);
            tail = tail->next;
        }
        return dummy.next;
    }
};
```

#### Python 3.12
```python
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        vals: list[int] = []
        curr = head
        while curr:
            vals.append(curr.val)
            curr = curr.next

        vals.pop(len(vals) - n)

        dummy = ListNode(0)
        tail = dummy
        for v in vals:
            tail.next = ListNode(v)
            tail = tail.next

        return dummy.next
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

class Solution {
    public ListNode removeNthFromEnd(ListNode head, int n) {
        List<Integer> vals = new ArrayList<>();
        ListNode curr = head;
        while (curr != null) {
            vals.add(curr.val);
            curr = curr.next;
        }

        vals.remove(vals.size() - n);

        ListNode dummy = new ListNode(0);
        ListNode tail = dummy;
        for (int v : vals) {
            tail.next = new ListNode(v);
            tail = tail.next;
        }
        return dummy.next;
    }
}
```

#### TypeScript
```typescript
function removeNthFromEnd(head: ListNode | null, n: number): ListNode | null {
    const vals: number[] = [];
    let curr = head;
    while (curr) {
        vals.push(curr.val);
        curr = curr.next;
    }

    vals.splice(vals.length - n, 1);

    const dummy = new ListNode(0);
    let tail = dummy;
    for (const v of vals) {
        tail.next = new ListNode(v);
        tail = tail.next;
    }
    return dummy.next;
}
```

#### Go
```go
package main

func removeNthFromEnd(head *ListNode, n int) *ListNode {
    var vals []int
    curr := head
    for curr != nil {
        vals = append(vals, curr.Val)
        curr = curr.Next
    }

    idx := len(vals) - n
    vals = append(vals[:idx], vals[idx+1:]...)

    dummy := &ListNode{Val: 0}
    tail := dummy
    for _, v := range vals {
        tail.Next = &ListNode{Val: v}
        tail = tail.Next
    }
    return dummy.Next
}
```

#### Rust
```rust
impl Solution {
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let mut vals = Vec::new();
        let mut curr = head.as_ref();
        while let Some(node) = curr {
            vals.push(node.val);
            curr = node.next.as_ref();
        }

        let idx = vals.len() - n as usize;
        vals.remove(idx);

        let mut dummy = Box::new(ListNode { val: 0, next: None });
        let mut tail = &mut dummy;
        for v in vals {
            tail.next = Some(Box::new(ListNode { val: v, next: None }));
            tail = tail.next.as_mut().unwrap();
        }

        dummy.next
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is a dummy sentinel node essential for singly linked list deletions?</summary>
Without a sentinel node, deleting the first node requires a special condition (`if (n == length) return head->next;`).
A dummy node guarantees that every node in the original list has a non-null predecessor, eliminating conditional branches.
</details>

<details>
<summary>2. What occurs when $n = sz$ (the head node is removed)?</summary>
The fast pointer advances $n$ steps from dummy and reaches the final node.
The slow pointer remains at `dummy`.
Executing `slow.next = slow.next.next` unlinks `head`, returning the second node as the new head.
</details>

<details>
<summary>3. What happens when $n = 1$ (the tail node is removed)?</summary>
The fast pointer reaches the node before the end, and the slow pointer advances until it is right before the tail.
`slow.next = slow.next.next` assigns `nullptr` to `slow.next`, cleanly detaching the tail node.
</details>

<details>
<summary>4. Why is manual memory deallocation (`delete` or `free`) critical in C++ but not in Java or Go?</summary>
C++ does not have garbage collection.
Simply unlinking a node leaves its heap memory allocated, causing a memory leak unless explicitly freed.
Java and Go automatically reclaim unreferenced objects via tracing garbage collectors.
</details>

<details>
<summary>5. How does Rust's ownership system handle removing a node from a Box-based linked list?</summary>
Rust uses `Option<Box<ListNode>>`, where each node exclusively owns the next node.
To excise a node, we use `Option::take()` on `node.next`, move the grandchild pointer to the predecessor's `next`, and let the excised `Box` drop automatically.
</details>

<details>
<summary>6. Can this problem be solved recursively in a single post-order traversal?</summary>
Yes. In a recursive function, traverse to the end of the list and return an index counter on the unwinding call stack.
When the unwinding counter equals $n + 1$, relink the current node's `next` pointer to `next.next`.
</details>

<details>
<summary>7. What is the space overhead of the recursive post-order method?</summary>
Because recursion unwinds from the tail, the maximum call stack depth is $O(N)$, which uses $O(N)$ auxiliary stack frames.
</details>

<details>
<summary>8. How do hardware cache misses affect linked list traversal compared to array indexing?</summary>
Linked list nodes reside at arbitrary heap memory locations, causing CPU cache misses on nearly every pointer dereference.
In contrast, array buffers are laid out in contiguous memory and benefit from cache line spatial prefetching.
</details>

<details>
<summary>9. What is the difference between advancing `fast` by $n$ steps versus advancing `fast` by $n + 1$ steps?</summary>
If advanced by $n$ steps from `dummy`, the traversal terminates when `fast.next == nullptr`.
If advanced by $n + 1$ steps, the traversal terminates when `fast == nullptr`.
Both position `slow` at the exact same predecessor node.
</details>

<details>
<summary>10. What edge case occurs when the list has only 1 node and $n = 1$?</summary>
`dummy.next` points to the single node.
`fast` advances 1 step to the single node.
The loop `while (fast.next != null)` does not execute.
`slow.next` is set to `null`, and `dummy.next` returns `null` (an empty list).
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/remove-nth-node-from-end-of-list.cpp)
- [Python Implementation](../Python/remove-nth-node-from-end-of-list.py)
- [Java Implementation](../Java/remove-nth-node-from-end-of-list.java)
- [TypeScript Implementation](../TypeScript/remove-nth-node-from-end-of-list.ts)
- [Go Implementation](../Golang/remove-nth-node-from-end-of-list.go)
- [Rust Implementation](../Rust/remove-nth-node-from-end-of-list.rs)

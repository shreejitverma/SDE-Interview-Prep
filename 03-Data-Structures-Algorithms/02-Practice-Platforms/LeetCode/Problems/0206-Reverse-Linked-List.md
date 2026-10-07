---
id: leetcode-0206-reverse-linked-list
title: "LeetCode 0206: Reverse Linked List"
tags:
  - dsa
  - leetcode
  - linked-list
  - recursion
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/reverse-linked-list/"
---

# LeetCode 0206: Reverse Linked List

## 1. Problem Formalization and Constraints

Given the head of a singly linked list, reverse the list, and return the reversed list.

### Constraints
- The number of nodes in the list is the range $[0, 5000]$.
- $-5000 \le \text{Node.val} \le 5000$

### Follow-up
A linked list can be reversed either iteratively or recursively.
Could you implement both?

### Examples
- **Example 1**:
  - Input: `head = [1,2,3,4,5]`
  - Output: `[5,4,3,2,1]`
- **Example 2**:
  - Input: `head = [1,2]`
  - Output: `[2,1]`
- **Example 3**:
  - Input: `head = []`
  - Output: `[]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Three-Pointer Iterative Pointer Reversal | $O(N)$ | $O(1)$ | Maintains `prev`, `curr`, and `nextTemp` pointers; strictly optimal memory efficiency. |
| **Tier 2 (Space-Optimized Alternative)** | Tail-Call Recursive In-Place Accumulator | $O(N)$ | $O(1)$ (with TCO) | Inverts pointers recursively passing `(curr, prev)` accumulators. |
| **Tier 3 (Time-Optimized Alternative)** | Structural Post-Order Recursion | $O(N)$ | $O(N)$ stack | Traverses to the tail first, re-attaching `head.next.next = head` on stack unwind. |
| **Tier 4 (Brute Force)** | Auxiliary LIFO Stack Node Buffering | $O(N)$ | $O(N)$ | Pushes all nodes into an auxiliary stack and rebuilds links sequentially. |

---

## 3. Tier 1: Most Optimal Solution (Three-Pointer Iterative Reversal)

### 3.1 Algorithmic Mechanics and Invariant Proof

We initialize two pointers: `prev = null` and `curr = head`.
During each iteration while `curr != null`:
1. Save the forward successor: `nextTemp = curr.next`.
2. Invert the current node's pointer: `curr.next = prev`.
3. Advance the accumulator: `prev = curr`.
4. Advance the scan pointer: `curr = nextTemp`.
When `curr` becomes null, `prev` points to the new head of the reversed list.

**Inductive Invariant**:
At the start of each loop iteration:
- The sublist preceding `curr` has been reversed and has head pointer `prev`.
- The sublist starting at `curr` remains in its original orientation.
- The link between `curr` and its predecessor is uncoupled without losing the remaining chain via `nextTemp`.
Upon exhaustion of the input list, `prev` references the completely inverted sequence.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly $N$ iterations visiting each node once.
- **Space Complexity**: $O(1)$. Auxiliary space strictly bounded to three scalar pointer registers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* prev = nullptr;
        ListNode* curr = head;
        while (curr) {
            ListNode* nextTemp = curr->next;
            curr->next = prev;
            prev = curr;
            curr = nextTemp;
        }
        return prev;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        prev = None
        curr = head
        while curr:
            next_temp = curr.next
            curr.next = prev
            prev = curr
            curr = next_temp
        return prev
```

#### Java 21
```java
class Solution {
    public ListNode reverseList(ListNode head) {
        ListNode prev = null;
        ListNode curr = head;
        while (curr != null) {
            ListNode nextTemp = curr.next;
            curr.next = prev;
            prev = curr;
            curr = nextTemp;
        }
        return prev;
    }
}
```

#### TypeScript
```typescript
function reverseList(head: ListNode | null): ListNode | null {
    let prev: ListNode | null = null;
    let curr = head;
    while (curr !== null) {
        const nextTemp = curr.next;
        curr.next = prev;
        prev = curr;
        curr = nextTemp;
    }
    return prev;
}
```

#### Go
```go
package main

func reverseList(head *ListNode) *ListNode {
    var prev *ListNode
    curr := head
    for curr != nil {
        nextTemp := curr.Next
        curr.Next = prev
        prev = curr
        curr = nextTemp
    }
    return prev
}
```

#### Rust
```rust
impl Solution {
    pub fn reverse_list(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut prev = None;
        while let Some(mut node) = head {
            head = node.next;
            node.next = prev;
            prev = Some(node);
        }
        prev
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Tail-Call Recursive Accumulator)

### 4.1 Algorithmic Mechanics

We formulate reversal as a tail-recursive function `reverse(curr, prev)`.
If `curr == null`, return `prev`.
Otherwise, store `nextTemp = curr.next`, set `curr.next = prev`, and tail-call `reverse(nextTemp, curr)`.
Compilers supporting tail-call optimization (TCO) convert this directly into an iterative loop in machine code.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear steps.
- **Space Complexity**: $O(1)$ under tail-call optimization ($O(N)$ stack frames without TCO).

### 4.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
    ListNode* reverseHelper(ListNode* curr, ListNode* prev) {
        if (!curr) return prev;
        ListNode* nextTemp = curr->next;
        curr->next = prev;
        return reverseHelper(nextTemp, curr);
    }
public:
    ListNode* reverseList(ListNode* head) {
        return reverseHelper(head, nullptr);
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        def reverse_helper(curr, prev):
            if not curr:
                return prev
            next_temp = curr.next
            curr.next = prev
            return reverse_helper(next_temp, curr)

        return reverse_helper(head, None)
```

#### Java 21
```java
class Solution {
    private ListNode reverseHelper(ListNode curr, ListNode prev) {
        if (curr == null) return prev;
        ListNode nextTemp = curr.next;
        curr.next = prev;
        return reverseHelper(nextTemp, curr);
    }

    public ListNode reverseList(ListNode head) {
        return reverseHelper(head, null);
    }
}
```

#### TypeScript
```typescript
function reverseList(head: ListNode | null): ListNode | null {
    function reverseHelper(curr: ListNode | null, prev: ListNode | null): ListNode | null {
        if (curr === null) return prev;
        const nextTemp = curr.next;
        curr.next = prev;
        return reverseHelper(nextTemp, curr);
    }
    return reverseHelper(head, null);
}
```

#### Go
```go
package main

func reverseHelper(curr, prev *ListNode) *ListNode {
    if curr == nil {
        return prev
    }
    nextTemp := curr.Next
    curr.Next = prev
    return reverseHelper(nextTemp, curr)
}

func reverseList(head *ListNode) *ListNode {
    return reverseHelper(head, nil)
}
```

#### Rust
```rust
impl Solution {
    fn reverse_helper(
        head: Option<Box<ListNode>>,
        prev: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        match head {
            None => prev,
            Some(mut node) => {
                let next = node.next.take();
                node.next = prev;
                Self::reverse_helper(next, Some(node))
            }
        }
    }

    pub fn reverse_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        Self::reverse_helper(head, None)
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Classical Post-Order Recursion)

### 5.1 Algorithmic Mechanics

We drill down to the final tail node using structural recursion:
1. Base Case: If `head == null || head.next == null`, return `head` (this node is the new root).
2. Recursive Step: `newHead = reverseList(head.next)`.
3. Inversion Rewire:
   The node that originally succeeded `head` (`head.next`) must now point backward to `head`:
   $$\text{head.next.next} = \text{head}$$
   Sever the forward pointer to eliminate cycles:
   $$\text{head.next} = \text{null}$$
4. Propagate `newHead` up the call stack.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ operations.
- **Space Complexity**: $O(N)$ stack frames on the call stack.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        if (!head || !head->next) {
            return head;
        }
        ListNode* newHead = reverseList(head->next);
        head->next->next = head;
        head->next = nullptr;
        return newHead;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        if not head or not head.next:
            return head
        new_head = self.reverseList(head.next)
        head.next.next = head
        head.next = None
        return new_head
```

#### Java 21
```java
class Solution {
    public ListNode reverseList(ListNode head) {
        if (head == null || head.next == null) {
            return head;
        }
        ListNode newHead = reverseList(head.next);
        head.next.next = head;
        head.next = null;
        return newHead;
    }
}
```

#### TypeScript
```typescript
function reverseList(head: ListNode | null): ListNode | null {
    if (!head || !head.next) {
        return head;
    }
    const newHead = reverseList(head.next);
    head.next.next = head;
    head.next = null;
    return newHead;
}
```

#### Go
```go
package main

func reverseList(head *ListNode) *ListNode {
    if head == nil || head.Next == nil {
        return head
    }
    newHead := reverseList(head.Next)
    head.Next.Next = head
    head.Next = nil
    return newHead
}
```

#### Rust
```rust
impl Solution {
    pub fn reverse_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut prev = None;
        let mut curr = head;
        while let Some(mut node) = curr {
            curr = node.next.take();
            node.next = prev;
            prev = Some(node);
        }
        prev
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Auxiliary LIFO Stack Node Buffering)

### 6.1 Algorithmic Mechanics

We push all node pointers into a vector or explicit LIFO stack.
We then pop elements sequentially, wiring each popped node's `.next` pointer to the subsequent popped element.
The final node's `.next` is set to null.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$ two-pass traversal.
- **Space Complexity**: $O(N)$ auxiliary stack storage.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        if (!head) return nullptr;
        std::vector<ListNode*> stack;
        ListNode* curr = head;
        while (curr) {
            stack.push_back(curr);
            curr = curr->next;
        }
        ListNode* newHead = stack.back();
        for (int i = static_cast<int>(stack.size()) - 1; i > 0; --i) {
            stack[i]->next = stack[i - 1];
        }
        stack[0]->next = nullptr;
        return newHead;
    }
};
```

#### Python 3.12
```python
from typing import Optional

class Solution:
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        if not head:
            return None
        stack = []
        curr = head
        while curr:
            stack.append(curr)
            curr = curr.next
        new_head = stack[-1]
        for i in range(len(stack) - 1, 0, -1):
            stack[i].next = stack[i - 1]
        stack[0].next = None
        return new_head
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Deque;

class Solution {
    public ListNode reverseList(ListNode head) {
        if (head == null) return null;
        Deque<ListNode> stack = new ArrayDeque<>();
        ListNode curr = head;
        while (curr != null) {
            stack.push(curr);
            curr = curr.next;
        }
        ListNode newHead = stack.pop();
        curr = newHead;
        while (!stack.isEmpty()) {
            curr.next = stack.pop();
            curr = curr.next;
        }
        curr.next = null;
        return newHead;
    }
}
```

#### TypeScript
```typescript
function reverseList(head: ListNode | null): ListNode | null {
    if (!head) return null;
    const stack: ListNode[] = [];
    let curr: ListNode | null = head;
    while (curr) {
        stack.push(curr);
        curr = curr.next;
    }
    const newHead = stack[stack.length - 1];
    for (let i = stack.length - 1; i > 0; i--) {
        stack[i].next = stack[i - 1];
    }
    stack[0].next = null;
    return newHead;
}
```

#### Go
```go
package main

func reverseList(head *ListNode) *ListNode {
    if head == nil {
        return nil
    }
    var stack []*ListNode
    curr := head
    for curr != nil {
        stack = append(stack, curr)
        curr = curr.Next
    }
    newHead := stack[len(stack)-1]
    for i := len(stack) - 1; i > 0; i-- {
        stack[i].Next = stack[i-1]
    }
    stack[0].Next = nil
    return newHead
}
```

#### Rust
```rust
impl Solution {
    pub fn reverse_list(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut stack = Vec::new();
        while let Some(mut node) = head {
            head = node.next.take();
            stack.push(node);
        }
        let mut prev = None;
        while let Some(mut node) = stack.pop() {
            node.next = prev;
            prev = Some(node);
        }
        let mut reversed = None;
        while let Some(mut node) = prev {
            prev = node.next.take();
            node.next = reversed;
            reversed = Some(node);
        }
        reversed
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why must `head.next = nullptr` be explicitly set in the recursive approach?</summary>
In Tier 3, `head.next.next = head` creates a two-node cycle between `head` and its former successor.
If `head.next = nullptr` is omitted, the original head node (which becomes the new tail) retains a cycle back to its predecessor, producing an infinite loop during subsequent list traversals.
</details>

<details>
<summary>2. Why does the iterative approach guarantee zero memory allocations in compiled machine code?</summary>
The iterative method manipulates only existing memory addresses (`prev`, `curr`, `nextTemp`).
No calls to `malloc`, `new`, or garbage-collected allocation pools are made.
The compiled machine code executes entirely within CPU general-purpose registers (such as `rax`, `rcx`, `rdx`).
</details>

<details>
<summary>3. What is the impact of compiler Tail-Call Optimization (TCO) on Tier 2?</summary>
In languages with guaranteed TCO (like Scheme or certain Clang/GCC optimizations with `-O2`), the recursive call in `reverseHelper` is replaced by a `jmp` instruction.
The stack frame is reused, eliminating call stack growth and reducing space complexity from $O(N)$ to $O(1)$.
</details>

<details>
<summary>4. How does Rust's move semantics prevent data races during link reversal?</summary>
In Rust, each node is encapsulated in `Option<Box<ListNode>>`.
Calling `node.next.take()` sets the field to `None` while transferring ownership of the inner `Box` to the local variable.
This ensures that at no instant do two pointers point simultaneously to the same node in a way that violates Rust's aliasing XOR mutability invariant.
</details>

<details>
<summary>5. How does this algorithm form the basis for Reverse Nodes in k-Group (LeetCode 25)?</summary>
Reversing subsegments of length $K$ uses this identical three-pointer iterative inversion.
The boundary pointers are adjusted to reconnect the head and tail of each $K$-length segment with the surrounding list.
</details>

<details>
<summary>6. How does linked list reversal compare to array reversal in terms of cache performance?</summary>
Reversing an array (`std::reverse`) swaps elements symmetrically from both ends with sequential cache line access.
Reversing a linked list follows a forward pointer chain, potentially incurring a cache miss on every node access if nodes are non-contiguously allocated on the heap.
</details>

<details>
<summary>7. What happens if the input linked list contains a cycle?</summary>
If the input list contains a cycle, the iterative loop never terminates and will execute indefinitely until memory or time quotas expire.
Floyd's Cycle-Finding Algorithm (Tortoise and Hare) should precede reversal if input acyclicity is unverified.
</details>

<details>
<summary>8. How can we reverse a doubly linked list using this pattern?</summary>
In a doubly linked list, each node has both `.next` and `.prev` pointers.
For each node, we swap `.next` and `.prev` using a temporary variable, advancing `curr = curr.prev` (since the original `.next` is now stored in `.prev`).
</details>

<details>
<summary>9. Why is `ListNode* nextTemp = curr->next;` needed before `curr->next = prev;`?</summary>
Overwriting `curr->next = prev` severs the reference to the remainder of the linked list.
Saving `nextTemp` prior to overwriting preserves the reference to the rest of the unvisited chain.
</details>

<details>
<summary>10. What are the key unit test edge cases for linked list reversal?</summary>
1. Empty list: `head = null`, returning `null`.
2. Single-node list: `[1]`, returning `[1]`.
3. Two-node list: `[1, 2]`, returning `[2, 1]`.
4. Large list ($N = 5000$).
5. Identical value nodes: `[7, 7, 7, 7]`.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/reverse-linked-list.cpp)
- [Python Implementation](../Python/reverse-linked-list.py)
- [Java Implementation](../Java/reverse-linked-list.java)
- [TypeScript Implementation](../TypeScript/reverse-linked-list.ts)
- [Go Implementation](../Golang/reverse-linked-list.go)
- [Rust Implementation](../Rust/reverse-linked-list.rs)

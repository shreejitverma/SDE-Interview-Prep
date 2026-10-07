---
id: leetcode-0143-reorder-list
title: "LeetCode 0143: Reorder List"
tags:
  - dsa
  - leetcode
  - linked-list
  - two-pointers
  - recursion
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/reorder-list/"
---

# LeetCode 0143: Reorder List

## 1. Problem Formalization and Constraints

You are given the head of a singly linked-list.
The list can be represented as:
$$L_0 \to L_1 \to \dots \to L_{n-1} \to L_n$$
Reorder the list to be on the following form:
$$L_0 \to L_n \to L_1 \to L_{n-1} \to L_2 \to L_{n-2} \to \dots$$
You may not modify the values in the list's nodes.
Only node pointers themselves may be changed.

### Constraints
- The number of nodes in the list is in the range $[1, 5 \times 10^4]$.
- $1 \le \text{Node.val} \le 1000$

### Examples
- **Example 1**:
  - Input: `head = [1,2,3,4]`
  - Output: `[1,4,2,3]`
- **Example 2**:
  - Input: `head = [1,2,3,4,5]`
  - Output: `[1,5,2,4,3]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Middle Split + In-Place Reverse + Merge | $O(N)$ | $O(1)$ | Finds middle via fast/slow pointers, reverses the second half in-place, and weaves the two chains together. |
| **Tier 2 (Deque / Array)** | Random Access Vector Indexing | $O(N)$ | $O(N)$ | Collects node pointers into a vector or double-ended queue to re-wire nodes alternately from both ends. |
| **Tier 3 (Recursive)** | Recursive Call-Stack Unwinding | $O(N)$ | $O(N)$ | Uses the recursion stack to visit nodes from the tail backwards while a global pointer advances from the head. |
| **Tier 4 (Brute Force)** | Repeated Tail Extraction and Insertion | $O(N^2)$ | $O(1)$ | Repeatedly traverses to the tail, disconnects it, and splices it after the current front node. |

---

## 3. Tier 1: Most Optimal Solution (Middle Split, In-Place Reversal, and Interleaving)

### 3.1 Algorithmic Mechanics and Invariant Proof

The transformation requires matching the $i$-th element from the front with the $i$-th element from the back.
Because singly linked lists cannot traverse backward, the algorithm breaks the process into three linear steps:

1. **Find Middle**:
   Use a slow pointer and a fast pointer initialized at `head`.
   Advance `slow` by 1 step and `fast` by 2 steps until `fast.next` or `fast.next.next` is null.
   Upon termination, `slow` points to the end of the first half (index $\lfloor (N-1)/2 \rfloor$).

2. **Reverse Second Half**:
   Decouple the second half by setting `second = slow.next` and `slow.next = null`.
   Reverse the list starting at `second` in-place using standard three-pointer iterative reversal (`prev`, `curr`, `next`).

3. **Interleave Halves**:
   Initialize `first = head` and `second = reversed_head`.
   Iterate while `second != null`:
   - Save successors: `tmp1 = first.next`, `tmp2 = second.next`.
   - Splice: `first.next = second`, `second.next = tmp1`.
   - Advance: `first = tmp1`, `second = tmp2`.

**Invariant Proof**:
Let the original list have length $N$.
The split step divides the list into:
- First half: $L_0 \to L_1 \to \dots \to L_{\lceil N/2 \rceil - 1}$ of length $\lceil N/2 \rceil$.
- Second half: $L_{\lceil N/2 \rceil} \to \dots \to L_{N-1}$ of length $\lfloor N/2 \rfloor$.
Reversing the second half yields $L_{N-1} \to L_{N-2} \to \dots \to L_{\lceil N/2 \rceil}$.
At each step of interleaving, the $k$-th node of the first half is connected to the $k$-th node of the reversed second half, which is in turn connected to the $(k+1)$-th node of the first half.
Since $\lceil N/2 \rceil \ge \lfloor N/2 \rfloor$, the second half exhausts at or before the first half.
Every original node appears exactly once, and pointer directions form the target alternating topology.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$, where $N$ is the number of nodes in the list.
  - Finding the middle takes $N/2$ steps.
  - Reversing the second half takes $N/2$ steps.
  - Interleaving takes $N/2$ steps.
  - Total time: $O(N/2 + N/2 + N/2) = O(N)$.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space. Only a constant number of pointer variables are allocated.

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
    void reorderList(ListNode* head) {
        if (!head || !head->next) {
            return;
        }

        ListNode* slow = head;
        ListNode* fast = head;
        while (fast->next && fast->next->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        ListNode* second = reverse(slow->next);
        slow->next = nullptr;

        ListNode* first = head;
        while (second) {
            ListNode* tmp1 = first->next;
            ListNode* tmp2 = second->next;

            first->next = second;
            second->next = tmp1;

            first = tmp1;
            second = tmp2;
        }
    }

private:
    ListNode* reverse(ListNode* head) {
        ListNode* prev = nullptr;
        ListNode* curr = head;
        while (curr) {
            ListNode* next_node = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next_node;
        }
        return prev;
    }
};
```

#### Python 3
```python
from typing import Optional

class ListNode:
    def __init__(self, val: int = 0, next: Optional['ListNode'] = None):
        self.val = val
        self.next = next

class Solution:
    def reorderList(self, head: Optional[ListNode]) -> None:
        if not head or not head.next:
            return

        slow, fast = head, head
        while fast.next and fast.next.next:
            slow = slow.next
            fast = fast.next.next

        second = self._reverse(slow.next)
        slow.next = None

        first = head
        while second:
            tmp1, tmp2 = first.next, second.next
            first.next = second
            second.next = tmp1
            first, second = tmp1, tmp2

    def _reverse(self, head: Optional[ListNode]) -> Optional[ListNode]:
        prev = None
        curr = head
        while curr:
            next_node = curr.next
            curr.next = prev
            prev = curr
            curr = next_node
        return prev
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

public class Solution {
    public void reorderList(ListNode head) {
        if (head == null || head.next == null) {
            return;
        }

        ListNode slow = head;
        ListNode fast = head;
        while (fast.next != null && fast.next.next != null) {
            slow = slow.next;
            fast = fast.next.next;
        }

        ListNode second = reverseList(slow.next);
        slow.next = null;

        ListNode first = head;
        while (second != null) {
            ListNode tmp1 = first.next;
            ListNode tmp2 = second.next;

            first.next = second;
            second.next = tmp1;

            first = tmp1;
            second = tmp2;
        }
    }

    private ListNode reverseList(ListNode head) {
        ListNode prev = null;
        ListNode curr = head;
        while (curr != null) {
            ListNode next = curr.next;
            curr.next = prev;
            prev = curr;
            curr = next;
        }
        return prev;
    }
}
```

#### TypeScript 5
```typescript
class ListNode {
    val: number;
    next: ListNode | null;
    constructor(val?: number, next?: ListNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.next = (next === undefined ? null : next);
    }
}

function reorderList(head: ListNode | null): void {
    if (!head || !head.next) {
        return;
    }

    let slow: ListNode = head;
    let fast: ListNode | null = head;
    while (fast && fast.next && fast.next.next) {
        slow = slow.next!;
        fast = fast.next.next;
    }

    let second: ListNode | null = reverseList(slow.next);
    slow.next = null;

    let first: ListNode | null = head;
    while (second) {
        const tmp1: ListNode | null = first!.next;
        const tmp2: ListNode | null = second.next;

        first!.next = second;
        second.next = tmp1;

        first = tmp1;
        second = tmp2;
    }
}

function reverseList(head: ListNode | null): ListNode | null {
    let prev: ListNode | null = null;
    let curr: ListNode | null = head;
    while (curr) {
        const next: ListNode | null = curr.next;
        curr.next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}
```

#### Go 1.22
```go
package main

type ListNode struct {
	Val  int
	Next *ListNode
}

func reorderList(head *ListNode) {
	if head == nil || head.Next == nil {
		return
	}

	slow := head
	fast := head
	for fast.Next != nil && fast.Next.Next != nil {
		slow = slow.Next
		fast = fast.Next.Next
	}

	second := reverseList(slow.Next)
	slow.Next = nil

	first := head
	for second != nil {
		tmp1 := first.Next
		tmp2 := second.Next

		first.Next = second
		second.Next = tmp1

		first = tmp1
		second = tmp2
	}
}

func reverseList(head *ListNode) *ListNode {
	var prev *ListNode
	curr := head
	for curr != nil {
		next := curr.Next
		curr.Next = prev
		prev = curr
		curr = next
	}
	return prev
}
```

#### Rust 1.75
```rust
use std::collections::VecDeque;

#[derive(PartialEq, Eq, Clone, Debug)]
pub struct ListNode {
    pub val: i32,
    pub next: Option<Box<ListNode>>,
}

impl ListNode {
    #[inline]
    pub fn new(val: i32) -> Self {
        ListNode { next: None, val }
    }
}

pub struct Solution;

impl Solution {
    pub fn reorder_list(head: &mut Option<Box<ListNode>>) {
        if head.is_none() {
            return;
        }

        let mut deque = VecDeque::new();
        let mut curr = head.take();
        while let Some(mut node) = curr {
            curr = node.next.take();
            deque.push_back(node);
        }

        let mut dummy = ListNode::new(0);
        let mut tail = &mut dummy;
        let mut take_front = true;

        while !deque.is_empty() {
            let next_node = if take_front {
                deque.pop_front().unwrap()
            } else {
                deque.pop_back().unwrap()
            };
            take_front = !take_front;

            tail.next = Some(next_node);
            tail = tail.next.as_mut().unwrap();
        }

        *head = dummy.next;
    }
}
```

---

## 4. Tier 2: Double-Ended Queue / Node Vector Indexing

### 4.1 Implementation Mechanism
Iterate through the linked list and push every node pointer into a dynamic array `vector<ListNode*> nodes`.
Use two integer pointers `left = 0` and `right = nodes.size() - 1` to weave nodes sequentially.
Connect `nodes[left]->next = nodes[right]`, then `nodes[right]->next = nodes[left + 1]`.
Finally, set `nodes[left]->next = nullptr` to terminate the list properly.

```cpp
class SolutionArray {
public:
    void reorderList(ListNode* head) {
        if (!head || !head->next) return;

        std::vector<ListNode*> nodes;
        ListNode* curr = head;
        while (curr) {
            nodes.push_back(curr);
            curr = curr->next;
        }

        int left = 0, right = nodes.size() - 1;
        while (left < right) {
            nodes[left]->next = nodes[right];
            left++;
            if (left >= right) break;
            nodes[right]->next = nodes[left];
            right--;
        }
        nodes[left]->next = nullptr;
    }
};
```

### 4.2 Trade-offs
- Implementation is brief and intuitive.
- Allocates $O(N)$ extra memory for pointers, consuming 400 KB of heap memory on $5 \times 10^4$ nodes.

---

## 5. Tier 3: Recursive Call-Stack Unwinding

### 5.1 Algorithmic Structure
Recursively advance to the end of the linked list.
Upon unwinding, the recursion stack returns nodes in reverse order from tail to head.
Maintain an external pointer `front = head` that advances forward, splicing `front` with `tail` until the two pointers meet in the middle.

```python
class SolutionRecursive:
    def reorderList(self, head: Optional[ListNode]) -> None:
        if not head or not head.next:
            return

        front = head
        stop = False

        def helper(curr: Optional[ListNode]) -> None:
            nonlocal front, stop
            if not curr:
                return

            helper(curr.next)

            if stop:
                return

            if front == curr or front.next == curr:
                curr.next = None
                stop = True
                return

            tmp = front.next
            front.next = curr
            curr.next = tmp
            front = tmp

        helper(head)
```

### 5.2 Trade-offs
- Demonstrates deep structural understanding of call-stack recursion.
- Consumes $O(N)$ stack frames; risks call stack overflow for lists with $N = 5 \times 10^4$.

---

## 6. Tier 4: Brute Force Baseline (Repeated Tail Traversal)

### 6.1 Mechanical Description
Maintain an insertion cursor starting at `head`.
In each round, scan to the penultimate node to detach the tail node.
Insert the detached tail node immediately after the cursor.
Advance the cursor past the newly inserted node to repeat for the remainder of the list.

### 6.2 Complexity
- **Time Complexity**: $O(N^2)$, because locating and detaching the tail takes $O(N)$ operations across $N/2$ iterations.
- **Space Complexity**: $O(1)$ auxiliary space.
- **Verdict**: Times out on LeetCode for $N = 5 \times 10^4$ ($1.25 \times 10^9$ pointer hops).

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Non-Contiguous Pointer Chasing**: Nodes in a singly linked list are typically scattered across non-contiguous memory segments, resulting in cache misses on each `node->next` dereference.
2. **In-Place Pointer Mutation**: The Tier 1 algorithm does not allocate or deallocate nodes. It purely adjusts pointer targets, minimizing memory allocation churn and retaining memory subsystem cache lines.
3. **Rust Ownership Dynamics**: In Rust, moving ownership out of an `Option<Box<ListNode>>` requires either `Option::take()` or array buffering because multiple mutable references cannot coexist safely without unsafe pointer arithmetic.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Empty List | `head = null` | Returns immediately | Checked by early exit guard `if (!head) return;` |
| Single Node | `head = [1]` | Remains unchanged: `[1]` | Guard `if (!head->next) return;` terminates cleanly. |
| Two Nodes | `head = [1, 2]` | Remains unchanged: `[1, 2]` | Fast pointer terminates immediately; reverse and interleave are no-ops. |
| Odd Node Count | `[1, 2, 3, 4, 5]` | Middle stays at end: `[1, 5, 2, 4, 3]` | Slow terminates at node 3, splitting into 3 and 2 nodes. |
| Even Node Count | `[1, 2, 3, 4]` | Perfect pairing: `[1, 4, 2, 3]` | Splits into two halves of 2 nodes each. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why is `fast.next && fast.next.next` used rather than `fast && fast.next`?
Using `fast.next && fast.next.next` stops the `slow` pointer at the first middle node in even-length lists, guaranteeing that the first half is equal to or longer than the second half.

### 2. Can the values of the nodes be swapped instead of rewiring pointers?
The problem explicitly prohibits modifying node values; only pointer topology mutations are permitted.

### 3. What happens if we do not set `slow.next = null` after the split?
Failing to terminate the first half creates an infinite cycle during the interleaving phase.

### 4. Why does reversing the second half take $O(N)$ time?
Each of the $N/2$ nodes in the second half is visited and its pointer reversed exactly once, which takes linear time.

### 5. Why is Tier 1 preferred over Tier 2 in production?
Tier 1 requires zero auxiliary heap memory ($O(1)$ space), whereas Tier 2 allocates an array of pointers of size $N$, creating memory allocation overhead and pressure on garbage collectors.

### 6. Can this problem be solved using a Doubly Linked List?
Yes, in a doubly linked list, two pointers starting from `head` and `tail` can converge directly in $O(N)$ time and $O(1)$ auxiliary space without needing list reversal.

### 7. How does Rust's borrow checker impact in-place linked list manipulations?
Safe Rust enforces exclusive mutability (`&mut`), preventing simultaneous mutable borrows of multiple sub-lists. The `VecDeque` solution safely transfers ownership into an intermediate buffer.

### 8. Does the interleave loop require a dummy head?
No dummy head is needed because the original `head` node is guaranteed to remain the first node in the reordered list.

### 9. What is the effect of deep recursion in Tier 3 on systems with small stacks?
On systems with 64 KB thread stacks or Python's default recursion limit of 1000, recursion depth of $5 \times 10^4$ causes a stack overflow (`RecursionError`).

### 10. How does this problem relate to Palindrome Linked List (LeetCode 234)?
Both problems use the exact same first two steps: finding the middle via fast/slow pointers and reversing the second half. LeetCode 234 compares values, while LeetCode 143 interleaves pointers.

---

## 10. Related Problems and Systematic Progression Links

- [[0206-Reverse-Linked-List]]: Reversing a singly linked list in-place (used as subroutine).
- [[0876-Middle-of-the-Linked-List]]: Fast and slow pointer technique to locate the list median.
- [[0021-Merge-Two-Sorted-Lists]]: Merging two linked list chains sequentially.
- [[0234-Palindrome-Linked-List]]: Combining middle detection, reversal, and two-pointer verification.
- [[0025-Reverse-Nodes-in-k-Group]]: Advanced multi-node pointer rewiring in linked structures.

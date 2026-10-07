---
id: leetcode-0141-linked-list-cycle
title: "LeetCode 0141: Linked List Cycle"
tags:
  - dsa
  - leetcode
  - linked-list
  - two-pointers
  - floyds-algorithm
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/linked-list-cycle/"
---

# LeetCode 0141: Linked List Cycle

## 1. Problem Formalization and Constraints

Given `head`, the head of a linked list, determine if the linked list has a cycle in it.
There is a cycle in a linked list if there is some node in the list that can be reached again by continuously following the `next` pointer.
Internally, `pos` is used to denote the index of the node that tail's `next` pointer is connected to.
Note that `pos` is not passed as a parameter.
Return `true` if there is a cycle in the linked list.
Otherwise, return `false`.

### Constraints
- The number of the nodes in the list is in the range $[0, 10^4]$.
- $-10^5 \le \text{Node.val} \le 10^5$
- `pos` is `-1` or a valid index in the linked-list.

### Examples
- **Example 1**:
  - Input: `head = [3,2,0,-4], pos = 1`
  - Output: `true`
  - Explanation: There is a cycle in the linked list, where the tail connects to the 1st node (0-indexed).
- **Example 2**:
  - Input: `head = [1,2], pos = 0`
  - Output: `true`
  - Explanation: There is a cycle in the linked list, where the tail connects to the 0th node.
- **Example 3**:
  - Input: `head = [1], pos = -1`
  - Output: `false`
  - Explanation: There is no cycle in the linked list.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Floyd's Tortoise and Hare Algorithm | $O(N)$ | $O(1)$ | Dual pointers with differential velocities (1 vs 2 steps); eliminates memory overhead and protects immutability. |
| **Tier 2 (Destructive In-Place)** | Sentinel Pointer Rewiring | $O(N)$ | $O(1)$ | Rewires every visited node's `next` pointer to point to a dummy sentinel; mutates input structure permanently. |
| **Tier 3 (Hash Set)** | Visited Memory Address Set | $O(N)$ | $O(N)$ | Inserts pointer addresses into a hash set; detects cycles on first lookup collision. |
| **Tier 4 (Brute Force)** | Step Count Bounding | $O(N)$ | $O(1)$ | Advances pointer while counter $< 10^4 + 1$; assumes cycle if upper threshold exceeded. |

---

## 3. Tier 1: Most Optimal Solution (Floyd's Tortoise and Hare Algorithm)

### 3.1 Algorithmic Mechanics and Invariant Proof

Maintain two pointers starting at `head`:
- `slow` advances 1 node per iteration: `slow = slow.next`.
- `fast` advances 2 nodes per iteration: `fast = fast.next.next`.

If the list is acyclic, `fast` or `fast.next` encounters `nullptr` after at most $\lceil N / 2 \rceil$ steps, returning `false`.
If the list contains a cycle of length $C$:
Both pointers eventually enter the cycle.
Once both are inside the cycle, let $d$ denote the directed distance from `slow` to `fast` along the cycle ($0 \le d < C$).
In each subsequent iteration:
- `slow` moves forward by $1$.
- `fast` moves forward by $2$.
- The distance from `fast` to `slow` decreases by $(2 - 1) = 1 \pmod C$.
Because the relative speed difference is exactly 1 step per cycle, `fast` reduces the gap by 1 each iteration.
Therefore, `fast` must catch `slow` within at most $C$ steps, establishing `slow == fast` and proving termination.

**Invariant Proof**:
The relative distance $d_k = (d_0 - k) \pmod C$.
For $k = d_0$, $d_k \equiv 0 \pmod C$, ensuring that `fast` and `slow` land on the exact same node simultaneously without ever skipping over each other.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. If no cycle exists, $T \le N / 2$ steps. If a cycle exists, let non-cyclic length be $K$ and cycle length be $C$ where $K + C = N$. Entering the cycle takes $K$ steps, and meeting takes at most $C$ steps, yielding total steps $K + C = N$.
- **Auxiliary Space Complexity**: $O(1)$. Uses only two pointer variables.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

class Solution {
public:
    bool hasCycle(ListNode *head) {
        if (!head || !head->next) return false;

        ListNode *slow = head;
        ListNode *fast = head;

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;

            if (slow == fast) {
                return true;
            }
        }

        return false;
    }
};
```

#### Python 3
```python
from typing import Optional

class ListNode:
    def __init__(self, x):
        self.val = x
        self.next = None

class Solution:
    def hasCycle(self, head: Optional[ListNode]) -> bool:
        if not head or not head.next:
            return False

        slow = head
        fast = head

        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next

            if slow == fast:
                return True

        return False
```

#### Java 21
```java
class ListNode {
    int val;
    ListNode next;
    ListNode(int x) {
        val = x;
        next = null;
    }
}

public class Solution {
    public boolean hasCycle(ListNode head) {
        if (head == null || head.next == null) {
            return false;
        }

        ListNode slow = head;
        ListNode fast = head;

        while (fast != null && fast.next != null) {
            slow = slow.next;
            fast = fast.next.next;

            if (slow == fast) {
                return true;
            }
        }

        return false;
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

function hasCycle(head: ListNode | null): boolean {
    if (!head || !head.next) {
        return false;
    }

    let slow: ListNode | null = head;
    let fast: ListNode | null = head;

    while (fast && fast.next) {
        slow = slow!.next;
        fast = fast.next.next;

        if (slow === fast) {
            return true;
        }
    }

    return false;
}
```

#### Go 1.22
```go
package main

type ListNode struct {
	Val  int
	Next *ListNode
}

func hasCycle(head *ListNode) bool {
	if head == nil || head.Next == nil {
		return false
	}

	slow := head
	fast := head

	for fast != nil && fast.Next != nil {
		slow = slow.Next
		fast = fast.Next.Next

		if slow == fast {
			return true
		}
	}

	return false
}
```

#### Rust 2021
```rust
pub struct ListNode {
    pub val: i32,
    pub next: *const ListNode,
}

impl Solution {
    pub fn has_cycle(head: *const ListNode) -> bool {
        if head.is_null() {
            return false;
        }

        unsafe {
            let mut slow = head;
            let mut fast = head;

            while !fast.is_null() && !(*fast).next.is_null() {
                slow = (*slow).next;
                fast = (*(*fast).next).next;

                if slow == fast {
                    return true;
                }
            }
        }

        false
    }
}
```

---

## 4. Tier 2: Destructive In-Place Pointer Rewiring

### 4.1 Algorithmic Mechanics
Create a sentinel node `dummy`.
Iterate through the list:
- If `curr->next == &dummy`, a cycle exists because the pointer loops back to an already visited node.
- Otherwise, store `next_node = curr->next`, rewrite `curr->next = &dummy`, and advance `curr = next_node`.
This runs in $O(N)$ time and $O(1)$ space, but destroys the list topology.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$.
- **Space Complexity**: $O(1)$ auxiliary space.

### 4.3 Implementation (C++20)
```cpp
class Solution {
public:
    bool hasCycle(ListNode *head) {
        ListNode dummy(0);

        while (head != nullptr) {
            if (head->next == &dummy) {
                return true;
            }
            ListNode* next = head->next;
            head->next = &dummy;
            head = next;
        }

        return false;
    }
};
```

---

## 5. Tier 3: Visited Memory Address Set

### 5.1 Algorithmic Mechanics
Traverse the linked list and record the raw memory address of each node in a hash set `std::unordered_set<ListNode*>`.
If `head` is found in the set, a cycle exists.
If `head` reaches `nullptr`, the list is acyclic.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ average time.
- **Space Complexity**: $O(N)$ auxiliary memory for hash set buckets.

### 5.3 Implementation (C++20)
```cpp
#include <unordered_set>

class Solution {
public:
    bool hasCycle(ListNode *head) {
        std::unordered_set<ListNode*> seen;

        while (head != nullptr) {
            if (seen.count(head)) {
                return true;
            }
            seen.insert(head);
            head = head->next;
        }

        return false;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Step Count Bounding)

### 6.1 Algorithmic Mechanics
The problem constraints guarantee at most $10^4$ nodes.
Initialize an integer counter `steps = 0`.
Advance a single pointer forward, incrementing `steps++`.
If `steps > 10000`, conclude that a cycle exists.
If `nullptr` is hit prior to $10000$, conclude the list is acyclic.
This relies strictly on known finite problem constraints and fails on general inputs.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$ bounded by $O(\text{MAX\_NODES})$.
- **Space Complexity**: $O(1)$ memory.

### 6.3 Implementation (Python 3)
```python
from typing import Optional

class Solution:
    def hasCycle(self, head: Optional[ListNode]) -> bool:
        curr = head
        steps = 0
        limit = 10005

        while curr and steps < limit:
            curr = curr.next
            steps += 1

        return steps >= limit
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why can't the fast pointer skip over the slow pointer inside the cycle?</summary>
In each step, the distance from `fast` to `slow` along the direction of traversal decreases by exactly $(2 - 1) = 1$.
Because the decrement step size is an integer unit of 1, the distance must reach exactly 0 before it can become negative, ensuring collision.
</details>

<details>
<summary>2. What happens if the list contains 0 or 1 nodes?</summary>
If `head == nullptr` or `head->next == nullptr`, the initial guard `if (!head || !head->next) return false;` returns `false` in $O(1)$ time.
</details>

<details>
<summary>3. How does Linked List Cycle I differ from Linked List Cycle II (LeetCode 142)?</summary>
Cycle I only detects the presence of a cycle (`true`/`false`).
Cycle II locates the exact entry node of the cycle: after collision, resetting one pointer to `head` and advancing both by 1 step finds the entry node at their next collision.
</details>

<details>
<summary>4. Why does comparing node values instead of node pointers cause incorrect answers?</summary>
Multiple distinct nodes in the linked list can hold identical data values (such as all nodes having `val == 1`).
Identity must be determined by pointer memory address, not data contents.
</details>

<details>
<summary>5. What is the maximum number of steps before collision?</summary>
Let non-cyclic length be $K$ and cyclic length be $C$.
`slow` enters the cycle after $K$ steps.
At that instant, `fast` is at some position in the cycle.
`fast` will catch `slow` in at most $C - 1$ additional steps.
Total steps are bounded by $K + C = N$.
</details>

<details>
<summary>6. Why is Floyd's algorithm preferred over Brent's cycle detection algorithm here?</summary>
Brent's algorithm uses powers of 2 for step sizes and can reduce total steps by up to $36\%$ on average, but Floyd's algorithm is simpler to implement and has identical $O(N)$ time and $O(1)$ space.
</details>

<details>
<summary>7. What is the danger of the pointer rewiring method (Tier 2) in production code?</summary>
It mutates the input data structure permanently.
If another concurrent thread reads the list, or if the caller expects the list to remain intact, rewiring causes silent data corruption.
</details>

<details>
<summary>8. How does cycle detection apply to duplicate number detection in arrays (LeetCode 287)?</summary>
An array where each element $A[i]$ points to index $A[i]$ can be viewed as a functional graph with out-degree 1.
Floyd's algorithm detects duplicate values without mutating the array in $O(N)$ time and $O(1)$ space.
</details>

<details>
<summary>9. Why must `fast != nullptr && fast->next != nullptr` be checked in the while loop?</summary>
Because `fast` advances two steps via `fast->next->next`, dereferencing `fast->next` requires `fast != nullptr`, and dereferencing `fast->next->next` requires `fast->next != nullptr` to avoid null pointer segmentation faults.
</details>

<details>
<summary>10. How does memory caching affect two-pointer linked list traversal?</summary>
Linked list nodes allocated independently on the heap cause frequent L1/L2 cache misses.
`slow` and `fast` traverse memory non-contiguously, making linked list traversal memory-latency bound rather than CPU instruction bound.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/linked-list-cycle.cpp)
- [Python Implementation](../Python/linked-list-cycle.py)
- [Java Implementation](../Java/linked-list-cycle.java)
- [TypeScript Implementation](../TypeScript/linked-list-cycle.ts)
- [Go Implementation](../Golang/linked-list-cycle.go)
- [Rust Implementation](../Rust/linked-list-cycle.rs)

---
id: leetcode-0002-add-two-numbers
title: "LeetCode 0002: Add Two Numbers"
tags:
  - dsa
  - leetcode
  - linked-list
  - math
  - simulation
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/add-two-numbers/"
---

# LeetCode 0002: Add Two Numbers

## 1. Problem Formalization and Constraints

You are given two non-empty linked lists representing two non-negative integers.
The digits are stored in reverse order, and each of their nodes contains a single digit.
Add the two numbers and return the sum as a linked list.
You may assume the two numbers do not contain any leading zero, except the number 0 itself.

### Constraints
- The number of nodes in each linked list is in the range $[1, 100]$.
- $0 \le \text{Node.val} \le 9$
- It is guaranteed that the list represents a number that does not have leading zeros.

### Examples
- **Example 1**:
  - Input: `l1 = [2,4,3]`, `l2 = [5,6,4]`
  - Output: `[7,0,8]`
  - Explanation: $342 + 465 = 807$.
- **Example 2**:
  - Input: `l1 = [0]`, `l2 = [0]`
  - Output: `[0]`
- **Example 3**:
  - Input: `l1 = [9,9,9,9,9,9,9]`, `l2 = [9,9,9,9]`
  - Output: `[8,9,9,9,0,0,0,1]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative Simulation with Dummy Sentinel | $O(\max(N, M))$ | $O(1)$ auxiliary | Simulates column-by-column base-10 addition with carry bit; constructs output dynamically. |
| **Tier 2 (In-Place)** | Mutating Longest Existing List | $O(\max(N, M))$ | $O(1)$ total | Overwrites existing nodes of $l_1$ or $l_2$ directly; eliminates heap node allocations except on final carry overflow. |
| **Tier 3 (Recursive)** | Inductive Call-Frame Carry Passing | $O(\max(N, M))$ | $O(\max(N, M))$ | Recursively computes next node with forward carry parameter; consumes recursion call stack frames. |
| **Tier 4 (Brute Force)** | Big-Integer Full Decimal Conversion | $O(N + M)$ | $O(N + M)$ | Converts lists to arbitrary-precision BigInts, adds them mathematically, and reconstructs a linked list. |

---

## 3. Tier 1: Most Optimal Solution (Iterative Simulation with Dummy Sentinel)

### 3.1 Algorithmic Mechanics and Invariant Proof

Because digits are stored in reverse order, the head of each linked list corresponds to the least significant digit (ones column, $10^0$).
This matches standard elementary school addition from right to left:
1. Initialize a sentinel node `dummy` and pointer `curr = &dummy`.
2. Initialize integer `carry = 0`.
3. Loop while `l1 != nullptr`, `l2 != nullptr`, or `carry != 0`:
   - Extract digit from $l_1$ (or 0 if exhausted).
   - Extract digit from $l_2$ (or 0 if exhausted).
   - Compute `sum = val1 + val2 + carry`.
   - Update `carry = sum / 10`.
   - Allocate new node with digit `sum % 10` and append to `curr->next`.
   - Advance `curr` and active list pointers.
4. Return `dummy.next`.

**Invariant Proof**:
Let $A_k = \sum_{i=0}^{k-1} a_i 10^i$ and $B_k = \sum_{i=0}^{k-1} b_i 10^i$ denote the numbers formed by the first $k$ digits of $l_1$ and $l_2$.
At the start of step $k$, the generated output list encodes $\sum_{i=0}^{k-1} d_i 10^i$.
The induction hypothesis maintains:
$$A_k + B_k = \left(\sum_{i=0}^{k-1} d_i 10^i\right) + \text{carry} \cdot 10^k$$
In step $k$:
$$\text{sum} = a_k + b_k + \text{carry}$$
$$d_k = \text{sum} \pmod{10}$$
$$\text{carry}' = \lfloor \text{sum} / 10 \rfloor$$
Substituting $d_k$ and $\text{carry}'$ preserves the exact decimal invariant for step $k+1$.
When all digits and carry are processed, the accumulated list exactly equals the sum.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(\max(N, M))$, where $N$ and $M$ are lengths of $l_1$ and $l_2$. The loop executes $\max(N, M)$ times, plus at most one additional cycle if the highest place value overflows.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space excluding the returned output list of length $\max(N, M) + 1$.

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
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode dummy(0);
        ListNode* curr = &dummy;
        int carry = 0;

        while (l1 || l2 || carry) {
            int sum = carry;
            if (l1) {
                sum += l1->val;
                l1 = l1->next;
            }
            if (l2) {
                sum += l2->val;
                l2 = l2->next;
            }

            carry = sum / 10;
            curr->next = new ListNode(sum % 10);
            curr = curr->next;
        }

        return dummy.next;
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
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
        dummy = ListNode(0)
        curr = dummy
        carry = 0

        while l1 or l2 or carry:
            total = carry
            if l1:
                total += l1.val
                l1 = l1.next
            if l2:
                total += l2.val
                l2 = l2.next

            carry = total // 10
            curr.next = ListNode(total % 10)
            curr = curr.next

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

public class Solution {
    public ListNode addTwoNumbers(ListNode l1, ListNode l2) {
        ListNode dummy = new ListNode(0);
        ListNode curr = dummy;
        int carry = 0;

        while (l1 != null || l2 != null || carry != 0) {
            int sum = carry;
            if (l1 != null) {
                sum += l1.val;
                l1 = l1.next;
            }
            if (l2 != null) {
                sum += l2.val;
                l2 = l2.next;
            }

            carry = sum / 10;
            curr.next = new ListNode(sum % 10);
            curr = curr.next;
        }

        return dummy.next;
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

function addTwoNumbers(l1: ListNode | null, l2: ListNode | null): ListNode | null {
    const dummy = new ListNode(0);
    let curr = dummy;
    let carry = 0;

    while (l1 !== null || l2 !== null || carry !== 0) {
        let sum = carry;
        if (l1 !== null) {
            sum += l1.val;
            l1 = l1.next;
        }
        if (l2 !== null) {
            sum += l2.val;
            l2 = l2.next;
        }

        carry = Math.floor(sum / 10);
        curr.next = new ListNode(sum % 10);
        curr = curr.next;
    }

    return dummy.next;
}
```

#### Go 1.22
```go
package main

type ListNode struct {
	Val  int
	Next *ListNode
}

func addTwoNumbers(l1 *ListNode, l2 *ListNode) *ListNode {
	dummy := &ListNode{Val: 0}
	curr := dummy
	carry := 0

	for l1 != nil || l2 != nil || carry != 0 {
		sum := carry
		if l1 != nil {
			sum += l1.Val
			l1 = l1.Next
		}
		if l2 != nil {
			sum += l2.Val
			l2 = l2.Next
		}

		carry = sum / 10
		curr.Next = &ListNode{Val: sum % 10}
		curr = curr.Next
	}

	return dummy.Next
}
```

#### Rust 1.75
```rust
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
    pub fn add_two_numbers(
        mut l1: Option<Box<ListNode>>,
        mut l2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut dummy = ListNode::new(0);
        let mut tail = &mut dummy;
        let mut carry = 0;

        while l1.is_some() || l2.is_some() || carry != 0 {
            let mut sum = carry;

            if let Some(node) = l1 {
                sum += node.val;
                l1 = node.next;
            }
            if let Some(node) = l2 {
                sum += node.val;
                l2 = node.next;
            }

            carry = sum / 10;
            tail.next = Some(Box::new(ListNode::new(sum % 10)));
            tail = tail.next.as_mut().unwrap();
        }

        dummy.next
    }
}
```

---

## 4. Tier 2: In-Place Mutation of Existing Linked List

### 4.1 Implementation Mechanism
Rather than dynamically allocating $\max(N, M)$ new heap objects, we can reuse the nodes of `l1` in-place.
When `l1` runs out of nodes while `l2` or carry still has remaining terms, append `l2` to `l1`'s tail or allocate only the final overflow node.

```cpp
class SolutionInPlace {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* head = l1;
        ListNode* prev = nullptr;
        int carry = 0;

        while (l1 || l2 || carry) {
            if (!l1) {
                prev->next = new ListNode(0);
                l1 = prev->next;
            }
            int sum = carry + l1->val + (l2 ? l2->val : 0);
            l1->val = sum % 10;
            carry = sum / 10;
            prev = l1;
            l1 = l1->next;
            if (l2) l2 = l2->next;
        }

        return head;
    }
};
```

### 4.2 Trade-offs
- Achieves absolute minimum heap allocations.
- Mutates input parameters, which may violate immutability contracts in concurrent or functional codebases.

---

## 5. Tier 3: Recursive Inductive Carry-Passing Decomposition

### 5.1 Algorithmic Structure
A recursive function takes `(l1, l2, carry)`.
At each frame, it computes `sum`, instantiates a `ListNode(sum % 10)`, and links its `next` pointer to `helper(l1.next, l2.next, sum / 10)`.

```python
class SolutionRecursive:
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode], carry: int = 0) -> Optional[ListNode]:
        if not l1 and not l2 and not carry:
            return None

        total = carry
        if l1:
            total += l1.val
            l1 = l1.next
        if l2:
            total += l2.val
            l2 = l2.next

        node = ListNode(total % 10)
        node.next = self.addTwoNumbers(l1, l2, total // 10)
        return node
```

### 5.2 Trade-offs
- Mathematically elegant and compact.
- Incurs $O(\max(N, M))$ stack frames, which increases overhead compared to iterative loops.

---

## 6. Tier 4: Brute Force Baseline (Arbitrary-Precision BigInt Conversion)

### 6.1 Mechanical Description
Traverse $l_1$ and $l_2$ entirely, reconstructing the numeric values $N_1$ and $N_2$ using positional arithmetic:
$$N_1 = \sum_{i=0}^{n-1} \text{node}_i \cdot 10^i$$
Sum $S = N_1 + N_2$, convert $S$ to a string or repeatedly divide by 10, and construct a new linked list.

### 6.2 Complexity
- **Time Complexity**: $O(N + M)$ for conversion and digit parsing.
- **Space Complexity**: $O(N + M)$ for string representation and BigInt limbs.
- **Verdict**: In languages without native arbitrary-precision arithmetic (C++, Java without `BigInteger`), standard 64-bit integer (`long long`) overflows immediately when lists exceed 18 digits.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Memory Allocator Pressure**: Iterative node allocation causes calls to `malloc` or `new` for each digit, fragmenting the heap.
2. In production systems, a custom slab allocator or memory pool preallocating a block of nodes eliminates allocator contention.
3. **Rust Box Overhead**: In Rust, `Box<ListNode>` wraps a heap pointer.
4. Using an arena allocator (such as `typed-arena` or index vectors) provides contiguous layout and better cache locality.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Unequal List Lengths | `[9, 9]` + `[1]` | Output `[0, 0, 1]` | Ternary / `if (l)` check pads shorter list with zeros. |
| Final Carry Overflow | `[5]` + `[5]` | Output `[0, 1]` | Loop condition checks `carry != 0`. |
| Single Zero Nodes | `[0]` + `[0]` | Output `[0]` | Correctly processes 0 without termination. |
| Long Cascade Carries | `[9, 9, 9]` + `[1]` | Output `[0, 0, 0, 1]` | Carries ripple correctly across all columns. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why are digits stored in reverse order in this problem?
Storing digits in reverse order aligns the least significant digit with the head of the list, allowing single-pass addition from ones to tens to hundreds without list reversal.

### 2. How does LeetCode 445 (Add Two Numbers II) differ?
In LeetCode 445, digits are stored in forward order (most significant digit first). That requires either reversing both input lists or utilizing stacks to access digits in reverse.

### 3. Why is a dummy sentinel node recommended?
The dummy head simplifies pointer handling by eliminating special-case branching for initializing the `head` of the result list.

### 4. What is the maximum possible value of `carry` in any step?
The maximum sum is $9 + 9 + 1 = 19$, where `carry = 1`. Therefore, `carry` is strictly binary: either 0 or 1.

### 5. Why can standard 64-bit integers not be used for direct calculation?
The problem constraints allow lists with up to 100 nodes, representing numbers up to $10^{100} - 1$, which vastly exceeds the $2^{64}-1 \approx 1.84 \times 10^{19}$ capacity of 64-bit unsigned integers.

### 6. Can memory leaks occur in C++?
In C++, newly allocated `ListNode` instances must be cleaned up if the list is subsequently discarded. In production, smart pointers or RAII list wrappers are preferred.

### 7. How does Rust prevent data races during in-place node mutation?
Rust enforces exclusive access through `&mut` references, guaranteeing that node pointers are not accessed by other threads or references during mutation.

### 8. Does the condition `while (l1 || l2 || carry)` execute extra iterations?
It executes exactly $\max(N, M)$ iterations if there is no final carry, or $\max(N, M) + 1$ iterations if a final carry of 1 is generated.

### 9. What is the space overhead of dummy sentinel allocation?
The dummy node is allocated on the function stack in C++ and Rust, incurring 0 bytes of dynamic heap allocation overhead.

### 10. Can this problem be solved with bitwise operations?
Base-10 addition relies on division and modulo operations (`/ 10` and `% 10`). Bitwise arithmetic is applicable to base-2 binary addition (LeetCode 67).

---

## 10. Related Problems and Systematic Progression Links

- [[0021-Merge-Two-Sorted-Lists]]: Two-pointer linear merging of linked list structures.
- [[0023-Merge-k-Sorted-Lists]]: Multi-way divide-and-conquer linked list merging.
- [[0206-Reverse-Linked-List]]: Reversing linked lists in-place.
- LeetCode 43 (Multiply Strings): Arbitrary-precision polynomial multiplication of numbers.
- LeetCode 67 (Add Binary): Base-2 addition on strings with carry propagation.
- LeetCode 445 (Add Two Numbers II): Adding numbers stored in forward order using stacks or list reversals.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/add-two-numbers.cpp)
- [Python Implementation](../Python/add-two-numbers.py)
- [Java Implementation](../Java/add-two-numbers.java)
- [TypeScript Implementation](../TypeScript/add-two-numbers.ts)
- [Go Implementation](../Golang/add-two-numbers.go)
- [Rust Implementation](../Rust/add-two-numbers.rs)

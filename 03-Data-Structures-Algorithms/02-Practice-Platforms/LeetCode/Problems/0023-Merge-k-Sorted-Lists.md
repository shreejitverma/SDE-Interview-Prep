---
id: leetcode-0023-merge-k-sorted-lists
title: "LeetCode 0023: Merge k Sorted Lists"
tags:
  - dsa
  - leetcode
  - linked-list
  - divide-and-conquer
  - heap
  - priority-queue
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/merge-k-sorted-lists/"
---

# LeetCode 0023: Merge k Sorted Lists

## 1. Problem Formalization and Constraints

You are given an array of $k$ linked-lists `lists`, each linked-list is sorted in ascending order.
Merge all the linked-lists into one sorted linked-list and return it.

### Constraints
- $k == \text{lists.length}$
- $0 \le k \le 10^4$
- $0 \le \text{lists}[i]\text{.length} \le 500$
- $-10^4 \le \text{lists}[i][j] \le 10^4$
- `lists[i]` is sorted in ascending order.
- The sum of `lists[i].length` will not exceed $10^4$.

### Examples
- **Example 1**:
  - Input: `lists = [[1,4,5],[1,3,4],[2,6]]`
  - Output: `[1,1,2,3,4,4,5,6]`
  - Explanation: The linked-lists are: `[1->4->5, 1->3->4, 2->6]`. Merging them into one sorted list yields `1->1->2->3->4->4->5->6`.
- **Example 2**:
  - Input: `lists = []`
  - Output: `[]`
- **Example 3**:
  - Input: `lists = [[]]`
  - Output: `[]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative Divide and Conquer | $O(N \log K)$ | $O(1)$ | Merges pairs in bottom-up rounds; reduces list count by half each round without container allocations. |
| **Tier 2 (Min-Heap)** | Priority Queue Frontier Tracking | $O(N \log K)$ | $O(K)$ | Maintains $K$ head pointers in a min-heap; pops smallest element and pushes successor in $O(\log K)$ time. |
| **Tier 3 (Sequential Accumulation)** | One-by-One Linear Merging | $O(N \times K)$ | $O(1)$ | Progressively merges each subsequent list into an accumulator; degrades on large $K$ due to repeated scanning. |
| **Tier 4 (Brute Force)** | Array Materialization & Full Sort | $O(N \log N)$ | $O(N)$ | Dumps all $N$ values into an array, sorts with generic quicksort, and constructs a fresh linked list. |

---

## 3. Tier 1: Most Optimal Solution (Iterative Divide and Conquer)

### 3.1 Algorithmic Mechanics and Invariant Proof

Instead of merging lists one by one, we pair up the lists and merge each pair using the standard two-pointer linked list merge algorithm:
1. In round 1, merge list 0 with list 1, list 2 with list 3, and so on.
2. In round 2, merge the resulting merged lists in pairs.
3. Repeat until only one combined list remains at index 0.

Iterative stride doubling eliminates call stack recursion overhead:
- Set `interval = 1`.
- While `interval < lists.length`:
  - For `i` from $0$ with step $2 \times \text{interval}$:
    - If $i + \text{interval} < \text{lists.length}$, set `lists[i] = mergeTwoLists(lists[i], lists[i + interval])`.
  - Double `interval *= 2`.
- Return `lists[0]`.

**Invariant Proof**:
At step $m$, there are $\lceil K / 2^m \rceil$ active lists.
The total number of nodes across all active lists remains invariant at $N$.
Each round merges disjoint pairs of lists, touching each node at most twice per level.
The height of the reduction tree is $\lceil \log_2 K \rceil$.
Therefore, every node participates in exactly $\log_2 K$ merge operations, proving an overall runtime of $O(N \log K)$ with strictly $O(1)$ auxiliary memory.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \log K)$, where $N$ is the total count of all nodes across all $K$ lists. There are $\lceil \log_2 K \rceil$ levels, and each level processes at most $N$ node comparisons.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space. The lists are merged strictly by rewiring pointer links in-place without dynamic container allocations.

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

#include <vector>

class Solution {
public:
    ListNode* mergeKLists(std::vector<ListNode*>& lists) {
        if (lists.empty()) return nullptr;

        size_t interval = 1;
        while (interval < lists.size()) {
            for (size_t i = 0; i + interval < lists.size(); i += interval * 2) {
                lists[i] = mergeTwoLists(lists[i], lists[i + interval]);
            }
            interval *= 2;
        }

        return lists[0];
    }

private:
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode dummy(0);
        ListNode* curr = &dummy;

        while (l1 && l2) {
            if (l1->val <= l2->val) {
                curr->next = l1;
                l1 = l1->next;
            } else {
                curr->next = l2;
                l2 = l2->next;
            }
            curr = curr->next;
        }

        curr->next = l1 ? l1 : l2;
        return dummy.next;
    }
};
```

#### Python 3
```python
from typing import List, Optional

class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next

class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        if not lists:
            return None

        def merge_two_lists(l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
            dummy = ListNode(0)
            curr = dummy

            while l1 and l2:
                if l1.val <= l2.val:
                    curr.next = l1
                    l1 = l1.next
                else:
                    curr.next = l2
                    l2 = l2.next
                curr = curr.next

            curr.next = l1 if l1 else l2
            return dummy.next

        interval = 1
        while interval < len(lists):
            for i in range(0, len(lists) - interval, interval * 2):
                lists[i] = merge_two_lists(lists[i], lists[i + interval])
            interval *= 2

        return lists[0]
```

#### Java 21
```java
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        if (lists == null || lists.length == 0) return null;

        int interval = 1;
        while (interval < lists.length) {
            for (int i = 0; i + interval < lists.length; i += interval * 2) {
                lists[i] = mergeTwoLists(lists[i], lists[i + interval]);
            }
            interval *= 2;
        }

        return lists[0];
    }

    private ListNode mergeTwoLists(ListNode l1, ListNode l2) {
        ListNode dummy = new ListNode(0);
        ListNode curr = dummy;

        while (l1 != null && l2 != null) {
            if (l1.val <= l2.val) {
                curr.next = l1;
                l1 = l1.next;
            } else {
                curr.next = l2;
                l2 = l2.next;
            }
            curr = curr.next;
        }

        curr.next = (l1 != null) ? l1 : l2;
        return dummy.next;
    }
}
```

#### TypeScript 5
```typescript
function mergeKLists(lists: Array<ListNode | null>): ListNode | null {
    if (!lists || lists.length === 0) return null;

    function mergeTwoLists(l1: ListNode | null, l2: ListNode | null): ListNode | null {
        const dummy = new ListNode(0);
        let curr = dummy;

        while (l1 && l2) {
            if (l1.val <= l2.val) {
                curr.next = l1;
                l1 = l1.next;
            } else {
                curr.next = l2;
                l2 = l2.next;
            }
            curr = curr.next;
        }

        curr.next = l1 ? l1 : l2;
        return dummy.next;
    }

    let interval = 1;
    while (interval < lists.length) {
        for (let i = 0; i + interval < lists.length; i += interval * 2) {
            lists[i] = mergeTwoLists(lists[i], lists[i + interval]);
        }
        interval *= 2;
    }

    return lists[0];
}
```

#### Go 1.22
```go
package main

func mergeKLists(lists []*ListNode) *ListNode {
	if len(lists) == 0 {
		return nil
	}

	interval := 1
	for interval < len(lists) {
		for i := 0; i+interval < len(lists); i += interval * 2 {
			lists[i] = mergeTwoLists(lists[i], lists[i+interval])
		}
		interval *= 2
	}

	return lists[0]
}

func mergeTwoLists(l1, l2 *ListNode) *ListNode {
	dummy := &ListNode{}
	curr := dummy

	for l1 != nil && l2 != nil {
		if l1.Val <= l2.Val {
			curr.Next = l1
			l1 = l1.Next
		} else {
			curr.Next = l2
			l2 = l2.Next
		}
		curr = curr.Next
	}

	if l1 != nil {
		curr.Next = l1
	} else {
		curr.Next = l2
	}

	return dummy.Next
}
```

#### Rust 2021
```rust
impl Solution {
    pub fn merge_k_lists(mut lists: Vec<Option<Box<ListNode>>>) -> Option<Box<ListNode>> {
        if lists.is_empty() {
            return None;
        }

        let mut interval = 1;
        while interval < lists.len() {
            let mut i = 0;
            while i + interval < lists.len() {
                let l1 = lists[i].take();
                let l2 = lists[i + interval].take();
                lists[i] = Self::merge_two_lists(l1, l2);
                i += interval * 2;
            }
            interval *= 2;
        }

        lists[0].take()
    }

    fn merge_two_lists(
        mut l1: Option<Box<ListNode>>,
        mut l2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut dummy = ListNode::new(0);
        let mut curr = &mut dummy;

        while l1.is_some() && l2.is_some() {
            if l1.as_ref().unwrap().val <= l2.as_ref().unwrap().val {
                let next = l1.as_mut().unwrap().next.take();
                curr.next = l1;
                l1 = next;
            } else {
                let next = l2.as_mut().unwrap().next.take();
                curr.next = l2;
                l2 = next;
            }
            curr = curr.next.as_mut().unwrap();
        }

        curr.next = if l1.is_some() { l1 } else { l2 };
        dummy.next
    }
}
```

---

## 4. Tier 2: Space-Optimized Heap (Min-Priority Queue)

### 4.1 Algorithmic Mechanics
Push the head pointer of each non-empty list into a min-priority queue ordered by `node->val`.
At each iteration:
- Extract the smallest node `minNode` from the heap in $O(\log K)$ time.
- Append `minNode` to the merged list tail.
- If `minNode->next` is non-null, insert `minNode->next` into the heap.
Repeat until the heap is empty.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \log K)$. Every one of the $N$ nodes is inserted and extracted from a heap of size at most $K$.
- **Space Complexity**: $O(K)$ auxiliary space to maintain the priority queue.

### 4.3 Implementation (C++20)
```cpp
#include <queue>
#include <vector>

class Solution {
public:
    ListNode* mergeKLists(std::vector<ListNode*>& lists) {
        auto cmp = [](ListNode* a, ListNode* b) {
            return a->val > b->val;
        };
        std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> pq(cmp);

        for (ListNode* head : lists) {
            if (head) pq.push(head);
        }

        ListNode dummy(0);
        ListNode* curr = &dummy;

        while (!pq.empty()) {
            ListNode* node = pq.top();
            pq.pop();

            curr->next = node;
            curr = curr->next;

            if (node->next) {
                pq.push(node->next);
            }
        }

        return dummy.next;
    }
};
```

---

## 5. Tier 3: Sequential Accumulation (One-by-One Linear Merging)

### 5.1 Algorithmic Mechanics
Initialize an accumulator list `result = lists[0]`.
Iterate sequentially through the remaining lists $i = 1 \dots K - 1$, merging `result = mergeTwoLists(result, lists[i])`.
Because early elements in `result` are repeatedly traversed across subsequent iterations, total runtime scales quadratically with $K$.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N \times K)$. The first list is compared $K - 1$ times, the second $K - 2$ times, yielding $\sum_{i=1}^K i \cdot \frac{N}{K} = O(N \times K)$.
- **Space Complexity**: $O(1)$ auxiliary space.

### 5.3 Implementation (C++20)
```cpp
class Solution {
public:
    ListNode* mergeKLists(std::vector<ListNode*>& lists) {
        if (lists.empty()) return nullptr;

        ListNode* result = lists[0];
        for (size_t i = 1; i < lists.size(); ++i) {
            result = mergeTwo(result, lists[i]);
        }

        return result;
    }

private:
    ListNode* mergeTwo(ListNode* l1, ListNode* l2) {
        ListNode dummy(0);
        ListNode* curr = &dummy;
        while (l1 && l2) {
            if (l1->val <= l2->val) {
                curr->next = l1;
                l1 = l1->next;
            } else {
                curr->next = l2;
                l2 = l2->next;
            }
            curr = curr->next;
        }
        curr->next = l1 ? l1 : l2;
        return dummy.next;
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Array Materialization & Full Sort)

### 6.1 Algorithmic Mechanics
Traverse all $K$ linked lists and extract all $N$ integer values into a flat dynamic array.
Sort the array using standard library introsort in $O(N \log N)$ time.
Iterate through the sorted array and allocate a new linked list node for each element.
This approach ignores the pre-existing sorted invariant of the input lists.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$.
- **Space Complexity**: $O(N)$ auxiliary space for the value vector.

### 6.3 Implementation (Python 3)
```python
from typing import List, Optional

class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        vals = []
        for l in lists:
            curr = l
            while curr:
                vals.append(curr.val)
                curr = curr.next

        vals.sort()
        dummy = ListNode(0)
        curr = dummy
        for v in vals:
            curr.next = ListNode(v)
            curr = curr.next

        return dummy.next
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why is Divide and Conquer (Tier 1) asymptotically superior to Sequential Merging (Tier 3)?</summary>
Sequential merging re-reads elements of previously merged lists for every subsequent list, resulting in $O(N \times K)$ operations.
Divide and conquer processes lists in a binary tree hierarchy of height $\log_2 K$, guaranteeing that every node is inspected only $\log_2 K$ times.
</details>

<details>
<summary>2. Why does Tier 1 have lower memory overhead than Tier 2 (Heap)?</summary>
The heap approach requires allocating an auxiliary priority queue container holding $K$ pointers and suffers priority queue pointer dereferencing and rebalancing overhead.
Divide and conquer merges existing list nodes by reference with zero heap container allocation ($O(1)$ space).
</details>

<details>
<summary>3. What happens if the input is an empty array `lists = []`?</summary>
The initial guard `if (lists.empty()) return nullptr;` catches the empty input and returns `nullptr` immediately.
</details>

<details>
<summary>4. What happens if `lists` contains multiple empty linked lists, such as `lists = [[], []]`?</summary>
`mergeTwoLists(nullptr, nullptr)` returns `nullptr`.
The algorithm gracefully returns `nullptr` without segmentation faults.
</details>

<details>
<summary>5. How does Rust's ownership model handle moving list heads into helper functions?</summary>
`Option::take` swaps `None` into the vector slot while taking ownership of `Some(Box<ListNode>)`.
This satisfies borrow checker rules during pairwise merges.
</details>

<details>
<summary>6. When would the Heap approach be preferred over Divide and Conquer?</summary>
When lists are streamed in real time or when elements must be yielded incrementally as a lazy iterator (generator), a min-heap produces each smallest element one at a time on demand.
</details>

<details>
<summary>7. What is the maximum number of comparisons performed when merging two lists of lengths $L_1$ and $L_2$?</summary>
At most $L_1 + L_2 - 1$ comparisons, because each comparison attaches at least one element to the merged list and the final remaining element is linked in $O(1)$.
</details>

<details>
<summary>8. How do duplicate values across multiple lists affect the merge?</summary>
The comparison check `l1.val <= l2.val` preserves stable sorting order for equivalent values.
</details>

<details>
<summary>9. What is the cache behavior of linked list merging versus array sorting?</summary>
Linked list nodes scattered randomly in heap memory trigger high cache miss penalties.
However, merging rewires existing pointers without creating new nodes, minimizing memory allocation traffic.
</details>

<details>
<summary>10. What is the relationship between this problem and external merge sort on disk?</summary>
Multi-way external merge sort partitions huge datasets across $K$ sorted files and uses a $K$-way min-heap (or tournament tree / loser tree) to stream-merge data into a single output file.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/merge-k-sorted-lists.cpp)
- [Python Implementation](../Python/merge-k-sorted-lists.py)
- [Java Implementation](../Java/merge-k-sorted-lists.java)
- [TypeScript Implementation](../TypeScript/merge-k-sorted-lists.ts)
- [Go Implementation](../Golang/merge-k-sorted-lists.go)
- [Rust Implementation](../Rust/merge-k-sorted-lists.rs)

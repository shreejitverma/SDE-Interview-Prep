---
id: leetcode-0295-find-median-from-data-stream
title: "LeetCode 0295: Find Median from Data Stream"
tags:
  - dsa
  - leetcode
  - design
  - heap
  - priority-queue
  - data-stream
  - two-pointers
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/find-median-from-data-stream/"
---

# LeetCode 0295: Find Median from Data Stream

## 1. Problem Formalization and Constraints

The median is the middle value in an ordered integer list.
If the size of the list is even, there is no middle value, and the median is the mean of the two middle values.
- For example, for `arr = [2,3,4]`, the median is `3`.
- For example, for `arr = [2,3]`, the median is `(2 + 3) / 2 = 2.5`.

Implement the `MedianFinder` class:
- `MedianFinder()`: Initializes the `MedianFinder` object.
- `void addNum(int num)`: Adds the integer `num` from the data stream to the data structure.
- `double findMedian()`: Returns the median of all elements so far. Answers within $10^{-5}$ of the actual answer will be accepted.

### Constraints
- $-10^5 \le \text{num} \le 10^5$
- There will be at least one element in the data structure before calling `findMedian`.
- At most $5 \times 10^4$ calls will be made to `addNum` and `findMedian`.

### Follow-up Questions
1. If all integer numbers from the stream are between 0 and 100, how would you optimize it?
2. If $99\%$ of all integer numbers from the stream are between 0 and 100, how would you optimize it?

### Examples
- **Example 1**:
  - Input:
    - `["MedianFinder", "addNum", "addNum", "findMedian", "addNum", "findMedian"]`
    - `[[], [1], [2], [], [3], []]`
  - Output:
    - `[null, null, null, 1.5, null, 2.0]`
  - Explanation:
    - `MedianFinder medianFinder = new MedianFinder();`
    - `medianFinder.addNum(1);    // arr = [1]`
    - `medianFinder.addNum(2);    // arr = [1, 2]`
    - `medianFinder.findMedian(); // return 1.5 (i.e., (1 + 2) / 2)`
    - `medianFinder.addNum(3);    // arr[1, 2, 3]`
    - `medianFinder.findMedian(); // return 2.0`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Two Heaps (Max-Heap + Min-Heap) | $O(\log N)$ add, $O(1)$ find | $O(N)$ total | Partitions numbers into lower half (`maxHeap`) and upper half (`minHeap`); maintains invariant $|\text{maxHeap}| - |\text{minHeap}| \in \{0, 1\}$. |
| **Tier 2 (Balanced BST)** | Two Multisets / Balanced BST | $O(\log N)$ add, $O(1)$ find | $O(N)$ total | Maintains sorted order with dynamic iterator pointers at the median; higher constant factor overhead than binary heaps. |
| **Tier 3 (Insertion Sort)** | Binary Search with Insertion (`std::vector`) | $O(N)$ add, $O(1)$ find | $O(N)$ total | Uses binary search `bisect` in $O(\log N)$ to find position, but shifts elements in $O(N)$; acceptable for small streams. |
| **Tier 4 (Brute Force)** | Unsorted Array with Full Sort | $O(1)$ add, $O(N \log N)$ find | $O(N)$ total | Appends incoming elements to a buffer; sorts the entire buffer on every `findMedian` call; prohibitively slow on frequent queries. |

---

## 3. Tier 1: Most Optimal Solution (Two Heaps)

### 3.1 Algorithmic Mechanics and Invariant Proof

The stream is partitioned into two halves:
- `max_heap` (Lower half): Stores the smaller half of the numbers. The root gives the maximum of this half.
- `min_heap` (Upper half): Stores the larger half of the numbers. The root gives the minimum of this half.

**Structural Invariants**:
1. **Ordering Invariant**: Every element in `max_heap` is $\le$ every element in `min_heap`:
   $$\max(\text{max\_heap}) \le \min(\text{min\_heap})$$
2. **Balancing Invariant**: The sizes of the two heaps differ by at most 1:
   $$0 \le |\text{max\_heap}| - |\text{min\_heap}| \le 1$$

**Operations**:
1. `addNum(num)`:
   - If `max_heap` is empty or `num <= max_heap.top()`, push `num` to `max_heap`.
   - Otherwise, push `num` to `min_heap`.
   - **Rebalance Step**:
     - If $|\text{max\_heap}| > |\text{min\_heap}| + 1$, transfer `max_heap.top()` to `min_heap`.
     - If $|\text{min\_heap}| > |\text{max\_heap}|$, transfer `min_heap.top()` to `max_heap`.
2. `findMedian()`:
   - If $|\text{max\_heap}| > |\text{min\_heap}|$, return `max_heap.top()`.
   - If $|\text{max\_heap}| == |\text{min\_heap}|$, return `(max_heap.top() + min_heap.top()) / 2.0`.

**Invariant Proof**:
Let $N = |\text{max\_heap}| + |\text{min\_heap}|$ be the total count of inserted elements.
By the ordering invariant, all elements in `max_heap` precede all elements in `min_heap` in the global sorted order.
If $N$ is odd, the balancing invariant enforces $|\text{max\_heap}| = (N + 1) / 2$ and $|\text{min\_heap}| = (N - 1) / 2$.
Thus, `max_heap.top()` is exactly the $\lceil N/2 \rceil$-th element, which is the exact mathematical median.
If $N$ is even, $|\text{max\_heap}| = |\text{min\_heap}| = N / 2$.
The two middle elements are `max_heap.top()` (the $(N/2)$-th element) and `min_heap.top()` (the $(N/2 + 1)$-th element).
Their arithmetic mean is by definition the median of the even-sized sequence.
Each rebalancing step restores both invariants in $O(\log N)$ time, establishing mathematical correctness.

### 3.2 Complexity Analysis
- **Time Complexity**:
  - `addNum(num)`: $O(\log N)$ for pushing and rebalancing heap elements.
  - `findMedian()`: $O(1)$ direct heap top inspections.
- **Auxiliary Space Complexity**: $O(N)$ total space to store the $N$ incoming elements across both priority queues.

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addNum: O(log N), findMedian: O(1)
// Space: O(N)

#include <queue>
#include <vector>

using namespace std;

class MedianFinder {
public:
    MedianFinder() = default;

    void addNum(int num) {
        if (max_heap_.empty() || num <= max_heap_.top()) {
            max_heap_.push(num);
        } else {
            min_heap_.push(num);
        }

        // Rebalance
        if (max_heap_.size() > min_heap_.size() + 1) {
            min_heap_.push(max_heap_.top());
            max_heap_.pop();
        } else if (min_heap_.size() > max_heap_.size()) {
            max_heap_.push(min_heap_.top());
            min_heap_.pop();
        }
    }

    double findMedian() const {
        if (max_heap_.size() > min_heap_.size()) {
            return max_heap_.top();
        }
        return (static_cast<double>(max_heap_.top()) + min_heap_.top()) / 2.0;
    }

private:
    priority_queue<int> max_heap_;                             // lower half
    priority_queue<int, vector<int>, greater<int>> min_heap_; // upper half
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  addNum: O(log N), findMedian: O(1)
# Space: O(N)

import heapq

class MedianFinder:
    def __init__(self):
        self.max_heap = []  # lower half (inverted values for max-heap)
        self.min_heap = []  # upper half

    def addNum(self, num: int) -> None:
        if not self.max_heap or num <= -self.max_heap[0]:
            heapq.heappush(self.max_heap, -num)
        else:
            heapq.heappush(self.min_heap, num)

        # Rebalance
        if len(self.max_heap) > len(self.min_heap) + 1:
            val = -heapq.heappop(self.max_heap)
            heapq.heappush(self.min_heap, val)
        elif len(self.min_heap) > len(self.max_heap):
            val = heapq.heappop(self.min_heap)
            heapq.heappush(self.max_heap, -val)

    def findMedian(self) -> float:
        if len(self.max_heap) > len(self.min_heap):
            return float(-self.max_heap[0])
        return (-self.max_heap[0] + self.min_heap[0]) / 2.0
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addNum: O(log N), findMedian: O(1)
// Space: O(N)

import java.util.Collections;
import java.util.PriorityQueue;

class MedianFinder {
    private final PriorityQueue<Integer> maxHeap;
    private final PriorityQueue<Integer> minHeap;

    public MedianFinder() {
        maxHeap = new PriorityQueue<>(Collections.reverseOrder());
        minHeap = new PriorityQueue<>();
    }

    public void addNum(int num) {
        if (maxHeap.isEmpty() || num <= maxHeap.peek()) {
            maxHeap.offer(num);
        } else {
            minHeap.offer(num);
        }

        // Rebalance: maxHeap size is equal to minHeap or 1 greater
        if (maxHeap.size() > minHeap.size() + 1) {
            minHeap.offer(maxHeap.poll());
        } else if (minHeap.size() > maxHeap.size()) {
            maxHeap.offer(minHeap.poll());
        }
    }

    public double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return maxHeap.peek();
        }
        return (maxHeap.peek() + minHeap.peek()) / 2.0;
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addNum: O(log N), findMedian: O(1)
// Space: O(N)

class BinaryHeap {
    private data: number[] = [];
    private compare: (a: number, b: number) => boolean;

    constructor(compare: (a: number, b: number) => boolean) {
        this.compare = compare;
    }

    push(val: number): void {
        this.data.push(val);
        this.bubbleUp(this.data.length - 1);
    }

    pop(): number | undefined {
        if (this.data.length === 0) return undefined;
        const top = this.data[0];
        const last = this.data.pop()!;
        if (this.data.length > 0) {
            this.data[0] = last;
            this.bubbleDown(0);
        }
        return top;
    }

    peek(): number | undefined {
        return this.data[0];
    }

    size(): number {
        return this.data.length;
    }

    private bubbleUp(idx: number): void {
        while (idx > 0) {
            const parent = Math.floor((idx - 1) / 2);
            if (this.compare(this.data[idx], this.data[parent])) {
                [this.data[idx], this.data[parent]] = [this.data[parent], this.data[idx]];
                idx = parent;
            } else {
                break;
            }
        }
    }

    private bubbleDown(idx: number): void {
        const len = this.data.length;
        while (true) {
            let target = idx;
            const left = 2 * idx + 1;
            const right = 2 * idx + 2;

            if (left < len && this.compare(this.data[left], this.data[target])) {
                target = left;
            }
            if (right < len && this.compare(this.data[right], this.data[target])) {
                target = right;
            }
            if (target !== idx) {
                [this.data[idx], this.data[target]] = [this.data[target], this.data[idx]];
                idx = target;
            } else {
                break;
            }
        }
    }
}

class MedianFinder {
    private maxHeap: BinaryHeap;
    private minHeap: BinaryHeap;

    constructor() {
        this.maxHeap = new BinaryHeap((a, b) => a > b);
        this.minHeap = new BinaryHeap((a, b) => a < b);
    }

    addNum(num: number): void {
        if (this.maxHeap.size() === 0 || num <= this.maxHeap.peek()!) {
            this.maxHeap.push(num);
        } else {
            this.minHeap.push(num);
        }

        if (this.maxHeap.size() > this.minHeap.size() + 1) {
            this.minHeap.push(this.maxHeap.pop()!);
        } else if (this.minHeap.size() > this.maxHeap.size()) {
            this.maxHeap.push(this.minHeap.pop()!);
        }
    }

    findMedian(): number {
        if (this.maxHeap.size() > this.minHeap.size()) {
            return this.maxHeap.peek()!;
        }
        return (this.maxHeap.peek()! + this.minHeap.peek()!) / 2;
    }
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addNum: O(log N), findMedian: O(1)
// Space: O(N)

package main

import (
	"container/heap"
)

type MinHeap []int

func (h MinHeap) Len() int           { return len(h) }
func (h MinHeap) Less(i, j int) bool { return h[i] < h[j] }
func (h MinHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *MinHeap) Push(x any)        { *h = append(*h, x.(int)) }
func (h *MinHeap) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[0 : n-1]
	return x
}

type MaxHeap []int

func (h MaxHeap) Len() int           { return len(h) }
func (h MaxHeap) Less(i, j int) bool { return h[i] > h[j] }
func (h MaxHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *MaxHeap) Push(x any)        { *h = append(*h, x.(int)) }
func (h *MaxHeap) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[0 : n-1]
	return x
}

type MedianFinder struct {
	maxHeap *MaxHeap
	minHeap *MinHeap
}

func Constructor() MedianFinder {
	maxH := &MaxHeap{}
	minH := &MinHeap{}
	heap.Init(maxH)
	heap.Init(minH)
	return MedianFinder{
		maxHeap: maxH,
		minHeap: minH,
	}
}

func (this *MedianFinder) AddNum(num int) {
	if this.maxHeap.Len() == 0 || num <= (*this.maxHeap)[0] {
		heap.Push(this.maxHeap, num)
	} else {
		heap.Push(this.minHeap, num)
	}

	if this.maxHeap.Len() > this.minHeap.Len()+1 {
		val := heap.Pop(this.maxHeap).(int)
		heap.Push(this.minHeap, val)
	} else if this.minHeap.Len() > this.maxHeap.Len() {
		val := heap.Pop(this.minHeap).(int)
		heap.Push(this.maxHeap, val)
	}
}

func (this *MedianFinder) FindMedian() float64 {
	if this.maxHeap.Len() > this.minHeap.Len() {
		return float64((*this.maxHeap)[0])
	}
	return float64((*this.maxHeap)[0]+(*this.minHeap)[0]) / 2.0
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  add_num: O(log N), find_median: O(1)
// Space: O(N)

use std::cmp::Reverse;
use std::collections::BinaryHeap;

pub struct MedianFinder {
    max_heap: BinaryHeap<i32>,
    min_heap: BinaryHeap<Reverse<i32>>,
}

impl MedianFinder {
    pub fn new() -> Self {
        MedianFinder {
            max_heap: BinaryHeap::new(),
            min_heap: BinaryHeap::new(),
        }
    }

    pub fn add_num(&mut self, num: i32) {
        if self.max_heap.is_empty() || num <= *self.max_heap.peek().unwrap() {
            self.max_heap.push(num);
        } else {
            self.min_heap.push(Reverse(num));
        }

        if self.max_heap.len() > self.min_heap.len() + 1 {
            let val = self.max_heap.pop().unwrap();
            self.min_heap.push(Reverse(val));
        } else if self.min_heap.len() > self.max_heap.len() {
            let Reverse(val) = self.min_heap.pop().unwrap();
            self.max_heap.push(val);
        }
    }

    pub fn find_median(&self) -> f64 {
        if self.max_heap.len() > self.min_heap.len() {
            *self.max_heap.peek().unwrap() as f64
        } else {
            let top_max = *self.max_heap.peek().unwrap() as f64;
            let Reverse(top_min) = *self.min_heap.peek().unwrap();
            (top_max + top_min as f64) / 2.0
        }
    }
}
```

---

## 4. Tier 2: Self-Balancing Binary Search Tree (Two Multisets)

### 4.1 Mechanical Description
Instead of binary heaps, insert elements into a self-balancing binary search tree (such as `std::multiset` in C++).
Alternatively, maintain a single multiset along with one or two iterators pointing directly to the median element(s).
When a new element is inserted, update the iterators conditionally based on whether the inserted element is smaller or larger than the current median.

### 4.2 Trade-offs
- Supports deletions of arbitrary elements if required (unlike pure binary heaps).
- Node-based structures induce higher memory allocation overhead and poor CPU cache locality compared to array-backed heaps.

---

## 5. Tier 3: Binary Search with Insertion Sort

### 5.1 Mechanical Description
Maintain a single dynamic array sorted in ascending order.
When `addNum(num)` is called, use binary search (`std::lower_bound` or `bisect.insort`) to identify the insertion index in $O(\log N)$ time.
Shift following elements rightward to insert `num` in $O(N)$ time.
`findMedian()` directly accesses `arr[N / 2]` in $O(1)$ time.

### 5.2 Trade-offs
- Memory is strictly contiguous, giving optimal CPU cache locality.
- Shifting elements takes $O(N)$ time per insertion, yielding an overall quadratic $O(N^2)$ runtime for $N$ stream operations.

---

## 6. Tier 4: Unsorted Buffer with Sort-on-Demand (Brute Force Baseline)

### 6.1 Mechanical Description
Store all numbers in an unsorted list.
On every `findMedian()` call, sort the list in $O(N \log N)$ time and return the median element.

### 6.2 Trade-offs
- Fast $O(1)$ insertions.
- Unacceptable $O(N \log N)$ cost per query; times out under repeated calls.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Array-Backed Binary Heaps**: Both `max_heap` and `min_heap` are stored as flat dynamic arrays (`std::vector` in C++, `Vec` in Rust), guaranteeing contiguous memory layout and high cache locality.
2. **Integer Overflow Prevention**: When computing `(max_heap.top() + min_heap.top()) / 2.0`, cast to `double` or `int64_t` first to prevent 32-bit signed integer overflow when both numbers are large positive numbers (e.g., $10^5 + 10^5$).
3. **Follow-Up 1 Optimization ($0 \le \text{num} \le 100$)**:
   Use a fixed frequency array of size 101 and a total count variable. `addNum` increments `count[num]` in $O(1)$ time. `findMedian` scans prefix sums up to $N/2$ in $O(100) = O(1)$ time.
4. **Follow-Up 2 Optimization ($99\%$ in $[0, 100]$)**:
   Use a frequency array of size 101 for $[0, 100]$, and two heaps or BSTs for numbers $< 0$ and $> 100$. If the median falls in $[0, 100]$, retrieve it in $O(100)$ operations; otherwise inspect the outlier structures.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| First element query | `addNum(5)`, `findMedian()` | Returns `5.0` | `max_heap.size() == 1, min_heap.size() == 0`, returns `max_heap.top()` |
| Duplicate numbers | `addNum(2), addNum(2), addNum(2)` | Returns `2.0` | Heaps cleanly handle equal elements |
| Negative numbers | `nums = [-1, -2, -3]` | Returns `-2.0` | Proper sign preservation in comparisons |
| Large stream | $5 \times 10^4$ operations | Executes within 50ms | $O(\log N)$ logarithmic heap updates |
| Integer addition overflow | Two large values near $10^5$ | No overflow | Arithmetic performed as floating point `double` |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why do we keep `max_heap.size() >= min_heap.size()` instead of vice versa?
It is a stylistic convention. Keeping `max_heap` with equal or one more element means the median for an odd total count is always simply `max_heap.top()`.

### 2. Can we use Quickselect to find the median in $O(N)$?
Quickselect works on a static array in $O(N)$ average time. However, for a continuous data stream with frequent queries, Two Heaps provides $O(1)$ queries and $O(\log N)$ insertions, which is much faster.

### 3. Why does Python's `heapq` require negation for max-heap?
Python's standard `heapq` only provides a min-heap implementation. Pushing `-num` inverts the ordering, effectively producing a max-heap.

### 4. What happens when the stream contains $10^9$ numbers?
If the stream cannot fit in RAM, an approximate median streaming algorithm such as Greenwald-Khanna or t-digest is utilized.

### 5. What is the time complexity of building the heaps?
Inserting $N$ elements one by one takes $O(N \log N)$ total time across the entire stream.

### 6. Can `std::priority_queue` remove an arbitrary element?
No, standard heaps do not support arbitrary deletion in $O(\log N)$ without maintaining external index handles (indexed priority queue) or lazy removal.

### 7. How does the Two Heaps approach extend to arbitrary percentiles?
For any percentile $p \in (0, 1)$, maintain two heaps such that $|\text{lower\_heap}| / (|\text{lower\_heap}| + |\text{upper\_heap}|) \approx p$.

### 8. Why is `std::multiset` slower than `std::priority_queue` in C++?
`std::multiset` is a node-based red-black tree requiring dynamic heap allocations per node and pointer chasing, whereas `std::priority_queue` is backed by a flat contiguous `std::vector`.

### 9. What is the precision required for `findMedian`?
LeetCode accepts solutions within $10^{-5}$ of the true floating-point median.

### 10. How does Rust's `BinaryHeap` implement a min-heap?
Rust provides `std::cmp::Reverse<T>`, which reverses the natural ordering of `T`, enabling `BinaryHeap<Reverse<T>>` to function as a min-heap.

---

## 10. Related Problems and Systematic Progression Links

- [[0004-Median-of-Two-Sorted-Arrays]]: Binary search partition median on two static arrays.
- [[0023-Merge-k-Sorted-Lists]]: Priority queue k-way stream merging.
- [[0217-Contains-Duplicate]]: Stream membership testing.
- [[0347-Top-K-Frequent-Elements]]: Heap-based frequency thresholding.
- LeetCode 480 (Sliding Window Median): Median tracking over a sliding window with element removals.
- LeetCode 1825 (Finding MK Average): Multi-heap data stream bounded average calculation.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/find-median-from-data-stream.cpp)
- [Python Implementation](../Python/find-median-from-data-stream.py)
- [Java Implementation](../Java/find-median-from-data-stream.java)
- [TypeScript Implementation](../TypeScript/find-median-from-data-stream.ts)
- [Go Implementation](../Golang/find-median-from-data-stream.go)
- [Rust Implementation](../Rust/find-median-from-data-stream.rs)

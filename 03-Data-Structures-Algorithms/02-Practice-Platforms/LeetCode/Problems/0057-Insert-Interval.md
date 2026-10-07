---
id: leetcode-0057-insert-interval
title: "LeetCode 0057: Insert Interval"
tags:
  - dsa
  - leetcode
  - array
  - intervals
  - binary-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/insert-interval/"
---

# LeetCode 0057: Insert Interval

## 1. Problem Formalization and Constraints

You are given an array of non-overlapping intervals `intervals` where `intervals[i] = [start_i, end_i]` represent the start and the end of the $i$-th interval, and `intervals` is sorted in ascending order by `start_i`.
You are also given an interval `newInterval = [start, end]` that represents the start and end of another interval.

Insert `newInterval` into `intervals` such that `intervals` is still sorted in ascending order by `start_i` and `intervals` still does not have any overlapping intervals (merge overlapping intervals if necessary).
Return `intervals` after the insertion.
Note that you don't need to modify `intervals` in-place. You can make a new array and return it.

### Constraints
- $0 \le \text{intervals.length} \le 10^4$
- $\text{intervals}[i]\text{.length} == 2$
- $0 \le \text{start}_i \le \text{end}_i \le 10^5$
- `intervals` is sorted by `start_i` in ascending order.
- `newInterval.length == 2`
- $0 \le \text{start} \le \text{end} \le 10^5$

### Examples
- **Example 1**:
  - Input: `intervals = [[1,3],[6,9]]`, `newInterval = [2,5]`
  - Output: `[[1,5],[6,9]]`
- **Example 2**:
  - Input: `intervals = [[1,2],[3,5],[6,7],[8,10],[12,16]]`, `newInterval = [4,8]`
  - Output: `[[1,2],[3,10],[12,16]]`
  - Explanation: Because the new interval `[4,8]` overlaps with `[3,5],[6,7],[8,10]`.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Three-Phase Linear Sweep | $O(N)$ | $O(N)$ output | 1. Append strictly left disjoint intervals; 2. Merge overlapping intervals into `newInterval`; 3. Append strictly right disjoint intervals. |
| **Tier 2 (Binary Search)** | Binary Search Bounds + Array Slice | $O(\log N + N)$ | $O(N)$ output | Uses binary search `bisect` to find first overlapping and last overlapping indices; still requires $O(N)$ to copy disjoint intervals. |
| **Tier 3 (Append & Sort)** | Insert and Merge All Intervals | $O(N \log N)$ | $O(N)$ output | Appends `newInterval` to `intervals`, sorts by start time, and runs standard interval merge; simple reuse of LeetCode 56, but suboptimal $O(N \log N)$. |
| **Tier 4 (In-Place Mutation)** | In-Place Overwriting & Erasing | $O(N^2)$ worst-case | $O(1)$ auxiliary | Modifies the input vector directly; replaces overlapping intervals and erases gaps; vector `erase` causes quadratic memory shifts. |

---

## 3. Tier 1: Most Optimal Solution (Three-Phase Linear Sweep)

### 3.1 Algorithmic Mechanics and Invariant Proof

Because `intervals` is already sorted and non-overlapping, any insertion partitions the input into three disjoint temporal phases:
1. **Phase 1: Preceding Disjoint Intervals**:
   While $i < N$ and $\text{intervals}[i].\text{end} < \text{newInterval}.\text{start}$, $\text{intervals}[i]$ lies strictly before `newInterval`.
   Append $\text{intervals}[i]$ directly to `result`.
2. **Phase 2: Overlapping Region Merge**:
   While $i < N$ and $\text{intervals}[i].\text{start} \le \text{newInterval}.\text{end}$, $\text{intervals}[i]$ overlaps with the expanding `newInterval`.
   Expand `newInterval`:
   $$\text{newInterval}.\text{start} = \min(\text{newInterval}.\text{start}, \text{intervals}[i].\text{start})$$
   $$\text{newInterval}.\text{end} = \max(\text{newInterval}.\text{end}, \text{intervals}[i].\text{end})$$
   Advance $i$.
   Once the loop terminates, append the unified `newInterval` to `result`.
3. **Phase 3: Succeeding Disjoint Intervals**:
   While $i < N$, $\text{intervals}[i]$ lies strictly after the merged interval.
   Append $\text{intervals}[i]$ to `result`.

**Invariant Proof**:
Let the input intervals be mutually disjoint and sorted.
Any interval $I$ satisfies exactly one of three mutually exclusive relations with a target interval $T$:
1. $I$ is strictly to the left: $I.\text{end} < T.\text{start}$.
2. $I$ overlaps with $T$: $I.\text{start} \le T.\text{end}$ and $I.\text{end} \ge T.\text{start}$.
3. $I$ is strictly to the right: $I.\text{start} > T.\text{end}$.
Because intervals are pre-sorted by start time and mutually disjoint, all Phase 1 intervals strictly precede any overlapping intervals.
During Phase 2, each overlapping interval is merged into $T$ via union of intervals: $[ \min(T.\text{start}, I.\text{start}), \max(T.\text{end}, I.\text{end}) ]$.
By induction, $T$ remains a single contiguous interval covering the union of itself and all processed overlapping intervals.
Once an interval satisfies $I.\text{start} > T.\text{end}$, all subsequent intervals also satisfy this condition due to sorted order.
Therefore, the resulting sequence is strictly sorted and non-overlapping.
Since every input interval is examined exactly once, correctness is guaranteed in $O(N)$ time.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ where $N$ is the length of `intervals`. Every interval is processed once.
- **Auxiliary Space Complexity**: $O(N)$ space required to allocate the returned array of merged intervals ($O(1)$ auxiliary memory beyond the output).

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N) for output

#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
        vector<vector<int>> result;
        size_t i = 0;
        const size_t n = intervals.size();

        // 1. Add intervals ending before newInterval starts
        while (i < n && intervals[i][1] < newInterval[0]) {
            result.push_back(intervals[i]);
            ++i;
        }

        // 2. Merge overlapping intervals
        while (i < n && intervals[i][0] <= newInterval[1]) {
            newInterval[0] = min(newInterval[0], intervals[i][0]);
            newInterval[1] = max(newInterval[1], intervals[i][1]);
            ++i;
        }
        result.push_back(newInterval);

        // 3. Add remaining intervals
        while (i < n) {
            result.push_back(intervals[i]);
            ++i;
        }

        return result;
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N)
# Space: O(N) for output

from typing import List

class Solution:
    def insert(self, intervals: List[List[int]], newInterval: List[int]) -> List[List[int]]:
        result = []
        i = 0
        n = len(intervals)

        # 1. Add intervals ending before newInterval starts
        while i < n and intervals[i][1] < newInterval[0]:
            result.append(intervals[i])
            i += 1

        # 2. Merge overlapping intervals
        while i < n and intervals[i][0] <= newInterval[1]:
            newInterval[0] = min(newInterval[0], intervals[i][0])
            newInterval[1] = max(newInterval[1], intervals[i][1])
            i += 1
        result.append(newInterval)

        # 3. Add remaining intervals
        while i < n:
            result.append(intervals[i])
            i += 1

        return result
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N) for output list

import java.util.ArrayList;
import java.util.List;

class Solution {
    public int[][] insert(int[][] intervals, int[] newInterval) {
        List<int[]> result = new ArrayList<>();
        int i = 0;
        int n = intervals.length;

        // 1. Add intervals ending before newInterval starts
        while (i < n && intervals[i][1] < newInterval[0]) {
            result.add(intervals[i]);
            i++;
        }

        // 2. Merge overlapping intervals
        while (i < n && intervals[i][0] <= newInterval[1]) {
            newInterval[0] = Math.min(newInterval[0], intervals[i][0]);
            newInterval[1] = Math.max(newInterval[1], intervals[i][1]);
            i++;
        }
        result.add(newInterval);

        // 3. Add remaining intervals
        while (i < n) {
            result.add(intervals[i]);
            i++;
        }

        return result.toArray(new int[result.size()][]);
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
// Space: O(N)

function insert(intervals: number[][], newInterval: number[]): number[][] {
    const result: number[][] = [];
    let i = 0;
    const n = intervals.length;

    // 1. Add intervals ending before newInterval starts
    while (i < n && intervals[i][1] < newInterval[0]) {
        result.push(intervals[i]);
        i++;
    }

    // 2. Merge overlapping intervals
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = Math.min(newInterval[0], intervals[i][0]);
        newInterval[1] = Math.max(newInterval[1], intervals[i][1]);
        i++;
    }
    result.push(newInterval);

    // 3. Add remaining intervals
    while (i < n) {
        result.push(intervals[i]);
        i++;
    }

    return result;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

package main

func insert(intervals [][]int, newInterval []int) [][]int {
	result := make([][]int, 0, len(intervals)+1)
	i := 0
	n := len(intervals)

	// 1. Add intervals ending before newInterval starts
	for i < n && intervals[i][1] < newInterval[0] {
		result = append(result, intervals[i])
		i++
	}

	// 2. Merge overlapping intervals
	for i < n && intervals[i][0] <= newInterval[1] {
		if intervals[i][0] < newInterval[0] {
			newInterval[0] = intervals[i][0]
		}
		if intervals[i][1] > newInterval[1] {
			newInterval[1] = intervals[i][1]
		}
		i++
	}
	result = append(result, newInterval)

	// 3. Add remaining intervals
	for i < n {
		result = append(result, intervals[i])
		i++
	}

	return result
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

pub struct Solution;

impl Solution {
    pub fn insert(intervals: Vec<Vec<i32>>, mut new_interval: Vec<i32>) -> Vec<Vec<i32>> {
        let mut result = Vec::with_capacity(intervals.len() + 1);
        let mut i = 0;
        let n = intervals.len();

        // 1. Add intervals ending before new_interval starts
        while i < n && intervals[i][1] < new_interval[0] {
            result.push(intervals[i].clone());
            i += 1;
        }

        // 2. Merge overlapping intervals
        while i < n && intervals[i][0] <= new_interval[1] {
            new_interval[0] = new_interval[0].min(intervals[i][0]);
            new_interval[1] = new_interval[1].max(intervals[i][1]);
            i += 1;
        }
        result.push(new_interval);

        // 3. Add remaining intervals
        while i < n {
            result.push(intervals[i].clone());
            i += 1;
        }

        result
    }
}
```

---

## 4. Tier 2: Binary Search for Overlapping Boundaries

### 4.1 Mechanical Description
Use binary search (`std::lower_bound`) to locate the first interval where $\text{intervals}[i].\text{end} \ge \text{newInterval}.\text{start}$.
Use binary search (`std::upper_bound`) to locate the last interval where $\text{intervals}[j].\text{start} \le \text{newInterval}.\text{end}$.
Construct the output by copying prefix $0 \dots i-1$, merging $i \dots j$, and copying suffix $j+1 \dots N-1$.

### 4.2 Trade-offs
- Finds the overlapping indices in $O(\log N)$ time.
- Still requires $O(N)$ time to construct and copy the output list.

---

## 5. Tier 3: Append and Sort Pipeline (LeetCode 56 Reuse)

### 5.1 Mechanical Description
Append `newInterval` directly into `intervals`.
Sort the array of $N + 1$ intervals by start time.
Run the standard Merge Intervals algorithm from LeetCode 56.

### 5.2 Trade-offs
- Reuses existing tested code.
- Takes $O(N \log N)$ time, disregarding the existing sorted property of the input.

---

## 6. Tier 4: In-Place Mutation with Element Shifting

### 6.1 Mechanical Description
Mutate the existing dynamic array by updating overlapping intervals in-place and calling `erase()` on redundant slots.

### 6.2 Trade-offs
- Consumes $O(1)$ auxiliary space.
- Vector `erase` operations incur $O(N^2)$ quadratic memory movement in the worst case.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Pre-Allocation**: Allocating `result` with capacity $N + 1$ (`Vec::with_capacity(N + 1)`) avoids dynamic heap reallocations.
2. **Contiguous Vector Copies**: Slices of non-overlapping intervals can be copied using contiguous memory copy primitives (`memcpy` or Rust slice cloning).
3. **Branch Elimination**: The three contiguous while loops execute sequentially without back-tracking, ensuring deterministic branch prediction.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Empty input intervals | `intervals = []`, `new = [5, 7]` | Returns `[[5, 7]]` | Loops 1 and 3 are skipped; returns single interval |
| Insert before all | `intervals = [[3, 5]]`, `new = [1, 2]` | Returns `[[1, 2], [3, 5]]` | Merge loop skipped; appended before remaining |
| Insert after all | `intervals = [[1, 2]]`, `new = [3, 5]` | Returns `[[1, 2], [3, 5]]` | Phase 1 consumes all; merged interval appended last |
| Absorbs all intervals | `intervals = [[2, 3], [4, 5]]`, `new = [1, 10]` | Returns `[[1, 10]]` | Phase 2 merges all intervals into `[1, 10]` |
| Touching endpoints | `intervals = [[1, 2], [3, 5]]`, `new = [2, 3]` | Merges touching endpoints into `[[1, 5]]` | Condition `intervals[i][0] <= newInterval[1]` handles touching |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What is the fundamental difference between LeetCode 56 and LeetCode 57?
LeetCode 56 receives an unsorted array of potentially overlapping intervals, requiring $O(N \log N)$ sorting. LeetCode 57 guarantees that existing intervals are already sorted and non-overlapping, enabling an $O(N)$ linear insertion.

### 2. Can touching intervals be merged?
Yes. For example, `[1, 2]` and `[2, 3]` touch at `2` and merge into `[1, 3]`. The condition `intervals[i][0] <= newInterval[1]` correctly merges them.

### 3. Does binary search make the solution $O(\log N)$ overall?
No, because creating the output array or modifying the list still requires copying $O(N)$ elements in the worst case.

### 4. What happens if `intervals` is empty?
The loops checking $i < N$ will not execute; `result` will simply contain `newInterval`, returning `[newInterval]`.

### 5. Why do we update `newInterval` directly during Phase 2?
Because each overlapping interval can extend `newInterval` to the left or right, maintaining a running bounding interval.

### 6. Can an in-place $O(1)$ auxiliary space solution be achieved in $O(N)$ time?
In a singly linked list, yes. In a contiguous array, removing overlapping elements requires shifting subsequent elements, leading to $O(N^2)$ worst-case shifts.

### 7. What is the maximum size of `intervals`?
$N \le 10^4$, meaning the linear scan runs in under 1 millisecond.

### 8. What data structure would support $O(\log N)$ dynamic insertions and queries?
An Interval Tree or Segment Tree can insert and query intervals in $O(\log N)$ time.

### 9. Why does Phase 1 check `intervals[i][1] < newInterval[0]`?
Because an interval is strictly to the left if and only if its end is strictly less than the new interval's start.

### 10. Does Rust require cloning inner vectors?
Yes, `result.push(intervals[i].clone())` copies the 2-element vector unless consuming the input vector by value.

---

## 10. Related Problems and Systematic Progression Links

- [[0056-Merge-Intervals]]: General unsorted interval merging in $O(N \log N)$ time.
- [[0435-Non-overlapping-Intervals]]: Greedy activity selection and minimum interval removals.
- LeetCode 252 (Meeting Rooms): Interval overlap detection.
- LeetCode 253 (Meeting Rooms II): Multi-interval overlap point finding.
- LeetCode 715 (Range Module): Dynamic range tracking and interval operations.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/insert-interval.cpp)
- [Python Implementation](../Python/insert-interval.py)
- [Java Implementation](../Java/insert-interval.java)
- [TypeScript Implementation](../TypeScript/insert-interval.ts)
- [Go Implementation](../Golang/insert-interval.go)
- [Rust Implementation](../Rust/insert-interval.rs)

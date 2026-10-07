---
id: leetcode-0435-non-overlapping-intervals
title: "LeetCode 0435: Non-overlapping Intervals"
tags:
  - dsa
  - leetcode
  - intervals
  - greedy
  - sorting
  - dynamic-programming
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/non-overlapping-intervals/"
---

# LeetCode 0435: Non-overlapping Intervals

## 1. Problem Formalization and Constraints

Given an array of intervals `intervals` where `intervals[i] = [start_i, end_i]`, return the minimum number of intervals you need to remove to make the rest of the intervals non-overlapping.
Note that intervals which touch at a point are non-overlapping.
For example, `[1, 2]` and `[2, 3]` are non-overlapping.

### Constraints
- $1 \le \text{intervals.length} \le 10^5$
- $\text{intervals}[i]\text{.length} == 2$
- $-5 \times 10^4 \le \text{start}_i < \text{end}_i \le 5 \times 10^4$

### Examples
- **Example 1**:
  - Input: `intervals = [[1,2],[2,3],[3,4],[1,3]]`
  - Output: `1`
  - Explanation: `[1,3]` can be removed and the rest of the intervals are non-overlapping.
- **Example 2**:
  - Input: `intervals = [[1,2],[1,2],[1,2]]`
  - Output: `2`
  - Explanation: You need to remove two `[1,2]` to make the rest of the intervals non-overlapping.
- **Example 3**:
  - Input: `intervals = [[1,2],[2,3]]`
  - Output: `0`
  - Explanation: You do not need to remove any of the intervals since they're already non-overlapping.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Greedy Activity Selection (Sort by End Time) | $O(N \log N)$ | $O(1)$ auxiliary | Earliest Deadline First: sorts intervals by `end_i`; greedily keeps interval that frees resources earliest; optimal $O(1)$ extra space. |
| **Tier 2 (Greedy Start Sort)** | Greedy with Start Time Sort & Min End Tracking | $O(N \log N)$ | $O(1)$ auxiliary | Sorts by `start_i`; on overlap, discards the interval with larger end point (`prev_end = min(prev_end, curr_end)`); slightly more branch logic. |
| **Tier 3 (Dynamic Programming)** | Longest Compatible Subsequence DP | $O(N^2)$ | $O(N)$ total | Sorts intervals and computes maximum non-overlapping set via LIS-style DP ($dp[i] = 1 + \max(dp[j])$); quadratic time is too slow for $N = 10^5$. |
| **Tier 4 (Brute Force)** | Recursive Power Set Search | $O(2^N)$ | $O(N)$ stack | Toggles inclusion/exclusion for every interval and verifies non-overlap validity; completely intractable beyond $N \approx 20$. |

---

## 3. Tier 1: Most Optimal Solution (Greedy Activity Selection)

### 3.1 Algorithmic Mechanics and Invariant Proof

Minimizing the number of removed intervals is mathematically dual to maximizing the number of mutually compatible, non-overlapping intervals (the classic Activity Selection Problem).
If the maximum size of a non-overlapping set of intervals is $K$, then the minimum number of intervals to remove is $N - K$.

**Greedy Choice Rule**:
Sort all intervals in ascending order of their end times:
$$\text{intervals} = \langle I_1, I_2, \dots, I_N \rangle \quad \text{where } I_k.\text{end} \le I_{k+1}.\text{end}$$
Iterate through the sorted intervals:
1. Initialize `prev_end = intervals[0].end` and `removals = 0`.
2. For each subsequent interval $I_i$:
   - If $I_i.\text{start} < \text{prev\_end}$: An overlap occurs.
     Because all prior non-overlapping intervals selected have ends $\le \text{prev\_end} \le I_i.\text{end}$, keeping the earlier-ending interval leaves more room for subsequent intervals.
     Therefore, discard $I_i$ and increment `removals += 1`.
   - If $I_i.\text{start} \ge \text{prev\_end}$: No overlap.
     Retain $I_i$ and update `prev_end = I_i.end`.

**Invariant Proof (Exchange Argument)**:
Let $S = \langle a_1, a_2, \dots, a_k \rangle$ be the sequence of intervals chosen by our greedy strategy, sorted by end time.
Let $O = \langle o_1, o_2, \dots, o_m \rangle$ be an optimal solution with maximum size $m \ge k$.
If $a_1 \neq o_1$, because our algorithm picks the interval with the minimum possible end time among all available candidates, $a_1.\text{end} \le o_1.\text{end}$.
Replacing $o_1$ with $a_1$ in $O$ yields a valid non-overlapping set because $a_1$ ends before $o_1$ ends, so $a_1$ cannot conflict with $o_2, o_3, \dots, o_m$.
Continuing by induction, we can transform $O$ into $S$ without decreasing the number of selected intervals.
Hence, $k = m$, proving that the greedy choice achieves the maximum number of compatible intervals, and therefore minimizes removals.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$ dominated by sorting $N$ intervals. The subsequent linear scan executes in $O(N)$ time.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space when using an in-place sort algorithm (`std::sort` in C++, `sort_unstable_by_key` in Rust).

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) auxiliary

#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int eraseOverlapIntervals(vector<vector<int>>& intervals) {
        if (intervals.empty()) {
            return 0;
        }

        // Sort intervals by end time ascending
        sort(intervals.begin(), intervals.end(), [](const vector<int>& a, const vector<int>& b) {
            return a[1] < b[1];
        });

        int removals = 0;
        int prev_end = intervals[0][1];

        for (size_t i = 1; i < intervals.size(); ++i) {
            if (intervals[i][0] < prev_end) {
                ++removals;
            } else {
                prev_end = intervals[i][1];
            }
        }

        return removals;
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N log N)
# Space: O(1) or O(N) depending on Timsort

from typing import List

class Solution:
    def eraseOverlapIntervals(self, intervals: List[List[int]]) -> int:
        if not intervals:
            return 0

        # Sort intervals by end time ascending
        intervals.sort(key=lambda x: x[1])

        removals = 0
        prev_end = intervals[0][1]

        for i in range(1, len(intervals)):
            if intervals[i][0] < prev_end:
                removals += 1
            else:
                prev_end = intervals[i][1]

        return removals
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) or O(N) auxiliary

import java.util.Arrays;
import java.util.Comparator;

class Solution {
    public int eraseOverlapIntervals(int[][] intervals) {
        if (intervals == null || intervals.length == 0) {
            return 0;
        }

        // Sort by end time ascending
        Arrays.sort(intervals, Comparator.comparingInt(a -> a[1]));

        int removals = 0;
        int prevEnd = intervals[0][1];

        for (int i = 1; i < intervals.length; i++) {
            if (intervals[i][0] < prevEnd) {
                removals++;
            } else {
                prevEnd = intervals[i][1];
            }
        }

        return removals;
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) or O(N) auxiliary

function eraseOverlapIntervals(intervals: number[][]): number {
    if (intervals.length === 0) {
        return 0;
    }

    // Sort by end time ascending
    intervals.sort((a, b) => a[1] - b[1]);

    let removals = 0;
    let prevEnd = intervals[0][1];

    for (let i = 1; i < intervals.length; i++) {
        if (intervals[i][0] < prevEnd) {
            removals++;
        } else {
            prevEnd = intervals[i][1];
        }
    }

    return removals;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) auxiliary

package main

import "sort"

func eraseOverlapIntervals(intervals [][]int) int {
	if len(intervals) == 0 {
		return 0
	}

	sort.Slice(intervals, func(i, j int) bool {
		return intervals[i][1] < intervals[j][1]
	})

	removals := 0
	prevEnd := intervals[0][1]

	for i := 1; i < len(intervals); i++ {
		if intervals[i][0] < prevEnd {
			removals++
		} else {
			prevEnd = intervals[i][1]
		}
	}

	return removals
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) auxiliary

pub struct Solution;

impl Solution {
    pub fn erase_overlap_intervals(mut intervals: Vec<Vec<i32>>) -> i32 {
        if intervals.is_empty() {
            return 0;
        }

        intervals.sort_unstable_by_key(|k| k[1]);

        let mut removals = 0;
        let mut prev_end = intervals[0][1];

        for interval in intervals.iter().skip(1) {
            if interval[0] < prev_end {
                removals += 1;
            } else {
                prev_end = interval[1];
            }
        }

        removals
    }
}
```

---

## 4. Tier 2: Greedy with Start Time Sort & Minimum End Tracking

### 4.1 Mechanical Description
Sort intervals by start time ascending.
Maintain `prev_end = intervals[0][1]`.
When comparing with `curr = intervals[i]`:
- If `curr[0] < prev_end`: There is an overlap.
  Increment `removals += 1`.
  Update `prev_end = min(prev_end, curr[1])`, effectively removing whichever interval extends further to the right.
- If `curr[0] >= prev_end`: No overlap.
  Update `prev_end = curr[1]`.

### 4.2 Trade-offs
- Same asymptotic $O(N \log N)$ complexity.
- Requires updating `prev_end` conditionally via `min()`, making the code slightly more complex than sorting directly by end time.

---

## 5. Tier 3: Longest Compatible Subsequence DP

### 5.1 Mechanical Description
Sort intervals by start time.
Define $dp[i]$ as the maximum number of compatible non-overlapping intervals in the prefix ending with interval $i$.
For each $i \in [0, N-1]$, initialize $dp[i] = 1$.
For all $j < i$, if $intervals[j][1] \le intervals[i][0]$, $dp[i] = \max(dp[i], dp[j] + 1)$.
The minimum removals is $N - \max_{i} dp[i]$.

### 5.2 Trade-offs
- Parallels the classic Longest Increasing Subsequence (LIS) formulation.
- Incurs $O(N^2)$ quadratic runtime; times out for $N = 10^5$. Can be accelerated to $O(N \log N)$ using binary search, but the greedy solution is simpler.

---

## 6. Tier 4: Recursive Power Set Search (Brute Force Baseline)

### 6.1 Mechanical Description
Generate all $2^N$ possible subsets of intervals.
For each subset, verify whether all pairwise intervals are non-overlapping.
Among all valid non-overlapping subsets, find the maximum subset size $K$ and return $N - K$.

### 6.2 Trade-offs
- Exhaustive verification baseline.
- $O(2^N)$ exponential time complexity renders it completely unusable for $N > 20$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **In-Place Primitive Sorting**: Rust's `sort_unstable_by_key` operates without auxiliary heap allocation on contiguous vectors, yielding superior throughput compared to pointer-heavy collections.
2. **Cache-Friendly Scans**: After sorting, the greedy loop only reads two integers per interval sequentially from memory, executing with negligible branch mispredictions.
3. **Dual Problem Formulation**: Solving for max non-overlapping intervals ($K$) vs min removed intervals ($N - K$) is algebraically equivalent and yields identical operations.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single interval | `[[1, 2]]` | Returns `0` removals | Loop starts from index 1; returns `0` |
| Touching endpoints | `[[1, 2], [2, 3]]` | Non-overlapping; returns `0` | `interval[0] >= prev_end` correctly triggers no overlap |
| All identical intervals | `[[1, 2], [1, 2], [1, 2]]` | Retain 1, remove 2 | Evaluates strictly `< prev_end`; removes duplicates |
| Nested intervals | `[[1, 10], [2, 3]]` | Removes `[1, 10]`, keeps `[2, 3]` | End-time sort tests `[2, 3]` before `[1, 10]` |
| Negative coordinates | `[[-10, -5], [-4, 0]]` | Handled properly | Standard integer comparison supports negative numbers |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why do we sort by end time instead of start time?
Sorting by end time guarantees that the first interval chosen leaves the maximum possible remaining time for subsequent intervals, which is the exact greedy choice property.

### 2. Can we sort by start time and still achieve an optimal greedy solution?
Yes. When sorting by start time, on an overlap we greedily drop the interval that ends later by taking `prev_end = min(prev_end, curr_end)`.

### 3. What does "touching at a point are non-overlapping" mean?
An interval `[1, 2]` and `[2, 3]` do not overlap. The check for non-overlap is `curr.start >= prev.end`.

### 4. How does this problem relate to LeetCode 452 (Minimum Number of Arrows to Burst Balloons)?
LeetCode 452 is essentially identical: finding the minimum number of points to pierce all intervals corresponds to finding the number of overlapping clusters. In 452, touching points count as overlapping, whereas in 435 they do not.

### 5. Why is the time complexity $O(N \log N)$?
The bottleneck is sorting the $N$ intervals. The subsequent greedy selection pass is linear $O(N)$.

### 6. Can interval coordinates be negative?
Yes, the constraints allow coordinates from $-5 \times 10^4$ to $5 \times 10^4$.

### 7. Does this problem have a matroid structure?
Yes, the compatible interval subsets form an independent set in an interval greedoid / matroid, which formally explains why the greedy strategy finds the global optimum.

### 8. What is the maximum value of $N$?
$N \le 10^5$, meaning an $O(N \log N)$ algorithm completes in approximately 20ms, while an $O(N^2)$ DP algorithm would require over $10^{10}$ operations and time out.

### 9. Why use `sort_unstable` in Rust?
`sort_unstable_by_key` does not preserve the relative order of equal elements, avoiding extra memory allocation and running faster.

### 10. How does interval merging (LeetCode 56) differ from interval scheduling?
Interval merging combines overlapping intervals into one union. Non-overlapping intervals (interval scheduling) selects a subset of disjoint intervals to maximize set size or minimize removals.

---

## 10. Related Problems and Systematic Progression Links

- [[0056-Merge-Intervals]]: Overlapping interval merging and union computation.
- [[0300-Longest-Increasing-Subsequence]]: Subsequence optimization via dynamic programming and patient sorting.
- LeetCode 252 (Meeting Rooms): Determine if all intervals are disjoint.
- LeetCode 253 (Meeting Rooms II): Minimum rooms (chromatic number of interval graph).
- LeetCode 452 (Minimum Number of Arrows to Burst Balloons): Overlapping interval cluster piercing.
- LeetCode 646 (Maximum Length of Pair Chain): Activity selection with strict inequality.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/non-overlapping-intervals.cpp)
- [Python Implementation](../Python/non-overlapping-intervals.py)
- [Java Implementation](../Java/non-overlapping-intervals.java)
- [TypeScript Implementation](../TypeScript/non-overlapping-intervals.ts)
- [Go Implementation](../Golang/non-overlapping-intervals.go)
- [Rust Implementation](../Rust/non-overlapping-intervals.rs)

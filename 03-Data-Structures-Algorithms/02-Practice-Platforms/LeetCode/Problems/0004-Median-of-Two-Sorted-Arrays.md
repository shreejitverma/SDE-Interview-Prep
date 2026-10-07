---
id: leetcode-0004-median-of-two-sorted-arrays
title: "LeetCode 0004: Median of Two Sorted Arrays"
tags:
  - dsa
  - leetcode
  - binary-search
  - array
  - divide-and-conquer
level: hard
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/median-of-two-sorted-arrays/"
---

# LeetCode 0004: Median of Two Sorted Arrays

## 1. Problem Formalization and Constraints

Given two sorted arrays `nums1` and `nums2` of size $m$ and $n$ respectively, return the median of the two sorted arrays.
The overall run time complexity should be $O(\log(m + n))$ or better.

### Constraints
- $\text{nums1.length} == m$
- $\text{nums2.length} == n$
- $0 \le m \le 1000$
- $0 \le n \le 1000$
- $1 \le m + n \le 2000$
- $-10^6 \le \text{nums1}[i], \text{nums2}[i] \le 10^6$

### Examples
- **Example 1**:
  - Input: `nums1 = [1,3]`, `nums2 = [2]`
  - Output: `2.00000`
  - Explanation: Merged array = `[1,2,3]` and median is `2`.
- **Example 2**:
  - Input: `nums1 = [1,2]`, `nums2 = [3,4]`
  - Output: `2.50000`
  - Explanation: Merged array = `[1,2,3,4]` and median is $(2 + 3) / 2 = 2.5$.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Dual Binary Search on Shorter Array Cut | $O(\log(\min(M, N)))$ | $O(1)$ | Partitions shorter array; determines complementary cut in longer array to ensure equal split and sorted boundary invariants. |
| **Tier 2 (Counting)** | Two-Pointer Virtual Merge Scan | $O((M + N) / 2)$ | $O(1)$ | Traverses both arrays in ascending order up to index $\lfloor (M + N) / 2 \rfloor$ without allocating combined buffer. |
| **Tier 3 (Recursive)** | Recursive $k$-th Element Truncation | $O(\log(M + N))$ | $O(\log(M + N))$ | Compares elements at step $k/2$ across both arrays to discard half of the search candidate pool per recursive step. |
| **Tier 4 (Brute Force)** | Linear Array Merge and Full Indexing | $O(M + N)$ | $O(M + N)$ | Allocates unified buffer of size $M + N$, merges both arrays via standard two pointers, and reads middle element directly. |

---

## 3. Tier 1: Most Optimal Solution (Dual Binary Search on Partition Cut)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $A$ and $B$ denote the two sorted arrays with lengths $m$ and $n$.
Without loss of generality, assume $m \le n$ (swapping the arrays if $m > n$ ensures binary search runs on the smaller range).

We aim to divide the combined set of $m + n$ elements into two equal halves (Left Part and Right Part):
- If $m + n$ is even, Left Part and Right Part have equal size $(m + n) / 2$.
- If $m + n$ is odd, Left Part has $(m + n + 1) / 2$ elements, containing the median.

Let cut $i \in [0, m]$ partition array $A$ into $A[0 \dots i-1]$ and $A[i \dots m-1]$.
The cut in array $B$ is uniquely determined as $j = \lfloor (m + n + 1) / 2 \rfloor - i$.
This guarantees that $|A_{\text{left}}| + |B_{\text{left}}| = \lceil (m + n) / 2 \rceil$.

The partition is valid if and only if every element in the combined Left Part is less than or equal to every element in the combined Right Part.
Because $A$ and $B$ are individually sorted, this requires only two boundary conditions:
1. $\max(A_{\text{left}}) \le \min(B_{\text{right}})$, where $\max(A_{\text{left}}) = A[i-1]$ (or $-\infty$ if $i = 0$) and $\min(B_{\text{right}}) = B[j]$ (or $+\infty$ if $j = n$).
2. $\max(B_{\text{left}}) \le \min(A_{\text{right}})$, where $\max(B_{\text{left}}) = B[j-1]$ (or $-\infty$ if $j = 0$) and $\min(A_{\text{right}}) = A[i]$ (or $+\infty$ if $i = m$).

Binary search on $i \in [0, m]$:
- If $A[i-1] > B[j]$, cut $i$ is too far right; search the left interval $[0, i-1]$.
- If $B[j-1] > A[i]$, cut $i$ is too far left; search the right interval $[i+1, m]$.
- When both conditions hold, the optimal cut is found:
  - If $m + n$ is odd: $\text{Median} = \max(A[i-1], B[j-1])$.
  - If $m + n$ is even: $\text{Median} = \frac{\max(A[i-1], B[j-1]) + \min(A[i], B[j])}{2.0}$.

**Invariant Proof**:
The function $f(i) = A[i-1] - B[j]$ is strictly monotonically increasing with respect to $i$, because as $i$ increases, $A[i-1]$ is non-decreasing while $j$ decreases, making $B[j]$ non-increasing.
Therefore, by the Intermediate Value Theorem on discrete ordered domains, there exists a unique cut $i^*$ satisfying the sorted boundary conditions.
Binary search on the monotonic sequence terminates in $\lceil \log_2(m + 1) \rceil$ iterations, proving strict $O(\log(\min(m, n)))$ runtime and $O(1)$ space.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(\log(\min(M, N)))$. Binary search is performed on the smaller array of length $\min(M, N)$, halving the search space each step.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space. Only a constant number of partition indices and boundary values are stored in CPU registers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>
#include <climits>

class Solution {
public:
    double findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2) {
        if (nums1.size() > nums2.size()) {
            return findMedianSortedArrays(nums2, nums1);
        }

        int m = static_cast<int>(nums1.size());
        int n = static_cast<int>(nums2.size());
        int low = 0, high = m;

        while (low <= high) {
            int partitionX = low + (high - low) / 2;
            int partitionY = (m + n + 1) / 2 - partitionX;

            int maxLeftX = (partitionX == 0) ? INT_MIN : nums1[partitionX - 1];
            int minRightX = (partitionX == m) ? INT_MAX : nums1[partitionX];

            int maxLeftY = (partitionY == 0) ? INT_MIN : nums2[partitionY - 1];
            int minRightY = (partitionY == n) ? INT_MAX : nums2[partitionY];

            if (maxLeftX <= minRightY && maxLeftY <= minRightX) {
                if ((m + n) % 2 != 0) {
                    return static_cast<double>(std::max(maxLeftX, maxLeftY));
                } else {
                    return (static_cast<double>(std::max(maxLeftX, maxLeftY)) +
                            static_cast<double>(std::min(minRightX, minRightY))) / 2.0;
                }
            } else if (maxLeftX > minRightY) {
                high = partitionX - 1;
            } else {
                low = partitionX + 1;
            }
        }

        return 0.0;
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def findMedianSortedArrays(self, nums1: List[int], nums2: List[int]) -> float:
        if len(nums1) > len(nums2):
            return self.findMedianSortedArrays(nums2, nums1)

        m, n = len(nums1), len(nums2)
        low, high = 0, m

        while low <= high:
            partitionX = (low + high) // 2
            partitionY = (m + n + 1) // 2 - partitionX

            maxLeftX = float('-inf') if partitionX == 0 else nums1[partitionX - 1]
            minRightX = float('inf') if partitionX == m else nums1[partitionX]

            maxLeftY = float('-inf') if partitionY == 0 else nums2[partitionY - 1]
            minRightY = float('inf') if partitionY == n else nums2[partitionY]

            if maxLeftX <= minRightY and maxLeftY <= minRightX:
                if (m + n) % 2 != 0:
                    return float(max(maxLeftX, maxLeftY))
                else:
                    return (max(maxLeftX, maxLeftY) + min(minRightX, minRightY)) / 2.0
            elif maxLeftX > minRightY:
                high = partitionX - 1
            else:
                low = partitionX + 1

        return 0.0
```

#### Java 21
```java
public class Solution {
    public double findMedianSortedArrays(int[] nums1, int[] nums2) {
        if (nums1.length > nums2.length) {
            return findMedianSortedArrays(nums2, nums1);
        }

        int m = nums1.length;
        int n = nums2.length;
        int low = 0;
        int high = m;

        while (low <= high) {
            int partitionX = low + (high - low) / 2;
            int partitionY = (m + n + 1) / 2 - partitionX;

            int maxLeftX = (partitionX == 0) ? Integer.MIN_VALUE : nums1[partitionX - 1];
            int minRightX = (partitionX == m) ? Integer.MAX_VALUE : nums1[partitionX];

            int maxLeftY = (partitionY == 0) ? Integer.MIN_VALUE : nums2[partitionY - 1];
            int minRightY = (partitionY == n) ? Integer.MAX_VALUE : nums2[partitionY];

            if (maxLeftX <= minRightY && maxLeftY <= minRightX) {
                if ((m + n) % 2 != 0) {
                    return (double) Math.max(maxLeftX, maxLeftY);
                } else {
                    return ((double) Math.max(maxLeftX, maxLeftY) + Math.min(minRightX, minRightY)) / 2.0;
                }
            } else if (maxLeftX > minRightY) {
                high = partitionX - 1;
            } else {
                low = partitionX + 1;
            }
        }

        return 0.0;
    }
}
```

#### TypeScript 5
```typescript
function findMedianSortedArrays(nums1: number[], nums2: number[]): number {
    if (nums1.length > nums2.length) {
        return findMedianSortedArrays(nums2, nums1);
    }

    const m = nums1.length;
    const n = nums2.length;
    let low = 0;
    let high = m;

    while (low <= high) {
        const partitionX = Math.floor((low + high) / 2);
        const partitionY = Math.floor((m + n + 1) / 2) - partitionX;

        const maxLeftX = (partitionX === 0) ? -Infinity : nums1[partitionX - 1];
        const minRightX = (partitionX === m) ? Infinity : nums1[partitionX];

        const maxLeftY = (partitionY === 0) ? -Infinity : nums2[partitionY - 1];
        const minRightY = (partitionY === n) ? Infinity : nums2[partitionY];

        if (maxLeftX <= minRightY && maxLeftY <= minRightX) {
            if ((m + n) % 2 !== 0) {
                return Math.max(maxLeftX, maxLeftY);
            } else {
                return (Math.max(maxLeftX, maxLeftY) + Math.min(minRightX, minRightY)) / 2;
            }
        } else if (maxLeftX > minRightY) {
            high = partitionX - 1;
        } else {
            low = partitionX + 1;
        }
    }

    return 0.0;
}
```

#### Go 1.22
```go
package main

import "math"

func findMedianSortedArrays(nums1 []int, nums2 []int) float64 {
	if len(nums1) > len(nums2) {
		return findMedianSortedArrays(nums2, nums1)
	}

	m := len(nums1)
	n := len(nums2)
	low := 0
	high := m

	for low <= high {
		partitionX := (low + high) / 2
		partitionY := (m + n + 1) / 2 - partitionX

		maxLeftX := math.MinInt32
		if partitionX > 0 {
			maxLeftX = nums1[partitionX-1]
		}

		minRightX := math.MaxInt32
		if partitionX < m {
			minRightX = nums1[partitionX]
		}

		maxLeftY := math.MinInt32
		if partitionY > 0 {
			maxLeftY = nums2[partitionY-1]
		}

		minRightY := math.MaxInt32
		if partitionY < n {
			minRightY = nums2[partitionY]
		}

		if maxLeftX <= minRightY && maxLeftY <= minRightX {
			if (m+n)%2 != 0 {
				return float64(max(maxLeftX, maxLeftY))
			}
			return float64(max(maxLeftX, maxLeftY)+min(minRightX, minRightY)) / 2.0
		} else if maxLeftX > minRightY {
			high = partitionX - 1
		} else {
			low = partitionX + 1
		}
	}

	return 0.0
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
```

#### Rust 1.75
```rust
use std::cmp;

pub struct Solution;

impl Solution {
    pub fn find_median_sorted_arrays(nums1: Vec<i32>, nums2: Vec<i32>) -> f64 {
        if nums1.len() > nums2.len() {
            return Self::find_median_sorted_arrays(nums2, nums1);
        }

        let m = nums1.len();
        let n = nums2.len();
        let mut low = 0;
        let mut high = m;

        while low <= high {
            let partition_x = (low + high) / 2;
            let partition_y = (m + n + 1) / 2 - partition_x;

            let max_left_x = if partition_x == 0 { i32::MIN } else { nums1[partition_x - 1] };
            let min_right_x = if partition_x == m { i32::MAX } else { nums1[partition_x] };

            let max_left_y = if partition_y == 0 { i32::MIN } else { nums2[partition_y - 1] };
            let min_right_y = if partition_y == n { i32::MAX } else { nums2[partition_y] };

            if max_left_x <= min_right_y && max_left_y <= min_right_x {
                if (m + n) % 2 != 0 {
                    return cmp::max(max_left_x, max_left_y) as f64;
                } else {
                    return (cmp::max(max_left_x, max_left_y) as f64
                        + cmp::min(min_right_x, min_right_y) as f64)
                        / 2.0;
                }
            } else if max_left_x > min_right_y {
                high = partition_x - 1;
            } else {
                low = partition_x + 1;
            }
        }

        0.0
    }
}
```

---

## 4. Tier 2: Two-Pointer Virtual Merge Scan

### 4.1 Implementation Mechanism
Advance two pointers `p1` and `p2` tracking the two most recently observed elements (`prev` and `curr`).
Count steps until reaching $(m + n) / 2$.

```cpp
class SolutionTwoPointers {
public:
    double findMedianSortedArrays(const std::vector<int>& nums1, const std::vector<int>& nums2) {
        int m = nums1.size(), n = nums2.size();
        int total = m + n;
        int p1 = 0, p2 = 0;
        int prev = 0, curr = 0;

        for (int i = 0; i <= total / 2; ++i) {
            prev = curr;
            if (p1 < m && (p2 >= n || nums1[p1] <= nums2[p2])) {
                curr = nums1[p1++];
            } else {
                curr = nums2[p2++];
            }
        }

        if (total % 2 != 0) {
            return curr;
        }
        return (prev + curr) / 2.0;
    }
};
```

### 4.2 Trade-offs
- Simple linear scan requiring zero dynamic memory allocation.
- Runs in $O((M + N) / 2)$ time, which does not satisfy the strict $O(\log(M + N))$ requirement for large inputs.

---

## 5. Tier 3: Recursive $k$-th Element Truncation

### 5.1 Algorithmic Structure
Formulate the problem as finding the $k$-th smallest element in two sorted arrays.
At each step, inspect the elements at offset $\lfloor k / 2 \rfloor - 1$ in each array.
The smaller element and all preceding elements in that array cannot be part of the $k$-th element, and are discarded.

```python
class SolutionRecursiveKth:
    def findMedianSortedArrays(self, nums1: List[int], nums2: List[int]) -> float:
        total = len(nums1) + len(nums2)
        def get_kth(a_start: int, b_start: int, k: int) -> int:
            if a_start >= len(nums1):
                return nums2[b_start + k - 1]
            if b_start >= len(nums2):
                return nums1[a_start + k - 1]
            if k == 1:
                return min(nums1[a_start], nums2[b_start])

            half = k // 2
            mid_a = nums1[a_start + half - 1] if a_start + half - 1 < len(nums1) else float('inf')
            mid_b = nums2[b_start + half - 1] if b_start + half - 1 < len(nums2) else float('inf')

            if mid_a < mid_b:
                return get_kth(a_start + half, b_start, k - half)
            else:
                return get_kth(a_start, b_start + half, k - half)

        if total % 2 != 0:
            return float(get_kth(0, 0, total // 2 + 1))
        return (get_kth(0, 0, total // 2) + get_kth(0, 0, total // 2 + 1)) / 2.0
```

### 5.2 Trade-offs
- Clean divide-and-conquer strategy operating in $O(\log(M + N))$ time.
- Requires two separate recursive calls for even-length medians and consumes recursion call-stack memory.

---

## 6. Tier 4: Brute Force Baseline (Full Array Merge)

### 6.1 Mechanical Description
Allocate an array of size $m + n$.
Use standard two-pointer merging to copy all elements into the unified array.
Return the middle element directly via array indexing.

### 6.2 Complexity
- **Time Complexity**: $O(M + N)$ to copy and compare all elements.
- **Space Complexity**: $O(M + N)$ auxiliary memory for the merged buffer.
- **Verdict**: Inefficient and violates the explicit logarithmic complexity constraint.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Logarithmic Bisection Speed**: Since $\min(M, N) \le 1000$, binary search executes at most $\approx \lceil \log_2(1001) \rceil = 10$ iterations.
2. Ten memory lookups across contiguous arrays fit entirely inside L1 data cache lines.
3. **Register-Level Execution**: All boundary values (`maxLeftX`, `minRightX`, etc.) reside directly in CPU registers without memory bus traffic.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| One Empty Array | `nums1 = []`, `nums2 = [1]` | Returns median of non-empty array: `1.0` | $m=0$ cut results in $i=0$ with infinite boundary sentinels. |
| Disjoint Value Ranges | `[1, 2]` and `[3, 4]` | Returns `2.5` | Correctly places partition cuts at array extremes. |
| Single-Element Arrays | `[1]` and `[2]` | Returns `1.5` | Handles $m=1, n=1$ with single iteration. |
| Duplicate Values | `[1, 1]` and `[1, 1]` | Returns `1.0` | $\le$ and $\ge$ boundary checks handle equality correctly. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why binary search on the smaller array rather than the larger one?
Searching the smaller array guarantees $O(\log(\min(M, N)))$ time complexity and ensures the computed index $j = (m + n + 1) / 2 - i$ always stays within valid bounds $[0, n]$.

### 2. Why is $(m + n + 1) / 2$ used instead of $(m + n) / 2$?
Using $(m + n + 1) / 2$ ensures that integer division rounds up for odd lengths, guaranteeing that the Left Part contains the median element.

### 3. What role do `INT_MIN` and `INT_MAX` play?
When a partition cut falls at index 0 or $m$, one side of that array contributes zero elements. Using $-\infty$ and $+\infty$ allows boundary comparisons to proceed without special-case branches.

### 4. Can binary search loop infinitely on edge conditions?
No, because $i = \lfloor (\text{low} + \text{high}) / 2 \rfloor$ strictly contracts the search interval $[\text{low}, \text{high}]$ upon every update.

### 5. Why does division by 2.0 require floating-point promotion?
Integer division truncates the decimal part (e.g. $5 / 2 = 2$). Dividing by `2.0` preserves the floating-point result (`2.5`).

### 6. Can this approach generalize to $K$ sorted arrays?
For $K$ sorted arrays, partitioning becomes multi-dimensional. A more practical approach is binary searching over the value range in $O(K \cdot \log(\text{range}) \cdot \log N)$ time.

### 7. How does this compare with Quickselect?
Quickselect finds the median in $O(M + N)$ average time on unsorted arrays. When arrays are already sorted, dual binary search achieves $O(\log(\min(M, N)))$.

### 8. What is the difference between $O(\log(\min(M, N)))$ and $O(\log(M + N))$?
When $M \ll N$ (e.g. $M = 1$ and $N = 10^6$), $\log(\min(M, N)) = 0$, finding the median in $O(1)$ operations, whereas $\log(M + N) \approx 20$.

### 9. Why does Rust cast to `f64` explicitly?
Rust does not perform implicit numeric type coercion; integers must be explicitly cast using `as f64` before division.

### 10. Does this algorithm handle negative numbers correctly?
Yes. The comparisons rely strictly on the relative order of elements, which is invariant under negative integer values.

---

## 10. Related Problems and Systematic Progression Links

- [[0033-Search-in-Rotated-Sorted-Array]]: Binary search with modified partition invariants.
- [[0153-Find-Minimum-in-Rotated-Sorted-Array]]: Identifying rotation inflection point via logarithmic cuts.
- LeetCode 215 (Kth Largest Element in an Array): Order statistics and selection algorithms.
- LeetCode 240 (Search a 2D Matrix II): Stepwise and logarithmic searches across two-dimensional sorted spaces.
- LeetCode 378 (Kth Smallest Element in a Sorted Matrix): Range-based binary search on sorted matrices.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/median-of-two-sorted-arrays.cpp)
- [Python Implementation](../Python/median-of-two-sorted-arrays.py)
- [Java Implementation](../Java/median-of-two-sorted-arrays.java)
- [TypeScript Implementation](../TypeScript/median-of-two-sorted-arrays.ts)
- [Go Implementation](../Golang/median-of-two-sorted-arrays.go)
- [Rust Implementation](../Rust/median-of-two-sorted-arrays.rs)

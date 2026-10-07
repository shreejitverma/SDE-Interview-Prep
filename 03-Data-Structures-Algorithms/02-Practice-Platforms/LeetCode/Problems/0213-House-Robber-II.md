---
id: leetcode-0213-house-robber-ii
title: "LeetCode 0213: House Robber II"
tags:
  - dsa
  - leetcode
  - array
  - dynamic-programming
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/house-robber-ii/"
---

# LeetCode 0213: House Robber II

## 1. Problem Formalization and Constraints

You are a professional robber planning to rob houses along a street.
Each house has a certain amount of money stashed.
All houses at this place are arranged in a circle.
That means the first house is the neighbor of the last one.
Meanwhile, adjacent houses have a security system connected, and it will automatically contact the police if two adjacent houses were broken into on the same night.
Given an integer array `nums` representing the amount of money of each house, return the maximum amount of money you can rob tonight without alerting the police.

### Constraints
- $1 \le \text{nums.length} \le 100$
- $0 \le \text{nums}[i] \le 1000$

### Examples
- **Example 1**:
  - Input: `nums = [2,3,2]`
  - Output: `3`
  - Explanation: You cannot rob house 1 (money = 2) and then rob house 3 (money = 2), because they are adjacent houses.
- **Example 2**:
  - Input: `nums = [1,2,3,1]`
  - Output: `4`
  - Explanation: Rob house 1 (money = 1) and then rob house 3 (money = 3). Total amount you can rob = $1 + 3 = 4$.
- **Example 3**:
  - Input: `nums = [1,2,3]`
  - Output: `3`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Two-Pass Linear DP Range Decomposition | $O(N)$ | $O(1)$ auxiliary | Runs standard House Robber on $[0, N-2]$ and $[1, N-1]$; returns maximum of the two runs. |
| **Tier 2 (1D DP Array)** | Two Passes with 1D Tabulation Arrays | $O(N)$ | $O(N)$ auxiliary | Tabulates `dp` explicitly for both linear sub-ranges. |
| **Tier 3 (Memoized DFS with Flag)** | Top-Down DFS with Boundary State | $O(N)$ | $O(N)$ auxiliary | Recursion passing a boolean flag indicating whether house 0 was selected. |
| **Tier 4 (Brute Force)** | Circular Independent Set Enumeration | $O(2^N)$ | $O(N)$ auxiliary | Checks all $2^N$ house combinations, discarding any with adjacent robbed houses. |

---

## 3. Tier 1: Most Optimal Solution (Two-Pass Linear DP Range Decomposition)

### 3.1 Algorithmic Mechanics and Invariant Proof

Because the houses form a circle, house 0 and house $N - 1$ are adjacent and cannot both be robbed.
This partitions the search space into two mutually exhaustive and non-exclusive cases:
1. Case A (Exclude last house): Consider houses in the range $[0, N - 2]$. In this linear sequence, house 0 may or may not be robbed, but house $N - 1$ is never robbed.
2. Case B (Exclude first house): Consider houses in the range $[1, N - 1]$. In this linear sequence, house $N - 1$ may or may not be robbed, but house 0 is never robbed.

Any valid circular selection cannot include both house 0 and house $N - 1$:
- If an optimal selection robs house 0, it cannot rob house $N - 1$, so it is entirely contained in Case A.
- If an optimal selection robs house $N - 1$, it cannot rob house 0, so it is entirely contained in Case B.
- If an optimal selection robs neither house 0 nor house $N - 1$, it is contained in both Case A and Case B.

Therefore:
$$\text{Max Loot} = \max(\text{robLinear}(0, N - 2), \text{robLinear}(1, N - 1))$$
For edge case $N = 1$, return `nums[0]` directly because circular adjacency requires at least 2 houses.

**Invariant Proof**:
Let $S$ be the set of all subsets of $\{0, \dots, N-1\}$ with no adjacent elements modulo $N$.
Let $S_A = \{X \in S \mid N - 1 \notin X\}$ and $S_B = \{X \in S \mid 0 \notin X\}$.
Since no subset $X \in S$ can contain both $0$ and $N - 1$ (they are adjacent modulo $N$), every $X \in S$ must either omit $N - 1$ or omit $0$.
Thus $S = S_A \cup S_B$.
$S_A$ corresponds exactly to valid non-adjacent subsets of the linear array $\text{nums}[0 \dots N-2]$.
$S_B$ corresponds exactly to valid non-adjacent subsets of the linear array $\text{nums}[1 \dots N-1]$.
By the correctness of the linear House Robber algorithm, $\text{robLinear}(0, N-2)$ achieves $\max_{X \in S_A} \sum_{i \in X} \text{nums}[i]$, and $\text{robLinear}(1, N-1)$ achieves $\max_{X \in S_B} \sum_{i \in X} \text{nums}[i]$.
Hence $\max(\text{robLinear}(0, N-2), \text{robLinear}(1, N-1))$ achieves $\max_{X \in S} \sum_{i \in X} \text{nums}[i]$.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Performs two independent linear scans over $N - 1$ elements each, taking $2(N - 1) = O(N)$ time.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space using constant scalar registers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int rob(std::vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        if (nums.size() == 1) {
            return nums[0];
        }

        return std::max(robRange(nums, 0, nums.size() - 1),
                        robRange(nums, 1, nums.size()));
    }

private:
    int robRange(const std::vector<int>& nums, size_t start, size_t end) {
        int prev2 = 0;
        int prev1 = 0;

        for (size_t i = start; i < end; ++i) {
            int current = std::max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }
};
```

#### Python 3
```python
from typing import List


class Solution:
    def rob(self, nums: List[int]) -> int:
        if not nums:
            return 0
        if len(nums) == 1:
            return nums[0]

        return max(self._rob_range(nums, 0, len(nums) - 1),
                   self._rob_range(nums, 1, len(nums)))

    def _rob_range(self, nums: List[int], start: int, end: int) -> int:
        prev2, prev1 = 0, 0
        for i in range(start, end):
            prev2, prev1 = prev1, max(prev1, prev2 + nums[i])
        return prev1
```

#### Java 21
```java
class Solution {
    public int rob(int[] nums) {
        if (nums == null || nums.length == 0) {
            return 0;
        }
        if (nums.length == 1) {
            return nums[0];
        }

        return Math.max(robRange(nums, 0, nums.length - 1),
                        robRange(nums, 1, nums.length));
    }

    private int robRange(int[] nums, int start, int end) {
        int prev2 = 0;
        int prev1 = 0;

        for (int i = start; i < end; i++) {
            int current = Math.max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }
}
```

#### TypeScript
```typescript
function rob(nums: number[]): number {
    if (!nums || nums.length === 0) {
        return 0;
    }
    if (nums.length === 1) {
        return nums[0];
    }

    function robRange(start: number, end: number): number {
        let prev2 = 0;
        let prev1 = 0;

        for (let i = start; i < end; i++) {
            const current = Math.max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }

    return Math.max(robRange(0, nums.length - 1), robRange(1, nums.length));
}
```

#### Go
```go
package main

func rob(nums []int) int {
	if len(nums) == 0 {
		return 0
	}
	if len(nums) == 1 {
		return nums[0]
	}

	robRange := func(start, end int) int {
		prev2 := 0
		prev1 := 0

		for i := start; i < end; i++ {
			current := prev1
			if prev2+nums[i] > current {
				current = prev2 + nums[i]
			}
			prev2 = prev1
			prev1 = current
		}

		return prev1
	}

	r1 := robRange(0, len(nums)-1)
	r2 := robRange(1, len(nums))
	if r1 > r2 {
		return r1
	}
	return r2
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        if nums.is_empty() {
            return 0;
        }
        if nums.len() == 1 {
            return nums[0];
        }

        Self::rob_range(&nums, 0, nums.len() - 1)
            .max(Self::rob_range(&nums, 1, nums.len()))
    }

    fn rob_range(nums: &[i32], start: usize, end: usize) -> i32 {
        let mut prev2 = 0i32;
        let mut prev1 = 0i32;

        for i in start..end {
            let current = prev1.max(prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        prev1
    }
}
```

---

## 4. Tier 2: 1D Dynamic Programming Tabulation Two Passes

### 4.1 Mechanical Description
Allocate two explicit dynamic programming arrays `dp1` and `dp2` of size $N - 1$.
`dp1` computes linear house robber on slice `nums[0..N-2]`.
`dp2` computes linear house robber on slice `nums[1..N-1]`.
Return $\max(dp1[N-2], dp2[N-2])$.

### 4.2 Trade-offs
- Explicitly materializes states for inspection and debugging.
- Allocates $2 \times O(N)$ unnecessary array memory.

---

## 5. Tier 3: Top-Down Recursion with Boundary Flag

### 5.1 Mechanical Description
Use memoization with state `(index, robbed_first_house)` of size $N \times 2$.
When `index == N - 1`, if `robbed_first_house` is true, house $N - 1$ cannot be robbed; return 0.

### 5.2 Trade-offs
- Solves the problem in a single recursive tree.
- Complex state transitions with higher memoization storage overhead than two simple linear DP calls.

---

## 6. Tier 4: Circular Independent Set Enumeration (Brute Force Baseline)

### 6.1 Mechanical Description
Generate all $2^N$ binary subsets.
Verify that no two adjacent indices (including $(0, N - 1)$) are both selected.
Compute the sum of valid configurations and record the maximum.

### 6.2 Complexity & Deficiencies
- $O(2^N)$ time complexity; fails for $N > 25$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Subarray Slices vs Indices**: Passing index boundaries `start` and `end` to a helper function avoids slicing and copying arrays, eliminating heap allocations.
2. **Sequential Memory Access**: Both linear scans read contiguous elements sequentially, taking full advantage of CPU hardware prefetchers.
3. **Register-Only State**: Each scan maintains only two registers, executing in nanoseconds for arrays of length 100.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single house | `nums = [10]` | Returns `10` | Handled via guard clause `if (nums.size() == 1) return nums[0];`. |
| Two houses | `nums = [1, 2]` | Returns `2` | Range $[0, 0]$ yields 1; range $[1, 1]$ yields 2; max is 2. |
| Three houses | `nums = [2, 3, 2]` | Returns `3` | Can only choose 1 house due to mutual circular adjacency; max is 3. |
| All identical elements | `nums = [5, 5, 5, 5]` | Returns `10` | Robs 2 houses out of 4: $5 + 5 = 10$. |
| Large stash at circular ends | `nums = [100, 1, 1, 100]` | Returns `101` | Cannot rob both 100s; range A takes 100+1; range B takes 1+100; max is 101. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why is the guard clause for $N = 1$ necessary?
For $N = 1$, the subranges $[0, N-2] = [0, -1]$ and $[1, N-1] = [1, 0]$ are empty, returning 0 instead of `nums[0]`.

### 2. Why don't we need a third case where neither house 0 nor house $N-1$ is robbed?
The case where neither is robbed is a subset of both Case A and Case B, so its maximum is already captured.

### 3. How does this compare to House Robber I (LeetCode 198)?
House Robber I is linear with no link between ends. House Robber II reduces the circular topology to two linear House Robber I subproblems.

### 4. Can this be extended to tree structures?
Yes. House Robber III (LeetCode 337) places houses on tree nodes, solved using post-order tree dynamic programming.

### 5. Why is the time complexity $O(N)$ and not $O(N^2)$?
Running linear dynamic programming twice takes $2 \times O(N) = O(N)$ time.

### 6. Can negative values exist in `nums`?
Constraints guarantee non-negative values ($0 \le \text{nums}[i] \le 1000$).

### 7. What is the maximum possible loot?
For $N = 100$ and $\text{nums}[i] \le 1000$, maximum loot is $\le 50 \times 1000 = 50,000$, fitting comfortably in a 32-bit signed integer.

### 8. Why pass `start` and `end` rather than slicing in Python?
Slicing `nums[start:end]` allocates a new list in memory, whereas passing indices operates directly on the existing list in $O(1)$ space.

### 9. What is the maximum number of houses that can be robbed?
In a circle of $N$ houses, at most $\lfloor N / 2 \rfloor$ houses can be robbed without choosing adjacent neighbors.

### 10. Does circular dynamic programming always reduce to linear cases?
For circular problems with local dependencies (like neighbor constraints), splitting along one edge or vertex to break the cycle into linear chains is a standard algorithmic pattern.

---

## 10. Related Problems and Systematic Progression Links

- [[0070-Climbing-Stairs]]: 1D dynamic programming recurrence.
- [[0198-House-Robber]]: Linear street house robber foundation.
- LeetCode 337 (House Robber III): House robber arranged across a binary tree.
- LeetCode 256 (Paint House): Selecting non-adjacent house color combinations.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/house-robber-ii.cpp)
- [Python Implementation](../Python/house-robber-ii.py)
- [Java Implementation](../Java/house-robber-ii.java)
- [TypeScript Implementation](../TypeScript/house-robber-ii.ts)
- [Go Implementation](../Golang/house-robber-ii.go)
- [Rust Implementation](../Rust/house-robber-ii.rs)

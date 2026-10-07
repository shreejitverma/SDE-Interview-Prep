---
id: leetcode-0198-house-robber
title: "LeetCode 0198: House Robber"
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
  - "https://leetcode.com/problems/house-robber/"
---

# LeetCode 0198: House Robber

## 1. Problem Formalization and Constraints

You are a professional robber planning to rob houses along a street.
Each house has a certain amount of money stashed.
The only constraint stopping you from robbing each of them is that adjacent houses have security systems connected, and it will automatically contact the police if two adjacent houses were broken into on the same night.
Given an integer array `nums` representing the amount of money of each house, return the maximum amount of money you can rob tonight without alerting the police.

### Constraints
- $1 \le \text{nums.length} \le 100$
- $0 \le \text{nums}[i] \le 400$

### Examples
- **Example 1**:
  - Input: `nums = [1,2,3,1]`
  - Output: `4`
  - Explanation: Rob house 1 (money = 1) and then rob house 3 (money = 3). Total amount you can rob = $1 + 3 = 4$.
- **Example 2**:
  - Input: `nums = [2,7,9,3,1]`
  - Output: `12`
  - Explanation: Rob house 1 (money = 2), rob house 3 (money = 9), and rob house 5 (money = 1). Total amount you can rob = $2 + 9 + 1 = 12$.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Constant-Space Rolling DP | $O(N)$ | $O(1)$ auxiliary | Tracks `prev1` (max up to house $i-1$) and `prev2` (max up to house $i-2$); updates with `max(prev1, prev2 + num)`. |
| **Tier 2 (1D DP Array)** | Full 1D Tabulation Array | $O(N)$ | $O(N)$ auxiliary | Tabulates `dp[i] = max(dp[i-1], dp[i-2] + nums[i])` in an array of size $N$. |
| **Tier 3 (Memoized DFS)** | Top-Down Recursion with Memoization | $O(N)$ | $O(N)$ auxiliary | Recursively computes optimal choices with a 1D cache array. |
| **Tier 4 (Brute Force)** | Recursive Binary Decision Tree | $O(2^N)$ | $O(N)$ auxiliary | At each house, branches into rob or skip without memoization. |

---

## 3. Tier 1: Most Optimal Solution (Constant-Space Rolling DP)

### 3.1 Algorithmic Mechanics and Invariant Proof

At each house $i$, there are two mutually exclusive decisions:
1. Skip house $i$: The maximum money obtained is the maximum loot from the prefix ending at house $i - 1$ (`prev1`).
2. Rob house $i$: House $i - 1$ cannot be robbed. The maximum money obtained is the loot of house $i$ plus the maximum loot from the prefix ending at house $i - 2$ (`prev2 + nums[i]`).

The recurrence relation is:
$$\text{current} = \max(\text{prev1}, \text{prev2} + \text{nums}[i])$$
We update `prev2 = prev1` and `prev1 = current`.

**Invariant Proof**:
Let $M(k)$ be the maximum money that can be robbed from houses $0 \dots k$.
Base cases:
$M(0) = \text{nums}[0]$.
$M(1) = \max(\text{nums}[0], \text{nums}[1])$.
Inductive hypothesis: Assume for all $j < k$, $M(j)$ correctly represents the optimal loot for prefix $0 \dots j$.
To compute $M(k)$:
Any optimal selection for $0 \dots k$ either excludes house $k$ or includes house $k$.
- If it excludes house $k$, the selection is a valid subproblem on $0 \dots k-1$, with optimal value $M(k-1)$.
- If it includes house $k$, house $k-1$ cannot be included, and the remaining selection on $0 \dots k-2$ is independent with optimal value $M(k-2)$. The total value is $M(k-2) + \text{nums}[k]$.
Since these two cases exhaust all possibilities, $M(k) = \max(M(k-1), M(k-2) + \text{nums}[k])$.
Because computing $M(k)$ depends only on $M(k-1)$ and $M(k-2)$, two scalar variables maintain this invariant in $O(1)$ space.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Performs a single pass over `nums`, executing constant operations per element.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space using two primitive integers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int rob(std::vector<int>& nums) {
        int prev2 = 0;
        int prev1 = 0;

        for (int num : nums) {
            int current = std::max(prev1, prev2 + num);
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
        prev2, prev1 = 0, 0

        for num in nums:
            prev2, prev1 = prev1, max(prev1, prev2 + num)

        return prev1
```

#### Java 21
```java
class Solution {
    public int rob(int[] nums) {
        int prev2 = 0;
        int prev1 = 0;

        for (int num : nums) {
            int current = Math.max(prev1, prev2 + num);
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
    let prev2 = 0;
    let prev1 = 0;

    for (const num of nums) {
        const current = Math.max(prev1, prev2 + num);
        prev2 = prev1;
        prev1 = current;
    }

    return prev1;
}
```

#### Go
```go
package main

func rob(nums []int) int {
	prev2 := 0
	prev1 := 0

	for _, num := range nums {
		current := prev1
		if prev2+num > current {
			current = prev2 + num
		}
		prev2 = prev1
		prev1 = current
	}

	return prev1
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        let mut prev2 = 0i32;
        let mut prev1 = 0i32;

        for num in nums {
            let current = prev1.max(prev2 + num);
            prev2 = prev1;
            prev1 = current;
        }

        prev1
    }
}
```

---

## 4. Tier 2: 1D Dynamic Programming Tabulation Array

### 4.1 Mechanical Description
Allocate an array `dp` of size $N$.
Set `dp[0] = nums[0]`.
Set `dp[1] = max(nums[0], nums[1])`.
For $i$ from 2 to $N-1$:
`dp[i] = max(dp[i-1], dp[i-2] + nums[i])`.
Return `dp[N-1]`.

```python
def robDP(nums: list[int]) -> int:
    if not nums:
        return 0
    if len(nums) == 1:
        return nums[0]
    dp = [0] * len(nums)
    dp[0] = nums[0]
    dp[1] = max(nums[0], nums[1])
    for i in range(2, len(nums)):
        dp[i] = max(dp[i - 1], dp[i - 2] + nums[i])
    return dp[-1]
```

### 4.2 Trade-offs
- Provides historical progression of optimal loot at every house.
- Incurs $O(N)$ extra memory allocation.

---

## 5. Tier 3: Top-Down Recursion with Memoization

### 5.1 Mechanical Description
Define `robFrom(i)` returning maximum loot from house $i$ onward.
`robFrom(i) = max(robFrom(i + 1), nums[i] + robFrom(i + 2))`.
Cache results in an array of size $N$.

### 5.2 Trade-offs
- Natural formulation of recursive choice.
- Call stack depth of $O(N)$ frames adds function call overhead.

---

## 6. Tier 4: Recursive Binary Decision Tree (Brute Force Baseline)

### 6.1 Mechanical Description
Evaluate `robFrom(i)` without caching.
The decision tree evaluates $2^N$ leaves, taking $O(2^N)$ time and timing out on arrays with length $\ge 40$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Register Allocation**: The two scalar variables `prev1` and `prev2` reside completely in CPU general-purpose registers, resulting in zero cache misses during execution.
2. **Sequential Streaming**: Array `nums` is read linearly from start to finish, triggering hardware prefetching and maximizing memory bus bandwidth.
3. **No Allocation**: Zero heap allocations are performed in the optimal tier across all language implementations.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single house | `nums = [5]` | Returns `5` | Loop runs once: `prev1 = max(0, 0 + 5) = 5`. |
| Two houses | `nums = [3, 7]` | Returns `7` | Step 1 yields 3; step 2 yields $\max(3, 0 + 7) = 7$. |
| All houses with zero stash | `nums = [0, 0, 0]` | Returns `0` | Math holds cleanly: `max(0, 0 + 0) = 0`. |
| Equal values | `nums = [2, 2, 2, 2]` | Returns `4` | Robs alternating houses at index 0 and 2. |
| Ascending stashes | `nums = [1, 2, 3, 4, 5]` | Returns `9` | Robs houses at indices 0, 2, 4 ($1 + 3 + 5 = 9$). |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does initializing `prev1 = 0` and `prev2 = 0` handle arrays of length 1 cleanly?
Before the first house, no money has been robbed. At house 0, `current = max(0, 0 + nums[0]) = nums[0]`, seamlessly producing the correct base case without conditional branching.

### 2. Can two non-adjacent skipped houses ever be optimal?
Yes. If house stashes are `[100, 1, 1, 100]`, skipping both index 1 and index 2 yields $100 + 100 = 200$. The recurrence naturally accounts for this because `prev1` propagates the best prior decision.

### 3. How does this problem relate to Fibonacci numbers?
The state recurrence $dp[i] = \max(dp[i-1], dp[i-2] + nums[i])$ mirrors Fibonacci's $F(n) = F(n-1) + F(n-2)$, but replaces addition with selection under maximum.

### 4. What is the difference between House Robber and House Robber II (LeetCode 213)?
House Robber II arranges houses in a circle where the first and last houses are adjacent, preventing simultaneous selection of both.

### 5. What is the difference between House Robber and House Robber III (LeetCode 337)?
In House Robber III, houses are arranged as nodes of a binary tree, requiring tree dynamic programming (post-order traversal).

### 6. Can negative values exist in `nums`?
The problem constraints guarantee `nums[i] >= 0`. If negative values were present, robbing would never be forced, and $\max(0, \dots)$ checks would be needed.

### 7. What is the maximum possible return value?
$100 \times 400 = 40,000$, which easily fits inside standard 32-bit signed integers.

### 8. How does TypeScript handle numerical overflow?
JavaScript and TypeScript numbers are 64-bit double precision floats with exact integers up to $2^{53} - 1$, well beyond 40,000.

### 9. Why is this considered an optimal substructure problem?
The optimal solution for $N$ houses contains within it the optimal solutions for the subproblems of $N-1$ and $N-2$ houses.

### 10. Can this be solved greedily?
No. Always picking the largest available house can cause suboptimal choices; for instance, in `[10, 15, 10]`, greedy choice of 15 yields 15, while dynamic programming achieves $10 + 10 = 20$.

---

## 10. Related Problems and Systematic Progression Links

- [[0070-Climbing-Stairs]]: 1D dynamic programming recurrence.
- [[0152-Maximum-Product-Subarray]]: 1D dynamic programming with sign transitions.
- [[0213-House-Robber-II]]: House robber with circular neighborhood arrangement.
- LeetCode 337 (House Robber III): House robber arranged across a binary tree.
- LeetCode 256 (Paint House): Selecting non-adjacent house color combinations.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/house-robber.cpp)
- [Python Implementation](../Python/house-robber.py)
- [Java Implementation](../Java/house-robber.java)
- [TypeScript Implementation](../TypeScript/house-robber.ts)
- [Go Implementation](../Golang/house-robber.go)
- [Rust Implementation](../Rust/house-robber.rs)

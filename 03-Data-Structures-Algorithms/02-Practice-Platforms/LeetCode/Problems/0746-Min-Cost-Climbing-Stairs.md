---
id: leetcode-0746-min-cost-climbing-stairs
title: "LeetCode 0746: Min Cost Climbing Stairs"
tags:
  - dsa
  - leetcode
  - array
  - dynamic-programming
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/min-cost-climbing-stairs/"
---

# LeetCode 0746: Min Cost Climbing Stairs

## 1. Problem Formalization and Constraints

You are given an integer array `cost` where `cost[i]` is the cost of $i$-th step on a staircase.
Once you pay the cost, you can either climb one or two steps.
You can either start from the step with index 0, or the step with index 1.
Return the minimum cost to reach the top of the floor.

### Constraints
- $2 \le \text{cost.length} \le 1000$
- $0 \le \text{cost}[i] \le 999$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Space-Optimized Dynamic Programming | $O(N)$ | $O(1)$ | Tracks two rolling states; avoids dynamic array allocations. |
| **Tier 2 (Tabulation)** | 1D Tabulation Array | $O(N)$ | $O(N)$ | Full DP table $dp[i]$ representing minimum cost to reach step $i$. |
| **Tier 3 (Memoized DFS)** | Top-Down Recursion with Memoization | $O(N)$ | $O(N)$ | Recursive search with hash map or vector cache; recursion call overhead. |
| **Tier 4 (Brute Force)** | Exhaustive Branching Recursion | $O(2^N)$ | $O(N)$ | Binary branching tree of choices without memoization; exponential timeout. |

---

## 3. Tier 1: Most Optimal Solution (Space-Optimized Dynamic Programming)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $dp[i]$ be the minimum cost to reach step $i$.
To reach step $i$, one must either step from $i-1$ (paying `cost[i-1]`) or from $i-2$ (paying `cost[i-2]`).
Thus, the recurrence relation is:
$$\text{curr} = \text{cost}[i] + \min(\text{prev1}, \text{prev2})$$
Because computing $\text{curr}$ requires strictly the preceding two step states, only two integer variables are needed.
At termination, the top of the floor can be reached from either step $N-1$ or step $N-2$, so $\min(\text{prev1}, \text{prev2})$ gives the exact minimum cost.
This guarantees strict $O(N)$ linear runtime and $O(1)$ auxiliary memory.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    int minCostClimbingStairs(const std::vector<int>& cost) {
        int prev2 = 0;
        int prev1 = 0;

        for (int c : cost) {
            const int curr = c + std::min(prev1, prev2);
            prev2 = prev1;
            prev1 = curr;
        }

        return std::min(prev1, prev2);
    }
};
```

#### Python
```python
class Solution:
    def minCostClimbingStairs(self, cost: list[int]) -> int:
        prev2 = 0
        prev1 = 0

        for c in cost:
            curr = c + min(prev1, prev2)
            prev2 = prev1
            prev1 = curr

        return min(prev1, prev2)
```

#### Java
```java
class Solution {
    public int minCostClimbingStairs(int[] cost) {
        int prev2 = 0;
        int prev1 = 0;

        for (int c : cost) {
            int curr = c + Math.min(prev1, prev2);
            prev2 = prev1;
            prev1 = curr;
        }

        return Math.min(prev1, prev2);
    }
}
```

#### TypeScript
```typescript
function minCostClimbingStairs(cost: number[]): number {
    let prev2 = 0;
    let prev1 = 0;

    for (const c of cost) {
        const curr = c + Math.min(prev1, prev2);
        prev2 = prev1;
        prev1 = curr;
    }

    return Math.min(prev1, prev2);
}
```

#### Golang
```go
package main

func minCostClimbingStairs(cost []int) int {
	prev2 := 0
	prev1 := 0

	for _, c := range cost {
		minPrev := prev1
		if prev2 < minPrev {
			minPrev = prev2
		}
		curr := c + minPrev
		prev2 = prev1
		prev1 = curr
	}

	if prev1 < prev2 {
		return prev1
	}
	return prev2
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn min_cost_climbing_stairs(cost: Vec<i32>) -> i32 {
        let mut prev2 = 0;
        let mut prev1 = 0;

        for c in cost {
            let curr = c + prev1.min(prev2);
            prev2 = prev1;
            prev1 = curr;
        }

        prev1.min(prev2)
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(N)$ with a single sequential linear scan.
- **Space Complexity**: Strictly $O(1)$ auxiliary space using two primitive scalar variables.
- **Register Allocation**: Modern optimizing compilers keep `prev1` and `prev2` directly inside CPU hardware registers.

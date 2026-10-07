---
id: leetcode-0055-jump-game
title: "LeetCode 0055: Jump Game"
tags:
  - dsa
  - leetcode
  - array
  - dynamic-programming
  - greedy
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/jump-game/"
---

# LeetCode 0055: Jump Game

## 1. Problem Formalization and Constraints

You are given an integer array `nums`.
You are initially positioned at the array's first index, and each element in the array represents your maximum jump length at that position.
Return `true` if you can reach the last index, or `false` otherwise.

### Constraints
- $1 \le \text{nums.length} \le 10^4$
- $0 \le \text{nums}[i] \le 10^5$

### Examples
- **Example 1**:
  - Input: `nums = [2,3,1,1,4]`
  - Output: `true`
  - Explanation: Jump 1 step from index 0 to 1, then 3 steps to the last index.
- **Example 2**:
  - Input: `nums = [3,2,1,0,4]`
  - Output: `false`
  - Explanation: You will always arrive at index 3 no matter what. Its maximum jump length is 0, which makes it impossible to reach the last index.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Forward Greedy Reachability | $O(N)$ | $O(1)$ | Tracks `maxReachable = max(maxReachable, i + nums[i])`; short-circuits early if $i > \text{maxReachable}$ or $\text{maxReachable} \ge N - 1$. |
| **Tier 2 (Backward Greedy)** | Backward Target Shifting | $O(N)$ | $O(1)$ | Starts with `target = N - 1`; iterates backward; if $i + \text{nums}[i] \ge \text{target}$, shifts `target = i`; checks if `target == 0`. |
| **Tier 3 (Dynamic Programming)** | Tabulation / Reachability Array | $O(N^2)$ | $O(N)$ | $dp[i]$ marks reachability from index $i$; fills backwards; redundant inner loop over all jump lengths up to $\text{nums}[i]$. |
| **Tier 4 (Brute Force)** | Recursive Backtracking | $O(2^N)$ | $O(N)$ stack | Explores all possible jump choices from the current index recursively; suffers from exponential state explosion. |

---

## 3. Tier 1: Most Optimal Solution (Forward Greedy Reachability)

### 3.1 Algorithmic Mechanics and Invariant Proof

Maintain a variable `max_reachable` initialized to `0`.
Iterate through the array from index `i = 0` to `N - 1`:
1. If $i > \text{max\_reachable}$, the current index cannot be reached from any previously visited valid position. Return `false`.
2. Update the maximum reachable horizon:
   $$\text{max\_reachable} = \max(\text{max\_reachable}, i + \text{nums}[i])$$
3. If $\text{max\_reachable} \ge N - 1$, the destination is guaranteed to be reachable. Return `true`.
4. If the loop completes successfully, return `true`.

**Invariant Proof**:
Let $R_k$ denote the maximum reachable index after inspecting elements up to index $k$.
Base step: At $k = 0$, $R_0 = \text{nums}[0]$.
Inductive step: Any index $j \le R_k$ is reachable by transitivity because from some valid preceding index $m \le k$, a jump of length at most $\text{nums}[m]$ reaches or exceeds $j$.
If index $k + 1 > R_k$, no sequence of jumps from indices $0, \dots, k$ can ever reach $k + 1$.
Because all indices $> k$ require reaching an index $> R_k$ first, it is impossible to advance further, so returning `false` is strictly correct.
Conversely, if $R_k \ge N - 1$, there exists a valid sequence of jumps to or beyond the final index $N - 1$.
Thus, the single-pass greedy algorithm is correct and complete.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ single linear pass scanning up to $N$ elements.
- **Auxiliary Space Complexity**: $O(1)$ using only integer register variables.

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    bool canJump(vector<int>& nums) {
        int max_reachable = 0;
        const int n = static_cast<int>(nums.size());

        for (int i = 0; i < n; ++i) {
            if (i > max_reachable) {
                return false;
            }
            max_reachable = max(max_reachable, i + nums[i]);
            if (max_reachable >= n - 1) {
                return true;
            }
        }

        return true;
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N)
# Space: O(1)

from typing import List

class Solution:
    def canJump(self, nums: List[int]) -> bool:
        max_reachable = 0
        n = len(nums)

        for i, val in enumerate(nums):
            if i > max_reachable:
                return False
            max_reachable = max(max_reachable, i + val)
            if max_reachable >= n - 1:
                return True

        return True
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

class Solution {
    public boolean canJump(int[] nums) {
        int maxReachable = 0;
        for (int i = 0; i < nums.length; i++) {
            if (i > maxReachable) {
                return false;
            }
            maxReachable = Math.max(maxReachable, i + nums[i]);
            if (maxReachable >= nums.length - 1) {
                return true;
            }
        }
        return true;
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
// Space: O(1)

function canJump(nums: number[]): boolean {
    let maxReachable = 0;
    for (let i = 0; i < nums.length; i++) {
        if (i > maxReachable) {
            return false;
        }
        maxReachable = Math.max(maxReachable, i + nums[i]);
        if (maxReachable >= nums.length - 1) {
            return true;
        }
    }
    return true;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

func canJump(nums []int) bool {
	maxReachable := 0
	for i, val := range nums {
		if i > maxReachable {
			return false
		}
		if i+val > maxReachable {
			maxReachable = i + val
		}
		if maxReachable >= len(nums)-1 {
			return true
		}
	}
	return true
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn can_jump(nums: Vec<i32>) -> bool {
        let mut max_reachable = 0;
        let n = nums.len();

        for (i, &val) in nums.iter().enumerate() {
            if i > max_reachable {
                return false;
            }
            max_reachable = max_reachable.max(i + val as usize);
            if max_reachable >= n - 1 {
                return true;
            }
        }

        true
    }
}
```

---

## 4. Tier 2: Backward Greedy Goal Tracking

### 4.1 Mechanical Description
Set `target = nums.length - 1`.
Iterate backward from index `i = nums.length - 2` down to `0`.
If $i + \text{nums}[i] \ge \text{target}$, it means index $i$ can jump to the current target.
Therefore, shift `target = i`.
At the end of the scan, return `target == 0`.

### 4.2 Trade-offs
- Same $O(N)$ time and $O(1)$ space.
- Highly intuitive for verifying reachability backward from the finish line.

---

## 5. Tier 3: Dynamic Programming (Bottom-Up Memoization)

### 5.1 Mechanical Description
Create an array `dp` of size $N$ where `dp[i]` represents whether index $i$ can reach index $N - 1$.
Initialize `dp[N - 1] = true`.
For each index $i$ from $N - 2$ down to 0, check all reachable steps $j \in [1, \text{nums}[i]]$; if any `dp[i + j] == true`, mark `dp[i] = true`.

### 5.2 Trade-offs
- Classic tabulation DP formulation.
- Incurs $O(N^2)$ quadratic worst-case runtime and $O(N)$ memory, which is unnecessarily inefficient compared to the $O(N)$ greedy approach.

---

## 6. Tier 4: Recursive Backtracking (Brute Force Baseline)

### 6.1 Mechanical Description
Define `canJumpFrom(position)`:
If `position == N - 1`, return `true`.
For each jump length from 1 to `nums[position]`, recursively call `canJumpFrom(position + jump)`.
If any call returns `true`, return `true`.

### 6.2 Trade-offs
- Demonstrates search tree exploration.
- Exponential $O(2^N)$ time complexity causes Time Limit Exceeded on arrays with $N > 30$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Sequential Memory Streaming**: Forward traversal reads the array in exact sequential order, allowing hardware prefetchers to load cache lines with zero memory latency.
2. **Branch Predictor Performance**: For typical inputs without zero traps, the loop condition evaluates cleanly and early returns prevent redundant checks.
3. **Register-Only State**: Only one integer counter `max_reachable` is maintained, residing entirely within a CPU register.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single element array | `nums = [0]` | Returns `true` | Already at destination index 0 |
| Zero at start | `nums = [0, 2, 3]` | Returns `false` | Loop fails at $i = 1 > \text{max\_reachable} = 0$ |
| Trapped by zeros | `nums = [3, 2, 1, 0, 4]` | Returns `false` | Max reachable stays at 3, cannot cross index 3 |
| Large values | `nums = [100000]` | Returns `true` | Single element immediate return |
| Leap to end on first jump | `nums = [10, 0, 0]` | Returns `true` | `max_reachable >= N - 1` early exit |

---

## 9. Frequently Asked Questions (FAQ)

### 1. What is the fundamental difference between Jump Game I (55) and Jump Game II (45)?
Jump Game I only requires determining whether the last index is reachable (boolean). Jump Game II requires finding the minimum number of jumps to reach the last index.

### 2. Why does the greedy choice yield the global optimum?
Because reaching index $k$ allows making any jump of size $\le \text{nums}[k]$; a larger reach never eliminates any choices that a smaller reach would have allowed.

### 3. What role do zeros play in this problem?
Zeros are the only obstacles. A non-zero value can always advance at least 1 step; only when trapped behind zeros does reachability stall.

### 4. Can we stop scanning early?
Yes, as soon as `max_reachable >= nums.length - 1`, we can immediately return `true`.

### 5. Does the backward greedy approach require extra space?
No, backward greedy only tracks a single integer `target`, maintaining $O(1)$ space.

### 6. What happens if all elements are positive?
If all elements are $\ge 1$, reaching the end is always possible in $O(N)$ steps because each index can step at least 1 forward.

### 7. How does Python's `enumerate` impact performance?
`enumerate` provides clean idiomatic iteration in CPython without indexing overhead.

### 8. What is the maximum value of $N$?
$N \le 10^4$, meaning an $O(N)$ linear algorithm executes in less than 2 milliseconds.

### 9. Why does Rust require `usize` conversion?
Rust enforces strict type safety; array indices and lengths are `usize`, whereas array values are signed `i32`.

### 10. Can this problem be modeled as a graph?
Yes, as reachability on a Directed Acyclic Graph (DAG) where directed edges exist from $i$ to $i + 1, \dots, i + \text{nums}[i]$.

---

## 10. Related Problems and Systematic Progression Links

- [[0053-Maximum-Subarray]]: Kadane's algorithm single-pass greedy optimization.
- [[0121-Best-Time-to-Buy-and-Sell-Stock]]: One-pass tracking of optimal running bounds.
- [[0198-House-Robber]]: Linear dynamic programming decision making.
- [[0300-Longest-Increasing-Subsequence]]: Subsequence growth and patience sorting.
- LeetCode 45 (Jump Game II): Minimum jump count calculation.
- LeetCode 1306 (Jump Game III): Bidirectional graph reachability with BFS/DFS.
- LeetCode 1345 (Jump Game IV): Shortest path BFS on value-equality graphs.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/jump-game.cpp)
- [Python Implementation](../Python/jump-game.py)
- [Java Implementation](../Java/jump-game.java)
- [TypeScript Implementation](../TypeScript/jump-game.ts)
- [Go Implementation](../Golang/jump-game.go)
- [Rust Implementation](../Rust/jump-game.rs)

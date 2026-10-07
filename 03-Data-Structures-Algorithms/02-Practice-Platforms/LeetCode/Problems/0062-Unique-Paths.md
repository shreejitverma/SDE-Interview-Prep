---
id: leetcode-0062-unique-paths
title: "LeetCode 0062: Unique Paths"
tags:
  - dsa
  - leetcode
  - math
  - dynamic-programming
  - combinatorics
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/unique-paths/"
---

# LeetCode 0062: Unique Paths

## 1. Problem Formalization and Constraints

There is a robot on an $m \times n$ grid.
The robot is initially located at the top-left corner (`grid[0][0]`).
The robot tries to move to the bottom-right corner (`grid[m - 1][n - 1]`).
The robot can only move either down or right at any point in time.
Given the two integers $m$ and $n$, return the number of possible unique paths that the robot can take to reach the bottom-right corner.
The test cases are generated so that the answer will be less than or equal to $2 \times 10^9$.

### Constraints
- $1 \le m, n \le 100$

### Examples
- **Example 1**:
  - Input: `m = 3, n = 7`
  - Output: `28`
- **Example 2**:
  - Input: `m = 3, n = 2`
  - Output: `3`
  - Explanation: From the top-left corner, there are a total of 3 ways to reach the bottom-right corner:
    1. Right -> Down -> Down
    2. Down -> Down -> Right
    3. Down -> Right -> Down

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Combinatorial Formula $\binom{m+n-2}{\min(m-1, n-1)}$ | $O(\min(m, n))$ | $O(1)$ auxiliary | Computes binomial coefficient directly using iterative multiplicative reduction. |
| **Tier 2 (1D DP Array)** | Rolling DP Buffer | $O(m \cdot n)$ | $O(\min(m, n))$ auxiliary | Compresses 2D state into a 1D array updating `dp[j] += dp[j-1]`. |
| **Tier 3 (2D DP Matrix)** | Full Tabulation Grid | $O(m \cdot n)$ | $O(m \cdot n)$ auxiliary | Explicit $m \times n$ table with recurrence `dp[i][j] = dp[i-1][j] + dp[i][j-1]`. |
| **Tier 4 (Memoized DFS)** | Top-Down Recursion with Cache | $O(m \cdot n)$ | $O(m \cdot n)$ auxiliary | Recursively sums paths from `(i+1, j)` and `(i, j+1)` using a 2D memoization hash table. |

---

## 3. Tier 1: Most Optimal Solution (Combinatorial Formula)

### 3.1 Algorithmic Mechanics and Invariant Proof

To reach cell $(m - 1, n - 1)$ from $(0, 0)$, any valid path must make:
- Exactly $m - 1$ moves Down.
- Exactly $n - 1$ moves Right.
- Total moves: $(m - 1) + (n - 1) = m + n - 2$.

The problem is isomorphic to choosing which of the $m + n - 2$ moves are Down (or Right):
$$\text{Unique Paths} = \binom{m + n - 2}{m - 1} = \binom{m + n - 2}{n - 1} = \frac{(m + n - 2)!}{(m - 1)! (n - 1)!}$$

To prevent integer overflow in intermediate calculations:
1. Let $N = m + n - 2$ and $k = \min(m - 1, n - 1)$.
2. Use the iterative product formula:
$$\binom{N}{k} = \prod_{i=1}^{k} \frac{N - k + i}{i}$$
3. Perform multiplication before division at each step.
Because the product of any $i$ consecutive integers is always divisible by $i!$, the division is guaranteed to be exact at every step with no remainder.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(\min(m, n))$. Requires exactly $\min(m - 1, n - 1)$ multiplications and divisions. Since $\min(m, n) \le 100$, this executes in sub-microsecond time.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space requiring only scalar variables.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <algorithm>

class Solution {
public:
    int uniquePaths(int m, int n) {
        int total_steps = m + n - 2;
        int k = std::min(m - 1, n - 1);
        long long result = 1;

        for (int i = 1; i <= k; ++i) {
            result = result * (total_steps - k + i) / i;
        }

        return static_cast<int>(result);
    }
};
```

#### Python 3
```python
import math


class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        return math.comb(m + n - 2, min(m - 1, n - 1))
```

#### Java 21
```java
class Solution {
    public int uniquePaths(int m, int n) {
        int totalSteps = m + n - 2;
        int k = Math.min(m - 1, n - 1);
        long result = 1;

        for (int i = 1; i <= k; i++) {
            result = result * (totalSteps - k + i) / i;
        }

        return (int) result;
    }
}
```

#### TypeScript
```typescript
function uniquePaths(m: number, n: number): number {
    const totalSteps = m + n - 2;
    const k = Math.min(m - 1, n - 1);
    let result = 1;

    for (let i = 1; i <= k; i++) {
        result = (result * (totalSteps - k + i)) / i;
    }

    return Math.round(result);
}
```

#### Go
```go
package main

func uniquePaths(m int, n int) int {
	totalSteps := m + n - 2
	k := m - 1
	if n-1 < k {
		k = n - 1
	}

	result := int64(1)
	for i := 1; i <= k; i++ {
		result = result * int64(totalSteps-k+i) / int64(i)
	}

	return int(result)
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn unique_paths(m: i32, n: i32) -> i32 {
        let total_steps = (m + n - 2) as i64;
        let k = (m - 1).min(n - 1) as i64;
        let mut result = 1i64;

        for i in 1..=k {
            result = result * (total_steps - k + i) / i;
        }

        result as i32
    }
}
```

---

## 4. Tier 2: Rolling 1D Dynamic Programming Array

### 4.1 Mechanical Description
Observe that `dp[i][j]` only depends on `dp[i-1][j]` (the cell directly above) and `dp[i][j-1]` (the cell directly to the left).
We can compress the grid into a single row of size $n$:
Initialize `dp` of size $n$ filled with 1s.
For each row from 1 to $m-1$:
For each column $j$ from 1 to $n-1$:
Update `dp[j] = dp[j] + dp[j - 1]`.
Here `dp[j]` on the right side represents the value from row $i - 1$, and `dp[j - 1]` represents the updated value in the current row.

```python
def uniquePaths1D(m: int, n: int) -> int:
    if m < n:
        m, n = n, m
    dp = [1] * n
    for _ in range(1, m):
        for j in range(1, n):
            dp[j] += dp[j - 1]
    return dp[n - 1]
```

### 4.2 Trade-offs
- Naturally extends to grids with obstacles (LeetCode 63).
- Uses $O(\min(m, n))$ space, fitting completely inside L1 CPU cache.
- Runs in $O(m \cdot n)$ time compared to $O(\min(m, n))$ for combinatorics.

---

## 5. Tier 3: Full 2D Dynamic Programming Tabulation

### 5.1 Mechanical Description
Create an $m \times n$ matrix `dp`.
Base Cases: `dp[i][0] = 1` for all $i$, and `dp[0][j] = 1` for all $j$.
For each $i \in [1, m-1]$ and $j \in [1, n-1]$:
`dp[i][j] = dp[i - 1][j] + dp[i][j - 1]`.
Return `dp[m - 1][n - 1]`.

### 5.2 Trade-offs
- Conceptually clearest representation matching Pascal's triangle.
- Allocates unnecessary $O(m \cdot n)$ matrix memory on the heap.

---

## 6. Tier 4: Top-Down Recursion with Memoization

### 6.1 Mechanical Description
Define `dfs(i, j)` returning the number of paths from `(i, j)` to `(m-1, n-1)`.
`dfs(i, j) = dfs(i + 1, j) + dfs(i, j + 1)`, cached inside a memoization table.
Without memoization, the tree has $2^{m+n-2}$ nodes, causing Time Limit Exceeded.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Overflow Safety**: The constraint guarantees the answer $\le 2 \times 10^9$, fitting inside a standard 32-bit signed integer. However, intermediate products like $\text{result} \times (\text{total\_steps} - k + i)$ can exceed $2^{31} - 1$, requiring a 64-bit integer (`long long` / `int64`).
2. **Cache Locality**: In the 1D DP approach, iterating left to right provides sequential linear cache line streaming.
3. **Register-Only Computation**: Tier 1 executes entirely in CPU registers without any heap or stack memory accesses.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single row grid | `m = 1, n = 5` | Returns `1` | $k = \min(0, 4) = 0$; loop does not execute; returns 1. |
| Single column grid | `m = 5, n = 1` | Returns `1` | $k = \min(4, 0) = 0$; loop does not execute; returns 1. |
| Single cell grid | `m = 1, n = 1` | Returns `1` | $k = 0$; returns 1. |
| Square grid | `m = 10, n = 10` | Returns $\binom{18}{9} = 48620$ | Accurately computes symmetric Pascal coefficient. |
| Maximum grid dimension | `m = 100, n = 1` | Returns `1` | Degenerate dimension handled correctly. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why is this problem equivalent to Pascal's Triangle?
Each cell's value is the sum of the cell directly above and to its left. Rotating the grid by 45 degrees yields Pascal's triangle.

### 2. Can intermediate products overflow 64-bit integers?
No. For $m, n \le 100$, the maximum intermediate value during Tier 1 computation remains well below $2^{63} - 1$.

### 3. Why does dividing by `i` always yield an exact integer?
The product of any $i$ consecutive integers is a multiple of $i!$. Hence $\text{result} \times (N - k + i)$ is always divisible by $i$.

### 4. How does Unique Paths II (LeetCode 63) alter this?
When obstacles exist, cells with obstacles provide 0 paths, breaking the closed-form combinatorial formula and requiring dynamic programming.

### 5. Why does Python's `math.comb` work out of the box?
Python 3.8+ includes `math.comb(n, k)`, implemented in C with arbitrary precision arithmetic.

### 6. What is the time complexity difference between Tier 1 and Tier 2?
Tier 1 performs at most 100 iterations ($O(\min(m, n))$). Tier 2 performs $m \times n = 10,000$ iterations ($O(m \cdot n)$).

### 7. Does the order of $m$ and $n$ matter?
No. Because $\binom{m+n-2}{m-1} = \binom{m+n-2}{n-1}$, the grid is symmetric with respect to transposing rows and columns.

### 8. Why choose $\min(m - 1, n - 1)$ for $k$?
Choosing the smaller value minimizes the number of loop iterations in the multiplicative formula.

### 9. Can this problem be solved with matrix exponentiation?
While path counting in graphs can be done via adjacency matrix powers, grid DAG paths have an explicit combinatorial solution that is strictly superior.

### 10. How does this compare with Minimum Path Sum (LeetCode 64)?
Unique Paths counts paths (combinatorics/addition), whereas Minimum Path Sum minimizes cumulative cell costs (Bellman optimality/min).

---

## 10. Related Problems and Systematic Progression Links

- [[0070-Climbing-Stairs]]: 1D dynamic programming path counting.
- [[0078-Subsets]]: Combinatorial choice selection.
- LeetCode 63 (Unique Paths II): Grid path counting with obstacles.
- LeetCode 64 (Minimum Path Sum): Finding optimal cost path in a grid.
- LeetCode 980 (Unique Paths III): Hamiltonian paths visiting every non-obstacle cell.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/unique-paths.cpp)
- [Python Implementation](../Python/unique-paths.py)
- [Java Implementation](../Java/unique-paths.java)
- [TypeScript Implementation](../TypeScript/unique-paths.ts)
- [Go Implementation](../Golang/unique-paths.go)
- [Rust Implementation](../Rust/unique-paths.rs)

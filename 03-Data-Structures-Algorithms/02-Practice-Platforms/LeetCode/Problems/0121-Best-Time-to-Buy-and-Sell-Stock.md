---
id: leetcode-0121-best-time-to-buy-and-sell-stock
title: "LeetCode 0121: Best Time to Buy and Sell Stock"
tags:
  - dsa
  - leetcode
  - array
  - dynamic-programming
  - greedy
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/best-time-to-buy-and-sell-stock/"
---

# LeetCode 0121: Best Time to Buy and Sell Stock

## 1. Problem Formalization and Constraints

You are given an array `prices` where `prices[i]` is the price of a given stock on the $i$-th day.
You want to maximize your profit by choosing a single day to buy one stock and choosing a different day in the future to sell that stock.
Return the maximum profit you can achieve from this transaction.
If you cannot achieve any profit, return `0`.

### Constraints
- $1 \le \text{prices.length} \le 10^5$
- $0 \le \text{prices}[i] \le 10^4$

### Examples
- **Example 1**:
  - Input: `prices = [7,1,5,3,6,4]`
  - Output: `5`
  - Explanation: Buy on day 2 (price = 1) and sell on day 5 (price = 6), profit = $6 - 1 = 5$.
- **Example 2**:
  - Input: `prices = [7,6,4,3,1]`
  - Output: `0`
  - Explanation: In this case, no transactions are done and the max profit is 0.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | One-Pass Running Minimum (Kadane's Variant) | $O(N)$ | $O(1)$ | Tracks global minimum prefix while updating maximum profit greedily. |
| **Tier 2 (Space-Optimized)** | Register-Level State Machine DP | $O(N)$ | $O(1)$ | Maintains hold and release transition states in CPU registers. |
| **Tier 3 (Time-Optimized Alternative)** | Precomputed Prefix Min & Suffix Max | $O(N)$ | $O(N)$ | Explicit tabular precomputation of historical valleys and future peaks. |
| **Tier 4 (Brute Force)** | Exhaustive Pairwise Enumeration | $O(N^2)$ | $O(1)$ | Tests every $(i, j)$ pair with $i < j$; guaranteed TLE on $N = 10^5$. |

---

## 3. Tier 1: Most Optimal Solution (One-Pass Running Minimum)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $\text{prices}[0 \dots N-1]$ be the price series.
If we decide to sell on day $j$, the optimal day to have bought is some day $i < j$ that minimizes $\text{prices}[i]$.
Therefore, the maximum profit achievable by selling on day $j$ is:
$$\text{profit}(j) = \max(0, \text{prices}[j] - \min_{0 \le k < j} \text{prices}[k])$$
The global maximum profit across all possible selling days is:
$$\text{MaxProfit} = \max_{0 \le j < N} \text{profit}(j)$$

We maintain two scalar variables during a single forward pass:
1. `minPrice`: The minimum stock price encountered in $\text{prices}[0 \dots j]$.
2. `maxProfit`: The maximum difference observed between any $\text{prices}[j]$ and the preceding `minPrice`.

**Inductive Invariant**:
At step $j$, `minPrice` holds $\min_{0 \le k \le j} \text{prices}[k]$ and `maxProfit` holds $\max_{0 \le k \le j} (\text{prices}[k] - \min_{0 \le m < k} \text{prices}[m])$.
Advancing to $j+1$ requires only constant time updates, proving optimality in $O(N)$ time.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly $N$ elements are processed with constant-time operations per iteration.
- **Space Complexity**: $O(1)$. Auxiliary memory is strictly bounded to two scalar primitive variables.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>
#include <climits>

class Solution {
public:
    int maxProfit(const std::vector<int>& prices) {
        int minPrice = INT_MAX;
        int maxProfit = 0;
        for (int price : prices) {
            if (price < minPrice) {
                minPrice = price;
            } else if (price - minPrice > maxProfit) {
                maxProfit = price - minPrice;
            }
        }
        return maxProfit;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        min_price = float('inf')
        max_profit = 0
        for price in prices:
            if price < min_price:
                min_price = price
            elif price - min_price > max_profit:
                max_profit = price - min_price
        return max_profit
```

#### Java 21
```java
class Solution {
    public int maxProfit(int[] prices) {
        int minPrice = Integer.MAX_VALUE;
        int maxProfit = 0;
        for (int price : prices) {
            if (price < minPrice) {
                minPrice = price;
            } else if (price - minPrice > maxProfit) {
                maxProfit = price - minPrice;
            }
        }
        return maxProfit;
    }
}
```

#### TypeScript
```typescript
function maxProfit(prices: number[]): number {
    let minPrice = Infinity;
    let maxProfit = 0;
    for (const price of prices) {
        if (price < minPrice) {
            minPrice = price;
        } else if (price - minPrice > maxProfit) {
            maxProfit = price - minPrice;
        }
    }
    return maxProfit;
}
```

#### Go
```go
package main

import "math"

func maxProfit(prices []int) int {
    minPrice := math.MaxInt32
    maxProfit := 0
    for _, price := range prices {
        if price < minPrice {
            minPrice = price
        } else if price-minPrice > maxProfit {
            maxProfit = price - minPrice
        }
    }
    return maxProfit
}
```

#### Rust
```rust
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let mut min_price = i32::MAX;
        let mut max_profit = 0;
        for price in prices {
            if price < min_price {
                min_price = price;
            } else if price - min_price > max_profit {
                max_profit = price - min_price;
            }
        }
        max_profit
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized Solution (State Machine DP)

### 4.1 Algorithmic Mechanics and State Formulations

We formulate the problem as a finite state machine tracking two states at day $i$:
1. `hold`: Maximum balance after buying a stock (since we can only buy once, balance is $-\text{price}$).
2. `release`: Maximum balance after selling the stock ($\text{hold} + \text{price}$).

State recurrence transitions:
$$\text{hold}_{i} = \max(\text{hold}_{i-1}, -\text{prices}[i])$$
$$\text{release}_{i} = \max(\text{release}_{i-1}, \text{hold}_{i-1} + \text{prices}[i])$$

By keeping both states in local registers, space is rigorously $O(1)$ and maps directly onto general $k$-transaction DP formulations.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ single-pass scan.
- **Space Complexity**: $O(1)$ auxiliary memory.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>
#include <climits>

class Solution {
public:
    int maxProfit(const std::vector<int>& prices) {
        int hold = INT_MIN;
        int release = 0;
        for (int p : prices) {
            hold = std::max(hold, -p);
            release = std::max(release, hold + p);
        }
        return release;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        hold = float('-inf')
        release = 0
        for p in prices:
            hold = max(hold, -p)
            release = max(release, hold + p)
        return release
```

#### Java 21
```java
class Solution {
    public int maxProfit(int[] prices) {
        int hold = Integer.MIN_VALUE;
        int release = 0;
        for (int p : prices) {
            hold = Math.max(hold, -p);
            release = Math.max(release, hold + p);
        }
        return release;
    }
}
```

#### TypeScript
```typescript
function maxProfit(prices: number[]): number {
    let hold = -Infinity;
    let release = 0;
    for (const p of prices) {
        hold = Math.max(hold, -p);
        release = Math.max(release, hold + p);
    }
    return release;
}
```

#### Go
```go
package main

import "math"

func maxProfit(prices []int) int {
    hold := math.MinInt32
    release := 0
    for _, p := range prices {
        if -p > hold {
            hold = -p
        }
        if hold+p > release {
            release = hold + p
        }
    }
    return release
}
```

#### Rust
```rust
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let mut hold = i32::MIN;
        let mut release = 0;
        for p in prices {
            hold = hold.max(-p);
            release = release.max(hold + p);
        }
        release
    }
}
```

---

## 5. Tier 3: Time-Complexity Optimized Alternative (Prefix Min & Suffix Max)

### 5.1 Algorithmic Mechanics and Array Decomposition

To trade memory for structural explicitness, we precalculate two auxiliary vectors:
1. `prefixMin[i]`: Minimum price in $\text{prices}[0 \dots i]$.
2. `suffixMax[i]`: Maximum price in $\text{prices}[i \dots N-1]$.

The maximum profit across all buy dates $i$ is then directly given by:
$$\max_{0 \le i < N} (\text{suffixMax}[i] - \text{prefixMin}[i])$$

This pattern decomposes time into forward and backward dynamic sweeps, useful for problems requiring bidirectional window queries.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ with three linear iterations.
- **Space Complexity**: $O(N)$ auxiliary memory for the two prefix and suffix tables.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxProfit(const std::vector<int>& prices) {
        int n = prices.size();
        if (n <= 1) return 0;
        std::vector<int> prefixMin(n), suffixMax(n);
        prefixMin[0] = prices[0];
        for (int i = 1; i < n; ++i) {
            prefixMin[i] = std::min(prefixMin[i - 1], prices[i]);
        }
        suffixMax[n - 1] = prices[n - 1];
        for (int i = n - 2; i >= 0; --i) {
            suffixMax[i] = std::max(suffixMax[i + 1], prices[i]);
        }
        int maxProfit = 0;
        for (int i = 0; i < n; ++i) {
            maxProfit = std::max(maxProfit, suffixMax[i] - prefixMin[i]);
        }
        return maxProfit;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        n = len(prices)
        if n <= 1:
            return 0
        prefix_min = [0] * n
        suffix_max = [0] * n
        prefix_min[0] = prices[0]
        for i in range(1, n):
            prefix_min[i] = min(prefix_min[i - 1], prices[i])
        suffix_max[-1] = prices[-1]
        for i in range(n - 2, -1, -1):
            suffix_max[i] = max(suffix_max[i + 1], prices[i])
        return max(s - p for s, p in zip(suffix_max, prefix_min))
```

#### Java 21
```java
class Solution {
    public int maxProfit(int[] prices) {
        int n = prices.length;
        if (n <= 1) return 0;
        int[] prefixMin = new int[n];
        int[] suffixMax = new int[n];
        prefixMin[0] = prices[0];
        for (int i = 1; i < n; i++) {
            prefixMin[i] = Math.min(prefixMin[i - 1], prices[i]);
        }
        suffixMax[n - 1] = prices[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            suffixMax[i] = Math.max(suffixMax[i + 1], prices[i]);
        }
        int maxProfit = 0;
        for (int i = 0; i < n; i++) {
            maxProfit = Math.max(maxProfit, suffixMax[i] - prefixMin[i]);
        }
        return maxProfit;
    }
}
```

#### TypeScript
```typescript
function maxProfit(prices: number[]): number {
    const n = prices.length;
    if (n <= 1) return 0;
    const prefixMin = new Int32Array(n);
    const suffixMax = new Int32Array(n);
    prefixMin[0] = prices[0];
    for (let i = 1; i < n; i++) {
        prefixMin[i] = Math.min(prefixMin[i - 1], prices[i]);
    }
    suffixMax[n - 1] = prices[n - 1];
    for (let i = n - 2; i >= 0; i--) {
        suffixMax[i] = Math.max(suffixMax[i + 1], prices[i]);
    }
    let maxProfit = 0;
    for (let i = 0; i < n; i++) {
        maxProfit = Math.max(maxProfit, suffixMax[i] - prefixMin[i]);
    }
    return maxProfit;
}
```

#### Go
```go
package main

func maxProfit(prices []int) int {
    n := len(prices)
    if n <= 1 {
        return 0
    }
    prefixMin := make([]int, n)
    suffixMax := make([]int, n)
    prefixMin[0] = prices[0]
    for i := 1; i < n; i++ {
        if prices[i] < prefixMin[i-1] {
            prefixMin[i] = prices[i]
        } else {
            prefixMin[i] = prefixMin[i-1]
        }
    }
    suffixMax[n-1] = prices[n-1]
    for i := n - 2; i >= 0; i-- {
        if prices[i] > suffixMax[i+1] {
            suffixMax[i] = prices[i]
        } else {
            suffixMax[i] = suffixMax[i+1]
        }
    }
    maxProfit := 0
    for i := 0; i < n; i++ {
        diff := suffixMax[i] - prefixMin[i]
        if diff > maxProfit {
            maxProfit = diff
        }
    }
    return maxProfit
}
```

#### Rust
```rust
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let n = prices.len();
        if n <= 1 {
            return 0;
        }
        let mut prefix_min = vec![0; n];
        let mut suffix_max = vec![0; n];
        prefix_min[0] = prices[0];
        for i in 1..n {
            prefix_min[i] = prefix_min[i - 1].min(prices[i]);
        }
        suffix_max[n - 1] = prices[n - 1];
        for i in (0..n - 1).rev() {
            suffix_max[i] = suffix_max[i + 1].max(prices[i]);
        }
        let mut max_profit = 0;
        for i in 0..n {
            max_profit = max_profit.max(suffix_max[i] - prefix_min[i]);
        }
        max_profit
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Exhaustive Pairwise Enumeration)

### 6.1 Algorithmic Mechanics and Exhaustion Search

We iterate over all valid buy-sell pairs $(i, j)$ such that $0 \le i < j < N$.
For each pair, we calculate the potential profit $\text{prices}[j] - \text{prices}[i]$ and record the maximum across all $\frac{N(N-1)}{2}$ combinations.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Total pairwise iterations: $\sum_{i=0}^{N-1} (N - 1 - i) = \frac{N(N-1)}{2} \approx 5 \times 10^9$ operations for $N = 10^5$, exceeding typical 1-second CPU quotas.
- **Space Complexity**: $O(1)$ auxiliary space.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxProfit(const std::vector<int>& prices) {
        int maxProfit = 0;
        int n = prices.size();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                maxProfit = std::max(maxProfit, prices[j] - prices[i]);
            }
        }
        return maxProfit;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        max_profit = 0
        n = len(prices)
        for i in range(n):
            for j in range(i + 1, n):
                profit = prices[j] - prices[i]
                if profit > max_profit:
                    max_profit = profit
        return max_profit
```

#### Java 21
```java
class Solution {
    public int maxProfit(int[] prices) {
        int maxProfit = 0;
        int n = prices.length;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                int profit = prices[j] - prices[i];
                if (profit > maxProfit) {
                    maxProfit = profit;
                }
            }
        }
        return maxProfit;
    }
}
```

#### TypeScript
```typescript
function maxProfit(prices: number[]): number {
    let maxProfit = 0;
    const n = prices.length;
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            const profit = prices[j] - prices[i];
            if (profit > maxProfit) {
                maxProfit = profit;
            }
        }
    }
    return maxProfit;
}
```

#### Go
```go
package main

func maxProfit(prices []int) int {
    maxProfit := 0
    n := len(prices)
    for i := 0; i < n; i++ {
        for j := i + 1; j < n; j++ {
            profit := prices[j] - prices[i]
            if profit > maxProfit {
                maxProfit = profit
            }
        }
    }
    return maxProfit
}
```

#### Rust
```rust
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let mut max_profit = 0;
        let n = prices.len();
        for i in 0..n {
            for j in (i + 1)..n {
                let profit = prices[j] - prices[i];
                if profit > max_profit {
                    max_profit = profit;
                }
            }
        }
        max_profit
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. How is LeetCode 121 mathematically isomorphic to Kadane's Maximum Subarray problem?</summary>
Let $\Delta_i = \text{prices}[i] - \text{prices}[i-1]$ for $i \ge 1$ be the daily differences.
The profit between buy day $i$ and sell day $j$ is $\text{prices}[j] - \text{prices}[i] = \sum_{k=i+1}^j \Delta_k$.
Finding the maximum price difference is identical to finding the maximum contiguous subarray sum over the difference sequence $\Delta$.
Kadane's algorithm computes this in $O(N)$ time and $O(1)$ space.
</details>

<details>
<summary>2. Why does updating `minPrice` after checking `price - minPrice` yield identical results?</summary>
If `price < minPrice`, then `price - minPrice` is strictly negative or zero.
Since `maxProfit` is initialized to $0$ and cannot be negative, evaluating the difference before updating `minPrice` cannot produce a new maximum profit.
Branching mutually exclusively via `else if` saves a redundant subtraction when a new minimum price is found.
</details>

<details>
<summary>3. Can this problem be solved using a Divide and Conquer paradigm?</summary>
Yes. Dividing the array into left and right halves yields three possibilities for the optimal transaction:
1. Both buy and sell occur in the left subarray.
2. Both buy and sell occur in the right subarray.
3. Buy occurs in the left subarray and sell occurs in the right subarray (maximum in right minus minimum in left).
The recurrence relation is $T(N) = 2T(N/2) + O(1)$ when tracking min/max in post-order, running in $O(N)$ time and $O(\log N)$ stack frames.
</details>

<details>
<summary>4. What prevents this algorithm from selling before buying?</summary>
The loop processes elements in strict chronological order from index $0$ to $N-1$.
The variable `minPrice` only reflects values observed strictly up to or including the current day.
At no point can a future price influence the current `minPrice`, guaranteeing causality.
</details>

<details>
<summary>5. How does this solution extend to LeetCode 122 (unlimited transactions)?</summary>
In LeetCode 122, whenever $\text{prices}[i] > \text{prices}[i-1]$, we immediately take the profit $\text{prices}[i] - \text{prices}[i-1]$.
Because multiple transactions are allowed, every upward slope contributes to total profit, accumulating all positive daily increments greedily.
</details>

<details>
<summary>6. How does this solution extend to LeetCode 123 (at most two transactions)?</summary>
We extend the state machine to track four states: `buy1`, `sell1`, `buy2`, and `sell2`.
Each state represents the maximum profit achievable after that transaction step, running in $O(N)$ time and $O(1)$ auxiliary space.
</details>

<details>
<summary>7. What hardware-level optimizations benefit the one-pass loop in modern CPUs?</summary>
The loop has high branch predictability because `minPrice` is updated infrequently once a low valley is hit.
The array access pattern is strictly contiguous, allowing hardware L1 cache stream prefetchers to anticipate memory lines seamlessly.
Modern compilers can autovectorize the prefix minimum using SIMD instructions (`_mm256_min_epi32`).
</details>

<details>
<summary>8. In Rust, why do we pass `prices: Vec<i32>` rather than `&[i32]`?</summary>
LeetCode standard harness passes owned vectors (`Vec<i32>`).
In production Rust, accepting a slice `&[i32]` is more idiomatic because it avoids ownership transfer and enables zero-copy execution on arrays, vectors, and sub-slices alike.
</details>

<details>
<summary>9. What is the danger of initializing `minPrice` to `0` instead of infinity?</summary>
Since stock prices are non-negative ($0 \le \text{prices}[i]$), initializing `minPrice = 0` would falsely set the buy price to $0$ if no price is smaller than $0$.
If the actual prices are all strictly positive (for example, `[5, 4, 3]`), `minPrice` would stay $0$, yielding incorrect profits.
Initializing `minPrice` to $\infty$ or $\text{prices}[0]$ guarantees valid initialization.
</details>

<details>
<summary>10. How does memory alignment impact the performance of large price arrays?</summary>
64-byte cache line alignment ensures that four 16-byte SIMD blocks fit cleanly across cache line boundaries without false sharing or unaligned penalty splits.
For high-frequency algorithmic trading systems, keeping price arrays aligned to page boundaries reduces TLB misses during fast scans.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/best-time-to-buy-and-sell-stock.cpp)
- [Python Implementation](../Python/best-time-to-buy-and-sell-stock.py)
- [Java Implementation](../Java/best-time-to-buy-and-sell-stock.java)
- [TypeScript Implementation](../TypeScript/best-time-to-buy-and-sell-stock.ts)
- [Go Implementation](../Golang/best-time-to-buy-and-sell-stock.go)
- [Rust Implementation](../Rust/best-time-to-buy-and-sell-stock.rs)

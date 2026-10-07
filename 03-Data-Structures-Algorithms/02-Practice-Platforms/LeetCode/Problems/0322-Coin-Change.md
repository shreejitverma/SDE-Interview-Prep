---
id: leetcode-0322-coin-change
title: "LeetCode 0322: Coin Change"
tags:
  - dsa
  - leetcode
  - dynamic-programming
  - breadth-first-search
  - knapsack
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/coin-change/"
---

# LeetCode 0322: Coin Change

## 1. Problem Formalization and Constraints

You are given an integer array `coins` representing coins of different denominations and an integer `amount` representing a total amount of money.
Return the fewest number of coins that you need to make up that amount.
If that amount of money cannot be made up by any combination of the coins, return `-1`.
You may assume that you have an infinite number of each kind of coin.

### Constraints
- $1 \le \text{coins.length} \le 12$
- $1 \le \text{coins}[i] \le 2^{31} - 1$
- $0 \le \text{amount} \le 10^4$

### Examples
- **Example 1**:
  - Input: `coins = [1,2,5], amount = 11`
  - Output: `3`
  - Explanation: $11 = 5 + 5 + 1$ (3 coins).
- **Example 2**:
  - Input: `coins = [2], amount = 3`
  - Output: `-1`
- **Example 3**:
  - Input: `coins = [1], amount = 0`
  - Output: `0`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | 1D Unbounded Knapsack Tabulation | $O(A \times C)$ | $O(A)$ | Iterative bottom-up tabulation; caches minimal coins for all intermediate amounts up to target $A$. |
| **Tier 2 (Space-Optimized / Shortest Path)** | Breadth-First Search on State Graph | $O(A \times C)$ | $O(A)$ | Unweighted shortest path in state transition graph; terminates at first discovery of target $A$. |
| **Tier 3 (Time-Optimized Alternative)** | Top-Down Memoized Recursion | $O(A \times C)$ | $O(A)$ | Explores only reachable subproblems; introduces function call overhead and recursion stack. |
| **Tier 4 (Brute Force)** | Exhaustive Recursive Search | $O(C^A)$ | $O(A)$ | Explores all coin combinations without pruning; exponential tree traversal leads to time limit exceeded. |

---

## 3. Tier 1: Most Optimal Solution (1D Unbounded Knapsack Tabulation)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $dp[i]$ denote the minimum number of coins required to form amount $i$.
The base case is $dp[0] = 0$, since 0 coins are needed to produce amount 0.
For every amount $i$ from $1$ to $A$:
$$dp[i] = \min_{c \in \text{coins}, c \le i} (dp[i - c] + 1)$$
If an amount $i$ cannot be formed by any coin denomination, $dp[i]$ remains set to an initial sentinel value of $A + 1$ (or infinity).
At the end of iteration, if $dp[A] > A$, return $-1$; otherwise, return $dp[A]$.

**Invariant Proof**:
Optimal substructure holds: the optimal coin combination for amount $i$ must consist of one coin of value $c$ plus the optimal coin combination for amount $i - c$.
Since amounts are evaluated in increasing order, $dp[i - c]$ is guaranteed to already hold the optimal minimum coins for amount $i - c$.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(A \times C)$, where $A$ is `amount` and $C$ is the number of coins. The outer loop runs $A$ times and the inner loop checks each of the $C$ coin denominations.
- **Space Complexity**: $O(A)$. A single 1D array of size $A + 1$ stores minimum coin counts.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int coinChange(std::vector<int>& coins, int amount) {
        if (amount < 0) return -1;
        if (amount == 0) return 0;
        std::vector<int> dp(amount + 1, amount + 1);
        dp[0] = 0;

        for (int i = 1; i <= amount; ++i) {
            for (int c : coins) {
                if (c <= i) {
                    dp[i] = std::min(dp[i], dp[i - c] + 1);
                }
            }
        }
        return dp[amount] > amount ? -1 : dp[amount];
    }
};
```

#### Python 3.12
```python
class Solution:
    def coinChange(self, coins: list[int], amount: int) -> int:
        if amount <= 0:
            return 0
        dp = [amount + 1] * (amount + 1)
        dp[0] = 0

        for i in range(1, amount + 1):
            for c in coins:
                if c <= i:
                    dp[i] = min(dp[i], dp[i - c] + 1)

        return -1 if dp[amount] > amount else dp[amount]
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int coinChange(int[] coins, int amount) {
        if (amount <= 0) return 0;
        int[] dp = new int[amount + 1];
        Arrays.fill(dp, amount + 1);
        dp[0] = 0;

        for (int i = 1; i <= amount; i++) {
            for (int c : coins) {
                if (c <= i) {
                    dp[i] = Math.min(dp[i], dp[i - c] + 1);
                }
            }
        }
        return dp[amount] > amount ? -1 : dp[amount];
    }
}
```

#### TypeScript
```typescript
function coinChange(coins: number[], amount: number): number {
    if (amount <= 0) return 0;
    const dp = new Array(amount + 1).fill(amount + 1);
    dp[0] = 0;

    for (let i = 1; i <= amount; i++) {
        for (const c of coins) {
            if (c <= i) {
                dp[i] = Math.min(dp[i], dp[i - c] + 1);
            }
        }
    }
    return dp[amount] > amount ? -1 : dp[amount];
}
```

#### Go
```go
package main

func coinChange(coins []int, amount int) int {
    if amount <= 0 {
        return 0
    }
    dp := make([]int, amount+1)
    for i := 1; i <= amount; i++ {
        dp[i] = amount + 1
    }
    dp[0] = 0

    for i := 1; i <= amount; i++ {
        for _, c := range coins {
            if c <= i && dp[i-c]+1 < dp[i] {
                dp[i] = dp[i-c] + 1
            }
        }
    }

    if dp[amount] > amount {
        return -1
    }
    return dp[amount]
}
```

#### Rust
```rust
impl Solution {
    pub fn coin_change(coins: Vec<i32>, amount: i32) -> i32 {
        if amount <= 0 {
            return 0;
        }
        let amt = amount as usize;
        let mut dp = vec![amount + 1; amt + 1];
        dp[0] = 0;

        for i in 1..=amt {
            for &c in &coins {
                let coin_val = c as usize;
                if c > 0 && coin_val <= i {
                    dp[i] = dp[i].min(dp[i - coin_val] + 1);
                }
            }
        }

        if dp[amt] > amount {
            -1
        } else {
            dp[amt]
        }
    }
}
```

---

## 4. Tier 2: Space-Optimized / Shortest Path Solution (Breadth-First Search on State Graph)

### 4.1 Algorithmic Mechanics and Invariant Proof

The problem can be modeled as finding the shortest path in an unweighted directed graph where:
- Each vertex is an integer amount from $0$ to $A$.
- Directed edges exist from vertex $u$ to vertex $u + c$ for each coin $c$.
Using BFS, we start at source vertex $0$ and traverse level by level.
Because BFS visits vertices in order of increasing path length, the first time vertex $A$ is enqueued, the current level count is strictly the minimum coin count.
A boolean visited array prevents re-exploring amounts already reached at an equal or smaller level.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(A \times C)$. Each amount from $0$ to $A$ is added to the queue at most once and branches into at most $C$ neighbors.
- **Space Complexity**: $O(A)$. The queue and visited bitset store at most $A$ elements.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <queue>

class Solution {
public:
    int coinChange(std::vector<int>& coins, int amount) {
        if (amount == 0) return 0;
        std::queue<int> q;
        std::vector<bool> visited(amount + 1, false);

        q.push(0);
        visited[0] = true;
        int level = 0;

        while (!q.empty()) {
            int size = q.size();
            ++level;
            for (int i = 0; i < size; ++i) {
                int curr = q.front();
                q.pop();
                for (int c : coins) {
                    long long next = static_cast<long long>(curr) + c;
                    if (next == amount) return level;
                    if (next < amount && !visited[next]) {
                        visited[next] = true;
                        q.push(static_cast<int>(next));
                    }
                }
            }
        }
        return -1;
    }
};
```

#### Python 3.12
```python
from collections import deque

class Solution:
    def coinChange(self, coins: list[int], amount: int) -> int:
        if amount == 0:
            return 0
        q = deque([0])
        visited = [False] * (amount + 1)
        visited[0] = True
        level = 0

        while q:
            level += 1
            for _ in range(len(q)):
                curr = q.popleft()
                for c in coins:
                    nxt = curr + c
                    if nxt == amount:
                        return level
                    if nxt < amount and not visited[nxt]:
                        visited[nxt] = True
                        q.append(nxt)
        return -1
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Queue;

class Solution {
    public int coinChange(int[] coins, int amount) {
        if (amount == 0) return 0;
        Queue<Integer> q = new ArrayDeque<>();
        boolean[] visited = new boolean[amount + 1];

        q.add(0);
        visited[0] = true;
        int level = 0;

        while (!q.isEmpty()) {
            int size = q.size();
            level++;
            for (int i = 0; i < size; i++) {
                int curr = q.poll();
                for (int c : coins) {
                    long next = (long) curr + c;
                    if (next == amount) return level;
                    if (next < amount && !visited[(int) next]) {
                        visited[(int) next] = true;
                        q.add((int) next);
                    }
                }
            }
        }
        return -1;
    }
}
```

#### TypeScript
```typescript
function coinChange(coins: number[], amount: number): number {
    if (amount === 0) return 0;
    const q: number[] = [0];
    const visited = new Uint8Array(amount + 1);
    visited[0] = 1;
    let level = 0;

    let head = 0;
    while (head < q.length) {
        const size = q.length - head;
        level++;
        for (let i = 0; i < size; i++) {
            const curr = q[head++];
            for (const c of coins) {
                const next = curr + c;
                if (next === amount) return level;
                if (next < amount && visited[next] === 0) {
                    visited[next] = 1;
                    q.push(next);
                }
            }
        }
    }
    return -1;
}
```

#### Go
```go
package main

func coinChange(coins []int, amount int) int {
    if amount == 0 {
        return 0
    }
    q := []int{0}
    visited := make([]bool, amount+1)
    visited[0] = true
    level := 0

    for len(q) > 0 {
        size := len(q)
        level++
        for i := 0; i < size; i++ {
            curr := q[0]
            q = q[1:]
            for _, c := range coins {
                next := curr + c
                if next == amount {
                    return level
                }
                if next < amount && !visited[next] {
                    visited[next] = true
                    q = append(q, next)
                }
            }
        }
    }
    return -1
}
```

#### Rust
```rust
use std::collections::VecDeque;

impl Solution {
    pub fn coin_change(coins: Vec<i32>, amount: i32) -> i32 {
        if amount == 0 {
            return 0;
        }
        let amt = amount as usize;
        let mut q = VecDeque::new();
        let mut visited = vec![false; amt + 1];

        q.push_back(0);
        visited[0] = true;
        let mut level = 0;

        while !q.is_empty() {
            let size = q.len();
            level += 1;
            for _ in 0..size {
                let curr = q.pop_front().unwrap();
                for &c in &coins {
                    let next = curr + c as usize;
                    if next == amt {
                        return level;
                    }
                    if next < amt && !visited[next] {
                        visited[next] = true;
                        q.push_back(next);
                    }
                }
            }
        }
        -1
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Top-Down Memoized Recursion)

### 5.1 Algorithmic Mechanics and Invariant Proof

In the top-down formulation, we define function `solve(rem)` returning the minimum coins needed to make up remainder `rem`.
A memoization table `memo` records the result for each evaluated remainder:
- If `rem == 0`, return 0.
- If `rem < 0`, return $\infty$.
- If `memo[rem]` has already been computed, return `memo[rem]`.
- Otherwise, compute $\min_{c \in \text{coins}} (\text{solve}(rem - c) + 1)$, store in `memo[rem]`, and return.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(A \times C)$. Each subproblem from $1$ to $A$ is solved once and branches $C$ times.
- **Space Complexity**: $O(A)$. Array `memo` of size $A + 1$ plus recursion stack depth up to $A$.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int coinChange(std::vector<int>& coins, int amount) {
        std::vector<int> memo(amount + 1, -2);
        return solve(coins, amount, memo);
    }

private:
    int solve(const std::vector<int>& coins, int rem, std::vector<int>& memo) {
        if (rem < 0) return -1;
        if (rem == 0) return 0;
        if (memo[rem] != -2) return memo[rem];

        int min_coins = 1e9;
        for (int c : coins) {
            int res = solve(coins, rem - c, memo);
            if (res >= 0 && res < min_coins) {
                min_coins = res + 1;
            }
        }
        memo[rem] = (min_coins == 1e9) ? -1 : min_coins;
        return memo[rem];
    }
};
```

#### Python 3.12
```python
class Solution:
    def coinChange(self, coins: list[int], amount: int) -> int:
        memo: dict[int, int] = {0: 0}

        def solve(rem: int) -> int:
            if rem < 0:
                return -1
            if rem in memo:
                return memo[rem]
            min_coins = float('inf')
            for c in coins:
                res = solve(rem - c)
                if res != -1:
                    min_coins = min(min_coins, res + 1)
            memo[rem] = -1 if min_coins == float('inf') else int(min_coins)
            return memo[rem]

        return solve(amount)
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int coinChange(int[] coins, int amount) {
        int[] memo = new int[amount + 1];
        Arrays.fill(memo, -2);
        return solve(coins, amount, memo);
    }

    private int solve(int[] coins, int rem, int[] memo) {
        if (rem < 0) return -1;
        if (rem == 0) return 0;
        if (memo[rem] != -2) return memo[rem];

        int minCoins = Integer.MAX_VALUE;
        for (int c : coins) {
            int res = solve(coins, rem - c, memo);
            if (res >= 0 && res < minCoins) {
                minCoins = res + 1;
            }
        }
        memo[rem] = (minCoins == Integer.MAX_VALUE) ? -1 : minCoins;
        return memo[rem];
    }
}
```

#### TypeScript
```typescript
function coinChange(coins: number[], amount: number): number {
    const memo = new Int32Array(amount + 1).fill(-2);

    function solve(rem: number): number {
        if (rem < 0) return -1;
        if (rem === 0) return 0;
        if (memo[rem] !== -2) return memo[rem];

        let minCoins = 1e9;
        for (const c of coins) {
            const res = solve(rem - c);
            if (res >= 0 && res < minCoins) {
                minCoins = res + 1;
            }
        }
        memo[rem] = minCoins === 1e9 ? -1 : minCoins;
        return memo[rem];
    }

    return solve(amount);
}
```

#### Go
```go
package main

func coinChange(coins []int, amount int) int {
    memo := make([]int, amount+1)
    for i := range memo {
        memo[i] = -2
    }

    var solve func(rem int) int
    solve = func(rem int) int {
        if rem < 0 {
            return -1
        }
        if rem == 0 {
            return 0
        }
        if memo[rem] != -2 {
            return memo[rem]
        }
        minCoins := 1000000000
        for _, c := range coins {
            res := solve(rem - c)
            if res >= 0 && res < minCoins {
                minCoins = res + 1
            }
        }
        if minCoins == 1000000000 {
            memo[rem] = -1
        } else {
            memo[rem] = minCoins
        }
        return memo[rem]
    }

    return solve(amount)
}
```

#### Rust
```rust
impl Solution {
    pub fn coin_change(coins: Vec<i32>, amount: i32) -> i32 {
        if amount == 0 {
            return 0;
        }
        let amt = amount as usize;
        let mut memo = vec![-2; amt + 1];

        fn solve(coins: &[i32], rem: i32, memo: &mut [i32]) -> i32 {
            if rem < 0 {
                return -1;
            }
            if rem == 0 {
                return 0;
            }
            let idx = rem as usize;
            if memo[idx] != -2 {
                return memo[idx];
            }
            let mut min_coins = 1_000_000_000;
            for &c in coins {
                let res = solve(coins, rem - c, memo);
                if res >= 0 && res < min_coins {
                    min_coins = res + 1;
                }
            }
            let result = if min_coins == 1_000_000_000 { -1 } else { min_coins };
            memo[idx] = result;
            result
        }

        solve(&coins, amount, &mut memo)
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Exhaustive Recursive Search)

### 6.1 Algorithmic Mechanics and Invariant Proof

The brute force method exhaustively searches all combinations of coins by subtracting every available denomination from the remaining amount.
Without memoization or state pruning, identical subproblems are solved repeatedly across different recursion branches.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(C^A)$, where $C$ is the coin count and $A$ is the total amount. Each node branches $C$ ways down to depth $A$.
- **Space Complexity**: $O(A)$ maximum call stack recursion depth.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int coinChange(std::vector<int>& coins, int amount) {
        if (amount == 0) return 0;
        int res = helper(coins, amount);
        return res == 1e9 ? -1 : res;
    }

private:
    int helper(const std::vector<int>& coins, int rem) {
        if (rem < 0) return 1e9;
        if (rem == 0) return 0;
        int min_coins = 1e9;
        for (int c : coins) {
            min_coins = std::min(min_coins, 1 + helper(coins, rem - c));
        }
        return min_coins;
    }
};
```

#### Python 3.12
```python
class Solution:
    def coinChange(self, coins: list[int], amount: int) -> int:
        def helper(rem: int) -> float:
            if rem < 0:
                return float('inf')
            if rem == 0:
                return 0
            min_coins = float('inf')
            for c in coins:
                min_coins = min(min_coins, 1 + helper(rem - c))
            return min_coins

        res = helper(amount)
        return -1 if res == float('inf') else int(res)
```

#### Java 21
```java
class Solution {
    public int coinChange(int[] coins, int amount) {
        if (amount == 0) return 0;
        int res = helper(coins, amount);
        return res >= 1000000000 ? -1 : res;
    }

    private int helper(int[] coins, int rem) {
        if (rem < 0) return 1000000000;
        if (rem == 0) return 0;
        int minCoins = 1000000000;
        for (int c : coins) {
            minCoins = Math.min(minCoins, 1 + helper(coins, rem - c));
        }
        return minCoins;
    }
}
```

#### TypeScript
```typescript
function coinChange(coins: number[], amount: number): number {
    function helper(rem: number): number {
        if (rem < 0) return 1e9;
        if (rem === 0) return 0;
        let minCoins = 1e9;
        for (const c of coins) {
            minCoins = Math.min(minCoins, 1 + helper(rem - c));
        }
        return minCoins;
    }

    const res = helper(amount);
    return res >= 1e9 ? -1 : res;
}
```

#### Go
```go
package main

func coinChange(coins []int, amount int) int {
    var helper func(rem int) int
    helper = func(rem int) int {
        if rem < 0 {
            return 1000000000
        }
        if rem == 0 {
            return 0
        }
        minCoins := 1000000000
        for _, c := range coins {
            candidate := 1 + helper(rem - c)
            if candidate < minCoins {
                minCoins = candidate
            }
        }
        return minCoins
    }

    res := helper(amount)
    if res >= 1000000000 {
        return -1
    }
    return res
}
```

#### Rust
```rust
impl Solution {
    pub fn coin_change(coins: Vec<i32>, amount: i32) -> i32 {
        fn helper(coins: &[i32], rem: i32) -> i32 {
            if rem < 0 {
                return 1_000_000_000;
            }
            if rem == 0 {
                return 0;
            }
            let mut min_coins = 1_000_000_000;
            for &c in coins {
                min_coins = min_coins.min(1 + helper(coins, rem - c));
            }
            min_coins
        }

        let res = helper(&coins, amount);
        if res >= 1_000_000_000 {
            -1
        } else {
            res
        }
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why can a greedy approach (always choosing the largest coin denomination first) fail on Coin Change?</summary>
A greedy choice does not exhibit the optimal substructure property for arbitrary coin systems.
For example, given `coins = [1, 3, 4]` and `amount = 6`, a greedy algorithm picks `4 + 1 + 1` (3 coins), whereas the optimal answer is `3 + 3` (2 coins).
Only canonical coin systems (such as standard US currency) guarantee greedy optimality.
</details>

<details>
<summary>2. Why is initializing the DP array with `amount + 1` mathematically sound?</summary>
The smallest coin value is 1, so the maximum possible number of coins needed for any amount $A$ is $A$.
Therefore, $A + 1$ acts as a safe upper bound representing infinity without risking integer overflow when adding 1.
</details>

<details>
<summary>3. What is the fundamental difference between Coin Change 1 (minimum coins) and Coin Change 2 (combination count)?</summary>
Coin Change 1 finds the shortest path or minimum coin count, so the loop ordering (outer amounts vs outer coins) does not affect the optimal minimum.
Coin Change 2 counts unique combinations, requiring coins to be the outer loop to prevent counting permutations like $(1, 2)$ and $(2, 1)$ separately.
</details>

<details>
<summary>4. When is the Breadth-First Search (BFS) solution practically faster than bottom-up DP?</summary>
When the answer requires very few coins (e.g. `amount = 100` and `coins = [100, 1]`), BFS discovers the target on level 1 after checking only one node.
Bottom-up DP unconditionally calculates all intermediate values from 1 to 100 regardless of how quickly the target is reached.
</details>

<details>
<summary>5. How does sorting the coin denominations in descending order optimize the search?</summary>
In top-down DFS or branch-and-bound, trying larger coins first allows the algorithm to find small coin counts rapidly and prune deeper branches whose count already exceeds the current best.
</details>

<details>
<summary>6. What happens when `amount == 0`?</summary>
Zero coins are required to form an amount of 0.
The algorithm handles this as an edge case or via base case $dp[0] = 0$, immediately returning 0.
</details>

<details>
<summary>7. How can 32-bit integer overflow happen when updating `dp[i]`?</summary>
If the initial sentinel value is set to `INT_MAX`, computing `dp[i - c] + 1` wraps around to `INT_MIN` in languages without overflow checks.
Using `amount + 1` avoids overflow entirely.
</details>

<details>
<summary>8. Can Coin Change be solved using matrix exponentiation?</summary>
No. The transitions involve the non-linear $\min$ operation over varying coin offsets rather than linear recurrence additions, making standard matrix exponentiation inapplicable.
</details>

<details>
<summary>9. What is the space complexity advantage of 1D DP over 2D DP for unbounded knapsack?</summary>
Because each coin can be reused infinitely many times, state updates for the current coin depend directly on the updated values within the same row.
This allows in-place overwriting of a single 1D array of size $A + 1$, reducing space from $O(C \times A)$ to $O(A)$.
</details>

<details>
<summary>10. What is the Frobenius Coin Problem and how does it relate to Coin Change?</summary>
The Frobenius Coin Problem asks for the largest integer amount that cannot be formed using a given set of coin denominations that are coprime.
If an amount exceeds the Frobenius number, a valid coin combination is mathematically guaranteed to exist.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/coin-change.cpp)
- [Python Implementation](../Python/coin-change.py)
- [Java Implementation](../Java/coin-change.java)
- [TypeScript Implementation](../TypeScript/coin-change.ts)
- [Go Implementation](../Golang/coin-change.go)
- [Rust Implementation](../Rust/coin-change.rs)

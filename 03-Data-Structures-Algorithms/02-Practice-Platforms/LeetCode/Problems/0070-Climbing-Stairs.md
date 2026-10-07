---
id: leetcode-0070-climbing-stairs
title: "LeetCode 0070: Climbing Stairs"
tags:
  - dsa
  - leetcode
  - dynamic-programming
  - math
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/climbing-stairs/"
---

# LeetCode 0070: Climbing Stairs

## 1. Problem Formalization and Constraints

You are climbing a staircase.
It takes $n$ steps to reach the top.
Each time you can either climb 1 or 2 steps.
In how many distinct ways can you climb to the top?

### Constraints
- $1 \le n \le 45$

### Examples
- **Example 1**:
  - Input: `n = 2`
  - Output: `2`
  - Explanation: There are two ways to climb to the top:
    1. 1 step + 1 step
    2. 2 steps
- **Example 2**:
  - Input: `n = 3`
  - Output: `3`
  - Explanation: There are three ways to climb to the top:
    1. 1 step + 1 step + 1 step
    2. 1 step + 2 steps
    3. 2 steps + 1 step

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Iterative State-Collapsed Fibonacci DP | $O(N)$ | $O(1)$ | Tracks only the previous two states; exact integer precision without floating-point drift. |
| **Tier 2 (Space-Optimized Alternative)** | Logarithmic Matrix Exponentiation | $O(\log N)$ | $O(1)$ | Exponentiates the $2 \times 2$ Fibonacci transition matrix in $\log_2 N$ matrix multiplications. |
| **Tier 3 (Time-Optimized Alternative)** | Linear Dynamic Programming Tabulation Array | $O(N)$ | $O(N)$ | Pre-allocates an array of size $N + 1$ to store complete historical step counts. |
| **Tier 4 (Brute Force)** | Unmemoized Binary Recursion Tree | $O(2^N)$ | $O(N)$ | Evaluates all branches recursively; combinatorial explosion causing immediate TLE. |

---

## 3. Tier 1: Most Optimal Solution (State-Collapsed Fibonacci DP)

### 3.1 Algorithmic Mechanics and Invariant Proof

To reach step $i$, the final move must be either:
1. A 1-step leap from step $i - 1$.
2. A 2-step leap from step $i - 2$.

By the rule of sum for mutually exclusive choices, the total number of distinct ways to reach step $i$ is:
$$\text{ways}(i) = \text{ways}(i - 1) + \text{ways}(i - 2)$$
with base cases $\text{ways}(1) = 1$ and $\text{ways}(2) = 2$.
This sequence is identical to the shifted Fibonacci sequence $F_{n+1}$.

Because $\text{ways}(i)$ depends strictly on the immediately preceding two values, we maintain two scalar variables `prev2` and `prev1`, updating them iteratively in a forward pass.

**Inductive Invariant**:
At iteration $i$, `prev1` holds $\text{ways}(i - 1)$ and `prev2` holds $\text{ways}(i - 2)$.
Computing `current = prev1 + prev2` yields $\text{ways}(i)$ correctly in $O(1)$ time and space.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly $N - 2$ iterations for $N \ge 3$.
- **Space Complexity**: $O(1)$. Auxiliary memory bounded to three scalar primitive integers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    int climbStairs(int n) {
        if (n <= 2) return n;
        int prev2 = 1, prev1 = 2;
        for (int i = 3; i <= n; ++i) {
            int current = prev1 + prev2;
            prev2 = prev1;
            prev1 = current;
        }
        return prev1;
    }
};
```

#### Python 3.12
```python
class Solution:
    def climbStairs(self, n: int) -> int:
        if n <= 2:
            return n
        prev2, prev1 = 1, 2
        for _ in range(3, n + 1):
            prev2, prev1 = prev1, prev1 + prev2
        return prev1
```

#### Java 21
```java
class Solution {
    public int climbStairs(int n) {
        if (n <= 2) {
            return n;
        }
        int prev2 = 1;
        int prev1 = 2;
        for (int i = 3; i <= n; i++) {
            int current = prev1 + prev2;
            prev2 = prev1;
            prev1 = current;
        }
        return prev1;
    }
}
```

#### TypeScript
```typescript
function climbStairs(n: number): number {
    if (n <= 2) {
        return n;
    }
    let prev2 = 1;
    let prev1 = 2;
    for (let i = 3; i <= n; i++) {
        const current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }
    return prev1;
}
```

#### Go
```go
package main

func climbStairs(n int) int {
    if n <= 2 {
        return n
    }
    prev2, prev1 := 1, 2
    for i := 3; i <= n; i++ {
        current := prev1 + prev2
        prev2 = prev1
        prev1 = current
    }
    return prev1
}
```

#### Rust
```rust
impl Solution {
    pub fn climb_stairs(n: i32) -> i32 {
        if n <= 2 {
            return n;
        }
        let mut prev2 = 1;
        let mut prev1 = 2;
        for _ in 3..=n {
            let current = prev1 + prev2;
            prev2 = prev1;
            prev1 = current;
        }
        prev1
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Logarithmic Matrix Exponentiation)

### 4.1 Algorithmic Mechanics

We express the Fibonacci recurrence as a linear state transition system:
$$\begin{bmatrix} F(n+1) \\ F(n) \end{bmatrix} = \begin{bmatrix} 1 & 1 \\ 1 & 0 \end{bmatrix} \begin{bmatrix} F(n) \\ F(n-1) \end{bmatrix}$$
By induction:
$$\begin{bmatrix} F(n+1) \\ F(n) \end{bmatrix} = \begin{bmatrix} 1 & 1 \\ 1 & 0 \end{bmatrix}^n \begin{bmatrix} F(1) \\ F(0) \end{bmatrix}$$
Using binary exponentiation (repeated squaring), the matrix power $M^n$ is evaluated in $O(\log N)$ multiplications.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$.
- **Space Complexity**: $O(1)$ auxiliary scalar matrix registers.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
    using Matrix = std::vector<std::vector<long long>>;

    Matrix multiply(const Matrix& A, const Matrix& B) {
        Matrix C = {{0, 0}, {0, 0}};
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                    C[i][j] += A[i][k] * B[k][j];
        return C;
    }

    Matrix power(Matrix A, int p) {
        Matrix res = {{1, 0}, {0, 1}};
        while (p > 0) {
            if (p & 1) res = multiply(res, A);
            A = multiply(A, A);
            p >>= 1;
        }
        return res;
    }

public:
    int climbStairs(int n) {
        if (n <= 2) return n;
        Matrix T = {{1, 1}, {1, 0}};
        Matrix Tn = power(T, n);
        return Tn[0][0];
    }
};
```

#### Python 3.12
```python
class Solution:
    def climbStairs(self, n: int) -> int:
        if n <= 2:
            return n

        def multiply(A, B):
            return [
                [A[0][0] * B[0][0] + A[0][1] * B[1][0], A[0][0] * B[0][1] + A[0][1] * B[1][1]],
                [A[1][0] * B[0][0] + A[1][1] * B[1][0], A[1][0] * B[0][1] + A[1][1] * B[1][1]]
            ]

        def power(A, p):
            res = [[1, 0], [0, 1]]
            while p > 0:
                if p % 2 == 1:
                    res = multiply(res, A)
                A = multiply(A, A)
                p //= 2
            return res

        T = [[1, 1], [1, 0]]
        Tn = power(T, n)
        return Tn[0][0]
```

#### Java 21
```java
class Solution {
    private int[][] multiply(int[][] A, int[][] B) {
        int[][] C = new int[2][2];
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return C;
    }

    private int[][] power(int[][] A, int p) {
        int[][] res = {{1, 0}, {0, 1}};
        while (p > 0) {
            if ((p & 1) == 1) res = multiply(res, A);
            A = multiply(A, A);
            p >>= 1;
        }
        return res;
    }

    public int climbStairs(int n) {
        if (n <= 2) return n;
        int[][] T = {{1, 1}, {1, 0}};
        int[][] Tn = power(T, n);
        return Tn[0][0];
    }
}
```

#### TypeScript
```typescript
function climbStairs(n: number): number {
    if (n <= 2) return n;

    function multiply(A: number[][], B: number[][]): number[][] {
        return [
            [A[0][0] * B[0][0] + A[0][1] * B[1][0], A[0][0] * B[0][1] + A[0][1] * B[1][1]],
            [A[1][0] * B[0][0] + A[1][1] * B[1][0], A[1][0] * B[0][1] + A[1][1] * B[1][1]]
        ];
    }

    function power(A: number[][], p: number): number[][] {
        let res = [[1, 0], [0, 1]];
        while (p > 0) {
            if ((p & 1) === 1) res = multiply(res, A);
            A = multiply(A, A);
            p >>= 1;
        }
        return res;
    }

    const T = [[1, 1], [1, 0]];
    const Tn = power(T, n);
    return Tn[0][0];
}
```

#### Go
```go
package main

func multiplyMat(A, B [2][2]int) [2][2]int {
    var C [2][2]int
    for i := 0; i < 2; i++ {
        for j := 0; j < 2; j++ {
            for k := 0; k < 2; k++ {
                C[i][j] += A[i][k] * B[k][j]
            }
        }
    }
    return C
}

func powerMat(A [2][2]int, p int) [2][2]int {
    res := [2][2]int{{1, 0}, {0, 1}}
    for p > 0 {
        if p&1 == 1 {
            res = multiplyMat(res, A)
        }
        A = multiplyMat(A, A)
        p >>= 1
    }
    return res
}

func climbStairs(n int) int {
    if n <= 2 {
        return n
    }
    T := [2][2]int{{1, 1}, {1, 0}}
    Tn := powerMat(T, n)
    return Tn[0][0]
}
```

#### Rust
```rust
impl Solution {
    fn multiply(a: [[i64; 2]; 2], b: [[i64; 2]; 2]) -> [[i64; 2]; 2] {
        [
            [a[0][0] * b[0][0] + a[0][1] * b[1][0], a[0][0] * b[0][1] + a[0][1] * b[1][1]],
            [a[1][0] * b[0][0] + a[1][1] * b[1][0], a[1][0] * b[0][1] + a[1][1] * b[1][1]],
        ]
    }

    fn power(mut a: [[i64; 2]; 2], mut p: i32) -> [[i64; 2]; 2] {
        let mut res = [[1, 0], [0, 1]];
        while p > 0 {
            if p & 1 == 1 {
                res = Self::multiply(res, a);
            }
            a = Self::multiply(a, a);
            p >>= 1;
        }
        res
    }

    pub fn climb_stairs(n: i32) -> i32 {
        if n <= 2 {
            return n;
        }
        let t = [[1, 1], [1, 0]];
        let tn = Self::power(t, n);
        tn[0][0] as i32
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Tabulation Array)

### 5.1 Algorithmic Mechanics

We allocate an array `dp` of size $n + 1$ and populate it linearly:
`dp[i] = dp[i-1] + dp[i-2]`.
While requiring $O(N)$ memory, it allows random-access queries for any intermediate step $k \le n$.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear loop.
- **Space Complexity**: $O(N)$ auxiliary array.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    int climbStairs(int n) {
        if (n <= 2) return n;
        std::vector<int> dp(n + 1);
        dp[1] = 1;
        dp[2] = 2;
        for (int i = 3; i <= n; ++i) {
            dp[i] = dp[i - 1] + dp[i - 2];
        }
        return dp[n];
    }
};
```

#### Python 3.12
```python
class Solution:
    def climbStairs(self, n: int) -> int:
        if n <= 2:
            return n
        dp = [0] * (n + 1)
        dp[1] = 1
        dp[2] = 2
        for i in range(3, n + 1):
            dp[i] = dp[i - 1] + dp[i - 2]
        return dp[n]
```

#### Java 21
```java
class Solution {
    public int climbStairs(int n) {
        if (n <= 2) return n;
        int[] dp = new int[n + 1];
        dp[1] = 1;
        dp[2] = 2;
        for (int i = 3; i <= n; i++) {
            dp[i] = dp[i - 1] + dp[i - 2];
        }
        return dp[n];
    }
}
```

#### TypeScript
```typescript
function climbStairs(n: number): number {
    if (n <= 2) return n;
    const dp = new Int32Array(n + 1);
    dp[1] = 1;
    dp[2] = 2;
    for (let i = 3; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    return dp[n];
}
```

#### Go
```go
package main

func climbStairs(n int) int {
    if n <= 2 {
        return n
    }
    dp := make([]int, n+1)
    dp[1] = 1
    dp[2] = 2
    for i := 3; i <= n; i++ {
        dp[i] = dp[i-1] + dp[i-2]
    }
    return dp[n]
}
```

#### Rust
```rust
impl Solution {
    pub fn climb_stairs(n: i32) -> i32 {
        if n <= 2 {
            return n;
        }
        let n_usize = n as usize;
        let mut dp = vec![0; n_usize + 1];
        dp[1] = 1;
        dp[2] = 2;
        for i in 3..=n_usize {
            dp[i] = dp[i - 1] + dp[i - 2];
        }
        dp[n_usize]
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Unmemoized Binary Recursion)

### 6.1 Algorithmic Mechanics

Direct recursive branching: `climbStairs(n) = climbStairs(n - 1) + climbStairs(n - 2)`.
Because subproblems overlap exponentially without memoization, this forms a recursion tree of size $2^N$.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(2^N)$ function calls.
- **Space Complexity**: $O(N)$ recursion call stack frames.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
class Solution {
public:
    int climbStairs(int n) {
        if (n <= 2) return n;
        return climbStairs(n - 1) + climbStairs(n - 2);
    }
};
```

#### Python 3.12
```python
class Solution:
    def climbStairs(self, n: int) -> int:
        if n <= 2:
            return n
        return self.climbStairs(n - 1) + self.climbStairs(n - 2)
```

#### Java 21
```java
class Solution {
    public int climbStairs(int n) {
        if (n <= 2) return n;
        return climbStairs(n - 1) + climbStairs(n - 2);
    }
}
```

#### TypeScript
```typescript
function climbStairs(n: number): number {
    if (n <= 2) return n;
    return climbStairs(n - 1) + climbStairs(n - 2);
}
```

#### Go
```go
package main

func climbStairs(n int) int {
    if n <= 2 {
        return n
    }
    return climbStairs(n-1) + climbStairs(n-2)
}
```

#### Rust
```rust
impl Solution {
    pub fn climb_stairs(n: i32) -> i32 {
        if n <= 2 {
            return n;
        }
        Self::climb_stairs(n - 1) + Self::climb_stairs(n - 2)
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is Binet's closed-form formula $F_n = \frac{\phi^n - \psi^n}{\sqrt{5}}$ risky in competitive programming?</summary>
Binet's formula relies on floating-point arithmetic using $\phi = \frac{1 + \sqrt{5}}{2}$.
Because IEEE 754 double-precision floating-point numbers have only 53 bits of significand precision, rounding errors accumulate when raising $\phi$ to large powers, resulting in inaccurate integer roundings.
Integer matrix exponentiation avoids all floating-point precision hazards.
</details>

<details>
<summary>2. Why does the problem constraint limit $n$ to 45?</summary>
For $n = 45$, $\text{ways}(45) = 1,836,311,903$.
The maximum positive 32-bit signed integer (`INT_MAX`) is $2,147,483,647$.
At $n = 46$, $\text{ways}(46) = 2,971,215,073$, which overflows signed 32-bit integers.
Limiting $n \le 45$ ensures that results fit cleanly within standard 32-bit integers.
</details>

<details>
<summary>3. How does this problem extend to arbitrary step sizes (e.g., steps of size 1, 2, ..., $K$)?</summary>
When step choices are $1 \dots K$, the recurrence becomes $\text{ways}(i) = \sum_{j=1}^K \text{ways}(i - j)$.
We maintain a sliding window sum of the last $K$ elements in $O(1)$ time per step, achieving $O(N)$ overall runtime.
</details>

<details>
<summary>4. What is the relation between Climbing Stairs and Pascal's Triangle?</summary>
The number of ways to climb $n$ steps using $k$ double-steps is given by the binomial coefficient $\binom{n - k}{k}$.
Summing over all possible double-step counts gives $\sum_{k=0}^{\lfloor n/2 \rfloor} \binom{n - k}{k} = F_{n+1}$, representing the shallow diagonals of Pascal's triangle.
</details>

<details>
<summary>5. How does compiler loop unrolling optimize Tier 1?</summary>
Since $n \le 45$ is small, compilers with `-O3` can unroll the entire loop into a sequence of scalar additions.
Under full unrolling, the entire function executes in fewer than 60 CPU cycles.
</details>

<details>
<summary>6. How does Matrix Exponentiation scale if $n = 10^{18}$?</summary>
For $n = 10^{18}$, linear dynamic programming is impossible ($10^{18}$ cycles takes decades).
Matrix exponentiation computes $M^{10^{18}}$ in approximately $\approx 60$ matrix multiplications ($O(\log N)$ time), completing in fractions of a microsecond.
</details>

<details>
<summary>7. What happens if steps have varying costs (Min Cost Climbing Stairs - LC 746)?</summary>
The recurrence shifts from counting paths to finding the minimum cost path:
$\text{cost}(i) = \min(\text{cost}(i - 1), \text{cost}(i - 2)) + \text{price}[i]$.
The state-collapsed two-variable optimization applies identically.
</details>

<details>
<summary>8. How does top-down memoization compare to bottom-up tabulation?</summary>
Top-down memoization evaluates states on demand using recursion and an auxiliary memo table.
Bottom-up tabulation evaluates states systematically in topological order, eliminating recursion stack frames and allowing memory reduction from $O(N)$ to $O(1)$.
</details>

<details>
<summary>9. What CPU instructions accelerate modular Fibonacci calculations on modern hardware?</summary>
Vector fused multiply-add (FMA) instructions compute matrix products efficiently.
For large Fibonacci numbers modulo a prime, Montgomery multiplication intrinsics accelerate the modular arithmetic.
</details>

<details>
<summary>10. What are the key unit test edge cases for climbing stairs?</summary>
1. Minimal input: $n = 1$ (returns 1).
2. Boundary input: $n = 2$ (returns 2).
3. Small composite input: $n = 3$ (returns 3).
4. Medium input: $n = 10$ (returns 89).
5. Maximum constraint: $n = 45$ (returns 1836311903, verifies 32-bit integer limits).
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/climbing-stairs.cpp)
- [Python Implementation](../Python/climbing-stairs.py)
- [Java Implementation](../Java/climbing-stairs.java)
- [TypeScript Implementation](../TypeScript/climbing-stairs.ts)
- [Go Implementation](../Golang/climbing-stairs.go)
- [Rust Implementation](../Rust/climbing-stairs.rs)

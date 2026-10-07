---
id: leetcode-0152-maximum-product-subarray
title: "LeetCode 0152: Maximum Product Subarray"
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
  - "https://leetcode.com/problems/maximum-product-subarray/"
---

# LeetCode 0152: Maximum Product Subarray

## 1. Problem Formalization and Constraints

Given an integer array `nums`, find a subarray that has the largest product, and return the product.
The test cases are generated so that the answer will fit in a 32-bit integer.
A subarray is a contiguous non-empty sequence of elements within an array.

### Constraints
- $1 \le \text{nums.length} \le 2 \times 10^4$
- $-10 \le \text{nums}[i] \le 10$
- The product of any prefix or suffix of `nums` is guaranteed to fit in a 32-bit integer.

### Examples
- **Example 1**:
  - Input: `nums = [2,3,-2,4]`
  - Output: `6`
  - Explanation: `[2,3]` has the largest product 6.
- **Example 2**:
  - Input: `nums = [-2,0,-1]`
  - Output: `0`
  - Explanation: The result cannot be 2, because `[-2,-1]` is not a contiguous subarray.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Dual Running Extreme Kadane (Min/Max Tracking) | $O(N)$ | $O(1)$ | Swaps running min and max upon encountering negative values to capture sign flips. |
| **Tier 2 (Space-Optimized Alternative)** | Bidirectional Suffix-Prefix Sweep | $O(N)$ | $O(1)$ | Scans forward and backward simultaneously, resetting running product on zero. |
| **Tier 3 (Time-Optimized Alternative)** | Explicit 2D Dynamic Programming Tabulation | $O(N)$ | $O(N)$ | Allocates $2 \times N$ matrix to preserve history of maximum and minimum products. |
| **Tier 4 (Brute Force)** | Exhaustive Subarray Evaluation | $O(N^2)$ | $O(1)$ | Multiplies all contiguous spans $(i, j)$ incrementally; quadratic scaling. |

---

## 3. Tier 1: Most Optimal Solution (Dual Running Extremes)

### 3.1 Algorithmic Mechanics and Invariant Proof

Unlike sum accumulation where negative numbers only decrease the total, multiplication by a negative number reverses signs:
- A large positive product becomes a large negative product.
- A large negative product becomes a large positive product.

Consequently, the optimal subarray product ending at index $i$ requires tracking both:
1. $P_{\max}(i)$: Maximum product ending at index $i$.
2. $P_{\min}(i)$: Minimum product ending at index $i$.

When examining $\text{nums}[i]$:
- If $\text{nums}[i] < 0$, multiplication by $\text{nums}[i]$ maps the minimum to the potential new maximum and vice versa.
  We swap $P_{\max}$ and $P_{\min}$ before updating.
- The recurrence relations are:
  $$P_{\max}(i) = \max(\text{nums}[i], P_{\max}(i-1) \times \text{nums}[i])$$
  $$P_{\min}(i) = \min(\text{nums}[i], P_{\min}(i-1) \times \text{nums}[i])$$
- The global maximum is updated as $\text{Result} = \max(\text{Result}, P_{\max}(i))$.

**Inductive Invariant**:
At step $i$, $P_{\max}$ and $P_{\min}$ store the true global supremum and infimum contiguous products terminating exactly at index $i$.
Their values encompass either the singleton element $\text{nums}[i]$ (restarting at index $i$) or an extension of the optimal sequence ending at $i-1$.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. A single forward loop across $N$ elements with constant arithmetic operations per step.
- **Space Complexity**: $O(1)$. Exactly three scalar registers (`maxProd`, `minProd`, `result`).

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxProduct(const std::vector<int>& nums) {
        int maxProd = nums[0];
        int minProd = nums[0];
        int result = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            int x = nums[i];
            if (x < 0) {
                std::swap(maxProd, minProd);
            }
            maxProd = std::max(x, maxProd * x);
            minProd = std::min(x, minProd * x);
            result = std::max(result, maxProd);
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        max_prod = nums[0]
        min_prod = nums[0]
        result = nums[0]
        for x in nums[1:]:
            if x < 0:
                max_prod, min_prod = min_prod, max_prod
            max_prod = max(x, max_prod * x)
            min_prod = min(x, min_prod * x)
            result = max(result, max_prod)
        return result
```

#### Java 21
```java
class Solution {
    public int maxProduct(int[] nums) {
        int maxProd = nums[0];
        int minProd = nums[0];
        int result = nums[0];
        for (int i = 1; i < nums.length; i++) {
            int x = nums[i];
            if (x < 0) {
                int temp = maxProd;
                maxProd = minProd;
                minProd = temp;
            }
            maxProd = Math.max(x, maxProd * x);
            minProd = Math.min(x, minProd * x);
            result = Math.max(result, maxProd);
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function maxProduct(nums: number[]): number {
    let maxProd = nums[0];
    let minProd = nums[0];
    let result = nums[0];
    for (let i = 1; i < nums.length; i++) {
        const x = nums[i];
        if (x < 0) {
            const temp = maxProd;
            maxProd = minProd;
            minProd = temp;
        }
        maxProd = Math.max(x, maxProd * x);
        minProd = Math.min(x, minProd * x);
        result = Math.max(result, maxProd);
    }
    return result;
}
```

#### Go
```go
package main

func maxProduct(nums []int) int {
    maxProd := nums[0]
    minProd := nums[0]
    result := nums[0]
    for i := 1; i < len(nums); i++ {
        x := nums[i]
        if x < 0 {
            maxProd, minProd = minProd, maxProd
        }
        if x > maxProd*x {
            maxProd = x
        } else {
            maxProd = maxProd * x
        }
        if x < minProd*x {
            minProd = x
        } else {
            minProd = minProd * x
        }
        if maxProd > result {
            result = maxProd
        }
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let mut max_prod = nums[0];
        let mut min_prod = nums[0];
        let mut result = nums[0];
        for &x in nums.iter().skip(1) {
            if x < 0 {
                std::mem::swap(&mut max_prod, &mut min_prod);
            }
            max_prod = x.max(max_prod * x);
            min_prod = x.min(min_prod * x);
            result = result.max(max_prod);
        }
        result
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Bidirectional Prefix/Suffix Scan)

### 4.1 Algorithmic Mechanics

If an array has no zeros:
- If the count of negative numbers is even, the product of the whole array is positive and maximal.
- If the count of negative numbers is odd, the maximal subarray is either the prefix before the last negative number or the suffix after the first negative number.

Whenever a zero appears, it divides the array into independent subproblems.
Scanning the array from left-to-right computes all valid prefix products, while scanning from right-to-left computes all valid suffix products.
Resetting any accumulator to $1$ when it reaches $0$ guarantees that no subarray crosses a zero boundary.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ single loop pass from both ends simultaneously.
- **Space Complexity**: $O(1)$ auxiliary scalar space.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxProduct(const std::vector<int>& nums) {
        int n = nums.size();
        int leftProd = 1, rightProd = 1;
        int maxProd = nums[0];
        for (int i = 0; i < n; ++i) {
            leftProd = (leftProd == 0 ? 1 : leftProd) * nums[i];
            rightProd = (rightProd == 0 ? 1 : rightProd) * nums[n - 1 - i];
            maxProd = std::max({maxProd, leftProd, rightProd});
        }
        return maxProd;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        n = len(nums)
        left_prod = 1
        right_prod = 1
        max_prod = nums[0]
        for i in range(n):
            left_prod = (1 if left_prod == 0 else left_prod) * nums[i]
            right_prod = (1 if right_prod == 0 else right_prod) * nums[n - 1 - i]
            max_prod = max(max_prod, left_prod, right_prod)
        return max_prod
```

#### Java 21
```java
class Solution {
    public int maxProduct(int[] nums) {
        int n = nums.length;
        int leftProd = 1, rightProd = 1;
        int maxProd = nums[0];
        for (int i = 0; i < n; i++) {
            leftProd = (leftProd == 0 ? 1 : leftProd) * nums[i];
            rightProd = (rightProd == 0 ? 1 : rightProd) * nums[n - 1 - i];
            maxProd = Math.max(maxProd, Math.max(leftProd, rightProd));
        }
        return maxProd;
    }
}
```

#### TypeScript
```typescript
function maxProduct(nums: number[]): number {
    const n = nums.length;
    let leftProd = 1;
    let rightProd = 1;
    let maxProd = nums[0];
    for (let i = 0; i < n; i++) {
        leftProd = (leftProd === 0 ? 1 : leftProd) * nums[i];
        rightProd = (rightProd === 0 ? 1 : rightProd) * nums[n - 1 - i];
        maxProd = Math.max(maxProd, leftProd, rightProd);
    }
    return maxProd;
}
```

#### Go
```go
package main

func maxProduct(nums []int) int {
    n := len(nums)
    leftProd, rightProd := 1, 1
    maxProd := nums[0]
    for i := 0; i < n; i++ {
        if leftProd == 0 {
            leftProd = 1
        }
        leftProd *= nums[i]

        if rightProd == 0 {
            rightProd = 1
        }
        rightProd *= nums[n-1-i]

        if leftProd > maxProd {
            maxProd = leftProd
        }
        if rightProd > maxProd {
            maxProd = rightProd
        }
    }
    return maxProd
}
```

#### Rust
```rust
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut left_prod = 1;
        let mut right_prod = 1;
        let mut max_prod = nums[0];
        for i in 0..n {
            left_prod = (if left_prod == 0 { 1 } else { left_prod }) * nums[i];
            right_prod = (if right_prod == 0 { 1 } else { right_prod }) * nums[n - 1 - i];
            max_prod = max_prod.max(left_prod).max(right_prod);
        }
        max_prod
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Explicit 2D DP Table)

### 5.1 Algorithmic Mechanics

We allocate two explicit tabular vectors:
- `dpMax[i]`: Maximal contiguous product ending at index $i$.
- `dpMin[i]`: Minimal contiguous product ending at index $i$.

We populate the vectors using explicit transition checks without register reuse, which allows inspecting past states for backtracking.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear pass.
- **Space Complexity**: $O(N)$ auxiliary memory for the two vectors.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxProduct(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> dpMax(n), dpMin(n);
        dpMax[0] = dpMin[0] = nums[0];
        int result = nums[0];
        for (int i = 1; i < n; ++i) {
            int x = nums[i];
            dpMax[i] = std::max({x, dpMax[i - 1] * x, dpMin[i - 1] * x});
            dpMin[i] = std::min({x, dpMax[i - 1] * x, dpMin[i - 1] * x});
            result = std::max(result, dpMax[i]);
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        n = len(nums)
        dp_max = [0] * n
        dp_min = [0] * n
        dp_max[0] = dp_min[0] = nums[0]
        result = nums[0]
        for i in range(1, n):
            x = nums[i]
            dp_max[i] = max(x, dp_max[i - 1] * x, dp_min[i - 1] * x)
            dp_min[i] = min(x, dp_max[i - 1] * x, dp_min[i - 1] * x)
            result = max(result, dp_max[i])
        return result
```

#### Java 21
```java
class Solution {
    public int maxProduct(int[] nums) {
        int n = nums.length;
        int[] dpMax = new int[n];
        int[] dpMin = new int[n];
        dpMax[0] = dpMin[0] = nums[0];
        int result = nums[0];
        for (int i = 1; i < n; i++) {
            int x = nums[i];
            dpMax[i] = Math.max(x, Math.max(dpMax[i - 1] * x, dpMin[i - 1] * x));
            dpMin[i] = Math.min(x, Math.min(dpMax[i - 1] * x, dpMin[i - 1] * x));
            result = Math.max(result, dpMax[i]);
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function maxProduct(nums: number[]): number {
    const n = nums.length;
    const dpMax = new Int32Array(n);
    const dpMin = new Int32Array(n);
    dpMax[0] = dpMin[0] = nums[0];
    let result = nums[0];
    for (let i = 1; i < n; i++) {
        const x = nums[i];
        dpMax[i] = Math.max(x, dpMax[i - 1] * x, dpMin[i - 1] * x);
        dpMin[i] = Math.min(x, dpMax[i - 1] * x, dpMin[i - 1] * x);
        result = Math.max(result, dpMax[i]);
    }
    return result;
}
```

#### Go
```go
package main

func maxProduct(nums []int) int {
    n := len(nums)
    dpMax := make([]int, n)
    dpMin := make([]int, n)
    dpMax[0] = nums[0]
    dpMin[0] = nums[0]
    result := nums[0]
    for i := 1; i < n; i++ {
        x := nums[i]
        c1 := dpMax[i-1] * x
        c2 := dpMin[i-1] * x

        mx := x
        if c1 > mx {
            mx = c1
        }
        if c2 > mx {
            mx = c2
        }
        dpMax[i] = mx

        mn := x
        if c1 < mn {
            mn = c1
        }
        if c2 < mn {
            mn = c2
        }
        dpMin[i] = mn

        if dpMax[i] > result {
            result = dpMax[i]
        }
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut dp_max = vec![0; n];
        let mut dp_min = vec![0; n];
        dp_max[0] = nums[0];
        dp_min[0] = nums[0];
        let mut result = nums[0];
        for i in 1..n {
            let x = nums[i];
            dp_max[i] = x.max(dp_max[i - 1] * x).max(dp_min[i - 1] * x);
            dp_min[i] = x.min(dp_max[i - 1] * x).min(dp_min[i - 1] * x);
            result = result.max(dp_max[i]);
        }
        result
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Exhaustive Subarray Multiplication)

### 6.1 Algorithmic Mechanics

We compute the product of all subarrays $\text{nums}[i \dots j]$ with $0 \le i \le j < N$.
The outer loop fixes the start index $i$, and the inner loop multiplies by $\text{nums}[j]$ incrementally.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$ scalar multiplications.
- **Space Complexity**: $O(1)$ auxiliary space.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxProduct(const std::vector<int>& nums) {
        int n = nums.size();
        int result = nums[0];
        for (int i = 0; i < n; ++i) {
            int prod = 1;
            for (int j = i; j < n; ++j) {
                prod *= nums[j];
                result = std::max(result, prod);
            }
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        n = len(nums)
        result = nums[0]
        for i in range(n):
            prod = 1
            for j in range(i, n):
                prod *= nums[j]
                if prod > result:
                    result = prod
        return result
```

#### Java 21
```java
class Solution {
    public int maxProduct(int[] nums) {
        int n = nums.length;
        int result = nums[0];
        for (int i = 0; i < n; i++) {
            int prod = 1;
            for (int j = i; j < n; j++) {
                prod *= nums[j];
                result = Math.max(result, prod);
            }
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function maxProduct(nums: number[]): number {
    const n = nums.length;
    let result = nums[0];
    for (let i = 0; i < n; i++) {
        let prod = 1;
        for (let j = i; j < n; j++) {
            prod *= nums[j];
            if (prod > result) {
                result = prod;
            }
        }
    }
    return result;
}
```

#### Go
```go
package main

func maxProduct(nums []int) int {
    n := len(nums)
    result := nums[0]
    for i := 0; i < n; i++ {
        prod := 1
        for j := i; j < n; j++ {
            prod *= nums[j]
            if prod > result {
                result = prod
            }
        }
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut result = nums[0];
        for i in 0..n {
            let mut prod = 1;
            for j in i..n {
                prod *= nums[j];
                result = result.max(prod);
            }
        }
        result
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why does `std::mem::swap(&mut max_prod, &mut min_prod)` simplify the code when `x < 0`?</summary>
When $x < 0$, multiplying by $x$ inverts inequalities: if $A \le B$, then $A \times x \ge B \times x$.
Swapping the values before computing $\max(x, \text{max\_prod} \times x)$ ensures that the negative scalar multiplies the smallest previous product to yield the largest candidate, avoiding duplicate branching.
</details>

<details>
<summary>2. Why does the bidirectional sweep in Tier 2 reset running products to 1 when encountering 0?</summary>
A zero element collapses any containing subarray product to zero.
Because a subarray cannot span across a zero and remain non-zero, the zero partitions the array into independent contiguous components.
Resetting the accumulator to $1$ starts fresh evaluation on the subsequent component.
</details>

<details>
<summary>3. Can integer overflow occur if test cases contain long sequences of non-zero integers?</summary>
In general arithmetic, products grow exponentially ($2^{30}$ exceeds $10^9$).
However, problem constraints guarantee that the maximal product fits within standard signed 32-bit limits.
In real-world applications with arbitrary integers, using 64-bit integers (`int64_t` or `BigInt`) or saturating arithmetic prevents integer overflow.
</details>

<details>
<summary>4. How does this problem differ from the Maximum Subarray Sum problem?</summary>
Maximum Subarray Sum exhibits optimal substructure with monotonic accumulation: adding positive numbers increases sum, adding negative numbers decreases sum.
Maximum Product Subarray lacks monotonic ordering because two negative numbers multiply to form a positive number, requiring tracking both extrema simultaneously.
</details>

<details>
<summary>5. What happens if the array consists entirely of negative numbers?</summary>
If the number of negative elements is even, the product of all elements is positive and maximal.
If the number of negative elements is odd, dropping either the first or last negative element produces an even count, and the bidirectional sweep identifies the maximal prefix or suffix.
</details>

<details>
<summary>6. How can this algorithm be modified to return the exact subarray indices?</summary>
Record the starting index that contributed to each $P_{\max}$ and $P_{\min}$.
When swapping $P_{\max}$ and $P_{\min}$ on negative values, swap their corresponding start indices.
When restarting at $x$, reset the start index to the current index $i$.
</details>

<details>
<summary>7. What instruction-level parallelism (ILP) optimizations apply to the bidirectional sweep?</summary>
In Tier 2, `leftProd` and `rightProd` have zero data dependencies between each other.
The CPU pipeline can issue and execute both multiplier operations concurrently on separate execution ports, doubling throughput on superscalar cores.
</details>

<details>
<summary>8. How do floating-point versions of this problem behave?</summary>
For floating-point values in $(0, 1)$, multiplication decreases magnitudes, adding another inversion condition.
Tracking $\min$ and $\max$ remains necessary, but fractional values require careful handling of underflow toward zero.
</details>

<details>
<summary>9. Why is `nums[0]` used for initialization instead of `1` or `0`?</summary>
If the input array is `[-2]`, initializing to `0` would incorrectly return `0` instead of `-2`.
Initializing to `nums[0]` guarantees that single-element negative arrays return the valid element.
</details>

<details>
<summary>10. What is the cache behavior of the bidirectional sweep versus the single forward pass?</summary>
The single forward pass (Tier 1) accesses memory strictly monotonically from left to right, maintaining a single hardware prefetch stream.
The bidirectional sweep (Tier 2) accesses memory from both ends simultaneously, consuming two active hardware prefetch streams.
Both remain $O(N)$ with optimal L1 cache line hit rates.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/maximum-product-subarray.cpp)
- [Python Implementation](../Python/maximum-product-subarray.py)
- [Java Implementation](../Java/maximum-product-subarray.java)
- [TypeScript Implementation](../TypeScript/maximum-product-subarray.ts)
- [Go Implementation](../Golang/maximum-product-subarray.go)
- [Rust Implementation](../Rust/maximum-product-subarray.rs)

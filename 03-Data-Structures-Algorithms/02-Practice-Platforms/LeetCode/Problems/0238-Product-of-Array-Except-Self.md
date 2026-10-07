---
id: leetcode-0238-product-of-array-except-self
title: "LeetCode 0238: Product of Array Except Self"
tags:
  - dsa
  - leetcode
  - array
  - prefix-sum
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/product-of-array-except-self/"
---

# LeetCode 0238: Product of Array Except Self

## 1. Problem Formalization and Constraints

Given an integer array `nums`, return an array `answer` such that `answer[i]` is equal to the product of all the elements of `nums` except `nums[i]`.
The product of any prefix or suffix of `nums` is guaranteed to fit in a 32-bit integer.
You must write an algorithm that runs in $O(N)$ time and without using the division operation.

### Constraints
- $2 \le \text{nums.length} \le 10^5$
- $-30 \le \text{nums}[i] \le 30$
- The product of any prefix or suffix of `nums` is guaranteed to fit in a 32-bit integer.

### Follow-up
Can you solve the problem in $O(1)$ extra space complexity? (The output array does not count as extra space for space complexity analysis.)

### Examples
- **Example 1**:
  - Input: `nums = [1,2,3,4]`
  - Output: `[24,12,8,6]`
- **Example 2**:
  - Input: `nums = [-1,1,0,-3,3]`
  - Output: `[0,0,9,0,0]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Output Prefix Array + Running Suffix Scalar | $O(N)$ | $O(1)$ extra | Reuses the output array to accumulate prefix products, then updates in reverse with a scalar. |
| **Tier 2 (Space-Optimized Alternative)** | Symmetric Simultaneous In-Place Scan | $O(N)$ | $O(1)$ extra | Dual running scalars scanning prefix from left and suffix from right in a single pass. |
| **Tier 3 (Time-Optimized Alternative)** | Explicit Dual Arrays (Prefix & Suffix Tables) | $O(N)$ | $O(N)$ auxiliary | Separates left prefix and right suffix accumulation into dedicated allocated vectors. |
| **Tier 4 (Brute Force)** | Nested Iteration without Division | $O(N^2)$ | $O(1)$ auxiliary | Re-multiplies all elements for each index while skipping $i == j$; prohibitive $O(N^2)$ complexity. |

---

## 3. Tier 1: Most Optimal Solution (Prefix Accumulator + Running Suffix Scalar)

### 3.1 Algorithmic Mechanics and Invariant Proof

For any index $i$, the product of all elements except $\text{nums}[i]$ decomposes mathematically into:
$$\text{answer}[i] = \left(\prod_{k=0}^{i-1} \text{nums}[k]\right) \times \left(\prod_{k=i+1}^{N-1} \text{nums}[k]\right) = \text{Prefix}[i] \times \text{Suffix}[i]$$
with boundary conditions $\text{Prefix}[0] = 1$ and $\text{Suffix}[N-1] = 1$.

Instead of allocating two auxiliary arrays of size $N$:
1. Pass 1 (Left to Right): Populate `answer[i]` with the running prefix product $\prod_{k=0}^{i-1} \text{nums}[k]$.
   Specifically, `answer[0] = 1`, and for $i \in [1, N-1]$:
   $$\text{answer}[i] = \text{answer}[i-1] \times \text{nums}[i-1]$$
2. Pass 2 (Right to Left): Maintain a single integer scalar variable `right` initialized to $1$.
   Iterating backwards from $i = N-1$ down to $0$:
   Multiply `answer[i]` by `right`.
   Then update `right = right * nums[i]`.

**Correctness Invariant**:
At step $i$ in Pass 2, `answer[i]` contains $\text{Prefix}[i]$, while `right` holds $\prod_{k=i+1}^{N-1} \text{nums}[k] = \text{Suffix}[i]$.
Their product produces the exact target value without auxiliary heap overhead.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly two linear passes over the array of size $N$.
- **Space Complexity**: $O(1)$ auxiliary space beyond the required return buffer.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> productExceptSelf(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> result(n, 1);
        for (int i = 1; i < n; ++i) {
            result[i] = result[i - 1] * nums[i - 1];
        }
        int right = 1;
        for (int i = n - 1; i >= 0; --i) {
            result[i] *= right;
            right *= nums[i];
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def productExceptSelf(self, nums: List[int]) -> List[int]:
        n = len(nums)
        result = [1] * n
        for i in range(1, n):
            result[i] = result[i - 1] * nums[i - 1]
        right = 1
        for i in range(n - 1, -1, -1):
            result[i] *= right
            right *= nums[i]
        return result
```

#### Java 21
```java
class Solution {
    public int[] productExceptSelf(int[] nums) {
        int n = nums.length;
        int[] result = new int[n];
        result[0] = 1;
        for (int i = 1; i < n; i++) {
            result[i] = result[i - 1] * nums[i - 1];
        }
        int right = 1;
        for (int i = n - 1; i >= 0; i--) {
            result[i] *= right;
            right *= nums[i];
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function productExceptSelf(nums: number[]): number[] {
    const n = nums.length;
    const result = new Int32Array(n);
    result[0] = 1;
    for (let i = 1; i < n; i++) {
        result[i] = result[i - 1] * nums[i - 1];
    }
    let right = 1;
    for (let i = n - 1; i >= 0; i--) {
        result[i] *= right;
        right *= nums[i];
    }
    return Array.from(result);
}
```

#### Go
```go
package main

func productExceptSelf(nums []int) []int {
    n := len(nums)
    result := make([]int, n)
    result[0] = 1
    for i := 1; i < n; i++ {
        result[i] = result[i-1] * nums[i-1]
    }
    right := 1
    for i := n - 1; i >= 0; i-- {
        result[i] *= right
        right *= nums[i]
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn product_except_self(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut result = vec![1; n];
        for i in 1..n {
            result[i] = result[i - 1] * nums[i - 1];
        }
        let mut right = 1;
        for i in (0..n).rev() {
            result[i] *= right;
            right *= nums[i];
        }
        result
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Symmetric Dual Accumulator)

### 4.1 Algorithmic Mechanics

We can compute the result in a single pass of length $N$ by simultaneously updating prefix products from the front and suffix products from the back.
We maintain two running scalars:
- `leftProduct`: Accumulating from index $0$ to $N-1$.
- `rightProduct`: Accumulating from index $N-1$ down to $0$.

During iteration step $i$, we scale `result[i] *= leftProduct` and `result[N - 1 - i] *= rightProduct`.
This achieves identical $O(1)$ auxiliary space while demonstrating bidirectional pointer symmetry.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ single loop pass of length $N$.
- **Space Complexity**: $O(1)$ auxiliary memory excluding the result container.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> productExceptSelf(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> result(n, 1);
        int left = 1, right = 1;
        for (int i = 0; i < n; ++i) {
            result[i] *= left;
            left *= nums[i];
            result[n - 1 - i] *= right;
            right *= nums[n - 1 - i];
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def productExceptSelf(self, nums: List[int]) -> List[int]:
        n = len(nums)
        result = [1] * n
        left = 1
        right = 1
        for i in range(n):
            result[i] *= left
            left *= nums[i]
            result[n - 1 - i] *= right
            right *= nums[n - 1 - i]
        return result
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int[] productExceptSelf(int[] nums) {
        int n = nums.length;
        int[] result = new int[n];
        Arrays.fill(result, 1);
        int left = 1, right = 1;
        for (int i = 0; i < n; i++) {
            result[i] *= left;
            left *= nums[i];
            result[n - 1 - i] *= right;
            right *= nums[n - 1 - i];
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function productExceptSelf(nums: number[]): number[] {
    const n = nums.length;
    const result = new Int32Array(n).fill(1);
    let left = 1;
    let right = 1;
    for (let i = 0; i < n; i++) {
        result[i] *= left;
        left *= nums[i];
        result[n - 1 - i] *= right;
        right *= nums[n - 1 - i];
    }
    return Array.from(result);
}
```

#### Go
```go
package main

func productExceptSelf(nums []int) []int {
    n := len(nums)
    result := make([]int, n)
    for i := range result {
        result[i] = 1
    }
    left, right := 1, 1
    for i := 0; i < n; i++ {
        result[i] *= left
        left *= nums[i]
        result[n-1-i] *= right
        right *= nums[n-1-i]
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn product_except_self(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut result = vec![1; n];
        let mut left = 1;
        let mut right = 1;
        for i in 0..n {
            result[i] *= left;
            left *= nums[i];
            result[n - 1 - i] *= right;
            right *= nums[n - 1 - i];
        }
        result
    }
}
```

---

## 5. Tier 3: Time-Complexity Optimized Alternative (Explicit Prefix and Suffix Arrays)

### 5.1 Algorithmic Mechanics

We allocate two independent auxiliary vectors:
- `prefix[i] = \prod_{k=0}^{i-1} \text{nums}[k]`
- `suffix[i] = \prod_{k=i+1}^{N-1} \text{nums}[k]`

We compute both tables sequentially and combine them via `result[i] = prefix[i] * suffix[i]`.
While mathematically intuitive, this introduces $2N$ additional auxiliary words of memory.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ with three sequential linear iterations.
- **Space Complexity**: $O(N)$ auxiliary memory for two intermediate arrays.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> productExceptSelf(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> prefix(n, 1), suffix(n, 1), result(n);
        for (int i = 1; i < n; ++i) {
            prefix[i] = prefix[i - 1] * nums[i - 1];
        }
        for (int i = n - 2; i >= 0; --i) {
            suffix[i] = suffix[i + 1] * nums[i + 1];
        }
        for (int i = 0; i < n; ++i) {
            result[i] = prefix[i] * suffix[i];
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def productExceptSelf(self, nums: List[int]) -> List[int]:
        n = len(nums)
        prefix = [1] * n
        suffix = [1] * n
        for i in range(1, n):
            prefix[i] = prefix[i - 1] * nums[i - 1]
        for i in range(n - 2, -1, -1):
            suffix[i] = suffix[i + 1] * nums[i + 1]
        return [prefix[i] * suffix[i] for i in range(n)]
```

#### Java 21
```java
class Solution {
    public int[] productExceptSelf(int[] nums) {
        int n = nums.length;
        int[] prefix = new int[n];
        int[] suffix = new int[n];
        prefix[0] = 1;
        for (int i = 1; i < n; i++) {
            prefix[i] = prefix[i - 1] * nums[i - 1];
        }
        suffix[n - 1] = 1;
        for (int i = n - 2; i >= 0; i--) {
            suffix[i] = suffix[i + 1] * nums[i + 1];
        }
        int[] result = new int[n];
        for (int i = 0; i < n; i++) {
            result[i] = prefix[i] * suffix[i];
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function productExceptSelf(nums: number[]): number[] {
    const n = nums.length;
    const prefix = new Int32Array(n);
    const suffix = new Int32Array(n);
    prefix[0] = 1;
    for (let i = 1; i < n; i++) {
        prefix[i] = prefix[i - 1] * nums[i - 1];
    }
    suffix[n - 1] = 1;
    for (let i = n - 2; i >= 0; i--) {
        suffix[i] = suffix[i + 1] * nums[i + 1];
    }
    const result = new Array(n);
    for (let i = 0; i < n; i++) {
        result[i] = prefix[i] * suffix[i];
    }
    return result;
}
```

#### Go
```go
package main

func productExceptSelf(nums []int) []int {
    n := len(nums)
    prefix := make([]int, n)
    suffix := make([]int, n)
    prefix[0] = 1
    for i := 1; i < n; i++ {
        prefix[i] = prefix[i-1] * nums[i-1]
    }
    suffix[n-1] = 1
    for i := n - 2; i >= 0; i-- {
        suffix[i] = suffix[i+1] * nums[i+1]
    }
    result := make([]int, n)
    for i := 0; i < n; i++ {
        result[i] = prefix[i] * suffix[i]
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn product_except_self(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut prefix = vec![1; n];
        let mut suffix = vec![1; n];
        for i in 1..n {
            prefix[i] = prefix[i - 1] * nums[i - 1];
        }
        for i in (0..n - 1).rev() {
            suffix[i] = suffix[i + 1] * nums[i + 1];
        }
        let mut result = vec![0; n];
        for i in 0..n {
            result[i] = prefix[i] * suffix[i];
        }
        result
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Nested Loop Multiplication)

### 6.1 Algorithmic Mechanics

For every target index $i \in [0, N-1]$, initiate an inner loop from $j = 0$ to $N-1$.
Whenever $j \ne i$, multiply the running accumulator by $\text{nums}[j]$.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$ scalar multiplications.
- **Space Complexity**: $O(1)$ auxiliary space.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> productExceptSelf(const std::vector<int>& nums) {
        int n = nums.size();
        std::vector<int> result(n, 1);
        for (int i = 0; i < n; ++i) {
            int prod = 1;
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    prod *= nums[j];
                }
            }
            result[i] = prod;
        }
        return result;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def productExceptSelf(self, nums: List[int]) -> List[int]:
        n = len(nums)
        result = [1] * n
        for i in range(n):
            prod = 1
            for j in range(n):
                if i != j:
                    prod *= nums[j]
            result[i] = prod
        return result
```

#### Java 21
```java
class Solution {
    public int[] productExceptSelf(int[] nums) {
        int n = nums.length;
        int[] result = new int[n];
        for (int i = 0; i < n; i++) {
            int prod = 1;
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    prod *= nums[j];
                }
            }
            result[i] = prod;
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function productExceptSelf(nums: number[]): number[] {
    const n = nums.length;
    const result = new Array(n);
    for (let i = 0; i < n; i++) {
        let prod = 1;
        for (let j = 0; j < n; j++) {
            if (i !== j) {
                prod *= nums[j];
            }
        }
        result[i] = prod;
    }
    return result;
}
```

#### Go
```go
package main

func productExceptSelf(nums []int) []int {
    n := len(nums)
    result := make([]int, n)
    for i := 0; i < n; i++ {
        prod := 1
        for j := 0; j < n; j++ {
            if i != j {
                prod *= nums[j]
            }
        }
        result[i] = prod
    }
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn product_except_self(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut result = vec![1; n];
        for i in 0..n {
            let mut prod = 1;
            for j in 0..n {
                if i != j {
                    prod *= nums[j];
                }
            }
            result[i] = prod;
        }
        result
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is computing the total product and dividing by `nums[i]` disallowed?</summary>
The problem explicitly forbids division.
Furthermore, division fails catastrophically when elements are zero:
- If the array contains two or more zeros, every output entry is 0.
- If the array contains exactly one zero at index $k$, every entry is 0 except index $k$, which equals the product of all other elements.
Division by zero triggers a hardware trap or CPU exception.
</details>

<details>
<summary>2. Why does the Prefix-Suffix algorithm handle zeros naturally without special-case branches?</summary>
Zero values simply enter the multiplication chains.
Any prefix or suffix product that crosses an element with value $0$ becomes $0$.
Only the index hosting the single zero has both its preceding prefix and succeeding suffix non-zero, automatically yielding the correct product without conditional branching.
</details>

<details>
<summary>3. Can this problem be solved using logarithmic addition instead of multiplication?</summary>
Mathematically, $\prod x_i = \exp(\sum \ln |x_i|)$.
However, floating-point logarithmic conversions introduce catastrophic precision errors and cannot natively represent signed negatives and zeros without complex edge-case handling.
Fixed-width integer multiplication avoids all IEEE 754 rounding artifacts.
</details>

<details>
<summary>4. What are the memory access and cache locality implications of the backward pass?</summary>
While the forward pass accesses memory with positive unit stride ($i \to i+1$), the reverse pass accesses memory with negative unit stride ($i \to i-1$).
Modern L1/L2 hardware prefetchers detect negative unit stride streams immediately, ensuring zero cache stall penalties.
</details>

<details>
<summary>5. How does compiler autovectorization treat prefix product computations?</summary>
Prefix product represents a loop-carried dependency: $S[i] = S[i-1] \times A[i]$.
Unlike associative additions that can be unrolled with parallel accumulators, strict sequential dependencies limit standard SIMD unrolling unless parallel prefix scan algorithms (Blelloch or Hillis-Steele tree reductions) are used.
</details>

<details>
<summary>6. How can parallel prefix scan be applied to this problem on GPU or multi-core architectures?</summary>
By modeling multiplication over integers as a monoid $(\mathbb{Z}, \times, 1)$, a parallel work-efficient prefix scan computes the prefix products in $O(\log N)$ parallel steps and $O(N)$ total work across multiple cores.
</details>

<details>
<summary>7. Why does Tier 1 achieve true $O(1)$ auxiliary space while returning an array of size $N$?</summary>
Standard competitive programming complexity rules exclude the mandatory output memory from auxiliary space measurement.
Since no dynamically allocated hash tables, trees, or second arrays are instantiated, auxiliary heap space remains strictly $O(1)$.
</details>

<details>
<summary>8. In Java, why is returning primitive `int[]` significantly faster than `List<Integer>`?</summary>
`int[]` represents a single contiguous block of 32-bit values with zero object header overhead.
`List<Integer>` requires an array of 64-bit object references pointing to scattered 24-byte `Integer` heap objects, increasing memory by $>6\times$ and causing heavy pointer chasing.
</details>

<details>
<summary>9. What ensures that intermediate products do not overflow signed 32-bit integer limits?</summary>
The problem constraints explicitly state that the product of any prefix or suffix of `nums` is guaranteed to fit within a 32-bit signed integer.
This formal guarantee prevents undefined behavior from signed arithmetic overflow in C++ and Go.
</details>

<details>
<summary>10. How does the symmetric dual-pointer approach in Tier 2 compare to the two-pass approach in Tier 1?</summary>
Tier 2 runs in a single loop from $0$ to $N-1$, performing both forward and backward updates within the same iteration.
While executing in a single loop body, it accesses memory at both ends of the array simultaneously ($i$ and $N - 1 - i$), maintaining two concurrent cache lines instead of one.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/product-of-array-except-self.cpp)
- [Python Implementation](../Python/product-of-array-except-self.py)
- [Java Implementation](../Java/product-of-array-except-self.java)
- [TypeScript Implementation](../TypeScript/product-of-array-except-self.ts)
- [Go Implementation](../Golang/product-of-array-except-self.go)
- [Rust Implementation](../Rust/product-of-array-except-self.rs)

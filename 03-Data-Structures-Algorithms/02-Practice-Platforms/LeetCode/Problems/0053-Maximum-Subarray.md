---
id: leetcode-0053-maximum-subarray
title: "LeetCode 0053: Maximum Subarray"
tags:
  - dsa
  - leetcode
  - array
  - dynamic-programming
  - divide-and-conquer
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/maximum-subarray/"
---

# LeetCode 0053: Maximum Subarray

## 1. Problem Formalization and Constraints

Given an integer array `nums`, find the subarray with the largest sum, and return its sum.
A subarray is a contiguous non-empty sequence of elements within an array.

### Constraints
- $1 \le \text{nums.length} \le 10^5$
- $-10^4 \le \text{nums}[i] \le 10^4$

### Follow-up
If you have figured out the $O(N)$ solution, try coding another solution using the divide and conquer approach, which is more subtle.

### Examples
- **Example 1**:
  - Input: `nums = [-2,1,-3,4,-1,2,1,-5,4]`
  - Output: `6`
  - Explanation: The subarray `[4,-1,2,1]` has the largest sum `6`.
- **Example 2**:
  - Input: `nums = [1]`
  - Output: `1`
- **Example 3**:
  - Input: `nums = [5,4,-1,7,8]`
  - Output: `23`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Kadane's Algorithm | $O(N)$ | $O(1)$ | Dynamic programming with scalar accumulation; optimal sequential scan. |
| **Tier 2 (Space-Optimized Alternative)** | In-Place Tabulation | $O(N)$ | $O(1)$ | Mutates the input array in-place to store prefix maximums without new memory. |
| **Tier 3 (Time-Optimized Alternative)** | Divide and Conquer (Monoid Merge) | $O(N)$ | $O(\log N)$ | Parallelizable segment tree merge tracking prefix, suffix, total, and best sum. |
| **Tier 4 (Brute Force)** | Exhaustive Subarray Enumeration | $O(N^2)$ | $O(1)$ | Tests every contiguous pair $(i, j)$ with incremental sum accumulation. |

---

## 3. Tier 1: Most Optimal Solution (Kadane's Algorithm)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let $S(i)$ denote the maximum subarray sum ending strictly at index $i$.
For any index $i$, the optimal subarray ending at $i$ either:
1. Extends the optimal subarray ending at $i-1$ by appending $\text{nums}[i]$: $S(i-1) + \text{nums}[i]$.
2. Discards previous history and starts a fresh subarray consisting solely of $\text{nums}[i]$.

This yields the recurrence relation:
$$S(i) = \max(\text{nums}[i], S(i-1) + \text{nums}[i])$$
The global maximum contiguous subarray sum across the entire array is:
$$\text{MaxSum} = \max_{0 \le i < N} S(i)$$

Because $S(i)$ depends only on $S(i-1)$, we track it in a single scalar variable `currentSum`, updating `maxSum` at each step.

**Inductive Invariant**:
At step $i$, `currentSum` holds $\max_{0 \le k \le i} \sum_{j=k}^i \text{nums}[j]$ and `maxSum` holds the maximum sum found across all subarrays in $\text{nums}[0 \dots i]$.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Exactly $N$ scalar operations.
- **Space Complexity**: $O(1)$. Auxiliary space bounded to two primitive variables.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxSubArray(const std::vector<int>& nums) {
        int currentSum = nums[0];
        int maxSum = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            currentSum = std::max(nums[i], currentSum + nums[i]);
            maxSum = std::max(maxSum, currentSum);
        }
        return maxSum;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        current_sum = nums[0]
        max_sum = nums[0]
        for x in nums[1:]:
            current_sum = max(x, current_sum + x)
            max_sum = max(max_sum, current_sum)
        return max_sum
```

#### Java 21
```java
class Solution {
    public int maxSubArray(int[] nums) {
        int currentSum = nums[0];
        int maxSum = nums[0];
        for (int i = 1; i < nums.length; i++) {
            currentSum = Math.max(nums[i], currentSum + nums[i]);
            maxSum = Math.max(maxSum, currentSum);
        }
        return maxSum;
    }
}
```

#### TypeScript
```typescript
function maxSubArray(nums: number[]): number {
    let currentSum = nums[0];
    let maxSum = nums[0];
    for (let i = 1; i < nums.length; i++) {
        currentSum = Math.max(nums[i], currentSum + nums[i]);
        maxSum = Math.max(maxSum, currentSum);
    }
    return maxSum;
}
```

#### Go
```go
package main

func maxSubArray(nums []int) int {
    currentSum := nums[0]
    maxSum := nums[0]
    for i := 1; i < len(nums); i++ {
        if currentSum+nums[i] > nums[i] {
            currentSum += nums[i]
        } else {
            currentSum = nums[i]
        }
        if currentSum > maxSum {
            maxSum = currentSum
        }
    }
    return maxSum
}
```

#### Rust
```rust
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut current_sum = nums[0];
        let mut max_sum = nums[0];
        for &x in nums.iter().skip(1) {
            current_sum = x.max(current_sum + x);
            max_sum = max_sum.max(current_sum);
        }
        max_sum
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (In-Place Array Tabulation)

### 4.1 Algorithmic Mechanics

If the problem environment permits in-place mutation of the input array, we store the recurrence state directly inside the array:
$$\text{nums}[i] = \text{nums}[i] + \max(0, \text{nums}[i-1])$$

At each step, $\text{nums}[i]$ accumulates previous positive contributions.
We track the overall maximum in an accumulator.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear pass.
- **Space Complexity**: $O(1)$ auxiliary space via in-place mutation.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int maxSubArray(std::vector<int>& nums) {
        int maxSum = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i - 1] > 0) {
                nums[i] += nums[i - 1];
            }
            maxSum = std::max(maxSum, nums[i]);
        }
        return maxSum;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        max_sum = nums[0]
        for i in range(1, len(nums)):
            if nums[i - 1] > 0:
                nums[i] += nums[i - 1]
            if nums[i] > max_sum:
                max_sum = nums[i]
        return max_sum
```

#### Java 21
```java
class Solution {
    public int maxSubArray(int[] nums) {
        int maxSum = nums[0];
        for (int i = 1; i < nums.length; i++) {
            if (nums[i - 1] > 0) {
                nums[i] += nums[i - 1];
            }
            maxSum = Math.max(maxSum, nums[i]);
        }
        return maxSum;
    }
}
```

#### TypeScript
```typescript
function maxSubArray(nums: number[]): number {
    let maxSum = nums[0];
    for (let i = 1; i < nums.length; i++) {
        if (nums[i - 1] > 0) {
            nums[i] += nums[i - 1];
        }
        maxSum = Math.max(maxSum, nums[i]);
    }
    return maxSum;
}
```

#### Go
```go
package main

func maxSubArray(nums []int) int {
    maxSum := nums[0]
    for i := 1; i < len(nums); i++ {
        if nums[i-1] > 0 {
            nums[i] += nums[i-1]
        }
        if nums[i] > maxSum {
            maxSum = nums[i]
        }
    }
    return maxSum
}
```

#### Rust
```rust
impl Solution {
    pub fn max_sub_array(mut nums: Vec<i32>) -> i32 {
        let mut max_sum = nums[0];
        for i in 1..nums.len() {
            if nums[i - 1] > 0 {
                nums[i] += nums[i - 1];
            }
            max_sum = max_sum.max(nums[i]);
        }
        max_sum
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Divide and Conquer with Segment Tree Monoid)

### 5.1 Algorithmic Mechanics and Monoid Merge

Divide the array into left $[L, M]$ and right $[M+1, R]$ segments.
For any range, we track a 4-tuple struct $(T, P, S, M)$:
1. `totalSum` ($T$): Sum of all elements in the segment.
2. `prefixSum` ($P$): Maximum subarray sum anchored at the left boundary.
3. `suffixSum` ($S$): Maximum subarray sum anchored at the right boundary.
4. `maxSubSum` ($M$): Maximum contiguous subarray sum anywhere in the segment.

When merging left segment $A$ and right segment $B$:
- $T = A.T + B.T$
- $P = \max(A.P, A.T + B.P)$
- $S = \max(B.S, B.T + A.S)$
- $M = \max(\{A.M, B.M, A.S + B.P\})$

This algebraic merge operation is associative and forms a monoid, enabling tree decomposition and parallel prefix query computation in $O(\log N)$ depth.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ total work ($T(N) = 2T(N/2) + O(1)$).
- **Space Complexity**: $O(\log N)$ recursion call stack depth.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
    struct Node {
        int totalSum;
        int prefixSum;
        int suffixSum;
        int maxSubSum;
    };

    Node merge(const Node& a, const Node& b) {
        return {
            a.totalSum + b.totalSum,
            std::max(a.prefixSum, a.totalSum + b.prefixSum),
            std::max(b.suffixSum, b.totalSum + a.suffixSum),
            std::max({a.maxSubSum, b.maxSubSum, a.suffixSum + b.prefixSum})
        };
    }

    Node solve(const std::vector<int>& nums, int l, int r) {
        if (l == r) {
            return {nums[l], nums[l], nums[l], nums[l]};
        }
        int mid = l + (r - l) / 2;
        Node leftNode = solve(nums, l, mid);
        Node rightNode = solve(nums, mid + 1, r);
        return merge(leftNode, rightNode);
    }

public:
    int maxSubArray(const std::vector<int>& nums) {
        return solve(nums, 0, nums.size() - 1).maxSubSum;
    }
};
```

#### Python 3.12
```python
from typing import List, Tuple

class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        def solve(l: int, r: int) -> Tuple[int, int, int, int]:
            if l == r:
                x = nums[l]
                return (x, x, x, x)
            mid = (l + r) // 2
            t1, p1, s1, m1 = solve(l, mid)
            t2, p2, s2, m2 = solve(mid + 1, r)
            total = t1 + t2
            prefix = max(p1, t1 + p2)
            suffix = max(s2, t2 + s1)
            max_sub = max(m1, m2, s1 + p2)
            return (total, prefix, suffix, max_sub)

        return solve(0, len(nums) - 1)[3]
```

#### Java 21
```java
class Solution {
    static class Node {
        int totalSum, prefixSum, suffixSum, maxSubSum;
        Node(int t, int p, int s, int m) {
            this.totalSum = t;
            this.prefixSum = p;
            this.suffixSum = s;
            this.maxSubSum = m;
        }
    }

    private Node solve(int[] nums, int l, int r) {
        if (l == r) {
            return new Node(nums[l], nums[l], nums[l], nums[l]);
        }
        int mid = l + (r - l) / 2;
        Node left = solve(nums, l, mid);
        Node right = solve(nums, mid + 1, r);
        int total = left.totalSum + right.totalSum;
        int prefix = Math.max(left.prefixSum, left.totalSum + right.prefixSum);
        int suffix = Math.max(right.suffixSum, right.totalSum + left.suffixSum);
        int maxSub = Math.max(Math.max(left.maxSubSum, right.maxSubSum), left.suffixSum + right.prefixSum);
        return new Node(total, prefix, suffix, maxSub);
    }

    public int maxSubArray(int[] nums) {
        return solve(nums, 0, nums.length - 1).maxSubSum;
    }
}
```

#### TypeScript
```typescript
interface SegmentNode {
    totalSum: number;
    prefixSum: number;
    suffixSum: number;
    maxSubSum: number;
}

function maxSubArray(nums: number[]): number {
    function solve(l: number, r: number): SegmentNode {
        if (l === r) {
            const v = nums[l];
            return { totalSum: v, prefixSum: v, suffixSum: v, maxSubSum: v };
        }
        const mid = (l + r) >> 1;
        const left = solve(l, mid);
        const right = solve(mid + 1, r);
        return {
            totalSum: left.totalSum + right.totalSum,
            prefixSum: Math.max(left.prefixSum, left.totalSum + right.prefixSum),
            suffixSum: Math.max(right.suffixSum, right.totalSum + left.suffixSum),
            maxSubSum: Math.max(left.maxSubSum, right.maxSubSum, left.suffixSum + right.prefixSum),
        };
    }
    return solve(0, nums.length - 1).maxSubSub;
}
```

#### Go
```go
package main

type segNode struct {
    totalSum, prefixSum, suffixSum, maxSubSum int
}

func mergeNodes(a, b segNode) segNode {
    total := a.totalSum + b.totalSum
    prefix := a.prefixSum
    if a.totalSum+b.prefixSum > prefix {
        prefix = a.totalSum + b.prefixSum
    }
    suffix := b.suffixSum
    if b.totalSum+a.suffixSum > suffix {
        suffix = b.totalSum + a.suffixSum
    }
    maxSub := a.maxSubSum
    if b.maxSubSum > maxSub {
        maxSub = b.maxSubSum
    }
    if a.suffixSum+b.prefixSum > maxSub {
        maxSub = a.suffixSum + b.prefixSum
    }
    return segNode{total, prefix, suffix, maxSub}
}

func solveSeg(nums []int, l, r int) segNode {
    if l == r {
        v := nums[l]
        return segNode{v, v, v, v}
    }
    mid := l + (r-l)/2
    left := solveSeg(nums, l, mid)
    right := solveSeg(nums, mid+1, r)
    return mergeNodes(left, right)
}

func maxSubArray(nums []int) int {
    return solveSeg(nums, 0, len(nums)-1).maxSubSum
}
```

#### Rust
```rust
struct Node {
    total_sum: i32,
    prefix_sum: i32,
    suffix_sum: i32,
    max_sub_sum: i32,
}

impl Solution {
    fn solve(nums: &[i32], l: usize, r: usize) -> Node {
        if l == r {
            let v = nums[l];
            return Node {
                total_sum: v,
                prefix_sum: v,
                suffix_sum: v,
                max_sub_sum: v,
            };
        }
        let mid = l + (r - l) / 2;
        let left = Self::solve(nums, l, mid);
        let right = Self::solve(nums, mid + 1, r);
        Node {
            total_sum: left.total_sum + right.total_sum,
            prefix_sum: left.prefix_sum.max(left.total_sum + right.prefix_sum),
            suffix_sum: right.suffix_sum.max(right.total_sum + left.suffix_sum),
            max_sub_sum: left.max_sub_sum.max(right.max_sub_sum).max(left.suffix_sum + right.prefix_sum),
        }
    }

    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        Self::solve(&nums, 0, nums.len() - 1).max_sub_sum
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Exhaustive Running Sum)

### 6.1 Algorithmic Mechanics

We test all contiguous subarrays starting at index $i$ and ending at $j$.
An inner loop accumulates the sum sequentially in $O(1)$ per increment.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$ checks.
- **Space Complexity**: $O(1)$ auxiliary space.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>
#include <climits>

class Solution {
public:
    int maxSubArray(const std::vector<int>& nums) {
        int maxSum = INT_MIN;
        int n = nums.size();
        for (int i = 0; i < n; ++i) {
            int currentSum = 0;
            for (int j = i; j < n; ++j) {
                currentSum += nums[j];
                maxSum = std::max(maxSum, currentSum);
            }
        }
        return maxSum;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        max_sum = float('-inf')
        n = len(nums)
        for i in range(n):
            current_sum = 0
            for j in range(i, n):
                current_sum += nums[j]
                if current_sum > max_sum:
                    max_sum = current_sum
        return max_sum
```

#### Java 21
```java
class Solution {
    public int maxSubArray(int[] nums) {
        int maxSum = Integer.MIN_VALUE;
        int n = nums.length;
        for (int i = 0; i < n; i++) {
            int currentSum = 0;
            for (int j = i; j < n; j++) {
                currentSum += nums[j];
                maxSum = Math.max(maxSum, currentSum);
            }
        }
        return maxSum;
    }
}
```

#### TypeScript
```typescript
function maxSubArray(nums: number[]): number {
    let maxSum = -Infinity;
    const n = nums.length;
    for (let i = 0; i < n; i++) {
        let currentSum = 0;
        for (let j = i; j < n; j++) {
            currentSum += nums[j];
            if (currentSum > maxSum) {
                maxSum = currentSum;
            }
        }
    }
    return maxSum;
}
```

#### Go
```go
package main

import "math"

func maxSubArray(nums []int) int {
    maxSum := math.MinInt32
    n := len(nums)
    for i := 0; i < n; i++ {
        currentSum := 0
        for j := i; j < n; j++ {
            currentSum += nums[j]
            if currentSum > maxSum {
                maxSum = currentSum
            }
        }
    }
    return maxSum
}
```

#### Rust
```rust
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut max_sum = i32::MIN;
        let n = nums.len();
        for i in 0..n {
            let mut current_sum = 0;
            for j in i..n {
                current_sum += nums[j];
                max_sum = max_sum.max(current_sum);
            }
        }
        max_sum
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. How does Kadane's algorithm handle arrays consisting entirely of negative integers?</summary>
By initializing both `currentSum` and `maxSum` to `nums[0]` rather than $0$, the algorithm correctly picks the single least negative element.
For example, on `[-3, -1, -5]`, `maxSum` correctly evaluates to `-1`.
</details>

<details>
<summary>2. How can we recover the exact starting and ending indices of the maximal subarray?</summary>
Maintain three index pointers: `bestStart`, `bestEnd`, and `currentStart`.
When `nums[i] > currentSum + nums[i]`, reset `currentStart = i`.
Whenever `currentSum > maxSum`, update `bestStart = currentStart` and `bestEnd = i`.
</details>

<details>
<summary>3. Why is the Divide and Conquer approach valuable despite taking $O(\log N)$ stack space?</summary>
The Divide and Conquer algorithm is associative and forms an algebraic monoid.
This allows it to be embedded directly into a dynamic Segment Tree to support point updates and arbitrary range maximum subarray queries in $O(\log N)$ time per query.
</details>

<details>
<summary>4. What prevents Kadane's algorithm from running in parallel?</summary>
Kadane's recurrence $S(i) = \max(\text{nums}[i], S(i-1) + \text{nums}[i])$ is sequentially serial due to the loop-carried dependency on $S(i-1)$.
The Divide and Conquer monoid structure resolves this by enabling parallel map-reduce across GPU threads.
</details>

<details>
<summary>5. How does compiler instruction scheduling optimize the Kadane loop?</summary>
Because `currentSum = max(x, currentSum + x)` maps directly to a conditional select or conditional move instruction (`cmov` in x86, `csel` in ARM64), compilers produce branchless machine code that avoids branch misprediction penalties.
</details>

<details>
<summary>6. How does Maximum Subarray relate to Prefix Sum min/max tracking?</summary>
Let $P[i] = \sum_{k=0}^i \text{nums}[k]$ be the prefix sum array.
The sum of subarray $\text{nums}[i \dots j]$ is $P[j] - P[i-1]$.
Maximizing this sum corresponds to finding $\max_{j} (P[j] - \min_{k < j} P[k])$, which is mathematically identical to LeetCode 121 (Stock Profit).
</details>

<details>
<summary>7. What is the impact of integer overflow on cumulative sums in large inputs?</summary>
With $N = 10^5$ and $-10^4 \le \text{nums}[i] \le 10^4$, the maximum theoretical sum is $10^9$.
Because $10^9 < 2^{31} - 1 \approx 2.14 \times 10^9$, signed 32-bit integers will never overflow.
</details>

<details>
<summary>8. How does this algorithm extend to 2D matrices (Maximum Submatrix Sum)?</summary>
By fixing two row boundaries $r_1$ and $r_2$ and compressing columns into a 1D array of column sums, running Kadane's algorithm on the 1D compressed array finds the optimal 2D submatrix in $O(R^2 C)$ time.
</details>

<details>
<summary>9. Why is `nums.iter().skip(1)` idiomatic in Rust for this problem?</summary>
It eliminates index boundary checks and avoids panics on empty slices when combined with slice pattern matching.
The compiler optimizes iterator consumption into register-allocated loops with zero bounds-checking overhead.
</details>

<details>
<summary>10. What is the circular variant (LeetCode 918) and how does Kadane adapt to it?</summary>
In a circular array, the maximum subarray either does not wrap around (standard Kadane max) or wraps around the ends.
The wrap-around sum equals the total array sum minus the minimum subarray sum (standard Kadane min).
The answer is $\max(\text{maxSubarray}, \text{totalSum} - \text{minSubarray})$ (unless all elements are negative).
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/maximum-subarray.cpp)
- [Python Implementation](../Python/maximum-subarray.py)
- [Java Implementation](../Java/maximum-subarray.java)
- [TypeScript Implementation](../TypeScript/maximum-subarray.ts)
- [Go Implementation](../Golang/maximum-subarray.go)
- [Rust Implementation](../Rust/maximum-subarray.rs)

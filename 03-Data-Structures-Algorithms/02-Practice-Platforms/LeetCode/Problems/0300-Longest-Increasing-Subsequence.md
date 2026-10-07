---
id: leetcode-0300-longest-increasing-subsequence
title: "LeetCode 0300: Longest Increasing Subsequence"
tags:
  - dsa
  - leetcode
  - binary-search
  - dynamic-programming
  - patience-sorting
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/longest-increasing-subsequence/"
---

# LeetCode 0300: Longest Increasing Subsequence

## 1. Problem Formalization and Constraints

Given an integer array `nums`, return the length of the longest strictly increasing subsequence.
A subsequence is an array that can be derived from another array by deleting some or no elements without changing the order of the remaining elements.

### Constraints
- $1 \le \text{nums.length} \le 2500$
- $-10^4 \le \text{nums}[i] \le 10^4$

### Examples
- **Example 1**:
  - Input: `nums = [10,9,2,5,3,7,101,18]`
  - Output: `4`
  - Explanation: The longest increasing subsequence is `[2,3,7,101]`, therefore the length is 4.
- **Example 2**:
  - Input: `nums = [0,1,0,3,2,3]`
  - Output: `4`
- **Example 3**:
  - Input: `nums = [7,7,7,7,7,7,7]`
  - Output: `1`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Patience Sorting with Binary Search | $O(N \log N)$ | $O(N)$ | Maintains greedy smallest tails array; binary search finds insertion index in logarithmic time. |
| **Tier 2 (Space-Optimized)** | In-Place Patience Sorting | $O(N \log N)$ | $O(1)$ | Overwrites input buffer with greedy tail values; zero auxiliary heap allocations. |
| **Tier 3 (Time-Optimized Alternative)** | 1D Dynamic Programming Tabulation | $O(N^2)$ | $O(N)$ | Explicit subproblem state memoization; intuitive transitions but quadratic worst-case comparisons. |
| **Tier 4 (Brute Force)** | Exhaustive Recursive Search | $O(2^N)$ | $O(N)$ | Explores include/exclude choices for each element; exponential call stack explosion. |

---

## 3. Tier 1: Most Optimal Solution (Patience Sorting with Binary Search)

### 3.1 Algorithmic Mechanics and Invariant Proof

The patience sorting technique maintains an array `tails` where `tails[k]` represents the smallest tail of all increasing subsequences of length `k + 1` observed so far.
For each element $x$ in `nums`:
1. Use binary search (`lower_bound`) to locate the first index `idx` in `tails` such that $\text{tails}[idx] \ge x$.
2. If no such index exists ($x$ is greater than all existing tails), append $x$ to `tails`, extending the maximum LIS length found so far.
3. If an index `idx` is found, update $\text{tails}[idx] = x$. This greedy replacement lowers the tail value of length `idx + 1`, enabling subsequent elements to form longer sequences.

**Invariant Proof**:
The array `tails` remains strictly monotonically increasing at every iteration.
Because `tails` is sorted, binary search runs in $O(\log N)$ time.
At completion, the length of `tails` equals the length of the longest strictly increasing subsequence.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$. Processing each of the $N$ numbers requires a binary search across `tails` of length at most $N$.
- **Space Complexity**: $O(N)$. Auxiliary array `tails` requires at most $N$ elements in the worst case where `nums` is strictly increasing.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int lengthOfLIS(std::vector<int>& nums) {
        std::vector<int> tails;
        for (int x : nums) {
            auto it = std::lower_bound(tails.begin(), tails.end(), x);
            if (it == tails.end()) {
                tails.push_back(x);
            } else {
                *it = x;
            }
        }
        return static_cast<int>(tails.size());
    }
};
```

#### Python 3.12
```python
import bisect

class Solution:
    def lengthOfLIS(self, nums: list[int]) -> int:
        tails: list[int] = []
        for x in nums:
            idx = bisect.bisect_left(tails, x)
            if idx == len(tails):
                tails.append(x)
            else:
                tails[idx] = x
        return len(tails)
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class Solution {
    public int lengthOfLIS(int[] nums) {
        List<Integer> tails = new ArrayList<>();
        for (int x : nums) {
            int idx = Collections.binarySearch(tails, x);
            if (idx < 0) {
                idx = -(idx + 1);
            }
            if (idx == tails.size()) {
                tails.add(x);
            } else {
                tails.set(idx, x);
            }
        }
        return tails.size();
    }
}
```

#### TypeScript
```typescript
function lengthOfLIS(nums: number[]): number {
    const tails: number[] = [];
    for (const x of nums) {
        let left = 0;
        let right = tails.length;
        while (left < right) {
            const mid = (left + right) >> 1;
            if (tails[mid] < x) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        if (left === tails.length) {
            tails.push(x);
        } else {
            tails[left] = x;
        }
    }
    return tails.length;
}
```

#### Go
```go
package main

import "sort"

func lengthOfLIS(nums []int) int {
    tails := make([]int, 0, len(nums))
    for _, x := range nums {
        idx := sort.Search(len(tails), func(i int) bool {
            return tails[i] >= x
        })
        if idx == len(tails) {
            tails = append(tails, x)
        } else {
            tails[idx] = x
        }
    }
    return len(tails)
}
```

#### Rust
```rust
impl Solution {
    pub fn length_of_lis(nums: Vec<i32>) -> i32 {
        let mut tails: Vec<i32> = Vec::with_capacity(nums.len());
        for x in nums {
            let idx = match tails.binary_search(&x) {
                Ok(i) => i,
                Err(i) => i,
            };
            if idx == tails.len() {
                tails.push(x);
            } else {
                tails[idx] = x;
            }
        }
        tails.len() as i32
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized Solution (In-Place Patience Sorting)

### 4.1 Algorithmic Mechanics and Invariant Proof

Instead of allocating an auxiliary array or vector `tails`, we can reuse the prefix of `nums` itself as the `tails` buffer.
We maintain an integer pointer `len` representing the current length of the LIS prefix within `nums`.
For each incoming element $x$ in `nums`, we binary search within the subslice `nums[0 .. len]`.
If $x$ exceeds all values in `nums[0 .. len]`, we write $x$ at `nums[len]` and increment `len`.
Otherwise, we overwrite the first element in `nums[0 .. len]` that is greater than or equal to $x$.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$. Binary search is performed $N$ times over slices of size at most $N$.
- **Space Complexity**: $O(1)$ auxiliary space. The modification happens directly in the caller-provided buffer without extra heap allocation.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int lengthOfLIS(std::vector<int>& nums) {
        int len = 0;
        for (int x : nums) {
            auto it = std::lower_bound(nums.begin(), nums.begin() + len, x);
            if (it == nums.begin() + len) {
                nums[len++] = x;
            } else {
                *it = x;
            }
        }
        return len;
    }
};
```

#### Python 3.12
```python
import bisect

class Solution:
    def lengthOfLIS(self, nums: list[int]) -> int:
        length = 0
        for x in nums:
            idx = bisect.bisect_left(nums, x, 0, length)
            nums[idx] = x
            if idx == length:
                length += 1
        return length
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int lengthOfLIS(int[] nums) {
        int len = 0;
        for (int x : nums) {
            int idx = Arrays.binarySearch(nums, 0, len, x);
            if (idx < 0) {
                idx = -(idx + 1);
            }
            nums[idx] = x;
            if (idx == len) {
                len++;
            }
        }
        return len;
    }
}
```

#### TypeScript
```typescript
function lengthOfLIS(nums: number[]): number {
    let len = 0;
    for (const x of nums) {
        let left = 0;
        let right = len;
        while (left < right) {
            const mid = (left + right) >> 1;
            if (nums[mid] < x) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        nums[left] = x;
        if (left === len) {
            len++;
        }
    }
    return len;
}
```

#### Go
```go
package main

import "sort"

func lengthOfLIS(nums []int) int {
    length := 0
    for _, x := range nums {
        idx := sort.Search(length, func(i int) bool {
            return nums[i] >= x
        })
        nums[idx] = x
        if idx == length {
            length++
        }
    }
    return length
}
```

#### Rust
```rust
impl Solution {
    pub fn length_of_lis(mut nums: Vec<i32>) -> i32 {
        let mut len = 0;
        for i in 0..nums.len() {
            let x = nums[i];
            let idx = match nums[..len].binary_search(&x) {
                Ok(pos) => pos,
                Err(pos) => pos,
            };
            nums[idx] = x;
            if idx == len {
                len += 1;
            }
        }
        len as i32
    }
}
```

---

## 5. Tier 3: Time-Complexity Optimized Alternative Solution (1D Dynamic Programming Tabulation)

### 5.1 Algorithmic Mechanics and Invariant Proof

We define the subproblem $dp[i]$ as the length of the longest strictly increasing subsequence that ends strictly at index $i$.
For every index $i$ from $0$ to $N - 1$:
$$dp[i] = 1 + \max(\{dp[j] \mid 0 \le j < i \text{ and } nums[j] < nums[i]\} \cup \{0\})$$
We initialize each $dp[i] = 1$ since a single element forms an increasing subsequence of length 1.
We iterate through all prior indices $j < i$ to check if extending the sequence ending at $j$ yields a larger value.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Nested loops compare each element with all preceding elements.
- **Space Complexity**: $O(N)$. Table of size $N$ stores intermediate LIS values.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int lengthOfLIS(std::vector<int>& nums) {
        if (nums.empty()) return 0;
        int n = static_cast<int>(nums.size());
        std::vector<int> dp(n, 1);
        int max_len = 1;

        for (int i = 1; i < n; ++i) {
            for (int j = 0; j < i; ++j) {
                if (nums[j] < nums[i]) {
                    dp[i] = std::max(dp[i], dp[j] + 1);
                }
            }
            max_len = std::max(max_len, dp[i]);
        }
        return max_len;
    }
};
```

#### Python 3.12
```python
class Solution:
    def lengthOfLIS(self, nums: list[int]) -> int:
        if not nums:
            return 0
        n = len(nums)
        dp = [1] * n
        for i in range(1, n):
            for j in range(i):
                if nums[j] < nums[i]:
                    dp[i] = max(dp[i], dp[j] + 1)
        return max(dp)
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public int lengthOfLIS(int[] nums) {
        if (nums == null || nums.length == 0) return 0;
        int n = nums.length;
        int[] dp = new int[n];
        Arrays.fill(dp, 1);
        int maxLen = 1;

        for (int i = 1; i < n; i++) {
            for (int j = 0; j < i; j++) {
                if (nums[j] < nums[i]) {
                    dp[i] = Math.max(dp[i], dp[j] + 1);
                }
            }
            maxLen = Math.max(maxLen, dp[i]);
        }
        return maxLen;
    }
}
```

#### TypeScript
```typescript
function lengthOfLIS(nums: number[]): number {
    if (nums.length === 0) return 0;
    const n = nums.length;
    const dp = new Array(n).fill(1);
    let maxLen = 1;

    for (let i = 1; i < n; i++) {
        for (let j = 0; j < i; j++) {
            if (nums[j] < nums[i]) {
                dp[i] = Math.max(dp[i], dp[j] + 1);
            }
        }
        maxLen = Math.max(maxLen, dp[i]);
    }
    return maxLen;
}
```

#### Go
```go
package main

func lengthOfLIS(nums []int) int {
    if len(nums) == 0 {
        return 0
    }
    n := len(nums)
    dp := make([]int, n)
    maxLen := 1

    for i := 0; i < n; i++ {
        dp[i] = 1
        for j := 0; j < i; j++ {
            if nums[j] < nums[i] && dp[j]+1 > dp[i] {
                dp[i] = dp[j] + 1
            }
        }
        if dp[i] > maxLen {
            maxLen = dp[i]
        }
    }
    return maxLen
}
```

#### Rust
```rust
impl Solution {
    pub fn length_of_lis(nums: Vec<i32>) -> i32 {
        if nums.is_empty() {
            return 0;
        }
        let n = nums.len();
        let mut dp = vec![1; n];
        let mut max_len = 1;

        for i in 1..n {
            for j in 0..i {
                if nums[j] < nums[i] {
                    dp[i] = dp[i].max(dp[j] + 1);
                }
            }
            max_len = max_len.max(dp[i]);
        }
        max_len
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Exhaustive Recursive Search)

### 6.1 Algorithmic Mechanics and Invariant Proof

At each element `nums[i]`, we make a binary choice: either include `nums[i]` in the current increasing subsequence (if it is strictly greater than the previous included element), or skip `nums[i]`.
The recursive function explores both branches across all $N$ elements.
Because it evaluates all $2^N$ subsequences without memoization, it provides the baseline theoretical upper bound.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(2^N)$. Binary decision tree of depth $N$ produces $2^N$ leaf nodes.
- **Space Complexity**: $O(N)$. Maximum call stack depth equals the array length $N$.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int lengthOfLIS(std::vector<int>& nums) {
        return helper(nums, 0, -1);
    }

private:
    int helper(const std::vector<int>& nums, int index, int prev_index) {
        if (index == static_cast<int>(nums.size())) {
            return 0;
        }
        int take = 0;
        if (prev_index < 0 || nums[index] > nums[prev_index]) {
            take = 1 + helper(nums, index + 1, index);
        }
        int skip = helper(nums, index + 1, prev_index);
        return std::max(take, skip);
    }
};
```

#### Python 3.12
```python
class Solution:
    def lengthOfLIS(self, nums: list[int]) -> int:
        def helper(index: int, prev_index: int) -> int:
            if index == len(nums):
                return 0
            take = 0
            if prev_index < 0 or nums[index] > nums[prev_index]:
                take = 1 + helper(index + 1, index)
            skip = helper(index + 1, prev_index)
            return max(take, skip)
        return helper(0, -1)
```

#### Java 21
```java
class Solution {
    public int lengthOfLIS(int[] nums) {
        return helper(nums, 0, -1);
    }

    private int helper(int[] nums, int index, int prevIndex) {
        if (index == nums.length) {
            return 0;
        }
        int take = 0;
        if (prevIndex < 0 || nums[index] > nums[prevIndex]) {
            take = 1 + helper(nums, index + 1, index);
        }
        int skip = helper(nums, index + 1, prevIndex);
        return Math.max(take, skip);
    }
}
```

#### TypeScript
```typescript
function lengthOfLIS(nums: number[]): number {
    function helper(index: number, prevIndex: number): number {
        if (index === nums.length) {
            return 0;
        }
        let take = 0;
        if (prevIndex < 0 || nums[index] > nums[prevIndex]) {
            take = 1 + helper(index + 1, index);
        }
        const skip = helper(index + 1, prevIndex);
        return Math.max(take, skip);
    }
    return helper(0, -1);
}
```

#### Go
```go
package main

func lengthOfLIS(nums []int) int {
    var helper func(index int, prevIndex int) int
    helper = func(index int, prevIndex int) int {
        if index == len(nums) {
            return 0
        }
        take := 0
        if prevIndex < 0 || nums[index] > nums[prevIndex] {
            take = 1 + helper(index+1, index)
        }
        skip := helper(index+1, prevIndex)
        if take > skip {
            return take
        }
        return skip
    }
    return helper(0, -1)
}
```

#### Rust
```rust
impl Solution {
    pub fn length_of_lis(nums: Vec<i32>) -> i32 {
        fn helper(nums: &[i32], index: usize, prev_index: Option<usize>) -> i32 {
            if index == nums.len() {
                return 0;
            }
            let mut take = 0;
            if prev_index.map_or(true, |pi| nums[index] > nums[pi]) {
                take = 1 + helper(nums, index + 1, Some(index));
            }
            let skip = helper(nums, index + 1, prev_index);
            take.max(skip)
        }
        helper(&nums, 0, None)
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Does the `tails` array in patience sorting always represent an actual valid increasing subsequence?</summary>
No, the values inside `tails` do not necessarily form a valid subsequence because elements are greedily overwritten.
However, the length of `tails` is strictly guaranteed to equal the length of the longest increasing subsequence.
</details>

<details>
<summary>2. Why does patience sorting use `lower_bound` rather than `upper_bound` for strictly increasing subsequences?</summary>
For strictly increasing subsequences, duplicate numbers cannot extend a sequence.
`lower_bound` finds the first element where `tails[i] >= x`, ensuring that if `x` is already present, it overwrites that identical value rather than extending the array.
</details>

<details>
<summary>3. How would you modify patience sorting to solve the longest non-decreasing subsequence problem?</summary>
To allow duplicate values (non-decreasing subsequences), use `upper_bound` instead of `lower_bound`.
`upper_bound` finds the first element strictly greater than `x`, permitting identical values to extend the tails array.
</details>

<details>
<summary>4. What is the patience sorting card game analogy that mathematically proves the LIS length?</summary>
In the game of Patience, cards are dealt into piles where each card must be placed on the leftmost pile whose top card is greater than or equal to the current card.
If no such pile exists, a new pile is started to the right.
By Dilworth's Theorem, the minimum number of piles needed to cover the sequence equals the length of the longest increasing subsequence.
</details>

<details>
<summary>5. Can we reconstruct the actual sequence elements in $O(N \log N)$ time, or only the length?</summary>
Yes. In addition to updating `tails`, maintain a parent index array `parent` of size $N$ and an index tracker `pos` of size $N$.
Whenever an element at index $i$ is placed at position `idx` in `tails`, record `parent[i] = pos[idx - 1]`.
After processing, trace backward from the last element to reconstruct the sequence.
</details>

<details>
<summary>6. How does the in-place variation achieve $O(1)$ auxiliary space without compromising correctness?</summary>
It reuses the prefix `nums[0 .. len]` to store the smallest tails array.
Because we only read index $i$ after or at the moment we process it, and `len <= i`, the tail buffer never overwrites unprocessed future input elements.
</details>

<details>
<summary>7. Why does the standard $O(N^2)$ dynamic programming approach perform faster on very small inputs ($N < 15$)?</summary>
The $O(N^2)$ DP algorithm uses simple nested sequential memory reads with high CPU branch predictability and cache line prefetching.
Binary search introduces branch mispredictions and random memory jumps that carry higher constant overhead on tiny datasets.
</details>

<details>
<summary>8. How can a Fenwick Tree (Binary Indexed Tree) or Segment Tree solve LIS in $O(N \log N)$ time?</summary>
Coordinate-compress the input values and insert them into a Fenwick Tree where index $v$ stores the maximum LIS length ending with value $v$.
For each number $x$, query the range $[1, x - 1]$ to find the maximum existing LIS length, then update index $x$ with that value plus one.
</details>

<details>
<summary>9. What is the behavior of the optimal algorithm when all elements in the input array are identical?</summary>
`lower_bound` will repeatedly locate index 0 and overwrite `tails[0]` with the same number.
The `tails` array will never expand beyond length 1, correctly returning 1.
</details>

<details>
<summary>10. What is the worst-case behavior of the brute force recursive solution without memoization?</summary>
The call tree branches into two calls for each of the $N$ elements, generating $2^N$ calls.
For $N = 2500$, $2^{2500}$ exceeds the number of atoms in the observable universe and will timeout immediately.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/longest-increasing-subsequence.cpp)
- [Python Implementation](../Python/longest-increasing-subsequence.py)
- [Java Implementation](../Java/longest-increasing-subsequence.java)
- [TypeScript Implementation](../TypeScript/longest-increasing-subsequence.ts)
- [Go Implementation](../Golang/longest-increasing-subsequence.go)
- [Rust Implementation](../Rust/longest-increasing-subsequence.rs)

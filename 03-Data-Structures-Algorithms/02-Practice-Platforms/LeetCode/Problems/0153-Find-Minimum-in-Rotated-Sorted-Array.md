---
id: leetcode-0153-find-minimum-in-rotated-sorted-array
title: "LeetCode 0153: Find Minimum in Rotated Sorted Array"
tags:
  - dsa
  - leetcode
  - array
  - binary-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/find-minimum-in-rotated-sorted-array/"
---

# LeetCode 0153: Find Minimum in Rotated Sorted Array

## 1. Problem Formalization and Constraints

Suppose an array of length $n$ sorted in ascending order is rotated between $1$ and $n$ times.
For example, the array `nums = [0,1,2,4,5,6,7]` might become:
- `[4,5,6,7,0,1,2]` if it was rotated 4 times.
- `[0,1,2,4,5,6,7]` if it was rotated 7 times.
Notice that rotating an array `[a[0], a[1], ..., a[n-1]]` 1 time results in the array `[a[n-1], a[0], a[1], ..., a[n-2]]`.
Given the sorted rotated array `nums` of unique elements, return the minimum element of this array.
You must write an algorithm that runs in $O(\log n)$ time.

### Constraints
- $n == \text{nums.length}$
- $1 \le n \le 5000$
- $-5000 \le \text{nums}[i] \le 5000$
- All the integers of `nums` are unique.
- `nums` is sorted and rotated between $1$ and $n$ times.

### Examples
- **Example 1**:
  - Input: `nums = [3,4,5,1,2]`
  - Output: `1`
  - Explanation: The original array was `[1,2,3,4,5]` rotated 3 times.
- **Example 2**:
  - Input: `nums = [4,5,6,7,0,1,2]`
  - Output: `0`
- **Example 3**:
  - Input: `nums = [11,13,15,17]`
  - Output: `11`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Asymmetric Binary Search (Right Boundary Pivot) | $O(\log N)$ | $O(1)$ | Compares midpoint against right boundary to determine inflection side. |
| **Tier 2 (Space-Optimized Alternative)** | Inflection Boundary Verification | $O(\log N)$ | $O(1)$ | Explicitly checks adjacent inflection edges $(\text{nums}[m] > \text{nums}[m+1])$ for early exit. |
| **Tier 3 (Time-Optimized Alternative)** | Recursive Divide and Conquer | $O(\log N)$ | $O(\log N)$ | Bisects intervals recursively, pruning sorted halves in $O(1)$ comparisons. |
| **Tier 4 (Brute Force)** | Linear Search Scan | $O(N)$ | $O(1)$ | Scans all elements sequentially; fails to leverage logarithmic sorted invariants. |

---

## 3. Tier 1: Most Optimal Solution (Asymmetric Binary Search)

### 3.1 Algorithmic Mechanics and Invariant Proof

Let `nums` be a rotated sorted array of distinct elements with search window $[L, R]$.
Compare the midpoint $M = \lfloor(L + R)/2\rfloor$ with the right boundary $R$:
1. If $\text{nums}[M] > \text{nums}[R]$:
   The sequence from $M$ to $R$ is disrupted by an inflection drop.
   The minimum element must lie strictly to the right of $M$.
   We advance $L = M + 1$.
2. If $\text{nums}[M] \le \text{nums}[R]$:
   The subarray from $M$ to $R$ is monotonically increasing.
   The minimum element could be $\text{nums}[M]$ itself, or lie to the left of $M$.
   We contract $R = M$.

The loop terminates when $L == R$.

**Loop Invariant**:
Throughout iteration, the global minimum element $x^* = \min(\text{nums})$ always resides within the closed interval $[L, R]$.
Because the window strictly shrinks by at least one element on each step ($\lfloor(L+R)/2\rfloor < R$ whenever $L < R$), convergence is guaranteed in at most $\lceil\log_2 N\rceil$ steps.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$. The search space is halved at every iteration.
- **Space Complexity**: $O(1)$. Auxiliary space is strictly bounded to three scalar index pointers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    int findMin(const std::vector<int>& nums) {
        int left = 0;
        int right = static_cast<int>(nums.size()) - 1;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] > nums[right]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return nums[left];
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def findMin(self, nums: List[int]) -> int:
        left, right = 0, len(nums) - 1
        while left < right:
            mid = (left + right) // 2
            if nums[mid] > nums[right]:
                left = mid + 1
            else:
                right = mid
        return nums[left]
```

#### Java 21
```java
class Solution {
    public int findMin(int[] nums) {
        int left = 0;
        int right = nums.length - 1;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] > nums[right]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return nums[left];
    }
}
```

#### TypeScript
```typescript
function findMin(nums: number[]): number {
    let left = 0;
    let right = nums.length - 1;
    while (left < right) {
        const mid = (left + right) >> 1;
        if (nums[mid] > nums[right]) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return nums[left];
}
```

#### Go
```go
package main

func findMin(nums []int) int {
    left := 0
    right := len(nums) - 1
    for left < right {
        mid := left + (right-left)/2
        if nums[mid] > nums[right] {
            left = mid + 1
        } else {
            right = mid
        }
    }
    return nums[left]
}
```

#### Rust
```rust
impl Solution {
    pub fn find_min(nums: Vec<i32>) -> i32 {
        let mut left = 0;
        let mut right = nums.len() - 1;
        while left < right {
            let mid = left + (right - left) / 2;
            if nums[mid] > nums[right] {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        nums[left]
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Inflection Point Detection)

### 4.1 Algorithmic Mechanics

We can detect the inflection boundary early by checking if the midpoint itself neighbors the pivot:
1. If $\text{nums}[mid] > \text{nums}[mid + 1]$, then $\text{nums}[mid + 1]$ is the minimum element.
2. If $\text{nums}[mid - 1] > \text{nums}[mid]$, then $\text{nums}[mid]$ is the minimum element.
3. If the entire segment is already sorted ($\text{nums}[left] \le \text{nums}[right]$), the minimum is $\text{nums}[left]$.

Otherwise, we branch left or right based on comparison with $\text{nums}[0]$.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$ average with potential early exit in $O(1)$ steps.
- **Space Complexity**: $O(1)$ auxiliary storage.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    int findMin(const std::vector<int>& nums) {
        int n = nums.size();
        if (n == 1 || nums[0] < nums[n - 1]) return nums[0];
        int left = 0, right = n - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (mid + 1 < n && nums[mid] > nums[mid + 1]) {
                return nums[mid + 1];
            }
            if (mid - 1 >= 0 && nums[mid - 1] > nums[mid]) {
                return nums[mid];
            }
            if (nums[mid] >= nums[0]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return nums[0];
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def findMin(self, nums: List[int]) -> int:
        n = len(nums)
        if n == 1 or nums[0] < nums[-1]:
            return nums[0]
        left, right = 0, n - 1
        while left <= right:
            mid = (left + right) // 2
            if mid + 1 < n and nums[mid] > nums[mid + 1]:
                return nums[mid + 1]
            if mid - 1 >= 0 and nums[mid - 1] > nums[mid]:
                return nums[mid]
            if nums[mid] >= nums[0]:
                left = mid + 1
            else:
                right = mid - 1
        return nums[0]
```

#### Java 21
```java
class Solution {
    public int findMin(int[] nums) {
        int n = nums.length;
        if (n == 1 || nums[0] < nums[n - 1]) return nums[0];
        int left = 0, right = n - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (mid + 1 < n && nums[mid] > nums[mid + 1]) {
                return nums[mid + 1];
            }
            if (mid - 1 >= 0 && nums[mid - 1] > nums[mid]) {
                return nums[mid];
            }
            if (nums[mid] >= nums[0]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return nums[0];
    }
}
```

#### TypeScript
```typescript
function findMin(nums: number[]): number {
    const n = nums.length;
    if (n === 1 || nums[0] < nums[n - 1]) return nums[0];
    let left = 0;
    let right = n - 1;
    while (left <= right) {
        const mid = (left + right) >> 1;
        if (mid + 1 < n && nums[mid] > nums[mid + 1]) {
            return nums[mid + 1];
        }
        if (mid - 1 >= 0 && nums[mid - 1] > nums[mid]) {
            return nums[mid];
        }
        if (nums[mid] >= nums[0]) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return nums[0];
}
```

#### Go
```go
package main

func findMin(nums []int) int {
    n := len(nums)
    if n == 1 || nums[0] < nums[n-1] {
        return nums[0]
    }
    left, right := 0, n-1
    for left <= right {
        mid := left + (right-left)/2
        if mid+1 < n && nums[mid] > nums[mid+1] {
            return nums[mid+1]
        }
        if mid-1 >= 0 && nums[mid-1] > nums[mid] {
            return nums[mid]
        }
        if nums[mid] >= nums[0] {
            left = mid + 1
        } else {
            right = mid - 1
        }
    }
    return nums[0]
}
```

#### Rust
```rust
impl Solution {
    pub fn find_min(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        if n == 1 || nums[0] < nums[n - 1] {
            return nums[0];
        }
        let mut left = 0;
        let mut right = n - 1;
        while left <= right {
            let mid = left + (right - left) / 2;
            if mid + 1 < n && nums[mid] > nums[mid + 1] {
                return nums[mid + 1];
            }
            if mid > 0 && nums[mid - 1] > nums[mid] {
                return nums[mid];
            }
            if nums[mid] >= nums[0] {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        nums[0]
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Recursive Divide and Conquer)

### 5.1 Algorithmic Mechanics

We divide the range $[L, R]$ at mid.
If $\text{nums}[L] \le \text{nums}[R]$, the range is sorted and its minimum is $\text{nums}[L]$.
Otherwise, we recurse on the left half $[L, mid]$ and right half $[mid+1, R]$ and take the minimum of both.
Because one half is always sorted, its minimum is obtained in $O(1)$, keeping overall runtime $O(\log N)$.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$. Exactly one branch continues recursive decomposition.
- **Space Complexity**: $O(\log N)$ call stack frames.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
    int search(const std::vector<int>& nums, int l, int r) {
        if (nums[l] <= nums[r]) {
            return nums[l];
        }
        int mid = l + (r - l) / 2;
        return std::min(search(nums, l, mid), search(nums, mid + 1, r));
    }
public:
    int findMin(const std::vector<int>& nums) {
        return search(nums, 0, nums.size() - 1);
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def findMin(self, nums: List[int]) -> int:
        def search(l: int, r: int) -> int:
            if nums[l] <= nums[r]:
                return nums[l]
            mid = (l + r) // 2
            return min(search(l, mid), search(mid + 1, r))

        return search(0, len(nums) - 1)
```

#### Java 21
```java
class Solution {
    private int search(int[] nums, int l, int r) {
        if (nums[l] <= nums[r]) {
            return nums[l];
        }
        int mid = l + (r - l) / 2;
        return Math.min(search(nums, l, mid), search(nums, mid + 1, r));
    }

    public int findMin(int[] nums) {
        return search(nums, 0, nums.length - 1);
    }
}
```

#### TypeScript
```typescript
function findMin(nums: number[]): number {
    function search(l: number, r: number): number {
        if (nums[l] <= nums[r]) {
            return nums[l];
        }
        const mid = (l + r) >> 1;
        return Math.min(search(l, mid), search(mid + 1, r));
    }
    return search(0, nums.length - 1);
}
```

#### Go
```go
package main

func searchHelper(nums []int, l, r int) int {
    if nums[l] <= nums[r] {
        return nums[l]
    }
    mid := l + (r-l)/2
    leftMin := searchHelper(nums, l, mid)
    rightMin := searchHelper(nums, mid+1, r)
    if leftMin < rightMin {
        return leftMin
    }
    return rightMin
}

func findMin(nums []int) int {
    return searchHelper(nums, 0, len(nums)-1)
}
```

#### Rust
```rust
impl Solution {
    fn search(nums: &[i32], l: usize, r: usize) -> i32 {
        if nums[l] <= nums[r] {
            return nums[l];
        }
        let mid = l + (r - l) / 2;
        Self::search(nums, l, mid).min(Self::search(nums, mid + 1, r))
    }

    pub fn find_min(nums: Vec<i32>) -> i32 {
        Self::search(&nums, 0, nums.len() - 1)
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Linear Search)

### 6.1 Algorithmic Mechanics

We perform a sequential scan through all elements of `nums`, tracking the smallest element encountered.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear scan.
- **Space Complexity**: $O(1)$ auxiliary storage.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    int findMin(const std::vector<int>& nums) {
        int minVal = nums[0];
        for (int x : nums) {
            minVal = std::min(minVal, x);
        }
        return minVal;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def findMin(self, nums: List[int]) -> int:
        return min(nums)
```

#### Java 21
```java
class Solution {
    public int findMin(int[] nums) {
        int minVal = nums[0];
        for (int x : nums) {
            if (x < minVal) {
                minVal = x;
            }
        }
        return minVal;
    }
}
```

#### TypeScript
```typescript
function findMin(nums: number[]): number {
    let minVal = nums[0];
    for (const x of nums) {
        if (x < minVal) {
            minVal = x;
        }
    }
    return minVal;
}
```

#### Go
```go
package main

func findMin(nums []int) int {
    minVal := nums[0]
    for _, x := range nums {
        if x < minVal {
            minVal = x
        }
    }
    return minVal
}
```

#### Rust
```rust
impl Solution {
    pub fn find_min(nums: Vec<i32>) -> i32 {
        *nums.iter().min().unwrap()
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why do we compare `nums[mid]` against `nums[right]` rather than `nums[left]` in Tier 1?</summary>
Comparing with `nums[left]` is ambiguous when the array is unrotated (e.g., `[1, 2, 3]`).
In that case, `nums[mid] > nums[left]` holds both when the minimum is at index $0$ and when the rotation pivot is to the right.
Comparing against `nums[right]` creates a strictly unambiguous partition: if `nums[mid] > nums[right]`, the pivot drop is guaranteed to lie to the right of `mid`.
</details>

<details>
<summary>2. Why does Tier 1 use `right = mid` instead of `right = mid - 1`?</summary>
When `nums[mid] <= nums[right]`, `nums[mid]` could itself be the minimum element.
Discarding `mid` by setting `right = mid - 1` would eliminate the correct answer from the search window.
Conversely, when `nums[mid] > nums[right]`, `nums[mid]` cannot be the minimum because `nums[right]` is already strictly smaller, so setting `left = mid + 1` is safe.
</details>

<details>
<summary>3. What happens if the array contains duplicate elements (LeetCode 154)?</summary>
If `nums[mid] == nums[right]`, we cannot determine which half contains the pivot.
We must decrement `right--` linearly.
This degrades the worst-case time complexity to $O(N)$ when all elements are identical.
</details>

<details>
<summary>4. What prevents `mid = left + (right - left) / 2` from infinite looping when `left + 1 == right`?</summary>
When $right = left + 1$, integer division yields $mid = left$.
If `nums[mid] > nums[right]`, $left = mid + 1 = right$, terminating the loop.
If `nums[mid] <= nums[right]`, $right = mid = left$, terminating the loop.
In both branches, $right - left$ decreases by at least 1, guaranteeing termination.
</details>

<details>
<summary>5. How does this algorithm identify the number of rotations the original array underwent?</summary>
The index of the minimum element corresponds directly to the rotation count $k$.
If the minimum element is at index $k$, the array was rotated $k$ times to the right.
</details>

<details>
<summary>6. How does branch prediction perform during logarithmic binary search?</summary>
Because the inflection side changes dynamically, branch predictors have lower predictability than linear loops.
However, since the loop executes at most $\approx 13$ iterations for $N = 5000$, total misprediction penalty is negligible compared to the $O(N)$ cost of a linear scan.
</details>

<details>
<summary>7. In C++ and Go, why is `left + (right - left) / 2` preferred over `(left + right) / 2`?</summary>
When `left` and `right` are large positive integers near $2^{31} - 1$, their sum `left + right` overflows into a negative value, triggering undefined behavior or an out-of-bounds index.
The subtraction formulation avoids integer overflow.
</details>

<details>
<summary>8. How can we formulate this using standard library binary search functions (`std::partition_point`)?</summary>
In C++20, we can express the search range with a lambda:
`std::partition_point(nums.begin(), nums.end(), [&](int x) { return x > nums.back(); })`.
This directly returns an iterator to the minimum element.
</details>

<details>
<summary>9. What is the CPU cache footprint of logarithmic binary search?</summary>
Each jump cuts the array span in half, meaning early iterations touch memory locations spaced by $N/2, N/4, N/8$ elements, resulting in cold L1 cache misses.
Once the active search window fits inside a single 64-byte cache line (16 integers), subsequent iterations hit L1 cache with zero memory latency.
</details>

<details>
<summary>10. How does this foundation enable solving Search in Rotated Sorted Array (LeetCode 33)?</summary>
Once the pivot index of the minimum element is located in $O(\log N)$ time, the array is partitioned into two independently sorted subarrays $[0, k-1]$ and $[k, N-1]$.
A standard binary search on the appropriate half locates any target element in an additional $O(\log N)$ time.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/find-minimum-in-rotated-sorted-array.cpp)
- [Python Implementation](../Python/find-minimum-in-rotated-sorted-array.py)
- [Java Implementation](../Java/find-minimum-in-rotated-sorted-array.java)
- [TypeScript Implementation](../TypeScript/find-minimum-in-rotated-sorted-array.ts)
- [Go Implementation](../Golang/find-minimum-in-rotated-sorted-array.go)
- [Rust Implementation](../Rust/find-minimum-in-rotated-sorted-array.rs)

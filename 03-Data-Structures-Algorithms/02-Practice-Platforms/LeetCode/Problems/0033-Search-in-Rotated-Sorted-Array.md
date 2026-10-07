---
id: leetcode-0033-search-in-rotated-sorted-array
title: "LeetCode 0033: Search in Rotated Sorted Array"
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
  - "https://leetcode.com/problems/search-in-rotated-sorted-array/"
---

# LeetCode 0033: Search in Rotated Sorted Array

## 1. Problem Formalization and Constraints

There is an integer array `nums` sorted in ascending order (with distinct values).
Prior to being passed to your function, `nums` is possibly rotated at an unknown pivot index $k$ ($1 \le k < \text{nums.length}$) such that the resulting array is `[nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ..., nums[k-1]]` (0-indexed).
For example, `[0,1,2,4,5,6,7]` might be rotated at pivot index 3 and become `[4,5,6,7,0,1,2]`.
Given the array `nums` after the possible rotation and an integer `target`, return the index of `target` if it is in `nums`, or `-1` if it is not in `nums`.
You must write an algorithm with $O(\log n)$ runtime complexity.

### Constraints
- $1 \le \text{nums.length} \le 5000$
- $-10^4 \le \text{nums}[i] \le 10^4$
- All values of `nums` are unique.
- `nums` is an ascending array that is possibly rotated.
- $-10^4 \le \text{target} \le 10^4$

### Examples
- **Example 1**:
  - Input: `nums = [4,5,6,7,0,1,2], target = 0`
  - Output: `4`
- **Example 2**:
  - Input: `nums = [4,5,6,7,0,1,2], target = 3`
  - Output: `-1`
- **Example 3**:
  - Input: `nums = [1], target = 0`
  - Output: `-1`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Single-Pass Invariant Partitioning Binary Search | $O(\log N)$ | $O(1)$ | Identifies whether the left or right subarray is sorted, then checks containment. |
| **Tier 2 (Space-Optimized Alternative)** | Two-Pass Pivot Discovery + Subarray Binary Search | $O(\log N)$ | $O(1)$ | First locates the inflection pivot index, then runs standard binary search. |
| **Tier 3 (Time-Optimized Alternative)** | Recursive Interval Bisection | $O(\log N)$ | $O(\log N)$ | Bisects interval recursively, terminating pruned subtrees immediately. |
| **Tier 4 (Brute Force)** | Linear Search Scan | $O(N)$ | $O(1)$ | Scans every element sequentially; ignores sorted structure. |

---

## 3. Tier 1: Most Optimal Solution (Single-Pass Partitioning Binary Search)

### 3.1 Algorithmic Mechanics and Invariant Proof

For any range $[L, R]$ with midpoint $M = \lfloor(L + R)/2\rfloor$, at least one half ($[L, M]$ or $[M, R]$) is guaranteed to be strictly sorted.

1. **Check Midpoint**:
   If $\text{nums}[M] == \text{target}$, return $M$.
2. **Left Half is Sorted** ($\text{nums}[L] \le \text{nums}[M]$):
   - The elements in $[L, M]$ are strictly non-decreasing.
   - If $\text{nums}[L] \le \text{target} < \text{nums}[M]$, the target must lie in the left half: set $R = M - 1$.
   - Otherwise, the target must lie in the right half: set $L = M + 1$.
3. **Right Half is Sorted** ($\text{nums}[L] > \text{nums}[M]$):
   - The elements in $[M, R]$ are strictly non-decreasing.
   - If $\text{nums}[M] < \text{target} \le \text{nums}[R]$, the target must lie in the right half: set $L = M + 1$.
   - Otherwise, the target must lie in the left half: set $R = M - 1$.

**Inductive Invariant**:
If `target` exists in `nums`, it is strictly bounded within $[L, R]$ across all iterations.
Because each iteration shrinks the search window by at least half, the loop terminates in $\lceil\log_2 N\rceil$ steps.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$. Halves the search space at each iteration.
- **Space Complexity**: $O(1)$. Auxiliary space is strictly bounded to three scalar index pointers.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    int search(const std::vector<int>& nums, int target) {
        int left = 0;
        int right = static_cast<int>(nums.size()) - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] == target) {
                return mid;
            }
            if (nums[left] <= nums[mid]) {
                if (nums[left] <= target && target < nums[mid]) {
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            } else {
                if (nums[mid] < target && target <= nums[right]) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        }
        return -1;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def search(self, nums: List[int], target: int) -> int:
        left, right = 0, len(nums) - 1
        while left <= right:
            mid = (left + right) // 2
            if nums[mid] == target:
                return mid
            if nums[left] <= nums[mid]:
                if nums[left] <= target < nums[mid]:
                    right = mid - 1
                else:
                    left = mid + 1
            else:
                if nums[mid] < target <= nums[right]:
                    left = mid + 1
                else:
                    right = mid - 1
        return -1
```

#### Java 21
```java
class Solution {
    public int search(int[] nums, int target) {
        int left = 0;
        int right = nums.length - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] == target) {
                return mid;
            }
            if (nums[left] <= nums[mid]) {
                if (nums[left] <= target && target < nums[mid]) {
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            } else {
                if (nums[mid] < target && target <= nums[right]) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        }
        return -1;
    }
}
```

#### TypeScript
```typescript
function search(nums: number[], target: number): number {
    let left = 0;
    let right = nums.length - 1;
    while (left <= right) {
        const mid = (left + right) >> 1;
        if (nums[mid] === target) {
            return mid;
        }
        if (nums[left] <= nums[mid]) {
            if (nums[left] <= target && target < nums[mid]) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            if (nums[mid] < target && target <= nums[right]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }
    return -1;
}
```

#### Go
```go
package main

func search(nums []int, target int) int {
    left := 0
    right := len(nums) - 1
    for left <= right {
        mid := left + (right-left)/2
        if nums[mid] == target {
            return mid
        }
        if nums[left] <= nums[mid] {
            if nums[left] <= target && target < nums[mid] {
                right = mid - 1
            } else {
                left = mid + 1
            }
        } else {
            if nums[mid] < target && target <= nums[right] {
                left = mid + 1
            } else {
                right = mid - 1
            }
        }
    }
    return -1
}
```

#### Rust
```rust
impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let mut left: i32 = 0;
        let mut right: i32 = nums.len() as i32 - 1;
        while left <= right {
            let mid = left + (right - left) / 2;
            let m_val = nums[mid as usize];
            if m_val == target {
                return mid;
            }
            let l_val = nums[left as usize];
            let r_val = nums[right as usize];
            if l_val <= m_val {
                if l_val <= target && target < m_val {
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            } else {
                if m_val < target && target <= r_val {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        }
        -1
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Two-Pass Pivot + Subarray Search)

### 4.1 Algorithmic Mechanics

We decompose the problem into two sequential steps:
1. Pass 1: Find the index of the minimum element (the rotation pivot $k$) in $O(\log N)$ using LeetCode 153 logic.
2. Pass 2: The pivot divides the array into two sorted sequences: $\text{nums}[0 \dots k-1]$ and $\text{nums}[k \dots N-1]$.
   Determine which half contains `target` by comparing with $\text{nums}[N-1]$ or $\text{nums}[0]$, and execute a standard binary search on that half.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$ ($2 \log N$ operations).
- **Space Complexity**: $O(1)$ auxiliary scalar memory.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
    int findPivot(const std::vector<int>& nums) {
        int l = 0, r = nums.size() - 1;
        while (l < r) {
            int mid = l + (r - l) / 2;
            if (nums[mid] > nums[r]) l = mid + 1;
            else r = mid;
        }
        return l;
    }

    int binarySearch(const std::vector<int>& nums, int l, int r, int target) {
        while (l <= r) {
            int mid = l + (r - l) / 2;
            if (nums[mid] == target) return mid;
            if (nums[mid] < target) l = mid + 1;
            else r = mid - 1;
        }
        return -1;
    }

public:
    int search(const std::vector<int>& nums, int target) {
        int n = nums.size();
        int pivot = findPivot(nums);
        if (target >= nums[pivot] && target <= nums[n - 1]) {
            return binarySearch(nums, pivot, n - 1, target);
        }
        return binarySearch(nums, 0, pivot - 1, target);
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def search(self, nums: List[int], target: int) -> int:
        n = len(nums)
        l, r = 0, n - 1
        while l < r:
            mid = (l + r) // 2
            if nums[mid] > nums[r]:
                l = mid + 1
            else:
                r = mid
        pivot = l

        if target >= nums[pivot] and target <= nums[-1]:
            l, r = pivot, n - 1
        else:
            l, r = 0, pivot - 1

        while l <= r:
            mid = (l + r) // 2
            if nums[mid] == target:
                return mid
            if nums[mid] < target:
                l = mid + 1
            else:
                r = mid - 1
        return -1
```

#### Java 21
```java
class Solution {
    private int findPivot(int[] nums) {
        int l = 0, r = nums.length - 1;
        while (l < r) {
            int mid = l + (r - l) / 2;
            if (nums[mid] > nums[r]) l = mid + 1;
            else r = mid;
        }
        return l;
    }

    private int binarySearch(int[] nums, int l, int r, int target) {
        while (l <= r) {
            int mid = l + (r - l) / 2;
            if (nums[mid] == target) return mid;
            if (nums[mid] < target) l = mid + 1;
            else r = mid - 1;
        }
        return -1;
    }

    public int search(int[] nums, int target) {
        int n = nums.length;
        int pivot = findPivot(nums);
        if (target >= nums[pivot] && target <= nums[n - 1]) {
            return binarySearch(nums, pivot, n - 1, target);
        }
        return binarySearch(nums, 0, pivot - 1, target);
    }
}
```

#### TypeScript
```typescript
function search(nums: number[], target: number): number {
    const n = nums.length;
    let l = 0, r = n - 1;
    while (l < r) {
        const mid = (l + r) >> 1;
        if (nums[mid] > nums[r]) l = mid + 1;
        else r = mid;
    }
    const pivot = l;

    if (target >= nums[pivot] && target <= nums[n - 1]) {
        l = pivot;
        r = n - 1;
    } else {
        l = 0;
        r = pivot - 1;
    }

    while (l <= r) {
        const mid = (l + r) >> 1;
        if (nums[mid] === target) return mid;
        if (nums[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}
```

#### Go
```go
package main

func search(nums []int, target int) int {
    n := len(nums)
    l, r := 0, n-1
    for l < r {
        mid := l + (r-l)/2
        if nums[mid] > nums[r] {
            l = mid + 1
        } else {
            r = mid
        }
    }
    pivot := l

    if target >= nums[pivot] && target <= nums[n-1] {
        l, r = pivot, n-1
    } else {
        l, r = 0, pivot-1
    }

    for l <= r {
        mid := l + (r-l)/2
        if nums[mid] == target {
            return mid
        }
        if nums[mid] < target {
            l = mid + 1
        } else {
            r = mid - 1
        }
    }
    return -1
}
```

#### Rust
```rust
impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let n = nums.len();
        let mut l = 0;
        let mut r = n - 1;
        while l < r {
            let mid = l + (r - l) / 2;
            if nums[mid] > nums[r] {
                l = mid + 1;
            } else {
                r = mid;
            }
        }
        let pivot = l;

        let mut low: i32;
        let mut high: i32;
        if target >= nums[pivot] && target <= nums[n - 1] {
            low = pivot as i32;
            high = n as i32 - 1;
        } else {
            low = 0;
            high = pivot as i32 - 1;
        }

        while low <= high {
            let mid = low + (high - low) / 2;
            let val = nums[mid as usize];
            if val == target {
                return mid;
            }
            if val < target {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        -1
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Recursive Bisection Search)

### 5.1 Algorithmic Mechanics

We formulate the search recursively.
At each call on $[L, R]$, test if $L > R$.
If $\text{nums}[mid] == \text{target}$, return $mid$.
Otherwise, check whether left or right is sorted and recurse only into the subsegment containing the target.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(\log N)$. Tail recursion ensures single path descent.
- **Space Complexity**: $O(\log N)$ stack frames.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
    int searchHelper(const std::vector<int>& nums, int l, int r, int target) {
        if (l > r) return -1;
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        if (nums[l] <= nums[mid]) {
            if (nums[l] <= target && target < nums[mid]) {
                return searchHelper(nums, l, mid - 1, target);
            }
            return searchHelper(nums, mid + 1, r, target);
        } else {
            if (nums[mid] < target && target <= nums[r]) {
                return searchHelper(nums, mid + 1, r, target);
            }
            return searchHelper(nums, l, mid - 1, target);
        }
    }
public:
    int search(const std::vector<int>& nums, int target) {
        return searchHelper(nums, 0, nums.size() - 1, target);
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def search(self, nums: List[int], target: int) -> int:
        def search_helper(l: int, r: int) -> int:
            if l > r:
                return -1
            mid = (l + r) // 2
            if nums[mid] == target:
                return mid
            if nums[l] <= nums[mid]:
                if nums[l] <= target < nums[mid]:
                    return search_helper(l, mid - 1)
                return search_helper(mid + 1, r)
            else:
                if nums[mid] < target <= nums[r]:
                    return search_helper(mid + 1, r)
                return search_helper(l, mid - 1)

        return search_helper(0, len(nums) - 1)
```

#### Java 21
```java
class Solution {
    private int searchHelper(int[] nums, int l, int r, int target) {
        if (l > r) return -1;
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        if (nums[l] <= nums[mid]) {
            if (nums[l] <= target && target < nums[mid]) {
                return searchHelper(nums, l, mid - 1, target);
            }
            return searchHelper(nums, mid + 1, r, target);
        } else {
            if (nums[mid] < target && target <= nums[r]) {
                return searchHelper(nums, mid + 1, r, target);
            }
            return searchHelper(nums, l, mid - 1, target);
        }
    }

    public int search(int[] nums, int target) {
        return searchHelper(nums, 0, nums.length - 1, target);
    }
}
```

#### TypeScript
```typescript
function search(nums: number[], target: number): number {
    function searchHelper(l: number, r: number): number {
        if (l > r) return -1;
        const mid = (l + r) >> 1;
        if (nums[mid] === target) return mid;
        if (nums[l] <= nums[mid]) {
            if (nums[l] <= target && target < nums[mid]) {
                return searchHelper(l, mid - 1);
            }
            return searchHelper(mid + 1, r);
        } else {
            if (nums[mid] < target && target <= nums[r]) {
                return searchHelper(mid + 1, r);
            }
            return searchHelper(l, mid - 1);
        }
    }
    return searchHelper(0, nums.length - 1);
}
```

#### Go
```go
package main

func searchRec(nums []int, l, r, target int) int {
    if l > r {
        return -1
    }
    mid := l + (r-l)/2
    if nums[mid] == target {
        return mid
    }
    if nums[l] <= nums[mid] {
        if nums[l] <= target && target < nums[mid] {
            return searchRec(nums, l, mid-1, target)
        }
        return searchRec(nums, mid+1, r, target)
    } else {
        if nums[mid] < target && target <= nums[r] {
            return searchRec(nums, mid+1, r, target)
        }
        return searchRec(nums, l, mid-1, target)
    }
}

func search(nums []int, target int) int {
    return searchRec(nums, 0, len(nums)-1, target)
}
```

#### Rust
```rust
impl Solution {
    fn search_rec(nums: &[i32], l: i32, r: i32, target: i32) -> i32 {
        if l > r {
            return -1;
        }
        let mid = l + (r - l) / 2;
        let m_val = nums[mid as usize];
        if m_val == target {
            return mid;
        }
        let l_val = nums[l as usize];
        let r_val = nums[r as usize];
        if l_val <= m_val {
            if l_val <= target && target < m_val {
                Self::search_rec(nums, l, mid - 1, target)
            } else {
                Self::search_rec(nums, mid + 1, r, target)
            }
        } else {
            if m_val < target && target <= r_val {
                Self::search_rec(nums, mid + 1, r, target)
            } else {
                Self::search_rec(nums, l, mid - 1, target)
            }
        }
    }

    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        Self::search_rec(&nums, 0, nums.len() as i32 - 1, target)
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Linear Search)

### 6.1 Algorithmic Mechanics

We iterate linearly through the array from index $0$ to $N-1$, returning the index upon matching `target`.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear scans.
- **Space Complexity**: $O(1)$ auxiliary memory.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    int search(const std::vector<int>& nums, int target) {
        for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
            if (nums[i] == target) {
                return i;
            }
        }
        return -1;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def search(self, nums: List[int], target: int) -> int:
        for i, x in enumerate(nums):
            if x == target:
                return i
        return -1
```

#### Java 21
```java
class Solution {
    public int search(int[] nums, int target) {
        for (int i = 0; i < nums.length; i++) {
            if (nums[i] == target) {
                return i;
            }
        }
        return -1;
    }
}
```

#### TypeScript
```typescript
function search(nums: number[], target: number): number {
    for (let i = 0; i < nums.length; i++) {
        if (nums[i] === target) {
            return i;
        }
    }
    return -1;
}
```

#### Go
```go
package main

func search(nums []int, target int) int {
    for i, x := range nums {
        if x == target {
            return i
        }
    }
    return -1
}
```

#### Rust
```rust
impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        for (i, &x) in nums.iter().enumerate() {
            if x == target {
                return i as i32;
            }
        }
        -1
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is `nums[left] <= nums[mid]` using `<=` rather than `<`?</summary>
When $left == mid$ (which happens whenever the search interval narrows to two elements), `nums[left] == nums[mid]`.
Using strictly `<` would treat a single-element segment as an un-sorted right half, failing to check containment in the left element.
The `<=` operator ensures correct handling of single-element boundaries.
</details>

<details>
<summary>2. How does this solution handle duplicates (LeetCode 81)?</summary>
When elements can be duplicated, `nums[left] == nums[mid] == nums[right]` can occur (e.g., `[1, 0, 1, 1, 1]`).
In that scenario, it is impossible to deduce which half is sorted.
We must increment `left++` and decrement `right--`, degrading the worst-case runtime from $O(\log N)$ to $O(N)$.
</details>

<details>
<summary>3. Why is the single-pass approach in Tier 1 preferred over the two-pass approach in Tier 2?</summary>
Tier 1 executes at most $\approx \log_2 N$ comparisons in a single pass.
Tier 2 requires first finding the pivot ($\approx \log_2 N$ steps) and then performing a second binary search ($\approx \log_2 N$ steps).
Tier 1 saves approximately $50\%$ of the memory fetches and instruction branches.
</details>

<details>
<summary>4. What happens when the input array is not rotated at all?</summary>
If the array is already sorted, `nums[left] <= nums[mid]` holds for every iteration.
The algorithm behaves exactly like standard classical binary search with zero overhead.
</details>

<details>
<summary>5. Can this search be mapped onto virtual coordinates using modular arithmetic?</summary>
Yes. If the pivot offset $k$ is known, an index $i \in [0, N-1]$ maps to real index $(i + k) \bmod N$.
Standard binary search over $[0, N-1]$ with virtual element retrieval `nums[(mid + k) % N]` eliminates all branching inside the loop.
</details>

<details>
<summary>6. How does branch misprediction affect the performance of Tier 1?</summary>
The condition `nums[left] <= nums[mid]` and the sub-condition `nums[left] <= target && target < nums[mid]` introduce nested branches.
However, because the search interval converges in at most 13 steps for $N = 5000$, branch penalties remain negligible compared to cold cache line fetches.
</details>

<details>
<summary>7. Why does Rust require casting indices to `i32` when handling negative indices?</summary>
When $mid = 0$, $mid - 1$ underflows if represented as `usize`, producing a panic in debug mode or $2^{64}-1$ in release mode.
Using signed `i32` for binary search pointers prevents underflow when boundary checks like `right = mid - 1` evaluate to $-1$.
</details>

<details>
<summary>8. How does compiler loop unrolling impact short logarithmic searches?</summary>
Since $N \le 5000$, $\lceil\log_2 5000\rceil = 13$.
Compilers can partially unroll the while loop or inline helper predicates, eliminating branch overhead at the end of the iteration sequence.
</details>

<details>
<summary>9. What is the difference between this problem and searching in a bitonic array?</summary>
A rotated sorted array consists of two monotonically increasing sequences separated by a drop.
A bitonic array increases to a peak and then decreases monotonically.
Both can be partitioned in $O(\log N)$ time, but bitonic arrays require searching the decreasing half in reverse order.
</details>

<details>
<summary>10. What invariants must be checked when writing unit tests for rotated sorted array algorithms?</summary>
Key test cases include:
1. Array rotated by 0 positions (standard sorted).
2. Array rotated by 1 position (pivot at $N-1$).
3. Array rotated by $N-1$ positions (pivot at 1).
4. Single-element arrays (`nums = [1]`).
5. Two-element arrays (`[1, 3]` and `[3, 1]`).
6. Target present at boundaries: `nums[0]`, `nums[mid]`, `nums[N-1]`.
7. Target absent (smaller than min, larger than max, or in between gaps).
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/search-in-rotated-sorted-array.cpp)
- [Python Implementation](../Python/search-in-rotated-sorted-array.py)
- [Java Implementation](../Java/search-in-rotated-sorted-array.java)
- [TypeScript Implementation](../TypeScript/search-in-rotated-sorted-array.ts)
- [Go Implementation](../Golang/search-in-rotated-sorted-array.go)
- [Rust Implementation](../Rust/search-in-rotated-sorted-array.rs)

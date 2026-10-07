---
id: leetcode-0001-two-sum
title: "LeetCode 0001: Two Sum"
tags:
  - dsa
  - leetcode
  - array
  - hash-table
  - two-pointers
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/two-sum/"
---

# LeetCode 0001: Two Sum

## 1. Problem Formalization and Constraints

Given an array of integers `nums` and an integer `target`, return indices of the two numbers such that they add up to `target`.
You may assume that each input would have exactly one solution, and you may not use the same element twice.
You can return the answer in any order.

### Constraints
- $2 \le \text{nums.length} \le 10^4$
- $-10^9 \le \text{nums}[i] \le 10^9$
- $-10^9 \le \text{target} \le 10^9$
- Only one valid answer exists.

### Examples
- **Example 1**:
  - Input: `nums = [2,7,11,15], target = 9`
  - Output: `[0,1]` (Because `nums[0] + nums[1] == 9`)
- **Example 2**:
  - Input: `nums = [3,2,4], target = 6`
  - Output: `[1,2]`
- **Example 3**:
  - Input: `nums = [3,3], target = 6`
  - Output: `[0,1]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | One-Pass Hash Map | $O(N)$ | $O(N)$ | Complement lookup during linear scan; optimal amortized time. |
| **Tier 2 (Space-Optimized)** | Sort + In-Place Two Pointers | $O(N \log N)$ | $O(N)$ | Preserves original indices via pair array; avoids dynamic hash tables. |
| **Tier 3 (Time-Optimized Alternative)** | Two-Pass Hash Map | $O(N)$ | $O(N)$ | Pre-populates entire frequency table; requires duplicate handling. |
| **Tier 4 (Brute Force)** | Exhaustive Pairwise Enumeration | $O(N^2)$ | $O(1)$ | Tests every $(i, j)$ pair; minimal memory overhead but quadratic latency. |

---

## 3. Tier 1: Most Optimal Solution (One-Pass Hash Map)

### 3.1 Algorithmic Mechanics and Invariant Proof

The fundamental observation is that for every element $x = \text{nums}[i]$, the complementary value needed to reach the target is uniquely determined as $y = \text{target} - x$.
Instead of scanning the rest of the array to locate $y$, we maintain a hash table mapping seen values to their respective indices.

During iteration at index $i$:
1. Compute the complement $y = \text{target} - \text{nums}[i]$.
2. Query the hash table for $y$.
3. If $y$ exists in the table at index $j$, we have found the unique pair $(j, i)$ and terminate immediately.
4. Otherwise, insert the entry $(\text{nums}[i], i)$ into the hash table and advance $i$.

**Correctness Invariant**:
Since exactly one solution $(j^*, i^*)$ exists with $j^* < i^*$, when the pointer reaches $i^*$, the complement $\text{nums}[j^*]$ is guaranteed to already reside in the hash table.
This eliminates forward searches entirely.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ average. Each insertion and lookup in a hash table runs in $O(1)$ amortized time. In the worst case under pathological hash collisions, it degrades to $O(N^2)$, though universal hashing mitigates this.
- **Space Complexity**: $O(N)$. In the worst case, the hash map stores $N - 1$ key-value pairs before locating the matching pair at the final index.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_map>

class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        std::unordered_map<int, int> lookup;
        for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
            int complement = target - nums[i];
            auto it = lookup.find(complement);
            if (it != lookup.end()) {
                return {it->second, i};
            }
            lookup[nums[i]] = i;
        }
        return {};
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        seen = {}
        for i, num in enumerate(nums):
            complement = target - num
            if complement in seen:
                return [seen[complement], i]
            seen[num] = i
        return []
```

#### Java 21
```java
import java.util.HashMap;
import java.util.Map;

class Solution {
    public int[] twoSum(int[] nums, int target) {
        Map<Integer, Integer> seen = new HashMap<>();
        for (int i = 0; i < nums.length; i++) {
            int complement = target - nums[i];
            if (seen.containsKey(complement)) {
                return new int[]{seen.get(complement), i};
            }
            seen.put(nums[i], i);
        }
        return new int[0];
    }
}
```

#### TypeScript
```typescript
function twoSum(nums: number[], target: number): number[] {
    const seen = new Map<number, number>();
    for (let i = 0; i < nums.length; i++) {
        const complement = target - nums[i];
        if (seen.has(complement)) {
            return [seen.get(complement)!, i];
        }
        seen.set(nums[i], i);
    }
    return [];
}
```

#### Go
```go
package leetcode

func twoSum(nums []int, target int) []int {
    seen := make(map[int]int, len(nums))
    for i, num := range nums {
        complement := target - num
        if j, ok := seen[complement]; ok {
            return []int{j, i}
        }
        seen[num] = i
    }
    return nil
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
        let mut seen = HashMap::with_capacity(nums.len());
        for (i, &num) in nums.iter().enumerate() {
            let complement = target - num;
            if let Some(&j) = seen.get(&complement) {
                return vec![j as i32, i as i32];
            }
            seen.insert(num, i);
        }
        vec![]
    }
}
```

---

## 4. Tier 2: Space-Optimized Solution (Sort + Two Pointers)

### 4.1 Trade-Off Analysis
When working in memory-constrained embedded environments where dynamic hash allocation overhead (and hash collisions) is unacceptable, we trade time for predictable memory.
Because the problem requires returning original indices, we store elements with their original index as a list of pairs before sorting.
Once sorted, two pointers (`left` at 0, `right` at $N-1$) converge toward each other:
- If `sum == target`: Return stored original indices.
- If `sum < target`: Increment `left` to increase the sum.
- If `sum > target`: Decrement `right` to decrease the sum.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$ driven by the sort phase. The two-pointer traversal takes $O(N)$.
- **Space Complexity**: $O(N)$ auxiliary to store `(value, original_index)` pairs. In algorithms where only values (not indices) are required, this runs in $O(1)$ auxiliary space.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        int n = nums.size();
        std::vector<std::pair<int, int>> indexed(n);
        for (int i = 0; i < n; ++i) {
            indexed[i] = {nums[i], i};
        }
        std::sort(indexed.begin(), indexed.end());
        int left = 0, right = n - 1;
        while (left < right) {
            int sum = indexed[left].first + indexed[right].first;
            if (sum == target) {
                return {indexed[left].second, indexed[right].second};
            } else if (sum < target) {
                ++left;
            } else {
                --right;
            }
        }
        return {};
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        indexed = sorted([(val, idx) for idx, val in enumerate(nums)])
        left, right = 0, len(nums) - 1
        while left < right:
            current_sum = indexed[left][0] + indexed[right][0]
            if current_sum == target:
                return [indexed[left][1], indexed[right][1]]
            elif current_sum < target:
                left += 1
            else:
                right -= 1
        return []
```

#### Java 21
```java
import java.util.Arrays;
import java.util.Comparator;

class Solution {
    public int[] twoSum(int[] nums, int target) {
        int n = nums.length;
        int[][] indexed = new int[n][2];
        for (int i = 0; i < n; i++) {
            indexed[i][0] = nums[i];
            indexed[i][1] = i;
        }
        Arrays.sort(indexed, Comparator.comparingInt(a -> a[0]));
        int left = 0, right = n - 1;
        while (left < right) {
            int sum = indexed[left][0] + indexed[right][0];
            if (sum == target) {
                return new int[]{indexed[left][1], indexed[right][1]};
            } else if (sum < target) {
                left++;
            } else {
                right--;
            }
        }
        return new int[0];
    }
}
```

#### TypeScript
```typescript
function twoSum(nums: number[], target: number): number[] {
    const indexed = nums.map((val, idx) => ({ val, idx })).sort((a, b) => a.val - b.val);
    let left = 0, right = nums.length - 1;
    while (left < right) {
        const sum = indexed[left].val + indexed[right].val;
        if (sum === target) {
            return [indexed[left].idx, indexed[right].idx];
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }
    return [];
}
```

#### Go
```go
package leetcode

import "sort"

func twoSum(nums []int, target int) []int {
    type item struct{ val, idx int }
    indexed := make([]item, len(nums))
    for i, v := range nums {
        indexed[i] = item{val: v, idx: i}
    }
    sort.Slice(indexed, func(i, j int) bool {
        return indexed[i].val < indexed[j].val
    })
    left, right := 0, len(nums)-1
    for left < right {
        sum := indexed[left].val + indexed[right].val
        if sum == target {
            return []int{indexed[left].idx, indexed[right].idx}
        } else if sum < target {
            left++
        } else {
            right--
        }
    }
    return nil
}
```

#### Rust
```rust
impl Solution {
    pub fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
        let mut indexed: Vec<(i32, usize)> = nums.into_iter().enumerate().map(|(i, v)| (v, i)).collect();
        indexed.sort_unstable_by_key(|&(v, _)| v);
        let mut left = 0;
        let mut right = indexed.len() - 1;
        while left < right {
            let sum = indexed[left].0 + indexed[right].0;
            if sum == target {
                return vec![indexed[left].1 as i32, indexed[right].1 as i32];
            } else if sum < target {
                left += 1;
            } else {
                right -= 1;
            }
        }
        vec![]
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Two-Pass Hash Map)

### 5.1 Trade-Off Analysis
In this variation, Pass 1 inserts all `nums[i] -> i` pairs into the table.
Pass 2 inspects each index $i$, querying if `target - nums[i]` exists and is distinct from $i$ (`lookup[complement] != i`).
While still $O(N)$ time, it performs twice as many hash lookups as the one-pass approach and requires explicit index distinction checks to avoid pairing an element with itself.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ (Two linear scans across the array).
- **Space Complexity**: $O(N)$ (Stores all $N$ elements).

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_map>

class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        std::unordered_map<int, int> lookup;
        for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
            lookup[nums[i]] = i;
        }
        for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
            int complement = target - nums[i];
            auto it = lookup.find(complement);
            if (it != lookup.end() && it->second != i) {
                return {i, it->second};
            }
        }
        return {};
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        table = {num: i for i, num in enumerate(nums)}
        for i, num in enumerate(nums):
            complement = target - num
            if complement in table and table[complement] != i:
                return [i, table[complement]]
        return []
```

#### Java 21
```java
import java.util.HashMap;
import java.util.Map;

class Solution {
    public int[] twoSum(int[] nums, int target) {
        Map<Integer, Integer> map = new HashMap<>();
        for (int i = 0; i < nums.length; i++) {
            map.put(nums[i], i);
        }
        for (int i = 0; i < nums.length; i++) {
            int complement = target - nums[i];
            if (map.containsKey(complement) && map.get(complement) != i) {
                return new int[]{i, map.get(complement)};
            }
        }
        return new int[0];
    }
}
```

#### TypeScript
```typescript
function twoSum(nums: number[], target: number): number[] {
    const map = new Map<number, number>();
    nums.forEach((num, idx) => map.set(num, idx));
    for (let i = 0; i < nums.length; i++) {
        const complement = target - nums[i];
        if (map.has(complement) && map.get(complement) !== i) {
            return [i, map.get(complement)!];
        }
    }
    return [];
}
```

#### Go
```go
package leetcode

func twoSum(nums []int, target int) []int {
    lookup := make(map[int]int, len(nums))
    for i, v := range nums {
        lookup[v] = i
    }
    for i, v := range nums {
        complement := target - v
        if j, ok := lookup[complement]; ok && j != i {
            return []int{i, j}
        }
    }
    return nil
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
        let mut lookup = HashMap::new();
        for (i, &num) in nums.iter().enumerate() {
            lookup.insert(num, i);
        }
        for (i, &num) in nums.iter().enumerate() {
            let complement = target - num;
            if let Some(&j) = lookup.get(&complement) {
                if j != i {
                    return vec![i as i32, j as i32];
                }
            }
        }
        vec![]
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Exhaustive Search)

### 6.1 Algorithmic Mechanics
Iterate through all pairs of indices $(i, j)$ where $0 \le i < j < N$.
Evaluate if $\text{nums}[i] + \text{nums}[j] == \text{target}$.
The total number of pair evaluations is:

$$\binom{N}{2} = \frac{N(N - 1)}{2} = O(N^2)$$

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(1)$ auxiliary memory (zero heap allocations).

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        int n = nums.size();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (nums[i] + nums[j] == target) {
                    return {i, j};
                }
            }
        }
        return {};
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        n = len(nums)
        for i in range(n):
            for j in range(i + 1, n):
                if nums[i] + nums[j] == target:
                    return [i, j]
        return []
```

#### Java 21
```java
class Solution {
    public int[] twoSum(int[] nums, int target) {
        int n = nums.length;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (nums[i] + nums[j] == target) {
                    return new int[]{i, j};
                }
            }
        }
        return new int[0];
    }
}
```

#### TypeScript
```typescript
function twoSum(nums: number[], target: number): number[] {
    const n = nums.length;
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            if (nums[i] + nums[j] === target) {
                return [i, j];
            }
        }
    }
    return [];
}
```

#### Go
```go
package leetcode

func twoSum(nums []int, target int) []int {
    n := len(nums)
    for i := 0; i < n; i++ {
        for j := i + 1; j < n; j++ {
            if nums[i]+nums[j] == target {
                return []int{i, j}
            }
        }
    }
    return nil
}
```

#### Rust
```rust
impl Solution {
    pub fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
        let n = nums.len();
        for i in 0..n {
            for j in (i + 1)..n {
                if nums[i] + nums[j] == target {
                    return vec![i as i32, j as i32];
                }
            }
        }
        vec![]
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why does the one-pass hash map correctly handle duplicate values such as `nums = [3, 3], target = 6`?</summary>
When processing the second `3` at index 1, the complement is `6 - 3 = 3`.
The first `3` was inserted at index 0 during the preceding iteration.
The lookup finds index 0 and immediately returns `[0, 1]` without causing key collision issues because the entry is checked before the second element is inserted.
</details>

<details>
<summary>2. What is the danger of using a two-pass hash map when duplicate values exist in the input?</summary>
In a two-pass hash map, if duplicate numbers exist (e.g. `[3, 3]`), inserting both into a standard map will cause the second occurrence to overwrite the first.
However, because the second occurrence holds the higher index, querying `target - nums[0]` will find index 1, which satisfies `j != i` and still yields a correct answer.
Nevertheless, one-pass avoids all map overwrites entirely.
</details>

<details>
<summary>3. Why can integer overflow occur in languages like C++ or Java during Two Sum calculations?</summary>
If constraints allow values near $2^{31} - 1$, the expression `target - nums[i]` or `nums[i] + nums[j]` can overflow standard 32-bit signed integers (`INT_MAX`).
In LeetCode 1, constraints specify values up to $10^9$, which safely fit inside 32-bit signed integers ($[-2.14 \times 10^9, 2.14 \times 10^9]$).
</details>

<details>
<summary>4. What is the time complexity difference between `std::unordered_map` and `std::map` in C++?</summary>
`std::unordered_map` uses a hash table providing $O(1)$ average time complexity for lookup and insertion.
`std::map` uses a self-balancing Red-Black binary search tree providing strict $O(\log N)$ worst-case time complexity.
Using `std::map` increases total Two Sum time complexity to $O(N \log N)$.
</details>

<details>
<summary>5. How can an adversarial test case degrade the one-pass hash table solution to $O(N^2)$ in C++?</summary>
In GCC/Clang, `std::unordered_map<int, int>` uses a default identity hash function for integer keys.
An attacker crafting input values with identical modulo bucket values can force all elements into a single collision bucket, degrading lookup to an $O(N)$ linked-list traversal and the entire algorithm to $O(N^2)$.
In competitive programming, custom hash functors (such as `custom_hash` with splitmix64) prevent anti-hash tests.
</details>

<details>
<summary>6. Can Two Sum be solved in $O(1)$ auxiliary space if the input array is already sorted?</summary>
Yes. If the array is guaranteed to be sorted, the Two-Pointer technique (`left = 0`, `right = N - 1`) solves the problem in $O(N)$ time and $O(1)$ auxiliary space without requiring a hash map.
</details>

<details>
<summary>7. What happens if the problem allows multiple valid pairs? How does the optimal solution adapt?</summary>
If multiple pairs exist and all must be collected without duplicate index pairs, the two-pointer approach on a sorted array can skip identical elements after finding a match (`while (left < right && nums[left] == nums[left + 1]) ++left;`).
With a hash map, we map each value to a list of its indices and iterate through all pairings.
</details>

<details>
<summary>8. How does Rust's borrow checker ensure memory safety in the Two Sum HashMap implementation?</summary>
Rust requires borrowing `&complement` for map lookup and moving ownership or borrowing `num` for insertion.
By using `.iter().enumerate()`, we borrow each element cleanly as `&num` while the HashMap owns copyable `i32` keys and `usize` values, preventing iterator invalidation or memory corruption.
</details>

<details>
<summary>9. Why is the sort-based Two Pointer method considered space-optimized even though it takes $O(N)$ space here?</summary>
It requires $O(N)$ space only because we must return original indices, necessitating an array of index pairs.
If the problem asks whether a pair exists or asks to return the values themselves, in-place sorting uses $O(1)$ auxiliary space (or $O(\log N)$ recursion stack), making it strictly superior in memory usage to a hash table.
</details>

<details>
<summary>10. What is the CPU cache locality trade-off between the One-Pass Hash Map and the Two-Pointer Sort approach?</summary>
Hash tables exhibit poor spatial cache locality because buckets and linked nodes are scattered across arbitrary heap memory addresses, resulting in frequent CPU cache misses.
In contrast, contiguous array sorting and two-pointer scanning access memory sequentially, maximizing L1/L2 cache prefetching and often outperforming hash tables on small to medium inputs.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/two-sum.cpp)
- [Python Implementation](../Python/two-sum.py)
- [Java Implementation](../Java/two-sum.java)
- [TypeScript Implementation](../TypeScript/two-sum.ts)
- [Go Implementation](../Golang/two-sum.go)
- [Rust Implementation](../Rust/two-sum.rs)

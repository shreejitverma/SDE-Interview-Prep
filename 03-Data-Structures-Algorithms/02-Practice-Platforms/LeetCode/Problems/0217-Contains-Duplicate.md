---
id: leetcode-0217-contains-duplicate
title: "LeetCode 0217: Contains Duplicate"
tags:
  - dsa
  - leetcode
  - array
  - hash-table
  - sorting
level: easy
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/contains-duplicate/"
---

# LeetCode 0217: Contains Duplicate

## 1. Problem Formalization and Constraints

Given an integer array `nums`, return `true` if any value appears at least twice in the array, and return `false` if every element is distinct.

### Constraints
- $1 \le \text{nums.length} \le 10^5$
- $-10^9 \le \text{nums}[i] \le 10^9$

### Examples
- **Example 1**:
  - Input: `nums = [1,2,3,1]`
  - Output: `true`
- **Example 2**:
  - Input: `nums = [1,2,3,4]`
  - Output: `false`
- **Example 3**:
  - Input: `nums = [1,1,1,3,3,4,3,2,4,2]`
  - Output: `true`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Hash Set with Early Termination | $O(N)$ | $O(N)$ | Constant-time insertion with immediate exit upon collision detection. |
| **Tier 2 (Space-Optimized)** | In-Place Sort + Adjacent Scanning | $O(N \log N)$ | $O(1)$ | Eliminates dynamic heap allocations by rearranging elements sequentially. |
| **Tier 3 (Time-Optimized Alternative)** | Frequency Map Counting | $O(N)$ | $O(N)$ | Full multi-set frequency tracking for cardinality queries. |
| **Tier 4 (Brute Force)** | Exhaustive Pairwise Comparison | $O(N^2)$ | $O(1)$ | Quadratic comparison of all $(i, j)$ index pairs; TLE on large arrays. |

---

## 3. Tier 1: Most Optimal Solution (Hash Set with Early Termination)

### 3.1 Algorithmic Mechanics and Invariant Proof

We iterate through the array elements while maintaining a hash set of previously encountered distinct integers.
For each element $x \in \text{nums}$:
1. Query membership $x \in S$.
2. If $x \in S$, return `true` immediately.
3. If $x \notin S$, insert $x$ into $S$ and continue.
If the loop terminates after processing all $N$ elements without early return, all elements are unique, and we return `false`.

**Pigeonhole Invariant**:
If any duplicate exists at indices $i < j$ with $\text{nums}[i] == \text{nums}[j]$, the set $S$ contains $\text{nums}[i]$ when processing index $j$, guaranteeing detection in at most $j$ steps.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ average. Set lookups and insertions operate in $O(1)$ amortized time.
- **Space Complexity**: $O(N)$. At most $N$ unique integers are retained in the hash set.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_set>

class Solution {
public:
    bool containsDuplicate(const std::vector<int>& nums) {
        std::unordered_set<int> lookup;
        lookup.reserve(nums.size());
        for (int num : nums) {
            if (!lookup.insert(num).second) {
                return true;
            }
        }
        return false;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def containsDuplicate(self, nums: List[int]) -> bool:
        seen = set()
        for num in nums:
            if num in seen:
                return True
            seen.add(num)
        return False
```

#### Java 21
```java
import java.util.HashSet;
import java.util.Set;

class Solution {
    public boolean containsDuplicate(int[] nums) {
        Set<Integer> lookup = new HashSet<>();
        for (int num : nums) {
            if (!lookup.add(num)) {
                return true;
            }
        }
        return false;
    }
}
```

#### TypeScript
```typescript
function containsDuplicate(nums: number[]): boolean {
    const lookup = new Set<number>();
    for (const num of nums) {
        if (lookup.has(num)) {
            return true;
        }
        lookup.add(num);
    }
    return false;
}
```

#### Go
```go
package main

func containsDuplicate(nums []int) bool {
    lookup := make(map[int]struct{}, len(nums))
    for _, num := range nums {
        if _, exists := lookup[num]; exists {
            return true
        }
        lookup[num] = struct{}{}
    }
    return false
}
```

#### Rust
```rust
use std::collections::HashSet;

impl Solution {
    pub fn contains_duplicate(nums: Vec<i32>) -> bool {
        let mut lookup = HashSet::with_capacity(nums.len());
        for num in nums {
            if !lookup.insert(num) {
                return true;
            }
        }
        false
    }
}
```

---

## 4. Tier 2: Space-Complexity Optimized Solution (In-Place Sort)

### 4.1 Algorithmic Mechanics and Adjacent Scan

When memory constraints prohibit auxiliary heap allocation, we sort the array in-place.
Sorting clusters identical elements into contiguous memory positions.
A single linear pass checking if $\text{nums}[i] == \text{nums}[i+1]$ suffices to detect duplicates.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N \log N)$ governed by IntroSort / Dual-Pivot QuickSort / Pattern-defeating QuickSort.
- **Space Complexity**: $O(1)$ auxiliary space if sorting in-place (or $O(\log N)$ implicit recursion call stack).

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    bool containsDuplicate(std::vector<int>& nums) {
        std::sort(nums.begin(), nums.end());
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i] == nums[i - 1]) {
                return true;
            }
        }
        return false;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def containsDuplicate(self, nums: List[int]) -> bool:
        nums.sort()
        for i in range(1, len(nums)):
            if nums[i] == nums[i - 1]:
                return True
        return False
```

#### Java 21
```java
import java.util.Arrays;

class Solution {
    public boolean containsDuplicate(int[] nums) {
        Arrays.sort(nums);
        for (int i = 1; i < nums.length; i++) {
            if (nums[i] == nums[i - 1]) {
                return true;
            }
        }
        return false;
    }
}
```

#### TypeScript
```typescript
function containsDuplicate(nums: number[]): boolean {
    nums.sort((a, b) => a - b);
    for (let i = 1; i < nums.length; i++) {
        if (nums[i] === nums[i - 1]) {
            return true;
        }
    }
    return false;
}
```

#### Go
```go
package main

import "sort"

func containsDuplicate(nums []int) bool {
    sort.Ints(nums)
    for i := 1; i < len(nums); i++ {
        if nums[i] == nums[i-1] {
            return true
        }
    }
    return false
}
```

#### Rust
```rust
impl Solution {
    pub fn contains_duplicate(mut nums: Vec<i32>) -> bool {
        nums.sort_unstable();
        for i in 1..nums.len() {
            if nums[i] == nums[i - 1] {
                return true;
            }
        }
        false
    }
}
```

---

## 5. Tier 3: Time-Complexity Optimized Alternative (Frequency Table Tracking)

### 5.1 Algorithmic Mechanics and Frequency Accumulation

Rather than a simple boolean set, we record full item occurrence counts using a hash table or dictionary.
This alternative generalizes when the problem is modified to find the majority element or elements with count $\ge K$.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N)$ linear pass.
- **Space Complexity**: $O(N)$ storing counts for all distinct numbers.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_map>

class Solution {
public:
    bool containsDuplicate(const std::vector<int>& nums) {
        std::unordered_map<int, int> counts;
        for (int num : nums) {
            if (++counts[num] > 1) {
                return true;
            }
        }
        return false;
    }
};
```

#### Python 3.12
```python
from typing import List
from collections import Counter

class Solution:
    def containsDuplicate(self, nums: List[int]) -> bool:
        counts = Counter()
        for num in nums:
            counts[num] += 1
            if counts[num] > 1:
                return True
        return False
```

#### Java 21
```java
import java.util.HashMap;
import java.util.Map;

class Solution {
    public boolean containsDuplicate(int[] nums) {
        Map<Integer, Integer> counts = new HashMap<>();
        for (int num : nums) {
            int count = counts.getOrDefault(num, 0) + 1;
            if (count > 1) {
                return true;
            }
            counts.put(num, count);
        }
        return false;
    }
}
```

#### TypeScript
```typescript
function containsDuplicate(nums: number[]): boolean {
    const counts = new Map<number, number>();
    for (const num of nums) {
        const count = (counts.get(num) ?? 0) + 1;
        if (count > 1) {
            return true;
        }
        counts.set(num, count);
    }
    return false;
}
```

#### Go
```go
package main

func containsDuplicate(nums []int) bool {
    counts := make(map[int]int, len(nums))
    for _, num := range nums {
        counts[num]++
        if counts[num] > 1 {
            return true
        }
    }
    return false
}
```

#### Rust
```rust
use std::collections::HashMap;

impl Solution {
    pub fn contains_duplicate(nums: Vec<i32>) -> bool {
        let mut counts = HashMap::with_capacity(nums.len());
        for num in nums {
            let entry = counts.entry(num).or_insert(0);
            *entry += 1;
            if *entry > 1 {
                return true;
            }
        }
        false
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (Exhaustive Pairwise Comparison)

### 6.1 Algorithmic Mechanics

We inspect every pair of distinct indices $(i, j)$ with $0 \le i < j < N$ and check if $\text{nums}[i] == \text{nums}[j]$.
If any match is found, return `true`.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$ comparisons.
- **Space Complexity**: $O(1)$ auxiliary memory.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    bool containsDuplicate(const std::vector<int>& nums) {
        int n = nums.size();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (nums[i] == nums[j]) {
                    return true;
                }
            }
        }
        return false;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def containsDuplicate(self, nums: List[int]) -> bool:
        n = len(nums)
        for i in range(n):
            for j in range(i + 1, n):
                if nums[i] == nums[j]:
                    return True
        return False
```

#### Java 21
```java
class Solution {
    public boolean containsDuplicate(int[] nums) {
        int n = nums.length;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (nums[i] == nums[j]) {
                    return true;
                }
            }
        }
        return false;
    }
}
```

#### TypeScript
```typescript
function containsDuplicate(nums: number[]): boolean {
    const n = nums.length;
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            if (nums[i] === nums[j]) {
                return true;
            }
        }
    }
    return false;
}
```

#### Go
```go
package main

func containsDuplicate(nums []int) bool {
    n := len(nums)
    for i := 0; i < n; i++ {
        for j := i + 1; j < n; j++ {
            if nums[i] == nums[j] {
                return true
            }
        }
    }
    return false
}
```

#### Rust
```rust
impl Solution {
    pub fn contains_duplicate(nums: Vec<i32>) -> bool {
        let n = nums.len();
        for i in 0..n {
            for j in (i + 1)..n {
                if nums[i] == nums[j] {
                    return true;
                }
            }
        }
        false
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is `std::unordered_set::insert(x).second` faster than calling `find(x)` followed by `insert(x)`?</summary>
Calling `find` followed by `insert` performs two independent hash computations and bucket index searches.
In contrast, `insert(x)` computes the hash and navigates to the bucket once, returning a pair `std::pair<iterator, bool>`.
The boolean `.second` directly flags whether the insertion took place (new unique key) or failed because the key already existed.
</details>

<details>
<summary>2. Why does Python's `len(set(nums)) < len(nums)` perform differently from an early-exit loop?</summary>
Constructing `set(nums)` unconditionally allocates and hashes every single element in `nums` even if the very first two elements are identical.
The manual early-exit loop terminates in $O(1)$ time on best-case inputs (`nums[0] == nums[1]`), while `set(nums)` always incurs full $O(N)$ allocation and execution time.
</details>

<details>
<summary>3. Why is `nums.sort_unstable()` preferred over `nums.sort()` in Rust for this problem?</summary>
`sort_unstable()` uses pattern-defeating quicksort (pdqsort), which does not allocate auxiliary memory and runs faster than stable mergesort (Timsort).
Since we only compare identical primitive integers, stability (preserving relative order of equal keys) provides zero benefit.
</details>

<details>
<summary>4. How does reserving bucket capacity (`reserve(nums.size())`) optimize hash set performance?</summary>
Dynamic resizing triggers table rehashing when load factor limits are crossed, allocating larger bucket buffers and copying existing nodes.
Pre-reserving capacity allocates sufficient buckets up front, eliminating all dynamic rehash latency.
</details>

<details>
<summary>5. When is the $O(N \log N)$ sorting approach practically superior to the $O(N)$ hash set?</summary>
When working in memory-constrained environments (such as embedded firmware or low-footprint microcontrollers) where heap allocation is prohibited.
Additionally, contiguous array sorting benefits from high cache locality, avoiding pointer-chasing cache misses inherent to hash table buckets.
</details>

<details>
<summary>6. How can a BitSet or BitMap replace the hash set if values are constrained to $[0, K)$?</summary>
If numbers fall in a compact non-negative range $[0, K)$, a contiguous bit array where each bit represents the presence of an integer requires only $K / 8$ bytes of memory.
Membership tests and insertions execute in a single CPU cycle via bitwise shifts and bitwise OR operations.
</details>

<details>
<summary>7. What is the impact of Java's boxed `Integer` wrapper on GC overhead during large tests?</summary>
`HashSet<Integer>` boxes each primitive `int` into an object on the heap, consuming 16 to 24 bytes per node in addition to reference pointers.
For $10^5$ items, this creates significant garbage collection pressure.
Primitive collections (such as Fastutil's `IntOpenHashSet`) avoid boxing and drastically reduce memory footprint.
</details>

<details>
<summary>8. How does Go handle set lookups idiomatic without a built-in Set container?</summary>
Go uses `map[K]struct{}`.
Because `struct{}` is an empty struct of zero bytes, the map stores keys without allocating memory for values, serving as an optimal zero-memory-cost hash set.
</details>

<details>
<summary>9. How does this problem relate to the Birthday Paradox in hashing?</summary>
In an array of length $N$ where elements are drawn uniformly from a universe $U$, the probability of a collision reaches $50\%$ when $N \approx \sqrt{2U \ln 2}$.
For hash tables with hash universe $2^{32}$, collisions become statistically probable after only $\approx 77,000$ distinct items.
</details>

<details>
<summary>10. How can SIMD instructions accelerate duplicate detection on sorted data?</summary>
On sorted vectors, checking $\text{nums}[i] == \text{nums}[i+1]$ across 8 or 16 integers simultaneously is done using vector comparison intrinsics (`_mm256_cmpeq_epi32`).
A single vector bitmask test (`_mm256_movemask_epi8`) reveals adjacent duplicate pairs across 8 elements in parallel in one CPU cycle.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/contains-duplicate.cpp)
- [Python Implementation](../Python/contains-duplicate.py)
- [Java Implementation](../Java/contains-duplicate.java)
- [TypeScript Implementation](../TypeScript/contains-duplicate.ts)
- [Go Implementation](../Golang/contains-duplicate.go)
- [Rust Implementation](../Rust/contains-duplicate.rs)

---
id: leetcode-0128-longest-consecutive-sequence
title: "LeetCode 0128: Longest Consecutive Sequence"
tags:
  - dsa
  - leetcode
  - array
  - hash-table
  - union-find
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/longest-consecutive-sequence/"
---

# LeetCode 0128: Longest Consecutive Sequence

## 1. Problem Formalization and Constraints

Given an unsorted array of integers `nums`, return the length of the longest consecutive elements sequence.
You must write an algorithm that runs in $O(N)$ time.

### Constraints
- $0 \le \text{nums.length} \le 10^5$
- $-10^9 \le \text{nums}[i] \le 10^9$

### Examples
- **Example 1**:
  - Input: `nums = [100,4,200,1,3,2]`
  - Output: `4`
  - Explanation: The longest consecutive elements sequence is `[1, 2, 3, 4]`. Its length is 4.
- **Example 2**:
  - Input: `nums = [0,3,7,2,5,8,4,6,0,1]`
  - Output: `9`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Hash Set Sequence Head Detection | $O(N)$ | $O(N)$ auxiliary | Inserts elements into a hash set; only traverses sequences when `num - 1` is absent, guaranteeing $2N$ lookups. |
| **Tier 2 (Union-Find)** | Disjoint Set Union (DSU) | $O(N \alpha(N))$ | $O(N)$ auxiliary | Unifies `num` with `num + 1` if present; tracks component sizes using path compression and union by rank. |
| **Tier 3 (Sorting)** | Comparison-Based Sorting | $O(N \log N)$ | $O(1)$ or $O(N)$ auxiliary | Sorts array and counts consecutive elements in a single pass; violates strict $O(N)$ requirement. |
| **Tier 4 (Brute Force)** | Linear Search from Every Number | $O(N^3)$ | $O(1)$ auxiliary | For each element, repeatedly searches array for $x + 1$, $x + 2$, etc. |

---

## 3. Tier 1: Most Optimal Solution (Hash Set Sequence Head Detection)

### 3.1 Algorithmic Mechanics and Invariant Proof

To achieve $O(N)$ time without sorting, populate a hash set with all unique elements in `nums`.
For each element `num` in the set:
1. Check if `num - 1` exists in the set.
2. If `num - 1` exists, then `num` cannot be the start of a consecutive sequence, so skip it immediately.
3. If `num - 1` does not exist, `num` is the minimal element (the head) of a contiguous streak.
4. Incrementally count upwards (`num + 1`, `num + 2`, ...) until an element is missing from the set.
5. Update `longest_streak = max(longest_streak, current_streak)`.

**Invariant Proof**:
Let $S$ be the set of unique values in `nums`.
The integers in $S$ partition uniquely into disjoint maximal consecutive intervals $I_1, I_2, \dots, I_m$ where each $I_j = [a_j, b_j] \cap \mathbb{Z}$ with $a_j - 1 \notin S$ and $b_j + 1 \notin S$.
An element $x \in S$ triggers the inner `while` loop if and only if $x - 1 \notin S$.
Therefore, the inner loop is entered exactly once per interval $I_j$, precisely at its minimum element $a_j$.
Inside the inner loop, elements $a_j, a_j + 1, \dots, b_j$ are checked sequentially, performing exactly $|I_j| + 1$ hash lookups.
Summing across all disjoint intervals:
$$\text{Total Lookups} = |S| + \sum_{j=1}^m (|I_j| + 1) \le |S| + |S| + m \le 3|S| \le 3N$$
Because average hash set lookups take $O(1)$ time, the algorithm executes in guaranteed $O(N)$ amortized time.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$ amortized. Building the set takes $O(N)$. Each number is checked at most twice in the outer loop and inner loop.
- **Auxiliary Space Complexity**: $O(N)$ auxiliary space to store elements in the hash set.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_set>
#include <algorithm>

class Solution {
public:
    int longestConsecutive(std::vector<int>& nums) {
        std::unordered_set<int> num_set(nums.begin(), nums.end());
        int longest_streak = 0;

        for (int num : num_set) {
            if (!num_set.count(num - 1)) {
                int current_num = num;
                int current_streak = 1;

                while (num_set.count(current_num + 1)) {
                    current_num += 1;
                    current_streak += 1;
                }

                longest_streak = std::max(longest_streak, current_streak);
            }
        }

        return longest_streak;
    }
};
```

#### Python 3
```python
from typing import List


class Solution:
    def longestConsecutive(self, nums: List[int]) -> int:
        num_set = set(nums)
        longest_streak = 0

        for num in num_set:
            if num - 1 not in num_set:
                current_num = num
                current_streak = 1

                while current_num + 1 in num_set:
                    current_num += 1
                    current_streak += 1

                longest_streak = max(longest_streak, current_streak)

        return longest_streak
```

#### Java 21
```java
import java.util.HashSet;
import java.util.Set;

class Solution {
    public int longestConsecutive(int[] nums) {
        Set<Integer> numSet = new HashSet<>();
        for (int num : nums) {
            numSet.add(num);
        }

        int longestStreak = 0;

        for (int num : numSet) {
            if (!numSet.contains(num - 1)) {
                int currentNum = num;
                int currentStreak = 1;

                while (numSet.contains(currentNum + 1)) {
                    currentNum += 1;
                    currentStreak += 1;
                }

                longestStreak = Math.max(longestStreak, currentStreak);
            }
        }

        return longestStreak;
    }
}
```

#### TypeScript
```typescript
function longestConsecutive(nums: number[]): number {
    const numSet = new Set<number>(nums);
    let longestStreak = 0;

    for (const num of numSet) {
        if (!numSet.has(num - 1)) {
            let currentNum = num;
            let currentStreak = 1;

            while (numSet.has(currentNum + 1)) {
                currentNum += 1;
                currentStreak += 1;
            }

            longestStreak = Math.max(longestStreak, currentStreak);
        }
    }

    return longestStreak;
}
```

#### Go
```go
package main

func longestConsecutive(nums []int) int {
	numSet := make(map[int]struct{}, len(nums))
	for _, num := range nums {
		numSet[num] = struct{}{}
	}

	longestStreak := 0

	for num := range numSet {
		if _, exists := numSet[num-1]; !exists {
			currentNum := num
			currentStreak := 1

			for {
				if _, hasNext := numSet[currentNum+1]; hasNext {
					currentNum++
					currentStreak++
				} else {
					break
				}
			}

			if currentStreak > longestStreak {
				longestStreak = currentStreak
			}
		}
	}

	return longestStreak
}
```

#### Rust
```rust
use std::collections::HashSet;

pub struct Solution;

impl Solution {
    pub fn longest_consecutive(nums: Vec<i32>) -> i32 {
        let num_set: HashSet<i32> = nums.into_iter().collect();
        let mut longest_streak = 0;

        for &num in &num_set {
            if !num_set.contains(&(num - 1)) {
                let mut current_num = num;
                let mut current_streak = 1;

                while num_set.contains(&(current_num + 1)) {
                    current_num += 1;
                    current_streak += 1;
                }

                longest_streak = longest_streak.max(current_streak);
            }
        }

        longest_streak
    }
}
```

---

## 4. Tier 2: Disjoint Set Union (Union-Find)

### 4.1 Mechanical Description
Map each unique number to a parent pointer and component size.
For each `num` in the set:
If `num + 1` exists in the set, union the set containing `num` with the set containing `num + 1`.
Track component sizes and return the maximum component size across all roots.

### 4.2 Trade-offs
- Naturally supports dynamic updates where numbers are streamed one at a time.
- Requires $O(N \alpha(N))$ time due to Ackermann inverse overhead and higher constant factors from tree pointer updates.

---

## 5. Tier 3: Comparison-Based Sorting

### 5.1 Mechanical Description
Sort `nums` in ascending order.
Iterate through the sorted array, skipping duplicate elements (`nums[i] == nums[i-1]`).
If `nums[i] == nums[i-1] + 1`, increment the current streak; otherwise, reset to 1.

### 5.2 Trade-offs
- Simple to implement and requires $O(1)$ auxiliary space if in-place sorting is used.
- Violates the problem's strict $O(N)$ time complexity requirement ($O(N \log N)$).

---

## 6. Tier 4: Linear Search Brute Force

### 6.1 Mechanical Description
For each element `x` in `nums`, check if `x + 1` exists by scanning the array ($O(N)$), then check `x + 2`, etc.
Yields worst-case time complexity of $O(N^3)$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Hash Table Overhead**: Node-based hash tables (like `std::unordered_set` in libstdc++) allocate separate heap nodes per element, causing cache misses. Flat open-addressing hash sets (like Rust's `HashSet` with SwissTable or C++ `absl::flat_hash_set`) achieve much higher cache locality.
2. **Duplicate Deduplication**: Populating the set eliminates duplicate values upfront, avoiding redundant sequence expansions.
3. **Range Extremes**: Values up to $10^9$ prevent direct array indexing, necessitating hashing rather than direct boolean flags.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Empty array | `nums = []` | Returns `0` | Loop does not execute; returns initial 0. |
| Single element | `nums = [1]` | Returns `1` | `num - 1` is absent; inner loop checks `num + 1`, returns 1. |
| All identical elements | `nums = [2, 2, 2, 2]` | Returns `1` | Set deduplication reduces input to `{2}`, yielding streak of 1. |
| Negative integers | `nums = [-3, -2, -1, 0, 1]` | Returns `5` | Arithmetic handles negative bounds without special cases. |
| Integer overflow at boundaries | `nums = [INT_MAX]` | Does not overflow | Adding 1 could overflow 32-bit signed int if not guarded or if typed as 64-bit. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does checking `num - 1` ensure $O(N)$ complexity?
It guarantees that each sequence is traversed starting only from its smallest number. Non-head numbers are skipped in $O(1)$ time.

### 2. What happens if hash collisions occur?
With standard hashing, average time is $O(N)$. Pathological anti-hash test cases could cause $O(N^2)$ degradation in languages without randomized hash seeds.

### 3. Why is sorting not allowed if it easily fits within time limits?
The problem statement explicitly specifies: "You must write an algorithm that runs in $O(N)$ time."

### 4. How does Rust's `HashSet` perform compared to C++ `std::unordered_set`?
Rust's standard `HashSet` uses SwissTable SIMD-accelerated open addressing, resulting in faster lookups and better cache locality than C++'s default chained hash table.

### 5. Can Radix Sort achieve $O(N)$ time?
Yes, Radix Sort can sort 32-bit integers in $O(N)$ time (with a constant factor of 4 passes for 8-bit bytes), but the hash set solution is more idiomatic and general.

### 6. Does this problem support floating-point numbers?
No, the concept of consecutive integers ($\Delta = 1$) is defined exclusively over integers.

### 7. How does Union-Find compare in memory usage?
Union-Find requires two hash maps (`parent` and `size`), doubling the memory consumption compared to a single hash set.

### 8. What is the maximum possible return value?
The maximum return value is $N = 10^5$, which easily fits in a standard integer.

### 9. Why does Go use `struct{}` as the map value?
In Go, `map[int]struct{}` uses zero bytes for map values, effectively implementing a memory-efficient set.

### 10. How does this relate to Longest Increasing Subsequence (LeetCode 300)?
Longest Increasing Subsequence does not require elements to be consecutive in value or contiguous in position, requiring $O(N \log N)$ dynamic programming.

---

## 10. Related Problems and Systematic Progression Links

- [[0001-Two-Sum]]: Hash table value indexing and lookup.
- [[0217-Contains-Duplicate]]: Set deduplication fundamentals.
- [[0300-Longest-Increasing-Subsequence]]: Finding longest subsequences under ordering constraints.
- LeetCode 298 (Binary Tree Longest Consecutive Sequence): Consecutive sequences in tree topologies.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/longest-consecutive-sequence.cpp)
- [Python Implementation](../Python/longest-consecutive-sequence.py)
- [Java Implementation](../Java/longest-consecutive-sequence.java)
- [TypeScript Implementation](../TypeScript/longest-consecutive-sequence.ts)
- [Go Implementation](../Golang/longest-consecutive-sequence.go)
- [Rust Implementation](../Rust/longest-consecutive-sequence.rs)

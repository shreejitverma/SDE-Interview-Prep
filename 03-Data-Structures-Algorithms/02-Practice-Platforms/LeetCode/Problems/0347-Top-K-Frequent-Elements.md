---
id: leetcode-0347-top-k-frequent-elements
title: "LeetCode 0347: Top K Frequent Elements"
tags:
  - dsa
  - leetcode
  - array
  - hash-table
  - divide-and-conquer
  - sorting
  - heap
  - bucket-sort
  - counting
  - quickselect
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/top-k-frequent-elements/"
---

# LeetCode 0347: Top K Frequent Elements

## 1. Problem Formalization and Constraints

Given an integer array `nums` and an integer `k`, return the `k` most frequent elements.
You may return the answer in any order.

### Constraints
- $1 \le \text{nums.length} \le 10^5$
- $-10^4 \le \text{nums}[i] \le 10^4$
- `k` is in the range $[1, \text{number of unique elements in the array}]$.
- It is guaranteed that the answer is unique.

### Follow-up
Your algorithm's time complexity must be better than $O(N \log N)$, where $N$ is the array's size.

### Examples
- **Example 1**:
  - Input: `nums = [1,1,1,2,2,3], k = 2`
  - Output: `[1,2]`
- **Example 2**:
  - Input: `nums = [1], k = 1`
  - Output: `[1]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Bucket Sort by Frequency | $O(N)$ | $O(N)$ auxiliary | Uses array of buckets indexed by frequency $[0 \dots N]$; collects top $k$ from right to left in strict linear time. |
| **Tier 2 (QuickSelect)** | Hoare's QuickSelect on Unique Elements | $O(N)$ average, $O(N^2)$ worst | $O(N)$ auxiliary | Partitions unique elements around pivot frequencies; avoids sorting entire array. |
| **Tier 3 (Min-Heap)** | Bounded Min-Heap of Size $k$ | $O(N \log k)$ | $O(N + k)$ auxiliary | Maintains heap of size $k$; evicts smallest frequency when heap size exceeds $k$. |
| **Tier 4 (Full Sort)** | Comparison-Based Sort | $O(N \log N)$ | $O(N)$ auxiliary | Sorts frequency-value pairs descending; violates the follow-up requirement. |

*Notation*: $N = \text{nums.length}$, $U$ is the number of unique elements ($U \le N$), and $k$ is the target rank.

---

## 3. Tier 1: Most Optimal Solution (Bucket Sort by Frequency)

### 3.1 Algorithmic Mechanics and Invariant Proof

The maximum frequency any element can have in `nums` is $N$.
1. Count the frequency of each element using a hash map `counts`.
2. Construct an array of buckets `buckets` of length $N + 1$, where `buckets[f]` stores a list of elements that appear exactly $f$ times.
3. For each `(num, freq)` in `counts`, append `num` to `buckets[freq]`.
4. Traverse `buckets` in descending order from index $N$ down to 1.
5. Append elements from each bucket to `result` until `result.size() == k`.
6. Return `result`.

**Invariant Proof**:
Let $f(x)$ be the frequency of element $x$ in `nums`.
The domain of $f$ is the set of unique elements $U \subseteq \text{nums}$, with $1 \le f(x) \le N$ for all $x \in U$.
The buckets partition $U$ into $N$ disjoint sets $B_1, B_2, \dots, B_N$ where $B_i = \{x \in U \mid f(x) = i\}$.
By definition, if $i > j$, every element in $B_i$ has strictly higher frequency than every element in $B_j$.
Iterating through buckets from index $N$ downwards processes elements in strictly non-increasing order of frequency.
The first $k$ elements encountered are therefore guaranteed to be the $k$ elements with the largest frequencies in the dataset.
Because $\sum_{i=1}^N |B_i| = |U| \le N$, scanning the buckets visits at most $N + |U| \le 2N$ total items, proving strict $O(N)$ time.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N)$. Counting frequencies takes $O(N)$. Placing unique elements into buckets takes $O(|U|) \le O(N)$. Scanning buckets from $N$ down to 1 takes $O(N + |U|) = O(N)$.
- **Auxiliary Space Complexity**: $O(N)$ auxiliary space to store the frequency map and bucket lists.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <unordered_map>

class Solution {
public:
    std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
        std::unordered_map<int, int> counts;
        for (int num : nums) {
            counts[num]++;
        }

        std::vector<std::vector<int>> buckets(nums.size() + 1);
        for (const auto& [num, freq] : counts) {
            buckets[freq].push_back(num);
        }

        std::vector<int> result;
        result.reserve(k);

        for (int i = static_cast<int>(buckets.size()) - 1; i >= 0 && static_cast<int>(result.size()) < k; --i) {
            for (int num : buckets[i]) {
                result.push_back(num);
                if (static_cast<int>(result.size()) == k) {
                    break;
                }
            }
        }

        return result;
    }
};
```

#### Python 3
```python
from collections import Counter
from typing import List


class Solution:
    def topKFrequent(self, nums: List[int], k: int) -> List[int]:
        counts = Counter(nums)
        buckets: List[List[int]] = [[] for _ in range(len(nums) + 1)]

        for num, freq in counts.items():
            buckets[freq].append(num)

        result: List[int] = []
        for i in range(len(buckets) - 1, 0, -1):
            for num in buckets[i]:
                result.append(num)
                if len(result) == k:
                    return result

        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

class Solution {
    public int[] topKFrequent(int[] nums, int k) {
        Map<Integer, Integer> counts = new HashMap<>();
        for (int num : nums) {
            counts.put(num, counts.getOrDefault(num, 0) + 1);
        }

        List<Integer>[] buckets = new List[nums.length + 1];
        for (int i = 0; i <= nums.length; i++) {
            buckets[i] = new ArrayList<>();
        }

        for (Map.Entry<Integer, Integer> entry : counts.entrySet()) {
            buckets[entry.getValue()].add(entry.getKey());
        }

        int[] result = new int[k];
        int idx = 0;

        for (int i = buckets.length - 1; i >= 0 && idx < k; i--) {
            for (int num : buckets[i]) {
                result[idx++] = num;
                if (idx == k) {
                    return result;
                }
            }
        }

        return result;
    }
}
```

#### TypeScript
```typescript
function topKFrequent(nums: number[], k: number): number[] {
    const counts = new Map<number, number>();
    for (const num of nums) {
        counts.set(num, (counts.get(num) || 0) + 1);
    }

    const buckets: number[][] = Array.from({ length: nums.length + 1 }, () => []);
    for (const [num, freq] of counts.entries()) {
        buckets[freq].push(num);
    }

    const result: number[] = [];
    for (let i = buckets.length - 1; i >= 0 && result.length < k; i--) {
        for (const num of buckets[i]) {
            result.push(num);
            if (result.length === k) {
                return result;
            }
        }
    }

    return result;
}
```

#### Go
```go
package main

func topKFrequent(nums []int, k int) []int {
	counts := make(map[int]int)
	for _, num := range nums {
		counts[num]++
	}

	buckets := make([][]int, len(nums)+1)
	for num, freq := range counts {
		buckets[freq] = append(buckets[freq], num)
	}

	result := make([]int, 0, k)
	for i := len(buckets) - 1; i >= 0 && len(result) < k; i-- {
		for _, num := range buckets[i] {
			result = append(result, num)
			if len(result) == k {
				return result
			}
		}
	}

	return result
}
```

#### Rust
```rust
use std::collections::HashMap;

pub struct Solution;

impl Solution {
    pub fn top_k_frequent(nums: Vec<i32>, k: i32) -> Vec<i32> {
        let n = nums.len();
        let mut counts = HashMap::new();
        for num in nums {
            *counts.entry(num).or_insert(0) += 1;
        }

        let mut buckets = vec![Vec::new(); n + 1];
        for (num, freq) in counts {
            buckets[freq].push(num);
        }

        let mut result = Vec::with_capacity(k as usize);
        for i in (1..=n).rev() {
            for &num in &buckets[i] {
                result.push(num);
                if result.len() == k as usize {
                    return result;
                }
            }
        }

        result
    }
}
```

---

## 4. Tier 2: QuickSelect on Unique Elements

### 4.1 Mechanical Description
Extract unique elements into a list of pairs `(frequency, value)`.
Run QuickSelect (Hoare's selection) targeting index $|U| - k$.
After partitioning, all elements to the right of index $|U| - k$ have frequencies greater than or equal to the pivot, yielding the top $k$ items.

### 4.2 Trade-offs
- Modifies array in place with $O(1)$ extra memory beyond the unique element list.
- Average time is $O(N)$, but worst-case time degrades to $O(N^2)$ on adversarial pivot selections unless median-of-medians is used.

---

## 5. Tier 3: Bounded Min-Heap of Size $k$

### 5.1 Mechanical Description
Count element frequencies.
Push pairs `(freq, num)` into a min-heap.
Whenever the heap size exceeds $k$, pop the minimum frequency element.
The heap retains exactly the $k$ most frequent elements.

### 5.2 Trade-offs
- Uses $O(k)$ auxiliary heap memory, ideal for streaming scenarios where $N$ is unbounded.
- Runs in $O(N \log k)$ time, which is slower than $O(N)$ bucket sort for large $k$.

---

## 6. Tier 4: Comparison-Based Full Sort

### 6.1 Mechanical Description
Count frequencies and sort all unique pairs descending by frequency.
Take the first $k$ elements.

### 6.2 Complexity & Deficiencies
- Runs in $O(N \log N)$ time, explicitly violating the follow-up constraint.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Bucket Array Allocation**: Allocating `vector<vector<int>>` of size $N + 1$ requires $N + 1$ vector headers. When $N = 10^5$, this consumes modest RAM ($\approx 2.4$ MB), which fits comfortably within modern L3 cache.
2. **Result Capacity Reserve**: Pre-allocating `result.reserve(k)` avoids dynamic memory reallocation during result extraction.
3. **Sparse Buckets**: Many buckets are empty; simple pointer skips over empty vectors take minimal CPU cycles.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| All elements identical | `nums = [1, 1, 1], k = 1` | Returns `[1]` | Bucket at index 3 contains 1; returned immediately. |
| All elements unique | `nums = [1, 2, 3], k = 2` | Returns any 2 elements | Bucket at index 1 contains all 3; takes first 2. |
| $k$ equals number of unique elements | `k = |U|` | Returns all unique elements | Traverses down to lowest bucket, gathering all items. |
| Negative numbers | `nums = [-1, -1, 2], k = 1` | Returns `[-1]` | Hash map keys support negative integers naturally. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does Bucket Sort achieve $O(N)$ time when general sorting takes $O(N \log N)$?
Because frequency is bounded by $[1, N]$. When keys are bounded integers, non-comparison sorts (like Bucket Sort and Counting Sort) run in linear time.

### 2. Can two elements have the same frequency?
Yes. The problem specifies that the final answer is unique, but intermediate elements can share frequencies as long as the boundary between $k$-th and $(k+1)$-th is distinct.

### 3. How does QuickSelect compare with Bucket Sort in practice?
QuickSelect uses less memory because it does not allocate $N + 1$ bucket arrays, but Bucket Sort has guaranteed $O(N)$ worst-case time without pivot degradation.

### 4. Why is a Min-Heap used instead of a Max-Heap for the $O(N \log k)$ solution?
A min-heap evicts the smallest among the top candidates, keeping the heap bounded to size $k$. A max-heap would need to store all $|U|$ elements ($O(N \log N)$).

### 5. What if the input array is an infinite stream?
For streaming data, Bucket Sort is inapplicable because $N$ is unbounded. The bounded Min-Heap approach ($O(N \log k)$) or Count-Min Sketch is preferred.

### 6. Can $k$ exceed the number of unique elements?
The problem constraints guarantee that $k$ is within $[1, \text{number of unique elements}]$.

### 7. How does Python's `Counter.most_common(k)` work?
Python's `most_common(k)` uses `heapq.nlargest`, achieving $O(N \log k)$ time using an underlying C implementation.

### 8. What is the maximum possible size of `buckets`?
$N + 1 = 100,001$ vectors, taking roughly 2.4 MB of memory.

### 9. Why does Java use an array of lists `List<Integer>[]`?
In Java, generic array creation `new List<Integer>[N + 1]` triggers unchecked cast warnings, but provides the fastest bucket indexed storage.

### 10. How does Rust initialize the bucket vector?
Rust uses `vec![Vec::new(); n + 1]`, initializing empty vectors without pre-allocating inner vector heap buffers until elements are inserted.

---

## 10. Related Problems and Systematic Progression Links

- [[0001-Two-Sum]]: Hash table frequency and value mapping.
- [[0049-Group-Anagrams]]: Frequency-based grouping.
- [[0217-Contains-Duplicate]]: Frequency threshold detection.
- [[0242-Valid-Anagram]]: Character frequency count matching.
- LeetCode 215 (Kth Largest Element in an Array): QuickSelect and heap selection.
- LeetCode 692 (Top K Frequent Words): Top $k$ elements with lexicographical tie-breaking.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/top-k-frequent-elements.cpp)
- [Python Implementation](../Python/top-k-frequent-elements.py)
- [Java Implementation](../Java/top-k-frequent-elements.java)
- [TypeScript Implementation](../TypeScript/top-k-frequent-elements.ts)
- [Go Implementation](../Golang/top-k-frequent-elements.go)
- [Rust Implementation](../Rust/top-k-frequent-elements.rs)

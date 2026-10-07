---
id: leetcode-0046-permutations
title: "LeetCode 0046: Permutations"
tags:
  - dsa
  - leetcode
  - array
  - backtracking
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/permutations/"
---

# LeetCode 0046: Permutations

## 1. Problem Formalization and Constraints

Given an array `nums` of distinct integers, return all the possible permutations.
You can return the answer in any order.

### Constraints
- $1 \le \text{nums.length} \le 6$
- $-10 \le \text{nums}[i] \le 10$
- All the integers of `nums` are unique.

### Examples
- **Example 1**:
  - Input: `nums = [1,2,3]`
  - Output: `[[1,2,3],[1,3,2],[2,1,3],[2,3,1],[3,1,2],[3,2,1]]`
- **Example 2**:
  - Input: `nums = [0,1]`
  - Output: `[[0,1],[1,0]]`
- **Example 3**:
  - Input: `nums = [1]`
  - Output: `[[1]]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | In-Place Swap Backtracking | $O(N \cdot N!)$ | $O(N)$ auxiliary | Swaps elements in place to generate permutations at position `first`; $O(1)$ auxiliary buffer space. |
| **Tier 2 (Visited Buffer)** | DFS with Visited Boolean Array | $O(N \cdot N!)$ | $O(N)$ auxiliary | Tracks element usage via boolean `used` array; constructs permutations in canonical order. |
| **Tier 3 (Iterative Insertion)**| Prefix Permutation Expansion | $O(N \cdot N!)$ | $O(N \cdot N!)$ auxiliary | Starts with `[[]]`; inserts next element at every index of existing permutations. |
| **Tier 4 (Next Permutation)** | Repeated Lexicographical Generation | $O(N \cdot N!)$ | $O(1)$ auxiliary | Sorts array initially and iteratively computes `next_permutation` $N! - 1$ times. |

---

## 3. Tier 1: Most Optimal Solution (In-Place Swap Backtracking)

### 3.1 Algorithmic Mechanics and Invariant Proof

Given an array `nums` of $N$ distinct elements:
1. Define a recursive function `backtrack(first)` where `first` is the index of the element currently being fixed.
2. When `first == N`, all positions have been fixed, so add a copy of `nums` to `result`.
3. For each index $i$ from `first` to $N-1$:
   - Swap `nums[first]` with `nums[i]`, placing element `nums[i]` into the current slot.
   - Recurse to `backtrack(first + 1)`.
   - Swap `nums[first]` with `nums[i]` back to restore the previous array state.

**Invariant Proof**:
Let $P(k)$ be the inductive hypothesis: calling `backtrack(k)` on array `nums` generates all $(N-k)!$ permutations of the subarray $\text{nums}[k \dots N-1]$, leaves $\text{nums}[0 \dots k-1]$ unchanged, and restores the original content of $\text{nums}[k \dots N-1]$ upon termination.
Base Case: For $k = N$, exactly $(N - N)! = 1$ permutation exists. The array is appended to `result` and the function returns, preserving the array.
Inductive Step: For $k < N$, any of the $N - k$ elements currently in $\text{nums}[k \dots N-1]$ can occupy position $k$.
For each $i \in [k, N-1]$, swapping $\text{nums}[k]$ and $\text{nums}[i]$ selects that element for position $k$.
By the inductive hypothesis $P(k+1)$, the recursive call produces all $(N - k - 1)!$ permutations of the remaining elements.
Because all elements in `nums` are distinct, the $N - k$ branches choose distinct elements for position $k$.
Hence, the union of all branches produces $(N - k) \times (N - k - 1)! = (N - k)!$ mutually disjoint permutations.
Swapping back restores the array state before the next branch.
Therefore, calling `backtrack(0)` generates all $N!$ distinct permutations.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \cdot N!)$. The recursion tree has $N!$ leaves, each taking $O(N)$ time to copy into the output list. The total internal nodes sum to $\sum_{k=0}^{N-1} \frac{N!}{k!} \le e \cdot N!$, so tree traversal contributes $O(N!)$ work.
- **Auxiliary Space Complexity**: $O(N)$ auxiliary space for the recursive call stack depth. No auxiliary path array or visited hash set is required.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <utility>

class Solution {
public:
    std::vector<std::vector<int>> permute(std::vector<int>& nums) {
        std::vector<std::vector<int>> result;
        backtrack(nums, 0, result);
        return result;
    }

private:
    void backtrack(std::vector<int>& nums, size_t first, std::vector<std::vector<int>>& result) {
        if (first == nums.size()) {
            result.push_back(nums);
            return;
        }

        for (size_t i = first; i < nums.size(); ++i) {
            std::swap(nums[first], nums[i]);
            backtrack(nums, first + 1, result);
            std::swap(nums[first], nums[i]);
        }
    }
};
```

#### Python 3
```python
from typing import List


class Solution:
    def permute(self, nums: List[int]) -> List[List[int]]:
        result: List[List[int]] = []

        def backtrack(first: int) -> None:
            if first == len(nums):
                result.append(list(nums))
                return

            for i in range(first, len(nums)):
                nums[first], nums[i] = nums[i], nums[first]
                backtrack(first + 1)
                nums[first], nums[i] = nums[i], nums[first]

        backtrack(0)
        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class Solution {
    public List<List<Integer>> permute(int[] nums) {
        List<List<Integer>> result = new ArrayList<>();
        List<Integer> current = new ArrayList<>();
        for (int num : nums) {
            current.add(num);
        }
        backtrack(current, 0, result);
        return result;
    }

    private void backtrack(List<Integer> current, int first, List<List<Integer>> result) {
        if (first == current.size()) {
            result.add(new ArrayList<>(current));
            return;
        }

        for (int i = first; i < current.size(); i++) {
            Collections.swap(current, first, i);
            backtrack(current, first + 1, result);
            Collections.swap(current, first, i);
        }
    }
}
```

#### TypeScript
```typescript
function permute(nums: number[]): number[][] {
    const result: number[][] = [];
    const arr = [...nums];

    function backtrack(first: number): void {
        if (first === arr.length) {
            result.push([...arr]);
            return;
        }

        for (let i = first; i < arr.length; i++) {
            [arr[first], arr[i]] = [arr[i], arr[first]];
            backtrack(first + 1);
            [arr[first], arr[i]] = [arr[i], arr[first]];
        }
    }

    backtrack(0);
    return result;
}
```

#### Go
```go
package main

func permute(nums []int) [][]int {
	var result [][]int
	arr := make([]int, len(nums))
	copy(arr, nums)

	var backtrack func(first int)
	backtrack = func(first int) {
		if first == len(arr) {
			perm := make([]int, len(arr))
			copy(perm, arr)
			result = append(result, perm)
			return
		}

		for i := first; i < len(arr); i++ {
			arr[first], arr[i] = arr[i], arr[first]
			backtrack(first + 1)
			arr[first], arr[i] = arr[i], arr[first]
		}
	}

	backtrack(0)
	return result
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn permute(mut nums: Vec<i32>) -> Vec<Vec<i32>> {
        let mut result = Vec::new();
        Self::backtrack(&mut nums, 0, &mut result);
        result
    }

    fn backtrack(nums: &mut [i32], first: usize, result: &mut Vec<Vec<i32>>) {
        if first == nums.len() {
            result.push(nums.to_vec());
            return;
        }

        for i in first..nums.len() {
            nums.swap(first, i);
            Self::backtrack(nums, first + 1, result);
            nums.swap(first, i);
        }
    }
}
```

---

## 4. Tier 2: DFS with Visited Boolean Array

### 4.1 Mechanical Description
Instead of swapping elements within `nums`, maintain a boolean array `used` of size $N$ and an auxiliary path array `path`.
At each level of recursion, iterate over all indices $i \in [0, N-1]$:
If `used[i]` is false:
Mark `used[i] = true`, append `nums[i]` to `path`, recurse, and then pop and reset `used[i] = false`.

```python
def permuteVisited(nums: list[int]) -> list[list[int]]:
    result: list[list[int]] = []
    used = [False] * len(nums)
    path: list[int] = []

    def dfs():
        if len(path) == len(nums):
            result.append(list(path))
            return
        for i in range(len(nums)):
            if not used[i]:
                used[i] = True
                path.append(nums[i])
                dfs()
                path.pop()
                used[i] = False

    dfs()
    return result
```

### 4.2 Trade-offs
- Produces permutations in lexicographical order if `nums` is sorted initially.
- Requires $O(N)$ extra memory for the boolean array and auxiliary path buffer.
- Performs $N$ iterations in the loop at every recursive call, leading to slight branch misprediction overhead.

---

## 5. Tier 3: Iterative Prefix Permutation Expansion

### 5.1 Mechanical Description
Start with a list containing one empty permutation: `result = [[]]`.
For each number `x` in `nums`:
Generate a new list of permutations by inserting `x` at every possible position $j \in [0, \text{len}(p)]$ of every existing permutation $p \in \text{result}$.
Replace `result` with the new list.

### 5.2 Trade-offs
- Completely eliminates recursion and call-stack limits.
- High memory churn and garbage collection pressure due to allocating and copying numerous intermediate lists.

---

## 6. Tier 4: Repeated Lexicographical Generation (Next Permutation)

### 6.1 Mechanical Description
Sort `nums` in ascending order to establish the lexicographically smallest permutation.
Save a copy to `result`.
Repeatedly apply the standard `next_permutation` algorithm $N! - 1$ times, saving each subsequent permutation.

### 6.2 Complexity & Deficiencies
- Each `next_permutation` step takes $O(N)$ amortized time.
- Requires inverting subarrays via two pointers.
- More complex implementation than recursive swapping while providing no asymptotic advantage.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **In-Place Mutation**: Tier 1 swaps elements directly inside the contiguous array `nums`, maximizing CPU L1 cache line hits.
2. **Result Pre-Allocation**: The total number of permutations is known in advance ($N!$). Reserving capacity `result.reserve(factorial(n))` prevents dynamic vector reallocations.
3. **Register-Only Swaps**: Swapping two primitive integers compiles to a single pair of `mov` or `xchg` instructions without memory fence overhead.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single element array | `nums = [1]` | Returns `[[1]]` | Loop executes for `i = 0`, hits base case on next call. |
| Two element array | `nums = [0, 1]` | Returns `[[0, 1], [1, 0]]` | Swaps positions 0 and 1, producing exactly 2 results. |
| Negative numbers | `nums = [-10, 5, 0]` | Preserves values | Swapping functions independently of element signs. |
| Maximum input size | $N = 6$ | Produces $6! = 720$ permutations | $720 \times 6$ integers occupies less than 20 KB of RAM. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does Tier 1 swap back after recursion?
The second swap restores the array to its original configuration so subsequent loop iterations explore valid branches without corrupted state.

### 2. Are the permutations generated in lexicographical order?
No. In-place swapping generates permutations in tree branch order, which is not strictly lexicographical.

### 3. How does this differ from Permutations II (LeetCode 47)?
Permutations II allows duplicate numbers in `nums`, requiring branch pruning (either sorting and skipping duplicate choices or using a set per level).

### 4. What is the difference between Permutations and Subsets (LeetCode 78)?
Subsets generates all combinations of all lengths ($2^N$ total). Permutations generates all reorderings of all $N$ elements ($N!$ total).

### 5. Why is $N$ constrained to $1 \le N \le 6$?
Because $N!$ grows super-exponentially. For $N = 10$, $10! = 3,628,800$, which would exceed memory limits and execution time budgets.

### 6. Can bit manipulation be used instead of a boolean visited array?
Yes. An integer bitmask where the $i$-th bit represents whether element $i$ has been used can replace the boolean array, operating in $O(1)$ auxiliary space.

### 7. How does Python's `itertools.permutations` compare?
`itertools.permutations` is implemented in C and runs faster than interpreted Python recursion, but solving with manual backtracking demonstrates core algorithmic comprehension.

### 8. Why does Rust use `nums.swap(first, i)`?
`swap` in Rust safely exchanges elements within a mutable slice without violating borrow checker rules.

### 9. Why does Go require allocating a new slice for each result?
Slices in Go are reference headers sharing underlying array storage. Without `copy`, subsequent swaps would mutate previously stored permutations.

### 10. Does Heap's algorithm generate permutations faster?
Heap's algorithm minimizes element swaps (exactly one swap per permutation), but copying the permutation into the result list still dominates at $O(N \cdot N!)$.

---

## 10. Related Problems and Systematic Progression Links

- [[0017-Letter-Combinations-of-a-Phone-Number]]: Backtracking across phone dial pad digits.
- [[0022-Generate-Parentheses]]: Backtracking with balanced parenthesis constraints.
- [[0039-Combination-Sum]]: Multiset combinations summing to target.
- [[0078-Subsets]]: Generating power sets of varying lengths.
- LeetCode 31 (Next Permutation): Generating the immediate next lexicographical permutation.
- LeetCode 47 (Permutations II): Permutations containing duplicate elements.
- LeetCode 60 (Permutation Sequence): Finding the $k$-th lexicographical permutation directly.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/permutations.cpp)
- [Python Implementation](../Python/permutations.py)
- [Java Implementation](../Java/permutations.java)
- [TypeScript Implementation](../TypeScript/permutations.ts)
- [Go Implementation](../Golang/permutations.go)
- [Rust Implementation](../Rust/permutations.rs)

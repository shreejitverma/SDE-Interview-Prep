---
id: leetcode-0078-subsets
title: "LeetCode 0078: Subsets"
tags:
  - dsa
  - leetcode
  - array
  - backtracking
  - bit-manipulation
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/subsets/"
---

# LeetCode 0078: Subsets

## 1. Problem Formalization and Constraints

Given an integer array `nums` of unique elements, return all possible subsets (the power set).
The solution set must not contain duplicate subsets.
Return the solution in any order.

### Constraints
- $1 \le \text{nums.length} \le 10$
- $-10 \le \text{nums}[i] \le 10$
- All the numbers of `nums` are unique.

### Examples
- **Example 1**:
  - Input: `nums = [1,2,3]`
  - Output: `[[],[1],[2],[1,2],[3],[1,3],[2,3],[1,2,3]]`
- **Example 2**:
  - Input: `nums = [0]`
  - Output: `[[],[0]]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Cascading Iterative Prefix Expansion | $O(N \cdot 2^N)$ | $O(1)$ auxiliary | Starts with `[[]]`; for each element, clones all existing subsets and appends the new element. |
| **Tier 2 (Bitmask)** | Binary Representation Enumeration | $O(N \cdot 2^N)$ | $O(1)$ auxiliary | Iterates integers from $0$ to $2^N - 1$; extracts element $k$ whenever the $k$-th bit is set. |
| **Tier 3 (Backtracking)** | Backtracking Depth-First Search | $O(N \cdot 2^N)$ | $O(N)$ auxiliary | Uses a recursive function with a reused path buffer, branching on start index. |
| **Tier 4 (Binary Tree)** | Recursive Include / Exclude Decision Tree | $O(N \cdot 2^N)$ | $O(N)$ auxiliary | At each index, creates two recursive calls: one omitting and one including the current element. |

---

## 3. Tier 1: Most Optimal Solution (Cascading Iterative Prefix Expansion)

### 3.1 Algorithmic Mechanics and Invariant Proof

The power set of a set $S$ with $N$ elements contains $2^N$ subsets.
We construct the power set inductively:
1. Initialize `result = [[]]` containing only the empty subset.
2. For each number $x \in \text{nums}$:
   - Let $K$ be the current size of `result`.
   - For each index $j \in [0, K-1]$:
     - Clone `result[j]`.
     - Append $x$ to the cloned subset.
     - Add the new subset to `result`.
3. Return `result`.

**Invariant Proof**:
Let $S_k = \{x_1, \dots, x_k\}$ denote the prefix of the first $k$ elements of `nums`.
The power set of $S_k$ satisfies the recursive identity:
$$\mathcal{P}(S_k) = \mathcal{P}(S_{k-1}) \cup \{A \cup \{x_k\} \mid A \in \mathcal{P}(S_{k-1})\}$$
Base Case: For $k=0$, $S_0 = \emptyset$ and $\mathcal{P}(\emptyset) = \{\emptyset\}$, which matches `result = [[]]`.
Inductive Step: Assume `result` contains all $2^{k-1}$ subsets of $S_{k-1}$.
By copying every existing subset and appending $x_k$, we generate exactly the $2^{k-1}$ subsets of $S_k$ that contain $x_k$.
Since the elements in `nums` are unique, $\mathcal{P}(S_{k-1})$ and the new subsets are mutually disjoint.
Their union contains all $2^k$ subsets of $S_k$ without duplicates or omissions.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N \cdot 2^N)$. In step $k$, $2^{k-1}$ subsets are cloned and appended with one element. The total operations across all steps sum to $\sum_{k=1}^N k \binom{N}{k} = N 2^{N-1} = O(N \cdot 2^N)$.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space excluding the output list, requiring zero call-stack frames.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<std::vector<int>> subsets(std::vector<int>& nums) {
        std::vector<std::vector<int>> result = {{}};

        for (int num : nums) {
            size_t current_size = result.size();
            for (size_t i = 0; i < current_size; ++i) {
                result.push_back(result[i]);
                result.back().push_back(num);
            }
        }

        return result;
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def subsets(self, nums: List[int]) -> List[List[int]]:
        result: List[List[int]] = [[]]

        for num in nums:
            result += [curr + [num] for curr in result]

        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

public class Solution {
    public List<List<Integer>> subsets(int[] nums) {
        List<List<Integer>> result = new ArrayList<>();
        result.add(new ArrayList<>());

        for (int num : nums) {
            int currentSize = result.size();
            for (int i = 0; i < currentSize; i++) {
                List<Integer> subset = new ArrayList<>(result.get(i));
                subset.add(num);
                result.add(subset);
            }
        }

        return result;
    }
}
```

#### TypeScript 5
```typescript
function subsets(nums: number[]): number[][] {
    const result: number[][] = [[]];

    for (const num of nums) {
        const currentSize = result.length;
        for (let i = 0; i < currentSize; i++) {
            result.push([...result[i], num]);
        }
    }

    return result;
}
```

#### Go 1.22
```go
package main

func subsets(nums []int) [][]int {
	result := [][]int{{}}

	for _, num := range nums {
		currentSize := len(result)
		for i := 0; i < currentSize; i++ {
			subset := make([]int, len(result[i])+1)
			copy(subset, result[i])
			subset[len(result[i])] = num
			result = append(result, subset)
		}
	}

	return result
}
```

#### Rust 1.75
```rust
pub struct Solution;

impl Solution {
    pub fn subsets(nums: Vec<i32>) -> Vec<Vec<i32>> {
        let mut result: Vec<Vec<i32>> = vec![vec![]];

        for &num in &nums {
            let current_size = result.len();
            for i in 0..current_size {
                let mut subset = result[i].clone();
                subset.push(num);
                result.push(subset);
            }
        }

        result
    }
}
```

---

## 4. Tier 2: Bitmask Enumeration of Power Set

### 4.1 Implementation Mechanism
Each subset corresponds to an integer bitmask $m \in [0, 2^N - 1]$.
Bit $k$ is set in $m$ if and only if element $\text{nums}[k]$ is included in the subset.

```cpp
class SolutionBitmask {
public:
    std::vector<std::vector<int>> subsets(const std::vector<int>& nums) {
        int n = nums.size();
        int total = 1 << n;
        std::vector<std::vector<int>> result;
        result.reserve(total);

        for (int mask = 0; mask < total; ++mask) {
            std::vector<int> subset;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    subset.push_back(nums[i]);
                }
            }
            result.push_back(std::move(subset));
        }

        return result;
    }
};
```

### 4.2 Trade-offs
- Maps subsets to integers directly, enabling easy parallelization.
- For each subset, inspects all $N$ bit positions, performing slightly more conditional checks than Tier 1.

---

## 5. Tier 3: Backtracking Depth-First Search with Reused Vector

### 5.1 Algorithmic Structure
A recursive function `backtrack(start)` pushes the current path into `result`, then iterates through elements from `start` to $N-1$, pushing `nums[i]`, recursing on `i + 1`, and popping `nums[i]`.

```python
class SolutionBacktracking:
    def subsets(self, nums: List[int]) -> List[List[int]]:
        result = []
        path = []

        def backtrack(start: int) -> None:
            result.append(list(path))
            for i in range(start, len(nums)):
                path.append(nums[i])
                backtrack(i + 1)
                path.pop()

        backtrack(0)
        return result
```

### 5.2 Trade-offs
- Standard pattern that generalizes cleanly to problems with sum constraints (e.g. Combination Sum) or duplicates (Subsets II).
- Incurs $O(N)$ call-stack space.

---

## 6. Tier 4: Recursive Include / Exclude Decision Tree

### 6.1 Mechanical Description
At each index $i \in [0, N-1]$, fork into two recursive calls: one that omits `nums[i]` and one that appends `nums[i]` to `path`.

### 6.2 Complexity
- **Time Complexity**: $O(N \cdot 2^N)$ across $2^{N+1} - 1$ tree nodes.
- **Space Complexity**: $O(N)$ call-stack space.
- **Verdict**: Functionally equivalent to DFS, but visits leaves only at maximum depth $N$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Pre-Allocation**: Since the size of the power set is known in advance ($2^N$), pre-allocating the vector capacity via `result.reserve(1 << n)` prevents multiple memory reallocations.
2. **Contiguous Access**: In Tier 1, cloning earlier subsets reads contiguous rows already stored in memory, providing high L1 cache hit rates.
3. **Bit Shifting**: Evaluating `1 << n` translates directly to a single CPU bitwise shift instruction.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Single Element Array | `nums = [0]` | Returns `[[], [0]]` | Loop executes once, producing 2 subsets. |
| Maximum Size Array | $N = 10$ | Returns $2^{10} = 1024$ subsets | $1024 \times 10$ integers fit within 40 KB of memory. |
| Negative Integers | `nums = [-5, -2, 1]` | Preserves negative values | Value signs do not affect subset membership. |
| Empty Input (Theoretical) | `nums = []` | Returns `[[]]` | Initialization `result = {{}}` handles empty input gracefully. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does the power set of a set with $N$ elements have $2^N$ elements?
Each element has two independent binary choices: it is either included or excluded. For $N$ independent choices, the total count is $2 \times 2 \times \dots \times 2 = 2^N$.

### 2. Can subsets be returned in any order?
Yes. The problem statement explicitly permits output subsets in any order.

### 3. What is the difference between Subsets and Subsets II (LeetCode 90)?
In LeetCode 90, `nums` can contain duplicate values, requiring array sorting and duplicate skip logic (`if (i > start && nums[i] == nums[i-1]) continue`).

### 4. Why is Tier 1 faster than Bitmasking in practice?
Tier 1 only appends the newest element to existing subsets, performing $\sum_{k=1}^N 2^{k-1} = 2^N - 1$ copy operations. Bitmasking tests $N$ bits for all $2^N$ masks, performing $N 2^N$ bit checks.

### 5. Why does Python's `result += [curr + [num] for curr in result]` work cleanly?
List comprehensions in Python create a new list for each subset in C, running at native interpreter speeds.

### 6. Can $N$ exceed 30 in this problem?
No. For $N = 31$, $2^{31}$ subsets would require over 2 billion vectors (tens of gigabytes of RAM), exceeding memory and time limits on modern computers.

### 7. How does this compare with generating Permutations (LeetCode 46)?
Subsets focuses on combinations of varying lengths without regard to ordering ($2^N$ total). Permutations focuses on different orderings of all $N$ elements ($N!$ total).

### 8. What is the space overhead of cloning slices in Go?
In Go, `copy(subset, result[i])` allocates a distinct underlying backing array for each slice, preventing unintended mutations between subsets.

### 9. Why does Rust require `&mut Vec<Vec<i32>>` in backtracking?
Rust requires mutable references to modify collections in place, preventing data races and memory corruption.

### 10. Can Gray Code order subsets such that adjacent subsets differ by only one element?
Yes. Iterating bitmasks using the Gray code formula $g = i \oplus (i \gg 1)$ generates the power set where each subset differs from its predecessor by exactly one element.

---

## 10. Related Problems and Systematic Progression Links

- [[0017-Letter-Combinations-of-a-Phone-Number]]: Cartesian product combinatorial expansion.
- [[0022-Generate-Parentheses]]: Constrained search space generation.
- LeetCode 39 (Combination Sum): Subsets with repeated candidate elements summing to target value.
- LeetCode 46 (Permutations): Generating full-length element permutations.
- LeetCode 77 (Combinations): Subsets restricted to fixed size $k$.
- LeetCode 90 (Subsets II): Generating unique subsets from arrays containing duplicate numbers.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/subsets.cpp)
- [Python Implementation](../Python/subsets.py)
- [Java Implementation](../Java/subsets.java)
- [TypeScript Implementation](../TypeScript/subsets.ts)
- [Go Implementation](../Golang/subsets.go)
- [Rust Implementation](../Rust/subsets.rs)

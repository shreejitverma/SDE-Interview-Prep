---
id: leetcode-0039-combination-sum
title: "LeetCode 0039: Combination Sum"
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
  - "https://leetcode.com/problems/combination-sum/"
---

# LeetCode 0039: Combination Sum

## 1. Problem Formalization and Constraints

Given an array of distinct integers `candidates` and a target integer `target`, return a list of all unique combinations of `candidates` where the chosen numbers sum to `target`.
You may return the combinations in any order.
The same number may be chosen from `candidates` an unlimited number of times.
Two combinations are unique if the frequency of at least one of the chosen numbers is different.
The test cases are generated such that the number of unique combinations that sum up to `target` is less than 150 combinations for the given input.

### Constraints
- $1 \le \text{candidates.length} \le 30$
- $2 \le \text{candidates}[i] \le 40$
- All elements of `candidates` are distinct.
- $1 \le \text{target} \le 40$

### Examples
- **Example 1**:
  - Input: `candidates = [2,3,6,7]`, `target = 7`
  - Output: `[[2,2,3],[7]]`
  - Explanation: 2 and 3 are candidates, and $2 + 2 + 3 = 7$. Note that 2 can be used multiple times. 7 is a candidate, and $7 = 7$.
- **Example 2**:
  - Input: `candidates = [2,3,5]`, `target = 8`
  - Output: `[[2,2,2,2],[2,3,3],[3,5]]`
- **Example 3**:
  - Input: `candidates = [2]`, `target = 1`
  - Output: `[]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Sorted Backtracking with Early Pruning | $O(N^{T/M + 1})$ | $O(T/M)$ auxiliary | Sorts candidates upfront; breaks inner branch loop as soon as `candidate > remaining_target`. |
| **Tier 2 (DP Tabulation)** | Unbounded Knapsack DP Table | $O(T \cdot N \cdot K)$ | $O(T \cdot K)$ auxiliary | Computes all combinations summing to each sub-target $s \in [1, T]$ iteratively. |
| **Tier 3 (Unsorted DFS)** | Unpruned Depth-First Search | $O(N^{T/M + 1})$ | $O(T/M)$ auxiliary | Explores all branches without sorting; checks condition at leaf nodes, generating dead search paths. |
| **Tier 4 (Brute Force)** | Multi-Set Permutation Filtering | $O(N^{T/M} \cdot (T/M)!)$ | $O(T/M)$ auxiliary | Generates all permutations that sum to target and dedupes using a set structure. |

*Notation*: $N$ is the length of `candidates`, $T$ is `target`, and $M = \min(\text{candidates})$. The maximum recursion depth is $T / M$.

---

## 3. Tier 1: Most Optimal Solution (Sorted Backtracking with Early Pruning)

### 3.1 Algorithmic Mechanics and Invariant Proof

To avoid duplicate combinations in output (e.g., `[2, 3, 2]` vs `[2, 2, 3]`), enforce a monotonic index order during exploration.
At any recursion node with remaining target `remain` and index pointer `start`:
1. Candidate elements at index $i \ge \text{start}$ may be chosen.
2. Because candidates are sorted in ascending order, if $\text{candidates}[i] > \text{remain}$, then every subsequent element $\text{candidates}[j]$ ($j > i$) also satisfies $\text{candidates}[j] > \text{remain}$.
3. We immediately break out of the loop, pruning entire subtrees of invalid sums.
4. When an element $\text{candidates}[i] \le \text{remain}$ is chosen, we recurse with `start = i` (allowing reuse of the same element) and `remain = remain - candidates[i]`.
5. Upon returning, we backtrack by popping the chosen element.

**Invariant Proof**:
Let $C = [c_0, c_1, \dots, c_{N-1}]$ be sorted in strictly ascending order ($c_0 < c_1 < \dots < c_{N-1}$).
Every valid multiset combination $\{c_{i_1}, c_{i_2}, \dots, c_{i_k}\}$ with $\sum c_{i_j} = T$ can be written uniquely in non-decreasing index order $i_1 \le i_2 \le \dots \le i_k$.
At depth $d$, choosing $i_{d} \ge i_{d-1}$ guarantees that each unique multiset corresponds to exactly one path in the search tree.
Furthermore, because $c_i > 0$, the remaining target strictly decreases at each step ($T - c_i < T$).
Hence the search tree has finite maximum depth $\lfloor T / c_0 \rfloor \le 40 / 2 = 20$.
Early termination on $c_i > \text{remain}$ is safe because all $c_j \ge c_i > \text{remain}$ for $j \ge i$, so no valid combinations can exist in those branches.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N^{T/M + 1})$ upper bound, where $N$ is the number of candidates, $T$ is target, and $M = \min(\text{candidates})$. With $M \ge 2$ and $T \le 40$, the maximum tree depth is 20. Pruning reduces the evaluated state space dramatically, well within milliseconds.
- **Auxiliary Space Complexity**: $O(T / M)$ auxiliary space consumed by the recursive call stack and the current path buffer, excluding the storage required for the output combinations.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    std::vector<std::vector<int>> combinationSum(std::vector<int>& candidates, int target) {
        std::sort(candidates.begin(), candidates.end());
        std::vector<std::vector<int>> result;
        std::vector<int> current;
        backtrack(candidates, target, 0, current, result);
        return result;
    }

private:
    void backtrack(const std::vector<int>& candidates, int remain, size_t start,
                   std::vector<int>& current, std::vector<std::vector<int>>& result) {
        if (remain == 0) {
            result.push_back(current);
            return;
        }

        for (size_t i = start; i < candidates.size(); ++i) {
            if (candidates[i] > remain) {
                break;
            }
            current.push_back(candidates[i]);
            backtrack(candidates, remain - candidates[i], i, current, result);
            current.pop_back();
        }
    }
};
```

#### Python 3
```python
from typing import List


class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        candidates.sort()
        result: List[List[int]] = []
        path: List[int] = []

        def backtrack(remain: int, start: int) -> None:
            if remain == 0:
                result.append(list(path))
                return

            for i in range(start, len(candidates)):
                if candidates[i] > remain:
                    break
                path.append(candidates[i])
                backtrack(remain - candidates[i], i)
                path.pop()

        backtrack(target, 0)
        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {
    public List<List<Integer>> combinationSum(int[] candidates, int target) {
        Arrays.sort(candidates);
        List<List<Integer>> result = new ArrayList<>();
        List<Integer> current = new ArrayList<>();
        backtrack(candidates, target, 0, current, result);
        return result;
    }

    private void backtrack(int[] candidates, int remain, int start,
                           List<Integer> current, List<List<Integer>> result) {
        if (remain == 0) {
            result.add(new ArrayList<>(current));
            return;
        }

        for (int i = start; i < candidates.length; i++) {
            if (candidates[i] > remain) {
                break;
            }
            current.add(candidates[i]);
            backtrack(candidates, remain - candidates[i], i, current, result);
            current.remove(current.size() - 1);
        }
    }
}
```

#### TypeScript
```typescript
function combinationSum(candidates: number[], target: number): number[][] {
    candidates.sort((a, b) => a - b);
    const result: number[][] = [];
    const path: number[] = [];

    function backtrack(remain: number, start: number): void {
        if (remain === 0) {
            result.push([...path]);
            return;
        }

        for (let i = start; i < candidates.length; i++) {
            if (candidates[i] > remain) {
                break;
            }
            path.push(candidates[i]);
            backtrack(remain - candidates[i], i);
            path.pop();
        }
    }

    backtrack(target, 0);
    return result;
}
```

#### Go
```go
package main

import "sort"

func combinationSum(candidates []int, target int) [][]int {
	sort.Ints(candidates)
	var result [][]int
	var path []int

	var backtrack func(remain int, start int)
	backtrack = func(remain int, start int) {
		if remain == 0 {
			comb := make([]int, len(path))
			copy(comb, path)
			result = append(result, comb)
			return
		}

		for i := start; i < len(candidates); i++ {
			if candidates[i] > remain {
				break
			}
			path = append(path, candidates[i])
			backtrack(remain-candidates[i], i)
			path = path[:len(path)-1]
		}
	}

	backtrack(target, 0)
	return result
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn combination_sum(mut candidates: Vec<i32>, target: i32) -> Vec<Vec<i32>> {
        candidates.sort_unstable();
        let mut result = Vec::new();
        let mut path = Vec::new();
        Self::backtrack(&candidates, target, 0, &mut path, &mut result);
        result
    }

    fn backtrack(
        candidates: &[i32],
        remain: i32,
        start: usize,
        path: &mut Vec<i32>,
        result: &mut Vec<Vec<i32>>,
    ) {
        if remain == 0 {
            result.push(path.clone());
            return;
        }

        for i in start..candidates.len() {
            if candidates[i] > remain {
                break;
            }
            path.push(candidates[i]);
            Self::backtrack(candidates, remain - candidates[i], i, path, result);
            path.pop();
        }
    }
}
```

---

## 4. Tier 2: Space-Optimized / Tabulation Alternative (Unbounded Knapsack DP)

### 4.1 Mechanical Description
Instead of recursion, maintain an array `dp` where `dp[s]` stores the list of all valid combinations summing to $s \in [0, T]$.
Initialize `dp[0] = [[]]`.
To enforce uniqueness, iterate through each candidate $c \in \text{candidates}$ in an outer loop:
For each sub-target $s$ from $c$ up to $T$:
For each combination `prev` in `dp[s - c]`:
Append `prev + [c]` to `dp[s]`.
Return `dp[target]`.

```python
def combinationSumDP(candidates: list[int], target: int) -> list[list[int]]:
    dp: list[list[list[int]]] = [[] for _ in range(target + 1)]
    dp[0] = [[]]

    for c in candidates:
        for s in range(c, target + 1):
            for prev in dp[s - c]:
                dp[s].append(prev + [c])

    return dp[target]
```

### 4.2 Trade-offs
- Eliminates function call overhead and recursion limits.
- Requires significantly higher auxiliary space to materialize all sub-target combinations simultaneously ($O(T \cdot K)$ memory).
- Slower than Tier 1 because it stores intermediate lists rather than reusing a single backtracking buffer.

---

## 5. Tier 3: Unpruned Depth-First Search

### 5.1 Mechanical Description
Maintain the same recursive exploration, but do not sort the `candidates` array upfront.
Instead of breaking early when `candidates[i] > remain`, execute recursive calls and check `if (remain < 0) return;` at the entry of the function.

### 5.2 Trade-offs
- Avoids the initial $O(N \log N)$ sorting step.
- Explores all candidate branches even when the remaining target is tiny, causing substantial overhead when candidates contain large numbers.

---

## 6. Tier 4: Multi-Set Permutation Filtering (Brute Force Baseline)

### 6.1 Mechanical Description
Generate all ordered sequences of candidate selections that sum to `target`.
Store each valid sequence in a sorted canonical form inside a hash set to eliminate permutations representing the same combination.

### 6.2 Complexity & Deficiencies
- Explores $O(K!)$ permutations per combination of length $K$.
- Causes prohibitive memory consumption and redundant work, failing under strict interview time constraints.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Reused Vector Buffer**: Backtracking uses a single vector `current` that is pushed and popped in place. This guarantees high L1 data cache locality without repeated heap allocations during traversal.
2. **Result Vector Sizing**: Allocating copies only upon reaching `remain == 0` ensures heap allocations occur exclusively for verified valid solutions.
3. **Sorting Overhead vs Savings**: Sorting $N \le 30$ integers takes under 100 nanoseconds and saves millions of CPU instructions by enabling branch pruning (`break`).

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Target smaller than all candidates | `candidates = [5, 10]`, `target = 3` | Returns `[]` | Outer loop breaks immediately on index 0 because `5 > 3`. |
| Single candidate divides target | `candidates = [2]`, `target = 8` | Returns `[[2,2,2,2]]` | Branches $T / c = 4$ times along index 0. |
| Target equals candidate | `candidates = [7]`, `target = 7` | Returns `[[7]]` | Produces single combination immediately. |
| Prime candidate combinations | `candidates = [2, 3, 5]`, `target = 8` | Returns `[[2,2,2,2],[2,3,3],[3,5]]` | Monotonic index progression prevents permutations like `[5, 3]`. |
| Maximum target with minimum candidate | `candidates = [2]`, `target = 40` | Returns 20 twos `[[2, ... , 2]]` | Recursion depth capped at 20 frames, preventing stack overflow. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why does passing `i` instead of `i + 1` allow repeated elements?
Passing `start = i` allows the next recursion level to select the same candidate again, enabling unbounded multiset generation.

### 2. How is duplicate permutation avoided without a hash set?
By restricting choices at depth $d$ to indices $i \ge \text{start}$, candidates are selected in non-decreasing order of index. This creates a canonical representation for every combination.

### 3. What is the difference between Combination Sum and Combination Sum II (LeetCode 40)?
In LeetCode 40, each candidate may only be used once, and `candidates` may contain duplicates, requiring duplicate branch skipping (`if (i > start && candidates[i] == candidates[i-1]) continue`).

### 4. Why is sorting the array crucial for Tier 1?
Sorting allows the condition `candidates[i] > remain` to terminate the loop with `break` instead of merely `continue`, cutting off the entire remaining loop for larger candidates.

### 5. What is the maximum possible recursion depth?
The minimum candidate value is 2 and the maximum target is 40. Therefore, the maximum call stack depth is $\lfloor 40 / 2 \rfloor = 20$.

### 6. Can dynamic programming be more efficient than backtracking here?
No. Because we must output every combination explicitly, and the number of combinations is small ($\le 150$), backtracking with in-place buffer reuse is faster and uses far less memory than DP.

### 7. How does Go handle slice mutations during backtracking?
In Go, `path = path[:len(path)-1]` truncates the slice header without reallocating, while `copy` allocates an independent slice when adding to `result`.

### 8. Why does Rust use `Vec::clone` when pushing to `result`?
Rust's ownership model requires cloning the `path` buffer into the output vector so that `path` can continue to be mutated across backtracking steps.

### 9. Could this problem have negative candidates?
If negative numbers or zero were permitted, candidates could sum to zero in infinite loops, requiring cycle detection and transforming the problem into an unbounded graph cycle search.

### 10. How does this problem relate to Coin Change (LeetCode 322)?
Coin Change seeks the minimum number of coins (an optimization problem best solved by DP in $O(N \cdot T)$), whereas Combination Sum seeks all valid combinations (an enumeration problem solved by backtracking).

---

## 10. Related Problems and Systematic Progression Links

- [[0017-Letter-Combinations-of-a-Phone-Number]]: Cartesian product combinatorial expansion.
- [[0022-Generate-Parentheses]]: Constrained search space backtracking.
- [[0078-Subsets]]: Generating power sets without target sum constraints.
- [[0322-Coin-Change]]: Minimum coins required to form target sum via dynamic programming.
- LeetCode 40 (Combination Sum II): Combination sum with bounded candidate frequencies.
- LeetCode 216 (Combination Sum III): Combination sum restricted to $k$ digits from 1 to 9.
- LeetCode 377 (Combination Sum IV): Counting ordered permutations that sum to target.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/combination-sum.cpp)
- [Python Implementation](../Python/combination-sum.py)
- [Java Implementation](../Java/combination-sum.java)
- [TypeScript Implementation](../TypeScript/combination-sum.ts)
- [Go Implementation](../Golang/combination-sum.go)
- [Rust Implementation](../Rust/combination-sum.rs)

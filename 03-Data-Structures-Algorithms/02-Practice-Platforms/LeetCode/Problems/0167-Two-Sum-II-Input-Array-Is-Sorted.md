---
id: leetcode-0167-two-sum-ii-input-array-is-sorted
title: "LeetCode 0167: Two Sum II - Input Array Is Sorted"
tags:
  - dsa
  - leetcode
  - array
  - two-pointers
  - binary-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/"
---

# LeetCode 0167: Two Sum II - Input Array Is Sorted

## 1. Problem Formalization and Constraints

Given a 1-indexed array of integers `numbers` that is already sorted in non-decreasing order, find two numbers such that they add up to a specific `target` number.
Let these two numbers be `numbers[index1]` and `numbers[index2]` where $1 \le \text{index1} < \text{index2} \le \text{numbers.length}$.
Return the indices of the two numbers, `index1` and `index2`, added by one as an integer array `[index1, index2]` of length 2.
The tests are generated such that there is exactly one solution.
You may not use the same element twice.
Your solution must use only constant extra space.

### Constraints
- $2 \le \text{numbers.length} \le 3 \cdot 10^4$
- $-1000 \le \text{numbers}[i] \le 1000$
- `numbers` is sorted in non-decreasing order.
- $-1000 \le \text{target} \le 1000$
- The tests are generated such that there is exactly one solution.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Two Pointers Converging Inward | $O(N)$ | $O(1)$ | Exploits sorted order monotonic sum invariant; strictly constant space. |
| **Tier 2 (Alternative)** | Binary Search for Complement | $O(N \log N)$ | $O(1)$ | Searches remainder `target - numbers[i]` on suffix $[i+1, N-1]$. |
| **Tier 3 (Hash Map)** | Hash Table Complements | $O(N)$ | $O(N)$ | Linear time complement lookup, but violates $O(1)$ space requirement. |
| **Tier 4 (Brute Force)** | Exhaustive Pairwise Enumeration | $O(N^2)$ | $O(1)$ | Tests every $(i, j)$ pair until sum matches; quadratic latency. |

---

## 3. Tier 1: Most Optimal Solution (Two Pointers Converging Inward)

### 3.1 Algorithmic Mechanics and Invariant Proof

Because the array is monotonically non-decreasing, the sum $S(l, r) = \text{numbers}[l] + \text{numbers}[r]$ exhibits strict monotonicity:
1. If $S(l, r) < \text{target}$, then for any $k \le r$, $\text{numbers}[l] + \text{numbers}[k] \le S(l, r) < \text{target}$.
Therefore, index $l$ cannot possibly pair with any remaining candidate index, and $l$ must be incremented.
2. If $S(l, r) > \text{target}$, then for any $k \ge l$, $\text{numbers}[k] + \text{numbers}[r] \ge S(l, r) > \text{target}$.
Therefore, index $r$ cannot possibly pair with any remaining candidate index, and $r$ must be decremented.
3. If $S(l, r) == \text{target}$, the unique solution is found, and 1-based indices $[l + 1, r + 1]$ are returned immediately.
Each iteration discards at least one candidate index, guaranteeing termination in at most $N$ steps.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& numbers, int target) {
        int left = 0;
        int right = static_cast<int>(numbers.size()) - 1;

        while (left < right) {
            const int sum = numbers[left] + numbers[right];
            if (sum == target) {
                return {left + 1, right + 1};
            } else if (sum < target) {
                ++left;
            } else {
                --right;
            }
        }
        return {-1, -1};
    }
};
```

#### Python
```python
class Solution:
    def twoSum(self, numbers: list[int], target: int) -> list[int]:
        left, right = 0, len(numbers) - 1

        while left < right:
            current_sum = numbers[left] + numbers[right]
            if current_sum == target:
                return [left + 1, right + 1]
            elif current_sum < target:
                left += 1
            else:
                right -= 1

        return [-1, -1]
```

#### Java
```java
class Solution {
    public int[] twoSum(int[] numbers, int target) {
        int left = 0;
        int right = numbers.length - 1;

        while (left < right) {
            int sum = numbers[left] + numbers[right];
            if (sum == target) {
                return new int[]{left + 1, right + 1};
            } else if (sum < target) {
                left++;
            } else {
                right--;
            }
        }
        return new int[]{-1, -1};
    }
}
```

#### TypeScript
```typescript
function twoSum(numbers: number[], target: number): number[] {
    let left = 0;
    let right = numbers.length - 1;

    while (left < right) {
        const sum = numbers[left] + numbers[right];
        if (sum === target) {
            return [left + 1, right + 1];
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }
    return [-1, -1];
}
```

#### Golang
```go
package leetcode

func twoSum(numbers []int, target int) []int {
	left := 0
	right := len(numbers) - 1

	for left < right {
		sum := numbers[left] + numbers[right]
		if sum == target {
			return []int{left + 1, right + 1}
		} else if sum < target {
			left++
		} else {
			right--
		}
	}
	return []int{-1, -1}
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn two_sum(numbers: Vec<i32>, target: i32) -> Vec<i32> {
        let mut left: usize = 0;
        let mut right: usize = numbers.len() - 1;

        while left < right {
            let sum = numbers[left] + numbers[right];
            if sum == target {
                return vec![(left + 1) as i32, (right + 1) as i32];
            } else if sum < target {
                left += 1;
            } else {
                right -= 1;
            }
        }
        vec![-1, -1]
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: Strictly $O(N)$ as each step reduces the interval length $r - l$ by at least 1.
- **Space Complexity**: Strictly $O(1)$ auxiliary storage using only two index pointer variables.
- **Cache Locality**: Accesses begin at array boundaries and proceed inwards; while not strictly sequential, no auxiliary dynamic allocation is incurred.

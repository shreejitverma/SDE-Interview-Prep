---
id: leetcode-0074-search-a-2d-matrix
title: "LeetCode 0074: Search a 2D Matrix"
tags:
  - dsa
  - leetcode
  - array
  - binary-search
  - matrix
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/search-a-2d-matrix/"
---

# LeetCode 0074: Search a 2D Matrix

## 1. Problem Formalization and Constraints

You are given an $m \times n$ integer matrix `matrix` with the following two properties:
1. Each row is sorted in non-decreasing order.
2. The first integer of each row is greater than the last integer of the previous row.
Given an integer `target`, return `true` if `target` is in `matrix` or `false` otherwise.
You must write a solution in $O(\log(m \cdot n))$ time complexity.

### Constraints
- $m == \text{matrix.length}$
- $n == \text{matrix}[i]\text{.length}$
- $1 \le m, n \le 100$
- $-10^4 \le \text{matrix}[i][j], \text{target} \le 10^4$

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Flattened 1D Binary Search | $O(\log(M \cdot N))$ | $O(1)$ | Treats $M \times N$ matrix as virtual 1D sorted array via division and modulo. |
| **Tier 2 (Two-Phase Search)** | Row Binary Search + Column Binary Search | $O(\log M + \log N)$ | $O(1)$ | Locates target candidate row first, then binary searches within that row. |
| **Tier 3 (Step-wise Search)** | Top-Right / Bottom-Left Search | $O(M + N)$ | $O(1)$ | Eliminates one row or column per step; optimal for LeetCode 240. |
| **Tier 4 (Brute Force)** | Exhaustive Matrix Scan | $O(M \cdot N)$ | $O(1)$ | Scans all elements linearly without exploiting sorted properties. |

---

## 3. Tier 1: Most Optimal Solution (Flattened 1D Binary Search)

### 3.1 Algorithmic Mechanics and Invariant Proof

Because each row is internally sorted and the first element of row $i+1$ exceeds the last element of row $i$, the entire matrix forms a single monotonically increasing sequence of length $M \cdot N$.
For any index $k \in [0, M \cdot N - 1]$, its corresponding matrix element is located at:
- $\text{row} = \lfloor k / N \rfloor$
- $\text{col} = k \pmod N$
Since this coordinate mapping is strictly bijective and order-preserving, standard binary search over the closed range $[0, M \cdot N - 1]$ guarantees convergence to the target in $\lceil \log_2(M \cdot N) \rceil$ steps.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
        if (matrix.empty() || matrix[0].empty()) return false;
        const int m = static_cast<int>(matrix.size());
        const int n = static_cast<int>(matrix[0].size());
        int left = 0, right = m * n - 1;

        while (left <= right) {
            const int mid = left + (right - left) / 2;
            const int val = matrix[mid / n][mid % n];
            if (val == target) {
                return true;
            } else if (val < target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return false;
    }
};
```

#### Python
```python
class Solution:
    def searchMatrix(self, matrix: list[list[int]], target: int) -> bool:
        if not matrix or not matrix[0]:
            return False

        m, n = len(matrix), len(matrix[0])
        left, right = 0, m * n - 1

        while left <= right:
            mid = (left + right) // 2
            val = matrix[mid // n][mid % n]
            if val == target:
                return True
            elif val < target:
                left = mid + 1
            else:
                right = mid - 1

        return False
```

#### Java
```java
class Solution {
    public boolean searchMatrix(int[][] matrix, int target) {
        if (matrix == null || matrix.length == 0 || matrix[0].length == 0) {
            return false;
        }

        int m = matrix.length;
        int n = matrix[0].length;
        int left = 0;
        int right = m * n - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            int val = matrix[mid / n][mid % n];
            if (val == target) {
                return true;
            } else if (val < target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return false;
    }
}
```

#### TypeScript
```typescript
function searchMatrix(matrix: number[][], target: number): boolean {
    if (matrix.length === 0 || matrix[0].length === 0) {
        return false;
    }

    const m = matrix.length;
    const n = matrix[0].length;
    let left = 0;
    let right = m * n - 1;

    while (left <= right) {
        const mid = Math.floor(left + (right - left) / 2);
        const val = matrix[Math.floor(mid / n)][mid % n];
        if (val === target) {
            return true;
        } else if (val < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return false;
}
```

#### Golang
```go
package leetcode

func searchMatrix(matrix [][]int, target int) bool {
	if len(matrix) == 0 || len(matrix[0]) == 0 {
		return false
	}

	m := len(matrix)
	n := len(matrix[0])
	left := 0
	right := m*n - 1

	for left <= right {
		mid := left + (right-left)/2
		val := matrix[mid/n][mid%n]
		if val == target {
			return true
		} else if val < target {
			left = mid + 1
		} else {
			right = mid - 1
		}
	}
	return false
}
```

#### Rust
```rust
pub struct Solution;

impl Solution {
    pub fn search_matrix(matrix: Vec<Vec<i32>>, target: i32) -> bool {
        if matrix.is_empty() || matrix[0].is_empty() {
            return false;
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut left: i32 = 0;
        let mut right: i32 = (m * n - 1) as i32;

        while left <= right {
            let mid = left + (right - left) / 2;
            let val = matrix[(mid as usize) / n][(mid as usize) % n];
            if val == target {
                return true;
            } else if val < target {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        false
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(\log(M \cdot N)) = O(\log M + \log N)$, performing standard logarithmic bisection.
- **Space Complexity**: Strictly $O(1)$ auxiliary storage.
- **Cache Efficiency**: Accesses jump across rows during early bisection phases, but localize rapidly as the search interval shrinks.

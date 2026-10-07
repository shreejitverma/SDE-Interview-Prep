---
id: leetcode-0073-set-matrix-zeroes
title: "LeetCode 0073: Set Matrix Zeroes"
tags:
  - dsa
  - leetcode
  - matrix
  - array
  - in-place
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/set-matrix-zeroes/"
---

# LeetCode 0073: Set Matrix Zeroes

## 1. Problem Formalization and Constraints

Given an $m \times n$ integer matrix `matrix`, if an element is $0$, set its entire row and column to $0$'s.
You must do it in place without allocating a second matrix.

### Constraints
- $m == \text{matrix.length}$
- $n == \text{matrix}[0]\text{.length}$
- $1 \le m, n \le 200$
- $-2^{31} \le \text{matrix}[i][j] \le 2^{31} - 1$

### Examples
- **Example 1**:
  - Input: `matrix = [[1,1,1],[1,0,1],[1,1,1]]`
  - Output: `[[1,0,1],[0,0,0],[1,0,1]]`
- **Example 2**:
  - Input: `matrix = [[0,1,2,0],[3,4,5,2],[1,3,1,5]]`
  - Output: `[[0,0,0,0],[0,4,5,0],[0,3,1,0]]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | In-Place First Row & First Column Markers | $O(M \times N)$ | $O(1)$ auxiliary | Embeds zero-state flags directly into row 0 and column 0, using a single boolean variable for column 0. |
| **Tier 2 (Linear Extra Space)** | Auxiliary 1D Row & Column Bit Vectors | $O(M \times N)$ | $O(M + N)$ | Tracks zero occurrences using two external boolean or bit arrays of lengths $M$ and $N$. |
| **Tier 3 (Hash Sets)** | Coordinate Tracking with Hash Sets | $O(M \times N)$ | $O(M + N)$ | Collects indices of zero rows and columns in hash sets before applying updates in a second pass. |
| **Tier 4 (Brute Force)** | Matrix Clone Snapshot Comparison | $O(M \times N)$ | $O(M \times N)$ | Allocates an identical copy of the matrix to read original zeros and overwrite the target matrix. |

---

## 3. Tier 1: Most Optimal Solution (In-Place Row & Column Markers)

### 3.1 Algorithmic Mechanics and Invariant Proof

Instead of allocating $M + N$ additional memory to remember which rows and columns should be zeroed, we repurpose the first row `matrix[0][..]` and first column `matrix[..][0]` as marker arrays:
1. Since `matrix[0][0]` is at the intersection of the first row and first column, it can only serve as the marker for the first row. We introduce a single boolean variable `firstColZero` to track whether the first column initially contains any zero.
2. **First Pass (Marking)**:
   - For each row $i \in [0, M-1]$:
     - If `matrix[i][0] == 0`, set `firstColZero = true`.
     - For each column $j \in [1, N-1]$:
       - If `matrix[i][j] == 0`, mark `matrix[i][0] = 0` and `matrix[0][j] = 0`.
3. **Second Pass (Zeroing Inner Cells)**:
   - Iterate backwards or over inner indices $i \in [1, M-1]$ and $j \in [1, N-1]$:
     - If `matrix[i][0] == 0` or `matrix[0][j] == 0`, set `matrix[i][j] = 0`.
4. **Third Pass (Zeroing Boundary Markers)**:
   - If `matrix[0][0] == 0`, zero out the entire first row: `matrix[0][j] = 0` for all $j \in [0, N-1]$.
   - If `firstColZero == true`, zero out the entire first column: `matrix[i][0] = 0` for all $i \in [0, M-1]$.

**Invariant Proof**:
The order of mutation guarantees correctness without information loss:
1. During the first pass, inner cells $(i, j)$ with $i \ge 1, j \ge 1$ only write to the boundaries `matrix[i][0]` and `matrix[0][j]`.
2. Because the boundaries are inspected during the second pass only after all zero locations have been recorded, existing zeros in the boundaries correctly represent projections of zero cells.
3. Updating inner cells first prevents marker flags in `matrix[0][j]` and `matrix[i][0]` from being overwritten prematurely.
4. Finally, updating row 0 and column 0 last ensures that boundary cells are set to zero only after all inner dependencies have finished reading them.
Thus, exactly the required rows and columns are zeroed with strictly $O(1)$ auxiliary space.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$, where $M$ is the number of rows and $N$ is the number of columns. The matrix is scanned twice, touching each cell a constant number of times.
- **Auxiliary Space Complexity**: $O(1)$ auxiliary space. The markers reside entirely inside existing matrix cells, requiring only a single boolean flag `firstColZero`.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    void setZeroes(std::vector<std::vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) {
            return;
        }

        int m = static_cast<int>(matrix.size());
        int n = static_cast<int>(matrix[0].size());
        bool firstColZero = false;

        for (int i = 0; i < m; ++i) {
            if (matrix[i][0] == 0) {
                firstColZero = true;
            }
            for (int j = 1; j < n; ++j) {
                if (matrix[i][j] == 0) {
                    matrix[i][0] = 0;
                    matrix[0][j] = 0;
                }
            }
        }

        for (int i = 1; i < m; ++i) {
            for (int j = 1; j < n; ++j) {
                if (matrix[i][0] == 0 || matrix[0][j] == 0) {
                    matrix[i][j] = 0;
                }
            }
        }

        if (matrix[0][0] == 0) {
            for (int j = 0; j < n; ++j) {
                matrix[0][j] = 0;
            }
        }

        if (firstColZero) {
            for (int i = 0; i < m; ++i) {
                matrix[i][0] = 0;
            }
        }
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def setZeroes(self, matrix: List[List[int]]) -> None:
        if not matrix or not matrix[0]:
            return

        m, n = len(matrix), len(matrix[0])
        first_col_zero = False

        for i in range(m):
            if matrix[i][0] == 0:
                first_col_zero = True
            for j in range(1, n):
                if matrix[i][j] == 0:
                    matrix[i][0] = 0
                    matrix[0][j] = 0

        for i in range(1, m):
            for j in range(1, n):
                if matrix[i][0] == 0 or matrix[0][j] == 0:
                    matrix[i][j] = 0

        if matrix[0][0] == 0:
            for j in range(n):
                matrix[0][j] = 0

        if first_col_zero:
            for i in range(m):
                matrix[i][0] = 0
```

#### Java 21
```java
public class Solution {
    public void setZeroes(int[][] matrix) {
        if (matrix == null || matrix.length == 0 || matrix[0].length == 0) {
            return;
        }

        int m = matrix.length;
        int n = matrix[0].length;
        boolean firstColZero = false;

        for (int i = 0; i < m; i++) {
            if (matrix[i][0] == 0) {
                firstColZero = true;
            }
            for (int j = 1; j < n; j++) {
                if (matrix[i][j] == 0) {
                    matrix[i][0] = 0;
                    matrix[0][j] = 0;
                }
            }
        }

        for (int i = 1; i < m; i++) {
            for (int j = 1; j < n; j++) {
                if (matrix[i][0] == 0 || matrix[0][j] == 0) {
                    matrix[i][j] = 0;
                }
            }
        }

        if (matrix[0][0] == 0) {
            for (int j = 0; j < n; j++) {
                matrix[0][j] = 0;
            }
        }

        if (firstColZero) {
            for (int i = 0; i < m; i++) {
                matrix[i][0] = 0;
            }
        }
    }
}
```

#### TypeScript 5
```typescript
function setZeroes(matrix: number[][]): void {
    if (!matrix || matrix.length === 0 || matrix[0].length === 0) {
        return;
    }

    const m = matrix.length;
    const n = matrix[0].length;
    let firstColZero = false;

    for (let i = 0; i < m; i++) {
        if (matrix[i][0] === 0) {
            firstColZero = true;
        }
        for (let j = 1; j < n; j++) {
            if (matrix[i][j] === 0) {
                matrix[i][0] = 0;
                matrix[0][j] = 0;
            }
        }
    }

    for (let i = 1; i < m; i++) {
        for (let j = 1; j < n; j++) {
            if (matrix[i][0] === 0 || matrix[0][j] === 0) {
                matrix[i][j] = 0;
            }
        }
    }

    if (matrix[0][0] === 0) {
        for (let j = 0; j < n; j++) {
            matrix[0][j] = 0;
        }
    }

    if (firstColZero) {
        for (let i = 0; i < m; i++) {
            matrix[i][0] = 0;
        }
    }
}
```

#### Go 1.22
```go
package main

func setZeroes(matrix [][]int) {
	if len(matrix) == 0 || len(matrix[0]) == 0 {
		return
	}

	m := len(matrix)
	n := len(matrix[0])
	firstColZero := false

	for i := 0; i < m; i++ {
		if matrix[i][0] == 0 {
			firstColZero = true
		}
		for j := 1; j < n; j++ {
			if matrix[i][j] == 0 {
				matrix[i][0] = 0
				matrix[0][j] = 0
			}
		}
	}

	for i := 1; i < m; i++ {
		for j := 1; j < n; j++ {
			if matrix[i][0] == 0 || matrix[0][j] == 0 {
				matrix[i][j] = 0
			}
		}
	}

	if matrix[0][0] == 0 {
		for j := 0; j < n; j++ {
			matrix[0][j] = 0
		}
	}

	if firstColZero {
		for i := 0; i < m; i++ {
			matrix[i][0] = 0
		}
	}
}
```

#### Rust 1.75
```rust
pub struct Solution;

impl Solution {
    pub fn set_zeroes(matrix: &mut Vec<Vec<i32>>) {
        if matrix.is_empty() || matrix[0].is_empty() {
            return;
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut first_col_zero = false;

        for i in 0..m {
            if matrix[i][0] == 0 {
                first_col_zero = true;
            }
            for j in 1..n {
                if matrix[i][j] == 0 {
                    matrix[i][0] = 0;
                    matrix[0][j] = 0;
                }
            }
        }

        for i in 1..m {
            for j in 1..n {
                if matrix[i][0] == 0 || matrix[0][j] == 0 {
                    matrix[i][j] = 0;
                }
            }
        }

        if matrix[0][0] == 0 {
            for j in 0..n {
                matrix[0][j] = 0;
            }
        }

        if first_col_zero {
            for i in 0..m {
                matrix[i][0] = 0;
            }
        }
    }
}
```

---

## 4. Tier 2: Linear Extra Space (Row & Column Bit Vectors)

### 4.1 Implementation Mechanism
Allocate two boolean arrays `rowHasZero[M]` and `colHasZero[N]`.
Pass 1 scans every cell $(i, j)$; if `matrix[i][j] == 0`, set `rowHasZero[i] = true` and `colHasZero[j] = true`.
Pass 2 updates every cell $(i, j)$ where `rowHasZero[i] || colHasZero[j]`.

```cpp
class SolutionLinearSpace {
public:
    void setZeroes(std::vector<std::vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        std::vector<bool> rows(m, false), cols(n, false);

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (matrix[i][j] == 0) {
                    rows[i] = true;
                    cols[j] = true;
                }
            }
        }

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (rows[i] || cols[j]) {
                    matrix[i][j] = 0;
                }
            }
        }
    }
};
```

### 4.2 Trade-offs
- Implementation logic is simple with minimal branching.
- Requires allocating $O(M + N)$ auxiliary memory.

---

## 5. Tier 3: Coordinate Tracking with Hash Sets

### 5.1 Algorithmic Structure
Insert zero-row and zero-column indices into two separate hash sets `zero_rows` and `zero_cols`.
Iterate through the hash sets to overwrite whole rows and columns.

```python
class SolutionHashSet:
    def setZeroes(self, matrix: List[List[int]]) -> None:
        m, n = len(matrix), len(matrix[0])
        zero_rows, zero_cols = set(), set()

        for i in range(m):
            for j in range(n):
                if matrix[i][j] == 0:
                    zero_rows.add(i)
                    zero_cols.add(j)

        for r in zero_rows:
            for j in range(n):
                matrix[r][j] = 0

        for c in zero_cols:
            for i in range(m):
                matrix[i][c] = 0
```

### 5.2 Trade-offs
- Avoids inspecting non-zero rows during the write pass.
- Incurs hash set hashing and dynamic node allocation overhead.

---

## 6. Tier 4: Brute Force Baseline (Full Matrix Clone Snapshot)

### 6.1 Mechanical Description
Create an exact deep clone of the $M \times N$ matrix.
Iterate through the clone to find cells with value 0, setting the corresponding row and column in the original matrix to 0.

### 6.2 Complexity
- **Time Complexity**: $O(M \times N \times (M + N))$ if zeroing iteratively, or $O(M \times N)$ with batch updates.
- **Space Complexity**: $O(M \times N)$ auxiliary space to store the cloned snapshot.
- **Verdict**: Explicitly violates the problem's in-place requirement.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Row-Major Memory Order**: Iterating outer loop over $i$ (rows) and inner loop over $j$ (columns) accesses consecutive elements in memory, maximizing CPU cache line utilization and hardware prefetching.
2. **Elimination of Branching in Inner Loops**: In the second pass, the condition `matrix[i][0] == 0 || matrix[0][j] == 0` evaluates memory locations that remain pinned in the L1 data cache.
3. **Bitwise Compression**: In Tier 2, using `std::vector<bool>` or bitmasks packs 8 boolean flags per byte, reducing memory traffic.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| Matrix with Single Cell | `[[0]]` or `[[1]]` | `[[0]]` stays 0; `[[1]]` stays 1 | Boundary conditions correctly preserve single cell state. |
| Single Row Matrix | `[[1, 0, 3]]` | Becomes `[[0, 0, 0]]` | `m=1` loops terminate safely. |
| Single Column Matrix | `[[1], [0], [3]]` | Becomes `[[0], [0], [0]]` | `firstColZero` triggers full column zeroing. |
| All Zeros Matrix | All elements 0 | Stays entirely 0 | Zero assignments are idempotent. |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why is an extra boolean variable `firstColZero` necessary?
`matrix[0][0]` is shared by both row 0 and column 0. If it were used for both, setting it to 0 would cause both row 0 and column 0 to be zeroed even if only one of them originally contained a 0.

### 2. Can we use a sentinel value like `INT_MAX - 1` instead?
No. Cell values can span the full range $[-2^{31}, 2^{31}-1]$, so any chosen sentinel value could collide with a valid input element.

### 3. What happens if we update the first row before inner cells?
Updating the first row prematurely would overwrite the column markers `matrix[0][j]`, causing subsequent inner cells to lose track of whether their columns should be zeroed.

### 4. Can the outer loops in the second pass run in reverse?
Yes. Iterating backwards from $m-1$ down to 1 and $n-1$ down to 1 is another valid way to ensure markers are read before modification.

### 5. Why does the problem emphasize an $O(1)$ space solution?
An $O(M \times N)$ or $O(M + N)$ approach is straightforward. The core algorithmic challenge lies in embedding metadata directly into the existing matrix structure.

### 6. Does this algorithm work on non-square rectangular matrices?
Yes. The dimensions $M$ and $N$ are independent, and all loops iterate over their respective dimensional bounds.

### 7. How does this compare with LeetCode 54 (Spiral Matrix)?
LeetCode 54 involves simulating boundary traversals around a matrix, whereas LeetCode 73 projects 2D zero-state constraints onto 1D boundary headers.

### 8. What is the cache impact of zeroing a column?
Zeroing a column accesses memory with stride $N \times \text{sizeof}(\text{int})$, which can cause cache misses across large matrices. Grouping column updates helps mitigate this.

### 9. Can SIMD vectorization accelerate this algorithm?
Yes. Setting entire rows to zero compiles to vectorized memset instructions (`AVX2`/`NEON`), clearing entire cache lines in single clock cycles.

### 10. Why is this solution considered production-grade?
It uses strictly $O(1)$ auxiliary memory, has optimal $O(M \times N)$ time complexity, avoids memory allocations, and handles all matrix dimensions robustly.

---

## 10. Related Problems and Systematic Progression Links

- [[0054-Spiral-Matrix]]: 2D matrix coordinate boundary manipulation.
- [[0048-Rotate-Image]]: In-place index transformations and matrix transpositions.
- LeetCode 36 (Valid Sudoku): Multi-dimensional row, column, and box constraint validation.
- LeetCode 289 (Game of Life): In-place state machine transitions using bitwise encoding in 2D grids.
- LeetCode 498 (Diagonal Traverse): Ordered matrix index scanning.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/set-matrix-zeroes.cpp)
- [Python Implementation](../Python/set-matrix-zeroes.py)
- [Java Implementation](../Java/set-matrix-zeroes.java)
- [TypeScript Implementation](../TypeScript/set-matrix-zeroes.ts)
- [Go Implementation](../Golang/set-matrix-zeroes.go)
- [Rust Implementation](../Rust/set-matrix-zeroes.rs)

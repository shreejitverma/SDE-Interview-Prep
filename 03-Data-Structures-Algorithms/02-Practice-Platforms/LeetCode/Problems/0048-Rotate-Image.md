---
id: leetcode-0048-rotate-image
title: "LeetCode 0048: Rotate Image"
tags:
  - dsa
  - leetcode
  - matrix
  - math
  - in-place
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/rotate-image/"
---

# LeetCode 0048: Rotate Image

## 1. Problem Formalization and Constraints

You are given an $n \times n$ 2D `matrix` representing an image, rotate the image by 90 degrees (clockwise).
You have to rotate the image in-place, which means you have to modify the input 2D matrix directly.
DO NOT allocate another 2D matrix and do the rotation.

### Constraints
- $n == \text{matrix.length} == \text{matrix}[i]\text{.length}$
- $1 \le n \le 20$
- $-1000 \le \text{matrix}[i][j] \le 1000$

### Examples
- **Example 1**:
  - Input: `matrix = [[1,2,3],[4,5,6],[7,8,9]]`
  - Output: `[[7,4,1],[8,5,2],[9,6,3]]`
- **Example 2**:
  - Input: `matrix = [[5,1,9,11],[2,4,8,10],[13,3,6,7],[15,14,12,16]]`
  - Output: `[[15,13,2,5],[14,3,4,1],[12,6,8,9],[16,7,10,11]]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Transpose and Horizontal Reflection | $O(N^2)$ | $O(1)$ | Transposes matrix along main diagonal, then reverses each row horizontally. |
| **Tier 2 (Ring 4-Way Rotation)** | Concentric Ring 4-Point Cycle | $O(N^2)$ | $O(1)$ | Rotates elements four at a time across $\lfloor N / 2 \rfloor$ concentric perimeter rings. |
| **Tier 3 (Vertical Reflection & Transpose)** | Flip Upside Down and Transpose | $O(N^2)$ | $O(1)$ | Reverses matrix rows vertically, then transposes across main diagonal. |
| **Tier 4 (Brute Force)** | Auxiliary Matrix Re-Mapping | $O(N^2)$ | $O(N^2)$ | Allocates fresh $N \times N$ matrix and copies $M_{\text{new}}[j][n - 1 - i] = M[i][j]$. |

---

## 3. Tier 1: Most Optimal Solution (Transpose and Horizontal Reflection)

### 3.1 Algorithmic Mechanics and Invariant Proof

A 90-degree clockwise rotation transforms each coordinate according to:
$$(i, j) \mapsto (j, n - 1 - i)$$

In linear algebra, this rotation matrix decomposes into two consecutive elementary reflections:
1. Reflection across the main diagonal (Matrix Transpose):
   $$(i, j) \mapsto (j, i)$$
2. Horizontal reflection across the vertical line of symmetry (Row Reversal):
   $$(j, i) \mapsto (j, n - 1 - i)$$

Algorithmic procedure:
1. Matrix Transpose: For every row $i$ from $0$ to $n - 1$ and column $j$ from $i + 1$ to $n - 1$, swap `matrix[i][j]` with `matrix[j][i]`.
2. Row Reversal: For every row $i$ from $0$ to $n - 1$, reverse the elements of row $i$ between column $0$ and $n - 1$.

**Invariant Proof**:
The composition of functions:
$$\sigma_{\text{reflect}} \circ \sigma_{\text{transpose}} (i, j) = \sigma_{\text{reflect}}(j, i) = (j, n - 1 - i)$$
This composite mapping maps every original index $(i, j)$ directly to the exact target 90-degree rotated coordinate.
Because transposition swaps distinct unordered pairs $(i, j)$ with $(j, i)$ exactly once, and row reversal swaps pairs along each row symmetrically, no element is overwritten before being saved.
The operations execute completely in-place without auxiliary buffer allocations.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Transposition performs $N(N - 1) / 2$ element swaps. Row reversal performs $N \times \lfloor N / 2 \rfloor$ swaps. Total memory swaps equal $N^2 - N/2 = O(N^2)$.
- **Auxiliary Space Complexity**: $O(1)$. Uses only primitive index and swap variables.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    void rotate(std::vector<std::vector<int>>& matrix) {
        int n = static_cast<int>(matrix.size());

        // Step 1: Transpose matrix
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                std::swap(matrix[i][j], matrix[j][i]);
            }
        }

        // Step 2: Reverse each row
        for (int i = 0; i < n; ++i) {
            std::reverse(matrix[i].begin(), matrix[i].end());
        }
    }
};
```

#### Python 3
```python
from typing import List

class Solution:
    def rotate(self, matrix: List[List[int]]) -> None:
        n = len(matrix)

        # Step 1: Transpose matrix
        for i in range(n):
            for j in range(i + 1, n):
                matrix[i][j], matrix[j][i] = matrix[j][i], matrix[i][j]

        # Step 2: Reverse each row
        for i in range(n):
            matrix[i].reverse()
```

#### Java 21
```java
class Solution {
    public void rotate(int[][] matrix) {
        int n = matrix.length;

        // Step 1: Transpose matrix
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                int temp = matrix[i][j];
                matrix[i][j] = matrix[j][i];
                matrix[j][i] = temp;
            }
        }

        // Step 2: Reverse each row
        for (int i = 0; i < n; i++) {
            int left = 0, right = n - 1;
            while (left < right) {
                int temp = matrix[i][left];
                matrix[i][left] = matrix[i][right];
                matrix[i][right] = temp;
                left++;
                right--;
            }
        }
    }
}
```

#### TypeScript 5
```typescript
function rotate(matrix: number[][]): void {
    const n = matrix.length;

    // Step 1: Transpose matrix
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            const temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }

    // Step 2: Reverse each row
    for (let i = 0; i < n; i++) {
        let left = 0;
        let right = n - 1;
        while (left < right) {
            const temp = matrix[i][left];
            matrix[i][left] = matrix[i][right];
            matrix[i][right] = temp;
            left++;
            right--;
        }
    }
}
```

#### Go 1.22
```go
package main

func rotate(matrix [][]int) {
	n := len(matrix)

	// Step 1: Transpose matrix
	for i := 0; i < n; i++ {
		for j := i + 1; j < n; j++ {
			matrix[i][j], matrix[j][i] = matrix[j][i], matrix[i][j]
		}
	}

	// Step 2: Reverse each row
	for i := 0; i < n; i++ {
		left, right := 0, n-1
		for left < right {
			matrix[i][left], matrix[i][right] = matrix[i][right], matrix[i][left]
			left++
			right--
		}
	}
}
```

#### Rust 2021
```rust
impl Solution {
    pub fn rotate(matrix: &mut Vec<Vec<i32>>) {
        let n = matrix.len();

        // Step 1: Transpose matrix
        for i in 0..n {
            for j in (i + 1)..n {
                let temp = matrix[i][j];
                matrix[i][j] = matrix[j][i];
                matrix[j][i] = temp;
            }
        }

        // Step 2: Reverse each row
        for i in 0..n {
            matrix[i].reverse();
        }
    }
}
```

---

## 4. Tier 2: Concentric Ring 4-Way Rotation

### 4.1 Algorithmic Mechanics
Process the matrix layer by layer from the outermost perimeter inward:
For layer `i` from $0$ to $\lfloor n / 2 \rfloor - 1$:
For offset `j` from $i$ to $n - 2 - i$:
Perform a 4-way cyclic exchange:
1. `temp = matrix[i][j]`
2. `matrix[i][j] = matrix[n - 1 - j][i]` (Bottom-left to Top-left)
3. `matrix[n - 1 - j][i] = matrix[n - 1 - i][n - 1 - j]` (Bottom-right to Bottom-left)
4. `matrix[n - 1 - i][n - 1 - j] = matrix[j][n - 1 - i]` (Top-right to Bottom-right)
5. `matrix[j][n - 1 - i] = temp` (Top-left to Top-right)

### 4.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$. Each of the $N^2$ elements is accessed and assigned once.
- **Space Complexity**: $O(1)$ auxiliary memory.

### 4.3 Implementation (C++20)
```cpp
#include <vector>

class Solution {
public:
    void rotate(std::vector<std::vector<int>>& matrix) {
        int n = static_cast<int>(matrix.size());

        for (int i = 0; i < n / 2; ++i) {
            for (int j = i; j < n - 1 - i; ++j) {
                int temp = matrix[i][j];
                matrix[i][j] = matrix[n - 1 - j][i];
                matrix[n - 1 - j][i] = matrix[n - 1 - i][n - 1 - j];
                matrix[n - 1 - i][n - 1 - j] = matrix[j][n - 1 - i];
                matrix[j][n - 1 - i] = temp;
            }
        }
    }
};
```

---

## 5. Tier 3: Vertical Reflection & Transpose

### 5.1 Algorithmic Mechanics
An equivalent algebraic decomposition reverses rows vertically first, then transposes along the main diagonal:
1. Swap row $i$ with row $n - 1 - i$ for $0 \le i < \lfloor n / 2 \rfloor$.
2. Transpose the matrix across the main diagonal: swap `matrix[i][j]` with `matrix[j][i]`.
The result is identical to clockwise 90-degree rotation.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(1)$ auxiliary space.

### 5.3 Implementation (C++20)
```cpp
#include <vector>
#include <algorithm>

class Solution {
public:
    void rotate(std::vector<std::vector<int>>& matrix) {
        int n = static_cast<int>(matrix.size());

        // Reverse rows vertically
        std::reverse(matrix.begin(), matrix.end());

        // Transpose
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                std::swap(matrix[i][j], matrix[j][i]);
            }
        }
    }
};
```

---

## 6. Tier 4: Brute Force Solution (Auxiliary Matrix Allocation)

### 6.1 Algorithmic Mechanics
Allocate a separate $N \times N$ 2D vector `rotated`.
Directly assign `rotated[j][n - 1 - i] = matrix[i][j]` for all $(i, j)$.
Copy `rotated` back into `matrix`.
While simple, this violates the problem's strict in-place modification requirement.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(N^2)$.
- **Space Complexity**: $O(N^2)$ auxiliary memory for the duplicate matrix.

### 6.3 Implementation (Python 3)
```python
from typing import List

class Solution:
    def rotate(self, matrix: List[List[int]]) -> None:
        n = len(matrix)
        copy_matrix = [[matrix[r][c] for c in range(n)] for r in range(n)]

        for r in range(n):
            for c in range(n):
                matrix[c][n - 1 - r] = copy_matrix[r][c]
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. How do you rotate a matrix 90 degrees counter-clockwise in-place?</summary>
To rotate 90 degrees counter-clockwise: transpose the matrix across the main diagonal, then reverse each column vertically (or reverse each row horizontally first, then transpose).
</details>

<details>
<summary>2. How do you rotate a matrix 180 degrees in-place?</summary>
Reverse each row horizontally, and then reverse each column vertically (or swap elements symmetrically across the center point).
</details>

<details>
<summary>3. Why does starting transposition at `j = i + 1` prevent redundant swaps?</summary>
If $j$ ranged from $0$ to $n - 1$, elements would be swapped twice: once at $(i, j)$ and again at $(j, i)$, restoring the original matrix.
Restricting $j > i$ processes only the upper triangle strictly above the diagonal.
</details>

<details>
<summary>4. What happens when $N = 1$?</summary>
The loops for both transpose and row reversal do not execute because $j = i + 1 = 1 \ge n$, and the single element remains in place correctly.
</details>

<details>
<summary>5. How does cache locality compare between Transpose-then-Reflect (Tier 1) and 4-Way Rotation (Tier 2)?</summary>
Tier 1 performs contiguous row operations during horizontal reflection, maximizing spatial locality and SIMD vectorization.
Tier 2 jumps across four corners of the matrix on each step, inducing more cache misses.
</details>

<details>
<summary>6. Can this algorithm rotate non-square $M \times N$ matrices in-place?</summary>
No. A 90-degree rotation transforms an $M \times N$ matrix into an $N \times M$ matrix, changing the underlying memory dimension layout which cannot be done in-place without complex cyclical permutation shuffles.
</details>

<details>
<summary>7. What is the total number of concentric layers in an $N \times N$ matrix?</summary>
There are $\lfloor N / 2 \rfloor$ layers.
When $N$ is odd, the central element at $(\lfloor N/2 \rfloor, \lfloor N/2 \rfloor)$ stays stationary under rotation.
</details>

<details>
<summary>8. In C++, how does `std::reverse(matrix.begin(), matrix.end())` execute in Tier 3?</summary>
Because `matrix` is a `vector<vector<int>>`, reversing `matrix` swaps inner vector pointers in $O(1)$ operations per row without copying elements.
</details>

<details>
<summary>9. Why is in-place matrix rotation critical in computer vision and game engines?</summary>
Large image textures ($4K \times 4K$) consume dozens of megabytes of video memory.
In-place operations prevent memory spikes and garbage collection pauses during real-time rendering.
</details>

<details>
<summary>10. How does reflection across the anti-diagonal compare with the main diagonal?</summary>
Reflection across the anti-diagonal maps $(i, j) \mapsto (n - 1 - j, n - 1 - i)$.
Combining anti-diagonal reflection with horizontal reflection yields counter-clockwise rotation.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/rotate-image.cpp)
- [Python Implementation](../Python/rotate-image.py)
- [Java Implementation](../Java/rotate-image.java)
- [TypeScript Implementation](../TypeScript/rotate-image.ts)
- [Go Implementation](../Golang/rotate-image.go)
- [Rust Implementation](../Rust/rotate-image.rs)

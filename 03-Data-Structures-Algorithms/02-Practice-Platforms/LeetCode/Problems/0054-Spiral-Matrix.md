---
id: leetcode-0054-spiral-matrix
title: "LeetCode 0054: Spiral Matrix"
tags:
  - dsa
  - leetcode
  - matrix
  - simulation
  - array
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/spiral-matrix/"
---

# LeetCode 0054: Spiral Matrix

## 1. Problem Formalization and Constraints

Given an $m \times n$ matrix, return all elements of the matrix in spiral order.

### Constraints
- $m == \text{matrix.length}$
- $n == \text{matrix}[i]\text{.length}$
- $1 \le m, n \le 10$
- $-100 \le \text{matrix}[i][j] \le 100$

### Examples
- **Example 1**:
  - Input: `matrix = [[1,2,3],[4,5,6],[7,8,9]]`
  - Output: `[1,2,3,6,9,8,7,4,5]`
- **Example 2**:
  - Input: `matrix = [[1,2,3,4],[5,6,7,8],[9,10,11,12]]`
  - Output: `[1,2,3,4,8,12,11,10,9,5,6,7]`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Shrinking Boundary Simulation | $O(M \times N)$ | $O(1)$ | Four boundaries (`top`, `bottom`, `left`, `right`) contract inward; no extra visited matrices. |
| **Tier 2 (Space-Optimized)** | In-Place Direction Vector Simulation | $O(M \times N)$ | $O(1)$ | Traverses with `(dr, dc)` vectors; modifies visited cells with an out-of-band sentinel value. |
| **Tier 3 (Time-Optimized Alternative)** | Explicit 2D Visited Matrix Simulation | $O(M \times N)$ | $O(M \times N)$ | Traverses with direction vectors and turns clockwise upon hitting true values in a boolean grid. |
| **Tier 4 (Brute Force)** | Recursive Layer Peeling | $O(M \times N)$ | $O(\min(M, N))$ | Peels one outer perimeter layer per recursive step; carries stack overhead proportional to layers. |

---

## 3. Tier 1: Most Optimal Solution (Shrinking Boundary Simulation)

### 3.1 Algorithmic Mechanics and Invariant Proof

Maintain four geometric boundaries:
- `top = 0`
- `bottom = m - 1`
- `left = 0`
- `right = n - 1`

At each cycle of the while loop (`top <= bottom && left <= right`):
1. Traverse left to right along row `top`: add `matrix[top][col]` for `col` from `left` to `right`.
Then increment `top++`.
2. Traverse top to bottom along column `right`: add `matrix[row][right]` for `row` from `top` to `bottom`.
Then decrement `right--`.
3. Check guard condition `if (top <= bottom)`: traverse right to left along row `bottom`: add `matrix[bottom][col]` for `col` from `right` down to `left`.
Then decrement `bottom--`.
4. Check guard condition `if (left <= right)`: traverse bottom to top along column `left`: add `matrix[row][left]` for `row` from `bottom` down to `top`.
Then increment `left++`.

**Invariant Proof**:
The guard checks in steps 3 and 4 prevent re-processing elements in degenerate cases (such as single-row or single-column submatrices) where `top > bottom` or `left > right` after steps 1 or 2.
Every cell is visited exactly once, yielding an optimal $O(M \times N)$ traversal.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$. Every element in the $M \times N$ matrix is visited exactly once.
- **Space Complexity**: $O(1)$ auxiliary space. Uses four integer variables for the boundaries.

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> spiralOrder(std::vector<std::vector<int>>& matrix) {
        std::vector<int> result;
        if (matrix.empty() || matrix[0].empty()) return result;

        int top = 0;
        int bottom = static_cast<int>(matrix.size()) - 1;
        int left = 0;
        int right = static_cast<int>(matrix[0].size()) - 1;

        while (top <= bottom && left <= right) {
            for (int col = left; col <= right; ++col) {
                result.push_back(matrix[top][col]);
            }
            ++top;

            for (int row = top; row <= bottom; ++row) {
                result.push_back(matrix[row][right]);
            }
            --right;

            if (top <= bottom) {
                for (int col = right; col >= left; --col) {
                    result.push_back(matrix[bottom][col]);
                }
                --bottom;
            }

            if (left <= right) {
                for (int row = bottom; row >= top; --row) {
                    result.push_back(matrix[row][left]);
                }
                ++left;
            }
        }
        return result;
    }
};
```

#### Python 3.12
```python
class Solution:
    def spiralOrder(self, matrix: list[list[int]]) -> list[int]:
        if not matrix or not matrix[0]:
            return []

        result: list[int] = []
        top, bottom = 0, len(matrix) - 1
        left, right = 0, len(matrix[0]) - 1

        while top <= bottom and left <= right:
            for col in range(left, right + 1):
                result.append(matrix[top][col])
            top += 1

            for row in range(top, bottom + 1):
                result.append(matrix[row][right])
            right -= 1

            if top <= bottom:
                for col in range(right, left - 1, -1):
                    result.append(matrix[bottom][col])
                bottom -= 1

            if left <= right:
                for row in range(bottom, top - 1, -1):
                    result.append(matrix[row][left])
                left += 1

        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

class Solution {
    public List<Integer> spiralOrder(int[][] matrix) {
        List<Integer> result = new ArrayList<>();
        if (matrix == null || matrix.length == 0 || matrix[0].length == 0) {
            return result;
        }

        int top = 0;
        int bottom = matrix.length - 1;
        int left = 0;
        int right = matrix[0].length - 1;

        while (top <= bottom && left <= right) {
            for (int col = left; col <= right; col++) {
                result.add(matrix[top][col]);
            }
            top++;

            for (int row = top; row <= bottom; row++) {
                result.add(matrix[row][right]);
            }
            right--;

            if (top <= bottom) {
                for (int col = right; col >= left; col--) {
                    result.add(matrix[bottom][col]);
                }
                bottom--;
            }

            if (left <= right) {
                for (int row = bottom; row >= top; row--) {
                    result.add(matrix[row][left]);
                }
                left++;
            }
        }

        return result;
    }
}
```

#### TypeScript
```typescript
function spiralOrder(matrix: number[][]): number[] {
    const result: number[] = [];
    if (matrix.length === 0 || matrix[0].length === 0) {
        return result;
    }

    let top = 0;
    let bottom = matrix.length - 1;
    let left = 0;
    let right = matrix[0].length - 1;

    while (top <= bottom && left <= right) {
        for (let col = left; col <= right; col++) {
            result.push(matrix[top][col]);
        }
        top++;

        for (let row = top; row <= bottom; row++) {
            result.push(matrix[row][right]);
        }
        right--;

        if (top <= bottom) {
            for (let col = right; col >= left; col--) {
                result.push(matrix[bottom][col]);
            }
            bottom--;
        }

        if (left <= right) {
            for (let row = bottom; row >= top; row--) {
                result.push(matrix[row][left]);
            }
            left++;
        }
    }

    return result;
}
```

#### Go
```go
package main

func spiralOrder(matrix [][]int) []int {
    if len(matrix) == 0 || len(matrix[0]) == 0 {
        return nil
    }

    m, n := len(matrix), len(matrix[0])
    result := make([]int, 0, m*n)

    top, bottom := 0, m-1
    left, right := 0, n-1

    for top <= bottom && left <= right {
        for col := left; col <= right; col++ {
            result = append(result, matrix[top][col])
        }
        top++

        for row := top; row <= bottom; row++ {
            result = append(result, matrix[row][right])
        }
        right--

        if top <= bottom {
            for col := right; col >= left; col-- {
                result = append(result, matrix[bottom][col])
            }
            bottom--
        }

        if left <= right {
            for row := bottom; row >= top; row-- {
                result = append(result, matrix[row][left])
            }
            left++
        }
    }

    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn spiral_order(matrix: Vec<Vec<i32>>) -> Vec<i32> {
        if matrix.is_empty() || matrix[0].is_empty() {
            return Vec::new();
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut result = Vec::with_capacity(m * n);

        let mut top = 0i32;
        let mut bottom = m as i32 - 1;
        let mut left = 0i32;
        let mut right = n as i32 - 1;

        while top <= bottom && left <= right {
            for col in left..=right {
                result.push(matrix[top as usize][col as usize]);
            }
            top += 1;

            for row in top..=bottom {
                result.push(matrix[row as usize][right as usize]);
            }
            right -= 1;

            if top <= bottom {
                for col in (left..=right).rev() {
                    result.push(matrix[bottom as usize][col as usize]);
                }
                bottom -= 1;
            }

            if left <= right {
                for row in (top..=bottom).rev() {
                    result.push(matrix[row as usize][left as usize]);
                }
                left += 1;
            }
        }

        result
    }
}
```

---

## 4. Tier 2: Space-Optimized Solution (In-Place Direction Vector Simulation)

### 4.1 Algorithmic Mechanics and Invariant Proof

Use direction offsets:
- Right: $(0, 1)$
- Down: $(1, 0)$
- Left: $(0, -1)$
- Up: $(-1, 0)$

Start at cell $(0, 0)$ heading Right.
After visiting cell $(r, c)$, overwrite its value with an out-of-range sentinel value (e.g. $101$, since $-100 \le \text{matrix}[i][j] \le 100$).
Whenever the next step leads out of bounds or into a cell containing the sentinel, turn $90^\circ$ clockwise by advancing the direction index: `dir = (dir + 1) % 4`.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$. Traverses exactly $M \times N$ cells.
- **Space Complexity**: $O(1)$ auxiliary space. Mutates input in-place without extra structures.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> spiralOrder(std::vector<std::vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return {};
        int m = static_cast<int>(matrix.size());
        int n = static_cast<int>(matrix[0].size());
        std::vector<int> result;
        result.reserve(m * n);

        int dr[4] = {0, 1, 0, -1};
        int dc[4] = {1, 0, -1, 0};
        int r = 0, c = 0, d = 0;

        for (int i = 0; i < m * n; ++i) {
            result.push_back(matrix[r][c]);
            matrix[r][c] = 101; // Out-of-bounds sentinel

            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n || matrix[nr][nc] == 101) {
                d = (d + 1) % 4;
                nr = r + dr[d];
                nc = c + dc[d];
            }
            r = nr;
            c = nc;
        }
        return result;
    }
};
```

#### Python 3.12
```python
class Solution:
    def spiralOrder(self, matrix: list[list[int]]) -> list[int]:
        if not matrix or not matrix[0]:
            return []

        m, n = len(matrix), len(matrix[0])
        result: list[int] = []
        dr = [0, 1, 0, -1]
        dc = [1, 0, -1, 0]
        r, c, d = 0, 0, 0

        for _ in range(m * n):
            result.append(matrix[r][c])
            matrix[r][c] = 101

            nr, nc = r + dr[d], c + dc[d]
            if not (0 <= nr < m and 0 <= nc < n and matrix[nr][nc] != 101):
                d = (d + 1) % 4
                nr, nc = r + dr[d], c + dc[d]
            r, c = nr, nc

        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

class Solution {
    public List<Integer> spiralOrder(int[][] matrix) {
        List<Integer> result = new ArrayList<>();
        if (matrix == null || matrix.length == 0) return result;

        int m = matrix.length;
        int n = matrix[0].length;
        int[] dr = {0, 1, 0, -1};
        int[] dc = {1, 0, -1, 0};
        int r = 0, c = 0, d = 0;

        for (int i = 0; i < m * n; i++) {
            result.add(matrix[r][c]);
            matrix[r][c] = 101;

            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n || matrix[nr][nc] == 101) {
                d = (d + 1) % 4;
                nr = r + dr[d];
                nc = c + dc[d];
            }
            r = nr;
            c = nc;
        }

        return result;
    }
}
```

#### TypeScript
```typescript
function spiralOrder(matrix: number[][]): number[] {
    if (matrix.length === 0 || matrix[0].length === 0) return [];

    const m = matrix.length;
    const n = matrix[0].length;
    const result: number[] = [];
    const dr = [0, 1, 0, -1];
    const dc = [1, 0, -1, 0];
    let r = 0, c = 0, d = 0;

    for (let i = 0; i < m * n; i++) {
        result.push(matrix[r][c]);
        matrix[r][c] = 101;

        let nr = r + dr[d];
        let nc = c + dc[d];
        if (nr < 0 || nr >= m || nc < 0 || nc >= n || matrix[nr][nc] === 101) {
            d = (d + 1) % 4;
            nr = r + dr[d];
            nc = c + dc[d];
        }
        r = nr;
        c = nc;
    }

    return result;
}
```

#### Go
```go
package main

func spiralOrder(matrix [][]int) []int {
    if len(matrix) == 0 || len(matrix[0]) == 0 {
        return nil
    }

    m, n := len(matrix), len(matrix[0])
    result := make([]int, 0, m*n)
    dr := []int{0, 1, 0, -1}
    dc := []int{1, 0, -1, 0}
    r, c, d := 0, 0, 0

    for i := 0; i < m*n; i++ {
        result = append(result, matrix[r][c])
        matrix[r][c] = 101

        nr := r + dr[d]
        nc := c + dc[d]
        if nr < 0 || nr >= m || nc < 0 || nc >= n || matrix[nr][nc] == 101 {
            d = (d + 1) % 4
            nr = r + dr[d]
            nc = c + dc[d]
        }
        r = nr
        c = nc
    }

    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn spiral_order(mut matrix: Vec<Vec<i32>>) -> Vec<i32> {
        if matrix.is_empty() || matrix[0].is_empty() {
            return Vec::new();
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut result = Vec::with_capacity(m * n);

        let dr = [0, 1, 0, -1];
        let dc = [1, 0, -1, 0];
        let mut r = 0i32;
        let mut c = 0i32;
        let mut d = 0usize;

        for _ in 0..(m * n) {
            result.push(matrix[r as usize][c as usize]);
            matrix[r as usize][c as usize] = 101;

            let mut nr = r + dr[d];
            let mut nc = c + dc[d];
            if nr < 0 || nr >= m as i32 || nc < 0 || nc >= n as i32 || matrix[nr as usize][nc as usize] == 101 {
                d = (d + 1) % 4;
                nr = r + dr[d];
                nc = c + dc[d];
            }
            r = nr;
            c = nc;
        }

        result
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative Solution (Explicit 2D Visited Matrix Simulation)

### 5.1 Algorithmic Mechanics and Invariant Proof

Instead of mutating the input matrix with sentinel numbers, allocate an auxiliary $M \times N$ boolean array `visited`.
Check boundaries and the `visited` grid on each step to decide when to turn.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$. Visits each cell exactly once.
- **Space Complexity**: $O(M \times N)$ auxiliary space for the boolean grid.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> spiralOrder(std::vector<std::vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return {};
        int m = static_cast<int>(matrix.size());
        int n = static_cast<int>(matrix[0].size());
        std::vector<std::vector<bool>> visited(m, std::vector<bool>(n, false));
        std::vector<int> result;

        int dr[4] = {0, 1, 0, -1};
        int dc[4] = {1, 0, -1, 0};
        int r = 0, c = 0, d = 0;

        for (int i = 0; i < m * n; ++i) {
            result.push_back(matrix[r][c]);
            visited[r][c] = true;

            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n || visited[nr][nc]) {
                d = (d + 1) % 4;
                nr = r + dr[d];
                nc = c + dc[d];
            }
            r = nr;
            c = nc;
        }
        return result;
    }
};
```

#### Python 3.12
```python
class Solution:
    def spiralOrder(self, matrix: list[list[int]]) -> list[int]:
        if not matrix or not matrix[0]:
            return []

        m, n = len(matrix), len(matrix[0])
        visited = [[False] * n for _ in range(m)]
        result: list[int] = []

        dr = [0, 1, 0, -1]
        dc = [1, 0, -1, 0]
        r, c, d = 0, 0, 0

        for _ in range(m * n):
            result.append(matrix[r][c])
            visited[r][c] = True

            nr, nc = r + dr[d], c + dc[d]
            if not (0 <= nr < m and 0 <= nc < n and not visited[nr][nc]):
                d = (d + 1) % 4
                nr, nc = r + dr[d], c + dc[d]
            r, c = nr, nc

        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

class Solution {
    public List<Integer> spiralOrder(int[][] matrix) {
        List<Integer> result = new ArrayList<>();
        if (matrix == null || matrix.length == 0) return result;

        int m = matrix.length;
        int n = matrix[0].length;
        boolean[][] visited = new boolean[m][n];

        int[] dr = {0, 1, 0, -1};
        int[] dc = {1, 0, -1, 0};
        int r = 0, c = 0, d = 0;

        for (int i = 0; i < m * n; i++) {
            result.add(matrix[r][c]);
            visited[r][c] = true;

            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n || visited[nr][nc]) {
                d = (d + 1) % 4;
                nr = r + dr[d];
                nc = c + dc[d];
            }
            r = nr;
            c = nc;
        }
        return result;
    }
}
```

#### TypeScript
```typescript
function spiralOrder(matrix: number[][]): number[] {
    if (matrix.length === 0 || matrix[0].length === 0) return [];

    const m = matrix.length;
    const n = matrix[0].length;
    const visited: boolean[][] = Array.from({ length: m }, () => new Array(n).fill(false));
    const result: number[] = [];

    const dr = [0, 1, 0, -1];
    const dc = [1, 0, -1, 0];
    let r = 0, c = 0, d = 0;

    for (let i = 0; i < m * n; i++) {
        result.push(matrix[r][c]);
        visited[r][c] = true;

        let nr = r + dr[d];
        let nc = c + dc[d];
        if (nr < 0 || nr >= m || nc < 0 || nc >= n || visited[nr][nc]) {
            d = (d + 1) % 4;
            nr = r + dr[d];
            nc = c + dc[d];
        }
        r = nr;
        c = nc;
    }

    return result;
}
```

#### Go
```go
package main

func spiralOrder(matrix [][]int) []int {
    if len(matrix) == 0 || len(matrix[0]) == 0 {
        return nil
    }

    m, n := len(matrix), len(matrix[0])
    visited := make([][]bool, m)
    for i := range visited {
        visited[i] = make([]bool, n)
    }

    result := make([]int, 0, m*n)
    dr := []int{0, 1, 0, -1}
    dc := []int{1, 0, -1, 0}
    r, c, d := 0, 0, 0

    for i := 0; i < m*n; i++ {
        result = append(result, matrix[r][c])
        visited[r][c] = true

        nr := r + dr[d]
        nc := c + dc[d]
        if nr < 0 || nr >= m || nc < 0 || nc >= n || visited[nr][nc] {
            d = (d + 1) % 4
            nr = r + dr[d]
            nc = c + dc[d]
        }
        r = nr
        c = nc
    }

    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn spiral_order(matrix: Vec<Vec<i32>>) -> Vec<i32> {
        if matrix.is_empty() || matrix[0].is_empty() {
            return Vec::new();
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut visited = vec![vec![false; n]; m];
        let mut result = Vec::with_capacity(m * n);

        let dr = [0, 1, 0, -1];
        let dc = [1, 0, -1, 0];
        let mut r = 0i32;
        let mut c = 0i32;
        let mut d = 0usize;

        for _ in 0..(m * n) {
            result.push(matrix[r as usize][c as usize]);
            visited[r as usize][c as usize] = true;

            let mut nr = r + dr[d];
            let mut nc = c + dc[d];
            if nr < 0 || nr >= m as i32 || nc < 0 || nc >= n as i32 || visited[nr as usize][nc as usize] {
                d = (d + 1) % 4;
                nr = r + dr[d];
                nc = c + dc[d];
            }
            r = nr;
            c = nc;
        }

        result
    }
}
```

---

## 6. Tier 4: Brute Force Solution (Recursive Layer Peeling)

### 6.1 Algorithmic Mechanics and Invariant Proof

Decompose the matrix into concentric rectangular rings.
At layer $k$, process the perimeter cells defined by submatrix $(k, k)$ to $(m - 1 - k, n - 1 - k)$.
After outputting the border elements, recursively call the function on layer $k + 1$.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$. Traverses every cell once across all concentric rings.
- **Space Complexity**: $O(\min(M, N))$ recursion stack depth.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
public:
    std::vector<int> spiralOrder(std::vector<std::vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return {};
        std::vector<int> result;
        peel(matrix, 0, static_cast<int>(matrix.size()) - 1, 0, static_cast<int>(matrix[0].size()) - 1, result);
        return result;
    }

private:
    void peel(const std::vector<std::vector<int>>& matrix, int top, int bottom, int left, int right, std::vector<int>& result) {
        if (top > bottom || left > right) return;

        for (int col = left; col <= right; ++col) result.push_back(matrix[top][col]);
        for (int row = top + 1; row <= bottom; ++row) result.push_back(matrix[row][right]);

        if (top < bottom) {
            for (int col = right - 1; col >= left; --col) result.push_back(matrix[bottom][col]);
        }
        if (left < right) {
            for (int row = bottom - 1; row > top; --row) result.push_back(matrix[row][left]);
        }

        peel(matrix, top + 1, bottom - 1, left + 1, right - 1, result);
    }
};
```

#### Python 3.12
```python
class Solution:
    def spiralOrder(self, matrix: list[list[int]]) -> list[int]:
        if not matrix or not matrix[0]:
            return []

        result: list[int] = []

        def peel(top: int, bottom: int, left: int, right: int) -> None:
            if top > bottom or left > right:
                return

            for col in range(left, right + 1):
                result.append(matrix[top][col])
            for row in range(top + 1, bottom + 1):
                result.append(matrix[row][right])

            if top < bottom:
                for col in range(right - 1, left - 1, -1):
                    result.append(matrix[bottom][col])
            if left < right:
                for row in range(bottom - 1, top, -1):
                    result.append(matrix[row][left])

            peel(top + 1, bottom - 1, left + 1, right - 1)

        peel(0, len(matrix) - 1, 0, len(matrix[0]) - 1)
        return result
```

#### Java 21
```java
import java.util.ArrayList;
import java.util.List;

class Solution {
    public List<Integer> spiralOrder(int[][] matrix) {
        List<Integer> result = new ArrayList<>();
        if (matrix == null || matrix.length == 0) return result;
        peel(matrix, 0, matrix.length - 1, 0, matrix[0].length - 1, result);
        return result;
    }

    private void peel(int[][] matrix, int top, int bottom, int left, int right, List<Integer> result) {
        if (top > bottom || left > right) return;

        for (int col = left; col <= right; col++) result.add(matrix[top][col]);
        for (int row = top + 1; row <= bottom; row++) result.add(matrix[row][right]);

        if (top < bottom) {
            for (int col = right - 1; col >= left; col--) result.add(matrix[bottom][col]);
        }
        if (left < right) {
            for (int row = bottom - 1; row > top; row--) result.add(matrix[row][left]);
        }

        peel(matrix, top + 1, bottom - 1, left + 1, right - 1, result);
    }
}
```

#### TypeScript
```typescript
function spiralOrder(matrix: number[][]): number[] {
    const result: number[] = [];
    if (matrix.length === 0 || matrix[0].length === 0) return result;

    function peel(top: number, bottom: number, left: number, right: number): void {
        if (top > bottom || left > right) return;

        for (let col = left; col <= right; col++) result.push(matrix[top][col]);
        for (let row = top + 1; row <= bottom; row++) result.push(matrix[row][right]);

        if (top < bottom) {
            for (let col = right - 1; col >= left; col--) result.push(matrix[bottom][col]);
        }
        if (left < right) {
            for (let row = bottom - 1; row > top; row--) result.push(matrix[row][left]);
        }

        peel(top + 1, bottom - 1, left + 1, right - 1);
    }

    peel(0, matrix.length - 1, 0, matrix[0].length - 1);
    return result;
}
```

#### Go
```go
package main

func spiralOrder(matrix [][]int) []int {
    if len(matrix) == 0 || len(matrix[0]) == 0 {
        return nil
    }

    result := make([]int, 0, len(matrix)*len(matrix[0]))

    var peel func(top, bottom, left, right int)
    peel = func(top, bottom, left, right int) {
        if top > bottom || left > right {
            return
        }

        for col := left; col <= right; col++ {
            result = append(result, matrix[top][col])
        }
        for row := top + 1; row <= bottom; row++ {
            result = append(result, matrix[row][right])
        }

        if top < bottom {
            for col := right - 1; col >= left; col-- {
                result = append(result, matrix[bottom][col])
            }
        }
        if left < right {
            for row := bottom - 1; row > top; row-- {
                result = append(result, matrix[row][left])
            }
        }

        peel(top+1, bottom-1, left+1, right-1)
    }

    peel(0, len(matrix)-1, 0, len(matrix[0])-1)
    return result
}
```

#### Rust
```rust
impl Solution {
    pub fn spiral_order(matrix: Vec<Vec<i32>>) -> Vec<i32> {
        if matrix.is_empty() || matrix[0].is_empty() {
            return Vec::new();
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut result = Vec::with_capacity(m * n);

        fn peel(matrix: &[Vec<i32>], top: i32, bottom: i32, left: i32, right: i32, result: &mut Vec<i32>) {
            if top > bottom || left > right {
                return;
            }

            for col in left..=right {
                result.push(matrix[top as usize][col as usize]);
            }
            for row in (top + 1)..=bottom {
                result.push(matrix[row as usize][right as usize]);
            }

            if top < bottom {
                for col in (left..=(right - 1)).rev() {
                    result.push(matrix[bottom as usize][col as usize]);
                }
            }
            if left < right {
                for row in ((top + 1)..=(bottom - 1)).rev() {
                    result.push(matrix[row as usize][left as usize]);
                }
            }

            peel(matrix, top + 1, bottom - 1, left + 1, right - 1, result);
        }

        peel(&matrix, 0, m as i32 - 1, 0, n as i32 - 1, &mut result);
        result
    }
}
```

---

## 7. Active Recall and Edge-Case Evaluation

<details>
<summary>1. Why are boundary guards `if (top <= bottom)` and `if (left <= right)` required in Tier 1?</summary>
When the matrix has an odd number of rows or columns, the final submatrix is a single row or single column.
After traversing the top row, `top` increments.
Without the check, the loop would execute the bottom row traversal backward over the exact same cells already visited.
</details>

<details>
<summary>2. What happens when the input matrix has dimension $1 \times N$?</summary>
The top row is processed from column $0$ to $N - 1$.
`top` increments to 1.
Because `top > bottom`, steps 3 and 4 are skipped, correctly returning the single row without duplicates.
</details>

<details>
<summary>3. What happens when the input matrix has dimension $M \times 1$?</summary>
The top row (1 element) is processed.
The right column processes the remaining $M - 1$ elements.
Because `left > right`, the upward traversal is skipped, producing the column in top-down order.
</details>

<details>
<summary>4. Why does direction vector simulation change direction when `matrix[nr][nc] == 101`?</summary>
The problem constraints state that all elements satisfy $-100 \le \text{matrix}[i][j] \le 100$.
By overwriting visited cells with 101, the cell acts as a boundary barrier without allocating extra memory.
</details>

<details>
<summary>5. How does cache locality compare between horizontal and vertical matrix traversals?</summary>
Horizontal traversals (row major) access contiguous memory addresses, maximizing L1 data cache prefetching.
Vertical traversals jump across row strides of size $N \times \text{sizeof(int)}$, inducing potential cache misses on large matrices.
</details>

<details>
<summary>6. How many total recursive layers exist in an $M \times N$ matrix?</summary>
The total number of concentric rings is $\lceil \frac{\min(M, N)}{2} \rceil$.
</details>

<details>
<summary>7. What is the maximum stack depth of the recursive solution for an $M \times N$ matrix?</summary>
The recursion depth is bounded by $\min(M, N) / 2$, which uses $O(\min(M, N))$ auxiliary stack memory.
</details>

<details>
<summary>8. How does Spiral Matrix II differ from Spiral Matrix I?</summary>
Spiral Matrix II gives an integer $n$ and requires generating an $n \times n$ matrix filled with numbers from $1$ to $n^2$ in spiral order, applying the inverse writing procedure.
</details>

<details>
<summary>9. What is the behavior when an empty matrix `[[]]` is passed?</summary>
The initial guard condition `if (matrix.empty() || matrix[0].empty())` immediately detects a dimension of 0 and returns an empty list `[]`.
</details>

<details>
<summary>10. Can Spiral Matrix be performed in counter-clockwise order?</summary>
Yes, by reversing the sequence of traversals: top-to-bottom along left, left-to-right along bottom, bottom-to-top along right, and right-to-left along top.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/spiral-matrix.cpp)
- [Python Implementation](../Python/spiral-matrix.py)
- [Java Implementation](../Java/spiral-matrix.java)
- [TypeScript Implementation](../TypeScript/spiral-matrix.ts)
- [Go Implementation](../Golang/spiral-matrix.go)
- [Rust Implementation](../Rust/spiral-matrix.rs)

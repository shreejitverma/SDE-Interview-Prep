---
id: leetcode-0417-pacific-atlantic-water-flow
title: "LeetCode 0417: Pacific Atlantic Water Flow"
tags:
  - dsa
  - leetcode
  - graph
  - matrix
  - depth-first-search
  - breadth-first-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/pacific-atlantic-water-flow/"
---

# LeetCode 0417: Pacific Atlantic Water Flow

## 1. Problem Formalization and Constraints

There is an $m \times n$ rectangular island that borders both the Pacific Ocean and Atlantic Ocean.
The Pacific Ocean touches the island's left and top edges, and the Atlantic Ocean touches the island's right and bottom edges.

The island is partitioned into a grid of square cells.
You are given an $m \times n$ integer matrix `heights` where `heights[r][c]` represents the height above sea level of the cell at coordinate `(r, c)`.

The island receives a lot of rain, and the rain water can flow to neighboring cells directly north, south, east, and west if the neighboring cell's height is less than or equal to the current cell's height.
Water can flow from any cell adjacent to an ocean directly into that ocean.

Return a 2D list of grid coordinates `result` where `result[i] = [r_i, c_i]` denotes that rain water can flow from cell `(r_i, c_i)` to both the Pacific and Atlantic oceans.

### Constraints
- $m == \text{heights.length}$
- $n == \text{heights}[r]\text{.length}$
- $1 \le m, n \le 200$
- $0 \le \text{heights}[r][c] \le 10^5$

### Examples
- **Example 1**:
  - Input: `heights = [[1,2,2,3,5],[3,2,3,4,4],[2,4,5,3,1],[6,7,1,4,5],[5,1,1,2,4]]`
  - Output: `[[0,4],[1,3],[1,4],[2,2],[3,0],[3,1],[4,0]]`
  - Explanation: Rain water can flow from the cells listed into both the Pacific and Atlantic Oceans.
- **Example 2**:
  - Input: `heights = [[1]]`
  - Output: `[[0,0]]`
  - Explanation: The sole cell borders both oceans simultaneously.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Reverse DFS from Ocean Boundaries | $O(M \cdot N)$ | $O(M \cdot N)$ | Inverts water flow: climbs upwards from ocean shorelines into the island; computes intersection of `pacific` and `atlantic` reachable bitsets in optimal linear time. |
| **Tier 2 (Multi-Source BFS)** | Reverse Multi-Source BFS | $O(M \cdot N)$ | $O(M \cdot N)$ | Seeds two queue instances with ocean border cells; traverses level-by-level non-decreasing heights; avoids recursive call-stack overhead. |
| **Tier 3 (Disjoint Set Union)** | Union-Find with Two Virtual Ocean Nodes | $O(M \cdot N \cdot \alpha(MN))$ | $O(M \cdot N)$ | Directs edges from higher cells to equal or lower neighbors; connects borders to virtual Pacific and Atlantic nodes; checks connectivity to both ocean roots. |
| **Tier 4 (Brute Force)** | Forward Search from Each Cell | $O(M^2 \cdot N^2)$ | $O(M \cdot N)$ | Initiates a forward DFS from every individual cell $(r, c)$ to check if both oceans are reachable; catastrophic redundant exploration across $M \times N$ graph searches. |

---

## 3. Tier 1: Most Optimal Solution (Reverse DFS from Ocean Boundaries)

### 3.1 Algorithmic Mechanics and Invariant Proof

Instead of tracing water flowing downhill from all internal cells to the oceans, reverse the physical flow:
Water can flow from cell $A$ to neighboring cell $B$ if and only if $\text{heights}[A] \ge \text{heights}[B]$.
Equivalently, reversing flow means water can climb from ocean shoreline $B$ to island cell $A$ if $\text{heights}[A] \ge \text{heights}[B]$.

**Procedure**:
1. Allocate two 2D boolean grids: `pacific[m][n]` and `atlantic[m][n]`, initially false.
2. Initialize DFS along the Pacific perimeter:
   - Top row $r = 0$, all $c \in [0, n-1]$.
   - Left column $c = 0$, all $r \in [0, m-1]$.
3. Initialize DFS along the Atlantic perimeter:
   - Bottom row $r = m - 1$, all $c \in [0, n-1]$.
   - Right column $c = n - 1$, all $r \in [0, m-1]$.
4. In `dfs(r, c, prev_val, reachable)`:
   - Terminate if out of bounds, already marked reachable, or $\text{heights}[r][c] < \text{prev\_val}$.
   - Mark `reachable[r][c] = true`.
   - Traverse the 4 cardinal neighbors passing $\text{heights}[r][c]$ as the new `prev_val`.
5. Iterate through all $(i, j)$: if `pacific[i][j] && atlantic[i][j]`, append `[i, j]` to `result`.

**Invariant Proof**:
Let $P$ and $A$ denote the sets of cells bordering the Pacific and Atlantic oceans respectively.
A cell $u$ can drain water into Pacific Ocean $\iff$ there exists a directed path $u = v_1, v_2, \dots, v_k \in P$ such that $\text{heights}[v_i] \ge \text{heights}[v_{i+1}]$ for all $1 \le i < k$.
By reversing edge directions, this condition is equivalent to the existence of an uphill path $v_k, v_{k-1}, \dots, v_1 = u$ from $v_k \in P$.
Because DFS starting at $P$ explores all cells reachable via non-decreasing paths, a cell $u$ is marked true in `pacific` if and only if water can drain from $u$ into the Pacific Ocean.
By symmetry, `atlantic[u] == true` if and only if water can drain from $u$ into the Atlantic Ocean.
Consequently, $u \in \text{result} \iff \text{pacific}[u] \land \text{atlantic}[u]$.
Since each cell is visited at most once per ocean pass, the algorithm is both optimal and exact.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(M \cdot N)$ where $M$ is row count and $N$ is column count. Each cell is visited at most once during the Pacific traversal and at most once during the Atlantic traversal. The final intersection scan takes $O(M \cdot N)$ time.
- **Auxiliary Space Complexity**: $O(M \cdot N)$ for the two boolean reachability matrices and $O(M \cdot N)$ worst-case call stack depth for DFS in a serpentine matrix.

### 3.3 Implementation Details Across Target Languages

#### C++
```cpp
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

#include <vector>

using namespace std;

class Solution {
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        if (heights.empty() || heights[0].empty()) {
            return {};
        }

        const int m = heights.size();
        const int n = heights[0].size();
        vector<vector<bool>> pacific(m, vector<bool>(n, false));
        vector<vector<bool>> atlantic(m, vector<bool>(n, false));

        for (int i = 0; i < m; ++i) {
            dfs(heights, i, 0, heights[i][0], pacific);
            dfs(heights, i, n - 1, heights[i][n - 1], atlantic);
        }
        for (int j = 0; j < n; ++j) {
            dfs(heights, 0, j, heights[0][j], pacific);
            dfs(heights, m - 1, j, heights[m - 1][j], atlantic);
        }

        vector<vector<int>> result;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (pacific[i][j] && atlantic[i][j]) {
                    result.push_back({i, j});
                }
            }
        }
        return result;
    }

private:
    void dfs(const vector<vector<int>>& heights, int r, int c, int prev_height, vector<vector<bool>>& reachable) {
        if (r < 0 || r >= static_cast<int>(heights.size()) ||
            c < 0 || c >= static_cast<int>(heights[0].size()) ||
            reachable[r][c] || heights[r][c] < prev_height) {
            return;
        }

        reachable[r][c] = true;
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};
        for (int i = 0; i < 4; ++i) {
            dfs(heights, r + dr[i], c + dc[i], heights[r][c], reachable);
        }
    }
};
```

#### Python
```python
# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(M * N)
# Space: O(M * N)

from typing import List

class Solution:
    def pacificAtlantic(self, heights: List[List[int]]) -> List[List[int]]:
        if not heights or not heights[0]:
            return []

        m, n = len(heights), len(heights[0])
        pacific = [[False] * n for _ in range(m)]
        atlantic = [[False] * n for _ in range(m)]

        def dfs(r: int, c: int, reachable: List[List[bool]], prev_val: int) -> None:
            if (
                r < 0 or r >= m or
                c < 0 or c >= n or
                reachable[r][c] or
                heights[r][c] < prev_val
            ):
                return

            reachable[r][c] = True
            for dr, dc in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                dfs(r + dr, c + dc, reachable, heights[r][c])

        for i in range(m):
            dfs(i, 0, pacific, heights[i][0])
            dfs(i, n - 1, atlantic, heights[i][n - 1])
        for j in range(n):
            dfs(0, j, pacific, heights[0][j])
            dfs(m - 1, j, atlantic, heights[m - 1][j])

        result = []
        for i in range(m):
            for j in range(n):
                if pacific[i][j] and atlantic[i][j]:
                    result.append([i, j])

        return result
```

#### Java
```java
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {
    public List<List<Integer>> pacificAtlantic(int[][] heights) {
        List<List<Integer>> result = new ArrayList<>();
        if (heights == null || heights.length == 0 || heights[0].length == 0) {
            return result;
        }

        int m = heights.length;
        int n = heights[0].length;
        boolean[][] pacific = new boolean[m][n];
        boolean[][] atlantic = new boolean[m][n];

        for (int i = 0; i < m; i++) {
            dfs(heights, i, 0, heights[i][0], pacific);
            dfs(heights, i, n - 1, heights[i][n - 1], atlantic);
        }
        for (int j = 0; j < n; j++) {
            dfs(heights, 0, j, heights[0][j], pacific);
            dfs(heights, m - 1, j, heights[m - 1][j], atlantic);
        }

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacific[i][j] && atlantic[i][j]) {
                    result.add(Arrays.asList(i, j));
                }
            }
        }

        return result;
    }

    private void dfs(int[][] heights, int r, int c, int prevVal, boolean[][] reachable) {
        if (r < 0 || r >= heights.length || c < 0 || c >= heights[0].length ||
            reachable[r][c] || heights[r][c] < prevVal) {
            return;
        }

        reachable[r][c] = true;
        int[] dr = {-1, 1, 0, 0};
        int[] dc = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            dfs(heights, r + dr[i], c + dc[i], heights[r][c], reachable);
        }
    }
}
```

#### TypeScript
```typescript
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

function pacificAtlantic(heights: number[][]): number[][] {
    if (!heights || heights.length === 0 || heights[0].length === 0) {
        return [];
    }

    const m = heights.length;
    const n = heights[0].length;
    const pacific: boolean[][] = Array.from({ length: m }, () => new Array(n).fill(false));
    const atlantic: boolean[][] = Array.from({ length: m }, () => new Array(n).fill(false));

    function dfs(r: number, c: number, reachable: boolean[][], prevVal: number): void {
        if (
            r < 0 || r >= m ||
            c < 0 || c >= n ||
            reachable[r][c] ||
            heights[r][c] < prevVal
        ) {
            return;
        }

        reachable[r][c] = true;
        const dr = [-1, 1, 0, 0];
        const dc = [0, 0, -1, 1];

        for (let i = 0; i < 4; i++) {
            dfs(r + dr[i], c + dc[i], reachable, heights[r][c]);
        }
    }

    for (let i = 0; i < m; i++) {
        dfs(i, 0, pacific, heights[i][0]);
        dfs(i, n - 1, atlantic, heights[i][n - 1]);
    }
    for (let j = 0; j < n; j++) {
        dfs(0, j, pacific, heights[0][j]);
        dfs(m - 1, j, atlantic, heights[m - 1][j]);
    }

    const result: number[][] = [];
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (pacific[i][j] && atlantic[i][j]) {
                result.push([i, j]);
            }
        }
    }

    return result;
}
```

#### Go
```go
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

package main

func pacificAtlantic(heights [][]int) [][]int {
	if len(heights) == 0 || len(heights[0]) == 0 {
		return nil
	}

	m, n := len(heights), len(heights[0])
	pacific := make([][]bool, m)
	atlantic := make([][]bool, m)
	for i := range pacific {
		pacific[i] = make([]bool, n)
		atlantic[i] = make([]bool, n)
	}

	var dfs func(r, c, prevVal int, reachable [][]bool)
	dfs = func(r, c, prevVal int, reachable [][]bool) {
		if r < 0 || r >= m || c < 0 || c >= n || reachable[r][c] || heights[r][c] < prevVal {
			return
		}

		reachable[r][c] = true
		dr := []int{-1, 1, 0, 0}
		dc := []int{0, 0, -1, 1}

		for i := 0; i < 4; i++ {
			dfs(r+dr[i], c+dc[i], heights[r][c], reachable)
		}
	}

	for i := 0; i < m; i++ {
		dfs(i, 0, heights[i][0], pacific)
		dfs(i, n-1, heights[i][n-1], atlantic)
	}
	for j := 0; j < n; j++ {
		dfs(0, j, heights[0][j], pacific)
		dfs(m-1, j, heights[m-1][j], atlantic)
	}

	var result [][]int
	for i := 0; i < m; i++ {
		for j := 0; j < n; j++ {
			if pacific[i][j] && atlantic[i][j] {
				result = append(result, []int{i, j})
			}
		}
	}

	return result
}
```

#### Rust
```rust
/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

pub struct Solution;

impl Solution {
    pub fn pacific_atlantic(heights: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        if heights.is_empty() || heights[0].is_empty() {
            return Vec::new();
        }

        let m = heights.len();
        let n = heights[0].len();
        let mut pacific = vec![vec![false; n]; m];
        let mut atlantic = vec![vec![false; n]; m];

        for i in 0..m {
            Self::dfs(&heights, i, 0, heights[i][0], &mut pacific);
            Self::dfs(&heights, i, n - 1, heights[i][n - 1], &mut atlantic);
        }
        for j in 0..n {
            Self::dfs(&heights, 0, j, heights[0][j], &mut pacific);
            Self::dfs(&heights, m - 1, j, heights[m - 1][j], &mut atlantic);
        }

        let mut result = Vec::new();
        for i in 0..m {
            for j in 0..n {
                if pacific[i][j] && atlantic[i][j] {
                    result.push(vec![i as i32, j as i32]);
                }
            }
        }

        result
    }

    fn dfs(
        heights: &[Vec<i32>],
        r: usize,
        c: usize,
        prev_val: i32,
        reachable: &mut [Vec<bool>],
    ) {
        let m = heights.len();
        let n = heights[0].len();

        if reachable[r][c] || heights[r][c] < prev_val {
            return;
        }

        reachable[r][c] = true;

        let dr = [-1, 1, 0, 0];
        let dc = [0, 0, -1, 1];

        for i in 0..4 {
            let nr = r as i32 + dr[i];
            let nc = c as i32 + dc[i];

            if nr >= 0 && nr < m as i32 && nc >= 0 && nc < n as i32 {
                Self::dfs(heights, nr as usize, nc as usize, heights[r][c], reachable);
            }
        }
    }
}
```

---

## 4. Tier 2: Multi-Source Reverse BFS with Queue

### 4.1 Mechanical Description
Enqueue all Pacific perimeter cells into `queueP`, and all Atlantic perimeter cells into `queueA`.
Process BFS from `queueP`: for each popped cell $(r, c)$, inspect all 4 adjacent neighbors $(nr, nc)$; if $heights[nr][nc] \ge heights[r][c]$ and not yet marked in `pacific`, mark reachable and push to `queueP`.
Perform the identical process for `queueA`.
Finally, find the intersection of both sets.

### 4.2 Trade-offs
- Avoids risk of recursion stack overflow in deeply nested paths.
- Slightly higher memory footprint due to explicit queue node allocations.

---

## 5. Tier 3: Disjoint Set Union (Union-Find) with Two Virtual Ocean Nodes

### 5.1 Mechanical Description
Create a Disjoint Set Union (DSU) data structure with $M \cdot N + 2$ elements, where indices $M \cdot N$ and $M \cdot N + 1$ represent virtual nodes for the Pacific and Atlantic oceans.
Connect all border cells to their respective ocean virtual nodes.
For each internal cell $(r, c)$, examine its 4 neighbors; if $heights[r][c] \ge heights[nr][nc]$, union $(r, c)$ and $(nr, nc)$.
After all unions, a cell $(r, c)$ is valid if `find(cell) == find(PACIFIC)` and `find(cell) == find(ATLANTIC)`.

### 5.2 Trade-offs
- Conceptually interesting and models dynamic reachability.
- DSU handles undirected connectivity, so directional water flow requires directed acyclic graph (DAG) adjustments or reachability propagation, introducing extra complexity.

---

## 6. Tier 4: Forward Search from Every Cell (Brute Force Baseline)

### 6.1 Mechanical Description
Iterate through every coordinate $(r, c)$ in the $M \times N$ matrix.
Run a dedicated DFS or BFS starting at $(r, c)$ flowing downhill ($heights[next] \le heights[curr]$).
Track if any Pacific border and Atlantic border can be reached during the traversal.

### 6.2 Trade-offs
- Simple to understand from the problem description.
- $O(M^2 \cdot N^2)$ time complexity leads directly to Time Limit Exceeded (TLE) on grids up to $200 \times 200$.

---

## 7. Memory Layout, Cache Locality, and Systems Considerations

1. **Bitmask Compaction**: A single integer grid `visited[m][n]` can use bit 0 for Pacific and bit 1 for Atlantic, packing both visited states into 1 byte per cell to save memory and improve cache locality.
2. **Direction Vectors**: Storing delta vectors `dr[]` and `dc[]` as static compile-time constants enables loop unrolling and eliminates dynamic branching inside hot inner loops.
3. **Cache-Friendly Traversal**: Row-major traversal during matrix scanning ensures sequential memory access across contiguous arrays.

---

## 8. Failure Modes, Edge Cases, and Boundary Defense

| Failure Mode / Edge Case | Input Shape | Expected Behavior | Defensive Implementation |
|:---|:---|:---|:---|
| $1 \times 1$ matrix | `[[1]]` | Returns `[[0, 0]]` | Cell touches both ocean borders on all sides |
| Flat plateau matrix | All cells identical height | All cells can reach both oceans | Non-decreasing condition `height >= prev` permits flat plateau flow |
| Strict mountain peak | Center high, edges low | Center and paths reach both | Reverse search climbs up to the center peak |
| Island basin | Center lower than all edges | Basin cannot flow outwards | Reverse search never descends into basin from higher rims |
| Narrow $1 \times N$ strip | Single row matrix | All cells reach both | Both top (Pacific) and bottom (Atlantic) touch every cell |

---

## 9. Frequently Asked Questions (FAQ)

### 1. Why reverse the flow from oceans instead of starting from cells?
Starting from the ocean borders explores the grid in 2 linear passes ($O(M \cdot N)$), whereas starting from each cell repeats traversals $M \cdot N$ times ($O(M^2 \cdot N^2)$).

### 2. Can water flow to cells of equal height?
Yes, the problem allows water to flow to neighboring cells if height is less than or equal to current cell. In reverse, flow can move to equal or greater height.

### 3. Does DFS encounter infinite cycles on flat plateaus?
No, because each cell is marked `reachable = true` on first visit and never revisited.

### 4. Why are the borders included in the search initialization?
Every cell on the top and left edges directly touches the Pacific Ocean, and every cell on the bottom and right edges directly touches the Atlantic Ocean.

### 5. Can a corner cell reach both oceans automatically?
Yes, `(0, n-1)` and `(m-1, 0)` are always in the result set because each directly touches both oceans.

### 6. Is BFS faster than DFS for this problem?
Both have identical $O(M \cdot N)$ asymptotic complexity. DFS has less memory allocation overhead, while BFS avoids deep recursion limits.

### 7. What happens if matrix dimensions are empty?
Guards `if (heights.empty() || heights[0].empty()) return {};` safely handle empty inputs.

### 8. How does bitwise OR optimize space in C++?
Instead of two boolean matrices, an `int` matrix with bitmasks `1` (Pacific) and `2` (Atlantic) can track reachability using `visited[r][c] |= flag`.

### 9. Why does Union-Find struggle with this problem?
Water flow is directed (only downhill). Standard Union-Find computes undirected connected components, requiring extra adaptations for directed reachability.

### 10. What is the maximum matrix size under the constraints?
$M, N \le 200$, resulting in at most 40,000 cells, comfortably processing in less than 20ms under $O(M \cdot N)$ linear search.

---

## 10. Related Problems and Systematic Progression Links

- [[0079-Word-Search]]: Backtracking traversal on 2D board.
- [[0133-Clone-Graph]]: Graph node exploration and reachability.
- [[0200-Number-of-Islands]]: Connected component exploration in a 2D grid.
- LeetCode 130 (Surrounded Regions): Boundary-connected graph search.
- LeetCode 1020 (Number of Enclaves): Boundary-connected grid component counting.
- LeetCode 1091 (Shortest Path in Binary Matrix): BFS shortest path on 2D grid.

---

## 11. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/pacific-atlantic-water-flow.cpp)
- [Python Implementation](../Python/pacific-atlantic-water-flow.py)
- [Java Implementation](../Java/pacific-atlantic-water-flow.java)
- [TypeScript Implementation](../TypeScript/pacific-atlantic-water-flow.ts)
- [Go Implementation](../Golang/pacific-atlantic-water-flow.go)
- [Rust Implementation](../Rust/pacific-atlantic-water-flow.rs)

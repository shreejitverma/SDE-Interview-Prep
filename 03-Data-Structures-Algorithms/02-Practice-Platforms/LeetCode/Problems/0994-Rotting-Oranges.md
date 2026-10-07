---
id: leetcode-0994-rotting-oranges
title: "LeetCode 0994: Rotting Oranges"
tags:
  - dsa
  - leetcode
  - array
  - matrix
  - breadth-first-search
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/rotting-oranges/"
---

# LeetCode 0994: Rotting Oranges

## 1. Problem Formalization and Constraints

You are given an $m \times n$ grid where each cell can have one of three values:
- `0` representing an empty cell,
- `1` representing a fresh orange, or
- `2` representing a rotten orange.
Every minute, any fresh orange that is 4-directionally adjacent to a rotten orange becomes rotten.
Return the minimum number of minutes that must elapse until no cell has a fresh orange.
If this is impossible, return `-1`.

### Constraints
- $m == \text{grid.length}$
- $n == \text{grid}[i]\text{.length}$
- $1 \le m, n \le 10$
- $\text{grid}[i][j]$ is `0`, `1`, or `2`.

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | Multi-Source Breadth-First Search | $O(M \times N)$ | $O(M \times N)$ | Enqueues all initially rotten cells; expands level-by-level simultaneously. |
| **Tier 2 (In-Place Timestamping)** | Timestamp In-Place Matrix Marking | $O(M \times N)$ | $O(M \times N)$ | Encodes minute of infection in grid value $2 + \text{minute}$; zero extra visited grid. |
| **Tier 3 (Iterative Round Simulation)** | Grid Matrix Brute Simulation | $O((M \times N)^2)$ | $O(M \times N)$ | Copies grid each minute; marks newly rotting cells until quiescence. |
| **Tier 4 (Single-Source BFS per Fresh)** | All-Pairs Fresh-to-Rotten Distance | $O((M \times N)^2)$ | $O(M \times N)$ | For every fresh orange, computes BFS distance to nearest rotten orange. |

---

## 3. Tier 1: Most Optimal Solution (Multi-Source Breadth-First Search)

### 3.1 Algorithmic Mechanics and Invariant Proof

Because infection spreads concurrently from all existing rotten oranges in unit time, this is an unweighted shortest path problem from multiple sources to all reachable nodes:
1. Enqueue all initial rotten coordinates $(r, c)$ into a FIFO queue.
2. Count the total number of initial fresh oranges: $\text{fresh}$.
3. If $\text{fresh} == 0$, return 0 immediately (zero minutes required).
4. While the queue is non-empty and $\text{fresh} > 0$:
   - For every node at the current level, inspect its 4 orthogonal neighbors.
   - If a neighbor $(nr, nc)$ contains a fresh orange (`grid[nr][nc] == 1`), mutate it to rotten (`grid[nr][nc] = 2`), decrement $\text{fresh}$, and push $(nr, nc)$ onto the queue.
   - After exhausting the current level, increment elapsed minutes by 1.
5. If $\text{fresh} == 0$, return elapsed minutes; otherwise, at least one fresh orange was disconnected from all infection sources, so return -1.
Because each cell is visited and added to the queue at most once, the algorithm executes in strict $O(M \times N)$ time.

### 3.2 Implementations Across 6 Languages

#### C++
```cpp
class Solution {
public:
    int orangesRotting(std::vector<std::vector<int>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        const int m = static_cast<int>(grid.size());
        const int n = static_cast<int>(grid[0].size());
        std::queue<std::pair<int, int>> q;
        int fresh = 0;

        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == 2) {
                    q.emplace(r, c);
                } else if (grid[r][c] == 1) {
                    ++fresh;
                }
            }
        }

        if (fresh == 0) return 0;

        int minutes = 0;
        static const int dr[4] = {-1, 1, 0, 0};
        static const int dc[4] = {0, 0, -1, 1};

        while (!q.empty() && fresh > 0) {
            const size_t sz = q.size();
            for (size_t i = 0; i < sz; ++i) {
                const auto [r, c] = q.front();
                q.pop();

                for (int d = 0; d < 4; ++d) {
                    const int nr = r + dr[d];
                    const int nc = c + dc[d];
                    if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == 1) {
                        grid[nr][nc] = 2;
                        --fresh;
                        q.emplace(nr, nc);
                    }
                }
            }
            ++minutes;
        }

        return (fresh == 0) ? minutes : -1;
    }
};
```

#### Python
```python
from collections import deque

class Solution:
    def orangesRotting(self, grid: list[list[int]]) -> int:
        if not grid or not grid[0]:
            return 0

        m, n = len(grid), len(grid[0])
        queue = deque()
        fresh = 0

        for r in range(m):
            for c in range(n):
                if grid[r][c] == 2:
                    queue.append((r, c))
                elif grid[r][c] == 1:
                    fresh += 1

        if fresh == 0:
            return 0

        minutes = 0
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

        while queue and fresh > 0:
            for _ in range(len(queue)):
                r, c = queue.popleft()
                for dr, dc in directions:
                    nr, nc = r + dr, c + dc
                    if 0 <= nr < m and 0 <= nc < n and grid[nr][nc] == 1:
                        grid[nr][nc] = 2
                        fresh -= 1
                        queue.append((nr, nc))
            minutes += 1

        return minutes if fresh == 0 else -1
```

#### Java
```java
import java.util.ArrayDeque;
import java.util.Queue;

class Solution {
    public int orangesRotting(int[][] grid) {
        if (grid == null || grid.length == 0 || grid[0].length == 0) return 0;
        int m = grid.length, n = grid[0].length;
        Queue<int[]> queue = new ArrayDeque<>();
        int fresh = 0;

        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == 2) {
                    queue.offer(new int[]{r, c});
                } else if (grid[r][c] == 1) {
                    fresh++;
                }
            }
        }

        if (fresh == 0) return 0;

        int minutes = 0;
        int[] dr = {-1, 1, 0, 0};
        int[] dc = {0, 0, -1, 1};

        while (!queue.isEmpty() && fresh > 0) {
            int sz = queue.size();
            for (int i = 0; i < sz; i++) {
                int[] cell = queue.poll();
                int r = cell[0], c = cell[1];

                for (int d = 0; d < 4; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == 1) {
                        grid[nr][nc] = 2;
                        fresh--;
                        queue.offer(new int[]{nr, nc});
                    }
                }
            }
            minutes++;
        }

        return (fresh == 0) ? minutes : -1;
    }
}
```

#### TypeScript
```typescript
function orangesRotting(grid: number[][]): number {
    if (grid.length === 0 || grid[0].length === 0) return 0;
    const m = grid.length, n = grid[0].length;
    const queue: [number, number][] = [];
    let fresh = 0;

    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (grid[r][c] === 2) {
                queue.push([r, c]);
            } else if (grid[r][c] === 1) {
                fresh++;
            }
        }
    }

    if (fresh === 0) return 0;

    let minutes = 0;
    const dr = [-1, 1, 0, 0];
    const dc = [0, 0, -1, 1];
    let head = 0;

    while (head < queue.length && fresh > 0) {
        const size = queue.length - head;
        for (let i = 0; i < size; i++) {
            const [r, c] = queue[head++];
            for (let d = 0; d < 4; d++) {
                const nr = r + dr[d], nc = c + dc[d];
                if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] === 1) {
                    grid[nr][nc] = 2;
                    fresh--;
                    queue.push([nr, nc]);
                }
            }
        }
        minutes++;
    }

    return fresh === 0 ? minutes : -1;
}
```

#### Golang
```go
package main

type point struct {
	r, c int
}

func orangesRotting(grid [][]int) int {
	if len(grid) == 0 || len(grid[0]) == 0 {
		return 0
	}
	m, n := len(grid), len(grid[0])
	queue := make([]point, 0)
	fresh := 0

	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			if grid[r][c] == 2 {
				queue = append(queue, point{r, c})
			} else if grid[r][c] == 1 {
				fresh++
			}
		}
	}

	if fresh == 0 {
		return 0
	}

	minutes := 0
	dr := []int{-1, 1, 0, 0}
	dc := []int{0, 0, -1, 1}

	for len(queue) > 0 && fresh > 0 {
		sz := len(queue)
		for i := 0; i < sz; i++ {
			curr := queue[0]
			queue = queue[1:]

			for d := 0; d < 4; d++ {
				nr := curr.r + dr[d]
				nc := curr.c + dc[d]
				if nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == 1 {
					grid[nr][nc] = 2
					fresh--
					queue = append(queue, point{nr, nc})
				}
			}
		}
		minutes++
	}

	if fresh == 0 {
		return minutes
	}
	return -1
}
```

#### Rust
```rust
use std::collections::VecDeque;

pub struct Solution;

impl Solution {
    pub fn oranges_rotting(mut grid: Vec<Vec<i32>>) -> i32 {
        if grid.is_empty() || grid[0].is_empty() {
            return 0;
        }
        let m = grid.len();
        let n = grid[0].len();
        let mut queue: VecDeque<(usize, usize)> = VecDeque::new();
        let mut fresh = 0;

        for r in 0..m {
            for c in 0..n {
                if grid[r][c] == 2 {
                    queue.push_back((r, c));
                } else if grid[r][c] == 1 {
                    fresh += 1;
                }
            }
        }

        if fresh == 0 {
            return 0;
        }

        let mut minutes = 0;
        let directions: [(isize, isize); 4] = [(-1, 0), (1, 0), (0, -1), (0, 1)];

        while !queue.is_empty() && fresh > 0 {
            let sz = queue.len();
            for _ in 0..sz {
                let (r, c) = queue.pop_front().unwrap();
                for &(dr, dc) in &directions {
                    let nr = r as isize + dr;
                    let nc = c as isize + dc;
                    if nr >= 0 && (nr as usize) < m && nc >= 0 && (nc as usize) < n {
                        let ur = nr as usize;
                        let uc = nc as usize;
                        if grid[ur][uc] == 1 {
                            grid[ur][uc] = 2;
                            fresh -= 1;
                            queue.push_back((ur, uc));
                        }
                    }
                }
            }
            minutes += 1;
        }

        if fresh == 0 { minutes } else { -1 }
    }
}
```

---

## 4. Complexity and Performance Analysis

- **Time Complexity**: $O(M \times N)$ since each matrix cell enters and leaves the BFS queue at most once.
- **Space Complexity**: $O(M \times N)$ auxiliary memory for the BFS queue in the worst-case scenario.
- **In-Place Mutation**: Mutating the grid from 1 to 2 avoids allocating a boolean `visited` matrix.

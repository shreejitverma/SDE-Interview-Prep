---
id: leetcode-0200-number-of-islands
title: "LeetCode 0200: Number of Islands"
tags:
  - dsa
  - leetcode
  - graph
  - dfs
  - bfs
  - union-find
level: medium
type: problem
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://leetcode.com/problems/number-of-islands/"
---

# LeetCode 0200: Number of Islands

## 1. Problem Formalization and Constraints

Given an $m \times n$ 2D binary grid `grid` which represents a map of `'1'`s (land) and `'0'`s (water), return the number of islands.
An island is surrounded by water and is formed by connecting adjacent lands horizontally or vertically.
You may assume all four edges of the grid are all surrounded by water.

### Constraints
- $m == \text{grid.length}$
- $n == \text{grid}[i]\text{.length}$
- $1 \le m, n \le 300$
- `grid[i][j]` is `'0'` or `'1'`.

### Examples
- **Example 1**:
  - Input:
    ```
    grid = [
      ["1","1","1","1","0"],
      ["1","1","0","1","0"],
      ["1","1","0","0","0"],
      ["0","0","0","0","0"]
    ]
    ```
  - Output: `1`
- **Example 2**:
  - Input:
    ```
    grid = [
      ["1","1","0","0","0"],
      ["1","1","0","0","0"],
      ["0","0","1","0","0"],
      ["0","0","0","1","1"]
    ]
    ```
  - Output: `3`

---

## 2. Solution Architecture and Progression Roadmap

| Tier | Strategy | Time Complexity | Auxiliary Space | Key Mechanism & Trade-offs |
|:---|:---|:---:|:---:|:---|
| **Tier 1 (Optimal)** | In-Place DFS Flood Fill ("Sink the Island") | $O(M \times N)$ | $O(M \times N)$ stack | Sinks land cells by overwriting `'1'` $\to$ `'0'` during depth-first traversal; zero auxiliary buffers. |
| **Tier 2 (Space-Optimized Alternative)** | Disjoint Set Union (Union-Find with Rank & Path Compression) | $O(M \times N \cdot \alpha(MN))$ | $O(M \times N)$ | Merges connected component sets dynamically; supports streaming graph inputs. |
| **Tier 3 (Time-Optimized Alternative)** | Iterative Breadth-First Search (BFS) Queue | $O(M \times N)$ | $O(\min(M, N))$ | Traverses connected components level-by-level; bounds queue size by grid perimeter. |
| **Tier 4 (Brute Force)** | External Visited Matrix Graph Traversal | $O(M \times N)$ | $O(M \times N)$ extra | Allocates a dedicated boolean matrix without mutating the input grid. |

---

## 3. Tier 1: Most Optimal Solution (In-Place DFS Flood Fill)

### 3.1 Algorithmic Mechanics and Invariant Proof

We scan every cell $(r, c)$ in the $M \times N$ grid:
1. When we encounter land (`grid[r][c] == '1'`), we increment the island counter `count++`.
2. We immediately trigger a depth-first search (`dfs(r, c)`) to sink the connected island:
   - Base case: If $(r, c)$ is out of bounds or `grid[r][c] != '1'`, return.
   - Mark as visited in-place: Set `grid[r][c] = '0'`.
   - Recursively visit all 4 orthogonal neighbors: $(r+1, c)$, $(r-1, c)$, $(r, c+1)$, $(r, c-1)$.

**Connected Component Invariant**:
Every connected component of land is traversed exactly once.
Because each land cell is mutated to `'0'` upon first discovery, no cell can be counted multiple times, and subsequent scans bypass sunken cells in $O(1)$ time.

### 3.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$. Every cell is visited at most a constant number of times (once by the outer loop, and once by DFS neighbor probes).
- **Space Complexity**: $O(M \times N)$ in the worst-case recursion call stack (e.g., a serpentine grid filled entirely with land).

### 3.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
    void dfs(std::vector<std::vector<char>>& grid, int r, int c, int m, int n) {
        if (r < 0 || r >= m || c < 0 || c >= n || grid[r][c] != '1') {
            return;
        }
        grid[r][c] = '0';
        dfs(grid, r + 1, c, m, n);
        dfs(grid, r - 1, c, m, n);
        dfs(grid, r, c + 1, m, n);
        dfs(grid, r, c - 1, m, n);
    }
public:
    int numIslands(std::vector<std::vector<char>>& grid) {
        if (grid.empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int count = 0;
        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '1') {
                    ++count;
                    dfs(grid, r, c, m, n);
                }
            }
        }
        return count;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def numIslands(self, grid: List[List[str]]) -> int:
        if not grid:
            return 0
        m, n = len(grid), len(grid[0])
        count = 0

        def dfs(r: int, c: int) -> None:
            if r < 0 or r >= m or c < 0 or c >= n or grid[r][c] != '1':
                return
            grid[r][c] = '0'
            dfs(r + 1, c)
            dfs(r - 1, c)
            dfs(r, c + 1)
            dfs(r, c - 1)

        for r in range(m):
            for c in range(n):
                if grid[r][c] == '1':
                    count += 1
                    dfs(r, c)
        return count
```

#### Java 21
```java
class Solution {
    public int numIslands(char[][] grid) {
        if (grid == null || grid.length == 0) return 0;
        int rows = grid.length;
        int cols = grid[0].length;
        int count = 0;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (grid[r][c] == '1') {
                    count++;
                    dfs(grid, r, c, rows, cols);
                }
            }
        }
        return count;
    }

    private void dfs(char[][] grid, int r, int c, int rows, int cols) {
        if (r < 0 || r >= rows || c < 0 || c >= cols || grid[r][c] != '1') {
            return;
        }
        grid[r][c] = '0';
        dfs(grid, r + 1, c, rows, cols);
        dfs(grid, r - 1, c, rows, cols);
        dfs(grid, r, c + 1, rows, cols);
        dfs(grid, r, c - 1, rows, cols);
    }
}
```

#### TypeScript
```typescript
function numIslands(grid: string[][]): number {
    if (!grid || grid.length === 0) return 0;
    const rows = grid.length;
    const cols = grid[0].length;
    let count = 0;

    function dfs(r: number, c: number): void {
        if (r < 0 || r >= rows || c < 0 || c >= cols || grid[r][c] !== '1') {
            return;
        }
        grid[r][c] = '0';
        dfs(r + 1, c);
        dfs(r - 1, c);
        dfs(r, c + 1);
        dfs(r, c - 1);
    }

    for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
            if (grid[r][c] === '1') {
                count++;
                dfs(r, c);
            }
        }
    }
    return count;
}
```

#### Go
```go
package main

func numIslands(grid [][]byte) int {
    if len(grid) == 0 {
        return 0
    }
    rows := len(grid)
    cols := len(grid[0])
    count := 0

    var dfs func(r, c int)
    dfs = func(r, c int) {
        if r < 0 || r >= rows || c < 0 || c >= cols || grid[r][c] != '1' {
            return
        }
        grid[r][c] = '0'
        dfs(r+1, c)
        dfs(r-1, c)
        dfs(r, c+1)
        dfs(r, c-1)
    }

    for r := 0; r < rows; r++ {
        for c := 0; c < cols; c++ {
            if grid[r][c] == '1' {
                count++
                dfs(r, c)
            }
        }
    }
    return count
}
```

#### Rust
```rust
impl Solution {
    pub fn num_islands(mut grid: Vec<Vec<char>>) -> i32 {
        if grid.is_empty() {
            return 0;
        }
        let rows = grid.len();
        let cols = grid[0].len();
        let mut count = 0;

        for r in 0..rows {
            for c in 0..cols {
                if grid[r][c] == '1' {
                    count += 1;
                    Self::dfs(&mut grid, r, c, rows, cols);
                }
            }
        }
        count
    }

    fn dfs(grid: &mut Vec<Vec<char>>, r: usize, c: usize, rows: usize, cols: usize) {
        if grid[r][c] != '1' {
            return;
        }
        grid[r][c] = '0';
        if r + 1 < rows {
            Self::dfs(grid, r + 1, c, rows, cols);
        }
        if r > 0 {
            Self::dfs(grid, r - 1, c, rows, cols);
        }
        if c + 1 < cols {
            Self::dfs(grid, r, c + 1, rows, cols);
        }
        if c > 0 {
            Self::dfs(grid, r, c - 1, rows, cols);
        }
    }
}
```

---

## 4. Tier 2: Space-Optimized Alternative (Disjoint Set Union / Union-Find)

### 4.1 Algorithmic Mechanics

We flatten 2D coordinates into 1D integers: $\text{id}(r, c) = r \times N + c$.
We initialize a Disjoint Set Union (DSU) structure where each land cell starts as its own root.
We scan the grid and union adjacent land cells (right $(r, c+1)$ and down $(r+1, c)$):
Whenever two disjoint land components are united, we decrement the total component count.

### 4.2 Complexity Analysis
- **Time Complexity**: $O(M \times N \cdot \alpha(MN))$ where $\alpha$ is the Inverse Ackermann function ($\alpha \le 4$).
- **Space Complexity**: $O(M \times N)$ storing parent and rank arrays.

### 4.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <numeric>

class UnionFind {
    std::vector<int> parent;
    std::vector<int> rank;
    int count;
public:
    UnionFind(const std::vector<std::vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        count = 0;
        parent.resize(m * n);
        rank.resize(m * n, 0);
        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '1') {
                    int id = r * n + c;
                    parent[id] = id;
                    ++count;
                }
            }
        }
    }

    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }

    void unite(int x, int y) {
        int rootX = find(x), rootY = find(y);
        if (rootX != rootY) {
            if (rank[rootX] < rank[rootY]) std::swap(rootX, rootY);
            parent[rootY] = rootX;
            if (rank[rootX] == rank[rootY]) ++rank[rootX];
            --count;
        }
    }

    int getCount() const { return count; }
};

class Solution {
public:
    int numIslands(const std::vector<std::vector<char>>& grid) {
        if (grid.empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        UnionFind uf(grid);
        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '1') {
                    int id = r * n + c;
                    if (r + 1 < m && grid[r + 1][c] == '1') uf.unite(id, (r + 1) * n + c);
                    if (c + 1 < n && grid[r][c + 1] == '1') uf.unite(id, r * n + (c + 1));
                }
            }
        }
        return uf.getCount();
    }
};
```

#### Python 3.12
```python
from typing import List

class UnionFind:
    def __init__(self, grid: List[List[str]]):
        m, n = len(grid), len(grid[0])
        self.parent = {}
        self.rank = {}
        self.count = 0
        for r in range(m):
            for c in range(n):
                if grid[r][c] == '1':
                    idx = r * n + c
                    self.parent[idx] = idx
                    self.rank[idx] = 0
                    self.count += 1

    def find(self, i: int) -> int:
        if self.parent[i] == i:
            return i
        self.parent[i] = self.find(self.parent[i])
        return self.parent[i]

    def union(self, x: int, y: int) -> None:
        root_x = self.find(x)
        root_y = self.find(y)
        if root_x != root_y:
            if self.rank[root_x] < self.rank[root_y]:
                root_x, root_y = root_y, root_x
            self.parent[root_y] = root_x
            if self.rank[root_x] == self.rank[root_y]:
                self.rank[root_x] += 1
            self.count -= 1

class Solution:
    def numIslands(self, grid: List[List[str]]) -> int:
        if not grid:
            return 0
        m, n = len(grid), len(grid[0])
        uf = UnionFind(grid)
        for r in range(m):
            for c in range(n):
                if grid[r][c] == '1':
                    idx = r * n + c
                    if r + 1 < m and grid[r + 1][c] == '1':
                        uf.union(idx, (r + 1) * n + c)
                    if c + 1 < n and grid[r][c + 1] == '1':
                        uf.union(idx, r * n + (c + 1))
        return uf.count
```

#### Java 21
```java
class Solution {
    static class UnionFind {
        int[] parent;
        int[] rank;
        int count;

        UnionFind(char[][] grid) {
            int m = grid.length, n = grid[0].length;
            count = 0;
            parent = new int[m * n];
            rank = new int[m * n];
            for (int r = 0; r < m; r++) {
                for (int c = 0; c < n; c++) {
                    if (grid[r][c] == '1') {
                        int id = r * n + c;
                        parent[id] = id;
                        count++;
                    }
                }
            }
        }

        int find(int i) {
            if (parent[i] == i) return i;
            return parent[i] = find(parent[i]);
        }

        void union(int x, int y) {
            int rootX = find(x), rootY = find(y);
            if (rootX != rootY) {
                if (rank[rootX] < rank[rootY]) {
                    int tmp = rootX; rootX = rootY; rootY = tmp;
                }
                parent[rootY] = rootX;
                if (rank[rootX] == rank[rootY]) rank[rootX]++;
                count--;
            }
        }
    }

    public int numIslands(char[][] grid) {
        if (grid == null || grid.length == 0) return 0;
        int m = grid.length, n = grid[0].length;
        UnionFind uf = new UnionFind(grid);
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == '1') {
                    int id = r * n + c;
                    if (r + 1 < m && grid[r + 1][c] == '1') uf.union(id, (r + 1) * n + c);
                    if (c + 1 < n && grid[r][c + 1] == '1') uf.union(id, r * n + (c + 1));
                }
            }
        }
        return uf.count;
    }
}
```

#### TypeScript
```typescript
class UnionFind {
    parent: Int32Array;
    rank: Int8Array;
    count: number;

    constructor(grid: string[][]) {
        const m = grid.length, n = grid[0].length;
        this.count = 0;
        this.parent = new Int32Array(m * n);
        this.rank = new Int8Array(m * n);
        for (let r = 0; r < m; r++) {
            for (let c = 0; c < n; c++) {
                if (grid[r][c] === '1') {
                    const id = r * n + c;
                    this.parent[id] = id;
                    this.count++;
                }
            }
        }
    }

    find(i: number): number {
        if (this.parent[i] === i) return i;
        this.parent[i] = this.find(this.parent[i]);
        return this.parent[i];
    }

    union(x: number, y: number): void {
        let rootX = this.find(x), rootY = this.find(y);
        if (rootX !== rootY) {
            if (this.rank[rootX] < this.rank[rootY]) {
                const tmp = rootX; rootX = rootY; rootY = tmp;
            }
            this.parent[rootY] = rootX;
            if (this.rank[rootX] === this.rank[rootY]) this.rank[rootX]++;
            this.count--;
        }
    }
}

function numIslands(grid: string[][]): number {
    if (!grid || grid.length === 0) return 0;
    const m = grid.length, n = grid[0].length;
    const uf = new UnionFind(grid);
    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (grid[r][c] === '1') {
                const id = r * n + c;
                if (r + 1 < m && grid[r + 1][c] === '1') uf.union(id, (r + 1) * n + c);
                if (c + 1 < n && grid[r][c + 1] === '1') uf.union(id, r * n + (c + 1));
            }
        }
    }
    return uf.count;
}
```

#### Go
```go
package main

type unionFind struct {
    parent []int
    rank   []int
    count  int
}

func newUF(grid [][]byte) *unionFind {
    m, n := len(grid), len(grid[0])
    uf := &unionFind{
        parent: make([]int, m*n),
        rank:   make([]int, m*n),
        count:  0,
    }
    for r := 0; r < m; r++ {
        for c := 0; c < n; c++ {
            if grid[r][c] == '1' {
                id := r*n + c
                uf.parent[id] = id
                uf.count++
            }
        }
    }
    return uf
}

func (uf *unionFind) find(i int) int {
    if uf.parent[i] == i {
        return i
    }
    uf.parent[i] = uf.find(uf.parent[i])
    return uf.parent[i]
}

func (uf *unionFind) union(x, y int) {
    rootX, rootY := uf.find(x), uf.find(y)
    if rootX != rootY {
        if uf.rank[rootX] < uf.rank[rootY] {
            rootX, rootY = rootY, rootX
        }
        uf.parent[rootY] = rootX
        if uf.rank[rootX] == uf.rank[rootY] {
            uf.rank[rootX]++
        }
        uf.count--
    }
}

func numIslands(grid [][]byte) int {
    if len(grid) == 0 {
        return 0
    }
    m, n := len(grid), len(grid[0])
    uf := newUF(grid)
    for r := 0; r < m; r++ {
        for c := 0; c < n; c++ {
            if grid[r][c] == '1' {
                id := r*n + c
                if r+1 < m && grid[r+1][c] == '1' {
                    uf.union(id, (r+1)*n+c)
                }
                if c+1 < n && grid[r][c+1] == '1' {
                    uf.union(id, r*n+(c+1))
                }
            }
        }
    }
    return uf.count
}
```

#### Rust
```rust
struct UnionFind {
    parent: Vec<usize>,
    rank: Vec<u8>,
    count: i32,
}

impl UnionFind {
    fn new(grid: &[Vec<char>]) -> Self {
        let m = grid.len();
        let n = grid[0].len();
        let mut parent = vec![0; m * n];
        let mut count = 0;
        for r in 0..m {
            for c in 0..n {
                if grid[r][c] == '1' {
                    let id = r * n + c;
                    parent[id] = id;
                    count += 1;
                }
            }
        }
        UnionFind {
            parent,
            rank: vec![0; m * n],
            count,
        }
    }

    fn find(&mut self, i: usize) -> usize {
        if self.parent[i] == i {
            i
        } else {
            let p = self.parent[i];
            self.parent[i] = self.find(p);
            self.parent[i]
        }
    }

    fn union(&mut self, x: usize, y: usize) {
        let mut root_x = self.find(x);
        let mut root_y = self.find(y);
        if root_x != root_y {
            if self.rank[root_x] < self.rank[root_y] {
                std::mem::swap(&mut root_x, &mut root_y);
            }
            self.parent[root_y] = root_x;
            if self.rank[root_x] == self.rank[root_y] {
                self.rank[root_x] += 1;
            }
            self.count -= 1;
        }
    }
}

impl Solution {
    pub fn num_islands(grid: Vec<Vec<char>>) -> i32 {
        if grid.is_empty() {
            return 0;
        }
        let m = grid.len();
        let n = grid[0].len();
        let mut uf = UnionFind::new(&grid);
        for r in 0..m {
            for c in 0..n {
                if grid[r][c] == '1' {
                    let id = r * n + c;
                    if r + 1 < m && grid[r + 1][c] == '1' {
                        uf.union(id, (r + 1) * n + c);
                    }
                    if c + 1 < n && grid[r][c + 1] == '1' {
                        uf.union(id, r * n + (c + 1));
                    }
                }
            }
        }
        uf.count
    }
}
```

---

## 5. Tier 3: Time-Optimized Alternative (Iterative BFS Queue)

### 5.1 Algorithmic Mechanics

Rather than recursive DFS call stack depth, we use an iterative FIFO queue.
When visiting $(r, c)$ with land:
1. Enqueue $(r, c)$ and immediately sink it: `grid[r][c] = '0'`.
2. While the queue is non-empty, dequeue $(currR, currC)$ and enqueue any of its 4 neighbors that contain `'1'`, marking them `'0'` immediately upon enqueue.

### 5.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$ linear operations.
- **Space Complexity**: $O(\min(M, N))$ maximum queue length bounded by the perimeter of the active wave.

### 5.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>
#include <queue>

class Solution {
public:
    int numIslands(std::vector<std::vector<char>>& grid) {
        if (grid.empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int count = 0;
        std::vector<std::pair<int, int>> dirs = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '1') {
                    ++count;
                    grid[r][c] = '0';
                    std::queue<std::pair<int, int>> q;
                    q.push({r, c});
                    while (!q.empty()) {
                        auto [currR, currC] = q.front();
                        q.pop();
                        for (auto [dr, dc] : dirs) {
                            int nr = currR + dr, nc = currC + dc;
                            if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == '1') {
                                grid[nr][nc] = '0';
                                q.push({nr, nc});
                            }
                        }
                    }
                }
            }
        }
        return count;
    }
};
```

#### Python 3.12
```python
from typing import List
from collections import deque

class Solution:
    def numIslands(self, grid: List[List[str]]) -> int:
        if not grid:
            return 0
        m, n = len(grid), len(grid[0])
        count = 0
        dirs = [(1, 0), (-1, 0), (0, 1), (0, -1)]
        for r in range(m):
            for c in range(n):
                if grid[r][c] == '1':
                    count += 1
                    grid[r][c] = '0'
                    q = deque([(r, c)])
                    while q:
                        curr_r, curr_c = q.popleft()
                        for dr, dc in dirs:
                            nr, nc = curr_r + dr, curr_c + dc
                            if 0 <= nr < m and 0 <= nc < n and grid[nr][nc] == '1':
                                grid[nr][nc] = '0'
                                q.append((nr, nc))
        return count
```

#### Java 21
```java
import java.util.ArrayDeque;
import java.util.Queue;

class Solution {
    public int numIslands(char[][] grid) {
        if (grid == null || grid.length == 0) return 0;
        int m = grid.length, n = grid[0].length;
        int count = 0;
        int[][] dirs = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == '1') {
                    count++;
                    grid[r][c] = '0';
                    Queue<int[]> q = new ArrayDeque<>();
                    q.offer(new int[]{r, c});
                    while (!q.isEmpty()) {
                        int[] curr = q.poll();
                        for (int[] d : dirs) {
                            int nr = curr[0] + d[0], nc = curr[1] + d[1];
                            if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == '1') {
                                grid[nr][nc] = '0';
                                q.offer(new int[]{nr, nc});
                            }
                        }
                    }
                }
            }
        }
        return count;
    }
}
```

#### TypeScript
```typescript
function numIslands(grid: string[][]): number {
    if (!grid || grid.length === 0) return 0;
    const m = grid.length, n = grid[0].length;
    let count = 0;
    const dirs = [[1, 0], [-1, 0], [0, 1], [0, -1]];
    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (grid[r][c] === '1') {
                count++;
                grid[r][c] = '0';
                const queue: number[] = [r * n + c];
                let head = 0;
                while (head < queue.length) {
                    const id = queue[head++];
                    const currR = Math.floor(id / n);
                    const currC = id % n;
                    for (const [dr, dc] of dirs) {
                        const nr = currR + dr, nc = currC + dc;
                        if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] === '1') {
                            grid[nr][nc] = '0';
                            queue.push(nr * n + nc);
                        }
                    }
                }
            }
        }
    }
    return count;
}
```

#### Go
```go
package main

func numIslands(grid [][]byte) int {
    if len(grid) == 0 {
        return 0
    }
    m, n := len(grid), len(grid[0])
    count := 0
    dirs := [][2]int{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
    for r := 0; r < m; r++ {
        for c := 0; c < n; c++ {
            if grid[r][c] == '1' {
                count++
                grid[r][c] = '0'
                queue := [][2]int{{r, c}}
                for len(queue) > 0 {
                    curr := queue[0]
                    queue = queue[1:]
                    for _, d := range dirs {
                        nr, nc := curr[0]+d[0], curr[1]+d[1]
                        if nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == '1' {
                            grid[nr][nc] = '0'
                            queue = append(queue, [2]int{nr, nc})
                        }
                    }
                }
            }
        }
    }
    return count
}
```

#### Rust
```rust
use std::collections::VecDeque;

impl Solution {
    pub fn num_islands(mut grid: Vec<Vec<char>>) -> i32 {
        if grid.is_empty() {
            return 0;
        }
        let m = grid.len();
        let n = grid[0].len();
        let mut count = 0;
        let dirs: [(i32, i32); 4] = [(1, 0), (-1, 0), (0, 1), (0, -1)];
        for r in 0..m {
            for c in 0..n {
                if grid[r][c] == '1' {
                    count += 1;
                    grid[r][c] = '0';
                    let mut q = VecDeque::new();
                    q.push_back((r, c));
                    while let Some((curr_r, curr_c)) = q.pop_front() {
                        for (dr, dc) in dirs {
                            let nr = curr_r as i32 + dr;
                            let nc = curr_c as i32 + dc;
                            if nr >= 0 && nr < m as i32 && nc >= 0 && nc < n as i32 {
                                let (unr, unc) = (nr as usize, nc as usize);
                                if grid[unr][unc] == '1' {
                                    grid[unr][unc] = '0';
                                    q.push_back((unr, unc));
                                }
                            }
                        }
                    }
                }
            }
        }
        count
    }
}
```

---

## 6. Tier 4: Brute-Force Solution (External Visited Matrix Traversal)

### 6.1 Algorithmic Mechanics

When modifying the input grid is forbidden (read-only input constraints), we allocate an auxiliary 2D boolean matrix `visited[m][n]`.
DFS explores connected land components using the visited flags without mutating `grid`.

### 6.2 Complexity Analysis
- **Time Complexity**: $O(M \times N)$ operations.
- **Space Complexity**: $O(M \times N)$ auxiliary memory for the visited matrix.

### 6.3 Multi-Language Implementations

#### C++20
```cpp
#include <vector>

class Solution {
    void dfs(const std::vector<std::vector<char>>& grid, std::vector<std::vector<bool>>& visited, int r, int c, int m, int n) {
        if (r < 0 || r >= m || c < 0 || c >= n || grid[r][c] != '1' || visited[r][c]) {
            return;
        }
        visited[r][c] = true;
        dfs(grid, visited, r + 1, c, m, n);
        dfs(grid, visited, r - 1, c, m, n);
        dfs(grid, visited, r, c + 1, m, n);
        dfs(grid, visited, r, c - 1, m, n);
    }
public:
    int numIslands(const std::vector<std::vector<char>>& grid) {
        if (grid.empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        std::vector<std::vector<bool>> visited(m, std::vector<bool>(n, false));
        int count = 0;
        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '1' && !visited[r][c]) {
                    ++count;
                    dfs(grid, visited, r, c, m, n);
                }
            }
        }
        return count;
    }
};
```

#### Python 3.12
```python
from typing import List

class Solution:
    def numIslands(self, grid: List[List[str]]) -> int:
        if not grid:
            return 0
        m, n = len(grid), len(grid[0])
        visited = [[False] * n for _ in range(m)]
        count = 0

        def dfs(r: int, c: int) -> None:
            if r < 0 or r >= m or c < 0 or c >= n or grid[r][c] != '1' or visited[r][c]:
                return
            visited[r][c] = True
            dfs(r + 1, c)
            dfs(r - 1, c)
            dfs(r, c + 1)
            dfs(r, c - 1)

        for r in range(m):
            for c in range(n):
                if grid[r][c] == '1' and not visited[r][c]:
                    count += 1
                    dfs(r, c)
        return count
```

#### Java 21
```java
class Solution {
    public int numIslands(char[][] grid) {
        if (grid == null || grid.length == 0) return 0;
        int m = grid.length, n = grid[0].length;
        boolean[][] visited = new boolean[m][n];
        int count = 0;
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == '1' && !visited[r][c]) {
                    count++;
                    dfs(grid, visited, r, c, m, n);
                }
            }
        }
        return count;
    }

    private void dfs(char[][] grid, boolean[][] visited, int r, int c, int m, int n) {
        if (r < 0 || r >= m || c < 0 || c >= n || grid[r][c] != '1' || visited[r][c]) {
            return;
        }
        visited[r][c] = true;
        dfs(grid, visited, r + 1, c, m, n);
        dfs(grid, visited, r - 1, c, m, n);
        dfs(grid, visited, r, c + 1, m, n);
        dfs(grid, visited, r, c - 1, m, n);
    }
}
```

#### TypeScript
```typescript
function numIslands(grid: string[][]): number {
    if (!grid || grid.length === 0) return 0;
    const m = grid.length, n = grid[0].length;
    const visited = Array.from({ length: m }, () => new Uint8Array(n));
    let count = 0;

    function dfs(r: number, c: number): void {
        if (r < 0 || r >= m || c < 0 || c >= n || grid[r][c] !== '1' || visited[r][c]) {
            return;
        }
        visited[r][c] = 1;
        dfs(r + 1, c);
        dfs(r - 1, c);
        dfs(r, c + 1);
        dfs(r, c - 1);
    }

    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (grid[r][c] === '1' && !visited[r][c]) {
                count++;
                dfs(r, c);
            }
        }
    }
    return count;
}
```

#### Go
```go
package main

func numIslands(grid [][]byte) int {
    if len(grid) == 0 {
        return 0
    }
    m, n := len(grid), len(grid[0])
    visited := make([][]bool, m)
    for i := range visited {
        visited[i] = make([]bool, n)
    }
    count := 0

    var dfs func(r, c int)
    dfs = func(r, c int) {
        if r < 0 || r >= m || c < 0 || c >= n || grid[r][c] != '1' || visited[r][c] {
            return
        }
        visited[r][c] = true
        dfs(r+1, c)
        dfs(r-1, c)
        dfs(r, c+1)
        dfs(r, c-1)
    }

    for r := 0; r < m; r++ {
        for c := 0; c < n; c++ {
            if grid[r][c] == '1' && !visited[r][c] {
                count++
                dfs(r, c)
            }
        }
    }
    return count
}
```

#### Rust
```rust
impl Solution {
    pub fn num_islands(grid: Vec<Vec<char>>) -> i32 {
        if grid.is_empty() {
            return 0;
        }
        let m = grid.len();
        let n = grid[0].len();
        let mut visited = vec![vec![false; n]; m];
        let mut count = 0;

        for r in 0..m {
            for c in 0..n {
                if grid[r][c] == '1' && !visited[r][c] {
                    count += 1;
                    Self::dfs(&grid, &mut visited, r, c, m, n);
                }
            }
        }
        count
    }

    fn dfs(grid: &[Vec<char>], visited: &mut [Vec<bool>], r: usize, c: usize, m: usize, n: usize) {
        if grid[r][c] != '1' || visited[r][c] {
            return;
        }
        visited[r][c] = true;
        if r + 1 < m {
            Self::dfs(grid, visited, r + 1, c, m, n);
        }
        if r > 0 {
            Self::dfs(grid, visited, r - 1, c, m, n);
        }
        if c + 1 < n {
            Self::dfs(grid, visited, r, c + 1, m, n);
        }
        if c > 0 {
            Self::dfs(grid, visited, r, c - 1, m, n);
        }
    }
}
```

---

## 7. Deep Dive Architectural FAQ

<details>
<summary>1. Why is setting `grid[r][c] = '0'` upon enqueue crucial in BFS?</summary>
In BFS, marking a node visited when it is popped from the queue rather than when it is pushed can cause multiple neighbors to enqueue the same node repeatedly.
Marking the node `'0'` immediately upon enqueue prevents duplicate queue insertions and bounds queue space strictly to $O(\min(M, N))$.
</details>

<details>
<summary>2. When should Disjoint Set Union (Union-Find) be preferred over DFS/BFS?</summary>
DSU is superior when the graph is dynamic or streaming (e.g., Number of Islands II - LC 305 where land positions are added one by one over time).
DSU processes dynamic edge additions in $O(\alpha(V))$ amortized time without re-running full traversals.
</details>

<details>
<summary>3. Why only check right and down neighbors in DSU instead of all 4 directions?</summary>
Because the nested loops iterate systematically from top-to-bottom and left-to-right, checking $(r, c+1)$ and $(r+1, c)$ connects every adjacent pair of cells.
Checking $(r-1, c)$ and $(r, c-1)$ is redundant because those connections were already forged when evaluating previous cells.
</details>

<details>
<summary>4. What causes stack overflow in recursive DFS on large grids?</summary>
In worst-case grid layouts ($M = 300, N = 300$, all land), recursive DFS recursion depth can reach $90,000$ stack frames.
Each frame consumes $32$ to $64$ bytes, requiring $\approx 5\text{MB}$ of thread stack space.
In environments with $1\text{MB}$ stack limits, this causes a stack overflow crash.
Iterative BFS or explicit heap-allocated stacks prevent stack exhaustion.
</details>

<details>
<summary>5. How does memory layout impact 2D grid traversal speed?</summary>
Storing rows in row-major contiguous memory (`grid[r][c]`) maximizes spatial locality when iterating across columns ($c \to c+1$).
Striding across rows ($r \to r+1$) jumps across cache lines, increasing L1 cache misses.
Nested loops should always order the outer loop as rows and inner loop as columns.
</details>

<details>
<summary>6. How can 2D coordinates be packed into a single integer?</summary>
Using bit shifting or modular mapping:
`id = (r << 16) | c` (for $N \le 65535$) or `id = r * N + c`.
Packing coordinates eliminates tuple allocation overhead and reduces queue memory by $50\%$.
</details>

<details>
<summary>7. How does this problem relate to Connected Component Labeling in computer vision?</summary>
In image processing, connected component labeling identifies disjoint foreground pixel blobs in binary masks.
The Two-Pass Union-Find algorithm and flood-fill morphological algorithms used here are the standard industry techniques for blob detection.
</details>

<details>
<summary>8. How do diagonal connections alter the algorithm?</summary>
If 8-directional connectivity is allowed (including diagonals), we expand the direction vectors from 4 to 8:
$\{(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)\}$.
The asymptotic complexity remains $O(M \times N)$.
</details>

<details>
<summary>9. What is the memory footprint difference between `std::vector<std::vector<bool>>` and `std::vector<char>`?</summary>
In C++, `std::vector<bool>` is a specialized template that packs booleans into individual bits ($1/8$ byte per cell).
A regular 2D char matrix uses 1 byte per cell.
`std::vector<bool>` reduces memory usage by $8\times$, though bit-masking operations introduce slight instruction overhead.
</details>

<details>
<summary>10. What are the key unit test edge cases for Number of Islands?</summary>
1. All water: all `'0'`s (returns 0).
2. All land: all `'1'`s (returns 1).
3. Checkerboard pattern: alternating `'1'` and `'0'` (no two land cells touch, returns $\lceil MN/2 \rceil$).
4. Single cell grid: `[["1"]]` (returns 1) and `[["0"]]` (returns 0).
5. Single row / single column grids ($1 \times N$ or $M \times 1$).
6. Concentric ring island enclosing a lake that encloses an island.
</details>

---

## 8. Standalone Implementation Links

Access the standalone compilable and runnable source files:
- [C++ Implementation](../C++/number-of-islands.cpp)
- [Python Implementation](../Python/number-of-islands.py)
- [Java Implementation](../Java/number-of-islands.java)
- [TypeScript Implementation](../TypeScript/number-of-islands.ts)
- [Go Implementation](../Golang/number-of-islands.go)
- [Rust Implementation](../Rust/number-of-islands.rs)

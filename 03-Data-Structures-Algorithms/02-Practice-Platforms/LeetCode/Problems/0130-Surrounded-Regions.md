---
title: LeetCode - 0130 - Surrounded Regions
tags:
  - leetcode
  - problem
  - graph
  - matrix
  - dfs
  - bfs
difficulty: medium
source: leetcode
problem_number: "0130"
topics:
  - Graph
  - Matrix
  - Depth-First Search
  - Breadth-First Search
---

# LeetCode 0130: Surrounded Regions

## Problem Breakdown

You are given an `m x n` matrix `board` containing `'X'` and `'O'`.
Capture all regions that are 4-directionally surrounded by `'X'`.
A region is captured by flipping all `'O'`s into `'X'`s in that surrounded region.
An `'O'` cell is considered surrounded if and only if no path of 4-directionally adjacent `'O'`s connects it to the perimeter boundary of the board.

### Boundary Escape Invariant

- Directly searching for surrounded regions requires checking whether every component reaches any boundary cell, which can be convoluted and prone to boundary leaks.
- Inversion insight: Any `'O'` connected to the outer boundary of the board **cannot** be captured.
- Therefore, we can invert the problem:
  1. Identify all `'O'`s situated along the four edges of the board.
  2. Perform a flood-fill traversal (DFS or BFS) starting exclusively from these perimeter `'O'`s, marking every reachable `'O'` with a temporary character (such as `'#'`).
  3. Every remaining `'O'` in the interior is strictly enclosed and cannot reach the perimeter; thus, it must be captured and flipped to `'X'`.
  4. Finally, restore the temporarily marked `'#'` cells back to `'O'`.

## Optimal Approaches

### Boundary Flood-Fill (DFS / BFS)

```
Initial Board:
X X X X
X O O X
X X O X
X O X X

Step 1: Traverse boundaries. Cell (3, 1) is 'O' on bottom edge.
DFS marks boundary-connected component:
X X X X
X O O X
X X # X
X # X X

Step 2: Scan full board. Unmarked 'O' -> 'X', '#' -> 'O':
X X X X
X X X X
X X O X
X O X X
```

1. Run DFS starting from all `'O'` cells on the first/last row and first/last column.
2. Inside DFS, replace visited `'O'` cells with `'#'` and recursively explore horizontal and vertical neighbors.
3. Once all boundary sweeps conclude, iterate through all cells `(i, j)`:
   - If `board[i][j] == 'O'`, replace with `'X'`.
   - If `board[i][j] == '#'`, restore to `'O'`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(m \cdot n)$ | Every grid cell is visited a constant number of times across the boundary and final sweeps. |
| **Space Complexity** | $O(m \cdot n)$ | Recursion stack memory in the worst case when the board is filled entirely with connected `'O'`s. |

## Common Traps & Edge Cases

- **Small Matrices ($m \le 2$ or $n \le 2$)**: All cells in such matrices are along or adjacent to boundaries, meaning no cell can ever be enclosed; the algorithm correctly leaves them as `'O'`.
- **Stack Overflow on Massive Grids**: On deeply nested recursive paths, an explicit BFS queue or iterative DFS can avoid call stack limits.
- **Diagonal Adjacency**: Cells are only connected 4-directionally (up, down, left, right), not diagonally.

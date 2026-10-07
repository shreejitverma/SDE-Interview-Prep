package main

/*
 * Problem: LeetCode 130 - Surrounded Regions
 * Difficulty: Medium
 * Concepts: Graph, Matrix, DFS, BFS
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n) recursion stack
 */

func solve(board [][]byte) {
	if len(board) == 0 || len(board[0]) == 0 {
		return
	}

	m := len(board)
	n := len(board[0])

	var dfs func(r, c int)
	dfs = func(r, c int) {
		if r < 0 || r >= m || c < 0 || c >= n || board[r][c] != 'O' {
			return
		}
		board[r][c] = '#'
		dfs(r+1, c)
		dfs(r-1, c)
		dfs(r, c+1)
		dfs(r, c-1)
	}

	// Step 1: Mark boundary-connected 'O's
	for i := 0; i < m; i++ {
		dfs(i, 0)
		dfs(i, n-1)
	}
	for j := 0; j < n; j++ {
		dfs(0, j)
		dfs(m-1, j)
	}

	// Step 2: Flip unvisited 'O' -> 'X', and '#' -> 'O'
	for i := 0; i < m; i++ {
		for j := 0; j < n; j++ {
			if board[i][j] == 'O' {
				board[i][j] = 'X'
			} else if board[i][j] == '#' {
				board[i][j] = 'O'
			}
		}
	}
}

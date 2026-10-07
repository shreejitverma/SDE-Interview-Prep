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

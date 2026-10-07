/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 994 - Rotting Oranges
 * Language: Golang
 *
 * Complexity:
 * - Time: O(M * N)
 * - Space: O(M * N)
 */

package main

type point struct {
	r, c int
}

func orangesRotting(grid [][]int) int {
	if len(grid) == 0 || len(grid[0]) == 0 {
		return 0
	}

	m := len(grid)
	n := len(grid[0])
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

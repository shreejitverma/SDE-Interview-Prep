/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 84 - Largest Rectangle in Histogram
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(N)
 */

package main

func largestRectangleArea(heights []int) int {
	n := len(heights)
	stack := make([]int, 0, n+1)
	maxArea := 0

	for i := 0; i <= n; i++ {
		currHeight := 0
		if i < n {
			currHeight = heights[i]
		}

		for len(stack) > 0 && heights[stack[len(stack)-1]] >= currHeight {
			h := heights[stack[len(stack)-1]]
			stack = stack[:len(stack)-1]
			width := i
			if len(stack) > 0 {
				width = i - 1 - stack[len(stack)-1]
			}
			area := h * width
			if area > maxArea {
				maxArea = area
			}
		}
		stack = append(stack, i)
	}

	return maxArea
}

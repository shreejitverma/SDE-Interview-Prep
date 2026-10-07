/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 739 - Daily Temperatures
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(N)
 */

package main

func dailyTemperatures(temperatures []int) []int {
	n := len(temperatures)
	result := make([]int, n)
	stack := make([]int, 0, n)

	for i := 0; i < n; i++ {
		for len(stack) > 0 && temperatures[stack[len(stack)-1]] < temperatures[i] {
			prevIdx := stack[len(stack)-1]
			stack = stack[:len(stack)-1]
			result[prevIdx] = i - prevIdx
		}
		stack = append(stack, i)
	}

	return result
}

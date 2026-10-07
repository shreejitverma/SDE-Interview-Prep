/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 167 - Two Sum II - Input Array Is Sorted
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

package main

func twoSum(numbers []int, target int) []int {
	left := 0
	right := len(numbers) - 1

	for left < right {
		sum := numbers[left] + numbers[right]
		if sum == target {
			return []int{left + 1, right + 1}
		} else if sum < target {
			left++
		} else {
			right--
		}
	}

	return []int{-1, -1}
}

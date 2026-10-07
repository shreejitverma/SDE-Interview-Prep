/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * 2^N)
// Space: O(N) auxiliary (recursion stack)

package main

func subsets(nums []int) [][]int {
	var result [][]int
	var path []int

	var backtrack func(start int)
	backtrack = func(start int) {
		temp := make([]int, len(path))
		copy(temp, path)
		result = append(result, temp)

		for i := start; i < len(nums); i++ {
			path = append(path, nums[i])
			backtrack(i + 1)
			path = path[:len(path)-1]
		}
	}

	backtrack(0)
	return result
}

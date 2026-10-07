/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * N!)
// Space: O(N) auxiliary (recursion stack depth)

package main

func permute(nums []int) [][]int {
	var result [][]int
	arr := make([]int, len(nums))
	copy(arr, nums)

	var backtrack func(first int)
	backtrack = func(first int) {
		if first == len(arr) {
			perm := make([]int, len(arr))
			copy(perm, arr)
			result = append(result, perm)
			return
		}

		for i := first; i < len(arr); i++ {
			arr[first], arr[i] = arr[i], arr[first]
			backtrack(first + 1)
			arr[first], arr[i] = arr[i], arr[first]
		}
	}

	backtrack(0)
	return result
}

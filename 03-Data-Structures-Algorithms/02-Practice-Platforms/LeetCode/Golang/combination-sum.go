/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N^(T/M + 1)) where N = candidates count, T = target, M = min(candidates)
// Space: O(T/M) auxiliary (recursion stack depth)

package main

import "sort"

func combinationSum(candidates []int, target int) [][]int {
	sort.Ints(candidates)
	var result [][]int
	var path []int

	var backtrack func(remain int, start int)
	backtrack = func(remain int, start int) {
		if remain == 0 {
			comb := make([]int, len(path))
			copy(comb, path)
			result = append(result, comb)
			return
		}

		for i := start; i < len(candidates); i++ {
			if candidates[i] > remain {
				break
			}
			path = append(path, candidates[i])
			backtrack(remain-candidates[i], i)
			path = path[:len(path)-1]
		}
	}

	backtrack(target, 0)
	return result
}

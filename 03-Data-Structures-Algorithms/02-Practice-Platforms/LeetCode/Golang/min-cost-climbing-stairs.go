/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 746 - Min Cost Climbing Stairs
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

package main

func minCostClimbingStairs(cost []int) int {
	prev2 := 0
	prev1 := 0

	for _, c := range cost {
		minPrev := prev1
		if prev2 < minPrev {
			minPrev = prev2
		}
		curr := c + minPrev
		prev2 = prev1
		prev1 = curr
	}

	if prev1 < prev2 {
		return prev1
	}
	return prev2
}

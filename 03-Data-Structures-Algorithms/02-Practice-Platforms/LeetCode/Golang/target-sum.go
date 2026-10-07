package main

/*
 * Problem: LeetCode 494 - Target Sum
 * Difficulty: Medium
 * Concepts: Dynamic Programming, 0/1 Knapsack, Subset Sum
 *
 * Time Complexity: O(n * S) where S = (total_sum + target) / 2
 * Space Complexity: O(S)
 */

func findTargetSumWays(nums []int, target int) int {
	totalSum := 0
	for _, num := range nums {
		totalSum += num
	}

	absTarget := target
	if absTarget < 0 {
		absTarget = -absTarget
	}

	if totalSum < absTarget || (totalSum+target)%2 != 0 {
		return 0
	}

	subsetSum := (totalSum + target) / 2
	dp := make([]int, subsetSum+1)
	dp[0] = 1

	for _, num := range nums {
		for s := subsetSum; s >= num; s-- {
			dp[s] += dp[s-num]
		}
	}

	return dp[subsetSum]
}

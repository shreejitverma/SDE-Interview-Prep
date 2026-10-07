package main

/*
 * Problem: LeetCode 134 - Gas Station
 * Difficulty: Medium
 * Concepts: Greedy, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

func canCompleteCircuit(gas []int, cost []int) int {
	totalTank := 0
	currentTank := 0
	startIndex := 0

	for i := 0; i < len(gas); i++ {
		balance := gas[i] - cost[i]
		totalTank += balance
		currentTank += balance

		if currentTank < 0 {
			startIndex = i + 1
			currentTank = 0
		}
	}

	if totalTank >= 0 {
		return startIndex
	}
	return -1
}

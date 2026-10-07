package main

/*
 * Problem: LeetCode 621 - Task Scheduler
 * Difficulty: Medium
 * Concepts: Greedy, Counting, Math
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

func leastInterval(tasks []byte, n int) int {
	var freq [26]int
	maxFreq := 0

	for _, task := range tasks {
		idx := task - 'A'
		freq[idx]++
		if freq[idx] > maxFreq {
			maxFreq = freq[idx]
		}
	}

	maxCount := 0
	for _, count := range freq {
		if count == maxFreq {
			maxCount++
		}
	}

	calculated := (maxFreq-1)*(n+1) + maxCount
	if len(tasks) > calculated {
		return len(tasks)
	}
	return calculated
}

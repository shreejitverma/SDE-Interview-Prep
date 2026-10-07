package main

/*
 * Problem: LeetCode 239 - Sliding Window Maximum
 * Difficulty: Hard
 * Concepts: Monotonic Queue, Sliding Window, Deque
 *
 * Time Complexity: O(n)
 * Space Complexity: O(k)
 */

func maxSlidingWindow(nums []int, k int) []int {
	n := len(nums)
	result := make([]int, 0, n-k+1)
	dq := make([]int, 0, k) // Slice as deque storing indices

	for i := 0; i < n; i++ {
		// Remove elements outside window
		if len(dq) > 0 && dq[0] <= i-k {
			dq = dq[1:]
		}

		// Maintain strictly decreasing order
		for len(dq) > 0 && nums[dq[len(dq)-1]] <= nums[i] {
			dq = dq[:len(dq)-1]
		}

		dq = append(dq, i)

		if i >= k-1 {
			result = append(result, nums[dq[0]])
		}
	}

	return result
}

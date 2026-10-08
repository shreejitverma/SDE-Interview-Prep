package main

/*
 * Problem: LeetCode 704 - Binary Search
 * Difficulty: Easy
 * Concepts: Binary Search, Array
 *
 * Time Complexity: O(log n)
 * Space Complexity: O(1)
 */

func search(nums []int, target int) int {
	left := 0
	right := len(nums) - 1

	for left <= right {
		mid := left + (right-left)/2
		if nums[mid] == target {
			return mid
		} else if nums[mid] < target {
			left = mid + 1
		} else {
			right = mid - 1
		}
	}

	return -1
}

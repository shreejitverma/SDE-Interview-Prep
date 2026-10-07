package main

import "math/rand"

/*
 * Problem: LeetCode 215 - Kth Largest Element in an Array
 * Difficulty: Medium
 * Concepts: QuickSelect, Min-Heap, Divide and Conquer
 *
 * Time Complexity: O(n) average
 * Space Complexity: O(1) iterative
 */

func findKthLargest(nums []int, k int) int {
	targetIdx := len(nums) - k
	left := 0
	right := len(nums) - 1

	for left <= right {
		pivotIdx := left + rand.Intn(right-left+1)
		pivotVal := nums[pivotIdx]

		lt := left
		gt := right
		i := left

		for i <= gt {
			if nums[i] < pivotVal {
				nums[i], nums[lt] = nums[lt], nums[i]
				i++
				lt++
			} else if nums[i] > pivotVal {
				nums[i], nums[gt] = nums[gt], nums[i]
				gt--
			} else {
				i++
			}
		}

		if targetIdx >= lt && targetIdx <= gt {
			return nums[targetIdx]
		} else if targetIdx < lt {
			right = lt - 1
		} else {
			left = gt + 1
		}
	}

	return nums[targetIdx]
}

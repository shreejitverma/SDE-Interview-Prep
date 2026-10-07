/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 268 - Missing Number
 * Difficulty: Easy
 * Language: Go (Golang)
 *
 * Performance Analysis:
 * - Time Complexity: O(N) single pass.
 * - Space Complexity: O(1) auxiliary space.
 */

package main

import "fmt"

func missingNumber(nums []int) int {
	missing := len(nums)
	for i, x := range nums {
		missing ^= i ^ x
	}
	return missing
}

func missingNumberGauss(nums []int) int {
	n := len(nums)
	diff := n * (n + 1) / 2
	for _, x := range nums {
		diff -= x
	}
	return diff
}

func main() {
	nums1 := []int{3, 0, 1}
	nums2 := []int{0, 1}
	nums3 := []int{9, 6, 4, 2, 3, 5, 7, 0, 1}
	fmt.Println("Missing 1:", missingNumber(nums1)) // 2
	fmt.Println("Missing 2:", missingNumber(nums2)) // 2
	fmt.Println("Missing 3:", missingNumber(nums3)) // 8
}

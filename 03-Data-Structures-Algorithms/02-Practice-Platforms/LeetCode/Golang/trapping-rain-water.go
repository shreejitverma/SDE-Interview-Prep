/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1) auxiliary

package main

func trap(height []int) int {
	if len(height) == 0 {
		return 0
	}

	left := 0
	right := len(height) - 1
	leftMax := 0
	rightMax := 0
	totalWater := 0

	for left < right {
		if height[left] <= height[right] {
			if height[left] >= leftMax {
				leftMax = height[left]
			} else {
				totalWater += leftMax - height[left]
			}
			left++
		} else {
			if height[right] >= rightMax {
				rightMax = height[right]
			} else {
				totalWater += rightMax - height[right]
			}
			right--
		}
	}

	return totalWater
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

func canJump(nums []int) bool {
	maxReachable := 0
	for i, val := range nums {
		if i > maxReachable {
			return false
		}
		if i+val > maxReachable {
			maxReachable = i + val
		}
		if maxReachable >= len(nums)-1 {
			return true
		}
	}
	return true
}

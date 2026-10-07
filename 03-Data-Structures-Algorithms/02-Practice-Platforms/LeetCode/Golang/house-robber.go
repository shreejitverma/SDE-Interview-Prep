/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

func rob(nums []int) int {
	prev2 := 0
	prev1 := 0

	for _, num := range nums {
		current := prev1
		if prev2+num > current {
			current = prev2 + num
		}
		prev2 = prev1
		prev1 = current
	}

	return prev1
}

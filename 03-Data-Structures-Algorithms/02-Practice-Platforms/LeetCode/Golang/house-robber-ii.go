/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

package main

func rob(nums []int) int {
	if len(nums) == 0 {
		return 0
	}
	if len(nums) == 1 {
		return nums[0]
	}

	robRange := func(start, end int) int {
		prev2 := 0
		prev1 := 0

		for i := start; i < end; i++ {
			current := prev1
			if prev2+nums[i] > current {
				current = prev2 + nums[i]
			}
			prev2 = prev1
			prev1 = current
		}

		return prev1
	}

	r1 := robRange(0, len(nums)-1)
	r2 := robRange(1, len(nums))
	if r1 > r2 {
		return r1
	}
	return r2
}

/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 875 - Koko Eating Bananas
 * Language: Golang
 *
 * Complexity:
 * - Time: O(N * log(max(piles)))
 * - Space: O(1)
 */

package main

func minEatingSpeed(piles []int, h int) int {
	left := 1
	right := 1
	for _, pile := range piles {
		if pile > right {
			right = pile
		}
	}

	canFinish := func(k int) bool {
		hours := 0
		for _, pile := range piles {
			hours += (pile + k - 1) / k
		}
		return hours <= h
	}

	for left <= right {
		mid := left + (right-left)/2
		if canFinish(mid) {
			right = mid - 1
		} else {
			left = mid + 1
		}
	}

	return left
}

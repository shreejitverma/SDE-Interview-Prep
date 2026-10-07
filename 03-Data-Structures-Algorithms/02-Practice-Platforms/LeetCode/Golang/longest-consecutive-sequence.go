/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

package main

func longestConsecutive(nums []int) int {
	numSet := make(map[int]struct{}, len(nums))
	for _, num := range nums {
		numSet[num] = struct{}{}
	}

	longestStreak := 0

	for num := range numSet {
		if _, exists := numSet[num-1]; !exists {
			currentNum := num
			currentStreak := 1

			for {
				if _, hasNext := numSet[currentNum+1]; hasNext {
					currentNum++
					currentStreak++
				} else {
					break
				}
			}

			if currentStreak > longestStreak {
				longestStreak = currentStreak
			}
		}
	}

	return longestStreak
}

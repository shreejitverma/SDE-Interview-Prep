/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

package main

func hammingWeight(num uint32) int {
	count := 0
	for num != 0 {
		num &= (num - 1)
		count++
	}
	return count
}

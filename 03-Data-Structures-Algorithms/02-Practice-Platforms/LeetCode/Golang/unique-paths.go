/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(min(M, N))
// Space: O(1)

package main

func uniquePaths(m int, n int) int {
	totalSteps := m + n - 2
	k := m - 1
	if n-1 < k {
		k = n - 1
	}

	result := int64(1)
	for i := 1; i <= k; i++ {
		result = result * int64(totalSteps-k+i) / int64(i)
	}

	return int(result)
}

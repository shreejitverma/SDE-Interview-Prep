package main

/*
 * Problem: LeetCode 72 - Edit Distance
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(min(m, n))
 */

func minDistance(word1 string, word2 string) int {
	m := len(word1)
	n := len(word2)

	if m < n {
		return minDistance(word2, word1)
	}

	dp := make([]int, n+1)
	for j := 0; j <= n; j++ {
		dp[j] = j
	}

	for i := 1; i <= m; i++ {
		prevDiag := dp[0]
		dp[0] = i

		for j := 1; j <= n; j++ {
			temp := dp[j]
			if word1[i-1] == word2[j-1] {
				dp[j] = prevDiag
			} else {
				minOp := dp[j]
				if dp[j-1] < minOp {
					minOp = dp[j-1]
				}
				if prevDiag < minOp {
					minOp = prevDiag
				}
				dp[j] = 1 + minOp
			}
			prevDiag = temp
		}
	}

	return dp[n]
}

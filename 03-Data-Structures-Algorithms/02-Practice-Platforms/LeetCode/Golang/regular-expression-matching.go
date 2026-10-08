package main

/*
 * Problem: LeetCode 10 - Regular Expression Matching
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String, Recursion
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n)
 */

func isMatch(s string, p string) bool {
	m := len(s)
	n := len(p)

	dp := make([][]bool, m+1)
	for i := range dp {
		dp[i] = make([]bool, n+1)
	}
	dp[0][0] = true

	for j := 2; j <= n; j++ {
		if p[j-1] == '*' {
			dp[0][j] = dp[0][j-2]
		}
	}

	for i := 1; i <= m; i++ {
		for j := 1; j <= n; j++ {
			if p[j-1] == '*' {
				dp[i][j] = dp[i][j-2]
				prev := p[j-2]
				if prev == '.' || prev == s[i-1] {
					dp[i][j] = dp[i][j] || dp[i-1][j]
				}
			} else if p[j-1] == '.' || p[j-1] == s[i-1] {
				dp[i][j] = dp[i-1][j-1]
			}
		}
	}

	return dp[m][n]
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * L_max) where L_max is maximum word length
// Space: O(N + M) auxiliary

package main

func wordBreak(s string, wordDict []string) bool {
	dict := make(map[string]bool, len(wordDict))
	maxLen := 0
	for _, w := range wordDict {
		dict[w] = true
		if len(w) > maxLen {
			maxLen = len(w)
		}
	}

	n := len(s)
	dp := make([]bool, n+1)
	dp[0] = true

	for i := 1; i <= n; i++ {
		start := i - maxLen
		if start < 0 {
			start = 0
		}
		for j := i - 1; j >= start; j-- {
			if dp[j] && dict[s[j:i]] {
				dp[i] = true
				break
			}
		}
	}

	return dp[n]
}

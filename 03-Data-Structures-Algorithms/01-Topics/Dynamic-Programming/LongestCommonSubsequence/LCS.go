/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(M * N)
// Space Complexity: O(min(M, N)) with 1D row rolling array

package main

import "fmt"

func longestCommonSubsequence(text1 string, text2 string) int {
	if len(text1) < len(text2) {
		text1, text2 = text2, text1
	}

	m, n := len(text1), len(text2)
	prev := make([]int, n+1)
	curr := make([]int, n+1)

	for i := 1; i <= m; i++ {
		for j := 1; j <= n; j++ {
			if text1[i-1] == text2[j-1] {
				curr[j] = prev[j-1] + 1
			} else {
				if prev[j] > curr[j-1] {
					curr[j] = prev[j]
				} else {
					curr[j] = curr[j-1]
				}
			}
		}
		copy(prev, curr)
		for k := range curr {
			curr[k] = 0
		}
	}

	return prev[n]
}

func main() {
	fmt.Println("LCS('abcde', 'ace'):", longestCommonSubsequence("abcde", "ace")) // 3
	fmt.Println("LCS('abc', 'abc'):", longestCommonSubsequence("abc", "abc"))     // 3
	fmt.Println("LCS('abc', 'def'):", longestCommonSubsequence("abc", "def"))     // 0
}

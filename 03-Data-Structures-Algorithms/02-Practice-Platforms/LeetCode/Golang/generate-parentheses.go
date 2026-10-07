/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(4^N / sqrt(N))
// Space: O(N) auxiliary (recursion stack)

package main

func generateParenthesis(n int) []string {
	var result []string
	path := make([]byte, 2*n)

	var backtrack func(openCount, closeCount int)
	backtrack = func(openCount, closeCount int) {
		if openCount == n && closeCount == n {
			result = append(result, string(path))
			return
		}

		if openCount < n {
			path[openCount+closeCount] = '('
			backtrack(openCount+1, closeCount)
		}

		if closeCount < openCount {
			path[openCount+closeCount] = ')'
			backtrack(openCount, closeCount+1)
		}
	}

	backtrack(0, 0)
	return result
}

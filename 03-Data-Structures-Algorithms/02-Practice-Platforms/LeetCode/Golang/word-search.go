/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N * 4^L) worst case, heavily pruned in practice
// Space: O(L) recursion stack

package main

func exist(board [][]byte, word string) bool {
	m := len(board)
	if m == 0 {
		return false
	}
	n := len(board[0])
	wordLen := len(word)
	if m*n < wordLen {
		return false
	}

	boardFreq := make([]int, 128)
	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			boardFreq[board[r][c]]++
		}
	}

	for i := 0; i < wordLen; i++ {
		boardFreq[word[i]]--
		if boardFreq[word[i]] < 0 {
			return false
		}
	}

	target := word
	// Re-count board frequency to check endpoints
	countFirst := 0
	countLast := 0
	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			if board[r][c] == word[0] {
				countFirst++
			}
			if board[r][c] == word[wordLen-1] {
				countLast++
			}
		}
	}
	if countFirst > countLast {
		runes := []byte(word)
		for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
			runes[i], runes[j] = runes[j], runes[i]
		}
		target = string(runes)
	}

	var dfs func(r, c, idx int) bool
	dfs = func(r, c, idx int) bool {
		if idx == len(target) {
			return true
		}
		if r < 0 || r >= m || c < 0 || c >= n || board[r][c] != target[idx] {
			return false
		}

		temp := board[r][c]
		board[r][c] = '#'

		found := dfs(r+1, c, idx+1) ||
			dfs(r-1, c, idx+1) ||
			dfs(r, c+1, idx+1) ||
			dfs(r, c-1, idx+1)

		board[r][c] = temp
		return found
	}

	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			if board[r][c] == target[0] && dfs(r, c, 0) {
				return true
			}
		}
	}

	return false
}

/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 212 - Word Search II
 * Difficulty: Hard
 * Language: Go (Golang)
 *
 * Performance Analysis:
 * - Time Complexity: O(M * N * 4 * 3^(L - 1)) worst-case pruned search.
 * - Space Complexity: O(Sum(len(words))) for Trie node pool, O(L) call stack.
 */

package main

import "fmt"

type TrieNode struct {
	children  [26]*TrieNode
	word      string
	wordCount int
}

func insertWord(root *TrieNode, word string) {
	curr := root
	for i := 0; i < len(word); i++ {
		idx := word[i] - 'a'
		if curr.children[idx] == nil {
			curr.children[idx] = &TrieNode{}
		}
		curr = curr.children[idx]
		curr.wordCount++
	}
	curr.word = word
}

func findWords(board [][]byte, words []string) []string {
	var result []string
	if len(board) == 0 || len(board[0]) == 0 || len(words) == 0 {
		return result
	}

	root := &TrieNode{}
	for _, w := range words {
		insertWord(root, w)
	}

	m, n := len(board), len(board[0])

	var dfs func(r, c int, parent *TrieNode, idx int)
	dfs = func(r, c int, parent *TrieNode, idx int) {
		curr := parent.children[idx]
		if curr == nil || curr.wordCount <= 0 {
			return
		}

		if curr.word != "" {
			result = append(result, curr.word)
			curr.word = "" // Mark consumed

			curr.wordCount--
			if curr.wordCount <= 0 {
				parent.children[idx] = nil
			}
		}

		ch := board[r][c]
		board[r][c] = '#' // Visited sentinel

		dr := [4]int{-1, 1, 0, 0}
		dc := [4]int{0, 0, -1, 1}

		for i := 0; i < 4; i++ {
			nr, nc := r+dr[i], c+dc[i]
			if nr >= 0 && nr < m && nc >= 0 && nc < n {
				nextChar := board[nr][nc]
				if nextChar != '#' {
					nextIdx := int(nextChar - 'a')
					if curr.children[nextIdx] != nil && curr.children[nextIdx].wordCount > 0 {
						dfs(nr, nc, curr, nextIdx)
					}
				}
			}
		}

		board[r][c] = ch // Backtrack
	}

	for r := 0; r < m; r++ {
		for c := 0; c < n; c++ {
			idx := int(board[r][c] - 'a')
			if idx >= 0 && idx < 26 && root.children[idx] != nil {
				dfs(r, c, root, idx)
			}
		}
	}

	return result
}

func main() {
	board := [][]byte{
		{'o', 'a', 'a', 'n'},
		{'e', 't', 'a', 'e'},
		{'i', 'h', 'k', 'r'},
		{'i', 'f', 'l', 'v'},
	}
	words := []string{"oath", "pea", "eat", "rain"}
	res := findWords(board, words)
	fmt.Println("Found words:", res)
}

package main

/*
 * Problem: LeetCode 36 - Valid Sudoku
 * Difficulty: Medium
 * Concepts: Matrix, Hash Table, Bit Manipulation
 *
 * Time Complexity: O(1) (fixed 9x9 board = 81 cells)
 * Space Complexity: O(1)
 */

func isValidSudoku(board [][]byte) bool {
	var rows [9]int
	var cols [9]int
	var boxes [9]int

	for r := 0; r < 9; r++ {
		for c := 0; c < 9; c++ {
			ch := board[r][c]
			if ch == '.' {
				continue
			}

			val := ch - '1'
			mask := 1 << val
			boxIdx := (r/3)*3 + (c / 3)

			if (rows[r]&mask) != 0 || (cols[c]&mask) != 0 || (boxes[boxIdx]&mask) != 0 {
				return false
			}

			rows[r] |= mask
			cols[c] |= mask
			boxes[boxIdx] |= mask
		}
	}

	return true
}

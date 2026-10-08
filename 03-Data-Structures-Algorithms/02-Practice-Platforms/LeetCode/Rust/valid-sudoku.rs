/*
 * Problem: LeetCode 36 - Valid Sudoku
 * Difficulty: Medium
 * Concepts: Matrix, Hash Table, Bit Manipulation
 *
 * Time Complexity: O(1) (fixed 9x9 board = 81 cells)
 * Space Complexity: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn is_valid_sudoku(board: Vec<Vec<char>>) -> bool {
        let mut rows = [0u16; 9];
        let mut cols = [0u16; 9];
        let mut boxes = [0u16; 9];

        for r in 0..9 {
            for c in 0..9 {
                let ch = board[r][c];
                if ch == '.' {
                    continue;
                }

                let val = ch as usize - '1' as usize;
                let mask = 1u16 << val;
                let box_idx = (r / 3) * 3 + (c / 3);

                if (rows[r] & mask) != 0 || (cols[c] & mask) != 0 || (boxes[box_idx] & mask) != 0 {
                    return false;
                }

                rows[r] |= mask;
                cols[c] |= mask;
                boxes[box_idx] |= mask;
            }
        }

        true
    }
}

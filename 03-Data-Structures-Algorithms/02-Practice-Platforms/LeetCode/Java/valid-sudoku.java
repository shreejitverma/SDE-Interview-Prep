/*
 * Problem: LeetCode 36 - Valid Sudoku
 * Difficulty: Medium
 * Concepts: Matrix, Hash Table, Bit Manipulation
 *
 * Time Complexity: O(1) (fixed 9x9 board = 81 cells)
 * Space Complexity: O(1)
 */

class Solution {
    public boolean isValidSudoku(char[][] board) {
        int[] rows = new int[9];
        int[] cols = new int[9];
        int[] boxes = new int[9];

        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                char ch = board[r][c];
                if (ch == '.') {
                    continue;
                }

                int val = ch - '1';
                int mask = 1 << val;
                int boxIdx = (r / 3) * 3 + (c / 3);

                if ((rows[r] & mask) != 0 || (cols[c] & mask) != 0 || (boxes[boxIdx] & mask) != 0) {
                    return false;
                }

                rows[r] |= mask;
                cols[c] |= mask;
                boxes[boxIdx] |= mask;
            }
        }

        return true;
    }
}

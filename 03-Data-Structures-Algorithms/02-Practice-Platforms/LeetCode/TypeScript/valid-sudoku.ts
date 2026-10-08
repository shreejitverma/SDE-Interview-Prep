/*
 * Problem: LeetCode 36 - Valid Sudoku
 * Difficulty: Medium
 * Concepts: Matrix, Hash Table, Bit Manipulation
 *
 * Time Complexity: O(1) (fixed 9x9 board = 81 cells)
 * Space Complexity: O(1)
 */

export function isValidSudoku(board: string[][]): boolean {
    const rows = new Array(9).fill(0);
    const cols = new Array(9).fill(0);
    const boxes = new Array(9).fill(0);

    for (let r = 0; r < 9; r++) {
        for (let c = 0; c < 9; c++) {
            const ch = board[r][c];
            if (ch === ".") {
                continue;
            }

            const val = parseInt(ch, 10) - 1;
            const mask = 1 << val;
            const boxIdx = Math.floor(r / 3) * 3 + Math.floor(c / 3);

            if ((rows[r] & mask) || (cols[c] & mask) || (boxes[boxIdx] & mask)) {
                return false;
            }

            rows[r] |= mask;
            cols[c] |= mask;
            boxes[boxIdx] |= mask;
        }
    }

    return true;
}

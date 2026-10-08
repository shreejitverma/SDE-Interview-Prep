"""
Problem: LeetCode 36 - Valid Sudoku
Difficulty: Medium
Concepts: Matrix, Hash Table, Bit Manipulation

Time Complexity: O(1) (fixed 9x9 board = 81 cells)
Space Complexity: O(1)
"""

from typing import List


class Solution:
    def isValidSudoku(self, board: List[List[str]]) -> bool:
        rows = [0] * 9
        cols = [0] * 9
        boxes = [0] * 9

        for r in range(9):
            for c in range(9):
                ch = board[r][c]
                if ch == ".":
                    continue

                val = int(ch) - 1
                mask = 1 << val
                box_idx = (r // 3) * 3 + (c // 3)

                if (rows[r] & mask) or (cols[c] & mask) or (boxes[box_idx] & mask):
                    return False

                rows[r] |= mask
                cols[c] |= mask
                boxes[box_idx] |= mask

        return True

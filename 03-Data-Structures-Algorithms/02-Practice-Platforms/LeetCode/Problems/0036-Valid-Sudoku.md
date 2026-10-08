---
title: LeetCode - 0036 - Valid Sudoku
tags:
  - leetcode
  - problem
  - matrix
  - hash-table
  - bit-manipulation
difficulty: medium
source: leetcode
problem_number: "0036"
topics:
  - Matrix
  - Hash Table
  - Bit Manipulation
  - Array
---

# LeetCode 0036: Valid Sudoku

## Problem Breakdown

Determine if a `9 x 9` Sudoku board is valid.
Only the filled cells need to be validated according to the following rules:
1. Each row must contain the digits `1-9` without repetition.
2. Each column must contain the digits `1-9` without repetition.
3. Each of the nine `3 x 3` sub-boxes of the grid must contain the digits `1-9` without repetition.
A Sudoku board (partially filled) could be valid but is not necessarily solvable.
Only the filled cells need to be validated.

### Bitmask Index Mapping

- The grid contains 81 cells across 9 rows, 9 columns, and 9 sub-boxes.
- A cell at `(r, c)` belongs to:
  - Row `r` ($0 \le r < 9$).
  - Column `c` ($0 \le c < 9$).
  - Sub-box $\text{box\_idx} = \lfloor r / 3 \rfloor \times 3 + \lfloor c / 3 \rfloor$ ($0 \le \text{box\_idx} < 9$).
- For digit `d` ($\in [1, 9]$), we map it to bit position $\text{val} = d - 1$ and bitmask $1 \ll \text{val}$.
- Checking whether digit `d` has already appeared in row, column, or sub-box is performed using a single bitwise AND operation:
  $$\text{collision} = (\text{rows}[r] \ \& \ \text{mask}) \lor (\text{cols}[c] \ \& \ \text{mask}) \lor (\text{boxes}[\text{box\_idx}] \ \& \ \text{mask})$$

## Optimal Approaches

### Single-Pass Bitmask Verification

```
Cell (r, c) = (4, 7) contains digit '5':
val = 5 - 1 = 4 -> mask = 1 << 4 = 16 (binary 000010000)
box_idx = (4 / 3) * 3 + (7 / 3) = 1 * 3 + 2 = 5

Check:
(rows[4] & 16) == 0?
(cols[7] & 16) == 0?
(boxes[5] & 16) == 0?

If all zero:
rows[4] |= 16
cols[7] |= 16
boxes[5] |= 16
Continue scan.
```

1. Allocate three arrays of integers of size 9 initialized to 0: `rows`, `cols`, and `boxes`.
2. Iterate `r` from 0 to 8 and `c` from 0 to 8:
   - If `board[r][c] == '.'`, skip.
   - Compute `val = board[r][c] - '1'` and `mask = 1 << val`.
   - Compute `box_idx = (r / 3) * 3 + (c / 3)`.
   - If any bitmask already has the `val`-th bit set, return `false`.
   - Set the bit in `rows[r]`, `cols[c]`, and `boxes[box_idx]`.
3. If no conflicts are detected across all 81 cells, return `true`.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **Time Complexity** | $O(1)$ | Strictly 81 cell inspections, independent of input configuration. |
| **Space Complexity** | $O(1)$ | Fixed 27 integer bitmask registers (three 9-element arrays). |

## Common Traps & Edge Cases

- **Index Calculation for 3x3 Boxes**: Ensure integer division is used; calculating $(r / 3) \times 3 + (c / 3)$ correctly partitions the 81 cells into 9 contiguous blocks.
- **Valid vs Solvable**: The problem does not ask whether a full solution exists (which is NP-complete); only the current board markings must be checked for duplicates.
- **Empty Cells**: Empty cells are represented by `'.'` and must be skipped without triggering validation failure.

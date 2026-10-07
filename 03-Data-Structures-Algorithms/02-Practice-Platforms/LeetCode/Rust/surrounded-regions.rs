/*
 * Problem: LeetCode 130 - Surrounded Regions
 * Difficulty: Medium
 * Concepts: Graph, Matrix, DFS, BFS
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n) recursion stack
 */

pub struct Solution;

impl Solution {
    pub fn solve(board: &mut Vec<Vec<char>>) {
        if board.is_empty() || board[0].is_empty() {
            return;
        }

        let m = board.len();
        let n = board[0].len();

        for i in 0..m {
            Self::dfs(board, i as i32, 0, m, n);
            Self::dfs(board, i as i32, (n - 1) as i32, m, n);
        }
        for j in 0..n {
            Self::dfs(board, 0, j as i32, m, n);
            Self::dfs(board, (m - 1) as i32, j as i32, m, n);
        }

        for i in 0..m {
            for j in 0..n {
                if board[i][j] == 'O' {
                    board[i][j] = 'X';
                } else if board[i][j] == '#' {
                    board[i][j] = 'O';
                }
            }
        }
    }

    fn dfs(board: &mut Vec<Vec<char>>, r: i32, c: i32, m: usize, n: usize) {
        if r < 0 || r >= m as i32 || c < 0 || c >= n as i32 {
            return;
        }

        let ur = r as usize;
        let uc = c as usize;

        if board[ur][uc] != 'O' {
            return;
        }

        board[ur][uc] = '#';
        Self::dfs(board, r + 1, c, m, n);
        Self::dfs(board, r - 1, c, m, n);
        Self::dfs(board, r, c + 1, m, n);
        Self::dfs(board, r, c - 1, m, n);
    }
}

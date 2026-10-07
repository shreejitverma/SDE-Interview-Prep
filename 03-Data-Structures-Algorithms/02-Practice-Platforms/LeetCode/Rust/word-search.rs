/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N * 4^L) worst case, heavily pruned in practice
// Space: O(L) recursion stack

impl Solution {
    pub fn exist(mut board: Vec<Vec<char>>, word: String) -> bool {
        let m = board.len();
        if m == 0 {
            return false;
        }
        let n = board[0].len();
        let word_len = word.len();
        if m * n < word_len {
            return false;
        }

        let mut board_freq = [0i32; 128];
        for r in 0..m {
            for c in 0..n {
                board_freq[board[r][c] as usize] += 1;
            }
        }

        let word_bytes = word.as_bytes();
        for &b in word_bytes {
            board_freq[b as usize] -= 1;
            if board_freq[b as usize] < 0 {
                return false;
            }
        }

        let target: Vec<char> = if board_freq[word_bytes[0] as usize]
            > board_freq[word_bytes[word_len - 1] as usize]
        {
            word.chars().rev().collect()
        } else {
            word.chars().collect()
        };

        fn dfs(
            board: &mut Vec<Vec<char>>,
            r: usize,
            c: usize,
            target: &[char],
            idx: usize,
            m: usize,
            n: usize,
        ) -> bool {
            if idx == target.len() {
                return true;
            }
            if board[r][c] != target[idx] {
                return false;
            }
            if idx + 1 == target.len() {
                return true;
            }

            let temp = board[r][c];
            board[r][c] = '#';

            let mut found = false;
            let dirs = [(0isize, 1isize), (0, -1), (1, 0), (-1, 0)];
            for (dr, dc) in dirs {
                let nr = r as isize + dr;
                let nc = c as isize + dc;
                if nr >= 0 && nr < m as isize && nc >= 0 && nc < n as isize {
                    if dfs(board, nr as usize, nc as usize, target, idx + 1, m, n) {
                        found = true;
                        break;
                    }
                }
            }

            board[r][c] = temp;
            found
        }

        for r in 0..m {
            for c in 0..n {
                if board[r][c] == target[0] && dfs(&mut board, r, c, &target, 0, m, n) {
                    return true;
                }
            }
        }

        false
    }
}

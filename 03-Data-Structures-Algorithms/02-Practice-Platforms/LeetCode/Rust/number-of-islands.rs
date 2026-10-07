/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m * n)
// Space: O(m * n)

impl Solution {
    pub fn num_islands(mut grid: Vec<Vec<char>>) -> i32 {
        if grid.is_empty() {
            return 0;
        }
        let rows = grid.len();
        let cols = grid[0].len();
        let mut count = 0;

        for r in 0..rows {
            for c in 0..cols {
                if grid[r][c] == '1' {
                    count += 1;
                    Self::dfs(&mut grid, r, c, rows, cols);
                }
            }
        }
        count
    }

    fn dfs(grid: &mut Vec<Vec<char>>, r: usize, c: usize, rows: usize, cols: usize) {
        if grid[r][c] != '1' {
            return;
        }
        grid[r][c] = '0';
        if r + 1 < rows {
            Self::dfs(grid, r + 1, c, rows, cols);
        }
        if r > 0 {
            Self::dfs(grid, r - 1, c, rows, cols);
        }
        if c + 1 < cols {
            Self::dfs(grid, r, c + 1, rows, cols);
        }
        if c > 0 {
            Self::dfs(grid, r, c - 1, rows, cols);
        }
    }
}

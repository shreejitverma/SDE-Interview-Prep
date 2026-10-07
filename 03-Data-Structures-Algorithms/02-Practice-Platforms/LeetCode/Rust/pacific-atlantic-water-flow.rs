/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

pub struct Solution;

impl Solution {
    pub fn pacific_atlantic(heights: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        if heights.is_empty() || heights[0].is_empty() {
            return Vec::new();
        }

        let m = heights.len();
        let n = heights[0].len();
        let mut pacific = vec![vec![false; n]; m];
        let mut atlantic = vec![vec![false; n]; m];

        for i in 0..m {
            Self::dfs(&heights, i, 0, heights[i][0], &mut pacific);
            Self::dfs(&heights, i, n - 1, heights[i][n - 1], &mut atlantic);
        }
        for j in 0..n {
            Self::dfs(&heights, 0, j, heights[0][j], &mut pacific);
            Self::dfs(&heights, m - 1, j, heights[m - 1][j], &mut atlantic);
        }

        let mut result = Vec::new();
        for i in 0..m {
            for j in 0..n {
                if pacific[i][j] && atlantic[i][j] {
                    result.push(vec![i as i32, j as i32]);
                }
            }
        }

        result
    }

    fn dfs(
        heights: &[Vec<i32>],
        r: usize,
        c: usize,
        prev_val: i32,
        reachable: &mut [Vec<bool>],
    ) {
        let m = heights.len();
        let n = heights[0].len();

        if reachable[r][c] || heights[r][c] < prev_val {
            return;
        }

        reachable[r][c] = true;

        let dr = [-1, 1, 0, 0];
        let dc = [0, 0, -1, 1];

        for i in 0..4 {
            let nr = r as i32 + dr[i];
            let nc = c as i32 + dc[i];

            if nr >= 0 && nr < m as i32 && nc >= 0 && nc < n as i32 {
                Self::dfs(heights, nr as usize, nc as usize, heights[r][c], reachable);
            }
        }
    }
}

/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 994 - Rotting Oranges
 * Language: Rust
 *
 * Complexity:
 * - Time: O(M * N)
 * - Space: O(M * N)
 */

use std::collections::VecDeque;

pub struct Solution;

impl Solution {
    pub fn oranges_rotting(mut grid: Vec<Vec<i32>>) -> i32 {
        if grid.is_empty() || grid[0].is_empty() {
            return 0;
        }

        let m = grid.len();
        let n = grid[0].len();
        let mut queue: VecDeque<(usize, usize)> = VecDeque::new();
        let mut fresh = 0;

        for r in 0..m {
            for c in 0..n {
                if grid[r][c] == 2 {
                    queue.push_back((r, c));
                } else if grid[r][c] == 1 {
                    fresh += 1;
                }
            }
        }

        if fresh == 0 {
            return 0;
        }

        let mut minutes = 0;
        let directions: [(isize, isize); 4] = [(-1, 0), (1, 0), (0, -1), (0, 1)];

        while !queue.is_empty() && fresh > 0 {
            let sz = queue.len();
            for _ in 0..sz {
                let (r, c) = queue.pop_front().unwrap();

                for &(dr, dc) in &directions {
                    let nr = r as isize + dr;
                    let nc = c as isize + dc;

                    if nr >= 0 && (nr as usize) < m && nc >= 0 && (nc as usize) < n {
                        let ur = nr as usize;
                        let uc = nc as usize;
                        if grid[ur][uc] == 1 {
                            grid[ur][uc] = 2;
                            fresh -= 1;
                            queue.push_back((ur, uc));
                        }
                    }
                }
            }
            minutes += 1;
        }

        if fresh == 0 { minutes } else { -1 }
    }
}

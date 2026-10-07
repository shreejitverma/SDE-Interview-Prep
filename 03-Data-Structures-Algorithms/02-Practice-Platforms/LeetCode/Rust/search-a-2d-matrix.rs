/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 74 - Search a 2D Matrix
 * Language: Rust
 *
 * Complexity:
 * - Time: O(log(M * N))
 * - Space: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn search_matrix(matrix: Vec<Vec<i32>>, target: i32) -> bool {
        if matrix.is_empty() || matrix[0].is_empty() {
            return false;
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut left: i32 = 0;
        let mut right: i32 = (m * n - 1) as i32;

        while left <= right {
            let mid = left + (right - left) / 2;
            let row = (mid as usize) / n;
            let col = (mid as usize) % n;
            let val = matrix[row][col];

            if val == target {
                return true;
            } else if val < target {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        false
    }
}

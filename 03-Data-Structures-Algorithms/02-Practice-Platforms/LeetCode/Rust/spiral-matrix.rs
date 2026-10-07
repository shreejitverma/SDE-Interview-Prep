/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m * n)
// Space: O(1) auxiliary

impl Solution {
    pub fn spiral_order(matrix: Vec<Vec<i32>>) -> Vec<i32> {
        if matrix.is_empty() || matrix[0].is_empty() {
            return Vec::new();
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut result = Vec::with_capacity(m * n);

        let mut top = 0;
        let mut bottom = m as i32 - 1;
        let mut left = 0;
        let mut right = n as i32 - 1;

        while top <= bottom && left <= right {
            for col in left..=right {
                result.push(matrix[top as usize][col as usize]);
            }
            top += 1;

            for row in top..=bottom {
                result.push(matrix[row as usize][right as usize]);
            }
            right -= 1;

            if top <= bottom {
                for col in (left..=right).rev() {
                    result.push(matrix[bottom as usize][col as usize]);
                }
                bottom -= 1;
            }

            if left <= right {
                for row in (top..=bottom).rev() {
                    result.push(matrix[row as usize][left as usize]);
                }
                left += 1;
            }
        }

        result
    }
}

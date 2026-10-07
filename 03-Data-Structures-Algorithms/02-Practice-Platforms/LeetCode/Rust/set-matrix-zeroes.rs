/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn set_zeroes(matrix: &mut Vec<Vec<i32>>) {
        if matrix.is_empty() || matrix[0].is_empty() {
            return;
        }

        let m = matrix.len();
        let n = matrix[0].len();
        let mut first_col_zero = false;

        for i in 0..m {
            if matrix[i][0] == 0 {
                first_col_zero = true;
            }
            for j in 1..n {
                if matrix[i][j] == 0 {
                    matrix[i][0] = 0;
                    matrix[0][j] = 0;
                }
            }
        }

        for i in 1..m {
            for j in 1..n {
                if matrix[i][0] == 0 || matrix[0][j] == 0 {
                    matrix[i][j] = 0;
                }
            }
        }

        if matrix[0][0] == 0 {
            for j in 0..n {
                matrix[0][j] = 0;
            }
        }

        if first_col_zero {
            for i in 0..m {
                matrix[i][0] = 0;
            }
        }
    }
}

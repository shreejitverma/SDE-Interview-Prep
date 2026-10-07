/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N^2)
// Space: O(1) auxiliary

impl Solution {
    pub fn rotate(matrix: &mut Vec<Vec<i32>>) {
        let n = matrix.len();

        // Step 1: Transpose matrix
        for i in 0..n {
            for j in (i + 1)..n {
                let temp = matrix[i][j];
                matrix[i][j] = matrix[j][i];
                matrix[j][i] = temp;
            }
        }

        // Step 2: Reverse each row horizontally
        for i in 0..n {
            matrix[i].reverse();
        }
    }
}

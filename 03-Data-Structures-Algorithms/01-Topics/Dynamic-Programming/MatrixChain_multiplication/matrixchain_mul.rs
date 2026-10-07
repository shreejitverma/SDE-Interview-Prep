/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N^3)
// Space Complexity: O(N^2)

use std::cmp;

pub fn matrix_chain_order(p: &[i32]) -> i32 {
    let n = p.len() - 1;
    let mut dp = vec![vec![0i32; n + 1]; n + 1];

    for len in 2..=n {
        for i in 1..=(n - len + 1) {
            let j = i + len - 1;
            dp[i][j] = i32::MAX;
            for k in i..j {
                let cost = dp[i][k] + dp[k + 1][j] + p[i - 1] * p[k] * p[j];
                dp[i][j] = cmp::min(dp[i][j], cost);
            }
        }
    }

    dp[1][n]
}

fn main() {
    let arr = [1, 2, 3, 4, 3];
    println!("Minimum number of multiplications is {}", matrix_chain_order(&arr));
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N * W)
// Space Complexity: O(W) with 1D array space optimization

use std::cmp;

pub fn knapsack01(weights: &[usize], values: &[i32], capacity: usize) -> i32 {
    let n = weights.len();
    let mut dp = vec![0i32; capacity + 1];

    for i in 0..n {
        let wt = weights[i];
        let val = values[i];
        for w in (wt..=capacity).rev() {
            dp[w] = cmp::max(dp[w], dp[w - wt] + val);
        }
    }

    dp[capacity]
}

fn main() {
    let weights = [10, 20, 30];
    let values = [60, 100, 120];
    let capacity = 50;

    println!("Max value in 0-1 Knapsack: {}", knapsack01(&weights, &values, capacity)); // 220
}

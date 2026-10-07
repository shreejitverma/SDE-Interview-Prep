/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(min(M, N))
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn unique_paths(m: i32, n: i32) -> i32 {
        let total_steps = (m + n - 2) as i64;
        let k = (m - 1).min(n - 1) as i64;
        let mut result = 1i64;

        for i in 1..=k {
            result = result * (total_steps - k + i) / i;
        }

        result as i32
    }
}

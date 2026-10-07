/*
 * Problem: LeetCode 72 - Edit Distance
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(min(m, n))
 */

pub struct Solution;

impl Solution {
    pub fn min_distance(word1: String, word2: String) -> i32 {
        let (w1, w2) = if word1.len() < word2.len() {
            (word2.as_bytes().to_vec(), word1.as_bytes().to_vec())
        } else {
            (word1.as_bytes().to_vec(), word2.as_bytes().to_vec())
        };

        let m = w1.len();
        let n = w2.len();

        let mut dp: Vec<i32> = (0..=n as i32).collect();

        for i in 1..=m {
            let mut prev_diag = dp[0];
            dp[0] = i as i32;

            for j in 1..=n {
                let temp = dp[j];
                if w1[i - 1] == w2[j - 1] {
                    dp[j] = prev_diag;
                } else {
                    dp[j] = 1 + prev_diag.min(dp[j]).min(dp[j - 1]);
                }
                prev_diag = temp;
            }
        }

        dp[n]
    }
}

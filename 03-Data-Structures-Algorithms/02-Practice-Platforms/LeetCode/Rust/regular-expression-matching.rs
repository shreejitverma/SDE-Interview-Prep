/*
 * Problem: LeetCode 10 - Regular Expression Matching
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String, Recursion
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n)
 */

pub struct Solution;

impl Solution {
    pub fn is_match(s: String, p: String) -> bool {
        let s_bytes = s.as_bytes();
        let p_bytes = p.as_bytes();
        let m = s_bytes.len();
        let n = p_bytes.len();

        let mut dp = vec![vec![false; n + 1]; m + 1];
        dp[0][0] = true;

        for j in 2..=n {
            if p_bytes[j - 1] == b'*' {
                dp[0][j] = dp[0][j - 2];
            }
        }

        for i in 1..=m {
            for j in 1..=n {
                if p_bytes[j - 1] == b'*' {
                    dp[i][j] = dp[i][j - 2];
                    let prev = p_bytes[j - 2];
                    if prev == b'.' || prev == s_bytes[i - 1] {
                        dp[i][j] = dp[i][j] || dp[i - 1][j];
                    }
                } else if p_bytes[j - 1] == b'.' || p_bytes[j - 1] == s_bytes[i - 1] {
                    dp[i][j] = dp[i - 1][j - 1];
                }
            }
        }

        dp[m][n]
    }
}

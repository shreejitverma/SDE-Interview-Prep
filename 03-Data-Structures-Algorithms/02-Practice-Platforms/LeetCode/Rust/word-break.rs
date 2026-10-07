/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * L_max) where L_max is maximum word length
// Space: O(N + M) auxiliary

use std::collections::HashSet;
use std::cmp;

impl Solution {
    pub fn word_break(s: String, word_dict: Vec<String>) -> bool {
        let max_len = word_dict.iter().map(|w| w.len()).max().unwrap_or(0);
        let dict: HashSet<&str> = word_dict.iter().map(|w| w.as_str()).collect();

        let n = s.len();
        let mut dp = vec![false; n + 1];
        dp[0] = true;

        for i in 1..=n {
            let start = i.saturating_sub(max_len);
            for j in (start..i).rev() {
                if dp[j] && dict.contains(&s[j..i]) {
                    dp[i] = true;
                    break;
                }
            }
        }

        dp[n]
    }
}

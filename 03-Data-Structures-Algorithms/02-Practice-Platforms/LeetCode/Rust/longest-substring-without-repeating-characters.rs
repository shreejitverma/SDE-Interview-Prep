/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(min(m, n))

use std::collections::HashMap;

impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let mut lookup = HashMap::new();
        let mut left = 0;
        let mut max_len = 0;
        for (right, c) in s.chars().enumerate() {
            if let Some(&prev_idx) = lookup.get(&c) {
                left = left.max(prev_idx + 1);
            }
            lookup.insert(c, right);
            max_len = max_len.max(right - left + 1);
        }
        max_len as i32
    }
}

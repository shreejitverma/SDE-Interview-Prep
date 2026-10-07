/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn num_decodings(s: String) -> i32 {
        let bytes = s.as_bytes();
        if bytes.is_empty() || bytes[0] == b'0' {
            return 0;
        }

        let mut prev2 = 1i32;
        let mut prev1 = 1i32;

        for i in 1..bytes.len() {
            let mut current = 0i32;

            // Single digit decode
            if bytes[i] != b'0' {
                current += prev1;
            }

            // Two digit decode
            let two_digit = (bytes[i - 1] - b'0') * 10 + (bytes[i] - b'0');
            if (10..=26).contains(&two_digit) {
                current += prev2;
            }

            prev2 = prev1;
            prev1 = current;
        }

        prev1
    }
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn reverse_bits(mut x: u32) -> u32 {
        let mut result = 0;
        for _ in 0..32 {
            result = (result << 1) | (x & 1);
            x >>= 1;
        }
        result
    }
}

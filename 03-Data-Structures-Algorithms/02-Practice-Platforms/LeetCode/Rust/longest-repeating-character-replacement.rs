/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn character_replacement(s: String, k: i32) -> i32 {
        let bytes = s.as_bytes();
        let mut count = [0i32; 26];
        let mut max_count = 0;
        let mut left = 0;
        let mut max_len = 0;

        for right in 0..bytes.len() {
            let idx = (bytes[right] - b'A') as usize;
            count[idx] += 1;
            max_count = max_count.max(count[idx]);

            if (right - left + 1) as i32 - max_count > k {
                let left_idx = (bytes[left] - b'A') as usize;
                count[left_idx] -= 1;
                left += 1;
            }

            max_len = max_len.max((right - left + 1) as i32);
        }

        max_len
    }
}

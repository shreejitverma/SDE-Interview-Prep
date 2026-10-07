/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m + n)
// Space: O(1) auxiliary

impl Solution {
    pub fn min_window(s: String, t: String) -> String {
        if s.len() < t.len() {
            return String::new();
        }

        let s_bytes = s.as_bytes();
        let t_bytes = t.as_bytes();

        let mut count = [0i32; 128];
        for &b in t_bytes {
            count[b as usize] += 1;
        }

        let mut remain = t_bytes.len() as i32;
        let mut left = 0usize;
        let mut min_start = 0usize;
        let mut min_len = usize::MAX;

        for right in 0..s_bytes.len() {
            let r_char = s_bytes[right] as usize;
            if count[r_char] > 0 {
                remain -= 1;
            }
            count[r_char] -= 1;

            while remain == 0 {
                let win_len = right - left + 1;
                if win_len < min_len {
                    min_len = win_len;
                    min_start = left;
                }

                let l_char = s_bytes[left] as usize;
                count[l_char] += 1;
                if count[l_char] > 0 {
                    remain += 1;
                }
                left += 1;
            }
        }

        if min_len == usize::MAX {
            String::new()
        } else {
            s[min_start..min_start + min_len].to_string()
        }
    }
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

impl Solution {
    pub fn longest_palindrome(s: String) -> String {
        if s.len() <= 1 {
            return s;
        }

        let mut t: Vec<u8> = Vec::with_capacity(2 * s.len() + 3);
        t.push(b'^');
        for &b in s.as_bytes() {
            t.push(b'#');
            t.push(b);
        }
        t.push(b'#');
        t.push(b'$');

        let n = t.len();
        let mut p = vec![0usize; n];
        let mut c = 0usize;
        let mut r = 0usize;

        for i in 1..n - 1 {
            let i_mirror = if 2 * c >= i { 2 * c - i } else { 0 };
            if r > i {
                p[i] = (r - i).min(p[i_mirror]);
            }
            while t[i + 1 + p[i]] == t[i - 1 - p[i]] {
                p[i] += 1;
            }
            if i + p[i] > r {
                c = i;
                r = i + p[i];
            }
        }

        let mut max_len = 0usize;
        let mut center_index = 0usize;
        for i in 1..n - 1 {
            if p[i] > max_len {
                max_len = p[i];
                center_index = i;
            }
        }

        let start = (center_index - max_len) / 2;
        s[start..start + max_len].to_string()
    }
}

//! Author: Shreejit Verma
//! GitHub: https://github.com/shreejitverma
//!
//! Problem: LeetCode 647 - Palindromic Substrings
//! Difficulty: Medium
//! Language: Rust
//!
//! Performance Analysis:
//! - Time Complexity: O(N) linear time using Manacher's algorithm; O(N^2) center expansion.
//! - Space Complexity: O(N) auxiliary space for transformed buffer and radius vector.

pub struct Solution;

impl Solution {
    pub fn count_substrings(s: String) -> i32 {
        if s.is_empty() {
            return 0;
        }

        // Preprocess string into "^#a#b#c#$" format
        let mut t = Vec::with_capacity(s.len() * 2 + 3);
        t.push(b'^');
        for &b in s.as_bytes() {
            t.push(b'#');
            t.push(b);
        }
        t.push(b'#');
        t.push(b'$');

        let n = t.len();
        let mut p = vec![0usize; n];
        let mut center = 0usize;
        let mut right = 0usize;
        let mut total = 0i32;

        for i in 1..n - 1 {
            let i_mirror = 2 * center - i;
            if right > i {
                p[i] = (right - i).min(p[i_mirror]);
            } else {
                p[i] = 0;
            }

            while t[i + 1 + p[i]] == t[i - 1 - p[i]] {
                p[i] += 1;
            }

            if i + p[i] > right {
                center = i;
                right = i + p[i];
            }

            total += ((p[i] + 1) / 2) as i32;
        }

        total
    }

    pub fn count_substrings_expand(s: String) -> i32 {
        let bytes = s.as_bytes();
        let n = bytes.len();
        let mut total = 0i32;

        let expand = |mut l: usize, mut r: usize| -> i32 {
            let mut count = 0;
            while r < n && bytes[l] == bytes[r] {
                count += 1;
                if l == 0 {
                    break;
                }
                l -= 1;
                r += 1;
            }
            count
        };

        for i in 0..n {
            total += expand(i, i);
            total += expand(i, i + 1);
        }

        total
    }
}

fn main() {
    println!("count_substrings('abc'): {}", Solution::count_substrings("abc".to_string())); // 3
    println!("count_substrings('aaa'): {}", Solution::count_substrings("aaa".to_string())); // 6
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(M * N)
// Space Complexity: O(min(M, N)) with 1D row rolling array

use std::cmp;

pub fn longest_common_subsequence(text1: &str, text2: &str) -> i32 {
    let (t1, t2) = if text1.len() < text2.len() {
        (text2.as_bytes(), text1.as_bytes())
    } else {
        (text1.as_bytes(), text2.as_bytes())
    };

    let m = t1.len();
    let n = t2.len();
    let mut prev = vec![0i32; n + 1];
    let mut curr = vec![0i32; n + 1];

    for i in 1..=m {
        for j in 1..=n {
            if t1[i - 1] == t2[j - 1] {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = cmp::max(prev[j], curr[j - 1]);
            }
        }
        prev.copy_from_slice(&curr);
        curr.fill(0);
    }

    prev[n]
}

fn main() {
    println!("LCS('abcde', 'ace'): {}", longest_common_subsequence("abcde", "ace")); // 3
    println!("LCS('abc', 'abc'): {}", longest_common_subsequence("abc", "abc"));     // 3
    println!("LCS('abc', 'def'): {}", longest_common_subsequence("abc", "def"));     // 0
}

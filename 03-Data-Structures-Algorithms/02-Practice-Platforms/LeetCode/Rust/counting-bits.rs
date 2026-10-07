//! Author: Shreejit Verma
//! GitHub: https://github.com/shreejitverma
//!
//! Problem: LeetCode 338 - Counting Bits
//! Difficulty: Easy
//! Language: Rust
//!
//! Performance Analysis:
//! - Time Complexity: O(N) linear single-pass dynamic programming.
//! - Space Complexity: O(1) auxiliary space (excluding output vector).

pub struct Solution;

impl Solution {
    pub fn count_bits(n: i32) -> Vec<i32> {
        let n = n as usize;
        let mut ans = vec![0i32; n + 1];
        for i in 1..=n {
            ans[i] = ans[i >> 1] + (i as i32 & 1);
        }
        ans
    }

    pub fn count_bits_kernighan(n: i32) -> Vec<i32> {
        let n = n as usize;
        let mut ans = vec![0i32; n + 1];
        for i in 1..=n {
            ans[i] = ans[i & (i - 1)] + 1;
        }
        ans
    }
}

fn main() {
    println!("count_bits(2): {:?}", Solution::count_bits(2)); // [0, 1, 1]
    println!("count_bits(5): {:?}", Solution::count_bits(5)); // [0, 1, 1, 2, 1, 2]
}

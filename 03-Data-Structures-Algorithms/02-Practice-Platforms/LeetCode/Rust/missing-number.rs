//! Author: Shreejit Verma
//! GitHub: https://github.com/shreejitverma
//!
//! Problem: LeetCode 268 - Missing Number
//! Difficulty: Easy
//! Language: Rust
//!
//! Performance Analysis:
//! - Time Complexity: O(N) single-pass bitwise XOR fold.
//! - Space Complexity: O(1) auxiliary space.

pub struct Solution;

impl Solution {
    pub fn missing_number(nums: Vec<i32>) -> i32 {
        let n = nums.len() as i32;
        nums.iter()
            .enumerate()
            .fold(n, |acc, (i, &val)| acc ^ (i as i32) ^ val)
    }

    pub fn missing_number_gauss(nums: Vec<i32>) -> i32 {
        let n = nums.len() as i32;
        let expected = n * (n + 1) / 2;
        let actual: i32 = nums.iter().sum();
        expected - actual
    }
}

fn main() {
    let nums1 = vec![3, 0, 1];
    let nums2 = vec![0, 1];
    let nums3 = vec![9, 6, 4, 2, 3, 5, 7, 0, 1];
    println!("Missing 1: {}", Solution::missing_number(nums1)); // 2
    println!("Missing 2: {}", Solution::missing_number(nums2)); // 2
    println!("Missing 3: {}", Solution::missing_number(nums3)); // 8
}

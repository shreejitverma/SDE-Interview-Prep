/*
 * Problem: LeetCode 704 - Binary Search
 * Difficulty: Easy
 * Concepts: Binary Search, Array
 *
 * Time Complexity: O(log n)
 * Space Complexity: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let mut left: i32 = 0;
        let mut right: i32 = nums.len() as i32 - 1;

        while left <= right {
            let mid = left + (right - left) / 2;
            let val = nums[mid as usize];
            if val == target {
                return mid;
            } else if val < target {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        -1
    }
}

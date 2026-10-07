/*
 * Problem: LeetCode 45 - Jump Game II
 * Difficulty: Medium
 * Concepts: Greedy, BFS, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn jump(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        if n <= 1 {
            return 0;
        }

        let mut jumps = 0;
        let mut current_end = 0;
        let mut farthest = 0;

        for i in 0..(n - 1) {
            farthest = farthest.max(i + nums[i] as usize);
            if i == current_end {
                jumps += 1;
                current_end = farthest;
                if current_end >= n - 1 {
                    break;
                }
            }
        }

        jumps
    }
}

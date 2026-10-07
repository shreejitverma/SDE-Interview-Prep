/*
 * Problem: LeetCode 287 - Find the Duplicate Number
 * Difficulty: Medium
 * Concepts: Two Pointers, Floyd's Cycle Detection, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn find_duplicate(nums: Vec<i32>) -> i32 {
        let mut slow = nums[0] as usize;
        let mut fast = nums[nums[0] as usize] as usize;

        // Phase 1: Detect cycle intersection
        while slow != fast {
            slow = nums[slow] as usize;
            fast = nums[nums[fast] as usize] as usize;
        }

        // Phase 2: Find cycle entrance
        fast = 0;
        while slow != fast {
            slow = nums[slow] as usize;
            fast = nums[fast] as usize;
        }

        slow as i32
    }
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        if nums.is_empty() {
            return 0;
        }
        if nums.len() == 1 {
            return nums[0];
        }

        Self::rob_range(&nums, 0, nums.len() - 1)
            .max(Self::rob_range(&nums, 1, nums.len()))
    }

    fn rob_range(nums: &[i32], start: usize, end: usize) -> i32 {
        let mut prev2 = 0i32;
        let mut prev1 = 0i32;

        for i in start..end {
            let current = prev1.max(prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        prev1
    }
}

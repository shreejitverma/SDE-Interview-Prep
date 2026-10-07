/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n^2)
// Space: O(1)

impl Solution {
    pub fn three_sum(mut nums: Vec<i32>) -> Vec<Vec<i32>> {
        let mut result = Vec::new();
        nums.sort_unstable();
        let n = nums.len();
        if n < 3 {
            return result;
        }
        for i in 0..n - 2 {
            if nums[i] > 0 {
                break;
            }
            if i > 0 && nums[i] == nums[i - 1] {
                continue;
            }
            let mut left = i + 1;
            let mut right = n - 1;
            while left < right {
                let sum = nums[i] + nums[left] + nums[right];
                if sum == 0 {
                    result.push(vec![nums[i], nums[left], nums[right]]);
                    while left < right && nums[left] == nums[left + 1] {
                        left += 1;
                    }
                    while left < right && nums[right] == nums[right - 1] {
                        right -= 1;
                    }
                    left += 1;
                    right -= 1;
                } else if sum < 0 {
                    left += 1;
                } else {
                    right -= 1;
                }
            }
        }
        result
    }
}

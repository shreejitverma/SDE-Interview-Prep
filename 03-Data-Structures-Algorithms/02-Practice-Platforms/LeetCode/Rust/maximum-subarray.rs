/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut current_sum = nums[0];
        let mut max_sum = nums[0];
        for &x in nums.iter().skip(1) {
            current_sum = x.max(current_sum + x);
            max_sum = max_sum.max(current_sum);
        }
        max_sum
    }
}

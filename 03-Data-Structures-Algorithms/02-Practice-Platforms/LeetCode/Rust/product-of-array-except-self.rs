/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn product_except_self(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut result = vec![1; n];
        for i in 1..n {
            result[i] = result[i - 1] * nums[i - 1];
        }
        let mut right = 1;
        for i in (0..n).rev() {
            result[i] *= right;
            right *= nums[i];
        }
        result
    }
}

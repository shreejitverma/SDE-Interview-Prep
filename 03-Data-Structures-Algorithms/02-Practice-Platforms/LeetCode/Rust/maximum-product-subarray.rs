/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let mut max_prod = nums[0];
        let mut min_prod = nums[0];
        let mut result = nums[0];
        for &x in nums.iter().skip(1) {
            if x < 0 {
                std::mem::swap(&mut max_prod, &mut min_prod);
            }
            max_prod = x.max(max_prod * x);
            min_prod = x.min(min_prod * x);
            result = result.max(max_prod);
        }
        result
    }
}

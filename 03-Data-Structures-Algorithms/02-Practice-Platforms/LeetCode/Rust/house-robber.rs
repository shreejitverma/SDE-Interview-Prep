/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        let mut prev2 = 0i32;
        let mut prev1 = 0i32;

        for num in nums {
            let current = prev1.max(prev2 + num);
            prev2 = prev1;
            prev1 = current;
        }

        prev1
    }
}

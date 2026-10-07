/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

pub struct Solution;

impl Solution {
    pub fn can_jump(nums: Vec<i32>) -> bool {
        let mut max_reachable = 0;
        let n = nums.len();

        for (i, &val) in nums.iter().enumerate() {
            if i > max_reachable {
                return false;
            }
            max_reachable = max_reachable.max(i + val as usize);
            if max_reachable >= n - 1 {
                return true;
            }
        }

        true
    }
}

/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

use std::collections::HashSet;

impl Solution {
    pub fn contains_duplicate(nums: Vec<i32>) -> bool {
        let mut lookup = HashSet::with_capacity(nums.len());
        for num in nums {
            if !lookup.insert(num) {
                return true;
            }
        }
        false
    }
}

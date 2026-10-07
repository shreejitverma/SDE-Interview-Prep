/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

use std::collections::HashMap;

pub struct Solution;

impl Solution {
    pub fn top_k_frequent(nums: Vec<i32>, k: i32) -> Vec<i32> {
        let n = nums.len();
        let mut counts = HashMap::new();
        for num in nums {
            *counts.entry(num).or_insert(0) += 1;
        }

        let mut buckets = vec![Vec::new(); n + 1];
        for (num, freq) in counts {
            buckets[freq].push(num);
        }

        let mut result = Vec::with_capacity(k as usize);
        for i in (1..=n).rev() {
            for &num in &buckets[i] {
                result.push(num);
                if result.len() == k as usize {
                    return result;
                }
            }
        }

        result
    }
}

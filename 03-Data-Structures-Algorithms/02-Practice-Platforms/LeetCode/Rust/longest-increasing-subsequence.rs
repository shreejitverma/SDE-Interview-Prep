/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(n)

impl Solution {
    pub fn length_of_lis(nums: Vec<i32>) -> i32 {
        if nums.is_empty() {
            return 0;
        }
        let mut tails: Vec<i32> = Vec::with_capacity(nums.len());
        for x in nums {
            let idx = match tails.binary_search(&x) {
                Ok(i) => i,
                Err(i) => i,
            };
            if idx == tails.len() {
                tails.push(x);
            } else {
                tails[idx] = x;
            }
        }
        tails.len() as i32
    }
}

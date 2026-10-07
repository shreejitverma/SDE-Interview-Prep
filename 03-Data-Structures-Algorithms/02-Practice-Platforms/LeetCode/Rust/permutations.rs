/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * N!)
// Space: O(N) auxiliary (recursion stack depth)

pub struct Solution;

impl Solution {
    pub fn permute(mut nums: Vec<i32>) -> Vec<Vec<i32>> {
        let mut result = Vec::new();
        Self::backtrack(&mut nums, 0, &mut result);
        result
    }

    fn backtrack(nums: &mut [i32], first: usize, result: &mut Vec<Vec<i32>>) {
        if first == nums.len() {
            result.push(nums.to_vec());
            return;
        }

        for i in first..nums.len() {
            nums.swap(first, i);
            Self::backtrack(nums, first + 1, result);
            nums.swap(first, i);
        }
    }
}

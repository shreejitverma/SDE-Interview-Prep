/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * 2^N)
// Space: O(N) auxiliary (recursion stack)

pub struct Solution;

impl Solution {
    pub fn subsets(nums: Vec<i32>) -> Vec<Vec<i32>> {
        let mut result = Vec::new();
        let mut path = Vec::new();
        Self::backtrack(&nums, 0, &mut path, &mut result);
        result
    }

    fn backtrack(nums: &[i32], start: usize, path: &mut Vec<i32>, result: &mut Vec<Vec<i32>>) {
        result.push(path.clone());

        for i in start..nums.len() {
            path.push(nums[i]);
            Self::backtrack(nums, i + 1, path, result);
            path.pop();
        }
    }
}

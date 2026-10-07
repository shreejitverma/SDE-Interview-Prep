/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 167 - Two Sum II - Input Array Is Sorted
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn two_sum(numbers: Vec<i32>, target: i32) -> Vec<i32> {
        let mut left: usize = 0;
        let mut right: usize = numbers.len() - 1;

        while left < right {
            let sum = numbers[left] + numbers[right];
            if sum == target {
                return vec![(left + 1) as i32, (right + 1) as i32];
            } else if sum < target {
                left += 1;
            } else {
                right -= 1;
            }
        }

        vec![-1, -1]
    }
}

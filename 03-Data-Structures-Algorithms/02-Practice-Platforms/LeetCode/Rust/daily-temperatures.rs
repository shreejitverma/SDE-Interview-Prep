/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 739 - Daily Temperatures
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(N)
 */

pub struct Solution;

impl Solution {
    pub fn daily_temperatures(temperatures: Vec<i32>) -> Vec<i32> {
        let n = temperatures.len();
        let mut result = vec![0; n];
        let mut stack: Vec<usize> = Vec::with_capacity(n);

        for i in 0..n {
            while let Some(&prev_idx) = stack.last() {
                if temperatures[prev_idx] < temperatures[i] {
                    stack.pop();
                    result[prev_idx] = (i - prev_idx) as i32;
                } else {
                    break;
                }
            }
            stack.push(i);
        }

        result
    }
}

/*
 * Problem: LeetCode 621 - Task Scheduler
 * Difficulty: Medium
 * Concepts: Greedy, Counting, Math
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn least_interval(tasks: Vec<char>, n: i32) -> i32 {
        let mut freq = [0; 26];
        let mut max_freq = 0;

        for &task in &tasks {
            let idx = (task as u8 - b'A') as usize;
            freq[idx] += 1;
            max_freq = max_freq.max(freq[idx]);
        }

        let mut max_count = 0;
        for &count in &freq {
            if count == max_freq {
                max_count += 1;
            }
        }

        let calculated = (max_freq - 1) * (n + 1) + max_count;
        (tasks.len() as i32).max(calculated)
    }
}

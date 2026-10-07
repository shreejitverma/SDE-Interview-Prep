/*
 * Problem: LeetCode 134 - Gas Station
 * Difficulty: Medium
 * Concepts: Greedy, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn can_complete_circuit(gas: Vec<i32>, cost: Vec<i32>) -> i32 {
        let mut total_tank = 0;
        let mut current_tank = 0;
        let mut start_index = 0;

        for i in 0..gas.len() {
            let balance = gas[i] - cost[i];
            total_tank += balance;
            current_tank += balance;

            if current_tank < 0 {
                start_index = (i + 1) as i32;
                current_tank = 0;
            }
        }

        if total_tank >= 0 {
            start_index
        } else {
            -1
        }
    }
}

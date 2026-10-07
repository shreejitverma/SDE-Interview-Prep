/*
 * Problem: LeetCode 494 - Target Sum
 * Difficulty: Medium
 * Concepts: Dynamic Programming, 0/1 Knapsack, Subset Sum
 *
 * Time Complexity: O(n * S) where S = (total_sum + target) / 2
 * Space Complexity: O(S)
 */

pub struct Solution;

impl Solution {
    pub fn find_target_sum_ways(nums: Vec<i32>, target: i32) -> i32 {
        let total_sum: i32 = nums.iter().sum();

        // sum(P) - sum(N) = target
        // sum(P) + sum(N) = total_sum
        // 2 * sum(P) = total_sum + target
        if total_sum < target.abs() || (total_sum + target) % 2 != 0 {
            return 0;
        }

        let subset_sum = ((total_sum + target) / 2) as usize;
        let mut dp = vec![0; subset_sum + 1];
        dp[0] = 1;

        for num in nums {
            let n = num as usize;
            for s in (n..=subset_sum).rev() {
                dp[s] += dp[s - n];
            }
        }

        dp[subset_sum]
    }
}

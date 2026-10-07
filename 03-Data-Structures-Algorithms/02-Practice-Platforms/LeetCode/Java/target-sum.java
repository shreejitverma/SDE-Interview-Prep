/*
 * Problem: LeetCode 494 - Target Sum
 * Difficulty: Medium
 * Concepts: Dynamic Programming, 0/1 Knapsack, Subset Sum
 *
 * Time Complexity: O(n * S) where S = (total_sum + target) / 2
 * Space Complexity: O(S)
 */

class Solution {
    public int findTargetSumWays(int[] nums, int target) {
        int totalSum = 0;
        for (int num : nums) {
            totalSum += num;
        }

        // sum(P) - sum(N) = target
        // sum(P) + sum(N) = totalSum
        // 2 * sum(P) = totalSum + target
        if (totalSum < Math.abs(target) || (totalSum + target) % 2 != 0) {
            return 0;
        }

        int subsetSum = (totalSum + target) / 2;
        int[] dp = new int[subsetSum + 1];
        dp[0] = 1;

        for (int num : nums) {
            for (int s = subsetSum; s >= num; s--) {
                dp[s] += dp[s - num];
            }
        }

        return dp[subsetSum];
    }
}

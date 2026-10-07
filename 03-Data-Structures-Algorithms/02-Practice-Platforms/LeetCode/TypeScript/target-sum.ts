/*
 * Problem: LeetCode 494 - Target Sum
 * Difficulty: Medium
 * Concepts: Dynamic Programming, 0/1 Knapsack, Subset Sum
 *
 * Time Complexity: O(n * S) where S = (total_sum + target) / 2
 * Space Complexity: O(S)
 */

export function findTargetSumWays(nums: number[], target: number): number {
    const totalSum = nums.reduce((acc, val) => acc + val, 0);

    // sum(P) - sum(N) = target
    // sum(P) + sum(N) = totalSum
    // 2 * sum(P) = totalSum + target
    if (totalSum < Math.abs(target) || (totalSum + target) % 2 !== 0) {
        return 0;
    }

    const subsetSum = (totalSum + target) / 2;
    const dp = new Array(subsetSum + 1).fill(0);
    dp[0] = 1;

    for (const num of nums) {
        for (let s = subsetSum; s >= num; s--) {
            dp[s] += dp[s - num];
        }
    }

    return dp[subsetSum];
}

"""
Problem: LeetCode 494 - Target Sum
Difficulty: Medium
Concepts: Dynamic Programming, 0/1 Knapsack, Subset Sum

Time Complexity: O(n * S) where S = (total_sum + target) // 2
Space Complexity: O(S)
"""

from typing import List


class Solution:
    def findTargetSumWays(self, nums: List[int], target: int) -> int:
        total_sum = sum(nums)

        # sum(P) - sum(N) = target
        # sum(P) + sum(N) = total_sum
        # 2 * sum(P) = total_sum + target
        if total_sum < abs(target) or (total_sum + target) % 2 != 0:
            return 0

        subset_sum = (total_sum + target) // 2
        dp = [0] * (subset_sum + 1)
        dp[0] = 1

        for num in nums:
            for s in range(subset_sum, num - 1, -1):
                dp[s] += dp[s - num]

        return dp[subset_sum]

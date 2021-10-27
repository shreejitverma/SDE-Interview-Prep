'''https://leetcode.com/problems/coin-change/
322. Coin Change
Medium

8774

223

Add to List

Share
You are given an integer array coins representing coins of different denominations and an integer amount representing a total amount of money.

Return the fewest number of coins that you need to make up that amount. If that amount of money cannot be made up by any combination of the coins, return -1.

You may assume that you have an infinite number of each kind of coin.

 

Example 1:

Input: coins = [1,2,5], amount = 11
Output: 3
Explanation: 11 = 5 + 5 + 1
Example 2:

Input: coins = [2], amount = 3
Output: -1
Example 3:

Input: coins = [1], amount = 0
Output: 0
Example 4:

Input: coins = [1], amount = 1
Output: 1
Example 5:

Input: coins = [1], amount = 2
Output: 2
 

Constraints:

1 <= coins.length <= 12
1 <= coins[i] <= 231 - 1
0 <= amount <= 104'''


# Time:  O(n * k), n is the number of coins, k is the amount of money
# Space: O(k)

class Solution(object):
    def coinChange(self, coins, amount):
        """
        :type coins: List[int]
        :type amount: int
        :rtype: int
        """
        INF = 0x7fffffff  # Using float("inf") would be slower.
        dp = [INF] * (amount + 1)
        dp[0] = 0
        for i in range(amount + 1):
            if dp[i] != INF:
                for coin in coins:
                    if i + coin <= amount:
                        dp[i + coin] = min(dp[i + coin], dp[i] + 1)
        return dp[amount] if dp[amount] != INF else -1

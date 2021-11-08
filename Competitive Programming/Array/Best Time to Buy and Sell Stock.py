'''https://leetcode.com/problems/best-time-to-buy-and-sell-stock/
121. Best Time to Buy and Sell Stock
Easy

11829

446

Add to List

Share
You are given an array prices where prices[i] is the price of a given stock on the ith day.

You want to maximize your profit by choosing a single day to buy one stock and choosing a different day in the future to sell that stock.

Return the maximum profit you can achieve from this transaction. If you cannot achieve any profit, return 0.

 

Example 1:

Input: prices = [7,1,5,3,6,4]
Output: 5
Explanation: Buy on day 2 (price = 1) and sell on day 5 (price = 6), profit = 6-1 = 5.
Note that buying on day 2 and selling on day 1 is not allowed because you must buy before you sell.
Example 2:

Input: prices = [7,6,4,3,1]
Output: 0
Explanation: In this case, no transactions are done and the max profit = 0.
 

Constraints:

1 <= prices.length <= 105
0 <= prices[i] <= 104'''

# Best Time to Buy and Sell Stock

# Example 1:

# Input: [7,1,5,3,6,4]
# Output: 5
# Explanation: Buy on day 2 (price = 1) and sell on day 5 (price = 6), profit = 6-1 = 5.
#              Not 7-1 = 6, as selling price needs to be larger than buying price.
# Example 2:

# Input: [7,6,4,3,1]
# Output: 0
# Explanation: In this case, no transaction is done, i.e. max profit = 0.


def BruteForce(prices):
    max_reward = 0
    for i in range(len(prices)):
        for j in range(i+1, len(prices)):
            max_reward = max(max_reward, prices[j]-prices[i])

    return max_reward


def one_pass(prices):
    min_price = 2**64
    max_reward = 0
    for price in prices:
        if price < min_price:
            min_price = price
        max_reward = max(price - min_price, max_reward)
    return max_reward


if __name__ == '__main__':
    inputs = [7, 1, 5, 3, 6, 4]
    print(BruteForce(inputs))
    print(one_pass(inputs))

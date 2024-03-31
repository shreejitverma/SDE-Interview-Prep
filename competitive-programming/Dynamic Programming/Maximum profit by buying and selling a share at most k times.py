'''https://practice.geeksforgeeks.org/problems/maximum-profit4657/1
https://www.geeksforgeeks.org/maximum-profit-by-buying-and-selling-a-share-at-most-k-times/
Maximum Profit 
Hard Accuracy: 64.63% Submissions: 8372 Points: 8
In the stock market, a person buys a stock and sells it on some future date. Given the stock prices of N days in an array A[ ] and a positive integer K, find out the maximum profit a person can make in at-most K transactions. A transaction is equivalent to (buying + selling) of a stock and new transaction can start only when the previous transaction has been completed.


Example 1:

Input: K = 2, N = 6
A = {10, 22, 5, 75, 65, 80}
Output: 87
Explaination: 
1st transaction: buy at 10 and sell at 22. 
2nd transaction : buy at 5 and sell at 80.
Example 2:

Input: K = 3, N = 4
A = {20, 580, 420, 900}
Output: 1040
Explaination: The trader can make at most 2 
transactions and giving him a profit of 1040.
Example 3:

Input: K = 1, N = 5
A = {100, 90, 80, 50, 25}
Output: 0
Explaination: Selling price is decreasing 
daily. So seller cannot have profit.

Your Task:
You do not need to read input or print anything. Your task is to complete the function maxProfit() which takes the values K, N and the array A[] as input parameters and returns the maximum profit.


Expected Time Complexity: O(N*K)
Expected Auxiliary Space: O(N*K)


Constraints:
1 ≤ N ≤ 500
1 ≤ K ≤ 200
1 ≤ A[ i ] ≤ 1000
'''

# Python3 program to find out maximum
# profit by buying and/ selling a share
# at most k times given the stock price
# of n days

# Function to find out maximum profit
# by buying & selling/ a share atmost
# k times given stock price of n days


def maxProfit(price, n, k):

    # Table to store results of subproblems
    # profit[t][i] stores maximum profit
    # using atmost t transactions up to
    # day i (including day i)
    profit = [[0 for i in range(n + 1)]
              for j in range(k + 1)]

    # Fill the table in bottom-up fashion
    for i in range(1, k + 1):
        prevDiff = float('-inf')

        for j in range(1, n):
            prevDiff = max(prevDiff,
                           profit[i - 1][j - 1] -
                           price[j - 1])
            profit[i][j] = max(profit[i][j - 1],
                               price[j] + prevDiff)

    return profit[k][n - 1]


# Driver Code
if __name__ == "__main__":

    k = 3
    price = [12, 14, 17, 10, 14, 13, 12, 15]
    n = len(price)

    print("Maximum profit is:",
          maxProfit(price, n, k))

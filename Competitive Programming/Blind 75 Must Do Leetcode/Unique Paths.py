'''https://leetcode.com/problems/unique-paths/
62. Unique Paths
Medium

6693

263

Add to List

Share
A robot is located at the top-left corner of a m x n grid (marked 'Start' in the diagram below).

The robot can only move either down or right at any point in time. The robot is trying to reach the bottom-right corner of the grid (marked 'Finish' in the diagram below).

How many possible unique paths are there?

 

Example 1:


Input: m = 3, n = 7
Output: 28
Example 2:

Input: m = 3, n = 2
Output: 3
Explanation:
From the top-left corner, there are a total of 3 ways to reach the bottom-right corner:
1. Right -> Down -> Down
2. Down -> Down -> Right
3. Down -> Right -> Down
Example 3:

Input: m = 7, n = 3
Output: 28
Example 4:

Input: m = 3, n = 3
Output: 6
 

Constraints:

1 <= m, n <= 100
It's guaranteed that the answer will be less than or equal to 2 * 109.'''

# [62] Unique Paths
#


class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        dp = [[0 for _ in range(n+1)] for _ in range(m+1)]
        dp[1] = [1 for _ in range(n+1)]
        for i in range(1, m+1):
            dp[i][1] = 1
        for i in range(1, m+1):
            for j in range(1, n+1):
                tmp = dp[i-1][j]+dp[i][j-1]
                dp[i][j] = tmp
        return dp[-1][-1]

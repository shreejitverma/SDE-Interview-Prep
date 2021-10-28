/*
https://leetcode.com/problems/unique-paths/
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
It's guaranteed that the answer will be less than or equal to 2 * 109.
*/

//Runtime: 4 ms, faster than 54.06% of C++ online submissions for Unique Paths.
//Memory Usage: 8.6 MB, less than 51.56% of C++ online submissions for Unique Paths.

class Solution
{
public:
    int uniquePaths(int m, int n)
    {
        vector<vector<int> > grid(m, vector<int>(n));
        // cout << m << " " << n << endl;
        //range of the length of diagonal: 0 ~ (max possible i) + (max possible j)
        for (int diag = 0; diag <= (n - 1) + (m - 1); diag++)
        {
            //i starts from the first position that j >= 0
            //i is increased in the loop and its upper bound is m-1
            //j + i must = diag
            //j is decreased in the loop and its lower bound is 0
            //dynamic programming
            for (int i = max(0, diag - (n - 1)), j = diag - i; i <= m - 1 && j >= 0; i++, j--)
            {
                // cout << i << " " << j << endl;
                if (i == 0 || j == 0)
                {
                    grid[i][j] = 1;
                }
                else
                {
                    grid[i][j] = grid[i - 1][j] + grid[i][j - 1];
                }
            }
        }

        return grid[m - 1][n - 1];
    }
};

//Runtime: 0 ms, faster than 100.00% of C++ online submissions for Unique Paths.
//Memory Usage: 6.5 MB, less than 100.00% of C++ online submissions for Unique Paths.
class Solution
{
public:
    int uniquePaths(int m, int n)
    {
        vector<vector<int> > dp(m + 1, vector(n + 1, 0));

        //base case
        dp[1][1] = 1;

        for (int i = 1; i <= m; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                if (i == 1 && j == 1)
                    continue;
                dp[i][j] = dp[i - 1][j] + dp[i][j - 1];
            }
        }

        return dp[m][n];
    }
};

//DP, O(n) space
//https://leetcode.com/problems/unique-paths/discuss/22954/C%2B%2B-DP
class Solution
{
public:
    int uniquePaths(int m, int n)
    {
        //base case: top boundary and left boundary
        // vector<vector<int>> dp(m, vector(n, 1));
        vector<int> row(n, 1);

        for (int i = 1; i < m; i++)
        {
            for (int j = 1; j < n; j++)
            {
                // dp[i][j] = dp[i-1][j] + dp[i][j-1];
                /*
                row[j] represnets for the value in last row,
                just like dp[i-1][j]
                */
                row[j] = row[j] + row[j - 1];
            }
        }

        // return dp[m-1][n-1];
        return row[n - 1];
    }
};
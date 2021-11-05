/*

https://www.geeksforgeeks.org/coin-change-dp-7/
https://practice.geeksforgeeks.org/problems/coin-change2448/1
Coin Change 
Medium Accuracy: 47.19% Submissions: 57001 Points: 4
Given a value N, find the number of ways to make change for N cents, if we have infinite supply of each of S = { S1, S2, .. , SM } valued coins.


Example 1:

Input:
n = 4 , m = 3
S[] = {1,2,3}
Output: 4
Explanation: Four Possible ways are:
{1,1,1,1},{1,1,2},{2,2},{1,3}.
Example 2:

Input:
n = 10 , m = 4
S[] ={2,5,3,6}
Output: 5
Explanation: Five Possible ways are:
{2,2,2,2,2}, {2,2,3,3}, {2,2,6}, {2,3,5} 
and {5,5}.

Your Task:
You don't need to read input or print anything. Your task is to complete the function count() which accepts an array S[] its size m and n as input parameter and returns the number of ways to make change for N cents.


Expected Time Complexity: O(m*n).
Expected Auxiliary Space: O(n).


Constraints:
1 <= n,m <= 103


*/

int count(int S[], int m, int n)
{
    // table[i] will be storing the number of solutions for
    // value i. We need n+1 rows as the table is constructed
    // in bottom up manner using the base case (n = 0)
    int table[n + 1];
    // Initialize all table values as 0
    memset(table, 0, sizeof(table));
    // Base case (If given value is 0)
    table[0] = 1;
    // Pick all coins one by one and update the table[] values
    // after the index greater than or equal to the value of the
    // picked coin
    for (int i = 0; i < m; i++)
        for (int j = S[i]; j <= n; j++)
            table[j] += table[j - S[i]];
    return table[n];
}
// Following is a Top Down DP Approach using memoization:
#include <bits/stdc++.h>
using namespace std;

int coinchange(vector<int> &a, int v, int n,
               vector<vector<int> > &dp)
{
    if (v == 0)
        return dp[n][v] = 1;
    if (n == 0)
        return 0;
    if (dp[n][v] != -1)
        return dp[n][v];
    if (a[n - 1] <= v)
    {
        // Either Pick this coin or not
        return dp[n][v] = coinchange(a, v - a[n - 1], n, dp) + coinchange(a, v, n - 1, dp);
    }
    else // We have no option but to leave this coin
        return dp[n][v] = coinchange(a, v, n - 1, dp);
}
int32_t main()
{
    int tc = 1;
    // cin >> tc;
    while (tc--)
    {
        int n, v;
        n = 3, v = 4;
        vector<int> a = {1, 2, 3};
        vector<vector<int> > dp(n + 1,
                                vector<int>(v + 1, -1));
        int res = coinchange(a, v, n, dp);
        cout << res << endl;
    }
}
// C++ program for coin change problem.
#include <bits/stdc++.h>

using namespace std;

int count(int S[], int m, int n)
{
    int i, j, x, y;

    // We need n+1 rows as the table
    // is constructed in bottom up
    // manner using the base case 0
    // value case (n = 0)
    int table[n + 1][m];

    // Fill the entries for 0
    // value case (n = 0)
    for (i = 0; i < m; i++)
        table[0][i] = 1;

    // Fill rest of the table entries
    // in bottom up manner
    for (i = 1; i < n + 1; i++)
    {
        for (j = 0; j < m; j++)
        {
            // Count of solutions including S[j]
            x = (i - S[j] >= 0) ? table[i - S[j]][j] : 0;

            // Count of solutions excluding S[j]
            y = (j >= 1) ? table[i][j - 1] : 0;

            // total count
            table[i][j] = x + y;
        }
    }
    return table[n][m - 1];
}

// Driver Code
int main()
{
    int arr[] = {1, 2, 3};
    int m = sizeof(arr) / sizeof(arr[0]);
    int n = 4;
    cout << count(arr, m, n);
    return 0;
}

// Recursive C++ program for
// coin change problem.
#include <bits/stdc++.h>
using namespace std;

// Returns the count of ways we can
// sum S[0...m-1] coins to get sum n
int count(int S[], int m, int n)
{

    // If n is 0 then there is 1 solution
    // (do not include any coin)
    if (n == 0)
        return 1;

    // If n is less than 0 then no
    // solution exists
    if (n < 0)
        return 0;

    // If there are no coins and n
    // is greater than 0, then no
    // solution exist
    if (m <= 0 && n >= 1)
        return 0;

    // count is sum of solutions (i)
    // including S[m-1] (ii) excluding S[m-1]
    return count(S, m - 1, n) +
           count(S, m, n - S[m - 1]);
}

// Driver code
int main()
{
    int i, j;
    int arr[] = {1, 2, 3};
    int m = sizeof(arr) / sizeof(arr[0]);

    cout << " " << count(arr, m, 4);

    return 0;
}

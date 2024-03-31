/*
https://practice.geeksforgeeks.org/problems/longest-common-substring1452/1
https://www.geeksforgeeks.org/longest-common-substring-dp-29/
Longest Common Substring 
Medium Accuracy: 52.09% Submissions: 32970 Points: 4
Given two strings. The task is to find the length of the longest common substring.


Example 1:

Input: S1 = "ABCDGH", S2 = "ACDGHR"
Output: 4
Explanation: The longest common substring
is "CDGH" which has length 4.
Example 2:

Input: S1 = "ABC", S2 "ACB"
Output: 1
Explanation: The longest common substrings
are "A", "B", "C" all having length 1.

Your Task:
You don't need to read input or print anything. Your task is to complete the function longestCommonSubstr() which takes the string S1, string S2 and their length n and m as inputs and returns the length of the longest common substring in S1 and S2.


Expected Time Complexity: O(n*m).
Expected Auxiliary Space: O(n*m).


Constraints:
1<=n, m<=1000
*/
class Solution
{
public:
    int longestCommonSubstr(string s1, string s2, int n, int m)
    {
        // code here

        int maxlen = 0;
        int dp[2][m + 1] = {0};
        for (int i = 0; i <= n; i++)
        {
            for (int j = 0; j <= m; j++)
            {
                if (i == 0 || j == 0)
                    dp[i % 2][j] = 0;
                else if (s1[i - 1] == s2[j - 1])
                {
                    dp[i % 2][j] = dp[(i - 1) % 2][j - 1] + 1;
                    maxlen = max(maxlen, dp[i % 2][j]);
                }
                else
                {
                    dp[i % 2][j] = 0;
                }
            }
        }
        return maxlen;
    }
};
// C++ implementation of the above approach
#include <bits/stdc++.h>
using namespace std;

// Function to find the length of the
// longest LCS
int LCSubStr(string s, string t, int n, int m)
{

    // Create DP table
    int dp[2][m + 1];
    int res = 0;

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= m; j++)
        {
            if (s[i - 1] == t[j - 1])
            {
                dp[i % 2][j] = dp[(i - 1) % 2][j - 1] + 1;
                if (dp[i % 2][j] > res)
                    res = dp[i % 2][j];
            }
            else
                dp[i % 2][j] = 0;
        }
    }
    return res;
}

// Driver Code
int main()
{
    string X = "OldSite:GeeksforGeeks.org";
    string Y = "NewSite:GeeksQuiz.com";

    int m = X.length();
    int n = Y.length();

    cout << LCSubStr(X, Y, m, n);
    return 0;
    cout << "GFG!";
    return 0;
}
// C++ program using to find length of the
// longest common substring  recursion
#include <iostream>

using namespace std;

string X, Y;

// Returns length of function f
// or longest common substring
// of X[0..m-1] and Y[0..n-1]
int lcs(int i, int j, int count)
{

    if (i == 0 || j == 0)
        return count;

    if (X[i - 1] == Y[j - 1])
    {
        count = lcs(i - 1, j - 1, count + 1);
    }
    count = max(count,
                max(lcs(i, j - 1, 0),
                    lcs(i - 1, j, 0)));
    return count;
}

// Driver code
int main()
{
    int n, m;

    X = "abcdxyz";
    Y = "xyzabcd";

    n = X.size();
    m = Y.size();

    cout << lcs(n, m, 0);

    return 0;
}

// C++ implementation of the above approach
#include <bits/stdc++.h>
using namespace std;

// Function to find the length of the
// longest LCS
int LCSubStr(string s, string t, int n, int m)
{

    // Create DP table
    int dp[2][m + 1];
    int res = 0;

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= m; j++)
        {
            if (s[i - 1] == t[j - 1])
            {
                dp[i % 2][j] = dp[(i - 1) % 2][j - 1] + 1;
                if (dp[i % 2][j] > res)
                    res = dp[i % 2][j];
            }
            else
                dp[i % 2][j] = 0;
        }
    }
    return res;
}

// Driver Code
int main()
{
    string X = "OldSite:GeeksforGeeks.org";
    string Y = "NewSite:GeeksQuiz.com";

    int m = X.length();
    int n = Y.length();

    cout << LCSubStr(X, Y, m, n);
    return 0;
    cout << "GFG!";
    return 0;
}
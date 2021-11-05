/*
https://practice.geeksforgeeks.org/problems/lcs-of-three-strings0028/1
https://www.geeksforgeeks.org/lcs-longest-common-subsequence-three-strings/
LCS of three strings 
Medium Accuracy: 54.38% Submissions: 10927 Points: 4
Given 3 strings A, B and C, the task is to find the longest common sub-sequence in all three given sequences.

Example 1:

Input:
A = "geeks", B = "geeksfor", 
C = "geeksforgeeks"
Output: 5
Explanation: "geeks"is the longest common
subsequence with length 5.
â€‹Example 2:

Input: 
A = "abcd", B = "efgh", C = "ijkl"
Output: 0
Explanation: There's no common subsequence
in all the strings.

Your Task:
You don't need to read input or print anything. Your task is to complete the function LCSof3() which takes the strings A, B, C and their lengths n1, n2, n3 as input and returns the length of the longest common subsequence in all the 3 strings.


Expected Time Complexity: O(n1*n2*n3).
Expected Auxiliary Space: O(n1*n2*n3).


Constraints:
1<=n1, n2, n3<=20
*/

// C++ program to find LCS of three strings
#include <bits/stdc++.h>
using namespace std;

/* Returns length of LCS for X[0..m-1], Y[0..n-1]
   and Z[0..o-1] */
int lcsOf3(string X, string Y, string Z, int m,
           int n, int o)
{
    int L[m + 1][n + 1][o + 1];

    /* Following steps build L[m+1][n+1][o+1] in
       bottom up fashion. Note that L[i][j][k]
       contains length of LCS of X[0..i-1] and
       Y[0..j-1]  and Z[0.....k-1]*/
    for (int i = 0; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            for (int k = 0; k <= o; k++)
            {
                if (i == 0 || j == 0 || k == 0)
                    L[i][j][k] = 0;

                else if (X[i - 1] == Y[j - 1] && X[i - 1] == Z[k - 1])
                    L[i][j][k] = L[i - 1][j - 1][k - 1] + 1;

                else
                    L[i][j][k] = max(max(L[i - 1][j][k],
                                         L[i][j - 1][k]),
                                     L[i][j][k - 1]);
            }
        }
    }

    /* L[m][n][o] contains length of LCS for
      X[0..n-1] and Y[0..m-1] and Z[0..o-1]*/
    return L[m][n][o];
}

/* Driver program to test above function */
int main()
{
    string X = "AGGT12";
    string Y = "12TXAYB";
    string Z = "12XBA";

    int m = X.length();
    int n = Y.length();
    int o = Z.length();

    cout << "Length of LCS is " << lcsOf3(X, Y, Z, m, n, o);

    return 0;
}

// C++ program to find LCS of three strings
#include <bits/stdc++.h>
using namespace std;

string X = "AGGT12";
string Y = "12TXAYB";
string Z = "12XBA";

int dp[100][100][100];

/* Returns length of LCS for X[0..m-1], Y[0..n-1]
and Z[0..o-1] */
int lcsOf3(int i, int j, int k)
{
    if (i == -1 || j == -1 || k == -1)
        return 0;
    if (dp[i][j][k] != -1)
        return dp[i][j][k];

    if (X[i] == Y[j] && Y[j] == Z[k])
        return dp[i][j][k] = 1 + lcsOf3(i - 1, j - 1, k - 1);
    else
        return dp[i][j][k] = max(max(lcsOf3(i - 1, j, k),
                                     lcsOf3(i, j - 1, k)),
                                 lcsOf3(i, j, k - 1));
}

// Driver code
int main()
{
    memset(dp, -1, sizeof(dp));
    int m = X.length();
    int n = Y.length();
    int o = Z.length();

    cout << "Length of LCS is " << lcsOf3(m - 1, n - 1, o - 1);
    // this code is contributed by Kushdeep Mittal
}
/*
https://practice.geeksforgeeks.org/problems/count-palindromic-subsequences/1

Count Palindromic Subsequences 
Medium Accuracy: 49.32% Submissions: 51477 Points: 4
Given a string str of length N, you have to find number of palindromic subsequence (need not necessarily be distinct) which could be formed from the string str.
Note: You have to return the answer module 109+7;
 

Example 1:

Input: 
Str = "abcd"
Output: 
4
Explanation:
palindromic subsequence are : "a" ,"b", "c" ,"d"
 

Example 2:

Input: 
Str = "aab"
Output: 
4
Explanation:
palindromic subsequence are :"a", "a", "b", "aa"
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function countPs() which takes a string str as input parameter and returns the number of palindromic subsequence.
 

Expected Time Complexity: O(N*N)
Expected Auxiliary Space: O(N*N)


Constraints:
1<=length of string str <=1000
*/

// { Driver Code Starts
// Counts Palindromic Subsequence in a given String
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    /*You are required to complete below method */
    long long int countPS(string str)
    {
        //Your code here
        //recurrence relation:
        //f(i, j) represents count of palindrome b/w i && j
        //f(i, j) = 1, if (i == j)
        //f(i, j) = f(i + 1 , j - 1) + 1, if (s[i] == s[j]);
        //f(i, j) = f(i + 1, j) + f(i, j - 1) - f(i + 1, j - 1) if (s[i]!= s[j])
        // for all i <= j;

        //order of evalution
        //decreasing order for i, increasing order for j

        int n = str.size(), MOD = 1e9 + 7;
        int dp[n + 1][n + 1];
        memset(dp, 0, sizeof dp);
        for (int i = n - 1; i >= 0; i--)
        {
            //base case

            dp[i][i] = 1;
            for (int j = i + 1; j < n; j++)
            {

                //a. if we ith and jth are not making a palindrome
                //with (i + 1, j - 1)
                //then dp[i][j] = (cnt of palindrome in (i + 1, j) + cnt of palindrome in (i, j + 1)
                //- cnt of palindrome in (i + 1, j - 1) since we are counting them twice.
                //pre calculate the above value and store it in a variable say val as it will be used
                //in both situation (making palindrome and not making palindrome).

                //b. if making a palindrome with (i + 1, j - 1) then
                //we have two options
                //1.make the palindrome
                // dp[i][j] += dp[i+1][j-1]
                //2.don't make the palindrome
                // dp[i][j] += val

                long long int val = (dp[i + 1][j] + (dp[i][j - 1] - dp[i + 1][j - 1] + MOD) % MOD) % MOD;
                if (j < n && str[i] == str[j])
                    dp[i][j] = (dp[i + 1][j - 1] + 1 + val) % MOD;
                else
                    dp[i][j] = val;
            }
        }
        return dp[0][n - 1];
    }
};

// { Driver Code Starts.
// Driver program
int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        string str;
        cin >> str;
        Solution ob;
        long long int ans = ob.countPS(str);
        cout << ans << endl;
    }
} // } Driver Code Ends
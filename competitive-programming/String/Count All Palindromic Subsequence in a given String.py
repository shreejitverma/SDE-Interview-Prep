'''https://practice.geeksforgeeks.org/problems/count-palindromic-subsequences/1

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
1<=length of string str <=1000'''


import sys


class Solution:
    # Your task is to complete this function
    # Function should return an integer
    def countPs(self, string):
        # Code here
        n = len(string)
        dp = list()

        for _ in range(n):
            dp.append([0]*n)

        for i in range(n):
            dp[i][i] = 1

        for l in range(2, n+1):
            for i in range(n):
                k = l+i-1

                if k < n:
                    # print(i,k)
                    if string[i] == string[k]:
                        dp[i][k] = dp[i+1][k] + dp[i][k-1] + 1
                    else:
                        dp[i][k] = dp[i+1][k] + dp[i][k-1] - dp[i+1][k-1]
        # print(dp)
        return dp[0][n-1] % (10**9+7)

# {
#  Driver Code Starts
# Initial template for Python 3


sys.setrecursionlimit(10**6)

if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        ob = Solution()
        print(ob.countPs(input().strip()))

# } Driver Code Ends

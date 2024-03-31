/*
https://practice.geeksforgeeks.org/problems/subset-sum-problem2014/1
Partition Equal Subset Sum 
Medium Accuracy: 38.0% Submissions: 75935 Points: 4
Given an array arr[] of size N, check if it can be partitioned into two parts such that the sum of elements in both parts is the same.

Example 1:

Input: N = 4
arr = {1, 5, 11, 5}
Output: YES
Explaination: 
The two parts are {1, 5, 5} and {11}.
Example 2:

Input: N = 3
arr = {1, 3, 5}
Output: NO
Explaination: This array can never be 
partitioned into two such parts.

Your Task:
You do not need to read input or print anything. Your task is to complete the function equalPartition() which takes the value N and the array as input parameters and returns 1 if the partition is possible. Otherwise, returns 0.


Expected Time Complexity: O(N*sum of elements)
Expected Auxiliary Space: O(N*sum of elements)


Constraints:
1 ≤ N ≤ 100
1 ≤ arr[i] ≤ 1000
*/
class Solution
{
public:
    int equalPartition(int N, int arr[])
    {
        // code here
        long sum = 0;
        for (int i = 0; i < N; i++)
            sum += arr[i];

        if (sum % 2 != 0)
            return 0;
        else
        {
            //subset sum problem....
            bool dp[N + 1][(sum / 2) + 1];

            for (int i = 0; i <= N; i++)
            {
                for (int j = 0; j <= sum / 2 + 1; j++)
                {
                    if (i == 0)
                        dp[i][j] = false;
                    if (j == 0)
                        dp[i][j] = true;
                }
            }

            for (int i = 1; i <= N; i++)
            {
                for (int j = 1; j <= sum / 2; j++)
                {
                    if (arr[i - 1] <= j)
                        dp[i][j] = dp[i - 1][j - arr[i - 1]] || dp[i - 1][j];
                    else
                        dp[i][j] = dp[i - 1][j];
                }
            }
            return dp[N][sum / 2];
        }
    }
};

// Initial Template for C++

#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
// User function Template for C++

class Solution
{
public:
    int equalPartition(int n, int arr[])
    {
        // code here
        int sum = 0;
        int i, j;

        // Calculate sum of all elements
        for (i = 0; i < n; i++)
            sum += arr[i];

        if (sum % 2 != 0)
            return false;

        bool part[sum / 2 + 1];

        // Initialize the part array
        // as 0
        for (i = 0; i <= sum / 2; i++)
        {
            part[i] = 0;
        }

        // Fill the partition table in bottom up manner

        for (i = 0; i < n; i++)
        {
            // the element to be included
            // in the sum cannot be
            // greater than the sum
            for (j = sum / 2; j >= arr[i];
                 j--)
            { // check if sum - arr[i]
                // could be formed
                // from a subset
                // using elements
                // before index i
                if (part[j - arr[i]] == 1 || j == arr[i])
                    part[j] = 1;
            }
        }

        return part[sum / 2];
    }
};

// { Driver Code Starts.

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int N;
        cin >> N;
        int arr[N];
        for (int i = 0; i < N; i++)
            cin >> arr[i];

        Solution ob;
        if (ob.equalPartition(N, arr))
            cout << "YES\n";
        else
            cout << "NO\n";
    }
    return 0;
} // } Driver Code Ends
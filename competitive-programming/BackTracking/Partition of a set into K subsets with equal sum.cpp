/*
https://practice.geeksforgeeks.org/problems/partition-array-to-k-subsets/1
Partition array to K subsets 
Hard Accuracy: 43.89% Submissions: 13428 Points: 8
Given an integer array a[ ] of N elements and an integer K, the task is to check if the array a[ ] could be divided into K non-empty subsets with equal sum of elements.
Note: All elements of this array should be part of exactly one partition.

Example 1:

Input: 
N = 5
a[] = {2,1,4,5,6}
K = 3
Output: 
1
Explanation: we can divide above array 
into 3 parts with equal sum as (2, 4), 
(1, 5), (6)
Example 2:

Input: 
N = 5 
a[] = {2,1,5,5,6}
K = 3
Output: 
0
Explanation: It is not possible to divide
above array into 3 parts with equal sum.
Your Task:
You don't need to read input or print anything. Your task is to complete the function isKPartitionPossible() which takes the array a[], the size of the array N, and the value of K as inputs and returns true(same as 1) if possible, otherwise false(same as 0).

Expected Time Complexity: O(N*2N).
Expected Auxiliary Space: O(2N).

Constraints:
1 ≤ K ≤ N ≤ 10
1 ≤ a[i] ≤ 100
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
/*You are required to complete this method */

class Solution
{
public:
    bool partition(vector<int> p, int A[], int r, int s)
    {
        if (r < 0)
            return true;

        int x = A[r];
        r--;

        for (int i = 0; i < p.size(); i++)
        {
            if (p[i] + x <= s)
            {
                p[i] += x;

                if (partition(p, A, r, s))
                    return true;

                p[i] -= x;
            }

            if (p[i] == 0)
                break;
        }

        return false;
    }

    bool isKPartitionPossible(int A[], int N, int K)
    {
        //Your code here

        sort(A, A + N);

        int sum = 0;
        for (int i = 0; i < N; i++)
            sum += A[i];

        if (sum % K != 0)
            return false;

        int r = N - 1;
        int s = sum / K;

        if (A[N - 1] > s)
            return false;

        int n = K;

        while (A[r] == s)
        {
            r--;
            K--;
        }

        vector<int> p;

        for (int i = 0; i < K; i++)
            p.push_back(0);

        return partition(p, A, r, s);
    }
};

// { Driver Code Starts.
int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int n;

        cin >> n;
        int a[n];
        for (int i = 0; i < n; i++)
            cin >> a[i];
        int k;
        cin >> k;
        Solution obj;
        cout << obj.isKPartitionPossible(a, n, k) << endl;
    }
} // } Driver Code Ends
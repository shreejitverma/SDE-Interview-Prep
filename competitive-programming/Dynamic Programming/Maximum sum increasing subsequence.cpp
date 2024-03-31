/*
https: // practice.geeksforgeeks.org/problems/maximum-sum-increasing-subsequence4749/1
https://www.geeksforgeeks.org/maximum-sum-increasing-subsequence-dp-14/
Maximum sum increasing subsequence
Medium Accuracy: 49.92 % Submissions: 28261 Points: 4
Given an array arr of N positive integers, the task is to find the maximum sum increasing subsequence of the given array.


Example 1:

Input: N = 5, arr[] = {1, 101, 2, 3, 100}
Output: 106
Explanation: The maximum sum of a
increasing sequence is obtained from
{1, 2, 3, 100}
Example 2:

Input: N = 3, arr[] = {1, 2, 3}
Output: 6
Explanation: The maximum sum of a
increasing sequence is obtained from
{1, 2, 3}

Your Task:
You don't need to read input or print anything. Complete the function maxSumIS() which takes N and array arr as input parameters and returns the maximum value.


Expected Time Complexity: O(N2)
Expected Auxiliary Space: O(N)


Constraints:
1 ≤ N ≤ 103
1 ≤ arr[i] ≤ 105
*/

/* Dynamic Programming implementation
of Maximum Sum Increasing Subsequence
(MSIS) problem */
#include <bits/stdc++.h>
using namespace std;

/* maxSumIS() returns the maximum
sum of increasing subsequence
in arr[] of size n */
int maxSumIS(int arr[], int n)
{
    int i, j, max = 0;
    int msis[n];

    /* Initialize msis values
    for all indexes */
    for (i = 0; i < n; i++)
        msis[i] = arr[i];

    /* Compute maximum sum values
    in bottom up manner */
    for (i = 1; i < n; i++)
        for (j = 0; j < i; j++)
            if (arr[i] > arr[j] &&
                msis[i] < msis[j] + arr[i])
                msis[i] = msis[j] + arr[i];

    /* Pick maximum of
    all msis values */
    for (i = 0; i < n; i++)
        if (max < msis[i])
            max = msis[i];

    return max;
}

// Driver Code
int main()
{
    int arr[] = {1, 101, 2, 3, 100, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Sum of maximum sum increasing "
            "subsequence is "
         << maxSumIS(arr, n) << endl;
    return 0;
}

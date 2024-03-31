'''https: // practice.geeksforgeeks.org/problems/maximum-sum-increasing-subsequence4749/1
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
'''

# Dynamic Programming bsed Python
# implementation of Maximum Sum
# Increasing Subsequence (MSIS)
# problem

# maxSumIS() returns the maximum
# sum of increasing subsequence
# in arr[] of size n


def maxSumIS(arr, n):
    max = 0
    msis = [0 for x in range(n)]

    # Initialize msis values
    # for all indexes
    for i in range(n):
        msis[i] = arr[i]

    # Compute maximum sum
    # values in bottom up manner
    for i in range(1, n):
        for j in range(i):
            if (arr[i] > arr[j] and
                    msis[i] < msis[j] + arr[i]):
                msis[i] = msis[j] + arr[i]

    # Pick maximum of
    # all msis values
    for i in range(n):
        if max < msis[i]:
            max = msis[i]

    return max


# Driver Code
arr = [1, 101, 2, 3, 100, 4, 5]
n = len(arr)
print("Sum of maximum sum increasing " +
      "subsequence is " +
      str(maxSumIS(arr, n)))

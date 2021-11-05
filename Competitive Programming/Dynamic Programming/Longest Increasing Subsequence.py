'''https://practice.geeksforgeeks.org/problems/longest-increasing-subsequence-1587115620/1
https://www.geeksforgeeks.org/longest-increasing-subsequence-dp-3/
https://www.geeksforgeeks.org/longest-monotonically-increasing-subsequence-size-n-log-n/
Longest Increasing Subsequence 
Medium Accuracy: 46.69% Submissions: 58428 Points: 4
Given an array of integers, find the length of the longest (strictly) increasing subsequence from the given array.

Example 1:

Input:
N = 16
A[]={0,8,4,12,2,10,6,14,1,9,5
     13,3,11,7,15}
Output: 6
Explanation:Longest increasing subsequence
0 2 6 9 13 15, which has length 6
Example 2:

Input:
N = 6
A[] = {5,8,3,7,9,1}
Output: 3
Explanation:Longest increasing subsequence
5 7 9, with length 3
Your Task:
Complete the function longestSubsequence() which takes the input array and its size as input parameters and returns the length of the longest increasing subsequence.

Expected Time Complexity : O( N*log(N) )
Expected Auxiliary Space: O(N)

Constraints:
1 ≤ N ≤ 105
0 ≤ A[i] ≤ 106'''

# Dynamic programming Python implementation
# of LIS problem

# lis returns length of the longest
# increasing subsequence in arr of size n


def lis(arr):
    n = len(arr)

    # Declare the list (array) for LIS and
    # initialize LIS values for all indexes
    lis = [1]*n

    # Compute optimized LIS values in bottom up manner
    for i in range(1, n):
        for j in range(0, i):
            if arr[i] > arr[j] and lis[i] < lis[j] + 1:
                lis[i] = lis[j]+1

    # Initialize maximum to 0 to get
    # the maximum of all LIS
    maximum = 0

    # Pick maximum of all LIS values
    for i in range(n):
        maximum = max(maximum, lis[i])

    return maximum
# end of lis function


# Driver program to test above function
arr = [10, 22, 9, 33, 21, 50, 41, 60]
print "Length of lis is", lis(arr)


# Dynamic Programming Approach of Finding LIS by reducing the problem to longest common Subsequence
'''Longest Increasing Subsequence Size (N log N)

Method 3: Dynamic Programming

If we closely observe the problem then we can convert this problem to longest Common Subsequence Problem. 
Firstly we will create another array of unique elements of original array and sort it. 
Now the longest increasing subsequence of our array must be present as a subsequence in our sorted array. 
That’s why our problem is now reduced to finding the common subsequence between the two arrays.'''


def lis(a):
    n = len(a)
    # Creating the sorted list
    b = sorted(list(set(a)))
    m = len(b)

    # Creating dp table for storing the answers of sub problems
    dp = [[-1 for i in range(m+1)] for j in range(n+1)]

    # Finding Longest common Subsequence of the two arrays
    for i in range(n+1):

        for j in range(m+1):
            if i == 0 or j == 0:
                dp[i][j] = 0
            elif a[i-1] == b[j-1]:
                dp[i][j] = 1+dp[i-1][j-1]
            else:
                dp[i][j] = max(dp[i-1][j], dp[i][j-1])
    return dp[-1][-1]


# Driver program to test above function
arr = [10, 22, 9, 33, 21, 50, 41, 60]
print("Length of lis is ", lis(arr))

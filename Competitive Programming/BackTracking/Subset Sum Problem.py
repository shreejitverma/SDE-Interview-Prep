'''https://practice.geeksforgeeks.org/problems/subset-sum-problem2014/1
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
1 ≤ arr[i] ≤ 1000'''


import sys


class Solution:
    def equalPartition(self, N, arr):
        # code here
        def find(ind, s):
            if s == 0:
                return True
            if ind == N:
                if s == 0:
                    return True
                return False
            if arr[ind] <= s:
                s -= arr[ind]
                if find(ind+1, s):
                    return True
                s += arr[ind]
            if find(ind+1, s):
                return True
            return False
        s = sum(arr)
        if s % 2:
            return 0
        s = s//2
        if find(0, s):
            return 1
        return 0

# {
#  Driver Code Starts
# Initial Template for Python3


input = sys.stdin.readline
if __name__ == '__main__':
    t = int(input())
    for _ in range(t):
        N = int(input())
        arr = input().split()
        for it in range(N):
            arr[it] = int(arr[it])

        ob = Solution()
        if (ob.equalPartition(N, arr) == 1):
            print("YES")
        else:
            print("NO")
# } Driver Code Ends


class Solution:
    def equalPartition(self, n, arr):
        # code here
        Sum = 0

        # Calculate sum of all elements
        for i in range(n):
            Sum += arr[i]
        if (Sum % 2 != 0):
            return 0
        part = [0] * ((Sum // 2) + 1)

        # Initialize the part array as 0
        for i in range((Sum // 2) + 1):
            part[i] = 0

        # Fill the partition table in bottom up manner
        for i in range(n):

            # the element to be included
            # in the sum cannot be
            # greater than the sum
            for j in range(Sum // 2, arr[i] - 1, -1):

                # check if sum - arr[i]
                # could be formed
                # from a subset
                # using elements
                # before index i
                if (part[j - arr[i]] == 1 or j == arr[i]):
                    part[j] = 1

        return part[Sum // 2]

# {
#  Driver Code Starts
# Initial Template for Python3


input = sys.stdin.readline
if __name__ == '__main__':
    t = int(input())
    for _ in range(t):
        N = int(input())
        arr = input().split()
        for it in range(N):
            arr[it] = int(arr[it])

        ob = Solution()
        if (ob.equalPartition(N, arr) == 1):
            print("YES")
        else:
            print("NO")
# } Driver Code Ends

'''https://practice.geeksforgeeks.org/problems/value-equal-to-index-value1330/1

Value equal to index value 
School Accuracy: 54.64% Submissions: 34251 Points: 0
Given an array Arr of N positive integers. Your task is to find the elements whose value is equal to that of its index value ( Consider 1-based indexing ).

Example 1:

Input: 
N = 5
Arr[] = {15, 2, 45, 12, 7}
Output: 2
Explanation: Only Arr[2] = 2 exists here.
Example 2:

Input: 
N = 1
Arr[] = {1}
Output: 1
Explanation: Here Arr[1] = 1 exists.
Your Task:  
You don't need to read input or print anything. Your task is to complete the function valueEqualToIndex() which takes the array of integers arr[] and n as parameters and returns an array of indices where the given conditions are satified. When there is not such element exists then return an empty array of length 0.

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)
Note: There can be more than one element in the array which have same value as their index. You need to include every such element's index. Follows 1-based indexing of the array.

Constraints:
1 ≤ N ≤ 105
1 ≤ Arr[i] ≤ 106'''
# User function Template for python3


class Solution:

    def valueEqualToIndex(self, arr, n):
        # code here
        v = list()
        i = 0
        for i in range(n):
            if (arr[i] == i + 1):
        v.append(arr[i])

    return v

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == '__main__':
    tc = int(input())
    while tc > 0:
        n = int(input())
        arr = list(map(int, input().strip().split()))
        ob = Solution()
        ans = ob.valueEqualToIndex(arr, n)
        if len(ans) == 0:
            print("Not Found")
        else:
            for x in ans:
                print(x, end=" ")
            print()
        tc -= 1

# } Driver Code Ends

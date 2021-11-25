'''https://practice.geeksforgeeks.org/problems/find-pair-given-difference1559/1
Find Pair Given Difference 
Easy Accuracy: 49.01% Submissions: 30117 Points: 2
Given an array Arr[] of size L and a number N, you need to write a program to find if there exists a pair of elements in the array whose difference is N.

Example 1:

Input:
L = 6, N = 78
arr[] = {5, 20, 3, 2, 5, 80}
Output: 1
Explanation: (2, 80) have difference of 78.
Example 2:

Input:
L = 5, N = 45
arr[] = {90, 70, 20, 80, 50}
Output: -1
Explanation: There is no pair with difference of 45.
Your Task:
You need not take input or print anything. Your task is to complete the function findPair() which takes array arr, size of the array L and N as input parameters and returns True if required pair exists, else return False.

Expected Time Complexity: O(L* Log(L)).
Expected Auxiliary Space: O(1).

Constraints:
1 ≤ L ≤ 104
1 ≤ Arr[i], N ≤ 105

'''
#User function Template for python3

class Solution:

    def findPair(self, arr, L,N):
        #code here
        arr.sort()
        last = len(arr) - 1
        prev = len(arr) - 2
        while(last > 0 and prev > -1):
            if arr[last] - arr[prev] == N:
                return(True)
            elif arr[last] - arr[prev] > N:
                last -= 1
            elif arr[last] - arr[prev] < N:
                prev -= 1
        return(False)

#{ 
#  Driver Code Starts
#Initial Template for Python 3

if __name__ == '__main__':

    t = int(input())

    for _ in range(t):
        L,N = [int(x) for x in input().split()]
        arr = [int(x) for x in input().split()]

        solObj = Solution()

        if(solObj.findPair(arr,L, N)):
            print(1)
        else:
            print(-1)
# } Driver Code Ends
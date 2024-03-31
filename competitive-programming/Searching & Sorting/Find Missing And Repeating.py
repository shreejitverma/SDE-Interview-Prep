'''https://practice.geeksforgeeks.org/problems/find-missing-and-repeating2512/1
Find Missing And Repeating 
Medium Accuracy: 37.77% Submissions: 77816 Points: 4
Given an unsorted array Arr of size N of positive integers. One number 'A' from set {1, 2, …N} is missing and one number 'B' occurs twice in array. Find these two numbers.

Example 1:

Input:
N = 2
Arr[] = {2, 2}
Output: 2 1
Explanation: Repeating number is 2 and 
smallest positive missing number is 1.
Example 2:

Input:
N = 3
Arr[] = {1, 3, 3}
Output: 3 2
Explanation: Repeating number is 3 and 
smallest positive missing number is 2.
Your Task:
You don't need to read input or print anything. Your task is to complete the function findTwoElement() which takes the array of integers arr and n as parameters and returns an array of integers of size 2 denoting the answer ( The first index contains B and second index contains A.)

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)

Constraints:
1 ≤ N ≤ 105
1 ≤ Arr[i] ≤ N

'''


#User function Template for python3

class Solution:
    def findTwoElement( self,arr, n):
#code here
        arr.sort()
        for i in range(1,n):
            if arr[i]==arr[i-1]:
                break
        dup=arr[i]
        miss=((n*(n+1))//2)-sum(arr)+dup
        return [dup,miss]

#{
#Driver Code Starts
#Initial Template for Python 3

if __name__ == '__main__': 

    tc=int(input())
    while tc > 0:
        n=int(input())
        arr=list(map(int, input().strip().split()))
        ob = Solution()
        ans=ob.findTwoElement(arr, n)
        print(str(ans[0])+" "+str(ans[1]))
        tc=tc-1
#} Driver Code Ends
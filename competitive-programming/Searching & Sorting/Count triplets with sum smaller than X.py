'''
https://practice.geeksforgeeks.org/problems/count-triplets-with-sum-smaller-than-x5549/1
Count triplets with sum smaller than X 
Medium Accuracy: 49.96% Submissions: 24766 Points: 4
Given an array arr[] of distinct integers of size N and a value sum, the task is to find the count of triplets (i, j, k), having (i<j<k) with the sum of (arr[i] + arr[j] + arr[k]) smaller than the given value sum.


Example 1:


Input: N = 4, sum = 2
arr[] = {-2, 0, 1, 3}
Output:  2
Explanation: Below are triplets with 
sum less than 2 (-2, 0, 1) and (-2, 0, 3). 
 

Example 2:


Input: N = 5, sum = 12
arr[] = {5, 1, 3, 4, 7}
Output: 4
Explanation: Below are triplets with 
sum less than 12 (1, 3, 4), (1, 3, 5), 
(1, 3, 7) and (1, 4, 5).

Your Task:
This is a function problem. You don't need to take any input, as it is already accomplished by the driver code. You just need to complete the function countTriplets() that take array arr[], integer N  and integer sum as parameters and returns the count of triplets.


Expected Time Complexity: O(N2).
Expected Auxiliary Space: O(1).


Constraints:
3 ≤ N ≤ 103
-103 ≤ arr[i] ≤ 103'''

class Solution:
    def countTriplets(self, arr, n, x):
        #code here
        arr.sort()
	    c=0
	   
        for k in range(0,n-2):
            i=k+1
            j=n-1
            while(i<j):
                #both are distinct so no use i<=j
                sum = arr[i]+arr[j]+arr[k]
                if(sum<x):
                    c+=(j-i)#take all small numbers
                    i+=1
                else:
                    j-=1
           
        return c;

#{ 
#  Driver Code Starts
if __name__ == '__main__': 
    t=int(input())
    for _ in range(0,t):
        l=list(map(int,input().split()))
        n=l[0]
        k=l[1]
        a=list(map(int,input().split()))
        ob = Solution()
        ans=ob.countTriplets(a,n,k)
        print(ans)
# } Driver Code Ends
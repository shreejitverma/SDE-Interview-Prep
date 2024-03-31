'''
https://practice.geeksforgeeks.org/problems/zero-sum-subarrays1825/1
Zero Sum Subarrays 
Medium Accuracy: 50.41% Submissions: 27258 Points: 4
You are given an array arr[] of size n. Find the total count of sub-arrays having their sum equal to 0.


Example 1:

Input:
n = 6
arr[] = {0,0,5,5,0,0}
Output: 6
Explanation: The 6 subarrays are 
[0], [0], [0], [0], [0,0], and [0,0].

Example 2:

Input:
n = 10
arr[] = {6,-1,-3,4,-2,2,4,6,-12,-7}
Output: 4
Explanation: The 4 subarrays are [-1 -3 4]
[-2 2], [2 4 6 -12], and [-1 -3 4 -2 2]

Your Task:
You don't need to read input or print anything. Complete the function findSubarray() that takes the array arr and its size n as input parameters and returns the total number of sub-arrays with 0 sum. 
 

Expected Time Complexity : O(n)
Expected Auxilliary Space : O(n)
 

Constraints:    
1<= n <= 107
-1010 <= arri <= 1010'''

#User function Template for python3

class Solution:
    #Function to count subarrays with sum equal to 0.
    def findSubArrays(self,arr,n):
        
        #return: count of sub-arrays having their sum equal to 0
        dict1=dict({0:1})
        sum1=0
        count=0
        for i in range(n):
            sum1+=arr[i]
        
            if sum1 not in dict1:
                dict1[sum1]=1
            else:
                dict1[sum1]+=1
                count+=dict1[sum1]-1
        
        return count

#{ 
#  Driver Code Starts
#Initial Template for Python 3

#contributed by RavinderSinghPB
        
if __name__=='__main__':
    t=int(input())
    for tc in range(t):
        
        N=int(input())
        A = [int(x) for x in input().strip().split(' ')]
        ob = Solution()
        print(ob.findSubArrays(A,N))
        
                
                
# } Driver Code Ends
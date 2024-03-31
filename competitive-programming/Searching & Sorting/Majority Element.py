'''https://practice.geeksforgeeks.org/problems/majority-element-1587115620/1#

Majority Element 
Medium Accuracy: 48.6% Submissions: 100k+ Points: 4
Given an array A of N elements. Find the majority element in the array. A majority element in an array A of size N is an element that appears more than N/2 times in the array.
 

Example 1:

Input:
N = 3 
A[] = {1,2,3} 
Output:
-1
Explanation:
Since, each element in 
{1,2,3} appears only once so there 
is no majority element.
Example 2:

Input:
N = 5 
A[] = {3,1,3,3,2} 
Output:
3
Explanation:
Since, 3 is present more
than N/2 times, so it is 
the majority element.

Your Task:
The task is to complete the function majorityElement() which returns the majority element in the array. If no majority exists, return -1.
 

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).
 

Constraints:
1 ≤ N ≤ 107
0 ≤ Ai ≤ 106'''

#User function template for Python 3

class Solution:
    def majorityElement(self, nums, N):
        #Your code here
        ans = nums[0]
        cnt = 1
        for i in nums:
            if (i == ans):
                cnt+=1
            else:
                cnt-=1
                if (cnt == 0):
                    ans = i
                    cnt = 1
         
        ct = 0
    
        for i in nums:
            if(i ==  ans):
                ct+=1
        return ans if (ct > N/2)  else -1

#{ 
#  Driver Code Starts
#Initial Template for Python 3

import math

from sys import stdin


def main():
        T=int(input())
        while(T>0):
            
            N=int(input())

            A=[int(x) for x in input().strip().split()]
            
            
            obj = Solution()
            print(obj.majorityElement(A,N))
            
            T-=1


if __name__ == "__main__":
    main()
# } Driver Code Ends
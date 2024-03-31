'''https://practice.geeksforgeeks.org/problems/stickler-theif-1587115621/1

Stickler Thief 
Easy Accuracy: 50.32% Submissions: 43113 Points: 2
Stickler the thief wants to loot money from a society having n houses in a single line. He is a weird person and follows a certain rule when looting the houses. According to the rule, he will never loot two consecutive houses. At the same time, he wants to maximize the amount he loots. The thief knows which house has what amount of money but is unable to come up with an optimal looting strategy. He asks for your help to find the maximum money he can get if he strictly follows the rule. Each house has a[i] amount of money present in it.

Example 1:

Input:
n = 6
a[] = {5,5,10,100,10,5}
Output: 110
Explanation: 5+100+5=110
Example 2:

Input:
n = 3
a[] = {1,2,3}
Output: 4
Explanation: 1+3=4
Your Task:
Complete the function FindMaxSum() which takes an array arr[] and n as input which returns the maximum money he can get following the rules

Expected Time Complexity: O(N).
Expected Space Complexity: O(N).

Constraints:
1 ≤ n ≤ 104
1 ≤ a[i] ≤ 104'''

#User function Template for python3

class Solution:  
    
    #Function to find the maximum money the thief can get.
    def FindMaxSum(self,a, n):
        
        # code here
        if n==0:
            return 0
        if n==1:
            return a[0]
        if n==2:
            return max(a[0],a[1])
        dp=[0]*n
        dp[0]=a[0]
        dp[1]=max(a[0],a[1])
        for i in range(2,n):
            dp[i]=max(a[i]+dp[i-2],dp[i-1])
        return dp[-1]

#{ 
#  Driver Code Starts
#Initial Template for Python 3
import atexit
import io
import sys
sys.setrecursionlimit(10**6)
# Contributed by : Nagendra Jha

if __name__ == '__main__':
    test_cases = int(input())
    for cases in range(test_cases):
        n = int(input())
        a = list(map(int,input().strip().split()))
        ob=Solution()
        print(ob.FindMaxSum(a,n))
# } Driver Code Ends
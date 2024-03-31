'''https://practice.geeksforgeeks.org/problems/find-the-median0527/1
Find the median 
School Accuracy: 50.0% Submissions: 29600 Points: 0
Given an array arr[] of N integers, calculate the median
 

Example 1:

Input: N = 5
arr[] = 90 100 78 89 67
Output: 89
Explanation: After sorting the array 
middle element is the median 

Example 2:

Input: N = 4
arr[] = 56 67 30 79
Output: 61
Explanation: In case of even number of 
elements, average of two middle elements 
is the median.

 

Your Task:
You don't need to read or print anything. Your task is to complete the function find_median() which takes the array as input parameter and returns the floor value of the median.
 

Expected Time Complexity: O(n * log(n))
Expected Space Complexity: O(1)
 

Constraints:
1 <= Length of Array <= 100
1 <= Elements of Array <= 100'''

# User function Template for python3


class Solution:
    def find_median(self, v):
        # Code here
        v.sort()
        a = len(v)
    if (a & 1):
        return v[int(a / 2)]
    else:
        return int((v[int((a - 1) / 2)] + v[int(a / 2)]) / 2)


# {
#  Driver Code Starts
# Initial Template for Python 3

if __name__ == '__main__':
    T = int(input())
    for i in range(T):
        n = int(input())
        v = list(map(int, input().split()))
        ob = Solution()
        ans = ob.find_median(v)
        print(ans)
# } Driver Code Ends

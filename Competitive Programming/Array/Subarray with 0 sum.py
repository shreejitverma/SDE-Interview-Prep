'''https://practice.geeksforgeeks.org/problems/subarray-with-0-sum-1587115621/1
Subarray with 0 sum 
Easy Accuracy: 49.91% Submissions: 74975 Points: 2
Given an array of positive and negative numbers. Find if there is a subarray (of size at-least one) with 0 sum.

Example 1:

Input:
5
4 2 -3 1 6

Output: 
Yes

Explanation: 
2, -3, 1 is the subarray 
with sum 0.
Example 2:

Input:
5
4 2 0 1 6

Output: 
Yes

Explanation: 
0 is one of the element 
in the array so there exist a 
subarray with sum 0.
Your Task:
You only need to complete the function subArrayExists() that takes array and n as parameters and returns true or false depending upon whether there is a subarray present with 0-sum or not. Printing will be taken care by the drivers code.

Expected Time Complexity: O(n).
Expected Auxiliary Space: O(n).

Constraints:
1 <= n <= 104
-105 <= a[i] <= 105'''

# User function Template for python3


class Solution:

    # Function to check whether there is a subarray present with 0-sum or not.
    def subArrayExists(self, arr, n):
        # Your code here
        # Return true or false

        s = set()

        sum = 0
        for i in range(n):
            sum += arr[i]

            if sum == 0 or sum in s:
                return True
            s.add(sum)

        return False
# {
#  Driver Code Starts
# Initial Template for Python 3


def main():
    T = int(input())
    while(T > 0):

        n = int(input())
        arr = [int(x) for x in input().strip().split()]
        if(Solution().subArrayExists(arr, n)):
            print("Yes")
        else:
            print("No")

        T -= 1


if __name__ == "__main__":
    main()
# } Driver Code Ends

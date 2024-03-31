'''https://practice.geeksforgeeks.org/problems/minimum-number-of-jumps-1587115620/1
Minimum number of jumps 
Medium Accuracy: 32.96% Submissions: 100k+ Points: 4
Given an array of N integers arr[] where each element represents the max number of steps that can be made forward from that element. Find the minimum number of jumps to reach the end of the array (starting from the first element). If an element is 0, then you cannot move through that element.
Note: Return -1 if you can't reach the end of the array.


Example 1:

Input:
N = 11 
arr[] = {1, 3, 5, 8, 9, 2, 6, 7, 6, 8, 9} 
Output: 3 
Explanation: 
First jump from 1st element to 2nd 
element with value 3. Now, from here 
we jump to 5th element with value 9, 
and from here we will jump to last. 
Example 2:

Input :
N = 6
arr = {1, 4, 3, 2, 6, 7}
Output: 2 
Explanation: 
First we jump from the 1st to 2nd element 
and then jump to the last element.

Your task:
You don't need to read input or print anything. Your task is to complete function minJumps() which takes the array arr and it's size N as input parameters and returns the minimum number of jumps. If not possible returns -1.


Expected Time Complexity: O(N)
Expected Space Complexity: O(1)


Constraints:
1 ≤ N ≤ 107
0 ≤ arri ≤ 107'''

# User function Template for python3


class Solution:
    def minJumps(self, arr, n):
        # code here
        if len(arr) <= 1:
        return 0

    # Return -1 if not possible to jump
    if arr[0] == 0:
        return -1

    # initialization
    maxReach = arr[0]
    step = arr[0]
    jump = 1

    # Start traversing array
    for i in range(1, len(arr)):

        # Check if we have reached the end of the array
        if i == len(arr) - 1:
            return jump

        # updating maxReach
        maxReach = max(maxReach, i+arr[i])

        # we use a step to get to the current index
        step -= 1

        # If no further steps left
        if step == 0:
            #  we must have used a jump
            jump += 1

            # Check if the current index/position  or lesser index
            # is the maximum reach point from the previous indexes
            if i >= maxReach:
                return -1

            # re-initialize the steps to the amount
            # of steps to reach maxReach from position i.
            step = maxReach - i

    return -1


# {
#  Driver Code Starts
# Initial Template for Python 3
if __name__ == '__main__':
    T = int(input())
    for i in range(T):
        n = int(input())
        Arr = [int(x) for x in input().split()]
        ob = Solution()
        ans = ob.minJumps(Arr, n)
        print(ans)
# } Driver Code Ends

'''https://practice.geeksforgeeks.org/problems/longest-consecutive-subsequence2449/1
Longest consecutive subsequence 
Medium Accuracy: 48.9% Submissions: 90141 Points: 4
Given an array of positive integers. Find the length of the longest sub-sequence such that elements in the subsequence are consecutive integers, the consecutive numbers can be in any order.
 

Example 1:

Input:
N = 7
a[] = {2,6,1,9,4,5,3}
Output:
6
Explanation:
The consecutive numbers here
are 1, 2, 3, 4, 5, 6. These 6 
numbers form the longest consecutive
subsquence.
Example 2:

Input:
N = 7
a[] = {1,9,3,10,4,20,2}
Output:
4
Explanation:
1, 2, 3, 4 is the longest
consecutive subsequence.

Your Task:
You don't need to read input or print anything. Your task is to complete the function findLongestConseqSubseq() which takes the array arr[] and the size of the array as inputs and returns the length of the longest subsequence of consecutive integers. 


Expected Time Complexity: O(N).
Expected Auxiliary Space: O(N).


Constraints:
1 <= N <= 105
0 <= a[i] <= 105'''

# User function Template for python3

import atexit
import sys
import io


class Solution:

    # arr[] : the input array
    # N : size of the array arr[]

    # Function to return length of longest subsequence of consecutive integers.
    def findLongestConseqSubseq(self, arr, N):
        # code here
        arr.sort()

        count = 1
        ans = 0

        v = []

        v.append(arr[0])

        for j in range(1, n):
            if arr[j-1] != arr[j]:
                v.append(arr[j])

        for i in range(len(v)):
            if v[i] == v[i-1] + 1:
                count += 1

            else:
                count = 1

            ans = max(ans, count)

        return ans

# {
#  Driver Code Starts
# Initial Template for Python 3


_INPUT_LINES = sys.stdin.read().splitlines()
input = iter(_INPUT_LINES).__next__
_OUTPUT_BUFFER = io.StringIO()
sys.stdout = _OUTPUT_BUFFER


@atexit.register
def write():
    sys.__stdout__.write(_OUTPUT_BUFFER.getvalue())


if __name__ == '__main__':
    t = int(input())
    for tt in range(t):
        n = int(input())
        a = list(map(int, input().strip().split()))
        print(Solution().findLongestConseqSubseq(a, n))
# } Driver Code Ends

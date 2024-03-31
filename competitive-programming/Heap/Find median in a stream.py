'''https://practice.geeksforgeeks.org/problems/find-median-in-a-stream-1587115620/1
Find median in a stream 
Hard Accuracy: 51.38% Submissions: 17203 Points: 8
Given an input stream of N integers. The task is to insert these numbers into a new stream and find the median of the stream formed by each insertion of X to the new stream.

Example 1:

Input:
N = 4
X[] = 5,15,1,3
Output:
5
10
5
4
Explanation:Flow in stream : 5, 15, 1, 3 
5 goes to stream --> median 5 (5) 
15 goes to stream --> median 10 (5,15) 
1 goes to stream --> median 5 (5,15,1) 
3 goes to stream --> median 4 (5,15,1 3) 
 

Example 2:

Input:
N = 3
X[] = 5,10,15
Output:
5
7.5
10
Explanation:Flow in stream : 5, 10, 15
5 goes to stream --> median 5 (5) 
10 goes to stream --> median 7.5 (5,10) 
15 goes to stream --> median 10 (5,10,15) 
Your Task:
You are required to complete the class Solution. 
It should have 2 data members to represent 2 heaps. 
It should have the following member functions:
insertHeap() which takes x as input and inserts it into the heap, the function should then call balanceHeaps() to balance the new heap.
balanceHeaps() does not take any arguments. It is supposed to balance the two heaps.
getMedian() does not take any arguments. It should return the current median of the stream.

Expected Time Complexity : O(nlogn)
Expected Auxilliary Space : O(n)
 
Constraints:
1 <= N <= 106
1 <= x <= 106'''

# {
# Driver Code Starts
# Initial Template for Python 3

import atexit
import io
import sys
import heapq
from collections import defaultdict
import math

# } Driver Code Ends
# User function Template for python3

''' 
use globals min_heap and max_heap, as per declared in driver code
use heapify modules , already imported by driver code
'''


class Solution:
    def __init__(self):
        self.minh, self.maxh = [], []
        heapq.heapify(self.minh)
        heapq.heapify(self.maxh)

    def balanceHeaps(self):
        # Balance the two heaps size , such that difference is not more than one.
        # code here
        if len(self.maxh) > len(self.minh)+1:
            heapq.heappush(self.minh, -heapq.heappop(self.maxh))
        elif len(self.minh) > len(self.maxh)+1:
            heapq.heappush(self.maxh, -heapq.heappop(self.minh))

    '''    
    You don't need to call getMedian it will be called itself by driver code
    for more info see drivers code below.
    '''

    def getMedian(self):
        # return the median of the data received till now.
        # code here
        if len(self.maxh) > len(self.minh):
            mx = heapq.heappop(self.maxh)
            heapq.heappush(self.maxh, mx)
            return -mx
        elif len(self.minh) > len(self.maxh):
            mn = heapq.heappop(self.minh)
            heapq.heappush(self.minh, mn)
            return mn
        else:
            mx = heapq.heappop(self.maxh)
            mn = heapq.heappop(self.minh)
            heapq.heappush(self.maxh, mx)
            heapq.heappush(self.minh, mn)
            return (mn-mx)/2

    def insertHeaps(self, x):
        #:param x: value to be inserted
        #:return: None
        # code here
        if len(self.maxh) == 0 or self.maxh[0] < -x:
            heapq.heappush(self.maxh, -x)
        else:
            heapq.heappush(self.minh, x)

        self.balanceHeaps()

# {
# Driver Code Starts.


if __name__ == '__main__':
    t = int(input())

    for _ in range(t):
        n = int(input())
        ob = Solution()
        for i in range(n):
            x = int(input())
            ob.insertHeaps(x)
            print(math.floor(ob.getMedian()))

# } Driver Code Ends

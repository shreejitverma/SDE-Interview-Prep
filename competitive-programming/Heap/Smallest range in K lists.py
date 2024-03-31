'''https://practice.geeksforgeeks.org/problems/find-smallest-range-containing-elements-from-k-lists/1
Smallest range in K lists 
Hard Accuracy: 50.36% Submissions: 11146 Points: 8
Given K sorted lists of integers, KSortedArray[] of size N each. The task is to find the smallest range that includes at least one element from each of the K lists. If more than one such range's are found, return the first such range found.

Example 1:

Input:
N = 5, K = 3
KSortedArray[][] = {{1 3 5 7 9},
                    {0 2 4 6 8},
                    {2 3 5 7 11}}
Output: 1 2
Explanation: K = 3
A:[1 3 5 7 9]
B:[0 2 4 6 8]
C:[2 3 5 7 11]
Smallest range is formed by number 1
present in first list and 2 is present
in both 2nd and 3rd list.
Example 2:

Input:
N = 4, K = 3
KSortedArray[][] = {{1 2 3 4},
                    {5 6 7 8},
                    {9 10 11 12}}
Output: 4 9
Your Task :

Complete the function findSmallestRange() that receives array , array size n and k as parameters and returns the output range (as a pair in cpp and array of size 2 in java and python)

Expected Time Complexity : O(n * k *log k)
Expected Auxilliary Space  : O(k)

Constraints:
1 <= K,N <= 500
0 <= a[ i ] <= 105'''

# User function Template for python3
import heapq


class Solution:
    def smallestRange(self, numbers, n, k):
        # code here
        # print the smallest range in a new line
        minheap = []
        maxx = -float('inf')
        for i in range(k):
            heapq.heappush(minheap, (numbers[i][0], i, 0))
            maxx = max(numbers[i][0], maxx)
        heapq.heapify(minheap)
        diff = maxx-minheap[0][0]
        ans = [minheap[0][0], maxx]

        while True:
            mini, r, c = heapq.heappop(minheap)
            if diff > maxx-mini:
                diff = maxx-mini
                ans = [mini, maxx]
            if c+1 >= len(numbers[r]):
                break
            num = numbers[r][c+1]
            maxx = max(maxx, num)
            heapq.heappush(minheap, (num, r, c+1))
        return ans

# {
#  Driver Code Starts
# Initial Template for Python 3


t = int(input())
for _ in range(t):
    line = input().strip().split()
    n = int(line[0])
    k = int(line[1])
    numbers = []
    for i in range(k):
        numbers.append([int(x) for x in input().strip().split()])
    r = Solution().smallestRange(numbers, n, k)
    print(r[0], r[1])
# } Driver Code Ends

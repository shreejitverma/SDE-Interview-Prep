'''https://practice.geeksforgeeks.org/problems/partition-array-to-k-subsets/1
Partition array to K subsets 
Hard Accuracy: 43.89% Submissions: 13428 Points: 8
Given an integer array a[ ] of N elements and an integer K, the task is to check if the array a[ ] could be divided into K non-empty subsets with equal sum of elements.
Note: All elements of this array should be part of exactly one partition.

Example 1:

Input: 
N = 5
a[] = {2,1,4,5,6}
K = 3
Output: 
1
Explanation: we can divide above array 
into 3 parts with equal sum as (2, 4), 
(1, 5), (6)
Example 2:

Input: 
N = 5 
a[] = {2,1,5,5,6}
K = 3
Output: 
0
Explanation: It is not possible to divide
above array into 3 parts with equal sum.
Your Task:
You don't need to read input or print anything. Your task is to complete the function isKPartitionPossible() which takes the array a[], the size of the array N, and the value of K as inputs and returns true(same as 1) if possible, otherwise false(same as 0).

Expected Time Complexity: O(N*2N).
Expected Auxiliary Space: O(2N).

Constraints:
1 ≤ K ≤ N ≤ 10
1 ≤ a[i] ≤ 100'''

from heapq import heappush, heappop, heapify


class Solution:
    def isKPartitionPossible(self, a, k):
        s = sum(a)
        if s % k != 0:
            return(False)
        tot = s//k
        h = [[0] for i in range(k)]
        heapify(h)
        a.sort(reverse=True)
        for i in a:
            [val] = heappop(h)
            heappush(h, [i+val])
        for [i] in h:
            if i != tot:
                return(False)
        return(True)
        # code here


# {
#  Driver Code Starts
if __name__ == '__main__':
    tcs = int(input())
    for _ in range(tcs):
        N = int(input())
        arr = [int(x) for x in input().split()]
        k = int(input())
        if Solution().isKPartitionPossible(arr, k):
            print(1)
        else:
            print(0)
# } Driver Code Ends

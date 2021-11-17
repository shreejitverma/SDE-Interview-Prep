'''https://practice.geeksforgeeks.org/problems/inversion-of-array-1587115620/1
Count Inversions 
Medium Accuracy: 39.43% Submissions: 100k+ Points: 4
Given an array of integers. Find the Inversion Count in the array. 

Inversion Count: For an array, inversion count indicates how far (or close) the array is from being sorted. If array is already sorted then the inversion count is 0. If an array is sorted in the reverse order then the inversion count is the maximum. 
Formally, two elements a[i] and a[j] form an inversion if a[i] > a[j] and i < j.
 

Example 1:

Input: N = 5, arr[] = {2, 4, 1, 3, 5}
Output: 3
Explanation: The sequence 2, 4, 1, 3, 5 
has three inversions (2, 1), (4, 1), (4, 3).
Example 2:

Input: N = 5
arr[] = {2, 3, 4, 5, 6}
Output: 0
Explanation: As the sequence is already 
sorted so there is no inversion count.
Example 3:

Input: N = 3, arr[] = {10, 10, 10}
Output: 0
Explanation: As all the elements of array 
are same, so there is no inversion count.
Your Task:
You don't need to read input or print anything. Your task is to complete the function inversionCount() which takes the array arr[] and the size of the array as inputs and returns the inversion count of the given array.

Expected Time Complexity: O(NLogN).
Expected Auxiliary Space: O(N).

Constraints:
1 ≤ N ≤ 5*105
1 ≤ arr[i] ≤ 1018'''

import atexit
import sys
import io


class Solution:
    # User function Template for python3

    # arr[]: Input Array
    # N : Size of the Array arr[]
    # Function to count inversions in the array.
    def inversionCount(self, arr, n):
        # Your Code Here
        self.c = 0
        self.temp = [None]*len(arr)
        self.mergeSort(arr, 0, n-1)
        return self.c

    def merge(self, arr, l, m, r):
        # code here

        k = l
        i, j = l, m
        while i < m and j <= r:
            if arr[i] <= arr[j]:
                self.temp[k] = arr[i]
                k += 1

                i += 1

            else:
                self.temp[k] = arr[j]
                k += 1
                j += 1
                # agar right array ka koi bhi index badha hua; to uke baad ke(from i to m ) ke saare index ke numbers use badhe hi honge coz they are sorted
                self.c += (m-i)
        while i < m:
            self.temp[k] = arr[i]
            k += 1
            i += 1
        while j <= r:
            self.temp[k] = arr[j]
            k += 1
            j += 1
        for i in range(l, r+1):
            arr[i] = self.temp[i]
        return

    def mergeSort(self, arr, l, r):
        if l < r:
            m = (l+r)//2
            self.mergeSort(arr, l, m)
            self.mergeSort(arr, m+1, r)
            self.merge(arr, l, m+1, r)
        return

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
        obj = Solution()
        print(obj.inversionCount(a, n))
# } Driver Code Ends

'''https://practice.geeksforgeeks.org/problems/sort-an-array-of-0s-1s-and-2s4231/1#
Sort an array of 0s, 1s and 2s 
Easy Accuracy: 51.36% Submissions: 100k+ Points: 2
Given an array of size N containing only 0s, 1s, and 2s; sort the array in ascending order.


Example 1:

Input: 
N = 5
arr[]= {0 2 1 2 0}
Output:
0 0 1 2 2
Explanation:
0s 1s and 2s are segregated 
into ascending order.
Example 2:

Input: 
N = 3
arr[] = {0 1 0}
Output:
0 0 1
Explanation:
0s 1s and 2s are segregated 
into ascending order.

Your Task:
You don't need to read input or print anything. Your task is to complete the function sort012() that takes an array arr and N as input parameters and sorts the array in-place.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)


Constraints:
1 <= N <= 10^6
0 <= A[i] <= 2'''


class Solution:
    def sort012(self, a, n):
        # code here
        l, m, h = 0, 0, n - 1
        while m <= h:
            if a[m] == 0:
                a[l], a[m] = a[m], a[l]
                l += 1
                m += 1
            elif a[m] == 1:
                m += 1
            else:
                a[m], a[h] = a[h], a[m]
                h -= 1

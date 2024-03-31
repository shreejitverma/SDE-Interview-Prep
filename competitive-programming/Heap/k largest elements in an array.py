'''https://practice.geeksforgeeks.org/problems/k-largest-elements4206/1
k largest elements 
Medium Accuracy: 53.0% Submissions: 27220 Points: 4
Given an array Arr of N positive integers, find K largest elements from the array.  The output elements should be printed in decreasing order.

Example 1:

Input:
N = 5, K = 2
Arr[] = {12, 5, 787, 1, 23}
Output: 787 23
Explanation: 1st largest element in the
array is 787 and second largest is 23.
Example 2:

Input:
N = 7, K = 3
Arr[] = {1, 23, 12, 9, 30, 2, 50}
Output: 50 30 23
Explanation: 3 Largest element in the
array are 50, 30 and 23.
Your Task:
You don't need to read input or print anything. Your task is to complete the function kLargest() which takes the array of integers arr, n and k as parameters and returns an array of integers denoting the answer. The array should be in decreasing order.

Expected Time Complexity: O(N + KlogK)
Expected Auxiliary Space: O(K+(N-K)*logK)

Constraints:
1 ≤ K ≤ N ≤ 105
1 ≤ Arr[i] ≤ 106'''


class Solution:

    def kLargest(self, arr, n, k):
        # code here
        # 		Sort the given array arr in reverse
        #         order.
    arr.sort(reverse=True)
    v = list()

    # Print the first kth largest elements
    for i in range(k):
        v.append(arr[i])

    return v

# Using Heap


def FirstKelements(arr, size, k):

    # Creating Min Heap for given
    # array with only k elements
    # Create min heap with priority queue
    minHeap = []
    for i in range(k):
        minHeap.append(arr[i])

    # Loop For each element in array
    # after the kth element
    for i in range(k, size):
        minHeap.sort()

        # If current element is smaller
        # than minimum ((top element of
        # the minHeap) element, do nothing
        # and continue to next element
        if (minHeap[0] > arr[i]):
            continue

        # Otherwise Change minimum element
        # (top element of the minHeap) to
        # current element by polling out
        # the top element of the minHeap
        else:
            minHeap.pop(0)
            minHeap.append(arr[i])

    # Now min heap contains k maximum
    # elements, Iterate and print
    for i in minHeap:
        print(i, end=" ")


# Driver code
arr = [11, 3, 2, 1, 15, 5, 4, 45, 88, 96, 50, 45]
size = len(arr)

# Size of Min Heap
k = 3
FirstKelements(arr, size, k)

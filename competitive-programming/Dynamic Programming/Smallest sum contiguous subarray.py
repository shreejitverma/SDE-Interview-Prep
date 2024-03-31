'''https://www.geeksforgeeks.org/smallest-sum-contiguous-subarray/
Given an array containing n integers. 
The problem is to find the sum of the elements of the contiguous subarray having the smallest(minimum) sum.
Examples: 
 

Input : arr[] = {3, -4, 2, -3, -1, 7, -5}
Output : -6
Subarray is {-4, 2, -3, -1} = -6

Input : arr = {2, 6, 8, 1, 4}
Output : 1

Naive Approach: Consider all the contiguous subarrays of different sizes and find their sum. The subarray having the smallest(minimum) sum is the required answer.
Efficient Approach: It is a variation to the problem of finding the largest sum contiguous subarray based on the idea of Kadane’s algorithm. 
Algorithm: 


smallestSumSubarr(arr, n)
    Initialize min_ending_here = INT_MAX
    Initialize min_so_far = INT_MAX
    
    for i = 0 to n-1
        if min_ending_here > 0
            min_ending_here = arr[i]        
        else
            min_ending_here += arr[i]
        min_so_far = min(min_so_far, min_ending_here)

    return min_so_far'''

# Python program to find the smallest sum
# contiguous subarray
import sys

# function to find the smallest sum
# contiguous subarray


def smallestSumSubarr(arr, n):
    # to store the minimum value that is ending
    # up to the current index
    min_ending_here = sys.maxsize

    # to store the minimum value encountered so far
    min_so_far = sys.maxsize

    # traverse the array elements
    for i in range(n):
        # if min_ending_here > 0, then it could not possibly
        # contribute to the minimum sum further
        if (min_ending_here > 0):
            min_ending_here = arr[i]

        # else add the value arr[i] to min_ending_here
        else:
            min_ending_here += arr[i]

        # update min_so_far
        min_so_far = min(min_so_far, min_ending_here)

    # required smallest sum contiguous subarray value
    return min_so_far


# Driver code
arr = [3, -4, 2, -3, -1, 7, -5]
n = len(arr)
print "Smallest sum: ", smallestSumSubarr(arr, n)

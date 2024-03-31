'''https://www.geeksforgeeks.org/k-th-largest-sum-contiguous-subarray/
Given an array of integers. Write a program to find the K-th largest sum of contiguous subarray within the array of numbers which has negative and positive numbers.

Examples: 
Input: a[] = {20, -5, -1} 
         k = 3
Output: 14
Explanation: All sum of contiguous 
subarrays are (20, 15, 14, -5, -6, -1) 
so the 3rd largest sum is 14.

Input: a[] = {10, -10, 20, -40} 
         k = 6
Output: -10 
Explanation: The 6th largest sum among 
sum of all contiguous subarrays is -10.
A brute force approach is to store all the contiguous sums in another array and sort it and print the k-th largest. But in the case of the number of elements being large, the array in which we store the contiguous sums will run out of memory as the number of contiguous subarrays will be large (quadratic order)



An efficient approach is to store the pre-sum of the array in a sum[] array. We can find sum of contiguous subarray from index i to j as sum[j]-sum[i-1] 

Now for storing the Kth largest sum, use a min heap (priority queue) in which we push the contiguous sums till we get K elements, once we have our K elements, check if the element is greater than the Kth element it is inserted to the min heap with popping out the top element in the min-heap, else not inserted. In the end, the top element in the min-heap will be your answer.

Below is the implementation of the above approach.  

C++JavaPython3C#
'''

# Python program to find the k-th largest sum
# of subarray
import heapq

# function to calculate kth largest element
# in contiguous subarray sum


def kthLargestSum(arr, n, k):

    # array to store predix sums
    sum = []
    sum.append(0)
    sum.append(arr[0])
    for i in range(2, n + 1):
        sum.append(sum[i - 1] + arr[i - 1])

    # priority_queue of min heap
    Q = []
    heapq.heapify(Q)

    # loop to calculate the contiguous subarray
    # sum position-wise
    for i in range(1, n + 1):

        # loop to traverse all positions that
        # form contiguous subarray
        for j in range(i, n + 1):
            x = sum[j] - sum[i - 1]

            # if queue has less then k elements,
            # then simply push it
            if len(Q) < k:
                heapq.heappush(Q, x)
            else:
                # it the min heap has equal to
                # k elements then just check
                # if the largest kth element is
                # smaller than x then insert
                # else its of no use
                if Q[0] < x:
                    heapq.heappop(Q)
                    heapq.heappush(Q, x)

    # the top element will be then kth
    # largest element
    return Q[0]


# Driver program to test above function
a = [10, -10, 20, -40]
n = len(a)
k = 6

# calls the function to find out the
# k-th largest sum
print(kthLargestSum(a, n, k))

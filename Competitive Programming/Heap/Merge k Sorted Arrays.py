'''https://practice.geeksforgeeks.org/problems/merge-k-sorted-arrays/1
https://www.geeksforgeeks.org/merge-k-sorted-arrays/

Merge k Sorted Arrays 
Medium Accuracy: 51.19% Submissions: 41264 Points: 4
Given K sorted arrays arranged in the form of a matrix of size K*K. 
The task is to merge them into one sorted array.
Example 1:

Input:
K = 3
arr[][] = {{1,2,3},{4,5,6},{7,8,9}}
Output: 1 2 3 4 5 6 7 8 9
Explanation:Above test case has 3 sorted
arrays of size 3, 3, 3
arr[][] = [[1, 2, 3],[4, 5, 6], 
[7, 8, 9]]
The merged list will be 
[1, 2, 3, 4, 5, 6, 7, 8, 9].
Example 2:

Input:
K = 4
arr[][]={{1,2,3,4}{2,2,3,4},
         {5,5,6,6},{7,8,9,9}}
Output:
1 2 2 2 3 3 4 4 5 5 6 6 7 8 9 9 
Explanation: Above test case has 4 sorted
arrays of size 4, 4, 4, 4
arr[][] = [[1, 2, 2, 2], [3, 3, 4, 4],
[5, 5, 6, 6]  [7, 8, 9, 9 ]]
The merged list will be 
[1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 
6, 6, 7, 8, 9, 9 ].
Your Task:
You do not need to read input or print anything. 
Your task is to complete mergeKArrays() function which takes 2 arguments, 
an arr[K][K] 2D Matrix containing K sorted arrays 
and an integer K denoting the number of sorted arrays, 
as input and returns the merged sorted array 
( as a pointer to the merged sorted arrays in cpp, as an ArrayList in java, and list in python)

Expected Time Complexity: O(K2*Log(K))
Expected Auxiliary Space: O(K)

Constraints:
1 <= K <= 100'''


from heapq import merge


class Solution:
    # Function to merge k sorted arrays.
    def mergeKArrays(self, arr, K):
        # code here
        # return merged list
        output = []
        for i in range(K):
            for j in range(K):
                output.append(arr[i][j])
        output.sort()
        return output


'''
https://www.geeksforgeeks.org/merge-k-sorted-arrays-set-2-different-sized-arrays/
Merge k sorted arrays | Set 2 (Different Sized Arrays)
Difficulty Level : Medium
Last Updated : 16 Feb, 2021
Given k sorted arrays of possibly different sizes, merge them and print the sorted output.
Examples: 
 

Input: k = 3 
      arr[][] = { {1, 3},
                  {2, 4, 6},
                  {0, 9, 10, 11}} ;
Output: 0 1 2 3 4 6 9 10 11 

Input: k = 2
      arr[][] = { {1, 3, 20},
                  {2, 4, 6}} ;
Output: 1 2 3 4 6 20 '''

# merge function merge two arrays
# of different or same length
# if n = max(n1, n2)
# time complexity of merge is (o(n log(n)))


# function for meging k arrays

def mergeK(arr, k):

    l = arr[0]
    for i in range(k-1):

        # when k = 0 it merge arr[1]
        # with arr[0] here in l arr[0]
        # is stored
        l = list(merge(l, arr[i + 1]))
    return l

# for printing array


def printArray(arr):
    print(*arr)


# driver code
arr = [[2, 6, 12],
       [1, 9],
       [23, 34, 90, 2000]]
k = 3

l = mergeK(arr, k)

printArray(l)

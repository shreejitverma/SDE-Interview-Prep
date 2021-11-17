'''https://www.geeksforgeeks.org/rearrange-array-alternating-positive-negative-items-o1-extra-space/
Given an array of positive and negative numbers, arrange them in an alternate fashion such that every positive number is followed by negative and vice-versa maintaining the order of appearance. 
Number of positive and negative numbers need not be equal. If there are more positive numbers they appear at the end of the array. If there are more negative numbers, they too appear in the end of the array.

Examples : 
Input:  arr[] = {1, 2, 3, -4, -1, 4}
Output: arr[] = {-4, 1, -1, 2, 3, 4}

Input:  arr[] = {-5, -2, 5, 2, 4, 7, 1, 8, 0, -8}
output: arr[] = {-5, 5, -2, 2, -8, 4, 7, 1, 8, 0}
Naive Approach : 
The above problem can be easily solved if O(n) extra space is allowed. It becomes interesting due to the limitations that O(1) extra space and order of appearances. 
The idea is to process array from left to right. While processing, find the first out of place element in the remaining unprocessed array. An element is out of place if it is negative and at odd index, or it is positive and at even index. Once we find an out of place element, we find the first element after it with opposite sign. We right rotate the subarray between these two elements (including these two).

Following is the implementation of above idea.  

Output
Given array is 
-5 -2 5 2 4 7 1 8 0 -8 
Rearranged array is 
-5 5 -2 2 -8 4 7 1 8 0 
Time Complexity : O(N^2)

Space Complexity : O(1)

Efficient Approach : 

We first sort the array in non-increasing order.Then we will count the number of positive and negative integers. Then we will swap the one negative and one positive

number till we reach our condition. This will rearrange the array elements because we are sorting the array and accessing the element from left to right according to our need.'''


# Python3 program to rearrange
# positive and negative integers
# in alternate fashion and
# maintaining the order of positive
# and negative numbers

# rotates the array to right by once
# from index 'outOfPlace to cur'


def rightRotate(arr, n, outOfPlace, cur):
    temp = arr[cur]
    for i in range(cur, outOfPlace, -1):
        arr[i] = arr[i - 1]
    arr[outOfPlace] = temp
    return arr


def rearrange(arr, n):
    outOfPlace = -1
    for index in range(n):
        if(outOfPlace >= 0):

            # if element at outOfPlace place in
            # negative and if element at index
            # is positive we can rotate the
            # array to right or if element
            # at outOfPlace place in positive and
            # if element at index is negative we
            # can rotate the array to right
            if((arr[index] >= 0 and arr[outOfPlace] < 0) or
               (arr[index] < 0 and arr[outOfPlace] >= 0)):
                arr = rightRotate(arr, n, outOfPlace, index)
                if(index-outOfPlace > 2):
                    outOfPlace += 2
                else:
                    outOfPlace = - 1

        if(outOfPlace == -1):

            # conditions for A[index] to
            # be in out of place
            if((arr[index] >= 0 and index % 2 == 0) or
               (arr[index] < 0 and index % 2 == 1)):
                outOfPlace = index
    return arr


# Driver Code
arr = [-5, -2, 5, 2, 4,
       7, 1, 8, 0, -8]

print("Given Array is:")
print(arr)

print("\nRearranged array is:")
print(rearrange(arr, len(arr)))

# Below is the implementation of the above approach


def rearrange(arr, n):
    # sort the array
    arr.sort()

    # initialize two pointers
    # one pointing to the negative number
    # one pointing to the positive number
    i, j = 1, 1
    while j < n:
        if arr[j] > 0:
            break
        j += 1

    # swap the numbers until the given condition gets satisfied
    while (arr[i] < 0) and (j < n):
        # swaping
        arr[i], arr[j] = arr[j], arr[i]

        # increment i by 2
        # because a negative number is followed by a positive number
        i += 2
        j += 1

    return(arr)


# Driver Code
# Given array
arr = [-5, -2, 5, 2, 4, 7, 1, 8, 0, -8]

ans = rearrange(arr, len(arr))
for num in ans:
    print(num, end=" ")
'''Output
-8 1 -2 0 -5 2 4 5 7 8 
Time Complexity: O(N*logN)

Space Complexity: O(1)'''

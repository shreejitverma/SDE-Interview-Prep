'''https://www.geeksforgeeks.org/move-negative-numbers-beginning-positive-end-constant-extra-space/
An array contains both positive and negative numbers in random order. Rearrange the array elements so that all negative numbers appear before all positive numbers.

Examples : 
Input: -12, 11, -13, -5, 6, -7, 5, -3, -6
Output: -12 -13 -5 -7 -3 -6 11 6 5
Note: Order of elements is not important here.

Approach 1:
The idea is to simply apply the partition process of quicksort. '''

# A Python 3 program to put
# all negative numbers before
# positive numbers


def rearrange(arr, n):

    # Please refer partition() in
    # below post
    # https://www.geeksforgeeks.org / quick-sort / j = 0
    j = 0
    for i in range(0, n):
        if (arr[i] < 0):
            temp = arr[i]
            arr[i] = arr[j]
            arr[j] = temp
            j = j + 1
    print(arr)


# Driver code
arr = [-1, 2, -3, 4, 5, 6, -7, 8, 9]
n = len(arr)
rearrange(arr, n)


'''Output
-1 -3 -7 4 5 6 2 8 9
Time complexity: O(N) 
Auxiliary Space: O(1)

Two Pointer Approach: The idea is to solve this problem with constant space and linear time is by using a two-pointer or two-variable approach where we simply take two variables like left and right which hold the 0 and N-1 indexes. Just need to check that :

Check If the left and right pointer elements are negative then simply increment the left pointer.
Otherwise, if the left element is positive and the right element is negative then simply swap the elements, and simultaneously increment and decrement the left and right pointers.
Else if the left element is positive and the right element is also positive then simply decrement the right pointer.
Repeat the above 3 steps until the left pointer ≤ right pointer.
Below is the implementation of the above approach:'''
# Python3 program of the
# above approach

# Function to shift all the
# the negative elements to
# the left of the array


def shiftall(arr, left, right):

    # Loop to iterate while the
    # left pointer is less than
    # the right pointer
    while left <= right:

        # Condition to check if the left
        # and right pointer negative
        if arr[left] < 0 and arr[right] < 0:
            left += 1

        # Condition to check if the left
        # pointer element is positive and
        # the right pointer element is
        # negative
        elif arr[left] > 0 and arr[right] < 0:
            arr[left], arr[right] = \
                arr[right], arr[left]
            left += 1
            right -= 1

        # Condition to check if the left
        # pointer is positive and right
        # pointer as well
        elif arr[left] > 0 and arr[right] > 0:
            right -= 1
        else:
            left += 1
            right -= 1

# Function to print the array


def display(arr):
    for i in range(len(arr)):
        print(arr[i], end=" ")
    print()


# Driver Code
if __name__ == "__main__":
    arr = [-12, 11, -13, -5,
           6, -7, 5, -3, 11]
    n = len(arr)
    shiftall(arr, 0, n-1)
    display(arr)
'''Output
-12 -3 -13 -5 -7 6 5 11 11
This is an in-place rearranging algorithm for arranging the positive and negative numbers where the order of elements is not maintained.

Time Complexity: O(N)
Auxiliary Space: O(1)'''

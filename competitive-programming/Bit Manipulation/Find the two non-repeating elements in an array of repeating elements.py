'''Given an array A containing 2*N+2 positive numbers, out of which 2*N numbers exist in pairs whereas the other two number occur exactly once and are distinct. Find the other two numbers.


Example 1:

Input:
N = 2
arr[] = {1, 2, 3, 2, 1, 4}
Output:
3 4
Explanation:
3 and 4 occur exactly once.
Example 2:

Input:
N = 1
arr[] = {2, 1, 3, 2}
Output:
1 3
Explanation:
1 3 occur exactly once.

Your Task:
You do not need to read or print anything. Your task is to complete the function singleNumber() which takes the array as input parameter and returns a list of two numbers which occur exactly once in the array. The list must be in ascending order.


Expected Time Complexity: O(N)
Expected Space Complexity: O(1)


Constraints:
1 <= length of array <= 106
1 <= Elements in array <= 5 * 106
'''

# Python3 program for above approach

# This function sets the values of
# *x and *y to non-repeating elements
# in an array arr[] of size n


def UniqueNumbers2(arr):
    sums = 0
    list = []
    n = len(arr)

    for i in range(0, n):

        # Xor  all the elements of the array
        # all the elements occurring twice will
        # cancel out each other remaining
        # two unique numbers will be xored
        sums = (sums ^ arr[i])

    # Bitwise & the sum with it's 2's Complement
    # Bitwise & will give us the sum containing
    # only the rightmost set bit
    sums = (sums & -sums)

    # sum1 and sum2 will contains 2 unique
    # elements elements initialized with 0 box
    # number xored with 0 is number itself
    sum1 = 0
    sum2 = 0

    # Traversing the array again
    for i in range(0, len(arr)):

        # Bitwise & the arr[i] with the sum
        # Two possibilities either result == 0
        # or result > 0
        if (arr[i] & sums) > 0:

            # If result > 0 then arr[i] xored
            # with the sum1
            sum1 = (sum1 ^ arr[i])

        else:

            # If result == 0 then arr[i]
            # xored with sum2
            sum2 = (sum2 ^ arr[i])

    list.append(sum1)
    list.append(sum2)
    list.sort()
    return list

    # # Print the the two unique numbers
    # print("The non-repeating elements are ", sum1, " and ", sum2)


# Driver Code
if __name__ == "__main__":

    arr = [2, 3, 7, 9, 11, 2, 3, 11]
    # n = len(arr)

    print(UniqueNumbers2(arr))

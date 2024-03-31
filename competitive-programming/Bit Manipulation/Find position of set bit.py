'''Given a number N having only one ‘1’ and all other ’0’s in its binary representation, find position of the only set bit. If there are 0 or more than 1 set bit the answer should be -1. Position of  set bit '1' should be counted starting with 1 from LSB side in binary representation of the number.

Example 1:

Input:
N = 2
Output:
2
Explanation:
2 is represented as "10" in Binary.
As we see there's only one set bit
and it's in Position 2 and thus the
Output 2.
Example 2:

Input:
N = 5
Output:
-1
Explanation:
5 is represented as "101" in Binary.
As we see there's two set bits
and thus the Output -1.
Your Task:
You don't need to read input or print anything. Your task is to complete the function findPosition() which takes an integer N as input and returns the answer.

Expected Time Complexity: O(log(N))
Expected Auxiliary Space: O(1)

Constraints:
0 <= N <= 108'''
# Python3 program to find position of
# only set bit in a given number

# A utility function to check
# whether n is power of 2 or
# not.


def isPowerOfTwo(n):
    return (True if(n > 0 and
                    ((n & (n - 1)) > 0))
            else False)

# Returns position of the
# only set bit in 'n'


def findPosition(n):
    if (isPowerOfTwo(n) == True):
        return -1

    i = 1
    pos = 1

    # Iterate through bits of n
    # till we find a set bit i&n
    # will be non-zero only when
    # 'i' and 'n' have a set bit
    # at same position
    while ((i & n) == 0):

        # Unset current bit and
        # set the next bit in 'i'
        i = i << 1

        # increment position
        pos += 1

    return pos

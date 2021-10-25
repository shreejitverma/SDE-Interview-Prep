'''
https://www.geeksforgeeks.org/count-total-set-bits-in-all-numbers-from-1-to-n-set-2/
https://practice.geeksforgeeks.org/problems/count-total-set-bits-1587115620/1
Count total set bits 
Medium Accuracy: 45.48% Submissions: 52672 Points: 4
You are given a number N. Find the total count of set bits for all numbers from 1 to N(both inclusive).

Example 1:

Input: N = 4
Output: 5
Explanation:
For numbers from 1 to 4.
For 1: 0 0 1 = 1 set bits
For 2: 0 1 0 = 1 set bits
For 3: 0 1 1 = 2 set bits
For 4: 1 0 0 = 1 set bits
Therefore, the total set bits is 5.
Example 2:

Input: N = 17
Output: 35
Explanation: From numbers 1 to 17(both inclusive), 
the total number of set bits is 35.

Your Task: The task is to complete the function countSetBits() that takes n as a parameter and returns the count of all bits.

Expected Time Complexity: O(log N).
Expected Auxiliary Space: O(1).

Constraints:
1 ≤ N ≤ 108

'''

# Python3 implementation of the approach

# Function to return the sum of the count
# of set bits in the integers from 1 to n


def countSetBits(n):

    # Ignore 0 as all the bits are unset
    n += 1

    # To store the powers of 2
    powerOf2 = 2

    # To store the result, it is initialized
    # with n/2 because the count of set
    # least significant bits in the integers
    # from 1 to n is n/2
    cnt = n // 2

    # Loop for every bit required to represent n
    while (powerOf2 <= n):

        # Total count of pairs of 0s and 1s
        totalPairs = n // powerOf2

        # totalPairs/2 gives the complete
        # count of the pairs of 1s
        # Multiplying it with the current power
        # of 2 will give the count of
        # 1s in the current bit
        cnt += (totalPairs // 2) * powerOf2

        # If the count of pairs was odd then
        # add the remaining 1s which could
        # not be groupped together
        if (totalPairs & 1):
            cnt += (n % powerOf2)
        else:
            cnt += 0

        # Next power of 2
        powerOf2 <<= 1

    # Return the result
    return cnt


# Driver code
if __name__ == "__main__":

    n = 14

    print(countSetBits(n))

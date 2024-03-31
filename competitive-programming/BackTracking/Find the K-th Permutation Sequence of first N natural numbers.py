'''https://www.geeksforgeeks.org/find-the-k-th-permutation-sequence-of-first-n-natural-numbers/
Given two integers N and K, find the Kth permutation sequence of numbers from 1 to N without using STL function.
Note: Assume that the inputs are such that Kth permutation of N number is always possible.

Examples: 

Naive Approach:
To solve the problem mentioned above the simple approach is to find all permutation sequences and output the kth out of them. But this method is not so efficient and takes more time, hence it can be optimized.



Efficient Approach:
To optimize the above method mentioned above, observe that the value of k can be directly used to find the number at each index of the sequence.  

The first position of an n length sequence is occupied by each of the numbers from 1 to n exactly n! / n that is (n-1)! number of times and in ascending order. So the first position of the kth sequence will be occupied by the number present at index = k / (n-1)! (according to 1-based indexing).
The currently found number can not occur again so it is removed from the original n numbers and now the problem reduces to finding the ( k % (n-1)! )th permutation sequence of the remaining n-1 numbers.
This process can be repeated until we have only one number left which will be placed in the first position of the last 1-length sequence.
The factorial values involved here can be very large as compared to k. So, the trick used to avoid the full computation of such large factorials is that as soon as the product n * (n-1) * … becomes greater than k, we no longer need to find the actual factorial value because: 
k / n_actual_factorial_value = 0 
and k / n_partial_factorial_value = 0 
when partial_factorial_value > k 
 

Below is the implementation of the above approach: '''

# Python3 program to find the kth permutation
# Sequence of first n natural numbers

# Function to find the index of number
# at first position of kth sequence of
# set of size n


def findFirstNumIndex(k, n):

    if (n == 1):
        return 0, k

    n -= 1

    first_num_index = 0

    # n_actual_fact = n!
    n_partial_fact = n

    while (k >= n_partial_fact and n > 1):
        n_partial_fact = n_partial_fact * (n - 1)
        n -= 1

    # First position of the kth sequence
    # will be occupied by the number present
    # at index = k / (n-1)!
    first_num_index = k // n_partial_fact

    k = k % n_partial_fact

    return first_num_index, k

# Function to find the
# kth permutation of n numbers


def findKthPermutation(n, k):

    # Store final answer
    ans = ""

    s = set()

    # Insert all natural number
    # upto n in set
    for i in range(1, n + 1):
        s.add(i)

    # Subtract 1 to get 0 based indexing
    k = k - 1

    for i in range(n):

        # Mark the first position
        itr = list(s)

        index, k = findFirstNumIndex(k, n - i)

        # itr now points to the
        # number at index in set s
        ans += str(itr[index])

        # remove current number from the set
        itr.pop(index)

        s = set(itr)

    return ans


# Driver code
if __name__ == '__main__':

    n = 3
    k = 4

    kth_perm_seq = findKthPermutation(n, k)

    print(kth_perm_seq)

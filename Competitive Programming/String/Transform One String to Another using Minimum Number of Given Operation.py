'''https://www.geeksforgeeks.org/transform-one-string-to-another-using-minimum-number-of-given-operation/

Given two strings A and B, the task is to convert A to B if possible. The only operation allowed is to put any character from A and insert it at front. Find if it’s possible to convert the string. If yes, then output minimum no. of operations required for transformation.

Examples: 

In case you wish to attend live classes with experts, please refer DSA Live Classes for Working Professionals and Competitive Programming Live for Students.
Input:  A = "ABD", B = "BAD"
Output: 1
Explanation: Pick B and insert it at front.

Input:  A = "EACBD", B = "EABCD"
Output: 3
Explanation: Pick B and insert at front, EACBD => BEACD
             Pick A and insert at front, BEACD => ABECD
             Pick E and insert at front, ABECD => EABCD

Checking whether a string can be transformed to another is simple. We need to check whether both strings have same number of characters and same set of characters. This can be easily done by creating a count array for first string and checking if second string has same count of every character. 
How to find minimum number of operations when we are sure that we can transform A to B? The idea is to start matching from last characters of both strings. If last characters match, then our task reduces to n-1 characters. If last characters don’t match, then find the position of B’s mismatching character in A. The difference between two positions indicates that these many characters of A must be moved before current character of A. 



Below is complete algorithm. 
1) Find if A can be transformed to B or not by first creating a count array for all characters of A, then checking with B if B has same count for every character. 
2) Initialize result as 0. 
3) Start traversing from end of both strings. 
……a) If current characters of A and B match, i.e., A[i] == B[j] 
………then do i = i-1 and j = j-1 
    b) If current characters don’t match, then search B[j] in remaining 
………A. While searching, keep incrementing result as these characters 
………must be moved ahead for A to B transformation.

Below are the implementations based on this idea.  '''
# Python program to find the minimum number of
# operations required to transform one string to other

# Function to find minimum number of operations required
# to transform A to B


def minOps(A, B):
    m = len(A)
    n = len(B)

    # This part checks whether conversion is possible or not
    if n != m:
        return -1

    count = [0] * 256

    for i in xrange(n):        # count characters in A
        count[ord(B[i])] += 1
    for i in xrange(n):        # subtract count for every char in B
        count[ord(A[i])] -= 1
    for i in xrange(256):    # Check if all counts become 0
        if count[i]:
            return -1

    # This part calculates the number of operations required
    res = 0
    i = n-1
    j = n-1
    while i >= 0:

        # if there is a mismatch, then keep incrementing
        # result 'res' until B[j] is not found in A[0..i]
        while i >= 0 and A[i] != B[j]:
            i -= 1
            res += 1

        # if A[i] and B[j] match
        if i >= 0:
            i -= 1
            j -= 1

    return res


# Driver program
A = "EACBD"
B = "EABCD"
print "Minimum number of operations required is " + str(minOps(A, B))

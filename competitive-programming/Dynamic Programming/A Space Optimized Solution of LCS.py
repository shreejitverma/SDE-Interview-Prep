'''https://www.geeksforgeeks.org/space-optimized-solution-lcs/
Given two strings, find the length of the longest subsequence present in both of them. 
 

Examples: 

LCS for input Sequences “ABCDGH” and “AEDFHR” is “ADH” of length 3. 
LCS for input Sequences “AGGTAB” and “GXTXAYB” is “GTAB” of length 4.
We have discussed a typical dynamic programming-based solution for LCS. 
We can optimize the space used by LCS problem. We know the recurrence relationship of the LCS problem is 

How to find the length of LCS is O(n) auxiliary space?

One important observation in the above simple implementation is, in each iteration of the outer loop 
we only need values from all columns of the previous row. So there is no need to store all rows in our DP matrix, 
we can just store two rows at a time and use them. In that way, used space will be reduced 
from L[m+1][n+1] to L[2][n+1]. 
Below is the implementation of the above idea. '''


# Space optimized Python
# implementation of LCS problem

# Returns length of LCS for
# X[0..m-1], Y[0..n-1]
def lcs(X, Y):

    # Find lengths of two strings
    m = len(X)
    n = len(Y)

    L = [[0 for i in range(n+1)] for j in range(2)]

    # Binary index, used to index current row and
    # previous row.
    bi = bool

    for i in range(m):
        # Compute current binary index
        bi = i & 1

        for j in range(n+1):
            if (i == 0 or j == 0):
                L[bi][j] = 0

            elif (X[i] == Y[j - 1]):
                L[bi][j] = L[1 - bi][j - 1] + 1

            else:
                L[bi][j] = max(L[1 - bi][j],
                               L[bi][j - 1])

    # Last filled entry contains length of LCS
    # for X[0..n-1] and Y[0..m-1]
    return L[bi][n]


# Driver Code
X = "AGGTAB"
Y = "GXTXAYB"

print("Length of LCS is", lcs(X, Y))

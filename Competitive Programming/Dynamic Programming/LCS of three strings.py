'''https://practice.geeksforgeeks.org/problems/lcs-of-three-strings0028/1
https://www.geeksforgeeks.org/lcs-longest-common-subsequence-three-strings/
LCS of three strings 
Medium Accuracy: 54.38% Submissions: 10927 Points: 4
Given 3 strings A, B and C, the task is to find the longest common sub-sequence in all three given sequences.

Example 1:

Input:
A = "geeks", B = "geeksfor", 
C = "geeksforgeeks"
Output: 5
Explanation: "geeks"is the longest common
subsequence with length 5.
â€‹Example 2:

Input: 
A = "abcd", B = "efgh", C = "ijkl"
Output: 0
Explanation: There's no common subsequence
in all the strings.

Your Task:
You don't need to read input or print anything. Your task is to complete the function LCSof3() which takes the strings A, B, C and their lengths n1, n2, n3 as input and returns the length of the longest common subsequence in all the 3 strings.


Expected Time Complexity: O(n1*n2*n3).
Expected Auxiliary Space: O(n1*n2*n3).


Constraints:
1<=n1, n2, n3<=20'''

# Python program to find
# LCS of three strings

# Returns length of LCS
# for X[0..m-1], Y[0..n-1]
# and Z[0..o-1]


def lcsOf3(X, Y, Z, m, n, o):

    L = [[[0 for i in range(o+1)] for j in range(n+1)]
         for k in range(m+1)]

    ''' Following steps build L[m+1][n+1][o+1] in
    bottom up fashion. Note that L[i][j][k]
    contains length of LCS of X[0..i-1] and
    Y[0..j-1] and Z[0.....k-1] '''
    for i in range(m+1):
        for j in range(n+1):
            for k in range(o+1):
                if (i == 0 or j == 0 or k == 0):
                    L[i][j][k] = 0

                elif (X[i-1] == Y[j-1] and
                      X[i-1] == Z[k-1]):
                    L[i][j][k] = L[i-1][j-1][k-1] + 1

                else:
                    L[i][j][k] = max(max(L[i-1][j][k],
                                         L[i][j-1][k]),
                                     L[i][j][k-1])

    # L[m][n][o] contains length of LCS for
    # X[0..n-1] and Y[0..m-1] and Z[0..o-1]
    return L[m][n][o]

# Driver program to test above function


X = 'AGGT12'
Y = '12TXAYB'
Z = '12XBA'

m = len(X)
n = len(Y)
o = len(Z)

print('Length of LCS is', lcsOf3(X, Y, Z, m, n, o))


# Python3 program to find LCS of
# three strings
X = "AGGT12"
Y = "12TXAYB"
Z = "12XBA"

dp = [[[-1 for i in range(100)]
       for j in range(100)]
      for k in range(100)]

# Returns length of LCS for
# X[0..m-1], Y[0..n-1] and Z[0..o-1]


def lcsOf3(i, j, k):

    if(i == -1 or j == -1 or k == -1):
        return 0

    if(dp[i][j][k] != -1):
        return dp[i][j][k]

    if(X[i] == Y[j] and Y[j] == Z[k]):
        dp[i][j][k] = 1 + lcsOf3(i - 1,
                                 j - 1, k - 1)
        return dp[i][j][k]

    else:
        dp[i][j][k] = max(max(lcsOf3(i - 1, j, k),
                              lcsOf3(i, j - 1, k)),
                          lcsOf3(i, j, k - 1))

        return dp[i][j][k]


# Driver code
if __name__ == "__main__":
    m = len(X)
    n = len(Y)
    o = len(Z)

    print("Length of LCS is",
          lcsOf3(m - 1, n - 1, o - 1))

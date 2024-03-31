'''https: // www.geeksforgeeks.org/longest-palindromic-subsequence-dp-12/
if the given sequence is “BBABCBCAB”, then the output should be 7 as “BABCBAB” is the longest palindromic subsequence in it. 
“BBBBB” and “BBCBB” are also palindromic subsequences of the given sequence, but not the longest ones.
The naive solution for this problem is to generate all subsequences of the given sequence 
and find the longest palindromic subsequence. This solution is exponential in term of time complexity. 
Let us see how this problem possesses both important properties of a Dynamic Programming (DP) Problem 
and can efficiently be solved using Dynamic Programming.
1) Optimal Substructure: 
Let X[0..n-1] be the input sequence of length n and L(0, n-1) be the length of the longest palindromic subsequence of X[0..n-1]. 
If last and first characters of X are same, then L(0, n-1) = L(1, n-2) + 2. 
Else L(0, n-1) = MAX (L(1, n-1), L(0, n-2)). 
 



Recommended: Please try your approach on {IDE} first, before moving on to the solution.
Following is a general recursive solution with all cases handled. 
 

// Every single character is a palindrome of length 1
L(i, i) = 1 for all indexes i in given sequence

// IF first and last characters are not same
If (X[i] != X[j])  L(i, j) =  max{L(i + 1, j),L(i, j - 1)} 

// If there are only 2 characters and both are same
Else if (j == i + 1) L(i, j) = 2  

// If there are more than two characters, and first and last 
// characters are same
Else L(i, j) =  L(i + 1, j - 1) + 2 
2) Overlapping Subproblems 
Following is a simple recursive implementation of the LPS problem. 
The implementation simply follows the recursive structure mentioned above. '''

# Python 3 program of above approach

# A utility function to get max
# of two egers


def max(x, y):
    if(x > y):
        return x
    return y

# Returns the length of the longest
# palindromic subsequence in seq


def lps(seq, i, j):

    # Base Case 1: If there is
    # only 1 character
    if (i == j):
        return 1

    # Base Case 2: If there are only 2
    # characters and both are same
    if (seq[i] == seq[j] and i + 1 == j):
        return 2

    # If the first and last characters match
    if (seq[i] == seq[j]):
        return lps(seq, i + 1, j - 1) + 2

    # If the first and last characters
    # do not match
    return max(lps(seq, i, j - 1),
               lps(seq, i + 1, j))


# Driver Code
if __name__ == '__main__':
    seq = "GEEKSFORGEEKS"
    n = len(seq)
    print("The length of the LPS is",
          lps(seq, 0, n - 1))

'''
Considering the above implementation, the following is a partial recursion tree 
for a sequence of length 6 with all different characters. 
 

               L(0, 5)
             /        \ 
            /          \  
        L(1,5)          L(0,4)
       /    \            /    \
      /      \          /      \
  L(2,5)    L(1,4)  L(1,4)  L(0,3)
In the above partial recursion tree, L(1, 4) is being solved twice. 
If we draw the complete recursion tree, then we can see that there are many subproblems which are solved again and again. 
Since the same subproblems are called again, this problem has Overlapping Subproblems property. 
So LPS problem has both properties (see this and this) of a dynamic programming problem. 
Like other typical Dynamic Programming(DP) problems, 
recomputations of same subproblems can be avoided by constructing a temporary array L[][] in a bottom-up manner.
Dynamic Programming Solution'''

# A Dynamic Programming based Python
# program for LPS problem Returns the length
#  of the longest palindromic subsequence in seq


def lps(str):
    n = len(str)

    # Create a table to store results of subproblems
    L = [[0 for x in range(n)] for x in range(n)]

    # Strings of length 1 are palindrome of length 1
    for i in range(n):
        L[i][i] = 1

    # Build the table. Note that the lower
    # diagonal values of table are
    # useless and not filled in the process.
    # The values are filled in a
    # manner similar to Matrix Chain
    # Multiplication DP solution (See
    # https://www.geeksforgeeks.org/dynamic-programming-set-8-matrix-chain-multiplication/
    # cl is length of substring
    for cl in range(2, n+1):
        for i in range(n-cl+1):
            j = i+cl-1
            if str[i] == str[j] and cl == 2:
                L[i][j] = 2
            elif str[i] == str[j]:
                L[i][j] = L[i+1][j-1] + 2
            else:
                L[i][j] = max(L[i][j-1], L[i+1][j])

    return L[0][n-1]


# Driver program to test above functions
seq = "GEEKS FOR GEEKS"
n = len(seq)
print("The length of the LPS is " + str(lps(seq)))
'''
Output: 
 

The length of the LPS is 7
Time Complexity of the above implementation is O(n^2) 
which is much better than the worst-case time complexity of Naive Recursive implementation.'''

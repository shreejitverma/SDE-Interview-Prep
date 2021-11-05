'''https://practice.geeksforgeeks.org/problems/longest-common-substring1452/1
https://www.geeksforgeeks.org/longest-common-substring-dp-29/
Longest Common Substring 
Medium Accuracy: 52.09% Submissions: 32970 Points: 4
Given two strings. The task is to find the length of the longest common substring.


Example 1:

Input: S1 = "ABCDGH", S2 = "ACDGHR"
Output: 4
Explanation: The longest common substring
is "CDGH" which has length 4.
Example 2:

Input: S1 = "ABC", S2 "ACB"
Output: 1
Explanation: The longest common substrings
are "A", "B", "C" all having length 1.

Your Task:
You don't need to read input or print anything. Your task is to complete the function longestCommonSubstr() which takes the string S1, string S2 and their length n and m as inputs and returns the length of the longest common substring in S1 and S2.


Expected Time Complexity: O(n*m).
Expected Auxiliary Space: O(n*m).


Constraints:
1<=n, m<=1000'''
# Python implementation of the above approach

# Function to find the length of the
# longest LCS
from operator import itemgetter
from functools import lru_cache


def LCSubStr(s, t, n, m):

    # Create DP table
    dp = [[0 for i in range(m + 1)] for j in range(2)]
    res = 0

    for i in range(1, n + 1):
        for j in range(1, m + 1):
            if(s[i - 1] == t[j - 1]):
                dp[i % 2][j] = dp[(i - 1) % 2][j - 1] + 1
                if(dp[i % 2][j] > res):
                    res = dp[i % 2][j]
            else:
                dp[i % 2][j] = 0
    return res


# Driver Code
X = "OldSite:GeeksforGeeks.org"
Y = "NewSite:GeeksQuiz.com"
m = len(X)
n = len(Y)

# Function call
print(LCSubStr(X, Y, m, n))
# Python code for the above approach


def longest_common_substring(x: str, y: str) -> (int, int, int):

    # function to find the longest common substring

    # Memorizing with maximum size of the memory as 1
    @lru_cache(maxsize=1)
    # function to find the longest common prefix
    def longest_common_prefix(i: int, j: int) -> int:

        if 0 <= i < len(x) and 0 <= j < len(y) and x[i] == y[j]:
            return 1 + longest_common_prefix(i + 1, j + 1)
        else:
            return 0

    # diagonally computing the subproplems
    # to decrease memory dependency
    def digonal_computation():

        # upper right triangle of the 2D array
        for k in range(len(x)):
            yield from ((longest_common_prefix(i, j), i, j)
                        for i, j in zip(range(k, -1, -1),
                                        range(len(y) - 1, -1, -1)))

        # lower left triangle of the 2D array
        for k in range(len(y)):
            yield from ((longest_common_prefix(i, j), i, j)
                        for i, j in zip(range(k, -1, -1),
                                        range(len(x) - 1, -1, -1)))

    # returning the maximum of all the subproblems
    return max(digonal_computation(), key=itemgetter(0), default=(0, 0, 0))


# Driver Code
if __name__ == '__main__':
    x: str = 'GeeksforGeeks'
    y: str = 'GeeksQuiz'
    length, i, j = longest_common_substring(x, y)
    print(f'length: {length}, i: {i}, j: {j}')
    print(f'x substring: {x[i: i + length]}')
    print(f'y substring: {y[j: j + length]}')


# Python3 program using to find length of
# the longest common substring recursion

# Returns length of function for longest
# common substring of X[0..m-1] and Y[0..n-1]


def lcs(i, j, count):

    if (i == 0 or j == 0):
        return count

    if (X[i - 1] == Y[j - 1]):
        count = lcs(i - 1, j - 1, count + 1)

    count = max(count, max(lcs(i, j - 1, 0),
                           lcs(i - 1, j, 0)))

    return count


# Driver code
if __name__ == "__main__":

    X = "abcdxyz"
    Y = "xyzabcd"

    n = len(X)
    m = len(Y)

    print(lcs(n, m, 0))

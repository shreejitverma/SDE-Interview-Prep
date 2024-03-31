'''https://practice.geeksforgeeks.org/problems/word-wrap1646/1
https://www.geeksforgeeks.org/word-wrap-problem-dp-19/
https://www.geeksforgeeks.org/word-wrap-problem-space-optimized-solution/
Word Wrap 
Medium Accuracy: 46.55% Submissions: 10397 Points: 4
Given an array nums[] of size n, where nums[i] denotes the number of characters in one word. Let K be the limit on the number of characters that can be put in one line (line width). Put line breaks in the given sequence such that the lines are printed neatly.
Assume that the length of each word is smaller than the line width. When line breaks are inserted there is a possibility that extra spaces are present in each line. The extra spaces include spaces put at the end of every line except the last one. 

You have to minimize the following total cost where total cost = Sum of cost of all lines, where cost of line is = (Number of extra spaces in the line)2.

Example 1:

Input: nums = {3,2,2,5}, k = 6
Output: 10
Explanation: Given a line can have 6
characters,
Line number 1: From word no. 1 to 1
Line number 2: From word no. 2 to 3
Line number 3: From word no. 4 to 4
So total cost = (6-3)2 + (6-2-2-1)2 = 32+12 = 10.
As in the first line word length = 3 thus
extra spaces = 6 - 3 = 3 and in the second line
there are two word of length 2 and there already
1 space between two word thus extra spaces
= 6 - 2 -2 -1 = 1. As mentioned in the problem
description there will be no extra spaces in
the last line. Placing first and second word
in first line and third word on second line
would take a cost of 02 + 42 = 16 (zero spaces
on first line and 6-2 = 4 spaces on second),
which isn't the minimum possible cost.
Example 2:

Input: nums = {3,2,2}, k = 4
Output: 5
Explanation: Given a line can have 4 
characters,
Line number 1: From word no. 1 to 1
Line number 2: From word no. 2 to 2
Line number 3: From word no. 3 to 3
Same explaination as above total cost
= (4 - 3)2 + (4 - 2)2 = 5.

Your Task:
You don't need to read or print anyhting. Your task is to complete the function solveWordWrap() which takes nums and k as input paramater and returns the minimized total cost.
 

Expected Time Complexity: O(n2)
Expected Space Complexity: O(n)
 

Constraints:
1 ≤ n ≤ 500
1 ≤ nums[i] ≤ 1000
max(nums[i]) ≤ k ≤ 1500'''


import sys


class Solution:
    def solveWordWrap(self, nums, k):
        # Code here
        n = len(nums)
    fin = [0]*n
    dp = [0]*n
    dp[n-1] = 0
    fin[n-1] = n-1
    d = 0

    for i in range(n-2, -1, -1):
        c = -1
        cos = 0
        dp[i] = float('inf')
        for j in range(i, n):
            c += nums[j]+1
            if c > k:
                break
            if j == n-1:
                l = 0
            else:
                l = (k-c)**2+dp[j+1]
            if dp[i] > l:
                dp[i] = l
                fin[i] = j

    return dp[0]

# A Dynamic programming solution
# for Word Wrap Problem


# A utility function to print
# the solution
# l[] represents lengths of different
# words in input sequence. For example,
# l[] = {3, 2, 2, 5} is for a sentence
# like "aaa bb cc ddddd". n is size of
# l[] and M is line width (maximum no.
# of characters that can fit in a line)
INF = 2147483647


def printSolution(p, n):
    k = 0
    if p[n] == 1:
        k = 1
    else:
        k = printSolution(p, p[n] - 1) + 1
    print('Line number ', k, ': From word no. ',
          p[n], 'to ', n)
    return k


def solveWordWrap(l, n, M):

    # For simplicity, 1 extra space is
    # used in all below arrays

    # extras[i][j] will have number
    # of extra spaces if words from i
    # to j are put in a single line
    extras = [[0 for i in range(n + 1)]
              for i in range(n + 1)]

    # lc[i][j] will have cost of a line
    # which has words from i to j
    lc = [[0 for i in range(n + 1)]
          for i in range(n + 1)]

    # c[i] will have total cost of
    # optimal arrangement of words
    # from 1 to i
    c = [0 for i in range(n + 1)]

    # p[] is used to print the solution.
    p = [0 for i in range(n + 1)]

    # calculate extra spaces in a single
    # line. The value extra[i][j] indicates
    # extra spaces if words from word number
    # i to j are placed in a single line
    for i in range(n + 1):
        extras[i][i] = M - l[i - 1]
        for j in range(i + 1, n + 1):
            extras[i][j] = (extras[i][j - 1] -
                            l[j - 1] - 1)

    # Calculate line cost corresponding
    # to the above calculated extra
    # spaces. The value lc[i][j] indicates
    # cost of putting words from word number
    # i to j in a single line
    for i in range(n + 1):
        for j in range(i, n + 1):
            if extras[i][j] < 0:
                lc[i][j] = INF
            elif j == n and extras[i][j] >= 0:
                lc[i][j] = 0
            else:
                lc[i][j] = (extras[i][j] *
                            extras[i][j])

    # Calculate minimum cost and find
    # minimum cost arrangement. The value
    # c[j] indicates optimized cost to
    # arrange words from word number 1 to j.
    c[0] = 0
    for j in range(1, n + 1):
        c[j] = INF
        for i in range(1, j + 1):
            if (c[i - 1] != INF and
                lc[i][j] != INF and
                    ((c[i - 1] + lc[i][j]) < c[j])):
                c[j] = c[i-1] + lc[i][j]
                p[j] = i
    printSolution(p, n)


# Driver Code
l = [3, 2, 2, 5]
n = len(l)
M = 6
solveWordWrap(l, n, M)


# Python 3 program for space optimized
# solution of Word Wrap problem.

# Function to find space optimized
# solution of Word Wrap problem.

def solveWordWrap(arr, n, k):

    dp = [0] * n

    # Array in which ans[i] store index
    # of last word in line starting with
    # word arr[i].
    ans = [0] * n

    # If only one word is present then
    # only one line is required. Cost
    # of last line is zero. Hence cost
    # of this line is zero. Ending point
    # is also n-1 as single word is
    # present.
    dp[n - 1] = 0
    ans[n - 1] = n - 1

    # Make each word first word of line
    # by iterating over each index in arr.
    for i in range(n - 2, -1, -1):
        currlen = -1
        dp[i] = sys.maxsize

        # Keep on adding words in current
        # line by iterating from starting
        # word upto last word in arr.
        for j in range(i, n):

            # Update number of characters
            # in current line. arr[j] is
            # number of characters in
            # current word and 1
            # represents space character
            # between two words.
            currlen += (arr[j] + 1)

            # If limit of characters
            # is violated then no more
            # words can be added to
            # current line.
            if (currlen > k):
                break

            # If current word that is
            # added to line is last
            # word of arr then current
            # line is last line. Cost of
            # last line is 0. Else cost
            # is square of extra spaces
            # plus cost of putting line
            # breaks in rest of words
            # from j+1 to n-1.
            if (j == n - 1):
                cost = 0
            else:
                cost = ((k - currlen) *
                        (k - currlen) + dp[j + 1])

            # Check if this arrangement gives
            # minimum cost for line starting
            # with word arr[i].
            if (cost < dp[i]):
                dp[i] = cost
                ans[i] = j

    # Print starting index and ending index
    # of words present in each line.
    i = 0
    while (i < n):
        print(i + 1, ans[i] + 1, end=" ")
        i = ans[i] + 1


# Driver Code
if __name__ == "__main__":

    arr = [3, 2, 2, 5]
    n = len(arr)
    M = 6
    solveWordWrap(arr, n, M)

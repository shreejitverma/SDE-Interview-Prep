'''https://practice.geeksforgeeks.org/problems/cutted-segments1642/1
https://www.geeksforgeeks.org/maximum-number-segments-lengths-b-c/
Maximize The Cut Segments 
Medium Accuracy: 38.76% Submissions: 66258 Points: 4
Given an integer N denoting the Length of a line segment. 
You need to cut the line segment in such a way that the cut length of a line segment each time is either x , y or z.
 Here x, y, and z are integers.
After performing all the cut operations, your total number of cut segments must be maximum.

Example 1:

Input:
N = 4
x = 2, y = 1, z = 1
Output: 4
Explanation:Total length is 4, and the cut
lengths are 2, 1 and 1.  We can make
maximum 4 segments each of length 1.
Example 2:

Input:
N = 5
x = 5, y = 3, z = 2
Output: 2
Explanation: Here total length is 5, and
the cut lengths are 5, 3 and 2. We can
make two segments of lengths 3 and 2.
Your Task:
You only need to complete the function maximizeTheCuts() that takes n, x, y, z as parameters 
and returns max number cuts.

Expected Time Complexity : O(N)
Expected Auxiliary Space: O(N)

Constraints
1 <= N, x, y, z <= 104

Approach : The approach used is Dynamic Programming. 
The base dp0 will be 0 as initially it has no segments. 
After that, iterate from 1 to n, and for each of the 3 states i.e, dpi+a, dpi+b and dpi+c, 
store the maximum value obtained by either using or not using the a, b or c segment. 
The 3 states to deal with are : 
 



dpi+a=max(dpi+1, dpi+a); 
dpi+b=max(dpi+1, dpi+b); 
dpi+c=max(dpi+1, dpi+c);

Below is the implementation of above idea : '''

# Python implementation
# to divide N into maximum
# number of segments of
# length a, b and c

# function to find
# the maximum number
# of segments


def maximumSegments(n, a, b, c):

    # stores the maximum
    # number of segments
    # each index can have
    dp = [-1] * (n + 10)

    # 0th index will have
    # 0 segments base case
    dp[0] = 0

    # traverse for all possible
    # segments till n
    for i in range(0, n):

        if (dp[i] != -1):

            # conditions
            if(i + a <= n):  # avoid buffer overflow
                dp[i + a] = max(dp[i] + 1,
                                dp[i + a])

            if(i + b <= n):  # avoid buffer overflow
                dp[i + b] = max(dp[i] + 1,
                                dp[i + b])

            if(i + c <= n):  # avoid buffer overflow
                dp[i + c] = max(dp[i] + 1,
                                dp[i + c])
    if(dp[n] < 0):
        return 0
    return dp[n]


# Driver code
n = 7
a = 5
b = 2
c = 5
print(maximumSegments(n, a,
                      b, c))

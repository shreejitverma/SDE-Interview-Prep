'''https://practice.geeksforgeeks.org/problems/max-rectangle/1
Max rectangle 
Medium Accuracy: 47.59% Submissions: 39571 Points: 4
Given a binary matrix M of size n X m. Find the maximum area of a rectangle formed only of 1s in the given matrix.

Example 1:

Input:
n = 4, m = 4
M[][] = {{0 1 1 0},
         {1 1 1 1},
         {1 1 1 1},
         {1 1 0 0}}
Output: 8
Explanation: For the above test case the
matrix will look like
0 1 1 0
1 1 1 1
1 1 1 1
1 1 0 0
the max size rectangle is 
1 1 1 1
1 1 1 1
and area is 4 *2 = 8.
Your Task: 
Your task is to complete the function maxArea which returns the maximum size rectangle area in a binary-sub-matrix with all 1’s. The function takes 3 arguments the first argument is the Matrix M[ ] [ ] and the next two are two integers n and m which denotes the size of the matrix M. 

Expected Time Complexity : O(n*m)
Expected Auixiliary Space : O(m)

Constraints:
1<=n,m<=1000
0<=M[][]<=1

Note:The Input/Ouput format and Example given are used for system's internal purpose, and should be used by a user for Expected Output only. As it is a function problem, hence a user should not read any input from stdin/console. The task is to complete the function specified, and not to write the full code.'''


class Solution:
    def hist(self, h):
        # stack s
        s = []
        i = 0
        maxarea = 0
        while i <= len(h):
            if i < len(h) and (len(s) == 0 or h[i] >= h[s[-1]]):
                s.append(i)
                i += 1
            else:
                if len(s) > 0:
                    height = h[s.pop()]
                    if len(s) > 0:
                        length = i-1-s[-1]
                    else:
                        length = i
                    area = height * length
                    maxarea = max(maxarea, area)
                else:
                    break
        return maxarea

    def maxArea(self, M, n, m):

        h = [0] * m
        maxarea = 0
        for r in range(n):
            # update height vector
            for c in range(m):
                if M[r][c] == 1:
                    h[c] += 1
                else:
                    h[c] = 0
            maxarea = max(maxarea, self.hist(h))
        return maxarea


# {
#  Driver Code Starts
# Initial Template for Python 3

# Driver Code
if __name__ == '__main__':
    t = int(input())
    for _ in range(t):
        R, C = map(int, input().strip().split())
        A = []
        for i in range(R):
            line = [int(x) for x in input().strip().split()]
            A.append(line)
        print(Solution().maxArea(A, R, C))

# This code is contributed
# by SHUBHAMSINGH10

# } Driver Code Ends

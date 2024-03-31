'''https://practice.geeksforgeeks.org/problems/rat-in-a-maze-problem/1
Rat in a Maze Problem - I 
Medium Accuracy: 37.73% Submissions: 88946 Points: 4
Consider a rat placed at (0, 0) in a square matrix of order N * N. It has to reach the destination at (N - 1, N - 1). Find all possible paths that the rat can take to reach from source to destination. The directions in which the rat can move are 'U'(up), 'D'(down), 'L' (left), 'R' (right). Value 0 at a cell in the matrix represents that it is blocked and rat cannot move to it while value 1 at a cell in the matrix represents that rat can be travel through it.
Note: In a path, no cell can be visited more than one time.

Example 1:

Input:
N = 4
m[][] = {{1, 0, 0, 0},
         {1, 1, 0, 1}, 
         {1, 1, 0, 0},
         {0, 1, 1, 1}}
Output:
DDRDRR DRDDRR
Explanation:
The rat can reach the destination at 
(3, 3) from (0, 0) by two paths - DRDDRR 
and DDRDRR, when printed in sorted order 
we get DDRDRR DRDDRR.
Example 2:
Input:
N = 2
m[][] = {{1, 0},
         {1, 0}}
Output:
-1
Explanation:
No path exists and destination cell is 
blocked.
Your Task:  
You don't need to read input or print anything. Complete the function printPath() which takes N and 2D array m[ ][ ] as input parameters and returns the list of paths in lexicographically increasing order. 
Note: In case of no path, return an empty list. The driver will output "-1" automatically.

Expected Time Complexity: O((3N^2)).
Expected Auxiliary Space: O(L * X), L = length of the path, X = number of paths.

Constraints:
2 ≤ N ≤ 5
0 ≤ m[i][j] ≤ 1'''


class Solution:
    def findPath(self, m, n):
        # code here
        def solve(row, col, matrix, n, i_index, j_index, check, ans, path):
            if row == n-1 and col == n-1:
                ans.append(path)
                return

            step = "DLRU"

            for index in range(0, 4):
                nr = row + i_index[index]   # nr = new_row
                nc = col + j_index[index]   # nc = new_col

                if nr < n and nc < n and nr >= 0 and nc >= 0 and check[nr][nc] == 0 and matrix[nr][nc] == 1:
                    check[row][col] = 1
                    solve(nr, nc, matrix, n, i_index, j_index,
                          check, ans, path+step[index])
                    check[row][col] = 0

        ans = []
        i_index = [1, 0, 0, -1]
        j_index = [0, -1, 1, 0]
        check = [[0]*n for i in range(n)]
        if m[0][0] == 1:
            solve(0, 0, m, n, i_index, j_index, check, ans, "")
        return ans
# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        n = list(map(int, input().strip().split()))
        arr = list(map(int, input().strip().split()))

        matrix = [[0 for i in range(n[0])]for j in range(n[0])]
        k = 0
        for i in range(n[0]):
            for j in range(n[0]):
                matrix[i][j] = arr[k]
                k += 1
        ob = Solution()
        result = ob.findPath(matrix, n[0])
        if len(result) == 0:
            print(-1)
        else:
            for x in result:
                print(x, end=" ")
            print()
# } Driver Code Ends

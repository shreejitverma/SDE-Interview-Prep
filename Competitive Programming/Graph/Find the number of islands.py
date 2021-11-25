'''https://practice.geeksforgeeks.org/problems/find-the-number-of-islands/1

Find the number of islands 
Medium Accuracy: 38.66% Submissions: 90417 Points: 4
Given a grid consisting of '0's(Water) and '1's(Land). Find the number of islands.
Note: An island is surrounded by water and is formed by connecting adjacent lands horizontally or vertically or diagonally i.e., in all 8 directions.
 

Example 1:

Input:
grid = {{0,1},{1,0},{1,1},{1,0}}
Output:
1
Explanation:
The grid is-
0 1
1 0
1 1
1 0
All lands are connected.
Example 2:

Input:
grid = {{0,1,1,1,0,0,0},{0,0,1,1,0,1,0}}
Output:
2
Expanation:
The grid is-
0 1 1 1 0 0 0
0 0 1 1 0 1 0 
There are two islands one is colored in blue 
and other in orange.
 

Your Task:
You don't need to read or print anything. Your task is to complete the function numIslands() which takes grid as input parameter and returns the total number of islands.
 

Expected Time Compelxity: O(n*m)
Expected Space Compelxity: O(n*m)
 

Constraints:
1 ≤ n, m ≤ 500'''

# User function Template for python3


class Solution:
    def numIslands(self, grid):
        visited = [[False for i in grid[0]] for j in grid]
        count = 0

        for i in range(len(grid)):
            for j in range(len(grid[i])):

                if visited[i][j] or int(grid[i][j]) == 0:
                    continue
                self.visitConnectedLands(grid, i, j, visited)
                count = count+1
        return count

    def visitConnectedLands(self, grid, i, j, visited):
        if i < 0 or j < 0 or i >= len(grid) or j >= len(grid[i]) or int(grid[i][j]) == 0 or visited[i][j]:
            return

        visited[i][j] = True
        self.visitConnectedLands(grid, i-1, j, visited)
        self.visitConnectedLands(grid, i-1, j+1, visited)
        self.visitConnectedLands(grid, i, j+1, visited)
        self.visitConnectedLands(grid, i+1, j+1, visited)
        self.visitConnectedLands(grid, i+1, j, visited)
        self.visitConnectedLands(grid, i+1, j-1, visited)
        self.visitConnectedLands(grid, i, j-1, visited)
        self.visitConnectedLands(grid, i-1, j-1, visited)

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == "__main__":
    for _ in range(int(input())):
        n, m = map(int, input().strip().split())
        grid = []
        for i in range(n):
            grid.append([int(i) for i in input().strip().split()])
        obj = Solution()
        print(obj.numIslands(grid))
# } Driver Code Ends

# Another One
# User function Template for python3


class Solution:
    def numIslands(self, grid):
        # code here
        import sys
        sys.setrecursionlimit(1000000)

        def sol(i, j, r, c, g):
            if g[i][j] == 0:
                return
            g[i][j] = 0
            if i+1 < r and g[i+1][j] == 1:
                sol(i+1, j, r, c, g)
            if j+1 < c and g[i][j+1] == 1:
                sol(i, j+1, r, c, g)
            if i-1 > -1 and g[i-1][j] == 1:
                sol(i-1, j, r, c, g)
            if j-1 > -1 and g[i][j-1] == 1:
                sol(i, j-1, r, c, g)
            if i+1 < r and j+1 < c and g[i+1][j+1] == 1:
                sol(i+1, j+1, r, c, g)
            if i+1 < r and j-1 > -1 and g[i+1][j-1] == 1:
                sol(i+1, j-1, r, c, g)
            if i-1 > -1 and j+1 < c and g[i-1][j+1] == 1:
                sol(i-1, j+1, r, c, g)
            if i-1 > -1 and j-1 > -1 and g[i-1][j-1] == 1:
                sol(i-1, j-1, r, c, g)
        r = len(grid)
        c = len(grid[0])
        ans = 0
        for i in range(r):
            for j in range(c):
                if grid[i][j] == 1:
                    sol(i, j, r, c, grid)
                    ans += 1
        return ans

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == "__main__":
    for _ in range(int(input())):
        n, m = map(int, input().strip().split())
        grid = []
        for i in range(n):
            grid.append([int(i) for i in input().strip().split()])
        obj = Solution()
        print(obj.numIslands(grid))
# } Driver Code Ends

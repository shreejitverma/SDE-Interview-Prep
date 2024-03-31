'''https://practice.geeksforgeeks.org/problems/solve-the-sudoku-1587115621/1
Solve the Sudoku 
Hard Accuracy: 48.74% Submissions: 21092 Points: 8
Given an incomplete Sudoku configuration in terms of a 9 x 9  2-D square matrix (grid[][]), the task to find a solved Sudoku. For simplicity, you may assume that there will be only one unique solution.

Sample Sudoku for you to get the logic for its solution:




Example 1:

Input:
grid[][] = 
[[3 0 6 5 0 8 4 0 0],
[5 2 0 0 0 0 0 0 0],
[0 8 7 0 0 0 0 3 1 ],
[0 0 3 0 1 0 0 8 0],
[9 0 0 8 6 3 0 0 5],
[0 5 0 0 9 0 6 0 0],
[1 3 0 0 0 0 2 5 0],
[0 0 0 0 0 0 0 7 4],
[0 0 5 2 0 6 3 0 0]]
Output:
3 1 6 5 7 8 4 9 2
5 2 9 1 3 4 7 6 8
4 8 7 6 2 9 5 3 1
2 6 3 4 1 5 9 8 7
9 7 4 8 6 3 1 2 5
8 5 1 7 9 2 6 4 3
1 3 8 9 4 7 2 5 6
6 9 2 3 5 1 8 7 4
7 4 5 2 8 6 3 1 9

Your Task:
You need to complete the two functions:
SolveSudoku(): Takes a grid as its argument and returns true if a solution is possible and false if it is not.
printGrid(): Takes a grid as its argument and prints the 81 numbers of the solved Sudoku in a single line with space separation.


Expected Time Complexity: O(9N*N).
Expected Auxiliary Space: O(N*N).


Constraints:
0 ≤ grid[i][j] ≤ 9'''


class Solution:

    # Function to find a solved Sudoku.
    def SolveSudoku(self, grid):
        def safe(x, y, k):
            for i in range(9):
                if grid[x][i] == k or grid[i][y] == k:
                    return False
            a = int(x//3)
            b = int(y//3)
            for i in range(3*a, 3*a+3):
                for j in range(3*b, 3*b+3):
                    if grid[i][j] == k:
                        return False
            return True

        def find_next(a, b):
            i, j = a, b
            if b == 8:
                i += 1
                j = 0
            else:
                j += 1
            while i < 9 and j < 9:
                if grid[i][j] == 0:
                    return i, j
                if j == 8:
                    i += 1
                    j = 0
                else:
                    j += 1
            return None, None

        def sudoku(x, y, k):
            grid[x][y] = k
            if x == 8 and y == 8:
                return True
            a, b = find_next(x, y)
            if a == None and b == None:
                return True
            for i in range(1, 10):
                if safe(a, b, i):
                    if sudoku(a, b, i):
                        return True
            grid[x][y] = 0
        x, y = find_next(0, -1)
        for i in range(1, 10):
            if safe(x, y, i) and sudoku(x, y, i):
                return True
        return False

    # Function to print grids of the Sudoku.

    def printGrid(self, arr):
        for i in arr:
            for j in i:
                print(j, end=" ")
        # Your code here

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == "__main__":
    t = int(input())
    while(t > 0):
        grid = [[0 for i in range(9)] for j in range(9)]
        row = [int(x) for x in input().strip().split()]
        k = 0
        for i in range(9):
            for j in range(9):
                grid[i][j] = row[k]
                k += 1

        ob = Solution()

        if(ob.SolveSudoku(grid) == True):
            ob.printGrid(grid)
            print()
        else:
            print("No solution exists")
        t = t-1
# } Driver Code Ends

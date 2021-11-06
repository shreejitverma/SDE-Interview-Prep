'''https://practice.geeksforgeeks.org/problems/rat-in-a-maze-problem/1
https://www.geeksforgeeks.org/rat-in-a-maze-problem-when-movement-in-all-possible-directions-is-allowed/
Rat in a Maze Problem - I 
Medium Accuracy: 37.73% Submissions: 88220 Points: 4
Consider a rat placed at (0, 0) in a square matrix of order N * N. 
It has to reach the destination at (N - 1, N - 1). 
Find all possible paths that the rat can take to reach from source to destination. 
The directions in which the rat can move are 'U'(up), 'D'(down), 'L' (left), 'R' (right). 
Value 0 at a cell in the matrix represents that it is blocked and rat cannot move to it 
while value 1 at a cell in the matrix represents that rat can be travel through it.
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
You don't need to read input or print anything. Complete the function printPath() which takes N and 2D array 
m[ ][ ] as input parameters and returns the list of paths in lexicographically increasing order. 
Note: In case of no path, return an empty list. The driver will output "-1" automatically.

Expected Time Complexity: O((3N^2)).
Expected Auxiliary Space: O(L * X), L = length of the path, X = number of paths.

Constraints:
2 ≤ N ≤ 5
0 ≤ m[i][j] ≤ 1

Solution: 
Approach: 

Start from the initial index (i.e. (0,0)) and look for the valid moves through the adjacent cells 
in the order Down->Left->Right->Up (so as to get the sorted paths) in the grid.
If the move is possible, then move to that cell while storing the character corresponding 
to the move(D,L,R,U) and again start looking for the valid move until the last index (i.e. (n-1,n-1)) is reached.
Also, keep on marking the cells as visited and when we traversed all the paths possible from that cell, 
then unmark that cell for other different paths and remove the character from the path formed.
As the last index of the grid(bottom right) is reached, then store the traversed path.
Below is the implementation of the above approach:  '''

from typing import List


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

# Python3 implementation of the above approach


MAX = 5


# Function returns true if the
# move taken is valid else
# it will return false.


def isSafe(row: int, col: int,
           m: List[List[int]], n: int,
           visited: List[List[bool]]) -> bool:

    if (row == -1 or row == n or
        col == -1 or col == n or
            visited[row][col] or m[row][col] == 0):
        return False

    return True

# Function to print all the possible
# paths from (0, 0) to (n-1, n-1).


def printPathUtil(row: int, col: int,
                  m: List[List[int]],
                  n: int, path: str,
                  possiblePaths: List[str],
                  visited: List[List[bool]]) -> None:

    # This will check the initial point
    # (i.e. (0, 0)) to start the paths.
    if (row == -1 or row == n or
        col == -1 or col == n or
            visited[row][col] or m[row][col] == 0):
        return

    # If reach the last cell (n-1, n-1)
    # then store the path and return
    if (row == n - 1 and col == n - 1):
        possiblePaths.append(path)
        return

    # Mark the cell as visited
    visited[row][col] = True

    # Try for all the 4 directions (down, left,
    # right, up) in the given order to get the
    # paths in lexicographical order

    # Check if downward move is valid
    if (isSafe(row + 1, col, m, n, visited)):
        path += 'D'
        printPathUtil(row + 1, col, m, n,
                      path, possiblePaths, visited)
        path = path[:-1]

    # Check if the left move is valid
    if (isSafe(row, col - 1, m, n, visited)):
        path += 'L'
        printPathUtil(row, col - 1, m, n,
                      path, possiblePaths, visited)
        path = path[:-1]

    # Check if the right move is valid
    if (isSafe(row, col + 1, m, n, visited)):
        path += 'R'
        printPathUtil(row, col + 1, m, n,
                      path, possiblePaths, visited)
        path = path[:-1]

    # Check if the upper move is valid
    if (isSafe(row - 1, col, m, n, visited)):
        path += 'U'
        printPathUtil(row - 1, col, m, n,
                      path, possiblePaths, visited)
        path = path[:-1]

    # Mark the cell as unvisited for
    # other possible paths
    visited[row][col] = False

# Function to store and print
# all the valid paths


def printPath(m: List[List[int]], n: int) -> None:

    # vector to store all the possible paths
    possiblePaths = []
    path = ""
    visited = [[False for _ in range(MAX)]
               for _ in range(n)]

    # Call the utility function to
    # find the valid paths
    printPathUtil(0, 0, m, n, path,
                  possiblePaths, visited)

    # Print all possible paths
    for i in range(len(possiblePaths)):
        print(possiblePaths[i], end=" ")


# Driver code
if __name__ == "__main__":

    m = [[1, 0, 0, 0, 0],
         [1, 1, 1, 1, 1],
         [1, 1, 1, 0, 1],
         [0, 0, 0, 0, 1],
         [0, 0, 0, 0, 1]]
    n = len(m)

    printPath(m, n)

'''Backtracking Algorithm: Backtracking is an algorithmic-technique 
for solving problems recursively by trying to build a solution incrementally. 
Solving one piece at a time, and removing those solutions that fail to satisfy the constraints of the problem at 
any point of time (by time, here, is referred to the time elapsed till reaching any level of the search tree) is 
the process of backtracking.

Approach: Form a recursive function, which will follow a path and check if the path reaches the destination or not. 
If the path does not reach the destination then backtrack and try other paths. 

Algorithm:  

Create a solution matrix, initially filled with 0’s.
Create a recursive function, which takes initial matrix, output matrix and position of rat (i, j).
if the position is out of the matrix or the position is not valid then return.
Mark the position output[i][j] as 1 and check if the current position is destination or not. 
If destination is reached print the output matrix and return.
Recursively call for position (i+1, j) and (i, j+1).
Unmark position (i, j), i.e output[i][j] = 0.'''

# Python3 program to solve Rat in a Maze
# problem using backracking

# Maze size
N = 4

# A utility function to print solution matrix sol


def printSolution(sol):

    for i in sol:
        for j in i:
            print(str(j) + " ", end="")
        print("")

# A utility function to check if x, y is valid
# index for N * N Maze


def isSafe(maze, x, y):

    if x >= 0 and x < N and y >= 0 and y < N and maze[x][y] == 1:
        return True

    return False


""" This function solves the Maze problem using Backtracking.
    It mainly uses solveMazeUtil() to solve the problem. It
    returns false if no path is possible, otherwise return
    true and prints the path in the form of 1s. Please note
    that there may be more than one solutions, this function
    prints one of the feasable solutions. """


def solveMaze(maze):

    # Creating a 4 * 4 2-D list
    sol = [[0 for j in range(4)] for i in range(4)]

    if solveMazeUtil(maze, 0, 0, sol) == False:
        print("Solution doesn't exist")
        return False

    printSolution(sol)
    return True

# A recursive utility function to solve Maze problem


def solveMazeUtil(maze, x, y, sol):

    # if (x, y is goal) return True
    if x == N - 1 and y == N - 1 and maze[x][y] == 1:
        sol[x][y] = 1
        return True

    # Check if maze[x][y] is valid
    if isSafe(maze, x, y) == True:
        # Check if the current block is already part of solution path.
        if sol[x][y] == 1:
            return False

        # mark x, y as part of solution path
        sol[x][y] = 1

        # Move forward in x direction
        if solveMazeUtil(maze, x + 1, y, sol) == True:
            return True

        # If moving in x direction doesn't give solution
        # then Move down in y direction
        if solveMazeUtil(maze, x, y + 1, sol) == True:
            return True

        # If moving in y direction doesn't give solution then
        # Move back in x direction
        if solveMazeUtil(maze, x - 1, y, sol) == True:
            return True

        # If moving in backwards in x direction doesn't give solution
        # then Move upwards in y direction
        if solveMazeUtil(maze, x, y - 1, sol) == True:
            return True

        # If none of the above movements work then
        # BACKTRACK: unmark x, y as part of solution path
        sol[x][y] = 0
        return False


# Driver program to test above function
if __name__ == "__main__":
    # Initialising the maze
    maze = [[1, 0, 0, 0],
            [1, 1, 0, 1],
            [0, 1, 0, 0],
            [1, 1, 1, 1]]

    solveMaze(maze)

/*
https://practice.geeksforgeeks.org/problems/rat-in-a-maze-problem/1
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
Below is the implementation of the above approach:  
*/
#define pb push_back
#define long long ll
class Solution
{
public:
    void ratmaze(vector<vector<int> > &m, int n, vector<string> &ans, int x, int y, vector<vector<int> > &visited, string op)
    {
        //base case:
        //1) if i go out of bounds
        if (x >= n || y >= n || x < 0 || y < 0)
        {
            return;
        }
        //2) the cell is already visited
        if (visited[x][y] == 1)
        {
            return;
        }
        //3) If the path is being blocked
        if (m[x][y] == 0)
        {
            return;
        }
        if ((x == n - 1) && (y == n - 1))
        {
            ans.pb(op);
            return;
        }

        //self work:
        visited[x][y] = 1;
        ratmaze(m, n, ans, x + 1, y, visited, op + 'D'); //down
        ratmaze(m, n, ans, x, y - 1, visited, op + 'L'); //left
        ratmaze(m, n, ans, x, y + 1, visited, op + 'R'); //right
        ratmaze(m, n, ans, x - 1, y, visited, op + 'U'); //up
        visited[x][y] = 0;
    }
    vector<string> findPath(vector<vector<int> > &m, int n)
    {
        // Your code goes here
        vector<string> ans;
        if (m[0][0] == 0 || m[n - 1][n - 1] == 0)
        {
            return ans;
        }
        int x = 0, y = 0;
        string op = "";
        vector<vector<int> > visited(n, vector<int>(n, 0));
        ratmaze(m, n, ans, x, y, visited, op);
        return ans;
    }
};
// C++ implementation of the above approach
#include <bits/stdc++.h>
#define MAX 5
using namespace std;

// Function returns true if the
// move taken is valid else
// it will return false.
bool isSafe(int row, int col, int m[][MAX],
            int n, bool visited[][MAX])
{
    if (row == -1 || row == n || col == -1 ||
        col == n || visited[row][col] || m[row][col] == 0)
        return false;

    return true;
}

// Function to print all the possible
// paths from (0, 0) to (n-1, n-1).
void printPathUtil(int row, int col, int m[][MAX],
                   int n, string &path, vector<string> &possiblePaths, bool visited[][MAX])
{
    // This will check the initial point
    // (i.e. (0, 0)) to start the paths.
    if (row == -1 || row == n || col == -1 || col == n || visited[row][col] || m[row][col] == 0)
        return;

    // If reach the last cell (n-1, n-1)
    // then store the path and return
    if (row == n - 1 && col == n - 1)
    {
        possiblePaths.push_back(path);
        return;
    }

    // Mark the cell as visited
    visited[row][col] = true;

    // Try for all the 4 directions (down, left,
    // right, up) in the given order to get the
    // paths in lexicographical order

    // Check if downward move is valid
    if (isSafe(row + 1, col, m, n, visited))
    {
        path.push_back('D');
        printPathUtil(row + 1, col, m, n,
                      path, possiblePaths, visited);
        path.pop_back();
    }

    // Check if the left move is valid
    if (isSafe(row, col - 1, m, n, visited))
    {
        path.push_back('L');
        printPathUtil(row, col - 1, m, n,
                      path, possiblePaths, visited);
        path.pop_back();
    }

    // Check if the right move is valid
    if (isSafe(row, col + 1, m, n, visited))
    {
        path.push_back('R');
        printPathUtil(row, col + 1, m, n,
                      path, possiblePaths, visited);
        path.pop_back();
    }

    // Check if the upper move is valid
    if (isSafe(row - 1, col, m, n, visited))
    {
        path.push_back('U');
        printPathUtil(row - 1, col, m, n,
                      path, possiblePaths, visited);
        path.pop_back();
    }

    // Mark the cell as unvisited for
    // other possible paths
    visited[row][col] = false;
}

// Function to store and print
// all the valid paths
void printPath(int m[MAX][MAX], int n)
{
    // vector to store all the possible paths
    vector<string> possiblePaths;
    string path;
    bool visited[n][MAX];
    memset(visited, false, sizeof(visited));

    // Call the utility function to
    // find the valid paths
    printPathUtil(0, 0, m, n, path,
                  possiblePaths, visited);

    // Print all possible paths
    for (int i = 0; i < possiblePaths.size(); i++)
        cout << possiblePaths[i] << " ";
}

// Driver code
int main()
{
    int m[MAX][MAX] = {{1, 0, 0, 0, 0},
                       {1, 1, 1, 1, 1},
                       {1, 1, 1, 0, 1},
                       {0, 0, 0, 0, 1},
                       {0, 0, 0, 0, 1}};
    int n = sizeof(m) / sizeof(m[0]);
    printPath(m, n);

    return 0;
}

/*
Backtracking Algorithm: Backtracking is an algorithmic-technique 
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
Unmark position (i, j), i.e output[i][j] = 0.
*/

/* C++ program to solve Rat in
a Maze problem using backtracking */
#include <stdio.h>

// Maze size
#define N 4

bool solveMazeUtil(
    int maze[N][N], int x,
    int y, int sol[N][N]);

/* A utility function to print
solution matrix sol[N][N] */
void printSolution(int sol[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf(" %d ", sol[i][j]);
        printf("\n");
    }
}

/* A utility function to check if x,
y is valid index for N*N maze */
bool isSafe(int maze[N][N], int x, int y)
{
    // if (x, y outside maze) return false
    if (
        x >= 0 && x < N && y >= 0 && y < N && maze[x][y] == 1)
        return true;

    return false;
}

/* This function solves the Maze problem
using Backtracking. It mainly uses
solveMazeUtil() to solve the problem.
It returns false if no path is possible,
otherwise return true and prints the path
in the form of 1s. Please note that there
may be more than one solutions, this
function prints one of the feasible
solutions.*/
bool solveMaze(int maze[N][N])
{
    int sol[N][N] = {{0, 0, 0, 0},
                     {0, 0, 0, 0},
                     {0, 0, 0, 0},
                     {0, 0, 0, 0}};

    if (solveMazeUtil(
            maze, 0, 0, sol) == false)
    {
        printf("Solution doesn't exist");
        return false;
    }

    printSolution(sol);
    return true;
}

/* A recursive utility function
to solve Maze problem */
bool solveMazeUtil(
    int maze[N][N], int x,
    int y, int sol[N][N])
{
    // if (x, y is goal) return true
    if (
        x == N - 1 && y == N - 1 && maze[x][y] == 1)
    {
        sol[x][y] = 1;
        return true;
    }

    // Check if maze[x][y] is valid
    if (isSafe(maze, x, y) == true)
    {
        // Check if the current block is already part of solution path.
        if (sol[x][y] == 1)
            return false;

        // mark x, y as part of solution path
        sol[x][y] = 1;

        /* Move forward in x direction */
        if (solveMazeUtil(
                maze, x + 1, y, sol) == true)
            return true;

        /* If moving in x direction
        doesn't give solution then
        Move down in y direction */
        if (solveMazeUtil(
                maze, x, y + 1, sol) == true)
            return true;

        /* If none of the above movements
        work then BACKTRACK: unmark
        x, y as part of solution path */
        sol[x][y] = 0;
        return false;
    }

    return false;
}

// driver program to test above function
int main()
{
    int maze[N][N] = {{1, 0, 0, 0},
                      {1, 1, 0, 1},
                      {0, 1, 0, 0},
                      {1, 1, 1, 1}};

    solveMaze(maze);
    return 0;
}
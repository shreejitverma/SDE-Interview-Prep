/*
https://practice.geeksforgeeks.org/problems/rat-in-a-maze-problem/1
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
0 ≤ m[i][j] ≤ 1
*/

class Solution
{
public:
    void find(vector<vector<int> > &m, string s, int n, int i, int j, vector<string> &ans)
    {
        if (i == n - 1 && j == n - 1)
        {
            ans.push_back(s);
            return;
        }
        if (i >= 0 && j >= 0 && i < n && j < n && m[i][j])
        {
            m[i][j] = 0;
            find(m, s + "D", n, i + 1, j, ans);
            find(m, s + "L", n, i, j - 1, ans);
            find(m, s + "R", n, i, j + 1, ans);
            find(m, s + "U", n, i - 1, j, ans);
            m[i][j] = 1;
        }
        return;
    }
    vector<string> findPath(vector<vector<int> > &m, int n)
    {
        vector<string> ans;
        if (m[0][0] == 0 || m[n - 1][n - 1] == 0)
            return ans;

        string s;
        find(m, s, n, 0, 0, ans);
        return ans;
    }
};
// Initial template for C++

#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
// User function template for C++
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

// { Driver Code Starts.

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int n;
        cin >> n;
        vector<vector<int> > m(n, vector<int>(n, 0));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cin >> m[i][j];
            }
        }
        Solution obj;
        vector<string> result = obj.findPath(m, n);
        if (result.size() == 0)
            cout << -1;
        else
            for (int i = 0; i < result.size(); i++)
                cout << result[i] << " ";
        cout << endl;
    }
    return 0;
} // } Driver Code Ends
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
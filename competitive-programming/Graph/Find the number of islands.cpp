/*
https://practice.geeksforgeeks.org/problems/find-the-number-of-islands/1

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
1 ≤ n, m ≤ 500
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    // Function to find the number of islands.
    void mark(int rows, int cols, int i, int j, vector<vector<char> > &grid)
    {
        if (i < 0 || i >= rows || j < 0 || j >= cols || grid[i][j] != '1')
        {
            return;
        }

        grid[i][j] = '2';
        mark(rows, cols, i + 1, j, grid);
        mark(rows, cols, i - 1, j, grid);
        mark(rows, cols, i, j + 1, grid);
        mark(rows, cols, i, j - 1, grid);
        mark(rows, cols, i - 1, j - 1, grid);
        mark(rows, cols, i + 1, j + 1, grid);
        mark(rows, cols, i + 1, j - 1, grid);
        mark(rows, cols, i - 1, j + 1, grid);
    }

    int numIslands(vector<vector<char> > &grid)
    {
        // Code here
        int rows = grid.size();
        int cols = grid[0].size();
        if (rows == 0)
        {
            return 0;
        }
        int is = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if (grid[i][j] == '1')
                {
                    mark(rows, cols, i, j, grid);
                    is++;
                }
            }
        }
        return is;
    }
};

// { Driver Code Starts.
int main()
{
    int tc;
    cin >> tc;
    while (tc--)
    {
        int n, m;
        cin >> n >> m;
        vector<vector<char> > grid(n, vector<char>(m, '#'));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                cin >> grid[i][j];
            }
        }
        Solution obj;
        int ans = obj.numIslands(grid);
        cout << ans << '\n';
    }
    return 0;
} // } Driver Code Ends
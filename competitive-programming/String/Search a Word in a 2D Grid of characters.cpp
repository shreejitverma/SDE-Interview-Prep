/*
https://practice.geeksforgeeks.org/problems/find-the-string-in-grid0111/1

Find the string in grid 
Medium Accuracy: 46.55% Submissions: 9799 Points: 4
Given a 2D grid of n*m of characters and a word, find all occurrences of given word in grid. A word can be matched in all 8 directions at any point. Word is said be found in a direction if all characters match in this direction (not in zig-zag form). The 8 directions are, horizontally left, horizontally right, vertically up, vertically down and 4 diagonal directions.
 

Example 1:

Input: grid = {{a,b,c},{d,r,f},{g,h,i}},
word = "abc"
Output: {{0,0}}
Expalnation: From (0,0) one can find "abc"
in horizontally right direction.
Example 2:

Input: grid = {{a,b,a,b},{a,b,e,b},{e,b,e,b}}
,word = "abe"
Output: {{0,0},{0,2},{1,0}}
Explanation: From (0,0) one can find "abe" in 
right-down diagonal. From (0,2) one can
find "abe" in left-down diagonal. From
(1,0) one can find "abe" in Horizontally right 
direction.
 

Your Task:
You don't need to read or print anyhting, Your task is to complete the function searchWord() which takes grid and word as input parameters and returns a list containg the positions from where the word originates in any direction. If there is no such position then returns an empty list.

Note: The returning list should be lexicographically smallest. If the word can be found in multiple directions strating from the same coordinates, the list should contain the coordinates only once. 
 

Expected Time Complexity: O(n*m*k) where k is constant
Exected Space Complexity: O(1)
 

Constraints:
1 <= n <= m <= 100
1 <= |word| <= 10
*/
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    // This function searches in all 8-direction from point
    // (row, col) in grid[][]
    bool search2D(vector<vector<char> > grid, int row, int col, string word, vector<int> x, vector<int> y)
    {
        int R = grid.size();
        int C = grid[0].size();
        // If first character of word doesn't match with
        // given starting point in grid.
        if (grid[row][col] != word[0])
            return false;

        int len = word.size();

        // Search word in all 8 directions starting from (row,col)
        for (int dir = 0; dir < 8; dir++)
        {
            // Initialize starting point for current direction
            int k, rd = row + x[dir], cd = col + y[dir];

            // First character is already checked, match remaining
            // characters
            for (k = 1; k < len; k++)
            {
                // If out of bound break
                if (rd >= R || rd < 0 || cd >= C || cd < 0)
                    break;

                // If not matched, break
                if (grid[rd][cd] != word[k])
                    break;

                // Moving in particular direction
                rd += x[dir], cd += y[dir];
            }

            // If all character matched, then value of must
            // be equal to length of word
            if (k == len)
                return true;
        }
        return false;
    }
    vector<vector<int> > searchWord(vector<vector<char> > grid, string word)
    {
        int row = grid.size();
        int col = grid[0].size();
        vector<int> x = {-1, -1, -1, 0, 0, 1, 1, 1};
        vector<int> y = {-1, 0, 1, -1, 1, -1, 0, 1};
        vector<vector<int> > ans;
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                if (search2D(grid, i, j, word, x, y))
                {
                    ans.push_back({i, j});
                }
            }
        }
        return ans;
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
        vector<vector<char> > grid(n, vector<char>(m, 'x'));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
                cin >> grid[i][j];
        }
        string word;
        cin >> word;
        Solution obj;
        vector<vector<int> > ans = obj.searchWord(grid, word);
        for (auto i : ans)
        {
            for (auto j : i)
                cout << j << " ";
            cout << "\n";
        }
        if (ans.size() == 0)
        {
            cout << "-1\n";
        }
    }
    return 0;
} // } Driver Code Ends
// C++ programs to search a word in a 2D grid
#include <bits/stdc++.h>
using namespace std;

// For searching in all 8 direction
int x[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int y[] = {-1, 0, 1, -1, 1, -1, 0, 1};

// This function searches in
// all 8-direction from point
// (row, col) in grid[][]
bool search2D(char *grid, int row, int col,
              string word, int R, int C)
{
    // If first character of word doesn't
    // match with given starting point in grid.
    if (*(grid + row * C + col) != word[0])
        return false;

    int len = word.length();

    // Search word in all 8 directions
    // starting from (row, col)
    for (int dir = 0; dir < 8; dir++)
    {
        // Initialize starting point
        // for current direction
        int k, rd = row + x[dir], cd = col + y[dir];

        // First character is already checked,
        // match remaining characters
        for (k = 1; k < len; k++)
        {
            // If out of bound break
            if (rd >= R || rd < 0 || cd >= C || cd < 0)
                break;

            // If not matched,  break
            if (*(grid + rd * C + cd) != word[k])
                break;

            // Moving in particular direction
            rd += x[dir], cd += y[dir];
        }

        // If all character matched, then value of k must
        // be equal to length of word
        if (k == len)
            return true;
    }
    return false;
}

// Searches given word in a given
// matrix in all 8 directions
void patternSearch(char *grid, string word,
                   int R, int C)
{
    // Consider every point as starting
    // point and search given word
    for (int row = 0; row < R; row++)
        for (int col = 0; col < C; col++)
            if (search2D(grid, row, col, word, R, C))
                cout << "pattern found at "
                     << row << ", "
                     << col << endl;
}

// Driver program
int main()
{
    int R = 3, C = 13;
    char grid[R][C] = {"GEEKSFORGEEKS",
                       "GEEKSQUIZGEEK",
                       "IDEQAPRACTICE"};

    patternSearch((char *)grid, "GEEKS", R, C);
    cout << endl;
    patternSearch((char *)grid, "EEE", R, C);
    return 0;
}
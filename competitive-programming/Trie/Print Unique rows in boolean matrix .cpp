/*
https://practice.geeksforgeeks.org/problems/unique-rows-in-boolean-matrix/1
https://www.geeksforgeeks.org/print-unique-rows/
Unique rows in boolean matrix 
Easy Accuracy: 49.41% Submissions: 12314 Points: 2
Given a binary matrix your task is to find all unique rows of the given matrix.

Example 1:

Input:
row = 3, col = 4 
M[][] = {{1 1 0 1},{1 0 0 1},{1 1 0 1}}
Output: 1 1 0 1 $1 0 0 1 $
Explanation: Above the matrix of size 3x4
looks like
1 1 0 1
1 0 0 1
1 1 0 1
The two unique rows are 1 1 0 1 and
1 0 0 1 .
Your Task:
You only need to implement the given function uniqueRow(). The function takes three arguments the first argument is a matrix M and the next two arguments are row and col denoting the rows and columns of the matrix. The function should return the list of the unique row of the martrix. Do not read input, instead use the arguments given in the function.

Note: The drivers code print the rows "$" separated.

Expected Time Complexity: O(row * col)
Expected Auxiliary Space: O(row * col)

Constraints:
1<=row,col<=40
0<=M[][]<=1
*/

vector<vector<int> > uniqueRow(int a[MAX][MAX], int row, int col)
{
    //Your code here

    unordered_set<string> s;
    string temp;
    vector<vector<int> > v;
    for (int i = 0; i < row; i++)
    {
        temp = "";
        vector<int> t;
        for (int j = 0; j < col; j++)
        {
            temp += to_string(a[i][j]);
            t.push_back(a[i][j]);
        }
        if (s.find(temp) != s.end())
            continue;
        v.push_back(t);
        s.insert(temp);
    }
    return v;
}

// Given a binary matrix of M X N of integers,
// you need to return only unique rows of binary array
#include <bits/stdc++.h>
using namespace std;
#define ROW 4
#define COL 5

// A Trie node
class Node
{
public:
    bool isEndOfCol;
    Node *child[2]; // Only two children needed for 0 and 1
};

// A utility function to allocate memory
// for a new Trie node
Node *newNode()
{
    Node *temp = new Node();
    temp->isEndOfCol = 0;
    temp->child[0] = temp->child[1] = NULL;
    return temp;
}

// Inserts a new matrix row to Trie.
// If row is already present,
// then returns 0, otherwise insets the row and
// return 1
bool insert(Node **root, int (*M)[COL],
            int row, int col)
{
    // base case
    if (*root == NULL)
        *root = newNode();

    // Recur if there are more entries in this row
    if (col < COL)
        return insert(&((*root)->child[M[row][col]]),
                      M, row, col + 1);

    else // If all entries of this row are processed
    {
        // unique row found, return 1
        if (!((*root)->isEndOfCol))
            return (*root)->isEndOfCol = 1;

        // duplicate row found, return 0
        return 0;
    }
}

// A utility function to print a row
void printRow(int (*M)[COL], int row)
{
    int i;
    for (i = 0; i < COL; ++i)
        cout << M[row][i] << " ";
    cout << endl;
}

// The main function that prints
// all unique rows in a given matrix.
void findUniqueRows(int (*M)[COL])
{
    Node *root = NULL; // create an empty Trie
    int i;

    // Iterate through all rows
    for (i = 0; i < ROW; ++i)

        // insert row to TRIE
        if (insert(&root, M, i, 0))

            // unique row found, print it
            printRow(M, i);
}

// Driver Code
int main()
{
    int M[ROW][COL] = {{0, 1, 0, 0, 1},
                       {1, 0, 1, 1, 0},
                       {0, 1, 0, 0, 1},
                       {1, 0, 1, 0, 0}};

    findUniqueRows(M);

    return 0;
}
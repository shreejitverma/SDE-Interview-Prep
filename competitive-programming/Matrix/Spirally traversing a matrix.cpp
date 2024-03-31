/*
https://practice.geeksforgeeks.org/problems/spirally-traversing-a-matrix-1587115621/1
https://www.geeksforgeeks.org/print-a-given-matrix-in-spiral-form/
Spirally traversing a matrix 
Medium Accuracy: 48.39% Submissions: 68433 Points: 4
Given a matrix of size r*c. Traverse the matrix in spiral form.

Example 1:

Input:
r = 4, c = 4
matrix[][] = {{1, 2, 3, 4},
           {5, 6, 7, 8},
           {9, 10, 11, 12},
           {13, 14, 15,16}}
Output: 
1 2 3 4 8 12 16 15 14 13 9 5 6 7 11 10
Explanation:

Example 2:

Input:
r = 3, c = 4  
matrix[][] = {{1, 2, 3, 4},
           {5, 6, 7, 8},
           {9, 10, 11, 12}}
Output: 
1 2 3 4 8 12 11 10 9 5 6 7
Explanation:
Applying same technique as shown above, 
output for the 2nd testcase will be 
1 2 3 4 8 12 11 10 9 5 6 7.

Your Task:
You dont need to read input or print anything. Complete the function spirallyTraverse() that takes matrix, r and c as input parameters and returns a list of integers denoting the spiral traversal of matrix. 

Expected Time Complexity: O(r*c)
Expected Auxiliary Space: O(r*c), for returning the answer only.

Constraints:
1 <= r, c <= 100
0 <= matrixi <= 100
*/

{
    int i, k = 0, l = 0;
    vector<int> res;

    /* k - starting row index
        m - ending row index
        l - starting column index
        n - ending column index
        i - iterator
    */

    while (k < m && l < n)
    {
        /* Print the first row from
               the remaining rows */
        for (i = l; i < n; ++i)
        {
            res.push_back(a[k][i]);
        }
        k++;

        /* Print the last column
         from the remaining columns */
        for (i = k; i < m; ++i)
        {
            res.push_back(a[i][n - 1]);
        }
        n--;

        /* Print the last row from
                the remaining rows */
        if (k < m)
        {
            for (i = n - 1; i >= l; --i)
            {
                res.push_back(a[m - 1][i]);
            }
            m--;
        }

        /* Print the first column from
                   the remaining columns */
        if (l < n)
        {
            for (i = m - 1; i >= k; --i)
            {
                res.push_back(a[i][l]);
            }
            l++;
        }
    }
    return res
}
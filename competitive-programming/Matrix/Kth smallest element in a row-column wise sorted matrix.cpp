/*
https://practice.geeksforgeeks.org/problems/kth-element-in-matrix/1
Kth element in Matrix 
Medium Accuracy: 49.98% Submissions: 24073 Points: 4
Given a N x N matrix, where every row and column is sorted in non-decreasing order. Find the kth smallest element in the matrix.

Example 1:
Input:
N = 4
mat[][] =     {{16, 28, 60, 64},
                   {22, 41, 63, 91},
                   {27, 50, 87, 93},
                   {36, 78, 87, 94 }}
K = 3
Output: 27
Explanation: 27 is the 3rd smallest element.
 

Example 2:
Input:
N = 4
mat[][] =     {{10, 20, 30, 40}
                   {15, 25, 35, 45}
                   {24, 29, 37, 48}
                   {32, 33, 39, 50}}
K = 7
Output: 30
Explanation: 30 is the 7th smallest element.


Your Task:
You don't need to read input or print anything. Complete the function kthsmallest() which takes the mat, N and K as input parameters and returns the kth smallest element in the matrix.
 

Expected Time Complexity: O(K*Log(N))
Expected Auxiliary Space: O(N)

 

Constraints:
1 <= N <= 50
1 <= mat[][] <= 10000
1 <= K <= N*N
*/

int kthSmallest(int mat[MAX][MAX], int n, int k)
{
    priority_queue<int> pq;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            pq.push(mat[i][j]);
            if (pq.size() > k)
            {
                pq.pop();
            }
        }
    }
    return pq.top();
}
// Another one
typedef pair<int, pair<int, int> > pi;
int kthSmallest(int mat[MAX][MAX], int n, int k)
{
    priority_queue<pi, vector<pi>, greater<pi> > pq;
    for (int j = 0; j < n; j++)
    {
        pq.push(make_pair(mat[0][j], make_pair(0, j)));
    }
    int ans = -1;
    pair<int, pair<int, int> > pp;
    while (k--)
    {
        pp = pq.top();
        pq.pop();
        int r = pp.second.first;
        int c = pp.second.second;
        if (r + 1 < n)
        {
            pq.push(make_pair(mat[r + 1][c], make_pair(r + 1, c)));
        }
    }
    return pp.first;
}

/*
https://practice.geeksforgeeks.org/problems/check-mirror-in-n-ary-tree1528/1
Check Mirror in N-ary tree 
Medium Accuracy: 57.95% Submissions: 6708 Points: 4
Given two n-ary trees. Check if they are mirror images of each other or not. You are also given e denoting the number of edges in both trees, and two arrays, A[] and B[]. Each array has 2*e space separated values u,v denoting an edge from u to v for the both trees.


Example 1:

Input:
n = 3, e = 2
A[] = {1, 2, 1, 3}
B[] = {1, 3, 1, 2}
Output:
1
Explanation:
   1          1
 / \        /  \
2   3      3    2 
As we can clearly see, the second tree
is mirror image of the first.
Example 2:

Input:
n = 3, e = 2
A[] = {1, 2, 1, 3}
B[] = {1, 2, 1, 3}
Output:
0
Explanation:
   1          1
 / \        /  \
2   3      2    3 
As we can clearly see, the second tree
isn't mirror image of the first.

Your Task:
You don't need to read input or print anything. Your task is to complete the function checkMirrorTree() which takes 2 Integers n, and e;  and two arrays A[] and B[] of size 2*e as input and returns 1 if the trees are mirror images of each other and 0 if not.


Expected Time Complexity: O(n)
Expected Auxiliary Space: O(n)


Constraints:
1 <= n,e <= 105
*/
// { Driver Code Starts
//Initial Template for C++

#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
//User function Template for C++

class Solution
{
public:
  int checkMirrorTree(int n, int e, int A[], int B[])
  {
    vector<stack<int> > s(n + 1);
    vector<queue<int> > q(n + 1);
    for (int i = 0; i < 2 * e; i += 2)
    {
      s[A[i]].push(A[i + 1]);
      q[B[i]].push(B[i + 1]);
    }
    // now take out the vectors and queues
    // for each of the nodes and compare them
    // one by one
    for (int i = 1; i <= n; i++)
    {
      while (!s[i].empty() && !q[i].empty())
      {
        int a = s[i].top();
        int b = q[i].front();
        if (a != b)
          return false;
        s[i].pop();
        q[i].pop();
      }
    }
    return true;
  }
};

// { Driver Code Starts.
int main()
{
  int t;
  cin >> t;
  while (t--)
  {
    int n, e;

    cin >> n >> e;
    int A[2 * e], B[2 * e];

    for (int i = 0; i < 2 * e; i++)
      cin >> A[i];

    for (int i = 0; i < 2 * e; i++)
      cin >> B[i];

    Solution ob;
    cout << ob.checkMirrorTree(n, e, A, B) << endl;
  }
  return 0;
} // } Driver Code Ends
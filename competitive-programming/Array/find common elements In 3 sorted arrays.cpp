/*
https://practice.geeksforgeeks.org/problems/common-elements1132/1
Common elements 
Easy Accuracy: 38.69% Submissions: 81306 Points: 2
Given three arrays sorted in increasing order. Find the elements that are common in all three arrays.
Note: can you take care of the duplicates without using any additional Data Structure?

Example 1:

Input:
n1 = 6; A = {1, 5, 10, 20, 40, 80}
n2 = 5; B = {6, 7, 20, 80, 100}
n3 = 8; C = {3, 4, 15, 20, 30, 70, 80, 120}
Output: 20 80
Explanation: 20 and 80 are the only
common elements in A, B and C.
 

Your Task:  
You don't need to read input or print anything. Your task is to complete the function commonElements() which take the 3 arrays A[], B[], C[] and their respective sizes n1, n2 and n3 as inputs and returns an array containing the common element present in all the 3 arrays in sorted order. 
If there are no such elements return an empty array. In this case the output will be printed as -1.

 

Expected Time Complexity: O(n1 + n2 + n3)
Expected Auxiliary Space: O(n1 + n2 + n3)

 

Constraints:
1 <= n1, n2, n3 <= 10^5
The array elements can be both positive or negative integers.
*/
class Solution
{
public:
    vector<int> commonElements(int A[], int B[], int C[], int n1, int n2, int n3)
    {
        vector<int> res;

        int i = 0, j = 0, k = 0;

        while (i < n1 and j < n2 and k < n3)
        {
            while (i > 0 and i < n1 and A[i - 1] == A[i])
            {
                i++;
            }

            while (j > 0 and j < n2 and B[j - 1] == B[j])
            {
                j++;
            }

            while (k > 0 and k < n3 and C[k - 1] == C[k])
            {
                k++;
            }

            if (i < n1 and j < n2 and k < n3)
            {
                if (A[i] == B[j] and B[j] == C[k])
                {
                    res.push_back(A[i]);

                    i++;
                    j++;
                    k++;
                }
                else if (A[i] < B[j])
                {
                    i++;
                }
                else if (B[j] < C[k])
                {
                    j++;
                }
                else
                {
                    k++;
                }
            }
        }

        return res;
    }
};
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    int check(int a, int b, int c)
    {
        int temp = min({a, b, c});

        if (temp == a)
            return 1;
        else if (temp == b)
            return 2;
        else
            return 3;
    }
    vector<int> commonElements(int A[], int B[], int C[], int n1, int n2, int n3)
    {
        //code here.
        vector<int> arr1;

        int p1 = 0, p2 = 0, p3 = 0;

        int rec = INT_MIN;

        while (p1 < n1 && p2 < n2 && p3 < n3)
        {
            if (A[p1] == B[p2] && B[p2] == C[p3])
            {
                if (rec != A[p1])
                {
                    rec = A[p1];
                    arr1.push_back(A[p1]);
                }
                ++p1;
            }
            else
            {
                int choice = check(A[p1], B[p2], C[p3]);

                if (choice == 1)
                    ++p1;
                else if (choice == 2)
                    ++p2;
                else
                    ++p3;
            }
        }

        if (arr1.size() == 0)
            arr1.push_back(-1);

        return arr1;
    }
};

// { Driver Code Starts.

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int n1, n2, n3;
        cin >> n1 >> n2 >> n3;
        int A[n1];
        int B[n2];
        int C[n3];

        for (int i = 0; i < n1; i++)
            cin >> A[i];
        for (int i = 0; i < n2; i++)
            cin >> B[i];
        for (int i = 0; i < n3; i++)
            cin >> C[i];

        Solution ob;

        vector<int> res = ob.commonElements(A, B, C, n1, n2, n3);
        if (res.size() == 0)
            cout << -1;
        for (int i = 0; i < res.size(); i++)
            cout << res[i] << " ";
        cout << endl;
    }
} // } Driver Code Ends
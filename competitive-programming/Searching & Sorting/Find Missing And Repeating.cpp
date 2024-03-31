/*
https://practice.geeksforgeeks.org/problems/find-missing-and-repeating2512/1
Find Missing And Repeating 
Medium Accuracy: 37.77% Submissions: 77816 Points: 4
Given an unsorted array Arr of size N of positive integers. One number 'A' from set {1, 2, …N} is missing and one number 'B' occurs twice in array. Find these two numbers.

Example 1:

Input:
N = 2
Arr[] = {2, 2}
Output: 2 1
Explanation: Repeating number is 2 and 
smallest positive missing number is 1.
Example 2:

Input:
N = 3
Arr[] = {1, 3, 3}
Output: 3 2
Explanation: Repeating number is 3 and 
smallest positive missing number is 2.
Your Task:
You don't need to read input or print anything. Your task is to complete the function findTwoElement() which takes the array of integers arr and n as parameters and returns an array of integers of size 2 denoting the answer ( The first index contains B and second index contains A.)

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)

Constraints:
1 ≤ N ≤ 105
1 ≤ Arr[i] ≤ N
*/

// { Driver Code Starts
#include <bits/stdc++.h>

using namespace std;

// } Driver Code Ends
class Solution
{
public:
    int *findTwoElement(int *arr, int N)
    {
        // code here
        int *res = new int(2);

        for (int i = 0; i < N; i++)
        {
            arr[i] -= 1;
        }

        for (int i = 0; i < N; i++)
        {
            arr[arr[i] % N] += N;
        }

        for (int i = 0; i < N; i++)
        {
            arr[i] /= N;

            if (arr[i] == 2)
            {
                res[0] = i + 1;
            }

            if (arr[i] == 0)
            {
                res[1] = i + 1;
            }
        }

        return res;
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
        int a[n];
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        Solution ob;
        auto ans = ob.findTwoElement(a, n);
        cout << ans[0] << " " << ans[1] << "\n";
    }
    return 0;
} // } Driver Code Ends
/*
https://practice.geeksforgeeks.org/problems/sort-an-array-of-0s-1s-and-2s4231/1#
Sort an array of 0s, 1s and 2s 
Easy Accuracy: 51.36% Submissions: 100k+ Points: 2
Given an array of size N containing only 0s, 1s, and 2s; sort the array in ascending order.


Example 1:

Input: 
N = 5
arr[]= {0 2 1 2 0}
Output:
0 0 1 2 2
Explanation:
0s 1s and 2s are segregated 
into ascending order.
Example 2:

Input: 
N = 3
arr[] = {0 1 0}
Output:
0 0 1
Explanation:
0s 1s and 2s are segregated 
into ascending order.

Your Task:
You don't need to read input or print anything. Your task is to complete the function sort012() that takes an array arr and N as input parameters and sorts the array in-place.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)


Constraints:
1 <= N <= 10^6
0 <= A[i] <= 2
*/

#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    void sort012(int arr[], int n)
    {
        int low = 0, mid = 0, high = n - 1;

        while (mid <= high)
        {
            switch (arr[mid])
            {
            case 0:
                swap(arr[low], arr[mid]);
                low++;
                mid++;
                break;

            case 1:
                mid++;
                break;

            case 2:
                swap(arr[mid], arr[high]);
                high--;
                break;
            }
        }
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
        ob.sort012(a, n);

        for (int i = 0; i < n; i++)
        {
            cout << a[i] << " ";
        }

        cout << endl;
    }
    return 0;
}

// } Driver Code Ends
/*
https://practice.geeksforgeeks.org/problems/first-and-last-occurrences-of-x3116/1
First and last occurrences of x 
Basic Accuracy: 53.04% Submissions: 41057 Points: 1
Given a sorted array arr containing n elements with possibly duplicate elements, the task is to find indexes of first and last occurrences of an element x in the given array.

Example 1:

Input:
n=9, x=5
arr[] = { 1, 3, 5, 5, 5, 5, 67, 123, 125 }
Output:  2 5
Explanation: First occurrence of 5 is at index 2 and last
             occurrence of 5 is at index 5. 
 

Example 2:

Input:
n=9, x=7
arr[] = { 1, 3, 5, 5, 5, 5, 7, 123, 125 }
Output:  6 6 

Your Task:
Since, this is a function problem. You don't need to take any input, as it is already accomplished by the driver code. You just need to complete the function find() that takes array arr, integer n and integer x as parameters and returns the required answer.
Note: If the number x is not found in the array just return both index as -1.

 
Expected Time Complexity: O(logN)
Expected Auxiliary Space: O(1).

 

Constraints:
1 ≤ N ≤ 107
*/

// Another One

vector<int> find(int arr[], int n, int x)
{
    auto it1 = lower_bound(arr, arr + n, x);

    if (it1 == arr + n || (*it1) != x)
    {
        return {-1, -1};
    }

    auto it2 = upper_bound(arr, arr + n, x);

    return {(it1 - arr), (it2 - 1 - arr)};
}
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;
vector<int> find(int a[], int n, int x);

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int n, x;
        cin >> n >> x;
        int arr[n], i;
        for (i = 0; i < n; i++)
            cin >> arr[i];
        vector<int> ans;
        ans = find(arr, n, x);
        cout << ans[0] << " " << ans[1] << endl;
    }
    return 0;
}

// } Driver Code Ends

int firstLastOcc(int arr[], int n, int x, string Y)
{
    int start = 0, end = n - 1;
    int res = -1;

    while (start <= end)
    {
        int mid = start + (end - start) / 2;

        /* for find first Occurence */

        if (arr[mid] == x && Y == "firstOcc")
            res = mid, end = mid - 1;

        /* for finding last Occurence */

        else if (arr[mid] == x && Y == "lastOcc")
            res = mid, start = mid + 1;

        else if (arr[mid] > x)
            end = mid - 1;

        else
            start = mid + 1;
    }
    return res;
}

vector<int> find(int arr[], int n, int x)
{

    return {firstLastOcc(arr, n, x, "firstOcc"), firstLastOcc(arr, n, x, "lastOcc")};
}
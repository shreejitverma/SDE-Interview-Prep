/*
https://practice.geeksforgeeks.org/problems/find-pair-given-difference1559/1
Find Pair Given Difference 
Easy Accuracy: 49.01% Submissions: 30117 Points: 2
Given an array Arr[] of size L and a number N, you need to write a program to find if there exists a pair of elements in the array whose difference is N.

Example 1:

Input:
L = 6, N = 78
arr[] = {5, 20, 3, 2, 5, 80}
Output: 1
Explanation: (2, 80) have difference of 78.
Example 2:

Input:
L = 5, N = 45
arr[] = {90, 70, 20, 80, 50}
Output: -1
Explanation: There is no pair with difference of 45.
Your Task:
You need not take input or print anything. Your task is to complete the function findPair() which takes array arr, size of the array L and N as input parameters and returns True if required pair exists, else return False.

Expected Time Complexity: O(L* Log(L)).
Expected Auxiliary Space: O(1).

Constraints:
1 ≤ L ≤ 104
1 ≤ Arr[i], N ≤ 105
*/

// { Driver Code Starts
#include <bits/stdc++.h>

using namespace std;

bool findPair(int arr[], int size, int n);

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int l, n;
        cin >> l >> n;
        int arr[l];
        for (int i = 0; i < l; i++)
            cin >> arr[i];
        if (findPair(arr, l, n))
            cout << 1 << endl;
        else
            cout << "-1" << endl;
    }

    return 0;
} // } Driver Code Ends

bool findPair(int arr[], int size, int n)
{
    sort(arr, arr + size);
    bool flag;
    for (int i = size - 1; i >= 0; i--)
    {
        int x = arr[i] - n;
        flag = binary_search(arr, arr + size, x);
        if (flag == true)
            return flag;
    }
    return flag;
}
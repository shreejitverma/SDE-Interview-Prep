/*
https://practice.geeksforgeeks.org/problems/maximum-product-subarray3604/1
Maximum Product Subarray 
Medium Accuracy: 29.84% Submissions: 89019 Points: 4
Given an array Arr[] that contains N integers (may be positive, negative or zero). Find the product of the maximum product subarray.

Example 1:

Input:
N = 5
Arr[] = {6, -3, -10, 0, 2}
Output: 180
Explanation: Subarray with maximum product
is [6, -3, -10] which gives product as 180.
Example 2:

Input:
N = 6
Arr[] = {2, 3, 4, 5, -1, 0}
Output: 120
Explanation: Subarray with maximum product
is [2, 3, 4, 5] which gives product as 120.
Your Task:
You don't need to read input or print anything. Your task is to complete the function maxProduct() which takes the array of integers arr and n as parameters and returns an integer denoting the answer.
Note: Use 64-bit integer data type to avoid overflow.

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)

Constraints:
1 ≤ N ≤ 500
-102 ≤ Arri ≤ 102
*/

// { Driver Code Starts
#include <bits/stdc++.h>

using namespace std;

// } Driver Code Ends
//User function template for C++
class Solution
{
public:
    // Function to find maximum product subarray
    long long maxProduct(vector<int> arr, int n)
    {
        // code here
        long long int ans = arr[0];
        long long int mx = ans;
        long long int mn = ans;
        for (long long int i = 1; i < n; i++)
        {

            if (arr[i] < 0)
                swap(mx, mn);
            if (arr[i] * mx > arr[i])
                mx = arr[i] * mx;
            else
                mx = arr[i];
            if (arr[i] * mn < arr[i])
            {
                mn = mn * arr[i];
            }
            else
                mn = arr[i];
            if (mx > ans)
                ans = mx;
            else
                ans = ans;
        }
        return ans;
    }
};

// { Driver Code Starts.

int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        int n, i;
        cin >> n;
        vector<int> arr(n);
        for (i = 0; i < n; i++)
        {
            cin >> arr[i];
        }
        Solution ob;
        auto ans = ob.maxProduct(arr, n);
        cout << ans << "\n";
    }
    return 0;
} // } Driver Code Ends
/*
https://practice.geeksforgeeks.org/problems/inversion-of-array-1587115620/1
Count Inversions 
Medium Accuracy: 39.43% Submissions: 100k+ Points: 4
Given an array of integers. Find the Inversion Count in the array. 

Inversion Count: For an array, inversion count indicates how far (or close) the array is from being sorted. If array is already sorted then the inversion count is 0. If an array is sorted in the reverse order then the inversion count is the maximum. 
Formally, two elements a[i] and a[j] form an inversion if a[i] > a[j] and i < j.
 

Example 1:

Input: N = 5, arr[] = {2, 4, 1, 3, 5}
Output: 3
Explanation: The sequence 2, 4, 1, 3, 5 
has three inversions (2, 1), (4, 1), (4, 3).
Example 2:

Input: N = 5
arr[] = {2, 3, 4, 5, 6}
Output: 0
Explanation: As the sequence is already 
sorted so there is no inversion count.
Example 3:

Input: N = 3, arr[] = {10, 10, 10}
Output: 0
Explanation: As all the elements of array 
are same, so there is no inversion count.
Your Task:
You don't need to read input or print anything. Your task is to complete the function inversionCount() which takes the array arr[] and the size of the array as inputs and returns the inversion count of the given array.

Expected Time Complexity: O(NLogN).
Expected Auxiliary Space: O(N).

Constraints:
1 ≤ N ≤ 5*105
1 ≤ arr[i] ≤ 1018
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
    long long merge(long long arr[], long long low, long long mid, long long high)
    {
        long long n1 = mid - low + 1;
        long long n2 = high - mid;

        long long right[n1], left[n2];

        for (long long i = 0; i < n1; i++)
        {
            left[i] = arr[low + i];
        }

        for (long long i = 0; i < n2; i++)
        {
            right[i] = arr[mid + i + 1];
        }

        long long i = 0, j = 0, k = low;
        long long res = 0;

        while (i < n1 && j < n2)
        {
            if (left[i] <= right[j])
            {
                arr[k++] = left[i++];
            }
            else
            {
                arr[k++] = right[j++];
                res += (n1 - i);
            }
        }

        while (i < n1)
        {
            arr[k++] = left[i++];
        }

        while (j < n2)
        {
            arr[k++] = right[j++];
        }

        return res;
    }

    long long mergeSort(long long arr[], long long low, long long high)
    {
        long long res = 0;

        if (low < high)
        {
            long long mid = low + (high - low) / 2;

            res += mergeSort(arr, low, mid);
            res += mergeSort(arr, mid + 1, high);

            res += merge(arr, low, mid, high);
        }

        return res;
    }

public:
    long long inversionCount(long long arr[], long long N)
    {
        return mergeSort(arr, 0, N - 1);
    }
};

// { Driver Code Starts.

int main()
{

    long long T;
    cin >> T;

    while (T--)
    {
        long long N;
        cin >> N;

        long long A[N];
        for (long long i = 0; i < N; i++)
        {
            cin >> A[i];
        }
        Solution obj;
        cout << obj.inversionCount(A, N) << endl;
    }

    return 0;
}
// } Driver Code Ends
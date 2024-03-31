/*
https://practice.geeksforgeeks.org/problems/find-smallest-range-containing-elements-from-k-lists/1
Smallest range in K lists 
Hard Accuracy: 50.36% Submissions: 11146 Points: 8
Given K sorted lists of integers, KSortedArray[] of size N each. The task is to find the smallest range that includes at least one element from each of the K lists. If more than one such range's are found, return the first such range found.

Example 1:

Input:
N = 5, K = 3
KSortedArray[][] = {{1 3 5 7 9},
                    {0 2 4 6 8},
                    {2 3 5 7 11}}
Output: 1 2
Explanation: K = 3
A:[1 3 5 7 9]
B:[0 2 4 6 8]
C:[2 3 5 7 11]
Smallest range is formed by number 1
present in first list and 2 is present
in both 2nd and 3rd list.
Example 2:

Input:
N = 4, K = 3
KSortedArray[][] = {{1 2 3 4},
                    {5 6 7 8},
                    {9 10 11 12}}
Output: 4 9
Your Task :

Complete the function findSmallestRange() that receives array , array size n and k as parameters and returns the output range (as a pair in cpp and array of size 2 in java and python)

Expected Time Complexity : O(n * k *log k)
Expected Auxilliary Space  : O(k)

Constraints:
1 <= K,N <= 500
0 <= a[ i ] <= 105
*/
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;
#define N 1000

// } Driver Code Ends
// you are required to complete this function
// function should print the required range
typedef pair<int, pair<int, int> > p;
class Solution
{
public:
    pair<int, int> findSmallestRange(int arr[][N], int n, int k)
    {
        priority_queue<p, vector<p>, greater<p> > pq; //min heap

        int minval = INT_MAX;
        int maxval = INT_MIN;
        int minRange = INT_MAX;

        for (int i = 0; i < k; i++)
        {
            pq.push({arr[i][0], {i, 0}});    //push first elem of every list
            maxval = max(maxval, arr[i][0]); //and update max val
        }

        int start = 0; //start and end of range
        int end = 0;

        while (true)
        {
            auto curr = pq.top(); //pop min(top)
            pq.pop();

            minval = curr.first; //update min val

            if (maxval - minval < minRange) //update range if < diff found
            {
                start = minval;
                end = maxval;
                minRange = end - start;
            }

            int i = curr.second.first;  // array no
            int j = curr.second.second; //index in array no

            if (j + 1 == n) //if list tarversed fully
                break;

            pq.push({arr[i][j + 1], {i, j + 1}}); //push next elem of this popped elm of same list

            if (maxval < arr[i][j + 1])
                maxval = arr[i][j + 1]; //keep updating max with next pushing elem
        }

        pair<int, int> res = {start, end};
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
        int n, k;
        cin >> n >> k;
        int arr[N][N];
        pair<int, int> rangee;
        for (int i = 0; i < k; i++)
            for (int j = 0; j < n; j++)
                cin >> arr[i][j];
        Solution obj;
        rangee = obj.findSmallestRange(arr, n, k);
        cout << rangee.first << " " << rangee.second << "\n";
    }
    return 0;
}

// } Driver Code Ends
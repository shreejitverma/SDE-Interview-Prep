/*
https: // practice.geeksforgeeks.org/problems/minimize-the-heights3351/1
https: // www.geeksforgeeks.org/minimize-the-maximum-difference-between-the-heights/

Minimize the Heights II
Medium Accuracy: 28.62 % Submissions: 100k + Points: 4
Given an array arr[] denoting heights of N towers and a positive integer K, 
you have to modify the height of each tower either by increasing or decreasing them by K only once. 
After modifying, height should be a non-negative integer.
Find out what could be the possible minimum difference of the height of shortest and 
longest towers after you have modified each tower.

A slight modification of the problem can be found here.


Example 1:

Input:
K = 2, N = 4
Arr[] = {1, 5, 8, 10}
Output:
5
Explanation:
The array can be modified as
{3, 3, 6, 8}. The difference between
the largest and the smallest is 8-3 = 5.
Example 2:

Input:
K = 3, N = 5
Arr[] = {3, 9, 12, 16, 20}
Output:
11
Explanation:
The array can be modified as
{6, 12, 9, 13, 17}. The difference between
the largest and the smallest is 17-6 = 11.

Your Task:
You don't need to read input or print anything. 
Your task is to complete the function getMinDiff() which takes the arr[],
 n and k as input parameters and returns an integer denoting the minimum difference.


Expected Time Complexity: O(N*logN)
Expected Auxiliary Space: O(N)

Constraints
1 ≤ K ≤ 104
1 ≤ N ≤ 105
1 ≤ Arr[i] ≤ 105

Given heights of n towers and a value k. We need to either increase or 
decrease the height of every tower by k(only once) where k > 0. 
The task is to minimize the difference between the heights of the longest and 
the shortest tower after modifications and output this difference.
Examples:

Input: arr[] = {1, 15, 10}, k = 6
Output:  Maximum difference is 5.
Explanation: We change 1 to 7, 15 to
9 and 10 to 4. Maximum difference is 5
(between 4 and 9). We can't get a lower
difference.

Input: arr[] = {1, 5, 15, 10}
k = 3
Output: Maximum difference is 8
arr[] = {4, 8, 12, 7}

Input: arr[] = {4, 6}
k = 10
Output: Maximum difference is 2
arr[] = {14, 16} OR {-6, -4}

Input: arr[] = {6, 10}
k = 3
Output: Maximum difference is 2
arr[] = {9, 7}

Input: arr[] = {1, 10, 14, 14, 14, 15}
k = 6
Output: Maximum difference is 5
arr[] = {7, 4, 8, 8, 8, 9}

Input: arr[] = {1, 2, 3}
k = 2
Output: Maximum difference is 2
arr[] = {3, 4, 5}

First, we try to sort the array and make each height of the tower maximum.
We do this by decreasing the height of all the towers towards the right by k and
increasing all the height of the towers towards the left(by k).
It is also possible that the tower you are trying to increase the height doesn’t have the maximum height.
Therefore we only need to check whether it has the maximum height or not
by comparing it with the last element towards the right side which is a[n]-k.
Since the array is sorted if the tower’s height is greater than the a[n]-k then
it’s the tallest tower available.
Similar reasoning can also be applied for finding the shortest tower.


Note: - We need not consider where a[i] < k because
the height of the tower can’t be negative so we have to neglect that case.

Time Complexity: O(nlogn)

Space Complexity: O(n)



*/

#include <bits/stdc++.h>
using namespace std;

#define pii pair<int, int>
class Solution
{

public:
    int getMinDiff(int arr[], int n, int k)
    {

        if (n == 1)
            return 0;

        sort(arr, arr + n);

        vector<pii> t;

        map<int, int> m;

        int n_ = 1;

        t.push_back(pii(arr[0] + k, arr[0]));

        t.push_back(pii(arr[0] - k, arr[0]));

        for (int i = 1; i < n; i++)
        {

            if (arr[i] != arr[i - 1])
            {

                t.push_back(pii(arr[i] + k, arr[i]));

                t.push_back(pii(arr[i] - k, arr[i]));

                m[arr[i]] = 0;

                n_++;
            }
        }

        sort(t.begin(), t.end());

        int l = 0, r = 0;

        int ans = t[t.size() - 1].first - t[0].first;

        int count = 0;

        while (r < t.size())
        {

            while (r < t.size() and count < n_)
            {

                if (m[t[r].second] == 0)
                    count++;

                m[t[r].second]++;

                r++;
            }

            if (r == t.size() and count < n_)
                break;

            ans = min(ans, t[r - 1].first - t[l].first);

            while (l <= r and count >= n_)
            {

                if (m[t[l].second] == 1)
                    count--;

                m[t[l].second]--;

                ans = min(ans, t[r - 1].first - t[l].first);

                l++;
            }
        }

        return ans;
    }
};

int main()
{
    Solution s;
    int k = 5, n = 10;
    int arr[n] = {8, 1, 5, 4, 7, 5, 7, 9, 4, 6};
    cout << s.getMinDiff(arr, n, k);
}
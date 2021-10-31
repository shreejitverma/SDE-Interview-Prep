'''https: // practice.geeksforgeeks.org/problems/minimize-the-heights3351/1
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
1 ≤ Arr[i] ≤ 105'''


class Solution:
    def getMinDiff(self, arr, n, k):
        arr.sort()
        ans = arr[-1] - arr[0]
        small = arr[0] + k
        large = arr[n-1] - k

        for i in range(n-1):
            mi = min(small, arr[i+1]-k)
            ma = max(large, arr[i]+k)
            if mi < 0:
                continue
            ans = min(ans, ma-mi)
        return ans

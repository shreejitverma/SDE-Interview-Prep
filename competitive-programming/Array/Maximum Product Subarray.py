'''https://practice.geeksforgeeks.org/problems/maximum-product-subarray3604/1
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
-102 ≤ Arri ≤ 102'''

# User function Template for python3


class Solution:

    # Function to find maximum
    # product subarray
    def maxProduct(self, a, n):
        # code here
        minVal = a[0]
    maxVal = a[0]

    maxProd = a[0]

    for i in range(1, n):

        if a[i] < 0:
            maxVal, minVal = minVal, maxVal

        maxVal = max(a[i], maxVal*a[i])
        minVal = min(a[i], minVal*a[i])

        maxProd = max(maxVal, maxProd)

    return maxProd
# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == '__main__':
    tc = int(input())
    while tc > 0:
        n = int(input())
        arr = list(map(int, input().strip().split()))
        ob = Solution()
        ans = ob.maxProduct(arr, n)
        print(ans)
        tc -= 1

# } Driver Code Ends

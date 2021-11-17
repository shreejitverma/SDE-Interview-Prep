'''https://practice.geeksforgeeks.org/problems/count-pairs-with-given-sum5022/1
Count pairs with given sum 
Easy Accuracy: 41.59% Submissions: 84181 Points: 2
Given an array of N integers, and an integer K, find the number of pairs of elements in the array whose sum is equal to K.


Example 1:

Input:
N = 4, K = 6
arr[] = {1, 5, 7, 1}
Output: 2
Explanation: 
arr[0] + arr[1] = 1 + 5 = 6 
and arr[1] + arr[3] = 5 + 1 = 6.

Example 2:

Input:
N = 4, X = 2
arr[] = {1, 1, 1, 1}
Output: 6
Explanation: 
Each 1 will produce sum 2 with any 1.

Your Task:
You don't need to read input or print anything. Your task is to complete the function getPairsCount() which takes arr[], n and k as input parameters and returns the number of pairs that have sum K.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)

Constraints:
1 <= N <= 105
1 <= K <= 108
1 <= Arr[i] <= 106'''

# User function Template for python3


class Solution:
    def getPairsCount(self, array, n, target):
        freqMap = {}
        for num in array:
            freqMap[num] = freqMap.get(num, 0) + 1

        pairs = 0
        for num in array:
            if (target - num) in freqMap:
                pairs += freqMap[target - num]
                if target == num + num:
                    pairs -= 1
        return pairs // 2


# {
#  Driver Code Starts
# Initial Template for Python 3

if __name__ == '__main__':
    tc = int(input())
    while tc > 0:
        n, k = list(map(int, input().strip().split()))
        arr = list(map(int, input().strip().split()))
        ob = Solution()
        ans = ob.getPairsCount(arr, n, k)
        print(ans)
        tc -= 1

# } Driver Code Ends

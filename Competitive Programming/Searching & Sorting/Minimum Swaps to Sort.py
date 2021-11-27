'''
https://practice.geeksforgeeks.org/problems/minimum-swaps/1
Minimum Swaps to Sort 
Medium Accuracy: 50.0% Submissions: 71576 Points: 4
Given an array of n distinct elements. Find the minimum number of swaps required to sort the array in strictly increasing order.


Example 1:

Input:
nums = {2, 8, 5, 4}
Output:
1
Explaination:
swap 8 with 4.
Example 2:

Input:
nums = {10, 19, 6, 3, 5}
Output:
2
Explaination:
swap 10 with 3 and swap 19 with 5.

Your Task:
You do not need to read input or print anything. Your task is to complete the function minSwaps() which takes the nums as input parameter and returns an integer denoting the minimum number of swaps required to sort the array. If the array is already sorted, return 0. 


Expected Time Complexity: O(nlogn)
Expected Auxiliary Space: O(n)


Constraints:
1 ≤ n ≤ 105
1 ≤ numsi ≤ 106'''


class Solution:
    def correctPosition(self, sorted_nums, val, n):
        start = 0
        end = n-1
        mid = start + (end-start)//2
        while start <= end:
            mid = start + (end-start)//2
            if sorted_nums[mid] == val:
                return mid
            elif sorted_nums[mid] < val:
                start = mid+1
            else:
                end = mid-1
        return mid

    # Function to find the minimum number of swaps required to sort the array.
    def minSwaps(self, nums):
        # Code here
        sorted_nums = sorted(nums)
        swaps = 0
        n = len(nums)
        i = 0
        while i < n:
            if nums[i] != sorted_nums[i]:
                j = self.correctPosition(sorted_nums, nums[i], n)
                nums[j], nums[i] = nums[i], nums[j]
                swaps += 1
                i -= 1
            i += 1
        return swaps

# {
#  Driver Code Starts


if __name__ == '__main__':
    T = int(input())
    for i in range(T):
        n = int(input())
        nums = list(map(int, input().split()))
        obj = Solution()
        ans = obj.minSwaps(nums)
        print(ans)

# } Driver Code Ends

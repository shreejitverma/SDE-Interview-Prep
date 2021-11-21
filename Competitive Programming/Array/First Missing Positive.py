'''https://leetcode.com/problems/first-missing-positive/
41. First Missing Positive
Hard

7625

1170

Add to List

Share
Given an unsorted integer array nums, return the smallest missing positive integer.

You must implement an algorithm that runs in O(n) time and uses constant extra space.

 

Example 1:

Input: nums = [1,2,0]
Output: 3
Example 2:

Input: nums = [3,4,-1,1]
Output: 2
Example 3:

Input: nums = [7,8,9,11,12]
Output: 1
 

Constraints:

1 <= nums.length <= 5 * 105
-231 <= nums[i] <= 231 - 1'''


class Solution:
    def firstMissingPositive(self, nums: List[int]) -> int:
        i = 0
        n = len(nums)
        while (i < n):
            correct = nums[i] - 1
            if (nums[i] > 0 and nums[i] <= n and nums[i] != nums[correct]):
                nums[i], nums[correct] = nums[correct], nums[i]
            else:
                i += 1
        for index in range(n):
            if (nums[index] != index+1):
                return index+1
        return n+1

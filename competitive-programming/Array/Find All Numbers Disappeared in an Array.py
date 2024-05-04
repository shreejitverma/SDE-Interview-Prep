'''https://leetcode.com/problems/find-all-numbers-disappeared-in-an-numsay/submissions/
448. Find All Numbers Disappeared in an Array
Easy

5401

344

Add to List

Share
Given an array nums of n integers where nums[i] is in the range [1, n], return an array of all the integers in the range [1, n] that do not appear in nums.

 

Example 1:

Input: nums = [4,3,2,7,8,2,3,1]
Output: [5,6]
Example 2:

Input: nums = [1,1]
Output: [2]
 

Constraints:

n == nums.length
1 <= n <= 105
1 <= nums[i] <= n
 

Follow up: Could you do it without extra space and in O(n) runtime? You may assume the returned list does not count as extra space.'''


class Solution:
    def findDisappearedNumbers(self, nums: List[int]) -> List[int]:
        i = 0
        n = len(nums)
        while (i < n):
            correct = nums[i] - 1
            if (nums[i] != nums[correct]):
                nums[i], nums[correct] = nums[correct], nums[i]
            else:
                i += 1
        ans = []

        for index in range(n):
            if (nums[index] != index+1):
                ans.append(index+1)
        return ans


class Solution:
    def findDisappearedNumbers(self, nums: List[int]) -> List[int]:
        for num in nums:
            # Mark the index corresponding to the visited number as negative
            index = abs(num) - 1
            nums[index] = -abs(nums[index])

        # Collect the indices of non-negative numbers (missing integers)
        result = [i + 1 for i, num in enumerate(nums) if num > 0]

        return result

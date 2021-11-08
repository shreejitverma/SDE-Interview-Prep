'''
https://leetcode.com/problems/next-permutation/
31. Next Permutation
Medium

7621

2522

Add to List

Share
Implement next permutation, which rearranges numbers into the lexicographically next greater permutation of numbers.

If such an arrangement is not possible, it must rearrange it as the lowest possible order (i.e., sorted in ascending order).

The replacement must be in place and use only constant extra memory.

 

Example 1:

Input: nums = [1,2,3]
Output: [1,3,2]
Example 2:

Input: nums = [3,2,1]
Output: [1,2,3]
Example 3:

Input: nums = [1,1,5]
Output: [1,5,1]
Example 4:

Input: nums = [1]
Output: [1]
 

Constraints:

1 <= nums.length <= 100
0 <= nums[i] <= 100'''

# Time:  O(n)
# Space: O(1)


class Solution(object):
    def nextPermutation(self, nums):
        """
        :type nums: List[int]
        :rtype: None Do not return anything, modify nums in-place instead.
        """
        k, l = -1, 0
        for i in reversed(xrange(len(nums)-1)):
            if nums[i] < nums[i+1]:
                k = i
                break
        else:
            nums.reverse()
            return

        for i in reversed(xrange(k+1, len(nums))):
            if nums[i] > nums[k]:
                l = i
                break
        nums[k], nums[l] = nums[l], nums[k]
        nums[k+1:] = nums[:k:-1]


# Time:  O(n)
# Space: O(1)
class Solution2(object):
    def nextPermutation(self, nums):
        """
        :type nums: List[int]
        :rtype: None Do not return anything, modify nums in-place instead.
        """
        k, l = -1, 0
        for i in xrange(len(nums)-1):
            if nums[i] < nums[i+1]:
                k = i

        if k == -1:
            nums.reverse()
            return

        for i in xrange(k+1, len(nums)):
            if nums[i] > nums[k]:
                l = i
        nums[k], nums[l] = nums[l], nums[k]
        nums[k+1:] = nums[:k:-1]


class Solution(object):
    def nextPermutation(self, nums):
        """
        :type nums: List[int]
        :rtype: void Do not return anything, modify nums in-place instead.
        """
        # http://fisherlei.blogspot.com/2012/12/leetcode-next-permutation.html
        i = j = len(nums)-1
        while i > 0 and nums[i] <= nums[i-1]:
            i -= 1
        i -= 1
        if i < 0:
            nums.reverse()
            return

        while j > i and nums[j] <= nums[i]:
            j -= 1

        nums[i], nums[j] = nums[j], nums[i]

        k, l = i+1, len(nums)-1

        while k < l:
            nums[k], nums[l] = nums[l], nums[k]
            k += 1
            l -= 1

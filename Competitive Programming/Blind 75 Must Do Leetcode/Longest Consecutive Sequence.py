'''https://leetcode.com/problems/longest-consecutive-sequence/
128. Longest Consecutive Sequence
Medium

7203

336

Add to List

Share
Given an unsorted array of integers nums, return the length of the longest consecutive elements sequence.

You must write an algorithm that runs in O(n) time.

 

Example 1:

Input: nums = [100,4,200,1,3,2]
Output: 4
Explanation: The longest consecutive elements sequence is [1, 2, 3, 4]. Therefore its length is 4.
Example 2:

Input: nums = [0,3,7,2,5,8,4,6,0,1]
Output: 9
 

Constraints:

0 <= nums.length <= 105
-109 <= nums[i] <= 109'''

#


class Solution:
    def longestConsecutive(self, nums: List[int]) -> int:
        num = set(nums)
        maxLen = 0
        while num:
            n = num.pop()
            i, l1, l2 = n+1, 0, 0
            while i in num:
                num.remove(i)
                i, l1 = i+1, l1+1
            i = n-1
            while i in num:
                num.remove(i)
                i, l2 = i-1, l2+1
            maxLen = max(maxLen, l1+l2+1)
        return maxLen

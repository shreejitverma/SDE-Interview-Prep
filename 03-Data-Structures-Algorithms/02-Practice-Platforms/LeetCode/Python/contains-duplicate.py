# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

# Time:  O(n)
# Space: O(n)

class Solution(object):
    # @param {integer[]} nums
    # @return {boolean}
    def containsDuplicate(self, nums):
        return len(nums) > len(set(nums))


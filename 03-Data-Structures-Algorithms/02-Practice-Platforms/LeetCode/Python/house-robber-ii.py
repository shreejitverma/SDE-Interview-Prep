# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N)
# Space: O(1)

from typing import List


class Solution:
    def rob(self, nums: List[int]) -> int:
        if not nums:
            return 0
        if len(nums) == 1:
            return nums[0]

        return max(self._rob_range(nums, 0, len(nums) - 1),
                   self._rob_range(nums, 1, len(nums)))

    def _rob_range(self, nums: List[int], start: int, end: int) -> int:
        prev2, prev1 = 0, 0
        for i in range(start, end):
            prev2, prev1 = prev1, max(prev1, prev2 + nums[i])
        return prev1

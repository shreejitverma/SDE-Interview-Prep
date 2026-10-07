# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(N * N!)
# Space: O(N) auxiliary (recursion stack depth)

from typing import List


class Solution:
    def permute(self, nums: List[int]) -> List[List[int]]:
        result: List[List[int]] = []

        def backtrack(first: int) -> None:
            if first == len(nums):
                result.append(list(nums))
                return

            for i in range(first, len(nums)):
                nums[first], nums[i] = nums[i], nums[first]
                backtrack(first + 1)
                nums[first], nums[i] = nums[i], nums[first]

        backtrack(0)
        return result

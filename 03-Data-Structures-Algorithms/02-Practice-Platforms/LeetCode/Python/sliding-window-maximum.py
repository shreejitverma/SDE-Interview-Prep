"""
Problem: LeetCode 239 - Sliding Window Maximum
Difficulty: Hard
Concepts: Monotonic Queue, Sliding Window, Deque

Time Complexity: O(n)
Space Complexity: O(k)
"""

from collections import deque
from typing import List


class Solution:
    def maxSlidingWindow(self, nums: List[int], k: int) -> List[int]:
        dq: deque[int] = deque()  # Indices in strictly decreasing value order
        result: List[int] = []

        for i, num in enumerate(nums):
            # Remove indices outside the window
            if dq and dq[0] <= i - k:
                dq.popleft()

            # Maintain monotonic decreasing order
            while dq and nums[dq[-1]] <= num:
                dq.pop()

            dq.append(i)

            if i >= k - 1:
                result.append(nums[dq[0]])

        return result

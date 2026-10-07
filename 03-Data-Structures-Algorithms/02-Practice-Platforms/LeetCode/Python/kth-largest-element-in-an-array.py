"""
Problem: LeetCode 215 - Kth Largest Element in an Array
Difficulty: Medium
Concepts: QuickSelect, Min-Heap, Divide and Conquer

Approach 1 (QuickSelect with 3-way partition):
Time Complexity: O(n) average, O(n^2) worst-case
Space Complexity: O(1) iterative auxiliary

Approach 2 (Min-Heap):
Time Complexity: O(n log k)
Space Complexity: O(k)
"""

import heapq
import random
from typing import List


class Solution:
    def findKthLargest(self, nums: List[int], k: int) -> int:
        target_idx = len(nums) - k
        left, right = 0, len(nums) - 1

        while left <= right:
            pivot_idx = random.randint(left, right)
            pivot_val = nums[pivot_idx]

            # 3-way Dutch National Flag partition
            lt, gt = left, right
            i = left

            while i <= gt:
                if nums[i] < pivot_val:
                    nums[i], nums[lt] = nums[lt], nums[i]
                    i += 1
                    lt += 1
                elif nums[i] > pivot_val:
                    nums[i], nums[gt] = nums[gt], nums[i]
                    gt -= 1
                else:
                    i += 1

            if lt <= target_idx <= gt:
                return nums[target_idx]
            elif target_idx < lt:
                right = lt - 1
            else:
                left = gt + 1

        return nums[target_idx]


class SolutionMinHeap:
    def findKthLargest(self, nums: List[int], k: int) -> int:
        heap: List[int] = []
        for num in nums:
            heapq.heappush(heap, num)
            if len(heap) > k:
                heapq.heappop(heap)
        return heap[0]

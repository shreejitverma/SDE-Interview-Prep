"""
Problem: LeetCode 134 - Gas Station
Difficulty: Medium
Concepts: Greedy, Array

Time Complexity: O(n)
Space Complexity: O(1)
"""

from typing import List


class Solution:
    def canCompleteCircuit(self, gas: List[int], cost: List[int]) -> int:
        total_tank = 0
        current_tank = 0
        start_index = 0

        for i in range(len(gas)):
            balance = gas[i] - cost[i]
            total_tank += balance
            current_tank += balance

            if current_tank < 0:
                start_index = i + 1
                current_tank = 0

        return start_index if total_tank >= 0 else -1

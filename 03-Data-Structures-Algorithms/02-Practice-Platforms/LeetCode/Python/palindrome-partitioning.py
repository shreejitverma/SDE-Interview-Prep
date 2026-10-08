"""
Problem: LeetCode 131 - Palindrome Partitioning
Difficulty: Medium
Concepts: Backtracking, String, Dynamic Programming

Time Complexity: O(n * 2^n)
Space Complexity: O(n) recursion stack
"""

from typing import List


class Solution:
    def partition(self, s: str) -> List[List[str]]:
        result: List[List[str]] = []
        current: List[str] = []

        def is_palindrome(sub: str) -> bool:
            return sub == sub[::-1]

        def backtrack(start: int) -> None:
            if start == len(s):
                result.append(list(current))
                return

            for end in range(start + 1, len(s) + 1):
                piece = s[start:end]
                if is_palindrome(piece):
                    current.append(piece)
                    backtrack(end)
                    current.pop()

        backtrack(0)
        return result

"""
Problem: LeetCode 150 - Evaluate Reverse Polish Notation
Difficulty: Medium
Concepts: Stack, Math, Array

Time Complexity: O(n)
Space Complexity: O(n)
"""

from typing import List


class Solution:
    def evalRPN(self, tokens: List[str]) -> int:
        stack: List[int] = []

        for token in tokens:
            if token in {"+", "-", "*", "/"}:
                b = stack.pop()
                a = stack.pop()
                if token == "+":
                    stack.append(a + b)
                elif token == "-":
                    stack.append(a - b)
                elif token == "*":
                    stack.append(a * b)
                else:
                    # Truncate division toward zero
                    stack.append(int(a / b))
            else:
                stack.append(int(token))

        return stack[-1]

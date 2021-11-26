'''
https://practice.geeksforgeeks.org/problems/evaluation-of-postfix-expression1735/1

Evaluation of Postfix Expression 
Easy Accuracy: 53.28% Submissions: 26054 Points: 2
Given string S representing a postfix expression, the task is to evaluate the expression and find the final value. Operators will only include the basic arithmetic operators like *, /, + and -.

 

Example 1:

Input: S = "231*+9-"
Output: -4
Explanation:
After solving the given expression, 
we have -4 as result.
Example 2:

Input: S = "123+*8-"
Output: -3
Explanation:
After solving the given postfix 
expression, we have -3 as result.

Your Task:
You do not need to read input or print anything. Complete the function evaluatePostfixExpression() that takes the string S denoting the expression as input parameter and returns the evaluated value.


Expected Time Complexity: O(|S|)
Expected Auixilliary Space: O(|S|)


Constraints:
1 ≤ |S| ≤ 105

0 ≤ |Si|≤ 9 (And given operators)'''
# User function Template for python3

import atexit
import sys
import io


class Solution:

    # Function to evaluate a postfix expression.
    def evaluatePostfix(self, S):
        # code here
        stack = []
        for i in S:
            if i == '*' or i == '/' or i == '+' or i == '-':
                val1 = stack.pop()
                val2 = stack.pop()
                if i == '*':
                    stack.append(int(val2) * int(val1))
                elif i == '/':
                    stack.append(int(val2) // int(val1))
                elif i == '+':
                    stack.append(int(val2) + int(val1))
                elif i == '-':
                    stack.append(int(val2) - int(val1))
            else:
                stack.append(i)
        return stack.pop()

# {
#  Driver Code Starts
# Initial Template for Python 3


# This code is contributed by Nikhil Kumar Singh(nickzuck_007)
_INPUT_LINES = sys.stdin.read().splitlines()
input = iter(_INPUT_LINES).__next__
_OUTPUT_BUFFER = io.StringIO()
sys.stdout = _OUTPUT_BUFFER


@atexit.register
def write():
    sys.__stdout__.write(_OUTPUT_BUFFER.getvalue())


if __name__ == '__main__':
    test_cases = int(input())
    for cases in range(test_cases):
        S = input()
        obj = Solution()
        print('{0:g}'.format(float(obj.evaluatePostfix(S))))
# } Driver Code Ends

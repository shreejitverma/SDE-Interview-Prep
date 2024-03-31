'''https://practice.geeksforgeeks.org/problems/parenthesis-checker2744/1

Parenthesis Checker 
Easy Accuracy: 49.12% Submissions: 100k+ Points: 2
Given an expression string x. Examine whether the pairs and the orders of “{“,”}”,”(“,”)”,”[“,”]” are correct in exp.
For example, the function should return 'true' for exp = “[()]{}{[()()]()}” and 'false' for exp = “[(])”.

Example 1:

Input:
{([])}
Output: 
true
Explanation: 
{ ( [ ] ) }. Same colored brackets can form 
balaced pairs, with 0 number of 
unbalanced bracket.
Example 2:

Input: 
()
Output: 
true
Explanation: 
(). Same bracket can form balanced pairs, 
and here only 1 type of bracket is 
present and in balanced way.
Example 3:

Input: 
([]
Output: 
false
Explanation: 
([]. Here square bracket is balanced but 
the small bracket is not balanced and 
Hence , the output will be unbalanced.
Your Task:
This is a function problem. You only need to complete the function ispar() that takes a string as a parameter and returns a boolean value true if brackets are balanced else returns false. The printing is done automatically by the driver code.

Expected Time Complexity: O(|x|)
Expected Auixilliary Space: O(|x|)

Constraints:
1 ≤ |x| ≤ 32000'''

# User function Template for python3

import atexit
import re
import sys
import io


class Solution:

    # Function to check if brackets are balanced or not.
    def ispar(self, expr):
        # code here
        stack = []

        # Traversing the Expression
        for char in expr:
            if char in ["(", "{", "["]:

                # Push the element in the stack
                stack.append(char)
            else:

                # IF current character is not opening
                # bracket, then it must be closing.
                # So stack cannot be empty at this point.
                if not stack:
                    return False
                current_char = stack.pop()
                if current_char == '(':
                    if char != ")":
                        return False
                if current_char == '{':
                    if char != "}":
                        return False
                if current_char == '[':
                    if char != "]":
                        return False

        # Check Empty Stack
        if stack:
            return False
        return True

# {
#  Driver Code Starts
# Initial Template for Python 3


# Contributed by : Nagendra Jha

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
        #n = int(input())
        #n,k = map(int,imput().strip().split())
        #a = list(map(int,input().strip().split()))
        s = str(input())
        obj = Solution()
        if obj.ispar(s):
            print("balanced")
        else:
            print("not balanced")
# } Driver Code Ends
# A small check for balanced parantheses in Python 3
# Added regex as a little extra


any_parantheses = re.compile("[\[({})\]]+")
opening_parantheses = ['[', '(', '{']
closing_parantheses = [']', ')', '}']


def is_balanced(expr):
    stack = []
    n = len(expr)
    for i in range(0, n):
        if (expr[i] in opening_parantheses):
            stack.append(expr[i])
        elif (expr[i] in closing_parantheses):
            matching_paranthesis = opening_parantheses[closing_parantheses.
                                                       index(expr[i])]
            if (stack.pop() != matching_paranthesis):
                return False

    return True if len(stack) == 0 else False


def main():
    expr = input(
        "Please enter an expression with correct parantheses ('[','(','{','}',')',']') or 'test' for a fixed test: "
    )
    if (expr == "test"):
        expr = "({}[](([{()}])))"

    if (not any_parantheses.match(expr)):
        print("No parantheses found! Please enter correct one!")
    elif (is_balanced(expr)):
        print("%s is balanced!" % expr)
    else:
        print("%s is not balanced!" % expr)


if __name__ == '__main__':
    main()

'''https://leetcode.com/problems/valid-parentheses/
20. Valid Parentheses
Easy

9509

379

Add to List

Share
Given a string s containing just the characters '(', ')', '{', '}', '[' and ']', determine if the input string is valid.

An input string is valid if:

Open brackets must be closed by the same type of brackets.
Open brackets must be closed in the correct order.
 

Example 1:

Input: s = "()"
Output: true
Example 2:

Input: s = "()[]{}"
Output: true
Example 3:

Input: s = "(]"
Output: false
Example 4:

Input: s = "([)]"
Output: false
Example 5:

Input: s = "{[]}"
Output: true
 

Constraints:

1 <= s.length <= 104
s consists of parentheses only '()[]{}'.'''


class Solution:
    def isValid(self, s: str) -> bool:
        dic = {')': '(', ']': '[', '}': '{'}
        tmp = []
        for item in s:
            if item in dic.values():
                tmp.append(item)
            elif item in dic.keys():
                if len(tmp) == 0 or dic[item] != tmp[-1]:
                    return False
                else:
                    tmp.pop(-1)
        return True if len(tmp) == 0 else False


if __name__ == "__main__":
    s = Solution()
    print(s.isValid(']'))

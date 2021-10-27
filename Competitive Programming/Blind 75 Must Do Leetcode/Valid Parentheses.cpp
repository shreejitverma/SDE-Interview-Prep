/*
https://leetcode.com/problems/valid-parentheses/
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
s consists of parentheses only '()[]{}'.*/

//Runtime: 4 ms, faster than 100.00% of C++ online submissions for Valid Parentheses.
//Memory Usage: 8.5 MB, less than 99.87% of C++ online submissions for Valid Parentheses.
//time complexity: O(n), space complexity: O(n)

class Solution
{
public:
    bool isValid(string s)
    {
        map<char, char> paren;
        paren['('] = ')';
        paren['{'] = '}';
        paren['['] = ']';

        set<char> keys = {'(', '{', '['};

        stack<char> stk;

        for (char c : s)
        {
            if (keys.find(c) != keys.end())
            { //left
                stk.push(c);
            }
            else if (!stk.empty() && paren[stk.top()] == c)
            { //right, match
                stk.pop();
            }
            else
            { //right, not match
                return false;
            }
        }

        return stk.empty();
    }
};
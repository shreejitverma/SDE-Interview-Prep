/*
https://leetcode.com/problems/remove-invalid-parentheses/
https://github.com/Seanforfun/Algorithm-and-Leetcode/blob/master/leetcode/301.%20Remove%20Invalid%20Parentheses.md
301. Remove Invalid Parentheses
Hard

4057

199

Add to List

Share
Given a string s that contains parentheses and letters, remove the minimum number of invalid parentheses to make the input string valid.

Return all the possible results. You may return the answer in any order.

 

Example 1:

Input: s = "()())()"
Output: ["(())()","()()()"]
Example 2:

Input: s = "(a)())()"
Output: ["(a())()","(a)()()"]
Example 3:

Input: s = ")("
Output: [""]
 

Constraints:

1 <= s.length <= 25
s consists of lowercase English letters and parentheses '(' and ')'.
There will be at most 20 parentheses in s.
*/

class Solution
{
public:
    vector<string> removeInvalidParentheses(string s)
    {
        vector<string> res;
        backtracking(res, s, 0, 0, false);
        return res;
    }

private:
    void backtracking(vector<string> &res, string s, int start, int lastD, bool rev)
    {
        int count = 0;
        for (int i = start; i < s.size(); ++i)
        {
            if (s[i] == '(')
                ++count;
            if (s[i] == ')')
                --count;
            if (count < 0)
            {
                for (int j = lastD; j <= i; ++j)
                {
                    if (s[j] == ')' && (!j || s[j - 1] != ')'))
                        backtracking(res, s.substr(0, j) + s.substr(j + 1), i, j, rev);
                }
                return;
            }
        }
        if (!rev)
            backtracking(res, mirror(s), 0, 0, true);
        else
            res.push_back(mirror(s));
    }

    string mirror(string s)
    {
        string res;
        for (int i = s.size() - 1; i >= 0; --i)
        {
            res += s[i] == ')' ? '(' : (s[i] == '(' ? ')' : s[i]);
        }
        return res;
    }
};
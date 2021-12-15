/*
https://leetcode.com/problems/longest-substring-without-repeating-characters/
3. Longest Substring Without Repeating Characters
Medium

19505

891

Add to List

Share
Given a string s, find the length of the longest substring without repeating characters.



Example 1:

Input: s = "abcabcbb"
Output: 3
Explanation: The answer is "abc", with the length of 3.
Example 2:

Input: s = "bbbbb"
Output: 1
Explanation: The answer is "b", with the length of 1.
Example 3:

Input: s = "pwwkew"
Output: 3
Explanation: The answer is "wke", with the length of 3.
Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.
Example 4:

Input: s = ""
Output: 0


Constraints:

0 <= s.length <= 5 * 104
s consists of English letters, digits, symbols and spaces.
*/

class Solution
{
public:
    int lengthOfLongestSubstring(string s)
    {

        int n = s.size();
        map<char, int> position;
        int ans = 0;

        for (int i = 0, j = 0; j < n; j++)
        {
            if (position.find(s[j]) != position.end())
            {
                i = max(i, position[s[j]] + 1);
            }
            ans = max(ans, j - i + 1);
            position[s[j]] = j;
        }

        return ans;
    }
};
class Solution
{
public:
    int lengthOfLongestSubstring(string s)
    {
        vector<int> chars(128);

        int left = 0;
        int right = 0;

        int res = 0;
        while (right < s.length())
        {
            char r = s[right];
            chars[r]++;

            while (chars[r] > 1)
            {
                char l = s[left];
                chars[l]--;
                left++;
            }

            res = max(res, right - left + 1);

            right++;
        }

        return res;
    }
};
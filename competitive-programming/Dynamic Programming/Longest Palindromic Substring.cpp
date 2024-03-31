/*
https://leetcode.com/problems/longest-palindromic-substring/
5. Longest Palindromic Substring
Medium

14140

834

Add to List

Share
Given a string s, return the longest palindromic substring in s.

 

Example 1:

Input: s = "babad"
Output: "bab"
Note: "aba" is also a valid answer.
Example 2:

Input: s = "cbbd"
Output: "bb"
Example 3:

Input: s = "a"
Output: "a"
Example 4:

Input: s = "ac"
Output: "a"
 

Constraints:

1 <= s.length <= 1000
s consist of only digits and English letters.
*/

class Solution
{
public:
    string longestPalindrome(string s)
    {
        int max_len = 0, len = 0, len1 = 0, len2 = 0;
        int left = 0, right = 0;
        for (int i = 0; i < s.length(); i++)
        {
            len1 = palindrome(s, i, i);
            len2 = palindrome(s, i, i + 1);
            len = max(len1, len2);
            if (len > max_len)
            {
                max_len = len;
                left = i - (len - 1) / 2;
                right = i + len / 2;
            }
        }
        return s.substr(left, max_len);
    }

    int palindrome(string s, int left, int right)
    {
        while (left >= 0 && right < s.length() && s[left] == s[right])
        {
            left--;
            right++;
        }
        return right - left - 1;
    }
};
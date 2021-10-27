'''https://leetcode.com/problems/valid-anagram/
242. Valid Anagram
Easy

3456

188

Add to List

Share
Given two strings s and t, return true if t is an anagram of s, and false otherwise.

 

Example 1:

Input: s = "anagram", t = "nagaram"
Output: true
Example 2:

Input: s = "rat", t = "car"
Output: false
 

Constraints:

1 <= s.length, t.length <= 5 * 104
s and t consist of lowercase English letters.'''


class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        # return sorted(s) == sorted(t)

        tmp = [0]*26
        if len(s) != len(t):
            return False

        for c1, c2 in zip(s, t):
            tmp[ord(c1)-ord('a')] += 1
            tmp[ord(c2)-ord('a')] -= 1
        for i in tmp:
            if i != 0:
                return False
        return True

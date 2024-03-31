'''https://leetcode.com/problems/longest-common-subsequence/
1143. Longest Common Subsequence
Medium

4663

55

Add to List

Share
Given two strings text1 and text2, return the length of their longest common subsequence. If there is no common subsequence, return 0.

A subsequence of a string is a new string generated from the original string with some characters (can be none) deleted without changing the relative order of the remaining characters.

For example, "ace" is a subsequence of "abcde".
A common subsequence of two strings is a subsequence that is common to both strings.

 

Example 1:

Input: text1 = "abcde", text2 = "ace" 
Output: 3  
Explanation: The longest common subsequence is "ace" and its length is 3.
Example 2:

Input: text1 = "abc", text2 = "abc"
Output: 3
Explanation: The longest common subsequence is "abc" and its length is 3.
Example 3:

Input: text1 = "abc", text2 = "def"
Output: 0
Explanation: There is no such common subsequence, so the result is 0.
 

Constraints:

1 <= text1.length, text2.length <= 1000
text1 and text2 consist of only lowercase English characters.'''

# Time:  O(m * n)
# Space: O(min(m, n))


class Solution(object):
    def longestCommonSubsequence(self, text1, text2):
        """
        :type text1: str
        :type text2: str
        :rtype: int
        """
        if len(text1) < len(text2):
            return self.longestCommonSubsequence(text2, text1)

        dp = [[0 for _ in range(len(text2)+1)] for _ in range(2)]
        for i in range(1, len(text1)+1):
            for j in range(1, len(text2)+1):
                dp[i % 2][j] = dp[(i-1) % 2][j-1]+1 if text1[i-1] == text2[j-1] \
                    else max(dp[(i-1) % 2][j], dp[i % 2][j-1])
        return dp[len(text1) % 2][len(text2)]

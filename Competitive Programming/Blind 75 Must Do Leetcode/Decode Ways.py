'''https://leetcode.com/problems/decode-ways/
91. Decode Ways
Medium

5626

3617

Add to List

Share
A message containing letters from A-Z can be encoded into numbers using the following mapping:

'A' -> "1"
'B' -> "2"
...
'Z' -> "26"
To decode an encoded message, all the digits must be grouped then mapped back into letters using the reverse of the mapping above (there may be multiple ways). For example, "11106" can be mapped into:

"AAJF" with the grouping (1 1 10 6)
"KJF" with the grouping (11 10 6)
Note that the grouping (1 11 06) is invalid because "06" cannot be mapped into 'F' since "6" is different from "06".

Given a string s containing only digits, return the number of ways to decode it.

The answer is guaranteed to fit in a 32-bit integer.

 

Example 1:

Input: s = "12"
Output: 2
Explanation: "12" could be decoded as "AB" (1 2) or "L" (12).
Example 2:

Input: s = "226"
Output: 3
Explanation: "226" could be decoded as "BZ" (2 26), "VF" (22 6), or "BBF" (2 2 6).
Example 3:

Input: s = "0"
Output: 0
Explanation: There is no character that is mapped to a number starting with 0.
The only valid mappings with 0 are 'J' -> "10" and 'T' -> "20", neither of which start with 0.
Hence, there are no valid ways to decode this since all digits need to be mapped.
Example 4:

Input: s = "06"
Output: 0
Explanation: "06" cannot be mapped to "F" because of the leading zero ("6" is different from "06").
 

Constraints:

1 <= s.length <= 100
s contains only digits and may contain leading zero(s).'''

#


class Solution:
    def numDecodings(self, s: str) -> int:
        return self.climbStairs(s)

    def climbStairs(self, s):
        if not s or s[0] == '0':
            return 0
        if len(s) == 1:
            return len(s)
        a, b, n = 1, 1, len(s)

        for i in range(1, n):
            if s[i] == '0':
                a = 0
            if 9 < int(s[i-1:i+1]) <= 26:
                a = a+b
                b = a-b
            else:
                b = a
        return a

    # T L E
    def numDecodings1(self, s):
        if not s:
            return 0
        if s[0] == '0':
            return 0

        if len(s) >= 2 and 0 < int(s[:2]) < 27:
            if s[0] != '0':
                return self.numDecodings1(s[1:]) + self.numDecodings1(s[2:])
            else:
                return self.numDecodings1(s[2:])
        else:
            return self.numDecodings1(s[1:])

    def numDecodings2(self, s):
        v, w, p = 0, int(s > ''), ''
        for d in s:
            v, w, p = w, (d > '0')*w + (9 < int(p+d) < 27)*v, d
        return w

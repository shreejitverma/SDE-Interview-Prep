/*
https://leetcode.com/problems/decode-ways/
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
s contains only digits and may contain leading zero(s).
*/

//dfs with memorization, add memorization to avoid TLE
//Runtime: 0 ms, faster than 100.00% of C++ online submissions for Decode Ways.
//Memory Usage: 6.4 MB, less than 38.81% of C++ online submissions for Decode Ways.
class Solution
{
public:
    vector<int> memo;

    int backtrack(string &s, int start)
    {
        if (start == s.size())
        {
            return 1;
        }
        else if (memo[start] != -1)
        {
            return memo[start];
        }
        else
        {
            int ret = 0;
            if (s[start] != '0')
            {
                ret += backtrack(s, start + 1);

                if (start + 1 < s.size() &&
                    (s[start] == '1' || (s[start] == '2' && s[start + 1] <= '6')))
                {
                    //if there remains >= 2 chars
                    ret += backtrack(s, start + 2);
                }
            }

            return memo[start] = ret;
        }
    }

    int numDecodings(string s)
    {
        memo = vector<int>(s.size(), -1);
        return backtrack(s, 0);
    }
};

//bottom-up DP
//Runtime: 0 ms, faster than 100.00% of C++ online submissions for Decode Ways.
//Memory Usage: 6.4 MB, less than 44.21% of C++ online submissions for Decode Ways.
class Solution
{
public:
    int numDecodings(string s)
    {
        int n = s.size();

        vector<int> dp(n + 1, 0);

        dp[n] = 1;

        for (int i = n - 1; i >= 0; --i)
        {
            if (s[i] != '0')
            {
                dp[i] += dp[i + 1];

                if (i + 1 < n &&
                    (s[i] == '1' || (s[i] == '2' && s[i + 1] <= '6')))
                {
                    dp[i] += dp[i + 2];
                }
            }
        }

        return dp[0];
    }
};
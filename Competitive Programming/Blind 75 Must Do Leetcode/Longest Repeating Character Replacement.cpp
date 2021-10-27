/*
https://leetcode.com/problems/longest-repeating-character-replacement/
424. Longest Repeating Character Replacement
Medium

3255

131

Add to List

Share
You are given a string s and an integer k. You can choose any character of the string and change it to any other uppercase English character. You can perform this operation at most k times.

Return the length of the longest substring containing the same letter you can get after performing the above operations.

 

Example 1:

Input: s = "ABAB", k = 2
Output: 4
Explanation: Replace the two 'A's with two 'B's or vice versa.
Example 2:

Input: s = "AABABBA", k = 1
Output: 4
Explanation: Replace the one 'A' in the middle with 'B' and form "AABBBBA".
The substring "BBBB" has the longest repeating letters, which is 4.
 

Constraints:

1 <= s.length <= 105
s consists of only uppercase English letters.
0 <= k <= s.length
*/

//Runtime: 16 ms, faster than 53.88% of C++ online submissions for Longest Repeating Character Replacement.
//Memory Usage: 8.7 MB, less than 100.00% of C++ online submissions for Longest Repeating Character Replacement.

class Solution
{
public:
    int characterReplacement(string s, int k)
    {
        int N = s.size();
        int ans = 0;

        // for(int i = 0; i < 26; i++){
        //     lengths[i+'A'] = 0;
        //     used[i+'A'] = 0;
        // }

        for (int i = 0; i < 26; i++)
        {
            int length = 0;
            int used = 0;
            int left = 0, right = 0;
            char c = 'A' + i;

            //skip the character not in s
            if (s.find(c) == string::npos)
                continue;
            // cout << c << endl;

            while (right < N)
            {
                if (left > 0 && s[left - 1] != c)
                {
                    used--;
                }
                // cout << "[" << left << ", " << right << "), used: " << used << endl;
                while (right < N && (s[right] == c || used < k))
                {
                    if (s[right] != c)
                    {
                        used++;
                    }
                    right++;
                    // cout << "[" << left << ", " << right << "), used: " << used << endl;
                }

                length = max(length, right - left);
                // cout << "[" << left << ", " << right << "), length: " << length << ", used: " << used << endl;

                left++;
            }

            ans = max(length, ans);

            // cout << "ans: " << ans << endl;
        }

        return ans;
    }
};
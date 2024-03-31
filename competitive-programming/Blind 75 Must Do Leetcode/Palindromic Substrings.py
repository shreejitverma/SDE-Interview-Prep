'''https://leetcode.com/problems/palindromic-substrings/
647. Palindromic Substrings
Medium

5258

144

Add to List

Share
Given a string s, return the number of palindromic substrings in it.

A string is a palindrome when it reads the same backward as forward.

A substring is a contiguous sequence of characters within the string.

 

Example 1:

Input: s = "abc"
Output: 3
Explanation: Three palindromic strings: "a", "b", "c".
Example 2:

Input: s = "aaa"
Output: 6
Explanation: Six palindromic strings: "a", "a", "a", "aa", "aa", "aaa".
 

Constraints:

1 <= s.length <= 1000
s consists of lowercase English letters.'''

"""
Given a string, your task is to count how many palindromic substrings in this string.
The substrings with different start indexes or end indexes are counted as different substrings even they consist of
same characters.
Example 1:
Input: "abc"
Output: 3
Explanation: Three palindromic strings: "a", "b", "c".
Example 2:
Input: "aaa"
Output: 6
Explanation: Six palindromic strings: "a", "a", "a", "aa", "aa", "aaa".
Note:
The input string length won't exceed 1000.
"""


class Solution:
    def countSubstrings(self, s):
        """
        :type s: str
        :rtype: int
        """

        if len(s) == 0:
            return 0
        elif len(s) == 1:
            return 1

        # palindromes = []
        num_palindromes = 0

        for middle in range(len(s)):
            left = middle
            right = middle

            while left >= 0 and right < len(s):
                if s[left] == s[right]:
                    # palindromes.append(s[left:right + 1])
                    num_palindromes += 1
                else:
                    break

                left -= 1
                right += 1

        for middle_left in range(len(s)):
            left = middle_left
            right = middle_left + 1

            while left >= 0 and right < len(s):
                if s[left] == s[right]:
                    # palindromes.append(s[left:right + 1])
                    num_palindromes += 1
                else:
                    break

                left -= 1
                right += 1

        # print(palindromes)
        # return len(palindromes)
        return num_palindromes


def main():
    sol = Solution()

    print(sol.countSubstrings("abc"))
    print(sol.countSubstrings("aaa"))
    print(sol.countSubstrings("abcdcba"))


if __name__ == '__main__':
    main()

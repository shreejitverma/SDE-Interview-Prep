'''https://leetcode.com/problems/valid-palindrome/
125. Valid Palindrome
Easy

2557

4442

Add to List

Share
Given a string s, determine if it is a palindrome, considering only alphanumeric characters and ignoring cases.

 

Example 1:

Input: s = "A man, a plan, a canal: Panama"
Output: true
Explanation: "amanaplanacanalpanama" is a palindrome.
Example 2:

Input: s = "race a car"
Output: false
Explanation: "raceacar" is not a palindrome.
 

Constraints:

1 <= s.length <= 2 * 105
s consists only of printable ASCII characters.'''


class Solution:
    def isPalindrome(self, s: str) -> bool:
        l, r = 0, len(s)-1

        while l <= r:
            if not s[l].isalpha() and not s[l].isdigit():
                l += 1
                continue
            if not s[r].isalpha() and not s[r].isdigit():
                r -= 1
                continue
            if s[l].lower() == s[r].lower():
                l, r = l+1, r-1
                continue
            else:
                return False
        return True


if __name__ == "__main__":
    s = Solution()
    res = s.isPalindrome("A man, a plan, a canal: Panama")
    print(res)

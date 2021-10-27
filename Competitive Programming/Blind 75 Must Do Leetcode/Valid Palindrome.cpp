/*
https://leetcode.com/problems/valid-palindrome/
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
s consists only of printable ASCII characters.*/

//Runtime: 12 ms, faster than 78.77% of C++ online submissions for Valid Palindrome.
//Memory Usage: 9.3 MB, less than 24.05% of C++ online submissions for Valid Palindrome.
class Solution
{
public:
    bool isPalindrome(string s)
    {
        int i = 0, j = s.size() - 1;
        while (i < j)
        {
            while (i < s.size() && !isalnum(s[i]))
                i++;
            while (j >= 0 && !isalnum(s[j]))
                j--;
            // cout << i << " " << j << endl;
            //we have scanned the whole string
            if (i >= j)
                break;
            //slower
            // if(i >= s.size() || j < 0) break;
            if (tolower(s[i]) != tolower(s[j]))
                return false;
            i++;
            j--;
        }
        return true;
    }
};

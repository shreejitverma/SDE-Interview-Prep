'''https://practice.geeksforgeeks.org/problems/longest-palindrome-in-a-string3411/1
Longest Palindrome in a String 
Medium Accuracy: 49.2% Submissions: 41302 Points: 4
Given a string S, find the longest palindromic substring in S. Substring of string S: S[ i . . . . j ] where 0 ≤ i ≤ j < len(S). Palindrome string: A string which reads the same backwards. More formally, S is palindrome if reverse(S) = S. Incase of conflict, return the substring which occurs first ( with the least starting index).


Example 1:

Input:
S = "aaaabbaa"
Output: aabbaa
Explanation: The longest Palindromic
substring is "aabbaa".
Example 2:

Input: 
S = "abc"
Output: a
Explanation: "a", "b" and "c" are the 
longest palindromes with same length.
The result is the one with the least
starting index.

Your Task:
You don't need to read input or print anything. Your task is to complete the function longestPalin() which takes the string S as input and returns the longest palindromic substring of S.


Expected Time Complexity: O(|S|2).
Expected Auxiliary Space: O(1).


Constraints:
1 ≤ |S| ≤ 103'''
# User function Template for python3


class Solution:
    def longestPalin(self, S):
        # code here
        res = ""
        resLen = 0
        for i in range(len(S)):

            # Odd Length
            left, right = i, i
            while left >= 0 and right < len(S) and S[left] == S[right]:
                if(right-left+1 > resLen):
                    resLen = right-left+1
                    res = S[left:right+1]
                left -= 1
                right += 1
            # EvenLength
            left = i
            right = i+1
            while left >= 0 and right < len(S) and S[left] == S[right]:
                if(right-left+1 > resLen):
                    resLen = right-left+1
                    res = S[left:right+1]
                left -= 1
                right += 1
        return res

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == '__main__':

    t = int(input())

    for _ in range(t):
        S = input()

        ob = Solution()

        ans = ob.longestPalin(S)

        print(ans)
# } Driver Code Ends

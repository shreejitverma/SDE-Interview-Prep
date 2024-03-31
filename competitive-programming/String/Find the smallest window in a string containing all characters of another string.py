'''https://practice.geeksforgeeks.org/problems/smallest-window-in-a-string-containing-all-the-characters-of-another-string-1587115621/1

Smallest window in a string containing all the characters of another string 
Medium Accuracy: 42.59% Submissions: 40283 Points: 4
Given two strings S and P. Find the smallest window in the string S consisting of all the characters(including duplicates) of the string P.  Return "-1" in case there is no such window present. In case there are multiple such windows of same length, return the one with the least starting index. 

Example 1:

Input:
S = "timetopractice"
P = "toc"
Output: 
toprac
Explanation: "toprac" is the smallest
substring in which "toc" can be found.
Example 2:

Input:
S = "zoomlazapzo"
P = "oza"
Output: 
apzo
Explanation: "apzo" is the smallest 
substring in which "oza" can be found.
Your Task:
You don't need to read input or print anything. Your task is to complete the function smallestWindow() which takes two string S and P as input paramters and returns the smallest window in string S having all the characters of the string P. In case there are multiple such windows of same length, return the one with the least starting index. 

Expected Time Complexity: O(|S|)
Expected Auxiliary Space: O(1)

Constraints: 
1 ≤ |S|, |P| ≤ 105'''
# User function Template for python3


import atexit
import sys
import io


class Solution:

    # Function to find the smallest window in the string s consisting
    # of all the characters of string p.
    def smallestWindow(self, s, p):
        # code here
        len1 = len(s)
        len2 = len(p)
        if len1 < len2:
            return - 1
        char = 256
        hash_s = [0]*256
        hash_p = [0]*256
        for i in p:
            hash_p[ord(i)] += 1
        count = 0
        start = 0
        si = 0
        ei = 0
        minsubs = -1
        for i in range(0, len1):
            hash_s[ord(s[i])] += 1
            if hash_s[ord(s[i])] <= hash_p[ord(s[i])]:
                count += 1
            if count == len2:

                while(hash_s[ord(s[start])] > hash_p[ord(s[start])]):

                    hash_s[ord(s[start])] -= 1
                    start += 1
                window = i-start+1
                if window < minsubs or minsubs == -1:
                    minsubs = window
                    si = start
                    ei = i
        if minsubs == -1:
            return -1
        else:
            return s[si:ei+1]

# {
#  Driver Code Starts
# Initial Template for Python 3


_INPUT_LINES = sys.stdin.read().splitlines()
input = iter(_INPUT_LINES).__next__
_OUTPUT_BUFFER = io.StringIO()
sys.stdout = _OUTPUT_BUFFER


@atexit.register
def write():
    sys.__stdout__.write(_OUTPUT_BUFFER.getvalue())


if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        s = str(input())
        p = str(input())
        ob = Solution()
        print(ob.smallestWindow(s, p))
# } Driver Code Ends

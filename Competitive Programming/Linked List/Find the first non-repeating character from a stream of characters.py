'''https://practice.geeksforgeeks.org/problems/first-non-repeating-character-in-a-stream1216/1
First non-repeating character in a stream 
Medium Accuracy: 51.34% Submissions: 30458 Points: 4
Given an input stream of A of n characters consisting only of lower case alphabets. The task is to find the first non repeating character, each time a character is inserted to the stream. If there is no such character then append '#' to the answer.
 

Example 1:

Input: A = "aabc"
Output: "a#bb"
Explanation: For every character first non
repeating character is as follow-
"a" - first non-repeating character is 'a'
"aa" - no non-repeating character so '#'
"aab" - first non-repeating character is 'b'
"aabc" - first non-repeating character is 'b'
Example 2:

Input: A = "zz"
Output: "z#"
Explanation: For every character first non
repeating character is as follow-
"z" - first non-repeating character is 'z'
"zz" - no non-repeating character so '#'
 

Your Task:
You don't need to read or print anything. Your task is to complete the function FirstNonRepeating() which takes A as input parameter and returns a string after processing the input stream.
 

Expected Time Complexity: O(26 * n)
Expected Space Complexity: O(26)
 

Constraints:
1 <= n <= 105'''

# User function Template for python3


class Solution:
    def FirstNonRepeating(self, A):
        # Code here
        q = []
    ans = ""
    cnt = {}
    for c in A:
        q.append(c)
        cnt[c] = 1 if not c in cnt else cnt[c]+1
        while len(q) != 0 and cnt[q[0]] > 1:
            q.pop(0)
        if len(q) == 0:
            ans += ('#')
        else:
            ans += (q[0])
    return(ans)

# {
#  Driver Code Starts
# Initial Template for Python 3


if __name__ == '__main__':
    T = int(input())
    for i in range(T):
        A = input()
        ob = Solution()
        ans = ob.FirstNonRepeating(A)
        print(ans)

# } Driver Code Ends

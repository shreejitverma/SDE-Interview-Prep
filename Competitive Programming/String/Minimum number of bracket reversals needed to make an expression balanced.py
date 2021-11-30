'''https://practice.geeksforgeeks.org/problems/count-the-reversals0401/1

Count the Reversals 
Medium Accuracy: 50.95% Submissions: 14982 Points: 4
Given a string S consisting of only opening and closing curly brackets '{' and '}', find out the minimum number of reversals required to convert the string into a balanced expression.
A reversal means changing '{' to '}' or vice-versa.

Example 1:

Input:
S = "}{{}}{{{"
Output: 3
Explanation: One way to balance is:
"{{{}}{}}". There is no balanced sequence
that can be formed in lesser reversals.
â€‹Example 2:

Input: 
S = "{{}{{{}{{}}{{"
Output: -1
Explanation: There's no way we can balance
this sequence of braces.
Your Task:
You don't need to read input or print anything. Your task is to complete the function countRev() which takes the string S as input parameter and returns the minimum number of reversals required to balance the bracket sequence. If balancing is not possible, return -1. 

Expected Time Complexity: O(|S|).
Expected Auxiliary Space: O(1).

Constraints:
1 ≤ |S| ≤ 105'''


def countRev(s):
    # your code here
    ans = 0
    n = len(s)
    if(n & 1):
        return -1
    cnt = 0

    for i in range(n):
        if(s[i] == '{'):
            cnt += 1
        else:
            if(cnt == 0):
                ans += 1
                cnt += 1
            else:
                cnt -= 1
    return ans + int(cnt/2)

# {
#  Driver Code Starts


t = int(input())
for tc in range(t):
    s = input()
    print(countRev(s))

# Contributed By: Pranay Bansal

# } Driver Code Ends

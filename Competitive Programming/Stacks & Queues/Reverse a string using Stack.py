'''https://practice.geeksforgeeks.org/problems/reverse-a-string-using-stack/1

Reverse a string using Stack 
Easy Accuracy: 54.33% Submissions: 44155 Points: 2
You are given a string S, the task is to reverse the string using stack.

 

Example 1:


Input: S="GeeksforGeeks"
Output: skeeGrofskeeG
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function reverse() which takes the string S as an input parameter and returns the reversed string.

 

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)

 

Constraints:
1 ≤ length of the string ≤ 100'''

# {
# Driver Code Starts

# } Driver Code Ends


def reverse(S):

    # Add code here
    ans = ''
    lst = list(S)
    for i in range(len(S)):
        ans += lst.pop()
    return ans


# {
# Driver Code Starts.
if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        str1 = input()
        print(reverse(str1))
# } Driver Code Ends

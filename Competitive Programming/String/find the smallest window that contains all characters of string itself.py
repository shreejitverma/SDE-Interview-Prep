'''https://practice.geeksforgeeks.org/problems/smallest-distant-window3132/1

Smallest distinct window 
Medium Accuracy: 50.29% Submissions: 16273 Points: 4
Given a string 's'. The task is to find the smallest window length that contains all the characters of the given string at least one time.
For eg. A = “aabcbcdbca”, then the result would be 4 as of the smallest window will be “dbca”.

 

Example 1:

Input : "AABBBCBBAC"
Output : 3
Explanation : Sub-string -> "BAC"
Example 2:
Input : "aaab"
Output : 2
Explanation : Sub-string -> "ab"
 
Example 3:
Input : "GEEKSGEEKSFOR"
Output : 8
Explanation : Sub-string -> "GEEKSFOR"
 


Your Task:  
You don't need to read input or print anything. Your task is to complete the function findSubString() which takes the string  S as inputs and returns the length of the smallest such string.


Expected Time Complexity: O(256.N)
Expected Auxiliary Space: O(256)

 

Constraints:
1 ≤ |S| ≤ 105
String may contain both type of English Alphabets.'''

# User function Template for python3


class Solution:
    def findSubString(self, str):
        # Your code goes here
        d = {i: 0 for i in str}
        n = len(str)
        ans = n
        count = i = 0
        for j in range(n):
            if d[str[j]] == 0:
                count += 1
            d[str[j]] += 1
            while count == len(d):
                ans = min(ans, 1+j-i)
                if d[str[i]] == 1:
                    break
                else:
                    d[str[i]] -= 1
                    i += 1
        return ans


# {
#  Driver Code Starts
# Initial Template for Python 3


def main():

    T = int(input())

    while(T > 0):
        str = input()
        ob = Solution()
        print(ob.findSubString(str))

        T -= 1


if __name__ == "__main__":
    main()

# } Driver Code Ends

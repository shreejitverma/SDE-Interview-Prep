'''https://practice.geeksforgeeks.org/problems/edit-distance3702/1
https://www.geeksforgeeks.org/edit-distance-dp-5/
Edit Distance 
Medium Accuracy: 49.98% Submissions: 44405 Points: 4
Given two strings s and t. Return the minimum number of operations required to convert s to t.
The possible operations are permitted:

Insert a character at any position of the string.
Remove any character from the string.
Replace any character from the string with any other character.
 

Example 1:

Input: 
s = "geek", t = "gesek"
Output: 1
Explanation: One operation is required 
inserting 's' between two 'e's of str1.
Example 2:

Input : 
s = "gfg", t = "gfg"
Output: 
0
Explanation: Both strings are same.
 

Your Task:
You don't need to read or print anything. Your task is to complete the function editDistance() which takes strings s and t as input parameters and returns the minimum number of operation to convert the string s to string t. 


Expected Time Complexity: O(|s|*|t|)
Expected Space Complexity: O(|s|*|t|)


Constraints:
1 ≤ Length of both strings ≤ 100
Both the strings are in lowercase.

What are the subproblems in this case? 
The idea is to process all characters one by one starting from either from left or right sides of both strings. 
Let us traverse from right corner, there are two possibilities for every pair of character being traversed.  

m: Length of str1 (first string)
n: Length of str2 (second string)
If last characters of two strings are same, nothing much to do. Ignore last characters and get count for remaining strings. 
So we recur for lengths m-1 and n-1.
Else (If last characters are not same), we consider all operations on ‘str1’, 
consider all three operations on last character of first string, recursively compute minimum cost for all three operations 
and take minimum of three values. 
Insert: Recur for m and n-1
Remove: Recur for m-1 and n
Replace: Recur for m-1 and n-1
Below is implementation of above Naive recursive solution.
'''
# A Space efficient Dynamic Programming
# based Python3 program to find minimum
# number operations to convert str1 to str2


def EditDistDP(str1, str2):

    len1 = len(str1)
    len2 = len(str2)

    # Create a DP array to memoize result
    # of previous computations
    DP = [[0 for i in range(len1 + 1)]
          for j in range(2)]

    # Base condition when second String
    # is empty then we remove all characters
    for i in range(0, len1 + 1):
        DP[0][i] = i

    # Start filling the DP
    # This loop run for every
    # character in second String
    for i in range(1, len2 + 1):

        # This loop compares the char from
        # second String with first String
        # characters
        for j in range(0, len1 + 1):

            # If first String is empty then
            # we have to perform add character
            # operation to get second String
            if (j == 0):
                DP[i % 2][j] = i

            # If character from both String
            # is same then we do not perform any
            # operation . here i % 2 is for bound
            # the row number.
            elif(str1[j - 1] == str2[i-1]):
                DP[i % 2][j] = DP[(i - 1) % 2][j - 1]

            # If character from both String is
            # not same then we take the minimum
            # from three specified operation
            else:
                DP[i % 2][j] = (1 + min(DP[(i - 1) % 2][j],
                                        min(DP[i % 2][j - 1],
                                        DP[(i - 1) % 2][j - 1])))

    # After complete fill the DP array
    # if the len2 is even then we end
    # up in the 0th row else we end up
    # in the 1th row so we take len2 % 2
    # to get row
    print(DP[len2 % 2][len1], "")


# Driver code
if __name__ == '__main__':

    str1 = "food"
    str2 = "money"

    EditDistDP(str1, str2)


def minDis(s1, s2, n, m, dp):

    # If any string is empty,
    # return the remaining characters of other string
    if(n == 0):
        return m
    if(m == 0):
        return n

    # To check if the recursive tree
    # for given n & m has already been executed
    if(dp[n][m] != -1):
        return dp[n][m]

    # If characters are equal, execute
    # recursive function for n-1, m-1
    if(s1[n - 1] == s2[m - 1]):
        if(dp[n - 1][m - 1] == -1):
            dp[n][m] = minDis(s1, s2, n - 1, m - 1, dp)
            return dp[n][m]
        else:
            dp[n][m] = dp[n - 1][m - 1]
            return dp[n][m]

    # If characters are nt equal, we need to
    # find the minimum cost out of all 3 operations.
    else:
        if(dp[n - 1][m] != -1):
            m1 = dp[n - 1][m]
        else:
            m1 = minDis(s1, s2, n - 1, m, dp)

        if(dp[n][m - 1] != -1):
            m2 = dp[n][m - 1]
        else:
            m2 = minDis(s1, s2, n, m - 1, dp)
        if(dp[n - 1][m - 1] != -1):
            m3 = dp[n - 1][m - 1]
        else:
            m3 = minDis(s1, s2, n - 1, m - 1, dp)

        dp[n][m] = 1 + min(m1, min(m2, m3))
        return dp[n][m]


        # Driver code
str1 = "voldemort"
str2 = "dumbledore"

n = len(str1)
m = len(str2)
dp = [[-1 for i in range(m + 1)] for j in range(n + 1)]

print(minDis(str1, str2, n, m, dp))

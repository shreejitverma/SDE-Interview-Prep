'''https://practice.geeksforgeeks.org/problems/power-set4302/1
https://www.geeksforgeeks.org/generating-distinct-subsequences-of-a-given-string-in-lexicographic-order/
Power Set 
Easy Accuracy: 48.41% Submissions: 14124 Points: 2
Given a string S find all possible subsequences of the string in lexicographically-sorted order.

Example 1:

Input : str = "abc"
Output: a ab abc ac b bc c
Explanation : There are 7 substrings that 
can be formed from abc.
Example 2:

Input: str = "aa"
Output: a a aa
Explanation : There are 3 substrings that 
can be formed from aa.
Your Task:
You don't need to read ot print anything. Your task is to complete the function AllPossibleStrings() which takes S as input parameter and returns a list of all possible substrings(non-empty) that can be formed from S in lexicographically-sorted order.
 

Expected Time Complexity: O(2n) where n is the length of the string
Expected Space Complexity : O(n * 2n)
 

Constraints: 
1 <= Length of string <= 16'''

# Python program to print all distinct
# subsequences of a string.

# Finds and stores result in st for a given
# string s.


def generate(st, s):
    if len(s) == 0:
        return

    # If current string is not already present.
    if s not in st:
        st.add(s)

        # Traverse current string, one by one
        # remove every character and recur.
        for i in range(len(s)):
            t = list(s).copy()
            t.remove(s[i])
            t = ''.join(t)
            generate(st, t)

    return


# Driver Code
if __name__ == "__main__":
    s = "xyz"
    st = set()
    generate(st, s)
    for i in st:
        print(i)

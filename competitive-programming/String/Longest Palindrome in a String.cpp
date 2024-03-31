/*
https://practice.geeksforgeeks.org/problems/longest-palindrome-in-a-string3411/1
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
1 ≤ |S| ≤ 103
*/

class Solution
{
public:
    string longestPalin(string S)
    {

        int n = S.length();
        vector<vector<int> > arr(n, vector<int>(n, 0));

        for (int g = 0; g < n; g++)
        {

            for (int i = 0, j = g; j < n; i++, j++)
            {
                int flag = 0;
                if (g == 0)
                {
                    arr[i][j] = 1;
                }

                else if (g == 1)
                {
                    if (S[i] == S[j])
                        arr[i][j] = 2;
                }

                else
                {
                    if (S[i] == S[j])
                        arr[i][j] = arr[i + 1][j - 1] > 0 ? j - i + 1 : 0;
                    else
                        arr[i][j] = 0;
                }
            }
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cout << arr[i][j] << " ";
            }
            cout << endl;
        }

        int len = 0;
        int start = 0;
        int end = 0;

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (arr[i][j] > len)
                {
                    len = arr[i][j];
                    start = i;
                    end = j;
                }
            }
        }

        return S.substr(start, len);
    }
};

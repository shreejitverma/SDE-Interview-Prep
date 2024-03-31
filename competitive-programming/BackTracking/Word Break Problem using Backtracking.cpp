/*
https://practice.geeksforgeeks.org/problems/word-break-part-23249/1
Word Break - Part 2 
Hard Accuracy: 61.41% Submissions: 5460 Points: 8
Given a string s and a dictionary of words dict of length n, add spaces in s to construct a sentence where each word is a valid dictionary word. Each dictionary word can be used more than once. Return all such possible sentences.

Follow examples for better understanding.

Example 1:

Input: s = "catsanddog", n = 5 
dict = {"cats", "cat", "and", "sand", "dog"}
Output: (cats and dog)(cat sand dog)
Explanation: All the words in the given 
sentences are present in the dictionary.
Example 2:

Input: s = "catsandog", n = 5
dict = {"cats", "cat", "and", "sand", "dog"}
Output: Empty
Explanation: There is no possible breaking 
of the string s where all the words are present 
in dict.
Your Task:
You do not need to read input or print anything. Your task is to complete the function wordBreak() which takes n, dict and s as input parameters and returns a list of possible sentences. If no sentence is possible it returns an empty list.

Expected Time Complexity: O(N2*n) where N = |s|
Expected Auxiliary Space: O(N2)

Constraints:
1 ≤ n ≤ 20
1 ≤ dict[i] ≤ 15
1 ≤ |s| ≤ 500 
*/

// User function Template for C++
void word_break(vector<string> &v, vector<string> &dict, string s, int idx, int n, string str)
{
    if (idx == n)
    {
        str.pop_back();
        v.push_back(str);
    }
    else
    {
        for (int k = idx; k < n; k++)
        {
            if (find(dict.begin(), dict.end(), s.substr(idx, k - idx + 1)) != dict.end())
            {
                string temp = s.substr(idx, k - idx + 1) + ' ';
                word_break(v, dict, s, k + 1, n, str + temp);
            }
        }
    }
    return;
}
class Solution
{
public:
    vector<string> wordBreak(int n, vector<string> &dict, string s)
    {
        vector<string> v;
        word_break(v, dict, s, 0, s.size(), "");
        return v;
    }
};
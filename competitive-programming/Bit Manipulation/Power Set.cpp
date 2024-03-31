/*
https://practice.geeksforgeeks.org/problems/power-set4302/1
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
1 <= Length of string <= 16
*/

// C++ program to print all distinct subsequences
// of a string.
#include <bits/stdc++.h>
using namespace std;

// Finds and stores result in st for a given
// string s.
void generate(set<string> &st, string s)
{
    if (s.size() == 0)
        return;

    // If current string is not already present.
    if (st.find(s) == st.end())
    {
        st.insert(s);

        // Traverse current string, one by one
        // remove every character and recur.
        for (int i = 0; i < s.size(); i++)
        {
            string t = s;
            t.erase(i, 1);
            generate(st, t);
        }
    }
    return;
}

vector<string> AllPossibleStrings(string s)
{
    // Code here
    int n = s.length();
    vector<string> v;
    for (int i = 0; i < (1 << n); i++)
    {
        string str;
        for (int j = 0; j < n; j++)
            if (i & (1 << j))
                str += s[j];
        if (!str.empty())
            v.push_back(str);
    }
    sort(v.begin(), v.end());
    return v;
}
// Driver code
int main()
{
    string s = "xyz";
    set<string> st;
    set<string>::iterator it;
    generate(st, s);
    for (auto it = st.begin(); it != st.end(); it++)
        cout << *it << endl;
    return 0;
}
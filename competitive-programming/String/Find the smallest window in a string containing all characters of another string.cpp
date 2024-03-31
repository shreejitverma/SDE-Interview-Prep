/*
https://practice.geeksforgeeks.org/problems/smallest-window-in-a-string-containing-all-the-characters-of-another-string-1587115621/1

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
1 ≤ |S|, |P| ≤ 105
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    //Function to find the smallest window in the string s consisting
    //of all the characters of string p.
    string smallestWindow(string s, string p)
    {
        // Your code here
        int hash[256] = {0};
        int uniqueWords = 0;

        for (int i = 0; i < p.size(); i++)
        {
            if (hash[p[i]] == 0)
            {
                uniqueWords++;
            }

            hash[p[i]]++;
        }

        int resCount = INT_MAX;
        int resStart = 0;

        int i = 0, j = 0;

        while (j < s.size())
        {
            hash[s[j]]--;

            if (hash[s[j]] == 0)
            {
                uniqueWords--;
            }

            if (uniqueWords == 0)
            {
                while (uniqueWords == 0)
                {
                    if (resCount > j - i + 1)
                    {
                        resCount = min(resCount, j - i + 1);

                        resStart = i;
                    }

                    hash[s[i]]++;

                    if (hash[s[i]] > 0)
                    {
                        uniqueWords++;
                    }

                    i++;
                }
            }

            j++;
        }

        if (resCount == INT_MAX)
        {
            return "-1";
        }

        return s.substr(resStart, resCount);
    }
};

// { Driver Code Starts.
int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        string s;
        cin >> s;
        string pat;
        cin >> pat;
        Solution obj;
        cout << obj.smallestWindow(s, pat) << endl;
    }
    return 0;
} // } Driver Code Ends
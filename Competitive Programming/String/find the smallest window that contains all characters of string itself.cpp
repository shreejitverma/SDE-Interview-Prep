/*
https://practice.geeksforgeeks.org/problems/smallest-distant-window3132/1

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
String may contain both type of English Alphabets.
*/
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    string findSubString(string str)
    {
        // Your code goes here
        int n = str.length();
        unordered_set<char> s;
        unordered_map<char, int> mp;
        for (int i = 0; i < n; i++)

        {

            s.insert(str[i]);
        }

        int distinctChar = s.size();
        if (distinctChar == n)
            return str;

        int l = 0, len = n;
        string ans = str;
        for (int r = 0; r < n; r++)

        {

            mp[str[r]]++;
            while (mp.size() == distinctChar && l <= r)
            {
                if (len > r - l + 1)
                {
                    len = r - l + 1;
                    ans = str.substr(l, len);
                }
                if (mp[str[l]] == 1)
                    mp.erase(str[l]);
                else
                    mp[str[l]]--;
                l++;
            }
        }
        // cout << ans << endl;
        return ans;
    }
};

// { Driver Code Starts.
// Driver code
int main()
{
    int t;
    cin >> t;
    while (t--)
    {

        string str;
        cin >> str;
        Solution ob;
        cout << ob.findSubString(str).size() << endl;
    }
    return 0;
} // } Driver Code Ends
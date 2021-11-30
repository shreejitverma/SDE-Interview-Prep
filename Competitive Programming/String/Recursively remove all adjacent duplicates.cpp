/*
https://practice.geeksforgeeks.org/problems/consecutive-elements2306/1

Remove Consecutive Characters 
Basic Accuracy: 56.33% Submissions: 10963 Points: 1
Given a string S delete the characters which are appearing more than once consecutively.

Example 1:

Input:
S = aabb
Output:  ab 
Explanation: 'a' at 2nd position is
appearing 2nd time consecutively.
Similiar explanation for b at
4th position.

Example 2:

Input:
S = aabaa
Output:  aba
Explanation: 'a' at 2nd position is
appearing 2nd time consecutively.
'a' at fifth position is appearing
2nd time consecutively.
 

Your Task:
You dont need to read input or print anything. Complete the function removeConsecutiveCharacter() which accepts a string as input parameter and returns modified string. 
 

Expected Time Complexity: O(|S|).
Expected Auxiliary Space: O(|S|).
 

Constraints:
1<=|S|<=105
All characters are lowercase alphabets.
*/
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends

class Solution
{
public:
    string removeConsecutiveCharacter(string s)
    {
        // code here.

        string res = "";
        res = s[0];
        for (int i = 1; i < s.length(); i++)

        {
            if (s[i] != res[res.length() - 1])

            {
                res += s[i];
            }
        }

        return res;
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
        Solution ob;
        cout << ob.removeConsecutiveCharacter(s) << endl;
    }
}

// } Driver Code Ends
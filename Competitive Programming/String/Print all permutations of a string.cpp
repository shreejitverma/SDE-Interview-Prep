/*
https: // practice.geeksforgeeks.org/problems/permutations-of-a-given-string2041/1
Permutations of a given string
Basic Accuracy: 49.85 % Submissions: 31222 Points: 1
Given a string S. The task is to print all permutations of a given string.


Example 1:

Input: ABC
Output:
ABC ACB BAC BCA CAB CBA
Explanation:
Given string ABC has permutations in 6
forms as ABC, ACB, BAC, BCA, CAB and CBA .
Example 2:

Input: ABSG
Output:
ABGS ABSG AGBS AGSB ASBG ASGB BAGS
BASG BGAS BGSA BSAG BSGA GABS GASB
GBAS GBSA GSAB GSBA SABG SAGB SBAG
SBGA SGAB SGBA
Explanation:
Given string ABSG has 24 permutations.


Your Task:
You don't need to read input or print anything. Your task is to complete the function find_permutaion() which takes the string S as input parameter and returns a vector of string in lexicographical order.


Expected Time Complexity: O(n! * n)

Expected Space Complexity: O(n)


Constraints:
1 <= length of string <= 5

*/
class Solution
{
public:
    vector<string> find_permutation(string str)
    {
        // Code here there
        // Sort the string in lexicographically
        // ascennding order
        vector<string> s;
        sort(str.begin(), str.end());

        // Keep printing next permutation while there
        // is next permutation
        do
        {
            s.push_back(str);
        } while (next_permutation(str.begin(), str.end()));

        return s;
    }
};

#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    vector<string> v;
    void go(string &S, int index, int n)
    {
        if (index >= n)
        {
            v.push_back(S);
            return;
        }

        for (int i = index; i < n; i++)
        {
            swap(S[i], S[index]);
            go(S, index + 1, n);
            swap(S[i], S[index]);
        }
    }

    vector<string> find_permutation(string S)
    {
        int n = S.length();

        go(S, 0, n);
        sort(v.begin(), v.end());
        return v;
    }
};

// { Driver Code Starts.
int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        string S;
        cin >> S;
        Solution ob;
        vector<string> ans = ob.find_permutation(S);
        for (auto i : ans)
        {
            cout << i << " ";
        }
        cout << "\n";
    }
    return 0;
}
// } Driver Code Ends
void permute(string str)
{
    // Sort the string in lexicographically
    // ascennding order
    sort(str.begin(), str.end());

    // Keep printing next permutation while there
    // is next permutation
    do
    {
        cout << str << " ";
    } while (next_permutation(str.begin(), str.end()));
}
int main()
{
    //code
    int t;
    cin >> t;
    while (t--)
    {
        string s;
        cin >> s;
        permute(s);
        cout << endl;
    }
    return 0;
}
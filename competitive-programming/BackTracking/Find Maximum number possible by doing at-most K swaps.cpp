/*
https://practice.geeksforgeeks.org/problems/largest-number-in-k-swaps-1587115620/1
Largest number in K swaps 
Medium Accuracy: 46.92% Submissions: 26080 Points: 4
Given a number K and string str of digits denoting a positive integer, build the largest number possible by performing swap operations on the digits of str at most K times.


Example 1:

Input:
K = 4
str = "1234567"
Output:
7654321
Explanation:
Three swaps can make the
input 1234567 to 7654321, swapping 1
with 7, 2 with 6 and finally 3 with 5
Example 2:

Input:
K = 3
str = "3435335"
Output:
5543333
Explanation:
Three swaps can make the input
3435335 to 5543333, swapping 3 
with 5, 4 with 5 and finally 3 with 4 

Your task:
You don't have to read input or print anything. Your task is to complete the function findMaximumNum() which takes the string and an integer as input and returns a string containing the largest number formed by perfoming the swap operation at most k times.


Expected Time Complexity: O(n!/(n-k)!) , where n = length of input string
Expected Auxiliary Space: O(n)


Constraints:
1 ≤ |str| ≤ 30
1 ≤ K ≤ 10
*/
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends

class Solution
{
public:
    //Function to find the largest number after k swaps.
    string ans;
    void solve(string str, int k, int id)
    {
        if (k == 0)
            return;
        char ma = str[id];
        for (int i = id + 1; i < str.size(); i++)
        {
            if (str[i] > ma)
                ma = str[i];
        }
        if (ma != str[id])
            k--;

        for (int i = str.size() - 1; i >= id; i--)
        {
            if (str[i] == ma)
            {
                swap(str[i], str[id]);
                ans = max(ans, str);
                solve(str, k, id + 1);
                swap(str[i], str[id]);
            }
        }
    }
    string findMaximumNum(string str, int k)
    {
        // code here.
        ans = str;
        solve(str, k, 0);
        return ans;
    }
};

// { Driver Code Starts.

int main()
{
    int t, k;
    string str;

    cin >> t;
    while (t--)
    {
        cin >> k >> str;
        Solution ob;
        cout << ob.findMaximumNum(str, k) << endl;
    }
    return 0;
}
// } Driver Code Ends

void findMaximumNum(string str, int k, string &max)
{
    // return if no swaps left
    if (k == 0)
        return;

    int n = str.length();

    // consider every digit
    for (int i = 0; i < n - 1; i++)
    {

        // and compare it with all digits after it
        for (int j = i + 1; j < n; j++)
        {
            // if digit at position i is less than digit
            // at position j, swap it and check for maximum
            // number so far and recurse for remaining swaps
            if (str[i] < str[j])
            {
                // swap str[i] with str[j]
                swap(str[i], str[j]);

                // If current num is more than maximum so far
                if (str.compare(max) > 0)
                    max = str;

                // recurse of the other k - 1 swaps
                findMaximumNum(str, k - 1, max);

                // backtrack
                swap(str[i], str[j]);
            }
        }
    }
}

int main()
{
    //code
    int t;
    cin >> t;
    while (t--)
    {
        int k;
        cin >> k;
        string ar;
        cin >> ar;

        string max = ar;
        findMaximumNum(ar, k, max);
        cout << max << endl;
    }
    return 0;
}

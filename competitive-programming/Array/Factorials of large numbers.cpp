/*
https://practice.geeksforgeeks.org/problems/factorials-of-large-numbers2508/1
Factorials of large numbers 
Medium Accuracy: 51.61% Submissions: 28359 Points: 4
Given an integer N, find its factorial.

Example 1:

Input: N = 5
Output: 120
Explanation : 5! = 1*2*3*4*5 = 120
Example 2:

Input: N = 10
Output: 3628800
Explanation :
10! = 1*2*3*4*5*6*7*8*9*10 = 3628800

Your Task:
You don't need to read input or print anything. Complete the function factorial() that takes integer N as input parameter and returns a list of integers denoting the digits that make up the factorial of N.


Expected Time Complexity : O(N2)
Expected Auxilliary Space : O(1)


Constraints:
1 ≤ N ≤ 1000
*/

class Solution
{
public:
    vector<int> factorial(int N)
    {
        // code here
        vector<int> res;
        res.push_back(1);
        for (int x = 2; x <= N; x++)
        {
            int carry = 0;
            for (int i = 0; i < res.size(); i++)
            {
                int prod = res[i] * x + carry;
                res[i] = prod % 10;
                carry = prod / 10;
            }
            while (carry != 0)
            {
                res.push_back(carry % 10);
                carry /= 10;
            }
        }
        reverse(res.begin(), res.end());
        return res;
    }
};
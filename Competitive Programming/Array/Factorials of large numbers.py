'''https://practice.geeksforgeeks.org/problems/factorials-of-large-numbers2508/1
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
1 ≤ N ≤ 1000'''


class Solution:
    def factorial(self, N):
        # code here
        k = 1
        for i in range(1, N+1):
            k *= i
        return str(k)


class Solution:
    def factorial(self, N):
        # code here

        res = list()
        res.append(1)
        for x in range(2, N+1):
            carry = 0
            for i in range(len(res)):
                prod = res[i]*x+carry
                res[i] = int(prod % 10)
                carry = int(prod/10)
            while(int(carry) != 0):
                res.append(carry % 10)
                carry = int(carry/10)
        # res.reverse()
        return res[::-1]

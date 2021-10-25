/*
https://www.geeksforgeeks.org/calculate-square-of-a-number-without-using-and-pow/
Calculate square of a number without using *, / and pow()
Difficulty Level : Medium
Last Updated : 23 Feb, 2021
Geek Week
Given an integer n, calculate the square of a number without using *, / and pow(). 

Examples : 

Input: n = 5
Output: 25

Input: 7
Output: 49

Input: n = 12
Output: 144
A Simple Solution is to repeatedly add n to result. 
*/

// Simple solution to calculate square without
// using * and pow()
#include <iostream>
using namespace std;

int square(int n)
{
    // handle negative input
    if (n < 0)
        n = -n;

    // Initialize result
    int res = n;

    // Add n to res n-1 times
    for (int i = 1; i < n; i++)
        res += n;

    return res;
}

// Driver code
int main()
{
    for (int n = 1; n <= 5; n++)
        cout << "n = " << n << ", n^2 = " << square(n)
             << endl;
    return 0;
}
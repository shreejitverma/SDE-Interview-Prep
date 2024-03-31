/*
https://www.geeksforgeeks.org/divide-two-integers-without-using-multiplication-division-mod-operator/

Divide two integers without using multiplication, division and mod operator
Difficulty Level : Medium
Last Updated : 03 Sep, 2021
Geek Week
Given a two integers say a and b. Find the quotient after dividing a by b without using multiplication, division and mod operator.

Example: 

Input : a = 10, b = 3
Output : 3

Input : a = 43, b = -8
Output :  -5 
Recommended: Please try your approach on {IDE} first, before moving on to the solution.
Approach : Keep subtracting the dividend from the divisor until dividend becomes less than divisor. 
The dividend becomes the remainder, and the number of times subtraction is done becomes the quotient. 
*/

// C++ implementation to Divide two
// integers without using multiplication,
// division and mod operator
#include <bits/stdc++.h>
using namespace std;

// Function to divide a by b and
// return floor value it
int divide(long long dividend, long long divisor)
{

    // Calculate sign of divisor i.e.,
    // sign will be negative only iff
    // either one of them is negative
    // otherwise it will be positive
    int sign = ((dividend < 0) ^
                (divisor < 0))
                   ? -1
                   : 1;

    // remove sign of operands
    dividend = abs(dividend);
    divisor = abs(divisor);

    // Initialize the quotient
    long long quotient = 0, temp = 0;

    // test down from the highest bit and
    // accumulate the tentative value for
    // valid bit
    for (int i = 31; i >= 0; --i)
    {

        if (temp + (divisor << i) <= dividend)
        {
            temp += divisor << i;
            quotient |= 1LL << i;
        }
    }
    //if the sign value computed earlier is -1 then negate the value of quotient
    if (sign == -1)
        quotient = -quotient;

    return quotient;
}

// Driver code
int main()
{
    int a = 10, b = 3;
    cout << divide(a, b) << "\n";

    a = 43, b = -8;
    cout << divide(a, b);

    return 0;
}
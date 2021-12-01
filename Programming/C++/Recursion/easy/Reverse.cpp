#include <iostream>
#include <math.h>
using namespace std;
int rev1(int n);
int helper(int n, int digits);
int rev2(int n);
int sum = 0;
int rev1(int n)
{

    if (n == 0)
    {
        return 0;
    }
    int rem = n % 10;
    sum = sum * 10 + rem;
    rev1(n / 10);
    return sum;
}
int rev2(int n)
{
    // sometimes you might need some additional variables in the argument
    // in that case, make another function
    int digits = (int)(log10(n) + 1);
    return helper(n, digits);
}

int helper(int n, int digits)
{
    if (n % 10 == n)
    {
        return n;
    }
    int rem = n % 10;
    return rem * (int)(pow(10, digits - 1)) + helper(n / 10, digits - 1);
}

int main()
{

    cout << rev1(1234) << endl;
    cout << rev2(56789) << endl;
    return 0;
}

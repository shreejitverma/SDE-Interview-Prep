#include <iostream>
#include <math.h>
using namespace std;
int rev(int n);
int helper(int n, int digits);

int rev(int n)
{
    // sometimes you might need some additional variables in the argument
    // in that case, make another function
    int digits = (int)(log10(n)) + 1;
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

bool palin(int n)
{
    if (n == rev(n))
    {
        cout << "Yes ";
    }
    else
    {
        cout << "No ";
    }
}

int main()
{
    cout << palin(1231) << rev(1231) << "\n";
    cout << palin(1234554321) << rev(1234554321) << "\n";
}

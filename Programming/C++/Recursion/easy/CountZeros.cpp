#include <iostream>
using namespace std;
int helper(int n, int c);
static int count(int n)
{
    return helper(n, 0);
}

// special pattern, how to pass a value to above calls
int helper(int n, int c)
{
    if (n == 0)
    {
        return c;
    }

    int rem = n % 10;
    if (rem == 0)
    {
        return helper(n / 10, c + 1);
    }
    return helper(n / 10, c);
}
int main()
{
    cout << count(30210004) << "\n";
}

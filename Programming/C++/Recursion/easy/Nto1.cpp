#include <iostream>
using namespace std;

// concept
void concept(int n)
{
    if (n == 0)
    {
        return;
    }
    cout << n << endl;
    //        concept(n--);
    concept(--n);
    //        n-- vs --n
}

void fun(int n)
{
    if (n == 0)
    {
        return;
    }
    cout << n << endl;
    fun(n - 1);
}

void funRev(int n)
{
    if (n == 0)
    {
        return;
    }
    funRev(n - 1);
    cout << n << endl;
}

void funBoth(int n)
{
    if (n == 0)
    {
        return;
    }
    cout << n << endl;
    funBoth(n - 1);
    cout << n << endl;
}
int main()
{
    funBoth(5);
    return 0;
}

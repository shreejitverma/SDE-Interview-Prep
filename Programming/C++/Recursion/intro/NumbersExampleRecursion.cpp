#include <iostream>
using namespace std;

void print(int n)
{
    // base condition
    if (n == 5)
    {
        cout << 5 << endl;
        return;
    }
    cout << n << endl;

    // recursive call
    // if you are calling a function again and again, you can treat it as a separate call in the stack

    // this is called tail recursion
    // this is the last function call
    print(n + 1);
}
int main()
{
    print(0);
    return 0;
}

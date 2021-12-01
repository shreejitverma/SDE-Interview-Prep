#include <iostream>
using namespace std;
int prod(int n)
{
    if (n % 10 == n)
    {
        return n;
    }
    return (n % 10) * prod(n / 10);
}
int main()
{
    int ans = prod(12345);
    cout << ans << endl;
}

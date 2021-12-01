

#include <iostream>
using namespace std;
int fibo(int n)
{
    // base condition
    // if (n < 2)
    // {
    //     return n;
    // }
    // return fibo(n - 1) + fibo(n - 2);
    int arr[2] = {0, 1};
    if (n < 2)
    {
        return arr[n];
    }
    for (int i = 1; i < n; i++)
    {
        int temp = arr[0];
        arr[0] = arr[1];
        arr[1] = arr[0] + temp;
    }
    return arr[1];
}
int main()
{
    for (int i = 0; i < 10; i++)
    {
        int ans = fibo(i);
        cout << ans << "\n";
    }
}

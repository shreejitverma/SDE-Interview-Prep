
#include <iostream>
using namespace std;

int search(int arr[], int target, int s, int e)
{
    if (s > e)
    {
        return -1;
    }
    int m = s + (e - s) / 2;
    if (arr[m] == target)
    {
        return m;
    }
    if (target < arr[m])
    {
        return search(arr, target, s, m - 1);
    }
    return search(arr, target, m + 1, e);
}

int main()
{
    int arr[7] = {1, 2, 3, 4, 55, 66, 78};
    int target = 78;
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << search(arr, target, 0, n - 1) << endl;
}

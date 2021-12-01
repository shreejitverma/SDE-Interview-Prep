#include <iostream>
#include <vector>
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

    if (arr[s] <= arr[m])
    {
        if (target >= arr[s] && target <= arr[m])
        {
            return search(arr, target, s, m - 1);
        }
        else
        {
            return search(arr, target, m + 1, e);
        }
    }

    if (target >= arr[m] && target <= arr[e])
    {
        return search(arr, target, m + 1, e);
    }

    return search(arr, target, s, m - 1);
}

int main()
{
    int arr[8] = {5, 6, 7, 8, 9, 1, 2, 3};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << search(arr, 8, 0, n - 1) << endl;
    cout << search(arr, 4, 0, n - 1) << endl;
}

// https://leetcode.com/problems/first-missing-positive/
#include <iostream>
using namespace std;
int firstMissingPositive(int arr[], int n)
{
    int i = 0;
    while (i < n)
    {
        int correct = arr[i] - 1;
        if (arr[i] > 0 && arr[i] <= n && arr[i] != arr[correct])
        {
            swap(arr[i], arr[correct]);
        }
        else
        {
            i++;
        }
    }

    // search for first missing number
    for (int index = 0; index < n; index++)
    {
        if (arr[index] != index + 1)
        {
            return index + 1;
        }
    }

    // case 2
    return n + 1;
}

int main()
{
    int arr[4] = {4, 0, 3, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << firstMissingPositive(arr, n);
    return 0;
}

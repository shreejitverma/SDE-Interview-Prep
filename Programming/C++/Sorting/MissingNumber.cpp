// https://leetcode.com/problems/missing-number/
// Amazon Question
#include <iostream>
using namespace std;
int missingNumber(int arr[], int n)
{
    int i = 0;
    while (i < n)
    {
        int correct = arr[i];
        if (arr[i] < n && arr[i] != arr[correct])
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
        if (arr[index] != index)
        {
            return index;
        }
    }

    // case 2
    return n;
}

int main()
{
    int arr[4] = {4, 0, 2, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << missingNumber(arr, n);
    return 0;
}
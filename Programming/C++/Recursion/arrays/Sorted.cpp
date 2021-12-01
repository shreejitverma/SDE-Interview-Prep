#include <iostream>
#include <vector>
using namespace std;

bool sorted(int arr[], int n, int index)
{
    // base condition
    if (index == n - 1)
    {
        return true;
    }

    return arr[index] < arr[index + 1] && sorted(arr, n, index + 1);
}
int main()
{
    int arr[6] = {1, 2, 3, 5, 8, 16};
    int arr1[6] = {1, 2, 3, 5, 16, 8};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << sorted(arr, n, 0) << "\n";
    cout << sorted(arr1, n, 0) << "\n";
}

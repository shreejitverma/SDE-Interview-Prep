#include <iostream>
using namespace std;
// https://leetcode.com/problems/find-the-duplicate-number/

int findDuplicate(int arr[], int n)
{
    int i = 0;
    while (i < n)
    {

        if (arr[i] != i + 1)
        {
            int correct = arr[i] - 1;
            if (arr[i] != arr[correct])
            {
                swap(arr[i], arr[correct]);
            }
            else
            {
                return arr[i];
            }
        }
        else
        {
            i++;
        }
    }
    return -1;
}

// void swap(int arr[], int first, int second)
// {
//     int temp = arr[first];
//     arr[first] = arr[second];
//     arr[second] = temp;
// }
int main()
{
    int arr[] = {1, 3, 4, 2, 2};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << findDuplicate(arr, n);

    return 0;
}

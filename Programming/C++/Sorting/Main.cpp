#include <iostream>
using namespace std;

int getMaxIndex(int arr[], int start, int end)
{
    int max = start;
    for (int i = start; i <= end; i++)
    {
        if (arr[max] < arr[i])
        {
            max = i;
        }
    }
    return max;
}

void insertion(int arr[], int n)
{
    // int n = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j > 0; j--)
        {
            if (arr[j] < arr[j - 1])
            {
                swap(arr[j], arr[j - 1]);
            }
            else
            {
                break;
            }
        }
    }
}

void selection(int arr[], int n)
{
    // int n = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < n; i++)
    {
        // find the max item in the remaining array and swap with correct index
        int last = n - i - 1;
        int maxIndex = getMaxIndex(arr, 0, last);
        swap(arr[maxIndex], arr[last]);
    }
}

// void swap(int arr[], int first, int second)
// {
//     int temp = arr[first];
//     arr[first] = arr[second];
//     arr[second] = temp;
// }

void bubble(int arr[], int n)
{
    bool swapped;
    // run the steps n-1 times
    // int n = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < n; i++)
    {
        swapped = false;
        // for each step, max item will come at the last respective index
        for (int j = 1; j < n - i; j++)
        {
            // swap if the item is smaller than the previous item
            if (arr[j] < arr[j - 1])
            {
                // swap
                swap(arr[j], arr[j - 1]);
                swapped = true;
            }
        }
        // if you did not swap for a particular value of i, it means the array is sorted hence stop the program
        if (!swapped)
        { // !false = true
            break;
        }
    }
}
void display(int arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
}
int main()
{
    int arr[5] = {5, 3, 4, 1, 2};
    int arr1[6] = {5, 3, 4, 1, 2, 6};
    int arr2[7] = {5, 3, 4, 1, 2, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);
    insertion(arr, n);
    display(arr, n);
    n = sizeof(arr1) / sizeof(arr1[0]);
    bubble(arr1, n);
    display(arr1, n);
    n = sizeof(arr2) / sizeof(arr2[0]);
    selection(arr2, n);
    display(arr2, n);
}

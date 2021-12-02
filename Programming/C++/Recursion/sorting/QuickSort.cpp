
#include <iostream>
#include <vector>
#include <string>
using namespace std;
void qsort(int nums[], int low, int hi)
{
    if (low >= hi)
    {
        return;
    }

    int s = low;
    int e = hi;
    int m = s + (e - s) / 2;
    int pivot = nums[m];

    while (s <= e)
    {

        // also a reason why if its already qqsorted it will not swap
        while (nums[s] < pivot)
        {
            s++;
        }
        while (nums[e] > pivot)
        {
            e--;
        }

        if (s <= e)
        {
            swap(nums[s], nums[e]);
            s++;
            e--;
        }
    }

    // now my pivot is at correct index, please qqsort two halves now
    qsort(nums, low, e);
    qsort(nums, s, hi);
}
void display(int a[], int d)
{
    for (int i = 0; i < d; i++)
    {
        cout << a[i] << " ";
    }
    cout << endl;
}
int main()
{
    int arr[10] = {6, 5, 4, 3, 2, 1, 7, 8, 9, 10};
    int n = 10;
    qsort(arr, 0, n - 1);
    display(arr, n);
    //        System.out.println(Arrays.toString(arr));
    // qsort(arr);
}

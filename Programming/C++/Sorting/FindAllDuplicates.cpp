#include <iostream>
#include <list>
using namespace std;
// https://leetcode.com/problems/find-all-duplicates-in-an-array/
list<int> findDuplicates(int arr[], int n)
{
    int i = 0;
    while (i < n)
    {
        int correct = arr[i] - 1;
        if (arr[i] != arr[correct])
        {
            swap(arr[i], arr[correct]);
        }
        else
        {
            i++;
        }
    }

    list<int> ans;
    for (int index = 0; index < n; index++)
    {
        if (arr[index] != index + 1)
        {
            ans.push_back(arr[index]);
        }
    }

    return ans;
}

// static void swap(int[] arr, int first, int second)
// {
//     int temp = arr[first];
//     arr[first] = arr[second];
//     arr[second] = temp;
// }
void showlist(list<int> g)
{
    list<int>::iterator it;
    for (it = g.begin(); it != g.end(); ++it)
        cout << *it << " ";
    cout << '\n';
}
int main()
{
    list<int> l;
    int arr[] = {4, 3, 2, 7, 8, 2, 3, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    l = findDuplicates(arr, n);
    showlist(l);

    return 0;
}
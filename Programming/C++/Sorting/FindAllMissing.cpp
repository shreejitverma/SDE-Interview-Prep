// https://leetcode.com/problems/find-all-numbers-disappeared-in-an-array/
// Asked in Google
#include <iostream>
#include <list>
using namespace std;
list<int> findDisappearedNumbers(int nums[], int n)
{
    int i = 0;
    while (i < n)
    {
        int correct = nums[i] - 1;
        if (nums[i] != nums[correct])
        {
            swap(nums[i], nums[correct]);
        }
        else
        {
            i++;
        }
    }

    // just find missing numbers
    list<int> ans;
    for (int index = 0; index < n; index++)
    {
        if (nums[index] != index + 1)
        {
            ans.push_back(index + 1);
        }
    }

    return ans;
}
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
    int arr[4] = {4, 0, 2, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    l = findDisappearedNumbers(arr, n);
    showlist(l);
    return 0;
}
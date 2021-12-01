// https://leetcode.com/problems/set-mismatch/
#include <iostream>
#include <list>
using namespace std;
list<int> findErrorNums(int arr[], int n)
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

    // search for first missing number
    for (int index = 0; index < n; index++)
    {
        if (arr[index] != index + 1)
        {
            list<int> ans;

            ans.push_back(arr[index]);
            ans.push_back(index + 1);
            return ans;
        }
    }
    list<int> ans;

    ans.push_back(-1);
    ans.push_back(-1);
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
    int arr[4] = {1, 2, 2, 4};
    int n = sizeof(arr) / sizeof(arr[0]);
    l = findErrorNums(arr, n);
    showlist(l);
    return 0;
}
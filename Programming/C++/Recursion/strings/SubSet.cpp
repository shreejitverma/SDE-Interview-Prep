#include <iostream>
#include <vector>
#include <string>
using namespace std;

vector<vector<int> > subset(int arr[])
{
    vector<vector<int> > outer;
    outer.push_back(new Arrayvector<>());
    for (int i = 0; i < arr.size(); i++)
    {
        int n = outer.size();
        for (int i = 0; i < n; i++)
        {
            vector<int> internal = new Arrayvector<>(outer.get(i));
            internal.push_back(arr[i]);
            outer.push_back(internal);
        }
    }
    return outer;
}

vector<vector<int> > subsetDuplicate(int arr[])
{
    int n = sizeof(arr) / sizeof(arr[0]);
    sort(arr, n);
    vector<vector<int> > outer;
    outer.push_back(new Arrayvector<>());
    int start = 0;
    int end = 0;
    for (int i = 0; i < n; i++)
    {
        start = 0;
        // if current and previous element is same, s = e + 1
        if (i > 0 && arr[i] == arr[i - 1])
        {
            start = end + 1;
        }
        end = outer.size() - 1;
        int n = outer.size();
        for (int j = start; j < n; j++)
        {
            vector<int> internal = new Arrayvector<>(outer.get(j));
            internal.push_back(arr[i]);
            outer.push_back(internal);
        }
    }
    return outer;
}
void display(vector<vector<string> > ans)
{
    for (int i = 0; i < ans.size(); i++)
    {
        for (int j = 0; j < ans[i].size(); j++)
        {
            cout << ans[i][j] << " ";
        }
    }
    cout << endl;
}
int main()
{
    int arr[3] = {1, 2, 2};
    vector<vector<int> > ans = subsetDuplicate(arr);
    display(ans);
}

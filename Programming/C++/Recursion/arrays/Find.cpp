#include <iostream>
#include <vector>
using namespace std;

bool find(int arr[], int n, int target, int index)
{

    if (index == n)
    {
        return false;
    }
    return arr[index] == target || find(arr, n, target, index + 1);
}

int findIndex(int arr[], int n, int target, int index)
{
    if (index == n)
    {
        return -1;
    }
    if (arr[index] == target)
    {
        return index;
    }
    else
    {
        return findIndex(arr, n, target, index + 1);
    }
}

int findIndexLast(int arr[], int n, int target, int index)
{
    if (index == -1)
    {
        return -1;
    }
    if (arr[index] == target)
    {
        return index;
    }
    else
    {
        return findIndexLast(arr, n, target, index - 1);
    }
}
vector<int> lista;
void findAllIndex0(int arr[], int n, int target, int index)
{
    if (index == n)
    {
        return;
    }
    if (arr[index] == target)
    {
        lista.push_back(index);
    }
    findAllIndex0(arr, n, target, index + 1);
}

vector<int> findAllIndex1(int arr[], int n, int target, int index, vector<int> list)
{
    if (index == n)
    {
        return list;
    }
    if (arr[index] == target)
    {
        list.push_back(index);
    }
    return findAllIndex1(arr, n, target, index + 1, list);
}

vector<int> findAllIndex2(int arr[], int n, int target, int index)
{

    vector<int> list;

    if (index == n)
    {
        return list;
    }

    // this will contain answer for that function call only
    if (arr[index] == target)
    {
        list.push_back(index);
    }
    vector<int> ansFromBelowCalls = findAllIndex2(arr, n, target, index + 1);
    vector<int> v(list.size() + ansFromBelowCalls.size());
    vector<int>::iterator it, st;

    it = set_union(list.begin(),
                   list.end(),
                   ansFromBelowCalls.begin(),
                   ansFromBelowCalls.end(),
                   v.begin());
    // list.insert(list.end(), ansFromBelowCalls.begin(), ansFromBelowCalls.end());

    return v;
}

int main()
{
    int arr[6] = {2, 3, 1, 4, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    //        System.out.println(find(arr, 4, 0));
    //        System.out.println(findIndex(arr, 4, 0));
    //        System.out.println(findIndexLast(arr, 4, arr.length-1));
    //        findAllIndex(arr, 4, 0);
    //        System.out.println(list);

    //        ArrayList<Integer> list = new ArrayList<>();
    //        ArrayList<Integer> ans = findAllIndex(arr, 4, 0, list);
    //        System.out.println(ans);
    //        System.out.println(list);
    vector<int> list, list1;
    cout << find(arr, n, 4, 0) << endl;
    cout << findIndex(arr, n, 4, 0) << endl;
    cout << findIndexLast(arr, n, 4, 0) << endl;
    cout << "Kya bol reli hai public" << endl;
    findAllIndex0(arr, n, 4, 0);
    for (int i = 0; i < lista.size(); i++)
        cout << lista[i] << " ";
    cout << endl;
    cout << "Bht Hard Bht Hard" << endl;
    list = findAllIndex1(arr, n, 4, 0, list);
    for (int i = 0; i < list.size(); i++)
        cout << list[i] << " ";
    cout << endl;
    list1 = findAllIndex2(arr, n, 4, 0);
    for (int i = 0; i < list1.size(); i++)
        cout << list1[i] << " ";
    cout << endl;
    return 0;
}

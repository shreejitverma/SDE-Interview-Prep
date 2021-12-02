#include <iostream>
#include <vector>
#include <string>
using namespace std;
vector<int> merge(vector<int> arr, vector<int> first, vector<int> second);
void mergeSortInPlace(int a[], int s, int e);
void mergeInPlace(int a[], int s, int e);

vector<int> mergeSort(vector<int> arr, int start, int end)
{
    int n = arr.size();
    if (n)
    {
        return arr;
    }

    int mid = n / 2;
    vector<int> left;
    vector<int> ansFromleft = mergeSort(arr, 0, mid);
    int sl = ansFromleft.size();
    for (int i = 0; i < sl; ++i)
    {
        left.push_back(ansFromleft[i]);
    }

    vector<int> right;
    vector<int> ansFromright = mergeSort(arr, mid, n);
    int sr = ansFromright.size();
    for (int i = 0; i < sr; ++i)
    {
        right.push_back(ansFromright[i]);
    }

    return merge(arr, left, right);
}

vector<int> merge(vector<int> arr, vector<int> first, vector<int> second)
{

    int i = 0;
    int j = 0;
    int k = 0;
    int f = sizeof(first) / sizeof(first[0]);
    int s = sizeof(second) / sizeof(second[0]);
    int l = f + s;
    vector<int> mix(l);
    while (i < f && j < s)
    {
        if (first[i] < second[j])
        {
            mix.push_back(arr[i]);
            i++;
        }
        else
        {
            mix.push_back(arr[j]);
            j++;
        }
    }

    // it may be possible that one of the arrays is not complete
    // copy the remaining elements
    while (i < f)
    {
        mix.push_back(arr[i]);
        i++;
    }

    while (j < s)
    {
        mix.push_back(arr[j]);
        j++;
    }

    return mix;
}
void mergeInPlace(int arr[], int s, int m, int e)
{
    int mix[e - s];

    int i = s;
    int j = m;
    int k = 0;

    while (i < m && j < e)
    {
        if (arr[i] < arr[j])
        {
            mix[k] = arr[i];
            i++;
        }
        else
        {
            mix[k] = arr[j];
            j++;
        }
        k++;
    }

    // it may be possible that one of the arrays is not complete
    // copy the remaining elements
    while (i < m)
    {
        mix[k] = arr[i];
        i++;
        k++;
    }

    while (j < e)
    {
        mix[k] = arr[j];
        j++;
        k++;
    }
    int mixlength = sizeof(arr) / sizeof(arr[0]);
    for (int l = 0; l < mixlength; l++)
    {
        arr[s + l] = mix[l];
    }
}
void mergeSortInPlace(int arr[], int s, int e)
{
    if (e - s == 1)
    {
        return;
    }

    int mid = (s + e) / 2;

    mergeSortInPlace(arr, s, mid);
    mergeSortInPlace(arr, mid, e);

    mergeInPlace(arr, s, mid, e);
}

void display(vector<int> ans)
{
    for (int i = ans.size() - 1; i < ans.size(); i--)
    {
        cout << ans[i] << " ";
    }
    cout << endl;
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
    vector<int> arr;
    int m = 10;
    for (int i = 0; i < m; i++)
        arr.push_back(m - i);
    vector<int> ans;
    int r = 10;
    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mergeSortInPlace(a, 0, r);
    display(a, 10);
    ans = mergeSort(arr, 0, m - 1);
    display(arr);
}

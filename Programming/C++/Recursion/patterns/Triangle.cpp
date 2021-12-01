
#include <iostream>
#include <vector>
#include <string>
using namespace std;

void triangle2(int r, int c)
{
    if (r == 0)
    {
        return;
    }
    if (c < r)
    {
        triangle2(r, c + 1);
        cout << "*";
    }
    else
    {
        triangle2(r - 1, 0);
        cout << endl;
    }
}

void triangle(int r, int c)
{
    if (r == 0)
    {
        return;
    }
    if (c < r)
    {
        cout << "*";
        triangle(r, c + 1);
    }
    else
    {
        cout << endl;
        triangle(r - 1, 0);
    }
}

void bubble(int arr[], int r, int c)
{
    if (r == 0)
    {
        return;
    }
    if (c < r)
    {

        if (arr[c] > arr[c + 1])
        {
            // swap
            swap(arr[c], arr[c + 1]);
        }

        bubble(arr, r, c + 1);
    }
    else
    {
        bubble(arr, r - 1, 0);
    }
}

void selection(int arr[], int r, int c, int max)
{
    if (r == 0)
    {
        return;
    }
    if (c < r)
    {
        if (arr[c] > arr[max])
        {
            selection(arr, r, c + 1, c);
        }
        else
        {
            selection(arr, r, c + 1, max);
        }
    }
    else
    {
        swap(arr[max], arr[r - 1]);
        selection(arr, r - 1, 0, 0);
    }
}
void display(int arr[], int n)
{
    cout << endl;
    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
}
int main()
{
    triangle2(4, 0);
    int arr[] = {1, 4, 3, 5, 6, 7, 8, 9, 10, 11};
    int n = sizeof(arr) / sizeof(arr[0]);
    selection(arr, n, 0, 0);
    display(arr, n);
}
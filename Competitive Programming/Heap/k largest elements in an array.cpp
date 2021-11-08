/*
https://practice.geeksforgeeks.org/problems/k-largest-elements4206/1
k largest elements 
Medium Accuracy: 53.0% Submissions: 27220 Points: 4
Given an array Arr of N positive integers, find K largest elements from the array.  The output elements should be printed in decreasing order.

Example 1:

Input:
N = 5, K = 2
Arr[] = {12, 5, 787, 1, 23}
Output: 787 23
Explanation: 1st largest element in the
array is 787 and second largest is 23.
Example 2:

Input:
N = 7, K = 3
Arr[] = {1, 23, 12, 9, 30, 2, 50}
Output: 50 30 23
Explanation: 3 Largest element in the
array are 50, 30 and 23.
Your Task:
You don't need to read input or print anything. Your task is to complete the function kLargest() which takes the array of integers arr, n and k as parameters and returns an array of integers denoting the answer. The array should be in decreasing order.

Expected Time Complexity: O(N + KlogK)
Expected Auxiliary Space: O(K+(N-K)*logK)

Constraints:
1 ≤ K ≤ N ≤ 105
1 ≤ Arr[i] ≤ 106
*/

class Solution
{
public:
    vector<int> kLargest(int arr[], int n, int k)
    {
        // code here

        // Sort the given array arr in reverse
        // order.
        sort(arr, arr + n, greater<int>());
        vector<int> v;

        // Print the first kth largest elements
        for (int i = 0; i < k; i++)
            v.push_back(arr[i]);

        return v;
    }
};

/*
C++ Sol. → Time complexity = O(n+k*logn)

Steps. 1 build a max heap

Steps. 2 Extract max from max heap, replace it with INT_MIN and apply heapify algo.

Step 3. Apply Step2 to k times.

 */

vector<int> kLargest(int arr[], int n, int k)
{
    // code here
    buildHeap(arr, n);
    vector<int> ans(k);
    for (int i = 0; i < k; i++)
    {
        ans[i] = arr[0];
        arr[0] = INT_MIN;
        heapify(arr, n, 0);
    }
    //cout<<arr[i]<<" ";
    //ans[i]=arr[i];
    //cout<<"\n";
    //sort(ans.begin(), ans.end(), greater<>());
    return ans;
}

void heapify(int arr[], int n, int i)
{
    // Your Code Here
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int largest = i;
    if (left < n && arr[left] > arr[largest])
        largest = left;
    if (right < n && arr[right] > arr[largest])
        largest = right;
    if (largest != i)
    {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}
// Rearranges input array so that it becomes a max heap
void buildHeap(int arr[], int n)
{
    // Your Code Here
    for (int i = (n / 2) - 1; i >= 0; i--)
    {
        heapify(arr, n, i);
    }
}

// Using quicksort and pivot

int partition(int arr[], int l, int r)
{
    int pivot = l;
    int low = l + 1, high = r;
    while (low <= high)
    {
        while (low <= r && arr[low] <= arr[pivot])
            low++;
        while (high >= l && arr[high] > arr[pivot])
            high--;
        if (low < high)
            swap(arr[low], arr[high]);
    }
    swap(arr[high], arr[pivot]);
    return high;
}
void quickselect(int arr[], int l, int r, int k)
{
    while (l <= r)
    {
        int mid = partition(arr, l, r);
        if (mid == k - 1)
            return;
        if (mid < k)
            quickselect(arr, mid + 1, r, k);
        return quickselect(arr, l, mid - 1, k);
    }
}
vector<int> kLargest(int arr[], int n, int k)
{
    quickselect(arr, 0, n - 1, n - k);
    vector<int> result(k);
    for (int i = 0; i < k; i++)
    {
        result[i] = arr[i + n - k];
    }
    sort(result.rbegin(), result.rend());
    return result;
}

#include <bits/stdc++.h>
using namespace std;

//picks up last element between start and end
int findPivot(int a[], int start, int end)
{

    // Selecting the pivot element
    int pivot = a[end];

    // Initially partition-index will be
    // at starting
    int pIndex = start;

    for (int i = start; i < end; i++)
    {

        // If an element is lesser than pivot, swap it.
        if (a[i] <= pivot)
        {
            swap(a[i], a[pIndex]);

            // Incrementing pIndex for further
            // swapping.
            pIndex++;
        }
    }

    // Lastly swapping or the
    // correct position of pivot
    swap(a[pIndex], a[end]);
    return pIndex;
}

//THIS PART OF CODE IS CONTRIBUTED BY - rjrachit
//Picks up random pivot element between start and end
int findRandomPivot(int arr[], int start, int end)
{
    int n = end - start + 1;
    // Selecting the random pivot index
    int pivotInd = random() % n;
    swap(arr[end], arr[start + pivotInd]);
    int pivot = arr[end];
    //initialising pivoting point to start index
    pivotInd = start;
    for (int i = start; i < end; i++)
    {

        // If an element is lesser than pivot, swap it.
        if (arr[i] <= pivot)
        {
            swap(arr[i], arr[pivotInd]);

            // Incrementing pivotIndex for further
            // swapping.
            pivotInd++;
        }
    }

    // Lastly swapping or the
    // correct position of pivot
    swap(arr[pivotInd], arr[end]);
    return pivotInd;
}
//THIS PART OF CODE IS CONTRIBUTED BY - rjrachit

void SmallestLargest(int a[], int low, int high, int k,
                     int n)
{
    if (low == high)
        return;
    else
    {
        int pivotIndex = findRandomPivot(a, low, high);

        if (k == pivotIndex)
        {
            cout << k << " smallest elements are : ";
            for (int i = 0; i < pivotIndex; i++)
                cout << a[i] << "  ";

            cout << endl;

            cout << k << " largest elements are : ";
            for (int i = (n - pivotIndex); i < n; i++)
                cout << a[i] << "  ";
        }

        else if (k < pivotIndex)
            SmallestLargest(a, low, pivotIndex - 1, k, n);

        else if (k > pivotIndex)
            SmallestLargest(a, pivotIndex + 1, high, k, n);
    }
}

// Driver Code
int main()
{
    int a[] = {11, 3, 2, 1, 15, 5, 4, 45, 88, 96, 50, 45};
    int n = sizeof(a) / sizeof(a[0]);

    int low = 0;
    int high = n - 1;

    // Lets assume k is 3
    int k = 4;

    // Function Call
    SmallestLargest(a, low, high, k, n);

    return 0;
}
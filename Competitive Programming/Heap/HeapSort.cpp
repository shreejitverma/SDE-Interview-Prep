/*
https://www.geeksforgeeks.org/heap-sort/
Heap sort is a comparison-based sorting technique based on Binary Heap data structure. 
It is similar to selection sort where we first find the minimum element 
and place the minimum element at the beginning. 
We repeat the same process for the remaining elements.

What is Binary Heap? 
Let us first define a Complete Binary Tree. 
A complete binary tree is a binary tree in which every level, 
except possibly the last, is completely filled, and all nodes are as far left as possible (Source Wikipedia)
A Binary Heap is a Complete Binary Tree where items are stored 
in a special order such that the value in a parent node 
is greater(or smaller) than the values in its two children nodes. 
The former is called max heap and the latter is called min-heap. 
The heap can be represented by a binary tree or array.

Why array based representation for Binary Heap? 
Since a Binary Heap is a Complete Binary Tree, it can be easily represented as an array 
and the array-based representation is space-efficient. 
If the parent node is stored at index I, 
the left child can be calculated by 2 * I + 1 
and the right child by 2 * I + 2 (assuming the indexing starts at 0).



Heap Sort Algorithm for sorting in increasing order: 
1. Build a max heap from the input data. 
2. At this point, the largest item is stored at the root of the heap. 
Replace it with the last item of the heap followed by reducing the size of heap by 1. 
Finally, heapify the root of the tree. 
3. Repeat step 2 while the size of the heap is greater than 1.

How to build the heap? 
Heapify procedure can be applied to a node only if its children nodes are heapified. 
So the heapification must be performed in the bottom-up order.
Lets understand with the help of an example:

Input data: 4, 10, 3, 5, 1
         4(0)
        /   \
     10(1)   3(2)
    /   \
 5(3)    1(4)

The numbers in bracket represent the indices in the array 
representation of data.

Applying heapify procedure to index 1:
         4(0)
        /   \
    10(1)    3(2)
    /   \
5(3)    1(4)

Applying heapify procedure to index 0:
        10(0)
        /  \
     5(1)  3(2)
    /   \
 4(3)    1(4)
The heapify procedure calls itself recursively to build heap
 in top down manner.
*/

// C++ program for implementation of Heap Sort
#include <iostream>

using namespace std;

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
void heapify(int arr[], int n, int i)
{
    int largest = i;   // Initialize largest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // If left child is larger than root
    if (l < n && arr[l] > arr[largest])
        largest = l;

    // If right child is larger than largest so far
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // If largest is not root
    if (largest != i)
    {
        swap(arr[i], arr[largest]);

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

// main function to do heap sort
void heapSort(int arr[], int n)
{
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--)
    {
        // Move current root to end
        swap(arr[0], arr[i]);

        // call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

/* A utility function to print array of size n */
void printArray(int arr[], int n)
{
    for (int i = 0; i < n; ++i)
        cout << arr[i] << " ";
    cout << "\n";
}

// Driver code
int main()
{
    int arr[] = {12, 11, 13, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);

    heapSort(arr, n);

    cout << "Sorted array is \n";
    printArray(arr, n);
}
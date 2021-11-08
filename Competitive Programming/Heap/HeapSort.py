'''https://www.geeksforgeeks.org/heap-sort/
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
 in top down manner.'''

# Python program for implementation of heap Sort

# To heapify subtree rooted at index i.
# n is size of heap


def heapify(arr, n, i):
    largest = i  # Initialize largest as root
    l = 2 * i + 1     # left = 2*i + 1
    r = 2 * i + 2     # right = 2*i + 2

    # See if left child of root exists and is
    # greater than root
    if l < n and arr[largest] < arr[l]:
        largest = l

    # See if right child of root exists and is
    # greater than root
    if r < n and arr[largest] < arr[r]:
        largest = r

    # Change root, if needed
    if largest != i:
        arr[i], arr[largest] = arr[largest], arr[i]  # swap

        # Heapify the root.
        heapify(arr, n, largest)

# The main function to sort an array of given size


def heapSort(arr):
    n = len(arr)

    # Build a maxheap.
    for i in range(n//2 - 1, -1, -1):
        heapify(arr, n, i)

    # One by one extract elements
    for i in range(n-1, 0, -1):
        arr[i], arr[0] = arr[0], arr[i]  # swap
        heapify(arr, i, 0)


# Driver code
arr = [12, 11, 13, 5, 6, 7]
heapSort(arr)
n = len(arr)
print("Sorted array is")
for i in range(n):
    print("%d" % arr[i]),

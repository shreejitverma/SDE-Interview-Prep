'''https://www.geeksforgeeks.org/building-heap-from-array/
Given an array of N elements. The task is to build a Binary Heap from the given array. 
The heap can be either Max Heap or Min Heap.

Example:
Input: arr[] = {4, 10, 3, 5, 1}
Output: Corresponding Max-Heap:
       10
     /   \
    5     3
   /  \
  4    1

Input: arr[] = {1, 3, 5, 4, 6, 13, 10, 9, 8, 15, 17}
Output: Corresponding Max-Heap:
                 17
              /      \
            15         13
          /    \      /  \
         9      6    5   10
        / \    /  \
       4   8  3    1


Suppose the given input elements are: 4, 10, 3, 5, 1.



The corresponding complete binary tree for this array of elements [4, 10, 3, 5, 1] will be:

       4
     /   \
    10    3
   /  \
  5    1

Note:
Root is at index 0 in array.
Left child of i-th node is at (2*i + 1)th index.
Right child of i-th node is at (2*i + 2)th index.
Parent of i-th node is at (i-1)/2 index.
Simple Approach: Suppose, we need to build a Max-Heap from the above-given array elements. 
It can be clearly seen that the above complete binary tree formed does not follow the Heap property. 
So, the idea is to heapify the complete binary tree formed from the array 
in reverse level order following a top-down approach.

That is first heapify, the last node in level order traversal of the tree, then heapify the second last node and so on.

Time Complexity: Heapify a single node takes O(log N) time complexity where N is the total number of Nodes. 
Therefore, building the entire Heap will take N heapify operations and the total time complexity will be O(N*logN).

In reality, building a heap takes O(n) time depending on the implementation which can be seen here.

Optimized Approach: The above approach can be optimized by observing the fact that the leaf nodes need 
not to be heapified as they already follow the heap property. 
Also, the array representation of the complete binary tree contains the level order traversal of the tree.

So the idea is to find the position of the last non-leaf node 
and perform the heapify operation of each non-leaf node in reverse level order.

Last non-leaf node = parent of last-node.
or, Last non-leaf node = parent of node at (n-1)th index.
or, Last non-leaf node = Node at index ((n-1) - 1)/2.
                       = (n/2) - 1.
Illustration:

Array = {1, 3, 5, 4, 6, 13, 10, 9, 8, 15, 17}

Corresponding Complete Binary Tree is:
                 1
              /     \
            3         5
         /    \     /  \
        4      6   13  10
       / \    / \
      9   8  15 17

The task to build a Max-Heap from above array.

Total Nodes = 11.
Last Non-leaf node index = (11/2) - 1 = 4.
Therefore, last non-leaf node = 6.

To build the heap, heapify only the nodes:
[1, 3, 5, 4, 6] in reverse order.

Heapify 6: Swap 6 and 17.
                 1
              /     \
            3         5
         /    \      /  \
        4      17   13  10
       / \    /  \
      9   8  15   6

Heapify 4: Swap 4 and 9.
                 1
              /     \
            3         5
         /    \      /  \
        9      17   13  10
       / \    /  \
      4   8  15   6

Heapify 5: Swap 13 and 5.
                 1
              /     \
            3         13
         /    \      /  \
        9      17   5   10
       / \    /  \
      4   8  15   6

Heapify 3: First Swap 3 and 17, again swap 3 and 15.
                 1
              /     \
            17         13
          /    \      /  \
         9      15   5   10
        / \    /  \
       4   8  3   6

Heapify 1: First Swap 1 and 17, again swap 1 and 15, 
           finally swap 1 and 6.
                 17
              /      \
            15         13
          /    \      /  \
         9      6    5   10
        / \    /  \
       4   8  3    1'''


# Python3 program for building Heap from Array

# To heapify a subtree rooted with node i
# which is an index in arr[]. N is size of heap
def heapify(arr, n, i):

    largest = i  # Initialize largest as root
    l = 2 * i + 1  # left = 2*i + 1
    r = 2 * i + 2  # right = 2*i + 2

    # If left child is larger than root
    if l < n and arr[l] > arr[largest]:
        largest = l

    # If right child is larger than largest so far
    if r < n and arr[r] > arr[largest]:
        largest = r

    # If largest is not root
    if largest != i:
        arr[i], arr[largest] = arr[largest], arr[i]

        # Recursively heapify the affected sub-tree
        heapify(arr, n, largest)

# Function to build a Max-Heap from the given array


def buildHeap(arr, n):

    # Index of last non-leaf node
    startIdx = n // 2 - 1

    # Perform reverse level order traversal
    # from last non-leaf node and heapify
    # each node
    for i in range(startIdx, -1, -1):
        heapify(arr, n, i)

# A utility function to print the array
# representation of Heap


def printHeap(arr, n):
    print("Array representation of Heap is:")

    for i in range(n):
        print(arr[i], end=" ")
    print()


# Driver Code
if __name__ == '__main__':

    # Binary Tree Representation
    # of input array
    # 1
    #         /     \
    # 3         5
    #     / \     / \
    # 4     6 13 10
    # / \ / \
    # 9 8 15 17
    arr = [1, 3, 5, 4, 6, 13,
           10, 9, 8, 15, 17]

    n = len(arr)

    buildHeap(arr, n)

    printHeap(arr, n)

    # Final Heap:
    # 17
    #         /     \
    # 15         13
    #     / \     / \
    # 9     6 5 10
    #     / \ / \
    # 4 8 3 1

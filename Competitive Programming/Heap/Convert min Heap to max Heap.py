'''https://www.geeksforgeeks.org/convert-min-heap-to-max-heap/
Given array representation of min Heap, convert it to max Heap in O(n) time. 
Example : 
 

Input: arr[] = [3 5 9 6 8 20 10 12 18 9]
         3
      /     \
     5       9
   /   \    /  \
  6     8  20   10
 /  \   /
12   18 9 


Output: arr[] = [20 18 10 12 9 9 3 5 6 8] OR 
       [any Max Heap formed from input elements]

         20
       /    \
     18      10
   /    \    /  \
  12     9  9    3
 /  \   /
5    6 8 

The problem might look complex at first look. But our final goal is to only build the max heap. The idea is very simple – we simply build Max Heap without caring about the input. We start from the bottom-most and rightmost internal mode of min Heap and heapify all internal modes in the bottom-up way to build the Max heap.
Below is its implementation
 '''

# A Python3 program to convert min Heap
# to max Heap

# to heapify a subtree with root
# at given index


def MaxHeapify(arr, i, n):
    l = 2 * i + 1
    r = 2 * i + 2
    largest = i
    if l < n and arr[l] > arr[i]:
        largest = l
    if r < n and arr[r] > arr[largest]:
        largest = r
    if largest != i:
        arr[i], arr[largest] = arr[largest], arr[i]
        MaxHeapify(arr, largest, n)

# This function basically builds max heap


def convertMaxHeap(arr, n):

    # Start from bottommost and rightmost
    # internal mode and heapify all
    # internal modes in bottom up way
    for i in range(int((n - 2) / 2), -1, -1):
        MaxHeapify(arr, i, n)

# A utility function to print a
# given array of given size


def printArray(arr, size):
    for i in range(size):
        print(arr[i], end=" ")
    print()


# Driver Code
if __name__ == '__main__':

    # array representing Min Heap
    arr = [3, 5, 9, 6, 8, 20, 10, 12, 18, 9]
    n = len(arr)

    print("Min Heap array : ")
    printArray(arr, n)

    convertMaxHeap(arr, n)

    print("Max Heap array : ")
    printArray(arr, n)

'''https://www.geeksforgeeks.org/sliding-window-maximum-maximum-of-all-subarrays-of-size-k/
Given an array and an integer K, find the maximum for each and every contiguous subarray of size k.

Input: arr[] = {1, 2, 3, 1, 4, 5, 2, 3, 6}, K = 3 
Output: 3 3 4 5 5 5 6
Explanation: 
Maximum of 1, 2, 3 is 3
Maximum of 2, 3, 1 is 3
Maximum of 3, 1, 4 is 4
Maximum of 1, 4, 5 is 5
Maximum of 4, 5, 2 is 5 
Maximum of 5, 2, 3 is 5
Maximum of 2, 3, 6 is 6

Input: arr[] = {8, 5, 10, 7, 9, 4, 15, 12, 90, 13}, K = 4 
Output: 10 10 10 15 15 90 90
Explanation:
Maximum of first 4 elements is 10, similarly for next 4 
elements (i.e from index 1 to 4) is 10, So the sequence 
generated is 10 10 10 15 15 90 90


Method 1: This is a simple method to solve the above problem.



Approach: 
The idea is very basic run a nested loop, 
the outer loop which will mark the starting point of the subarray of length k, 
the inner loop will run from the starting index to index+k, 
k elements from starting index and print the maximum element among these k elements. 

Algorithm: 

Create a nested loop, the outer loop from starting index to n – k th elements. 
The inner loop will run for k iterations.
Create a variable to store the maximum of k elements traversed by the inner loop.
Find the maximum of k elements traversed by the inner loop.
Print the maximum element in every iteration of outer loop
Implementation: 
'''

# Python program to find the maximum for
# each and every contiguous subarray of
# size k

# Method to find the maximum for each
# and every contiguous subarray of s
# of size k


from collections import deque


def printMax(arr, n, k):
    max = 0

    for i in range(n - k + 1):
        max = arr[i]
        for j in range(1, k):
            if arr[i + j] > max:
                max = arr[i + j]
        print(str(max) + " ", end="")


# Driver method
if __name__ == "__main__":
    arr = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    n = len(arr)
    k = 3
    printMax(arr, n, k)


'''
Output
3 4 5 6 7 8 9 10 
Complexity Analysis: 

Time Complexity: O(N * K). 
The outer loop runs n-k+1 times and the inner loop runs k times for every iteration of outer loop. 
So time complexity is O((n-k+1)*k) which can also be written as O(N * K).
Space Complexity: O(1). 
No extra space is required.
Method 2: This method uses the Self-Balancing BST to solve the given problem.

Approach: 
To find maximum among k elements of the subarray the previous method uses a loop traversing through the elements. 
To reduce that time the idea is to use an AVL tree which returns the maximum element in log n time. 
So, traverse through the array and keep k elements in the BST and print the maximum in every iteration. 
AVL tree is a suitable data structure as lookup, insertion, 
and deletion all take O(log n) time in both the average and worst cases, 
where n is the number of nodes in the tree prior to the operation. 

Algorithm: 



Create a Self-balancing BST (AVL tree) to store and find the maximum element.
Traverse through the array from start to end.
Insert the element in the AVL tree.
If the loop counter is greater than or equal to k then delete i-k th element from the BST
Print the maximum element of the BST.
Implementation: 
'''


'''
Output
10 10 10 15 15 90 90 
Complexity Analysis:  

Time Complexity: O(N * Log k). 
Insertion, deletion and search takes log k time in a AVL tree. So the overall time Complexity is O(N * log k)
Space Complexity: O(k). 
The space required to store k elements in a BST is O(k).
Method 3: This method uses Deque to solve the above problem

Approach: 
Create a Deque, Qi of capacity k, 
that stores only useful elements of current window of k elements. 
An element is useful if it is in current window and 
is greater than all other elements on right side of it in current window. 
Process all array elements one by one and maintain Qi 
to contain useful elements of current window and these useful elements are maintained in sorted order. 
The element at front of the Qi is the largest and element at rear/back of Qi is the smallest of current window. 
Thanks to Aashish for suggesting this method.

Dry run of the above approach: 



Algorithm:  

Create a deque to store k elements.
Run a loop and insert first k elements in the deque. Before inserting the element, 
check if the element at the back of the queue is smaller than the current element, 
if it is so remove the element from the back of the deque, 
until all elements left in the deque are greater than the current element. 
Then insert the current element, at the back of the deque.
Now, run a loop from k to end of the array.
Print the front element of the deque.
Remove the element from the front of the queue if they are out of the current window.
Insert the next element in the deque. Before inserting the element, 
check if the element at the back of the queue is smaller than the current element, 
if it is so remove the element from the back of the deque, 
until all elements left in the deque are greater than the current element. 
Then insert the current element, at the back of the deque.
Print the maximum element of the last window.
Implementation: 
'''

# Python program to find the maximum for
# each and every contiguous subarray of
# size k


# A Deque (Double ended queue) based
# method for printing maximum element
# of all subarrays of size k

def printMax(arr, n, k):
    """ Create a Double Ended Queue, Qi that
    will store indexes of array elements.
    The queue will store indexes of useful
    elements in every window and it will
    maintain decreasing order of values from
    front to rear in Qi, i.e., arr[Qi.front[]]
    to arr[Qi.rear()] are sorted in decreasing
    order"""
    Qi = deque()

    # Process first k (or first window)
    # elements of array
    for i in range(k):

        # For every element, the previous
        # smaller elements are useless
        # so remove them from Qi
        while Qi and arr[i] >= arr[Qi[-1]]:
            Qi.pop()

        # Add new element at rear of queue
        Qi.append(i)

    # Process rest of the elements, i.e.
    # from arr[k] to arr[n-1]
    for i in range(k, n):

        # The element at the front of the
        # queue is the largest element of
        # previous window, so print it
        print(str(arr[Qi[0]]) + " ", end="")

        # Remove the elements which are
        # out of this window
        while Qi and Qi[0] <= i-k:

            # remove from front of deque
            Qi.popleft()

        # Remove all elements smaller than
        # the currently being added element
        # (Remove useless elements)
        while Qi and arr[i] >= arr[Qi[-1]]:
            Qi.pop()

        # Add current element at the rear of Qi
        Qi.append(i)

    # Print the maximum element of last window
    print(str(arr[Qi[0]]))


# Driver code
if __name__ == "__main__":
    arr = [12, 1, 78, 90, 57, 89, 56]
    k = 3
    printMax(arr, len(arr), k)

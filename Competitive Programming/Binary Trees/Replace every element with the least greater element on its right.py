'''https://www.geeksforgeeks.org/replace-every-element-with-the-least-greater-element-on-its-right/
Given an array of integers, replace every element with the least greater element on its right side in the array. If there are no greater elements on the right side, replace it with -1.

Examples: 
Input: [8, 58, 71, 18, 31, 32, 63, 92, 
         43, 3, 91, 93, 25, 80, 28]
Output: [18, 63, 80, 25, 32, 43, 80, 93, 
         80, 25, 93, -1, 28, -1, -1]
Recommended: Please try your approach on {IDE} first, before moving on to the solution.
A naive method is to run two loops. The outer loop will one by one pick array elements from left to right. The inner loop will find the smallest element greater than the picked element on its right side. Finally, the outer loop will replace the picked element with the element found by inner loop. The time complexity of this method will be O(n2).



A tricky solution would be to use Binary Search Trees. We start scanning the array from right to left and insert each element into the BST. For each inserted element, we replace it in the array by its inorder successor in BST. If the element inserted is the maximum so far (i.e. its inorder successor doesn’t exist), we replace it by -1.

Below is the implementation of the above idea – '''

# Python3 program to replace every element
# with the least greater element on its right

# A binary Tree node


class Node:

    def __init__(self, d):

        self.data = d
        self.left = None
        self.right = None

# A utility function to insert a new node with
# given data in BST and find its successor


def insert(node, data):

    global succ

    # If the tree is empty, return a new node
    root = node

    if (node == None):
        return Node(data)

    # If key is smaller than root's key, go to left
    # subtree and set successor as current node
    if (data < node.data):

        # print("1")
        succ = node
        root.left = insert(node.left, data)

    # Go to right subtree
    elif (data > node.data):
        root.right = insert(node.right, data)

    return root

# Function to replace every element with the
# least greater element on its right


def replace(arr, n):

    global succ
    root = None

    # Start from right to left
    for i in range(n - 1, -1, -1):
        succ = None

        # Insert current element into BST and
        # find its inorder successor
        root = insert(root, arr[i])

        # Replace element by its inorder
        # successor in BST
        if (succ):
            arr[i] = succ.data

        # No inorder successor
        else:
            arr[i] = -1

    return arr


# Driver code
if __name__ == '__main__':

    arr = [8, 58, 71, 18, 31, 32, 63,
           92, 43, 3, 91, 93, 25, 80, 28]
    n = len(arr)
    succ = None

    arr = replace(arr, n)

    print(*arr)

'''Output
18 63 80 25 32 43 80 93 80 25 93 -1 28 -1 -1 
The worst-case time complexity of the above solution is also O(n2) as it uses BST. The worst-case will happen when array is sorted in ascending or descending order. The complexity can easily be reduced to O(nlogn) by using balanced trees like red-black trees.

Another Approach:

We can use the Next Greater Element using stack algorithm to solve this problem in O(Nlog(N)) time and O(N) space.

Algorithm:

First, we take an array of pairs namely temp, and store each element and its index in this array,i.e. temp[i] will be storing {arr[i],i}.
Sort the array according to the array elements.
Now get the next greater index for each and every index of the temp array in an array namely index by using Next Greater Element using stack.
Now index[i] stores the index of the next least greater element of the element temp[i].first and if index[i] is -1, then it means that there is no least greater element of the element temp[i].second at its right side.
Now take a result array where result[i] will be equal to a[indexes[temp[i].second]] if index[i] is not -1 otherwise result[i] will be equal to -1.
Below is the implementation of the above approach'''

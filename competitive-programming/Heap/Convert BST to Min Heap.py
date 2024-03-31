'''https://www.geeksforgeeks.org/convert-bst-min-heap/
Given a binary search tree which is also a complete binary tree. The problem is to convert the given BST into a Min Heap with the condition that all the values in the left subtree of a node should be less than all the values in the right subtree of the node. This condition is applied on all the nodes in the so converted Min Heap. 
Examples: 
 

Input :          4
               /   \
              2     6
            /  \   /  \
           1   3  5    7  

Output :        1
              /   \
             2     5
           /  \   /  \
          3   4  6    7 

The given BST has been transformed into a
Min Heap.
All the nodes in the Min Heap satisfies the given
condition, that is, values in the left subtree of
a node should be less than the values in the right
subtree of the node. 

Create an array arr[] of size n, where n is the number of nodes in the given BST.
Perform the inorder traversal of the BST and copy the node values in the arr[] in sorted order.
Now perform the preorder traversal of the tree.
While traversing the root during the preorder traversal, one by one copy the values from the array arr[] to the nodes.
'''

# C++ implementation to convert the
# given BST to Min Heap

# structure of a node of BST


class Node:

    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None

# function for the inorder traversal
# of the tree so as to store the node
# values in 'arr' in sorted order


def inorderTraversal(root, arr):
    if root == None:
        return

    # first recur on left subtree
    inorderTraversal(root.left, arr)

    # then copy the data of the node
    arr.append(root.data)

    # now recur for right subtree
    inorderTraversal(root.right, arr)

# function to convert the given
# BST to MIN HEAP performs preorder
# traversal of the tree


def BSTToMinHeap(root, arr, i):
    if root == None:
        return

    # first copy data at index 'i' of
    # 'arr' to the node
    i[0] += 1
    root.data = arr[i[0]]

    # then recur on left subtree
    BSTToMinHeap(root.left, arr, i)

    # now recur on right subtree
    BSTToMinHeap(root.right, arr, i)

# utility function to convert the
# given BST to MIN HEAP


def convertToMinHeapUtil(root):

    # vector to store the data of
    # all the nodes of the BST
    arr = []
    i = [-1]

    # inorder traversal to populate 'arr'
    inorderTraversal(root, arr)

    # BST to MIN HEAP conversion
    BSTToMinHeap(root, arr, i)

# function for the preorder traversal
# of the tree


def preorderTraversal(root):
    if root == None:
        return

    # first print the root's data
    print(root.data, end=" ")

    # then recur on left subtree
    preorderTraversal(root.left)

    # now recur on right subtree
    preorderTraversal(root.right)


# Driver Code
if __name__ == '__main__':

    # BST formation
    root = Node(4)
    root.left = Node(2)
    root.right = Node(6)
    root.left.left = Node(1)
    root.left.right = Node(3)
    root.right.left = Node(5)
    root.right.right = Node(7)

    convertToMinHeapUtil(root)
    print("Preorder Traversal:")
    preorderTraversal(root)

'''Output:  

Preorder Traversal:
1 2 3 4 5 6 7
Time Complexity: O(n) 
Auxiliary Space: O(n)   '''

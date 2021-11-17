'''https://practice.geeksforgeeks.org/problems/binary-tree-to-bst/1
Binary Tree to BST 
Easy Accuracy: 50.0% Submissions: 29268 Points: 2
Given a Binary Tree, convert it to Binary Search Tree in such a way that keeps the original structure of Binary Tree intact.
 

Example 1:

Input:
      1
    /   \
   2     3
Output: 1 2 3

Example 2:

Input:
          1
       /    \
     2       3
   /        
 4       
Output: 1 2 3 4
Explanation:
The converted BST will be

        3
      /   \
    2     4
  /
 1
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function binaryTreeToBST() which takes the root of the Binary tree as input and returns the root of the BST. The driver code will print inorder traversal of the converted BST.


Expected Time Complexity: O(NLogN).
Expected Auxiliary Space: O(N).


Constraints:
1 <= Number of nodes <= 1000'''

# User function Template for python3

'''
# Tree Node
class Node:
    def __init__(self, val):
        self.right = None
        self.data = val
        self.left = None
'''




from collections import deque
class Solution:
    # The given root is the root of the Binary Tree
    # Return the root of the generated BST
    def InOrder(self, root, inorder):
        # code here
        if(root != None):
            self.InOrder(root.left, inorder)
            inorder.append(root.data)
            self.InOrder(root.right, inorder)

    # Helper function that copies contents of sorted array
    # to Binary tree
    def arrayToBST(self, arr, root):

        # Base Case
        if root is None:
            return

        # First update the left subtree
        self.arrayToBST(arr, root.left)

        # now update root's data delete the value from array
        root.data = arr[0]
        arr.pop(0)

        # Finally update the right subtree
        self.arrayToBST(arr, root.right)

    # This function converts a given binary tree to BST
    def binaryTreeToBST(self, root):

        # Base Case: Tree is empty
        if root is None:
            return

        # Create the temp array and store the inorder traversal
        # of tree
        arr = []
        self.InOrder(root, arr)

        # Sort the array
        arr.sort()

        # copy array elements back to binary tree
        self.arrayToBST(arr, root)

        return root

# {
#  Driver Code Starts
# Initial Template for Python 3


# Tree Node


class Node:
    def __init__(self, val):
        self.right = None
        self.data = val
        self.left = None

# Function to Build Tree


def buildTree(s):
    # Corner Case
    if(len(s) == 0 or s[0] == "N"):
        return None

    # Creating list of strings from input
    # string after spliting by space
    ip = list(map(str, s.split()))

    # Create the root of the tree
    root = Node(int(ip[0]))
    size = 0
    q = deque()

    # Push the root to the queue
    q.append(root)
    size = size+1

    # Starting from the second element
    i = 1
    while(size > 0 and i < len(ip)):
        # Get and remove the front of the queue
        currNode = q[0]
        q.popleft()
        size = size-1

        # Get the current node's value from the string
        currVal = ip[i]

        # If the left child is not null
        if(currVal != "N"):

            # Create the left child for the current node
            currNode.left = Node(int(currVal))

            # Push it to the queue
            q.append(currNode.left)
            size = size+1
        # For the right child
        i = i+1
        if(i >= len(ip)):
            break
        currVal = ip[i]

        # If the right child is not null
        if(currVal != "N"):

            # Create the right child for the current node
            currNode.right = Node(int(currVal))

            # Push it to the queue
            q.append(currNode.right)
            size = size+1
        i = i+1
    return root


def printInorder(root):
    if root is None:
        return
    printInorder(root.left)
    print(root.data, end=' ')
    printInorder(root.right)


if __name__ == "__main__":
    t = int(input())
    for _ in range(0, t):
        s = input()
        root = buildTree(s)
        Solution().binaryTreeToBST(root)
        printInorder(root)
        print()
# } Driver Code Ends

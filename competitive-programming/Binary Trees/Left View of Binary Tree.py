'''https: // practice.geeksforgeeks.org/problems/left-view-of-binary-tree/1
https://www.geeksforgeeks.org/print-left-view-binary-tree/
https://www.geeksforgeeks.org/iterative-method-to-print-left-view-of-a-binary-tree/
Given a Binary Tree, print Left view of it. Left view of a Binary Tree is set of nodes visible when tree is visited from Left side. The task is to complete the function leftView(), which accepts root of the tree as argument.

Left view of following tree is 1 2 4 8.

          1
       /     \
     2        3
   /     \    /    \
  4     5   6    7
   \
     8   

Example 1:

Input:
   1
 /  \
3    2
Output: 1 3

Example 2:

Input:

Output: 10 20 40
Your Task:
You just have to complete the function leftView() that prints the left view. The newline is automatically appended by the driver code.
Expected Time Complexity: O(N).
Expected Auxiliary Space: O(Height of the Tree).

Constraints:
0 <= Number of nodes <= 100
1 <= Data of a node <= 1000'''
# Recursion
# Python program to print left view of Binary Tree

# A binary tree node


class Node:

    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None


# Recursive function print left view of a binary tree
def leftViewUtil(root, level, max_level):

    # Base Case
    if root is None:
        return

    # If this is the first node of its level
    if (max_level[0] < level):
        print "% d\t" % (root.data),
        max_level[0] = level

    # Recur for left and right subtree
    leftViewUtil(root.left, level + 1, max_level)
    leftViewUtil(root.right, level + 1, max_level)


# A wrapper over leftViewUtil()
def leftView(root):
    max_level = [0]
    leftViewUtil(root, 1, max_level)


# Driver program to test above function
root = Node(12)
root.left = Node(10)
root.right = Node(20)
root.right.left = Node(25)
root.right.right = Node(40)

leftView(root)

# Iteration
# Python3 program to print left view of
# Binary Tree

# Binary Tree Node
""" utility that allocates a newNode
with the given key """


class newNode:

    # Construct to create a newNode
    def __init__(self, key):
        self.data = key
        self.left = None
        self.right = None
        self.hd = 0

# function to print left view of
# binary tree


def printRightView(root):

    if (not root):
        return

    q = []
    q.append(root)

    while (len(q)):

        # number of nodes at current level
        n = len(q)

        # Traverse all nodes of current level
        for i in range(1, n + 1):
            temp = q[0]
            q.pop(0)

            # Print the left most element
            # at the level
            if (i == 1):
                print(temp.data, end=" ")

            # Add left node to queue
            if (temp.left != None):
                q.append(temp.left)

            # Add right node to queue
            if (temp.right != None):
                q.append(temp.right)


# Driver Code
if __name__ == '__main__':

    root = newNode(10)
    root.left = newNode(2)
    root.right = newNode(3)
    root.left.left = newNode(7)
    root.left.right = newNode(8)
    root.right.right = newNode(15)
    root.right.left = newNode(12)
    root.right.right.left = newNode(14)
    printRightView(root)

# Python3 program to print the
# left view of Binary Tree

# Binary Tree Node
""" utility that allocates a newNode
with the given key """


class newNode:

    # Construct to create a newNode
    def __init__(self, key):
        self.data = key
        self.left = None
        self.right = None
        self.hd = 0

# Utility function to print the left
# view of the binary tree


def leftViewUtil(root, q):

    if (root == None):
        return

    # append root
    q.append(root)

    # Delimiter
    q.append(None)

    while (len(q)):
        temp = q[0]

        if (temp):

            # Prints first node of each level
            print(temp.data, end=" ")

            # append children of all nodes
            # at current level
            while (q[0] != None):
                temp = q[0]

                # If left child is present
                # append into queue
                if (temp.left):
                    q.append(temp.left)

                # If right child is present
                # append into queue
                if (temp.right):
                    q.append(temp.right)

                # Pop the current node
                q.pop(0)

            # append delimiter
            # for the next level
            q.append(None)

        # Pop the delimiter of
        # the previous level
        q.pop(0)

# Function to print the leftView
# of Binary Tree


def leftView(root):

    # Queue to store all
    # the nodes of the tree
    q = []

    leftViewUtil(root, q)


# Driver Code
if __name__ == '__main__':

    root = newNode(10)
    root.left = newNode(12)
    root.right = newNode(3)
    root.left.right = newNode(4)
    root.right.left = newNode(5)
    root.right.left.right = newNode(6)
    root.right.left.right.left = newNode(18)
    root.right.left.right.right = newNode(7)
    leftView(root)

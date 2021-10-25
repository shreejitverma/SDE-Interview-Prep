# https://www.techiedelight.com/preorder-tree-traversal-iterative-recursive/
# Data structure to store a binary tree node
class Node:
    def __init__(self, data=None, left=None, right=None):
        self.data = data
        self.left = left
        self.right = right


# Recursive function to perform preorder traversal on the tree
def preorder(root):

    # return if the current node is empty
    if root is None:
        return

    # Display the data part of the root (or current node)
    print(root.data, end=' ')

    # Traverse the left subtree
    preorder(root.left)

    # Traverse the right subtree
    preorder(root.right)


if __name__ == '__main__':

    ''' Construct the following tree
               1
             /   \
            /     \
           2       3
          /      /   \
         /      /     \
        4      5       6
              / \
             /   \
            7     8
    '''

    root = Node(1)
    root.left = Node(2)
    root.right = Node(3)
    root.left.left = Node(4)
    root.right.left = Node(5)
    root.right.right = Node(6)
    root.right.left.left = Node(7)
    root.right.left.right = Node(8)

    preorder(root)

    from collections import deque


# Data structure to store a binary tree node
class Node:
    def __init__(self, data=None, left=None, right=None):
        self.data = data
        self.left = left
        self.right = right


# Iterative function to perform preorder traversal on the tree
def preorderIterative(root):

    # return if the tree is empty
    if root is None:
        return

    # create an empty stack and push the root node
    stack = deque()
    stack.append(root)

    # loop till stack is empty
    while stack:

        # pop a node from the stack and print it
        curr = stack.pop()

        print(curr.data, end=' ')

        # push the right child of the popped node into the stack
        if curr.right:
            stack.append(curr.right)

        # push the left child of the popped node into the stack
        if curr.left:
            stack.append(curr.left)

    # the right child must be pushed first so that the left child
    # is processed first (LIFO order)


if __name__ == '__main__':

    ''' Construct the following tree
               1
             /   \
            /     \
           2       3
          /      /   \
         /      /     \
        4      5       6
              / \
             /   \
            7     8
    '''

    root = Node(1)
    root.left = Node(2)
    root.right = Node(3)
    root.left.left = Node(4)
    root.right.left = Node(5)
    root.right.right = Node(6)
    root.right.left.left = Node(7)
    root.right.left.right = Node(8)

    preorderIterative(root)

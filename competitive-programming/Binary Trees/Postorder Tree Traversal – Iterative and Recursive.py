# https://www.techiedelight.com/postorder-tree-traversal-iterative-recursive/
# Data structure to store a binary tree node
class Node:
    def __init__(self, data=None, left=None, right=None):
        self.data = data
        self.left = left
        self.right = right


# Recursive function to perform postorder traversal on the tree
def postorder(root):

    # return if the current node is empty
    if root is None:
        return

    # Traverse the left subtree
    postorder(root.left)

    # Traverse the right subtree
    postorder(root.right)

    # Display the data part of the root (or current node)
    print(root.data, end=' ')


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

    postorder(root)

    from collections import deque


# Data structure to store a binary tree node
class Node:
    def __init__(self, data=None, left=None, right=None):
        self.data = data
        self.left = left
        self.right = right


# Iterative function to perform postorder traversal on the tree
def postorderIterative(root):

    # return if the tree is empty
    if root is None:
        return

    # create an empty stack and push the root node
    stack = deque()
    stack.append(root)

    # create another stack to store postorder traversal
    out = deque()

    # loop till stack is empty
    while stack:

        # pop a node from the stack and push the data into the output stack
        curr = stack.pop()
        out.append(curr.data)

        # push the left and right child of the popped node into the stack
        if curr.left:
            stack.append(curr.left)

        if curr.right:
            stack.append(curr.right)

    # print postorder traversal
    while out:
        print(out.pop(), end=' ')


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

    postorderIterative(root)

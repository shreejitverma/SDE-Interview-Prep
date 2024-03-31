'''https://www.geeksforgeeks.org/create-a-mirror-tree-from-the-given-binary-tree/
Create a mirror tree from the given binary tree
Difficulty Level : Easy
Last Updated : 12 Oct, 2021
Geek Week
Given a binary tree, the task is to create a new binary tree which is a mirror image of the given binary tree.

Examples: 

Input:
        5
       / \
      3   6
     / \
    2   4
Output:
Inorder of original tree: 2 3 4 5 6 
Inorder of mirror tree: 6 5 4 3 2
Mirror tree will be:
  5
 / \
6   3
   / \
  4   2

Input:
        2
       / \
      1   8
     /     \
    12      9
Output:
Inorder of original tree: 12 1 2 8 9 
Inorder of mirror tree: 9 8 2 1 12'''

# code


class Node:
    def __init__(self, data):
        self.left = None
        self.right = None
        self.data = data


def inOrder(root):
    if root is not None:
        inOrder(root.left)
        print(root.data, end=' ')
        inOrder(root.right)

# we recursively call the mirror function which swaps the right subtree with the left subtree.


def mirror(root):
    if root is None:
        return
    mirror(root.left)
    mirror(root.right)
    root.left, root.right = root.right, root.left


root = Node(1)
root.left = Node(2)
root.right = Node(3)
root.left.left = Node(4)
root.right.left = Node(5)

print("The inorder traversal of the tree before mirroring:", end=' ')
print(inOrder(root))
# 4 2 1 5 3
print()
mirror(root)
print("The inorder traversal of the tree after mirroring:", end=' ')
print(inOrder(root))

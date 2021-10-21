# Python program to do inorder traversal without recursion
 
# A binary tree node
class Node:
     
    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None
 
# Iterative function for inorder tree traversal
def inOrder(root):
     
    # Set current to root of binary tree
    current = root
    stack = [] # initialize stack
    done = 0
     
    while True:
         
        # Reach the left most Node of the current Node
        if current is not None:
             
            # Place pointer to a tree node on the stack
            # before traversing the node's left subtree
            stack.append(current)
         
            current = current.left
 
         
        # BackTrack from the empty subtree and visit the Node
        # at the top of the stack; however, if the stack is
        # empty you are done
        elif(stack):
            current = stack.pop()
            print(current.data, end=" ") # Python 3 printing
         
            # We have visited the node and its left
            # subtree. Now, it's right subtree's turn
            current = current.right
 
        else:
            break
      
    print()
 
# Driver program to test above function
 
""" Constructed binary tree is
            1
          /   \
         2     3
       /  \
      4    5   """
 
root = Node(1)
root.left = Node(2)
root.right = Node(3)
root.left.left = Node(4)
root.left.right = Node(5)
 
inOrder(root)
 
# This code is contributed by Nikhil Kumar Singh(nickzuck_007)
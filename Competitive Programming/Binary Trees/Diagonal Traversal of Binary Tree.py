'''https://www.geeksforgeeks.org/diagonal-traversal-of-binary-tree/
Consider lines of slope -1 passing between nodes. Given a Binary Tree, print all diagonal elements in a binary tree belonging to the same line.

Input : Root of below tree

Output : 
Diagonal Traversal of binary tree : 
 8 10 14
 3 6 7 13
 1 4
Observation : root and root->right values will be prioritized over all root->left values.
The idea is to use a map. We use different slope distances and use them as key in the map. Value in the map is a vector (or dynamic array) of nodes. We traverse the tree to store values in the map. Once map is built, we print the contents of it. 

Below is the implementation of the above idea.'''

# Python program for diagonal
# traversal of Binary Tree

# A binary tree node
from collections import deque


class Node:

    # Constructor to create a
    # new binary tree node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None


""" root - root of the binary tree
   d -  distance of current line from rightmost
        -topmost slope.
   diagonalPrint - multimap to store Diagonal
                   elements (Passed by Reference) """


def diagonalPrintUtil(root, d, diagonalPrintMap):

    # Base Case
    if root is None:
        return

    # Store all nodes of same line
    # together as a vector
    try:
        diagonalPrintMap[d].append(root.data)
    except KeyError:
        diagonalPrintMap[d] = [root.data]

    # Increase the vertical distance
    # if left child
    diagonalPrintUtil(root.left,
                      d+1, diagonalPrintMap)

    # Vertical distance remains
    # same for right child
    diagonalPrintUtil(root.right,
                      d, diagonalPrintMap)


# Print diagonal traversal of given binary tree
def diagonalPrint(root):

    # Create a dict to store diagonal elements
    diagonalPrintMap = dict()

    # Find the diagonal traversal
    diagonalPrintUtil(root, 0, diagonalPrintMap)

    print "Diagonal Traversal of binary tree : "
    for i in diagonalPrintMap:
        for j in diagonalPrintMap[i]:
            print j,
        print ""


# Driver Program
root = Node(8)
root.left = Node(3)
root.right = Node(10)
root.left.left = Node(1)
root.left.right = Node(6)
root.right.right = Node(14)
root.right.right.left = Node(13)
root.left.right.left = Node(4)
root.left.right.right = Node(7)

diagonalPrint(root)


'''Output
Diagonal Traversal of binary tree : 
8 10 14 
3 6 7 13 
1 4 
The time complexity of this solution is O( N logN ) and the space complexity is O( N )

We can solve the same problem using queue and iterative algorithm. '''


# A binary tree node


class Node:

    # Constructor to create a
    # new binary tree node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None


def diagonal(root):
    out = []
    node = root

    # queue to store left nodes
    left_q = deque()
    while node:

        # append data to output array
        out.append(node.data)

        # if left available add it to the queue
        if node.left:
            left_q.appendleft(node.left)

        # if right is available change the node
        if node.right:
            node = node.right
        else:

            # else pop the left_q
            if len(left_q) >= 1:
                node = left_q.pop()
            else:
                node = None
    return out


# Driver Code
root = Node(8)
root.left = Node(3)
root.right = Node(10)
root.left.left = Node(1)
root.left.right = Node(6)
root.right.right = Node(14)
root.right.right.left = Node(13)
root.left.right.left = Node(4)
root.left.right.right = Node(7)

print(diagonal(root))

'''Output
[8, 10, 14, 3, 6, 7, 13, 1, 4]
The time complexity of this solution is O( N logN ) and the space complexity is O( N )

Approach 2 : Using Queue.

Every node will help to generate the next diagonal. We will push the element in the queue only when its left is available. We will process the node and move to its right.'''

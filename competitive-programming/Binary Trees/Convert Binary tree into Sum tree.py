'''https://practice.geeksforgeeks.org/problems/transform-to-sum-tree/1
Transform to Sum Tree 
Easy Accuracy: 50.22% Submissions: 41768 Points: 2
Given a Binary Tree of size N , where each node can have positive or negative values. Convert this to a tree where each node contains the sum of the left and right sub trees of the original tree. The values of leaf nodes are changed to 0.

Example 1:

Input:
             10
          /      \
        -2        6
       /   \     /  \
     8     -4   7    5

Output:
            20
          /    \
        4        12
       /  \     /  \
     0     0   0    0

Explanation:

           (4-2+12+6)
          /           \
      (8-4)          (7+5)
       /   \         /  \
      0     0       0    0
 

Your Task:  
You dont need to read input or print anything. Complete the function toSumTree() which takes root node as input parameter and modifies the given tree in-place.

Note: If you click on Compile and Test the output will be the in-order traversal of the modified tree.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(height of tree)
 

Constraints:
1 ≤ N ≤ 104'''

# User function Template for python3

'''
# Node Class:
class Node:
    def __init__(self,val):
        self.data = val
        self.left = None
        self.right = None
'''




from collections import deque
import sys
class Solution:
    def fun(self, root):
        if(root == None):
            return 0
        else:
            s = root.data+self.fun(root.left)+self.fun(root.right)
            root.data = s-root.data
            return s

    def toSumTree(self, root):
        # code here
        self.fun(root)

# {
#  Driver Code Starts
# Initial Template for Python 3


# Contributed by Sudarshan Sharma
sys.setrecursionlimit(10**6)
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

# A utility function to print
# inorder traversal of a Binary Tree


def printInorder(Node):
    if (Node == None):
        return
    printInorder(Node.left)
    print(Node.data, end=" ")
    printInorder(Node.right)


if __name__ == "__main__":
    t = int(input())
    for _ in range(0, t):
        s = input()
        root = buildTree(s)
        ob = Solution()
        ob.toSumTree(root)
        printInorder(root)
        print()
# } Driver Code Ends

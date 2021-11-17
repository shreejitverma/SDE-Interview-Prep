'''https://practice.geeksforgeeks.org/problems/boundary-traversal-of-binary-tree/1
Boundary Traversal of binary tree 
Medium Accuracy: 26.78% Submissions: 100k+ Points: 4
Given a Binary Tree, find its Boundary Traversal. The traversal should be in the following order: 

Left boundary nodes: defined as the path from the root to the left-most node ie- the leaf node you could reach when you always travel preferring the left subtree over the right subtree. 
Leaf nodes: All the leaf nodes except for the ones that are part of left or right boundary.
Reverse right boundary nodes: defined as the path from the right-most node to the root. The right-most node is the leaf node you could reach when you always travel preferring the right subtree over the left subtree. Exclude the root from this as it was already included in the traversal of left boundary nodes.
Note: If the root doesn't have a left subtree or right subtree, then the root itself is the left or right boundary. 

Example 1:

Input:
        1 
      /   \
     2     3  
    / \   / \ 
   4   5 6   7
      / \
     8   9
   
Output: 1 2 4 8 9 6 7 3
Explanation:


 

Example 2:

Input:
            1
           / 
          2
        /  \
       4    9
     /  \    \
    6    5    3
             /  \
            7     8

Output: 1 2 4 6 5 7 8
Explanation: 
As you can see we have not taken right
subtree. See Note

 

Your Task:
This is a function problem. You don't have to take input. Just complete the function boundary() that takes the root node as input and returns an array containing the boundary values in anti-clockwise.

 

Expected Time Complexity: O(N). 
Expected Auxiliary Space: O(Height of the Tree).

 

Constraints:
1 ≤ Number of nodes ≤ 105
1 ≤ Data of a node ≤ 105'''

# User function Template for python3

'''
class Node:
    def __init__(self, val):
        self.right = None
        self.data = val
        self.left = None
'''




from collections import deque
import sys
def AddLeftBoundary(root, ans):
    x = root.left

    while x:
        if(x.left != None or x.right != None):
            ans.append(x.data)
        if(x.left):
            x = x.left
        else:
            x = x.right


def AddLeaves(root, ans):
    if(root.left == None and root.right == None):
        ans.append(root.data)
        return

    if(root.left):
        AddLeaves(root.left, ans)
    if(root.right):
        AddLeaves(root.right, ans)


def AddRightBoundary(root, ans):
    x = root.right
    temp = []

    while x:
        if(x.left != None or x.right != None):
            temp.append(x.data)
        if(x.right):
            x = x.right
        else:
            x = x.left
    for i in range(len(temp) - 1, -1, -1):
        ans.append(temp[i])


class Solution:
    def printBoundaryView(self, root):
        # Code here
        ans = []
        if(root == None):
            return ans
        if(root.left == None and root.right == None):
            return root

        if(root.left != None or root.right != None):
            ans.append(root.data)

        AddLeftBoundary(root, ans)
        AddLeaves(root, ans)
        AddRightBoundary(root, ans)

        return ans

# {
#  Driver Code Starts
# Initial Template for Python 3


# function should return a list containing the boundary view of the binary tree
# {
#  Driver Code Starts

sys.setrecursionlimit(100000)
# Contributed by Sudarshan Sharma
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


if __name__ == "__main__":
    t = int(input())
    for _ in range(0, t):
        s = input()
        root = buildTree(s)
        obj = Solution()
        res = obj.printBoundaryView(root)
        for i in res:
            print(i, end=" ")
        print('')
# } Driver Code Ends

'''https://practice.geeksforgeeks.org/problems/find-k-th-smallest-element-in-bst/1
k-th smallest element in BST 
Medium Accuracy: 49.44% Submissions: 49545 Points: 4
Given a BST and an integer K. Find the Kth Smallest element in the BST. 

Example 1:

Input:
      2
    /   \
   1     3
K = 2
Output: 2
Example 2:

Input:
        2
      /  \
     1    3
K = 5
Output: -1
 

Your Task:
You don't need to read input or print anything. Your task is to complete the function KthSmallestElement() which takes the root of the BST and integer K as inputs and return the Kth smallest element in the BST, if no such element exists return -1.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).

Constraints:
1<=Number of nodes<=100000'''

# {
# Driver Code Starts
# Initial Template for Python 3

from collections import deque
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

 # } Driver Code Ends
# User function Template for python3


'''
class Node:
    def __init__(self, val):
        self.right = None
        self.data = val
        self.left = None
'''


class Solution:
    # Return the Kth smallest element in the given BST
    def __init__(self):
        self.lis = []

    def KthSmallestElement(self, root, K):
        self.inordertrav(root)
        if K > len(self.lis):
            return -1
        return self.lis[K-1]

    def inordertrav(self, root):
        if root.left is not None:
            self.inordertrav(root.left)
        self.lis.append(root.data)
        if root.right is not None:
            self.inordertrav(root.right)


# {
# Driver Code Starts.
if __name__ == "__main__":
    t = int(input())
    for _ in range(0, t):
        s = input()
        root = buildTree(s)
        k1 = int(input())
        print(Solution().KthSmallestElement(root, k1))

# } Driver Code Ends

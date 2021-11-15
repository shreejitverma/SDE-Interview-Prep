'''https://practice.geeksforgeeks.org/problems/is-binary-tree-heap/1
Is Binary Tree Heap 
Easy Accuracy: 50.0% Submissions: 31246 Points: 2
Given a binary tree. The task is to check whether the given tree follows the max heap property or not.
 

Example 1:

Input:
      5
    /  \
   2    3
Output: 1
Explanation: The given tree follows max-heap property since 5,
is root and it is greater than both its children.

Example 2:

Input:
       10
     /   \
    20   30 
  /   \
 40   60
Output: 0

Your Task:
You don't need to read input or print anything. Your task is to complete the function isHeap() which takes the root of Binary Tree as parameter returns True if the given binary tree is a heap else returns False.

Expected Time Complexity: O(N)
Expected Space Complexity: O(N)

Constraints:
1 ≤ Number of nodes ≤ 100
1 ≤ Data of a node ≤ 1000'''

# User Template for python3

'''
class Node:
    def __init__(self,val):
        self.data = val
        self.left = None
        self.right = None
'''




from collections import deque
import sys
class Solution:
    # Your Function Should return True/False
    def size(self, root):
        if not root:
            return 0
        return 1+self.size(root.left)+self.size(root.right)

    def minHeap(self, root, i, n):
        if not root:
            return True
        if i >= n:
            return False
        if (root.left and root.left.data >= root.data) or (root.right and root.right.data >= root.data):
            return False
        return self.minHeap(root.left, 2*i+1, n) and self.minHeap(root.right, 2*i+2, n)
    # Your Function Should return True/False

    def isHeap(self, root):
        # Code Here
        i = 0
        return self.minHeap(root, i, self.size(root))

# {
#  Driver Code Starts
# Initial Template for Python 3


# Contributed by Susanta Mukherjee
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


if __name__ == "__main__":
    t = int(input())
    for _ in range(0, t):
        root = buildTree(input())
        ob = Solution()
        if ob.isHeap(root):
            print(1)
        else:
            print(0)


# } Driver Code Ends

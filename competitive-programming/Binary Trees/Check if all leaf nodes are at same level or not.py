'''https://practice.geeksforgeeks.org/problems/leaf-at-same-level/1
Leaf at same level 
Easy Accuracy: 49.76% Submissions: 35248 Points: 2
Given a Binary Tree, check if all leaves are at same level or not.

Example 1:

Input: 
            1
          /   \
         2     3

Output: 1

Explanation: 
Leaves 2 and 3 are at same level.

Example 2:

Input:
            10
          /    \
        20      30
       /  \        
     10    15

Output: 0

Explanation:
Leaves 10, 15 and 30 are not at same level.

Your Task:  
You dont need to read input or print anything. Complete the function check() which takes root node as input parameter and returns true/false depending on whether all the leaf nodes are at the same level or not.
 

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(height of tree)
 

Constraints:
1 ≤ N ≤ 10^3'''

# User function Template for python3

from collections import deque


class Solution:
    # Your task is to complete this function
    # function should return True/False or 1/0
    first_leaf_node_height = 0

    def check(self, root):
        # Code here
        return Solution.traverse(self, root, 0)

    def traverse(self, root, current_height):
        # print("first",first_leaf_node_height)
        # print("current",current_height)
        # print("data",root.data)
        if root == None:
            return True

        if root.left == None and root.right == None:
            if self.first_leaf_node_height == 0:
                self.first_leaf_node_height = current_height
                # print("first",self.first_leaf_node_height)

            elif self.first_leaf_node_height != current_height:
                return False
            return True
        return Solution.traverse(self, root.left, current_height+1) and Solution.traverse(self, root.right, current_height+1)


# {
#  Driver Code Starts
# Initial Template for Python 3
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
        if Solution().check(root):
            print(1)
        else:
            print(0)


# } Driver Code Ends

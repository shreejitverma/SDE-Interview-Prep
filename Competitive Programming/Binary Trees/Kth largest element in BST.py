'''https://practice.geeksforgeeks.org/problems/kth-largest-element-in-bst/1
Kth largest element in BST 
Easy Accuracy: 42.26% Submissions: 51439 Points: 2
Given a Binary search tree. Your task is to complete the function which will return the Kth largest element without doing any modification in Binary Search Tree.


Example 1:

Input:
      4
    /   \
   2     9
k = 2 
Output: 4

Example 2:

Input:
       9
        \ 
          10
K = 1
Output: 10

Your Task:
You don't need to read input or print anything. Your task is to complete the function kthLargest() which takes the root of the BST and an integer K as inputs and returns the Kth largest element in the given BST.


Expected Time Complexity: O(H + K).
Expected Auxiliary Space: O(H)


Constraints:
1 <= N <= 1000
1 <= K <= N'''

# User function Template for python3

# class Node:
#     def __init__(self, val):
#         self.data = val
#         self.left = None
#         self.right = None

# return the Kth largest element in the given BST rooted at 'root'

from collections import deque


class Solution:
    def Helper(self, root, k, ans):
        if root == None:
            return -1
        self.Helper(root.right, k, ans)
        k[0] = k[0]-1
        if k[0] == 0:
            ans[0] = root.data
        self.Helper(root.left, k, ans)

    def kthLargest(self, root, k):
        ans = [-1]
        k = [k]
        self.Helper(root, k, ans)
        return ans[0]
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
        k = int(input())
        print(Solution().kthLargest(root, k))
# } Driver Code Ends

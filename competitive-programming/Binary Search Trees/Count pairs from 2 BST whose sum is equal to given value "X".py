'''https://practice.geeksforgeeks.org/problems/brothers-from-different-root/1
Brothers From Different Roots 
Easy Accuracy: 50.91% Submissions: 15113 Points: 2
Given two BSTs containing N1 and N2 distinct nodes respectively and given a value x. Your task is to complete the function countPairs(), that returns the count of all pairs from both the BSTs whose sum is equal to x.


Example 1:

Input:
BST1:
       5
     /   \
    3     7
   / \   / \
  2   4 6   8

BST2:
       10
     /    \
    6      15
   / \    /  \
  3   8  11   18

x = 16
Output:
3
Explanation:
The pairs are: (5, 11), (6, 10) and (8, 8)
 

Example 2:

Input:
BST1:
  1
   \
    3
   /
  2
BST2:
    3
   / \
  2   4
 /     
1

x = 4
Output:
3
Explanation:
The pairs are: (2, 2), (3, 1) and (1, 3)

Your Task:
You don't need to read input or print anything. Your task is to complete the function countPairs(), which takes 2 BST's as parameter in form of root1 and root2 and the integer x, that returns the count of all pairs from both the BSTs whose sum is equal to x.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N)


Constraints:
1 ≤ Number of nodes ≤ 105
1 ≤ Data of a node ≤ 106'''

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
# Tree Node
class Node:
    def __init__(self, val):
        self.right = None
        self.data = val
        self.left = None
'''


class Solution:
    def countPairs(self, root1, root2, x):
        # code here.
        d = {}

        def inorder(root, d):
            if root is None:
                return None
            inorder(root.left, d)
            if root.data not in d:
                d[root.data] = 1
            inorder(root.right, d)
        inorder(root1, d)
        c = [0]

        def pairs(root, c):
            if root is None:
                return
            pairs(root.left, c)
            s = x-root.data
            if s in d:
                c[0] += 1
            pairs(root.right, c)
        pairs(root2, c)
        return c[0]


# {
# Driver Code Starts.
if __name__ == "__main__":
    t = int(input())
    for _ in range(0, t):
        s1 = input()
        s2 = input()
        root1 = buildTree(s1)
        root2 = buildTree(s2)
        x = int(input())
        ob = Solution()
        print(ob.countPairs(root1, root2, x))
# } Driver Code Ends
